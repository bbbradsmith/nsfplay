#include <assert.h>
#include <typeinfo>
#include "nsfplay.h"

#include <time.h> // for srand() initialization

namespace xgm
{
  NSFPlayer::NSFPlayer () : PlayerMSP ()
  {
    const type_info &ti = typeid(this);
    sc[APU] = (apu = new NES_APU());
    sc[DMC] = (dmc = new NES_DMC());
    sc[FDS] = (fds = new NES_FDS());
    sc[FME7] = (fme7 = new NES_FME7());
    sc[MMC5] = (mmc5 = new NES_MMC5());
    sc[N106] = (n106 = new NES_N106());
    sc[VRC6] = (vrc6 = new NES_VRC6());
    sc[VRC7] = (vrc7 = new NES_VRC7());
    ld = new NESDetector();

    mmc5->SetCPU(&cpu); // BS MMC5 PCM read action requires CPU read access

    /* アンプ←フィルタ←レートコンバータ←音源 を接続 */
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
    delete mfilter;
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

  char *NSFPlayer::GetTitleString ()
  {
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

    // データにあわせたVMを作る
    bus.DetachAll ();
    layer.DetachAll ();
    mixer.DetachAll ();

    // ループディテクタの切り替え
    if((*config)["DETECT_ALT"])
    {
      const type_info &ti = typeid(ld);
      if(strcmp(ti.name(),"NESDetectorEx")!=0)
      {
        delete ld;
        ld = new NESDetectorEx();
      }
    }
    else
    {
      const type_info &ti = typeid(ld);
      if(strcmp(ti.name(),"NESDetector")!=0)
      {
        delete ld;
        ld = new NESDetector();
      }
    }

    // ループディテクタのアタッチ
    layer.Attach (ld);

    if (bmax) layer.Attach (&bank);
    layer.Attach (&mem);

    dmc->SetMemory (&layer);

    bus.Attach (&layer);
    bus.Attach (sc[APU]);
    bus.Attach (sc[DMC]);

    mixer.Attach (&amp[APU]);
    mixer.Attach (&amp[DMC]);

    if (nsf->use_vrc6)
    {
      bus.Attach (sc[VRC6]);
      mixer.Attach (&amp[VRC6]);
    }
    if (nsf->use_vrc7)
    {
      bus.Attach (sc[VRC7]);
      mixer.Attach (&amp[VRC7]);
    }
    if (nsf->use_fme7)
    {
      bus.Attach (sc[FME7]);
      mixer.Attach (&amp[FME7]);
    }
    if (nsf->use_mmc5)
    {
      bus.Attach (sc[MMC5]);
      mixer.Attach (&amp[MMC5]);
    }
    if (nsf->use_n106)
    {
      bus.Attach (sc[N106]);
      mixer.Attach (&amp[N106]);
    }
    if (nsf->use_fds)
    {
      bus.Attach (sc[FDS]);
      mixer.Attach (&amp[FDS]);
      mem.SetFDSMode (true);
      bank.SetFDSMode (true);

      /** 
       * 本来は必要なはずだが，FDSバンクの書き換えで対応する$E000-$FFFFを
       * 潰すものがあるのでコメントアウト．FDSバンクと通常バンク空間を分
       * けるべきなのか？
       */
      //bank.SetBankDefault(6, nsf->bankswitch[6]);
      //bank.SetBankDefault(7, nsf->bankswitch[7]);
    }
    else
    {
      mem.SetFDSMode (false);
      bank.SetFDSMode (false);
    }

    cpu.SetMemory (&bus);
  }

  void NSFPlayer::SetPlayFreq (double r)
  {
    rate = r;

    dmc->SetPal(UsePal(nsf->pal_ntsc));

    for (int i = 0; i < NES_DEVICE_MAX; i++)
    {
      if(config->GetDeviceConfig(i,"QUALITY"))
      {
        // レートコンバータを使用する
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
        sc[i]->SetClock(UsePal(nsf->pal_ntsc) ?
            config->GetValue("PAL_BASECYCLES").GetInt() :
            config->GetValue("NES_BASECYCLES").GetInt());

        sc[i]->SetRate(rate * MULT[i][config->GetDeviceConfig(i,"QUALITY").GetInt()&3]);
        rconv[i].SetClock(rate * MULT[i][config->GetDeviceConfig(i,"QUALITY").GetInt()&3]);
        rconv[i].SetRate(rate);
        rconv[i].Reset();
        filter[i].Attach (&rconv[i]);
      }
      else
      {
        // レートコンバータは使用しない
        sc[i]->SetRate (rate);
        filter[i].Attach (sc[i]);
      }
      // フィルタ動作周波数の設定
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
    ::srand((unsigned)::time(NULL)); // BS randomizing random generator

    mfilter->Reset();
    time_in_ms = 0;  
    silent_length = 0; 
    playtime_detected = false;
    click_mode = PRE_CLICK;
    total_render = 0;
    frame_render = (int)(rate)/60; // 演奏情報を更新する周期
    clock_rest = 0;

    cpu.NES_BASECYCLES = UsePal(nsf->pal_ntsc) ?
        config->GetValue("PAL_BASECYCLES").GetInt() :
        config->GetValue("NES_BASECYCLES").GetInt() ;

    // 演奏後にRAM空間を破壊される場合があるので，再ロード
    Reload ();
    // レートの設定はResetより前に行っておくこと
    SetPlayFreq (rate);
    // 全てのコンフィグレーションを適用
    config->Notify (-1);
    // バスをリセット 
    bus.Reset ();
    // CPUリセットは必ずバスより後（重要）
    cpu.Reset ();

    double speed;
    if((*config)["VSYNC_ADJUST"])
      speed = (UsePal(nsf->pal_ntsc) ? 50.0070 : 60.0988);
    else
      speed = 1000000.0 / (UsePal(nsf->pal_ntsc)?nsf->speed_pal:nsf->speed_ntsc);
    DEBUG_OUT("Playback mode: %s\n", UsePal(nsf->pal_ntsc)?"PAL":"NTSC");
    DEBUG_OUT("Playback speed: %f\n", speed);

    int song = nsf->song;
    if (nsf->nsfe_plst)
    {
      song = nsf->nsfe_plst[song];
    }

    cpu.Start (nsf->init_address, nsf->play_address, speed, song, UsePal(nsf->pal_ntsc)?1:0);

    // マスク更新
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
    if (mixer.IsFading () || playtime_detected || !nsf->playtime_unknown)
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
    if (mixer.IsFading () || playtime_detected || !nsf->playtime_unknown)
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
      int clock_per_sample;
      clock_per_sample = (int)((double)(1<<20)*cpu.NES_BASECYCLES/12/rate * ((*config)["MULT_SPEED"].GetInt()) / 256);

      //int fast_seek = (*config)["FAST_SEEK"];
      
      //if(fast_seek) cpu.SetMemory(&dummy_bus);

      for (UINT32 i = 0; i < length; i++)
      {
        total_render++;
        clock_rest += clock_per_sample;
        if (clock_rest > 0)
        {
            UINT32 clocks = cpu.Exec ( clock_rest>>20 );
            clock_rest -= (clocks << 20);
        }
      }

      //if(fast_seek) cpu.SetMemory(&bus);

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
    int clock_per_sample;
    int master_volume;

    master_volume = (*config)["MASTER_VOLUME"];
    clock_per_sample = (int)((double)(1<<20)*cpu.NES_BASECYCLES/12/rate*((*config)["MULT_SPEED"].GetInt())/256) ;

    for (i = 0; i < length; i++)
    {
      total_render++;

      clock_rest += clock_per_sample;
      if (clock_rest > 0)
      {
          UINT32 clocks = cpu.Exec ( clock_rest>>20 );
          clock_rest -= (clocks << 20);
      }

      UpdateInfo();

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
    nsf->song = (nsf->song + s) % nsf->songs;

    if( nsf->song <= nsf->start-1 )
    {
      nsf->song = nsf->start-1;
      return false;
    }
    else
      return true;
  }

  bool NSFPlayer::PrevSong (int s)
  {
    nsf->song = nsf->song + ( nsf->songs << 8 ) - s;
    nsf->song %= nsf->songs;
    return true;
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

      DEBUG_OUT("dcf: %3d > %f\n", (*config)["HPF"].GetInt(), dcf.GetFactor());
      DEBUG_OUT("lpf: %3d > %f\n", (*config)["LPF"].GetInt(), lpf.GetFactor());

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

  bool NSFPlayer::UsePal (UINT8 flags)
  {
      if (flags == 0) return false; // NTSC only
      if (flags == 1) return true; // PAL only
      // DUAL mode, use preference
      int pref = config->GetValue("PREFER_PAL").GetInt();
      return (pref != 0);
  }

}

