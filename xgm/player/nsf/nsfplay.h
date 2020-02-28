#ifndef _LIBNSF_H_
#define _LIBNSF_H_
#include "../player.h"
#include "nsfconfig.h"
#include "nsf.h"

#include "../../devices/CPU/nes_cpu.h"
#include "../../devices/Memory/nes_bank.h"
#include "../../devices/Memory/nes_mem.h"
#include "../../devices/Memory/nsf2_vectors.h"
#include "../../devices/Sound/nes_apu.h"
#include "../../devices/Sound/nes_vrc7.h"
#include "../../devices/Sound/nes_fme7.h"
#include "../../devices/Sound/nes_vrc6.h"
#include "../../devices/Sound/nes_dmc.h"
#include "../../devices/Sound/nes_mmc5.h"
#include "../../devices/Sound/nes_n106.h"
#include "../../devices/Sound/nes_fds.h"
#include "../../devices/Audio/filter.h"
#include "../../devices/Audio/mixer.h"
#include "../../devices/Audio/fader.h"
#include "../../devices/Audio/amplifier.h"
#include "../../devices/Audio/rconv.h"
#include "../../devices/Audio/echo.h"
#include "../../devices/Audio/MedianFilter.h"
#include "../../devices/Misc/nsf2_irq.h"
#include "../../devices/Misc/nes_detect.h"
#include "../../devices/Misc/log_cpu.h"

namespace xgm
{

  class NSFPlayer : public PlayerMSP
  {
  protected:
    NSFPlayerConfig *config;

    enum { PRE_CLICK, CLICKING, POST_CLICK };

    int click_mode;
    double rate;
    int nch; // number of channels
    int song;

    INT32 last_out;
    int silent_length;

    double cpu_clock_rest;
    double apu_clock_rest;

    int time_in_ms;             // ���t��������(ms)
    bool playtime_detected;     // ���t���Ԃ����o���ꂽ��true
    bool infinite;               // never fade out

    void Reload ();
    void DetectLoop ();
    void DetectSilent ();
    void CheckTerminal ();

  public:
    void UpdateInfo();

    Bus apu_bus;
    Layer stack;
    Layer layer;
    Mixer mixer;
    Fader fader;

    NES_CPU cpu;
    NES_MEM mem;
    NES_BANK bank;
    NSF2_Vectors nsf2_vectors;
    NSF2_IRQ nsf2_irq;

    ISoundChip *sc[NES_DEVICE_MAX];      // �T�E���h�`�b�v�̃C���X�^���X
    Amplifier amp[NES_DEVICE_MAX];       // �A���v
    RateConverter rconv;
    DCFilter dcf;                        // �ŏI�o�͒i�Ɋ|���钼���t�B���^
    Filter lpf;                          // �ŏI�o�͂Ɋ|���郍�[�p�X�t�B���^
    ILoopDetector *ld;                   // ���[�v���o��
    CPULogger *logcpu;                   // Logs CPU to file

    // �g���b�N�ԍ��̗�
    enum {
      APU1_TRK0=0, APU1_TRK1, 
      APU2_TRK0, APU2_TRK1, APU2_TRK2,
      FDS_TRK0,
      MMC5_TRK0, MMC5_TRK1, MMC5_TRK2,
      FME7_TRK0, FME7_TRK1, FME7_TRK2, FME7_TRK3, FME7_TRK4,
      VRC6_TRK0, VRC6_TRK1, VRC6_TRK2,
      VRC7_TRK0, VRC7_TRK1, VRC7_TRK2, VRC7_TRK3, VRC7_TRK4, VRC7_TRK5,
      N106_TRK0, N106_TRK1, N106_TRK2, N106_TRK3, N106_TRK4, N106_TRK5, N106_TRK6, N106_TRK7,
      VRC7_TRK6, VRC7_TRK7, VRC7_TRK8,
      NES_TRACK_MAX
    };
    InfoBuffer infobuf[NES_TRACK_MAX];   // �e�g���b�N�̏���ۑ�
    
    int total_render; // ����܂łɐ��������g�`�̃o�C�g��
    int frame_render; // �P�t���[�����̃o�C�g��
    int frame_in_ms;  // �P�t���[���̒���(ms)

    // �e�T�E���h�`�b�v�̃G�C���A�X�Q��
    NES_APU *apu;
    NES_DMC *dmc;
    NES_VRC6 *vrc6;
    NES_VRC7 *vrc7;
    NES_FME7 *fme7;
    NES_MMC5 *mmc5;
    NES_N106 *n106;
    NES_FDS *fds;

  public:
    NSF *nsf;
    NSFPlayer ();
    ~NSFPlayer ();

    /** �R���t�B�O���̃Z�b�g */
    virtual void SetConfig(PlayerConfig *pc) ;

    /** �f�[�^�����[�h���� */
    virtual bool Load (SoundData * sdat);

    /** �Đ����g����ݒ肷�� */
    virtual void SetPlayFreq (double);

    /**
     * Number of channels to output.
     */
    virtual void SetChannels(int);

    /** ���Z�b�g����D�O�̉��t�Ńf�[�^�̎��ȏ����������������Ă��Ă��C�����Ȃ��D */
    virtual void Reset ();

    /** ���݉��t���̋Ȕԍ���Ԃ� */
    virtual int GetSong ();

    /** �t�F�[�h�A�E�g���J�n���� */
    virtual void FadeOut (int fade_in_ms);

    /** ���t����Ȕԍ���ݒ肷�� */
    virtual bool SetSong (int s);
    virtual bool PrevSong (int s);
    virtual bool NextSong (int s);

    /** �����_�����O���s�� */
    virtual UINT32 Render (INT16 * b, UINT32 length);

    /** �����_�����O���X�L�b�v���� */
    virtual UINT32 Skip (UINT32 length);

    /** �Ȗ����擾���� */
    virtual const char *GetTitleString ();

    /** ���t���Ԃ��擾���� */
    virtual int GetLength ();

    /** ���t���Ԃ��������o���ꂽ���ǂ������`�F�b�N���� */
    virtual bool IsDetected ();

    /** ���t����~�������ǂ������`�F�b�N���� */
    virtual bool IsStopped ();

    /** ���݂�NES�������󋵂𕶎���Ƃ��Ċl������ */
    virtual void GetMemoryString (char *buf);   // Memory Dump as String

    /** ���݂�NES�������󋵂��l������ */
    virtual void GetMemory (UINT8 * buf);       // Memory Dump

    /** �R���t�B�O���[�V�����̍X�V���ʒm���󂯎��R�[���o�b�N */
    virtual void Notify (int id);

    // Notify for panning
    virtual void NotifyPan (int id);

    /** time_in_ms���_�ł̃f�o�C�X�����擾���� */
    virtual IDeviceInfo *GetInfo(int time_in_ms, int device_id);

    /** Whether to use PAL/NTSC/Dendy based on flags and REGION config */
    virtual int GetRegion(UINT8 flags, int pref);

    virtual bool GetInfinite();
    virtual void SetInfinite(bool inf);
    enum {
        REGION_NTSC = 0,
        REGION_PAL,
        REGION_DENDY
    };
  };

}// namespace

#endif
