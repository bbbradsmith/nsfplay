#ifndef KMP_PI_H
#define KMP_PI_H
/*

     KbMedia Player Plugin �w�b�_�t�@�C���i�e�X�g�Łj

*/

#define KMPMODULE_VERSION 100 /* KMPMODULE �̃o�[�W���� */
#define KMP_GETMODULE     kmp_GetTestModule  /* �܂��e�X�g��... */
#define SZ_KMP_GETMODULE "kmp_GetTestModule" /* �܂��e�X�g��... */

typedef void* HKMP; /* 'K'b'M'edia 'P'layer Plugin �� Handle */

typedef struct
{/* �I�[�v�������T�E���h�f�[�^�̏�� */
    DWORD dwSamplesPerSec;/* �T���v�����O���g��(44100, 22050 �Ȃ�) */
    DWORD dwChannels;     /* �`�����l����( mono = 1, stereo = 2) */
    DWORD dwBitsPerSample;/* �ʎq���r�b�g��( 8 or 16) */
    DWORD dwLength;       /* �Ȃ̒����iSPC �̂悤�Ɍv�Z�s�\�ȏꍇ�� 0xFFFFFFFF�j */
    DWORD dwSeekable;     /* �V�[�N���T�|�[�g���Ă���ꍇ�� 1�A���Ȃ��ꍇ�� 0 */
                          /* �i��G�c�ɃV�[�N�ł��Ă��A�V�[�N��̍Đ��ʒu�� */
                          /*  �s���m�ȏꍇ�� 0 �Ƃ���j */
    DWORD dwUnitRender;   /* Render �֐��̑�R�����͂��̒l�̔{�����n�����i�ǂ�Ȓl�ł��ǂ��ꍇ�� 0�j */
    DWORD dwReserved1;    /* ��� 0 */
    DWORD dwReserved2;    /* ��� 0 */
}SOUNDINFO;


typedef struct
{
    DWORD dwVersion;
    /* ���W���[���̃o�[�W�����BDLL �̃o�[�W�����ł͂Ȃ��B�K�� KMPMODULE_VERSION(=100) �ɂ��邱�ƁB */
    /* ���̒l�� KbMedia Player �����҂���l�ƈ�v���Ȃ��ꍇ�́AKbMedia Player */
    /* �ɂ���Ē����� FreeLibrary ���Ă΂��B */
    /* ���̏ꍇ�AInitDll() �� DeinitDll() ���Ă΂�Ȃ����Ƃɒ��ӁB */
    DWORD dwPluginVersion;
    /* �v���O�C���̃o�[�W���� */
    /* �����t�@�C�����̃v���O�C�������������ꍇ�́A�������傫�����̂�D��I�Ɏg�� */

    const char  *pszCopyright;
    /* ���쌠 */
    /* �o�[�W�������ł��̕����̕������\������\�� */
    /* NULL �ɂ��Ă��悢 */


    const char  *pszDescription;
    /* ���� */
    /* �o�[�W�������ł��̕����̕������\������\�� */
    /* NULL �ɂ��Ă��悢 */


    const char  **ppszSupportExts;
    /* �T�|�[�g����g���q�̕�����̔z��(�s���I�h�܂�) */
    /* NULL �ŏI���悤�ɂ��� */
    /* ��FppszSupportExts = {".mp1", ".mp2", ".mp3", "rmp", NULL}; */
    /* �g���q���P���Ȃ��ꍇ�́AKbMedia Player �ɂ���Ē����� FreeLibrary ���� */
    /* �΂��B���̏ꍇ�� InitDll() �� DeinitDll() ���Ă΂�Ȃ����Ƃɒ��ӁB */


    DWORD dwReentrant;
    /* �v���O�C�������������Đ����\�Ȏd�l�ȏꍇ�� 1, �s�\�ȏꍇ�� 0 */
    /* �����̊g���q���T�|�[�g���Ă��āA����̊g���q�͕��������Đ����\�ł����� */
    /* ���A�P�ł����������Đ����s�\�Ȃ��̂�����ꍇ�� 0 �ɂ��邱�ƁB */
    /* ���̒l�� 0 �ł����Ă��AOpen �֐��Ŗ߂����n���h�����L���ȊԂ� Open ���Ă� */
    /* �o���Ă��A���݉��t���̌��ʂɉe�����y�ڂ��Ȃ��悤�ɂ��邱�ƁB */
    /* �����I�ɃN���X�t�F�[�h�ɑΉ������Ƃ��ɂ��̒l���Q�Ƃ���\�� */


    void  (WINAPI *Init)(void);
    /* DLL�������BOpen �����Ăяo���O�� KbMedia Player �ɂ���Ĉ�x�����Ă΂��B */
    /* �K�v�Ȃ��ꍇ�� NULL �ɂ��Ă��ǂ��B */
    /* �o���邾�� DllMain �� PROCESS_ATTACH ��肱����ŏ���������i�N���̍������̂��߁j */


    void  (WINAPI *Deinit)(void);
    /* DLL�̌�n���BFreeLibrary �̒��O�Ɉ�x�����Ă΂��B */
    /* �K�v�Ȃ��ꍇ�� NULL �ɂ��Ă��ǂ��B */
    /* InitDll() ����x���Ă΂��� DeinitDll() ���Ăԉ\�������邱�Ƃɒ��� */


    HKMP (WINAPI *Open)(const char *cszFileName, SOUNDINFO *pInfo);
    /* �t�@�C�����J���B�K���������邱�ƁB */
    /* �G���[�̏ꍇ�� NULL ��Ԃ��B */
    /* �G���[�łȂ��ꍇ�� pInfo �ɓK�؂ȏ������邱�ƁB�K�؂ȏ�񂪓����� */
    /* ���Ȃ��ꍇ�idwBits �� 0 �Ȃǁj�� KbMedia Player �ɂ���Ē����� Close */
    /* ���Ă΂��B */
    /* pInfo �� NULL �̏ꍇ�͉������Ȃ��� NULL ��Ԃ����� */


    HKMP (WINAPI *OpenFromBuffer)(const BYTE *Buffer, DWORD dwSize, SOUNDINFO *pInfo);
    /* ����������J���B�T�|�[�g���Ȃ��ꍇ�� NULL �ɂ��邱�ƁB */
    /* �T�|�[�g���Ȃ�����Ƃ����āA��� NULL ��Ԃ��悤�Ɏ������Ă͂Ȃ�Ȃ��B */
    /* �����̊g���q�ɑΉ����Ă��Ă��A�P�ł��T�|�[�g���Ȃ��g���q������ꍇ�� */
    /* NULL �ɂ��Ȃ���΂Ȃ�Ȃ��B */
    /* pInfo �� NULL �̏ꍇ�͉������Ȃ��� NULL ��Ԃ����� */
    /* ���̂Ƃ���AKbMedia Player �{�̂�����ɑΉ����Ă܂���B(^_^; */

    void   (WINAPI *Close)(HKMP hKMP);
    /* �n���h�������B�K���������邱�ƁB */
    /* hKMP �� NULL ��n���Ă����v�Ȃ悤�ɂ��邱�ƁB */


    DWORD  (WINAPI *Render)(HKMP hKMP, BYTE* Buffer, DWORD dwSize);
    /* Buffer �� PCM ������B�K���������邱�ƁB */
    /* dwSize �̓o�b�t�@�̃T�C�Y�̃o�C�g���B�i�T���v�����ł͂Ȃ��j */
    /* �߂�l�� Buffer �ɏ������񂾃o�C�g���B�i�T���v�����ł͂Ȃ��j */
    /* dwSize ��菬�����l��Ԃ����牉�t�I���B */
    /* dwSize �� SOUNDINFO::dwUnitRender �̔{�����n�����B */
    /* �idwUnitRender �̒l���̂��̂ł͂Ȃ��A�{���̉\�������邱�Ƃɒ��Ӂj */
    /* Buffer �� NULL �� dwSize �� 0 �łȂ��ꍇ�́AdwSize ���������t�ʒu */
    /* ��i�߂邱�ƁB�����I�����Ă͂����Ȃ��B */
    /* �V�[�N���T�|�[�g���Ȃ��ꍇ�ɁA�_�~�[�� Render ���J��Ԃ����s���� */
    /* �����ɃV�[�N���鏈�������������邽�߁B�Ƃ����Ă��A�܂��{�̂̕��� */
    /* Buffer �� NULL ��n�����Ƃ͂Ȃ����ǁB(^^; */
    /*  */
    /* hKMP �� NULL ��n�����ꍇ�͉������Ȃ��� 0 ��Ԃ��悤�ɂ��邱�ƁB */


    DWORD  (WINAPI *SetPosition)(HKMP hKMP, DWORD dwPos);
    /* �V�[�N�B�K���������邱�ƁB */
    /* dwPos �̓V�[�N��̍Đ��ʒu�B�߂�l�̓V�[�N��̍Đ��ʒu�B�P�ʂ̓~���b�B */
    /* dwPos �Ɩ߂�l�͊��S�Ɉ�v����K�v�͂Ȃ��B�߂�l�Ɩ{���̍Đ��ʒu�� */
    /* �덷���傫���Ȃ�i�̎��Ƃ̓����Đ����Ɏx����������j�ꍇ�� Open ���� */
    /* SOUNDINFO �� dwSeekable �� 0 �ɂ��Ă������ƁB�덷���Ȃ����A�����Ă� */
    /* ���ɏ������ꍇ�� dwSeekable �� 1 �ɂ��Ă������ƁB�߂�l�����m�Ȃ� */
    /* �΁AdwPos �Ɩ߂�l�̍����傫���Ă� dwSeekable=1 �Ƃ��ėǂ��B */
    /* ���̂Ƃ���AKbMeida Player �{�̂� dwSeekable �̒l�����Ă܂���B(^_^; */
    /* dwSeekable �̒l�ɂ���āA�̎��f�[�^�Ɠ����Đ�����Ƃ��Ƃ��Ȃ��Ƃ��� */
    /* �V�[�N������ς���\��B */
    /* �V�[�N��S���T�|�[�g���Ȃ��ꍇ�́A�擪�ʒu�ɖ߂��� 0 ��Ԃ����ƁB */
    /* hKMP �� NULL ��n�����ꍇ�� 0 ��Ԃ��悤�ɂ��邱�ƁB */


}KMPMODULE;

typedef KMPMODULE* (WINAPI *pfnGetKMPModule)(void);
/*
    �����d�l���v�����Ȃ���[�B
    �^�C�g���ǂݍ��݂Ƃ����擾�n���ǂ����悤�B
    DLL �ŗL�� API �͂ǂ����悤�B

    ���t�����o���Ȃ���iMCI �݂����Ȃ�j�͂ǂ����悤�B
    ������ǂ�����΂����̂��c�B
    MIDI ���c�B

    SPC �݂����ȓ���Ȃ̂��ǂ�����΂����̂��c�B
    (Script700 �Ƃ����t�I�����Ȃ��Ƃ��j

    �t�@�C���P�ɕ����̋Ȃ������Ă�������݂��������c�B(NSF�Ƃ�)

    COM�C���^�[�t�F�[�X���ɂ������������Ƃ����l�������������c�B
    
    �{�̑��̖�肾���ǁA�g���q������������ǂ������΂����̂��c�B
*/
#endif
