#include "nes_vrc7.h"

namespace xgm
{
  NES_VRC7::NES_VRC7 ()
  {
    use_all_channels = false;
    patch_set = VRC7_NUKE_TONE;
    patch_custom = NULL;

    vrc7_s = vrc7_new ();
    SetClock(DEFAULT_CLOCK);

    for(int c=0;c<2;++c)
        //for(int t=0;t<6;++t)
        for(int t=0;t<9;++t) // HACK for YM2413 support
            sm[c][t] = 128;
  }

  NES_VRC7::~NES_VRC7 ()
  {
    vrc7_delete (vrc7_s);
  }

  void NES_VRC7::UseAllChannels(bool b)
  {
    use_all_channels = b;
  }

  void NES_VRC7::SetPatchSet(int p)
  {
    patch_set = p;
  }

  void NES_VRC7::SetPatchSetCustom (const UINT8* pset)
  {
    patch_custom = pset;
  }

  void NES_VRC7::SetClock (double c)
  {
    clock = c / 36;
	vrc7_set_clock_rate(vrc7_s,c);
  }

  void NES_VRC7::SetRate (double r)
  {
    rate = 49716;
    vrc7_set_sample_rate(vrc7_s,(uint32_t)rate);
  }

  void NES_VRC7::Reset ()
  {
	  vrc7_reset(vrc7_s);

	divider = 0;
	if (patch_custom)
		vrc7_set_patch_set(vrc7_s, (int) patch_custom);
	else
		vrc7_set_patch_set(vrc7_s, patch_set);
  }

  void NES_VRC7::SetStereoMix(int trk, xgm::INT16 mixl, xgm::INT16 mixr)
  {
      if (trk < 0) return;
      //if (trk > 5) return;
      if (trk >= VRC7_NUM_CHANNELS) return; 
      sm[0][trk] = mixl;
      sm[1][trk] = mixr;
	  vrc7_s->stereo_volume[STEREO_LEFT][trk] = (double)mixl / 128;
	  vrc7_s->stereo_volume[STEREO_RIGHT][trk] = (double)mixr / 128;
  }

  ITrackInfo *NES_VRC7::GetTrackInfo(int trk)
  {
    if(vrc7_s && trk<VRC7_NUM_CHANNELS)
    {
		
      trkinfo[trk].max_volume = 15;
      trkinfo[trk].volume = 15 - ((vrc7_s->channels[trk]->volume)&15);
	  trkinfo[trk]._freq = vrc7_s->channels[trk]->fNum;
      int blk = (vrc7_s->channels[trk]->octave)&7;
      trkinfo[trk].freq = clock*trkinfo[trk]._freq/(double)(0x80000>>blk);
      trkinfo[trk].tone = (vrc7_s->channels[trk]->instrument)&15;
      trkinfo[trk].key = vrc7_s->channels[trk]->trigger;
	  
      return &trkinfo[trk];
    }
    else
      return NULL;
  }

  bool NES_VRC7::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    if (adr == 0x9010)
    {
      vrc7_write_addr(vrc7_s, val);
      return true;
    }
    if (adr == 0x9030)
    {
	  vrc7_write_data(vrc7_s, val);
      return true;
    }
    else
      return false;
  }

  bool NES_VRC7::Read (UINT32 adr, UINT32 & val, UINT32 id)
  {
    return false;
  }

  void NES_VRC7::Tick (UINT32 clocks)
  {
    divider += clocks;
    while (divider >= 36)
    {
        divider -= 36;
        vrc7_tick(vrc7_s);
    }
  }

  UINT32 NES_VRC7::Render (INT32 b[2])
  {
	  b[0] = vrc7_s->signal[STEREO_LEFT][0];
	  b[1] = vrc7_s->signal[STEREO_RIGHT][0];

    b[0] >>= (7 - 4);
    b[1] >>= (7 - 4);

    // master volume adjustment
    const INT32 MASTER = INT32(0.75 * 256.0);
    b[0] = (b[0] * MASTER) >> 8;
    b[1] = (b[1] * MASTER) >> 8;

    return 2;
  }
}
