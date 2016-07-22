#include "nsfplay.h"

#include <assert.h>
#include <typeinfo>
#include <time.h> // for srand() initialization
#include <cstring>

namespace xgm
{
  NSFPlayer::NSFPlayer () : PlayerMSP ()
  {
    nsf = NULL;

    const std::type_info &ti = typeid(this);
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

    dmc->SetAPU(apu); // set APU
    mmc5->SetCPU(&cpu); // MMC5 PCM read action requires CPU read access

    /* Av©tB^©[gRo[^©¹¹ ðÚ± */
    for (int i = 0; i < NES_DEVICE_MAX; i++)
    {
      rconv[i].Attach (sc[i]);
      filter[i].Attach (&rconv[i]);
      amp[i].Attach (&filter[i]);
    }
    mfilter = new MedianFilter(5);

    nch = 1;
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
    delete mfilter;
  }

  void NSFPlayer::SetConfig(PlayerConfig *pc)
  {
#if defined (WIN32)
    config = dynamic_cast<NSFPlayerConfig *>(pc);
#endif
    PlayerMSP::SetConfig(pc);
  }

  bool NSFPlayer::IsDetected ()
  {
    return playtime_detected;
  }

  char *NSFPlayer::GetTitleString ()
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
    UINT32 offset = 0;

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

    if (bmax) layer.Attach (&bank);
    layer.Attach (&mem);

    dmc->SetMemory (&layer);

    // APU units are combined into a single bus
    apu_bus.Attach (sc[APU]);
    apu_bus.Attach (sc[DMC]);
    stack.Attach(&apu_bus);

    mixer.Attach (&amp[APU]);
    mixer.Attach (&amp[DMC]);

    if (nsf->use_mmc5)
    {
      stack.Attach (sc[MMC5]);
      mixer.Attach (&amp[MMC5]);
    }
    if (nsf->use_vrc6)
    {
      stack.Attach (sc[VRC6]);
      mixer.Attach (&amp[VRC6]);
    }
    if (nsf->use_vrc7)
    {
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

      stack.Attach (sc[FDS]); // last before memory layer
      mixer.Attach (&amp[FDS]);
      mem.SetFDSMode (write_enable);
      bank.SetFDSMode (write_enable);

      bank.SetBankDefault(6, nsf->bankswitch[6]);
      bank.SetBankDefault(7, nsf->bankswitch[7]);
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

    int region = GetRegion(nsf->pal_ntsc);
    bool pal = (region == REGION_PAL);
    dmc->SetPal(pal);

    for (int i = 0; i < NES_DEVICE_MAX; i++)
    {
      int quality = config->GetDeviceConfig(i,"QUALITY");

      // [gRo[^ðgp·é
      int MULT[NES_DEVICE_MAX][4] = { 
        1, 5, 8, 20, // APU1
        1, 5, 8, 20, // DMC
        1, 5, 8,  8, // FME7
        1, 5, 8, 20, // MMC5
        1, 5, 8, 20, // N106
        1, 5, 8, 20, // VRC6
        1, 3, 3,  3, // VRC7
        1, 5, 8, 20  // FDS
      };

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
      sc[i]->SetClock(clock);

      int mult = config->GetDeviceConfig(i,"QUALITY").GetInt() & 3;

      sc[i]->SetRate(rate * MULT[i][mult]);

      rconv[i].SetClock(rate * MULT[i][mult]);
      rconv[i].SetRate(rate);
      rconv[i].Reset();

      if (quality)
      {
        filter[i].Attach (&rconv[i]);
      }
      else
      {
        // [gRo[^ÍgpµÈ¢
        filter[i].Attach (sc[i]);
      }
      // tB^®ìügÌÝè
      filter[i].SetRate(rate);
      filter[i].Reset();
    }
    mixer.Reset ();
    echo.SetRate(rate);
    echo.Reset();
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

    mfilter->Reset();
    time_in_ms = 0;  
    silent_length = 0; 
    playtime_detected = false;
    click_mode = PRE_CLICK;
    total_render = 0;
    frame_render = (int)(rate)/60; // tîñðXV·éüú
    apu_clock_rest = 0.0;
    cpu_clock_rest = 0.0;

    int region = GetRegion(nsf->pal_ntsc);
    switch (region)
    {
        default:
        case REGION_NTSC:
            cpu.NES_BASECYCLES = config->GetValue("NTSC_BASECYCLES").GetInt();
            break;
        case REGION_PAL:
            cpu.NES_BASECYCLES = config->GetValue("PAL_BASECYCLES").GetInt();
            break;
        case REGION_DENDY:
            cpu.NES_BASECYCLES = config->GetValue("DENDY_BASECYCLES").GetInt();
            break;
    }

    if (logcpu->GetLogLevel() > 0)
        logcpu->Begin(GetTitleString());

    // tãÉRAMóÔðjó³êéêª éÌÅCÄ[h
    Reload ();
    // [gÌÝèÍResetæèOÉsÁÄ¨­±Æ
    SetPlayFreq (rate);
    // SÄÌRtBO[VðKp
    config->Notify (-1);
    // oXðZbg 
    stack.Reset ();
    // CPUZbgÍK¸oXæèãidvj
    cpu.Reset ();

    double speed;
    if((*config)["VSYNC_ADJUST"])
        speed = ((region == REGION_NTSC) ? 60.0988 : 50.0070);
    else
        speed = 1000000.0 / ((region == REGION_NTSC)? nsf->speed_ntsc : nsf->speed_pal);
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

    cpu.Start (nsf->init_address, nsf->play_address, speed, song, (region == REGION_PAL)?1:0);

    // }XNXV
    apu->SetMask( (*config)["MASK"].GetInt()    );
    dmc->SetMask( (*config)["MASK"].GetInt()>>2 );
    fds->SetMask( (*config)["MASK"].GetInt()>>5 );
    mmc5->SetMask((*config)["MASK"].GetInt()>>6 );
    fme7->SetMask((*config)["MASK"].GetInt()>>9 );
    vrc6->SetMask((*config)["MASK"].GetInt()>>12);
    vrc7->SetMask((*config)["MASK"].GetInt()>>15);
    n106->SetMask((*config)["MASK"].GetInt()>>21);

    vrc7->SetPatchSet((*config)["VRC7_PATCH"].GetInt());

    for(int i=0;i<NES_TRACK_MAX;i++)
      infobuf[i].Clear();

    for (int i=0;i<NES_DEVICE_MAX;++i)
      NotifyPan(i);
  }

  void NSFPlayer::DetectSilent ()
  {
    if (mixer.IsFading () || playtime_detected || !nsf->playtime_unknown || nsf->UseNSFePlaytime())
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
    if (mixer.IsFading () || playtime_detected || !nsf->playtime_unknown || nsf->UseNSFePlaytime())
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
    if (mixer.IsFading ())
      return;

    if (time_in_ms + nsf->GetFadeTime () >= nsf->GetLength ())
      mixer.FadeStart (rate, nsf->GetFadeTime ());
  }

  UINT32 NSFPlayer::Skip (UINT32 length)
  {
    if (length)
    { 
      double apu_clock_per_sample = cpu.NES_BASECYCLES / rate;
      double cpu_clock_per_sample = apu_clock_per_sample * ((double)((*config)["MULT_SPEED"].GetInt())/256.0);

      for (UINT32 i = 0; i < length; i++)
      {
        total_render++;

        // tick CPU
        cpu_clock_rest += cpu_clock_per_sample;
        int cpu_clocks = (int)(cpu_clock_rest);
        if (cpu_clocks > 0)
        {
            UINT32 real_cpu_clocks = cpu.Exec ( cpu_clocks );
            cpu_clock_rest -= (double)(real_cpu_clocks);

            // tick APU frame sequencer
            dmc->TickFrameSequence(real_cpu_clocks);
            if (nsf->use_mmc5)
                mmc5->TickFrameSequence(real_cpu_clocks);
        }

        // skip APU / expansions
        apu_clock_rest += apu_clock_per_sample;
        int apu_clocks = (int)(apu_clock_rest);
        if (apu_clocks > 0)
        {
            //mixer.Tick(apu_clocks);
            apu_clock_rest -= (double)(apu_clocks);
        }
      }

      mixer.Skip (length);
      time_in_ms += (int)(1000 * length / rate * ((*config)["MULT_SPEED"].GetInt()) / 256) ;
      CheckTerminal ();
      DetectLoop ();
    }
    return length;
  }

  void NSFPlayer::FadeOut (int fade_in_ms)
  {
    if (fade_in_ms < 0)
      mixer.FadeStart (rate, (*config)["FADE_TIME"]);
    else
      mixer.FadeStart (rate, fade_in_ms);
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

    double apu_clock_per_sample = cpu.NES_BASECYCLES / rate;
    double cpu_clock_per_sample = apu_clock_per_sample * ((double)((*config)["MULT_SPEED"].GetInt())/256.0);

    for (i = 0; i < length; i++)
    {
      total_render++;

      // tick CPU
      cpu_clock_rest += cpu_clock_per_sample;
      int cpu_clocks = (int)(cpu_clock_rest);
      if (cpu_clocks > 0)
      {
          UINT32 real_cpu_clocks = cpu.Exec ( cpu_clocks );
          cpu_clock_rest -= (double)(real_cpu_clocks);

          // tick APU frame sequencer
          dmc->TickFrameSequence(real_cpu_clocks);
          if (nsf->use_mmc5)
              mmc5->TickFrameSequence(real_cpu_clocks);
      }

      UpdateInfo();

      // tick APU / expansions
      apu_clock_rest += apu_clock_per_sample;
      int apu_clocks = (int)(apu_clock_rest);
      if (apu_clocks > 0)
      {
          mixer.Tick(apu_clocks);
          apu_clock_rest -= (double)(apu_clocks);
      }

      // render output
      mixer.Render(buf);
      outm = (buf[0] + buf[1]) >> 1; // mono mix
      if (outm == last_out) silent_length++; else silent_length = 0;
      last_out = outm;

      // echo.FastRender(buf);
      dcf.FastRender(buf);
      lpf.FastRender(buf);
      cmp.FastRender(buf);

      //mfilter->Put(buf[0]);
      //out = mfilter->Get();

      out[0] = buf[0];
      out[1] = buf[1];
      out[0] = (out[0]*master_volume)>>8;
      out[1] = (out[1]*master_volume)>>8;

      if     (out[0]<-32767) out[0]=-32767;
      else if( 32767<out[0]) out[0]= 32767;

      if     (out[1]<-32767) out[1]=-32767;
      else if( 32767<out[1]) out[1]= 32767;

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
    }

    time_in_ms += (int)(1000 * length / rate * ((*config)["MULT_SPEED"].GetInt()) / 256);

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
    return mixer.IsFadeEnd ();
  }

  bool NSFPlayer::SetSong (int s)
  {
    nsf->song = s % nsf->songs;
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

      cmp.SetParam(
        0.01 * ((*config)["COMP_LIMIT"]), 
        0.01 * ((*config)["COMP_THRESHOLD"]),
        0.01 * ((*config)["COMP_VELOCITY"]));

      dcf.SetParam(270,(*config)["HPF"]);
      lpf.SetParam(4700.0,(*config)["LPF"]);

      //DEBUG_OUT("dcf: %3d > %f\n", (*config)["HPF"].GetInt(), dcf.GetFactor());
      //DEBUG_OUT("lpf: %3d > %f\n", (*config)["LPF"].GetInt(), lpf.GetFactor());

      return;
    }

    filter[id].SetParam (4700.0, config->GetDeviceConfig(id,"FILTER").GetInt());

    amp[id].SetVolume (config->GetDeviceConfig(id,"VOLUME"));
    amp[id].SetMute (config->GetDeviceConfig(id,"MUTE"));
    //amp[id].SetCompress (config->GetDeviceConfig(id,"THRESHOLD"), config->GetDeviceConfig(id,"TWEIGHT"));
    amp[id].SetCompress (config->GetDeviceConfig(id,"THRESHOLD"), -1);

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
      fme7->SetMask((*config)["MASK"].GetInt()>>9);
      break;
    case VRC6:
      vrc6->SetMask((*config)["MASK"].GetInt()>>12);
      break;
    case VRC7:
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

  int NSFPlayer::GetRegion (UINT8 flags)
  {
      int pref = config->GetValue("REGION").GetInt();

      // user forced region
      if (pref == 3) return REGION_NTSC;
      if (pref == 4) return REGION_PAL;
      if (pref == 5) return REGION_DENDY;

      // single-mode NSF
      if (flags == 0) return REGION_NTSC;
      if (flags == 1) return REGION_PAL;

      if (flags & 2) // dual mode
      {
          if (pref == 1) return REGION_NTSC;
          if (pref == 2) return REGION_PAL;
          // else pref == 0 or invalid, use auto setting based on flags bit
          return (flags & 1) ? REGION_PAL : REGION_NTSC;
      }

      return REGION_NTSC; // fallback for invalid flags
  }

}

