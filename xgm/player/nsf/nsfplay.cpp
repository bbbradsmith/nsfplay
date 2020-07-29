#include <assert.h>
#include <typeinfo>
#include <cstring>
#include "nsfplay.h"

#include <time.h> // for srand() initialization

namespace xgm
{
  int debug_mark = 0;

  NSFPlayer::NSFPlayer () : PlayerMSP ()
  {
    nsf = NULL;

    sc[APU] = (apu = new NES_APU());
    sc[DMC] = (dmc = new NES_DMC());
    sc[FDS] = (fds = new NES_FDS());
    sc[FME7] = (fme7 = new NES_FME7());
    sc[MMC5] = (mmc5 = new NES_MMC5());
    sc[N106] = (n106 = new NES_N106());
    sc[VRC6] = (vrc6 = new NES_VRC6());
    sc[VRC7] = (vrc7 = new NES_VRC7());
    ld = new NESDetector();
    logcpu = new CPULogger();

    nsf2_irq.SetCPU(&cpu); // IRQ
    dmc->SetAPU(apu); // set APU
    dmc->SetCPU(&cpu); // IRQ requires CPU access
    mmc5->SetCPU(&cpu); // MMC5 PCM read action requires CPU read access

    /* アンプ←フィルタ←レートコンバータ←音源 を接続 */
    for (int i = 0; i < NES_DEVICE_MAX; i++)
    {
      amp[i].Attach (sc[i]);
    }

    rconv.Attach(&mixer);
    fader.Attach(&rconv);

    nch = 1;
    infinite = false;
  }

  NSFPlayer::~NSFPlayer ()
  {
    delete apu;
    delete dmc;
    delete fds;
    delete fme7;
    delete mmc5;
    delete n106;
    delete vrc6;
    delete vrc7;
    delete ld;
    delete logcpu;
  }

  void NSFPlayer::SetConfig(PlayerConfig *pc)
  {
    config = dynamic_cast<NSFPlayerConfig *>(pc);
    PlayerMSP::SetConfig(pc);
  }

  bool NSFPlayer::IsDetected ()
  {
    return playtime_detected;
  }

  const char *NSFPlayer::GetTitleString ()
  {
    if (nsf == NULL) return "(not loaded)";

    static char buf[512];
    sprintf (buf, "%s", nsf->GetTitleString((*config)["TITLE_FORMAT"]));
    return buf;
  }

  bool NSFPlayer::Load (SoundData * sdat)
  {
    nsf = dynamic_cast < NSF * >(sdat);

    if((*config)["NSFE_PLAYLIST"] && nsf->nsfe_plst)
    {
      nsf->start = 1;
      nsf->songs = nsf->nsfe_plst_size;
    }
    else
    {
      nsf->nsfe_plst = NULL;
    }

    nsf->DebugOut ();
    Reload ();
    return true;
  }

  void NSFPlayer::Reload ()
  {
    int i, bmax = 0;

    assert (nsf);

    for (i = 0; i < 8; i++)
      if (bmax < nsf->bankswitch[i])
        bmax = nsf->bankswitch[i];

    mem.SetImage (nsf->body, nsf->load_address, nsf->bodysize);

    if (bmax)
    {
      bank.SetImage (nsf->body, nsf->load_address, nsf->bodysize);
      for (i = 0; i < 8; i++)
        bank.SetBankDefault (i + 8, nsf->bankswitch[i]);
    }

    // virtual machine controlling memory reads and writes
    // to various devices, expansions, etc.
    stack.DetachAll ();
    layer.DetachAll ();
    mixer.DetachAll ();
    apu_bus.DetachAll ();

    // select the loop detector
    if((*config)["DETECT_ALT"])
    {
      const std::type_info &ti = typeid(ld);
      if(strcmp(ti.name(),"NESDetectorEx")!=0)
      {
        delete ld;
        ld = new NESDetectorEx();
      }
    }
    else
    {
      const std::type_info &ti = typeid(ld);
      if(strcmp(ti.name(),"NESDetector")!=0)
      {
        delete ld;
        ld = new NESDetector();
      }
    }

    // loop detector ends up at the front of the stack
    // (will capture all writes, but does not capture write)
    stack.Attach (ld);

    int log_level = (*config)["LOG_CPU"];
    logcpu->SetOption(0, log_level);
    logcpu->SetSoundchip(nsf->soundchip);
    logcpu->SetNSF(nsf);
    if (log_level > 0)
    {
        logcpu->SetFilename((*config)["LOG_CPU_FILE"]);
        stack.Attach(logcpu);
        cpu.SetLogger(logcpu);
        logcpu->SetCPU(&cpu);
    }
    else
    {
        cpu.SetLogger(NULL);
    }

    // setup player program at PLAYER_RESERVED ($4100)
    const UINT8 PLAYER_PROGRAM[] =
    {
        0x20, 0x1E, 0x41, // $4100 JSR PLAY ($411E is a placeholder RTS)
        0x4C, 0x03, 0x41, // $4103 JMP to self (do nothing loop for detecting end of frame)
        0x48, 0x8A, 0x48, 0x98, 0x48, // $4106 PHA TXA PHA TYA PHA
        0xA9, 0x00, 0x8D, 0x00, 0x20, // $410B LDA #$00 STA $2000 (disable NMI re-entry)
        0x20, 0x1E, 0x41, // $4110 JSR PLAY for non-returning INIT ($411E placeholder)
        0xA9, 0x80, 0x8D, 0x00, 0x20, // $4113 LDA #$80 STA $2000 (re-enable NMI)
        0x68, 0xA8, 0x68, 0xAA, 0x68, // $4118 PLA TAY PLA TAX PLA
        0x40, // $411D RTI
        0x60, // $411E RTS
    };
    const int PLAYER_PROGRAM_SIZE = sizeof(PLAYER_PROGRAM);
    mem.SetReserved(PLAYER_PROGRAM, PLAYER_PROGRAM_SIZE);
    // Note: PLAYER_RESERVED+1,2 are used directly in nes_cpu.cpp.
    //       If the JSR PLAY at the start ever moves or changes, be sure to update that.

    if (nsf->nsf2_bits & 0x30) // uses IRQ or non-returning INIT
    {
        layer.Attach(&nsf2_vectors);
        nsf2_vectors.SetCPU(&cpu);
        nsf2_vectors.ForceVector(0,PLAYER_RESERVED+0x06); // NMI routine that calls PLAY
        nsf2_vectors.ForceVector(1,PLAYER_RESERVED+0x03); // Reset routine goes to "breaked" infinite loop (not used)
        nsf2_vectors.ForceVector(2,PLAYER_RESERVED+0x1D); // Default IRQ points to empty RTI.
        mem.Write(PLAYER_RESERVED+0x11,nsf->play_address & 0xFF);
        mem.Write(PLAYER_RESERVED+0x12,nsf->play_address >> 8);
    }
    if (nsf->nsf2_bits & 0x10) // uses IRQ
    {
        stack.Attach(&nsf2_irq);
    }

    if (bmax) layer.Attach (&bank);
    layer.Attach (&mem);

    dmc->SetMemory (&layer);

    // APU units are combined into a single bus
    apu_bus.Attach (sc[APU]);
    apu_bus.Attach (sc[DMC]);
    stack.Attach(&apu_bus);

    mixer.Attach (&amp[APU]);
    mixer.Attach (&amp[DMC]);

    rconv.SetCPU(&cpu);
    rconv.SetDMC(dmc);
    rconv.SetMMC5(NULL);

    if (nsf->use_mmc5)
    {
      stack.Attach (sc[MMC5]);
      mixer.Attach (&amp[MMC5]);
      rconv.SetMMC5(mmc5);
    }
    if (nsf->use_vrc6)
    {
      stack.Attach (sc[VRC6]);
      mixer.Attach (&amp[VRC6]);
    }
    if (nsf->use_vrc7)
    {
      int patch_set = (*config)["VRC7_PATCH"].GetInt();
      bool opll = nsf->vrc7_type == 1;
      if (config->GetDeviceOption(VRC7, NES_VRC7::OPT_OPLL).GetInt() != 0)
      {
        opll = true;
        nsf->vrc7_type = 1; // don't really want to modify nsf but this lets NSFTrackDialog notice the extra channels
      }

      if (opll) // YM2413 / OPLL
        patch_set = 7;
      vrc7->UseAllChannels(opll);
      vrc7->SetPatchSet(patch_set);
      vrc7->SetPatchSetCustom(nsf->vrc7_patches);
      stack.Attach (sc[VRC7]);
      mixer.Attach (&amp[VRC7]);
    }
    if (nsf->use_fme7)
    {
      stack.Attach (sc[FME7]);
      mixer.Attach (&amp[FME7]);
    }
    if (nsf->use_n106)
    {
      stack.Attach (sc[N106]);
      mixer.Attach (&amp[N106]);
    }
    if (nsf->use_fds)
    {
      bool write_enable = (config->GetDeviceOption(FDS, NES_FDS::OPT_WRITE_PROTECT).GetInt() == 0);
      bool multichip = 
        nsf->use_mmc5 ||
        nsf->use_vrc6 ||
        nsf->use_vrc7 ||
        nsf->use_fme7 ||
        nsf->use_n106 ;

      if (multichip) write_enable = false;

      stack.Attach (sc[FDS]); // last before memory layer
      mixer.Attach (&amp[FDS]);
      mem.SetFDSMode (write_enable);
      bank.SetFDSMode (write_enable);

      if (!multichip)
      {
        bank.SetBankDefault(6, nsf->bankswitch[6]);
        bank.SetBankDefault(7, nsf->bankswitch[7]);
      }
    }
    else
    {
      mem.SetFDSMode (false);
      bank.SetFDSMode (false);
    }

    // memory layer comes last
    stack.Attach (&layer);

    // NOTE: each layer in the stack is given a chance to take a read or write
    // exclusively. The stack is structured like this:
    //     loop detector > APU > expansions > main memory

    // main memory comes after other expansions because
    // when the FDS mode is enabled, VRC6/VRC7/5B have writable registers
    // in RAM areas of main memory. To prevent these from overwriting RAM
    // I allow the expansions above it in the stack to prevent them.

    // MMC5 comes high in the stack so that its PCM read behaviour
    // can reread from the stack below and does not get blocked by any
    // stack above.

    cpu.SetMemory (&stack);
  }

void NSFPlayer::SetPlayFreq (double r)
{
	rate = r;

	int region = GetRegion(nsf->regn, nsf->regn_pref);
	bool pal = (region == REGION_PAL);
	dmc->SetPal(pal);

	int quality = config->GetValue("QUALITY").GetInt();

	double clock;
	switch (region)
	{
		default:
		case REGION_NTSC:
			clock = config->GetValue("NTSC_BASECYCLES").GetInt();
			break;
		case REGION_PAL:
			clock = config->GetValue("PAL_BASECYCLES").GetInt();
			break;
		case REGION_DENDY:
			clock = config->GetValue("DENDY_BASECYCLES").GetInt();
			break;
	}
	double oversample = rate * quality;
	if (oversample > clock) oversample = clock;
	if (oversample < rate) oversample = rate;

	for (int i = 0; i < NES_DEVICE_MAX; i++)
	{
		sc[i]->SetClock(clock);
		sc[i]->SetRate(oversample);
	}
	rconv.SetClock(oversample);
	rconv.SetRate(rate);
	rconv.SetFastSkip(config->GetValue("FAST_SEEK")!=0);

	mixer.Reset();
	rconv.Reset();
	fader.Reset();
	lpf.SetRate(rate);
	lpf.Reset();
	dcf.SetRate(rate);
	dcf.Reset(); 
	DEBUG_OUT("rate: %f\n",rate);
}

  void NSFPlayer::SetChannels(int channels)
  {
    if (channels > 2) channels = 2;
    if (channels < 1) channels = 1;
    nch = channels;
  }

  void NSFPlayer::Reset ()
  {
    ::srand((unsigned)::time(NULL)); // randomizing random generator

    time_in_ms = 0;
    silent_length = 0;
    playtime_detected = false;
    total_render = 0;
    frame_render = (int)(rate)/60; // 演奏情報を更新する周期
    apu_clock_rest = 0.0;
    cpu_clock_rest = 0.0;

    int region = GetRegion(nsf->regn, nsf->regn_pref);
    switch (region)
    {
        default:
        case REGION_NTSC:
            cpu.nes_basecycles = config->GetValue("NTSC_BASECYCLES").GetInt();
            break;
        case REGION_PAL:
            cpu.nes_basecycles = config->GetValue("PAL_BASECYCLES").GetInt();
            break;
        case REGION_DENDY:
            cpu.nes_basecycles = config->GetValue("DENDY_BASECYCLES").GetInt();
            break;
    }

    if (logcpu->GetLogLevel() > 0)
        logcpu->Begin(GetTitleString());

    // 演奏後にRAM空間を破壊される場合があるので，再ロード
    Reload ();
    // レートの設定はResetより前に行っておくこと
    SetPlayFreq (rate);
    // 全てのコンフィグレーションを適用
    config->Notify (-1);
    // バスをリセット 
    stack.Reset ();
    // CPUリセットは必ずバスより後（重要）
    cpu.Reset ();

    double speed;
    if((*config)["VSYNC_ADJUST"])
        speed = ((region == REGION_NTSC) ? 60.0988 : 50.0070);
    else
    {
        UINT16 nsfspeed = (region == REGION_DENDY) ? nsf->speed_dendy :
                          (region == REGION_PAL  ) ? nsf->speed_pal :
                                                     nsf->speed_ntsc;
        speed = 1000000.0 / nsfspeed;
    }
    DEBUG_OUT("Playback mode: %s\n",
        (region==REGION_PAL)?"PAL":
        (region==REGION_DENDY)?"DENDY":
        "NTSC");
    DEBUG_OUT("Playback speed: %f\n", speed);

    int song = nsf->song;
    if (nsf->nsfe_plst)
    {
      song = nsf->nsfe_plst[song];
    }

    int region_register = (region == REGION_PAL) ? 1 : 0;
    if (region == REGION_DENDY && (nsf->regn & 4)) region_register = 2; // use 2 for Dendy iff explicitly supported, otherwise 0

    cpu.Start (
        nsf->init_address,
        nsf->play_address,
        speed,
        song,
        region_register,
        nsf->nsf2_bits,
        config->GetValue("IRQ_ENABLE").GetInt()!=0,
        &nsf2_irq );

    // マスク更新
    apu->SetMask( (*config)["MASK"].GetInt()    );
    dmc->SetMask( (*config)["MASK"].GetInt()>>2 );
    fds->SetMask( (*config)["MASK"].GetInt()>>5 );
    mmc5->SetMask((*config)["MASK"].GetInt()>>6 );
    fme7->SetMask((*config)["MASK"].GetInt()>>9 );
    vrc6->SetMask((*config)["MASK"].GetInt()>>12);
    vrc7->SetMask((*config)["MASK"].GetInt()>>15);
    n106->SetMask((*config)["MASK"].GetInt()>>21);

    for(int i=0;i<NES_TRACK_MAX;i++)
      infobuf[i].Clear();

    for (int i=0;i<NES_DEVICE_MAX;++i)
      NotifyPan(i);

    // suppress starting click by setting DC filter to balance the starting level at 0
    int quality = config->GetValue("QUALITY").GetInt();
    INT32 b[2];
    for (int i=0; i < NES_DEVICE_MAX; ++i) sc[i]->Tick(0); // determine starting state for all sound units
    fader.Tick(0);
    for (int i=0; i < (quality+1); ++i) fader.Render(b); // warm up rconv/render with enough sample to reach a steady state
    dcf.SetLevel(b); // DC filter will use the current DC level as its starting state
  }

  void NSFPlayer::DetectSilent ()
  {
    if (fader.IsFading () || playtime_detected || !nsf->playtime_unknown || nsf->UseNSFePlaytime())
      return;

    if ((*config)["MASK"].GetInt()==0 && (*config)["AUTO_STOP"].GetInt() &&
        (silent_length > rate * (*config)["STOP_SEC"].GetInt()))
    {
      playtime_detected = true;
      nsf->time_in_ms = time_in_ms - (*config)["STOP_SEC"].GetInt() * 1000 + 1000;
      nsf->loop_in_ms = 0;
      nsf->fade_in_ms = 0;
    }
  }

  void NSFPlayer::DetectLoop ()
  {
    if (fader.IsFading () || playtime_detected || !nsf->playtime_unknown || nsf->UseNSFePlaytime())
      return;

    if ((*config)["AUTO_DETECT"])
    {
      if (ld->IsLooped (time_in_ms, (*config)["DETECT_TIME"], (*config)["DETECT_INT"]))
      {
        playtime_detected = true;
        nsf->time_in_ms = ld->GetLoopEnd();
        nsf->loop_in_ms = ld->GetLoopEnd() - ld->GetLoopStart();
        nsf->fade_in_ms = -1;
      }
    }
  }

  void NSFPlayer::CheckTerminal ()
  {
    if(!infinite)
    {
      if (fader.IsFading ())
        return;

      if (time_in_ms + nsf->GetFadeTime () >= nsf->GetLength ())
        fader.FadeStart (rate, nsf->GetFadeTime ());
    }
  }

  UINT32 NSFPlayer::Skip (UINT32 length)
  {
    if (length)
    {
      int mult_speed = (*config)["MULT_SPEED"].GetInt();
      double apu_clock_per_sample = cpu.nes_basecycles / rate;
      double cpu_clock_per_sample = apu_clock_per_sample * ((double)(mult_speed)/256.0);

      for (UINT32 i = 0; i < length; i++)
      {
        total_render++;

        // tick CPU
        cpu_clock_rest += cpu_clock_per_sample;
        int cpu_clocks = (int)(cpu_clock_rest);
        // Moved to RateConverter:
        //if (cpu_clocks > 0)
        //{
        //    UINT32 real_cpu_clocks = cpu.Exec ( cpu_clocks );
        //    cpu_clock_rest -= (double)(real_cpu_clocks);
        //
        //  // tick APU frame sequencer
        //  dmc->TickFrameSequence(real_cpu_clocks);
        //  if (nsf->use_mmc5)
        //      mmc5->TickFrameSequence(real_cpu_clocks);
        //}
        rconv.TickCPU(cpu_clocks);
        cpu_clock_rest -= double(cpu_clocks);

        apu_clock_rest += apu_clock_per_sample;
        int apu_clocks = (int)(apu_clock_rest);
        if (apu_clocks > 0)
        {
            fader.Tick(apu_clocks); // ticks CPU via rconv as well
            apu_clock_rest -= (double)(apu_clocks);
        }

        fader.Skip(); // execute CPU/APU ticks via rconv.Skip
      }

      time_in_ms += (int)(1000 * length / rate * mult_speed / 256) ;
      CheckTerminal ();
      DetectLoop ();
    }
    return length;
  }

  void NSFPlayer::FadeOut (int fade_in_ms)
  {
    if (fade_in_ms < 0)
      fader.FadeStart (rate, (*config)["FADE_TIME"]);
    else
      fader.FadeStart (rate, fade_in_ms);
  }

  void NSFPlayer::UpdateInfo()
  {
    if(total_render%frame_render==0)
    {
      int i;

      for(i=0;i<2;i++)
        infobuf[APU1_TRK0+i].AddInfo(total_render,apu->GetTrackInfo(i));

      for(i=0;i<3;i++)
        infobuf[APU2_TRK0+i].AddInfo(total_render,dmc->GetTrackInfo(i));

      if(nsf->use_fds)
        infobuf[FDS_TRK0].AddInfo(total_render,fds->GetTrackInfo(0));

      if(nsf->use_vrc6)
      {
        for(i=0; i<3; i++)
          infobuf[VRC6_TRK0+i].AddInfo(total_render,vrc6->GetTrackInfo(i));
      }

      if(nsf->use_n106)
      {
        for(i=0;i<8;i++)
          infobuf[N106_TRK0+i].AddInfo(total_render,n106->GetTrackInfo(i));
      }

      if(nsf->use_vrc7)
      {
        for(i=0; i<6; i++)
          infobuf[VRC7_TRK0+i].AddInfo(total_render,vrc7->GetTrackInfo(i));
        if (nsf->vrc7_type == 1)
          for(i=6; i<9; i++)
            infobuf[VRC7_TRK6+i-6].AddInfo(total_render,vrc7->GetTrackInfo(i));
      }

      if(nsf->use_mmc5)
      {
        for(i=0; i<3; i++)
          infobuf[MMC5_TRK0+i].AddInfo(total_render,mmc5->GetTrackInfo(i));
      }

      if(nsf->use_fme7)
      {
        for(i=0; i<5; i++)
          infobuf[FME7_TRK0+i].AddInfo(total_render,fme7->GetTrackInfo(i));
      }
    }
  }
  
  IDeviceInfo *NSFPlayer::GetInfo(int time_in_ms, int id)
  {
    if(time_in_ms>=0)
    {
      int pos = (int)( rate * time_in_ms / 1000 );
      return infobuf[id].GetInfo(pos);
    }
    else
    {
      return infobuf[id].GetInfo(-1);
    }
  }

  UINT32 NSFPlayer::Render (INT16 * b, UINT32 length)
  {
    INT32 buf[2];
    INT32 out[2];
    INT32 outm;
    UINT32 i;
    int master_volume;

    master_volume = (*config)["MASTER_VOLUME"];

    int mult_speed = (*config)["MULT_SPEED"].GetInt();
    double apu_clock_per_sample = cpu.nes_basecycles / rate;
    double cpu_clock_per_sample = apu_clock_per_sample * ((double)(mult_speed)/256.0);

    for (i = 0; i < length; i++)
    {
      total_render++;

      // tick CPU
      cpu_clock_rest += cpu_clock_per_sample;
      int cpu_clocks = (int)(cpu_clock_rest);
      // Moved to RateConverter:
      //if (cpu_clocks > 0)
      //{
      //    UINT32 real_cpu_clocks = cpu.Exec ( cpu_clocks );
      //    cpu_clock_rest -= (double)(real_cpu_clocks);
      //
      //    // tick APU frame sequencer
      //    dmc->TickFrameSequence(real_cpu_clocks);
      //    if (nsf->use_mmc5)
      //        mmc5->TickFrameSequence(real_cpu_clocks);
      //}
      //UpdateInfo();
      rconv.TickCPU(cpu_clocks);
      cpu_clock_rest -= double(cpu_clocks);

      // tick fader and queue accumulated ticks for APU/CPU to be done during Render
      apu_clock_rest += apu_clock_per_sample;
      int apu_clocks = (int)(apu_clock_rest);
      if (apu_clocks > 0)
      {
          fader.Tick(apu_clocks);
          apu_clock_rest -= (double)(apu_clocks);
      }

      // render output
      fader.Render(buf); // ticks APU/CPU and renders with subdivision and resampling (also does UpdateInfo)
      outm = (buf[0] + buf[1]) >> 1; // mono mix
      if (outm == last_out) silent_length++; else silent_length = 0;
      last_out = outm;

      // echo.FastRender(buf);
      dcf.FastRender(buf);
      lpf.FastRender(buf);

      out[0] = buf[0];
      out[1] = buf[1];
      out[0] = (out[0]*master_volume)>>8;
      out[1] = (out[1]*master_volume)>>8;

      if     (out[0]<-32767) out[0]=-32767;
      else if( 32767<out[0]) out[0]= 32767;

      if     (out[1]<-32767) out[1]=-32767;
      else if( 32767<out[1]) out[1]= 32767;

      #if _DEBUG
          if (debug_mark)
          {
              out[0] = debug_mark;
              debug_mark = 0;
          }
      #endif

      if (nch == 2)
      {
          b[0] = out[0];
          b[1] = out[1];
      }
      else // if not 2 channels, presume mono
      {
          outm = (out[0] + out[1]) >> 1;
          for (int i=0; i < nch; ++i)
              b[0] = outm;
      }
      b += nch;

      UpdateInfo();
    }

    time_in_ms += (int)(1000 * length / rate * mult_speed / 256);

    CheckTerminal ();
    DetectLoop ();
    DetectSilent ();

    return length;
  }

  int NSFPlayer::GetLength ()
  {
    if (nsf == NULL) return 0;
    return nsf->GetLength ();
  }

  bool NSFPlayer::IsStopped ()
  {
    return fader.IsFadeEnd ();
  }

  bool NSFPlayer::SetSong (int s)
  {
    nsf->song = s % nsf->songs;
    UpdateInfinite();
    return true;
  }

  bool NSFPlayer::NextSong (int s)
  {
    nsf->song += s;
    bool result = true;
    while (nsf->song >= nsf->songs)
    {
        nsf->song -=nsf->songs;
        result = false;
    }
    UpdateInfinite();
    return result;
  }

  bool NSFPlayer::PrevSong (int s)
  {
    nsf->song -= s;
    bool result = true;
    while (nsf->song < 0)
    {
        nsf->song +=nsf->songs;
        result = false;
    }
    UpdateInfinite();
    return result;
  }

  int NSFPlayer::GetSong ()
  {
    return nsf->song;
  }

  void NSFPlayer::GetMemory (UINT8 * buf)
  {
    int i;
    UINT32 val;
    for (i = 0; i < 65536; i++)
    {
      cpu.Read (i, val);
      buf[i] = val;
    }
  }

  void NSFPlayer::GetMemoryString (char *buf)
  {
    static char itoa[] = "0123456789ABCDEF";
    UINT32 val;
    int i, t = 0;

    for (i = 0; i < 65536; i++)
    {
      cpu.Read (i, val);

      if ((i & 0xF) == 0)
      {
        if (i)
        {
          buf[t++] = '\r';
          buf[t++] = '\n';
        }
        buf[t++] = itoa[(i >> 12) & 0xF];
        buf[t++] = itoa[(i >> 8) & 0xF];
        buf[t++] = itoa[(i >> 4) & 0xF];
        buf[t++] = itoa[i & 0xF];
        buf[t++] = ':';
      }

      buf[t++] = itoa[(val >> 4) & 0xF];
      buf[t++] = itoa[val & 0xF];
      buf[t++] = ' ';
    }

    buf[t] = '\0';
  }

  // Update Configuration
  void NSFPlayer::Notify (int id)
  {
    int i;

    if (id == -1)
    {
      for (i = 0; i < NES_DEVICE_MAX; i++)
        Notify (i);

      dcf.SetParam(270,(*config)["HPF"]);
      lpf.SetParam(4700.0,(*config)["LPF"]);

      //DEBUG_OUT("dcf: %3d > %f\n", (*config)["HPF"].GetInt(), dcf.GetFactor());
      //DEBUG_OUT("lpf: %3d > %f\n", (*config)["LPF"].GetInt(), lpf.GetFactor());

      return;
    }

    // adjust volume by NSFE mixe chunk

    const int MIXE_DEVICE_MAP[NES_DEVICE_MAX] = { 0, 1, 7, 5, 6, 2, 3, 4 }; // map device ID to mixe
    const int MIXE_DEVICE_ADJUST[NSFE_MIXES] =
    {
        // millibels difference between device "square" and APU square
        0,    // APU (1x)
        -20, // DMC
        0,    // VRC6 (1x)
        1340, // VRC7
        690,  // FDS (~2.4x)
        0,    // MMC5 (1x)
        1540, // N163 (~6.0x)
        -250,  // 5B
    };

    const int mixe_device = MIXE_DEVICE_MAP[id];
    int mixe = MIXE_DEVICE_ADJUST[mixe_device];
    if (nsf)
    {
        int new_mixe = nsf->nsfe_mixe[mixe_device];
        if (new_mixe != NSFE_MIXE_DEFAULT)
        {
            mixe = new_mixe;
        }
    }
    mixe -= MIXE_DEVICE_ADJUST[mixe_device]; // millibels to adjust volume

    int device_volume = config->GetDeviceConfig(id,"VOLUME");
    if (mixe != 0)
    {
        double adjust = ::pow(10,double(mixe)/2000.0); // millibels to amplitude change
        device_volume = int(128.0 * adjust);
        if (device_volume > 1024) device_volume = 1024; // sanity clamp to 4x default
        if (device_volume < 0) device_volume = 0; // should be impossible
    }

    amp[id].SetVolume (device_volume);
    amp[id].SetMute (config->GetDeviceConfig(id,"MUTE"));

    switch (id)
    {
    case APU:
      for (i = 0; i < NES_APU::OPT_END; i++)
        apu->SetOption (i, config->GetDeviceOption(id,i));
      apu->SetMask((*config)["MASK"].GetInt());
      break;
    case DMC:
      for (i = 0; i < NES_DMC::OPT_END; i++)
        dmc->SetOption (i, config->GetDeviceOption(id,i));
      dmc->SetMask((*config)["MASK"].GetInt()>>2);
      break;
    case FDS:
      for (i = 0; i < NES_FDS::OPT_END; i++)
        fds->SetOption (i, config->GetDeviceOption(id,i).GetInt());
      fds->SetMask((*config)["MASK"].GetInt()>>5);
      break;
    case MMC5:
      for (i = 0; i < NES_MMC5::OPT_END; i++)
        mmc5->SetOption (i, config->GetDeviceOption(id,i));
      mmc5->SetMask((*config)["MASK"].GetInt()>>6);
      break;
    case FME7:
      for (i = 0; i < NES_FME7::OPT_END; i++)
        fme7->SetOption (i, config->GetDeviceOption(id,i));
      fme7->SetMask((*config)["MASK"].GetInt()>>9);
      break;
    case VRC6:
      for (i = 0; i < NES_VRC6::OPT_END; i++)
        vrc6->SetOption (i, config->GetDeviceOption(id,i));
      vrc6->SetMask((*config)["MASK"].GetInt()>>12);
      break;
    case VRC7:
      for (i = 0; i < NES_VRC7::OPT_END; i++)
        vrc7->SetOption (i, config->GetDeviceOption(id,i));
      vrc7->SetMask((*config)["MASK"].GetInt()>>15);
      break;
    case N106:
      for (i = 0; i < NES_N106::OPT_END; i++)
        n106->SetOption (i, config->GetDeviceOption(id,i));
      n106->SetMask((*config)["MASK"].GetInt()>>21);
      break;
    default:
      break;
    }

    NotifyPan(id);
    UpdateInfinite();
  }

  void NSFPlayer::NotifyPan (int id)
  {
    if (id == -1)
    {
      for (int i = 0; i < NES_DEVICE_MAX; i++)
        NotifyPan (i);
      return;
    }

    for (int i=0;i<NES_CHANNEL_MAX;++i)
    {
        int d = config->channel_device[i];
        if (d != id) continue;

        int pan = config->GetChannelConfig(i, "PAN"); // 0 = full left, 255 = full right, 128 = centre
        int vol = config->GetChannelConfig(i, "VOL"); // 128 = full volume
        int r = (pan + 1) >> 1; // +1 and truncation is intentional
        // r: 0 -> 0 (left), 128 -> 64 (mid), 255 -> 128 (right)
        if (r < 0) r = 0;
        if (r > 128) r = 128;
        int l = 128 - r;
        l = (l * vol) / 128;
        r = (r * vol) / 128;
        l <<= 1; // undo truncation
        r <<= 1;
        int ci = config->channel_device_index[i];
        switch(d)
        {
            case APU: apu->SetStereoMix(ci,l,r); break;
            case DMC: dmc->SetStereoMix(ci,l,r); break;
            case FDS: fds->SetStereoMix(ci,l,r); break;
            case MMC5: mmc5->SetStereoMix(ci,l,r); break;
            case FME7: fme7->SetStereoMix(ci,l,r); break;
            case VRC6: vrc6->SetStereoMix(ci,l,r); break;
            case VRC7: vrc7->SetStereoMix(ci,l,r); break;
            case N106: n106->SetStereoMix(ci,l,r); break;
        }
    }
  }

  int NSFPlayer::GetRegion (UINT8 flags, int pref)
  {
      int user = config->GetValue("REGION").GetInt();

      // user forced region
      if (user == 4) return REGION_NTSC;
      if (user == 5) return REGION_PAL;
      if (user == 6) return REGION_DENDY;

      int nsf_region = -1;
      int usr_region = -1;

      if (pref >= 0) // NSF preference
      {
          if (pref == 0) nsf_region = REGION_NTSC;
          if (pref == 1) nsf_region = REGION_PAL;
          if (pref == 2) nsf_region = REGION_DENDY;
      }

      if (user > 0) // user preference
      {
          if (user == 1) usr_region = REGION_NTSC;
          if (user == 2) usr_region = REGION_PAL;
          if (user == 3) usr_region = REGION_DENDY;

          // cancel preference if unavailable
          if (usr_region == REGION_DENDY && !(flags & 4)) usr_region = -1;
          if (usr_region == REGION_PAL   && !(flags & 2)) usr_region = -1;
          if (usr_region == REGION_NTSC  && !(flags & 1)) usr_region = -1;
      }

      // user preference first, then fall back to NSF preference
      int region = usr_region;
      if (region < 0) region = nsf_region;

      // extra fallback if the chosen region is not supported
      if (region == REGION_DENDY && !(flags & 4)) region = -1;
      if (region == REGION_PAL   && !(flags & 2)) region = -1;
      if (region == REGION_NTSC  && !(flags & 1)) region = -1;

      // a valid region!
      if (region >= REGION_NTSC && region <= REGION_DENDY) return region;

      // any port in a storm
      if (flags & 1) return REGION_NTSC;
      if (flags & 2) return REGION_PAL;
      if (flags & 4) return REGION_DENDY;

      // no valid regions? giving up
      return REGION_NTSC;
  }

  void NSFPlayer::UpdateInfinite()
  {
      infinite = 1 == (*config)["PLAY_ADVANCE"];
  }

}

