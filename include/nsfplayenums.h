#pragma once
// generated by nsfplayenums.py
// 2024-04-26 15:51:26

const int32_t NSF_LIST_COUNT =                                     12;
const int32_t NSF_LIST_ENABLE =                                     0;
const int32_t NSF_LIST_ENABLE_AUTO =                                1;
const int32_t NSF_LIST_SAMPLERATES =                                2;
const int32_t NSF_LIST_BITS =                                       3;
const int32_t NSF_LIST_WAVEOUT_BITS =                               4;
const int32_t NSF_LIST_DOWNSAMPLE =                                 5;
const int32_t NSF_LIST_REGION =                                     6;
const int32_t NSF_LIST_REGIONLIST =                                 7;
const int32_t NSF_LIST_OPLL =                                       8;
const int32_t NSF_LIST_ENCODING =                                   9;
const int32_t NSF_LIST_FILE_TYPE =                                 10;
const int32_t NSF_LIST_LOCALE =                                    11;

const int32_t NSF_LK_ENABLE_COUNT =                                 2;
const int32_t NSF_LK_ENABLE_OFF =                                   0;
const int32_t NSF_LK_ENABLE_ON =                                    1;

const int32_t NSF_LK_ENABLE_AUTO_COUNT =                            3;
const int32_t NSF_LK_ENABLE_AUTO_OFF =                              0;
const int32_t NSF_LK_ENABLE_AUTO_ON =                               1;
const int32_t NSF_LK_ENABLE_AUTO_AUTO =                             2;

const int32_t NSF_LK_SAMPLERATES_COUNT =                            8;
const int32_t NSF_LK_SAMPLERATES_SR_8000 =                          0;
const int32_t NSF_LK_SAMPLERATES_SR_11025 =                         1;
const int32_t NSF_LK_SAMPLERATES_SR_22050 =                         2;
const int32_t NSF_LK_SAMPLERATES_SR_44100 =                         3;
const int32_t NSF_LK_SAMPLERATES_SR_48000 =                         4;
const int32_t NSF_LK_SAMPLERATES_SR_96000 =                         5;
const int32_t NSF_LK_SAMPLERATES_SR_192000 =                        6;
const int32_t NSF_LK_SAMPLERATES_SR_CPU =                           7;

const int32_t NSF_LK_BITS_COUNT =                                   3;
const int32_t NSF_LK_BITS_BIT_8 =                                   0;
const int32_t NSF_LK_BITS_BIT_16 =                                  1;
const int32_t NSF_LK_BITS_BIT_32 =                                  2;

const int32_t NSF_LK_WAVEOUT_BITS_COUNT =                           4;
const int32_t NSF_LK_WAVEOUT_BITS_BIT_8 =                           0;
const int32_t NSF_LK_WAVEOUT_BITS_BIT_16 =                          1;
const int32_t NSF_LK_WAVEOUT_BITS_BIT_32 =                          2;
const int32_t NSF_LK_WAVEOUT_BITS_AUTO =                            3;

const int32_t NSF_LK_DOWNSAMPLE_COUNT =                             3;
const int32_t NSF_LK_DOWNSAMPLE_NONE =                              0;
const int32_t NSF_LK_DOWNSAMPLE_AVERAGE =                           1;
const int32_t NSF_LK_DOWNSAMPLE_SINC =                              2;

const int32_t NSF_LK_REGION_COUNT =                                 7;
const int32_t NSF_LK_REGION_AUTO =                                  0;
const int32_t NSF_LK_REGION_NTSC =                                  1;
const int32_t NSF_LK_REGION_PAL =                                   2;
const int32_t NSF_LK_REGION_DENDY =                                 3;
const int32_t NSF_LK_REGION_PREFER_NTSC =                           4;
const int32_t NSF_LK_REGION_PREFER_PAL =                            5;
const int32_t NSF_LK_REGION_PREFER_DENDY =                          6;

const int32_t NSF_LK_REGIONLIST_COUNT =                             3;
const int32_t NSF_LK_REGIONLIST_NTSC =                              0;
const int32_t NSF_LK_REGIONLIST_PAL =                               1;
const int32_t NSF_LK_REGIONLIST_DENDY =                             2;

const int32_t NSF_LK_OPLL_COUNT =                                  10;
const int32_t NSF_LK_OPLL_VRC7 =                                    0;
const int32_t NSF_LK_OPLL_YM2413 =                                  1;
const int32_t NSF_LK_OPLL_YMF281B =                                 2;
const int32_t NSF_LK_OPLL_VRC7_RW =                                 3;
const int32_t NSF_LK_OPLL_VRC7_FT36 =                               4;
const int32_t NSF_LK_OPLL_VRC7_FT35 =                               5;
const int32_t NSF_LK_OPLL_VRC7_MO =                                 6;
const int32_t NSF_LK_OPLL_VRC7_KT2 =                                7;
const int32_t NSF_LK_OPLL_VRC7_KT1 =                                8;
const int32_t NSF_LK_OPLL_YMF281B_CH =                              9;

const int32_t NSF_LK_ENCODING_COUNT =                               3;
const int32_t NSF_LK_ENCODING_UTF8 =                                0;
const int32_t NSF_LK_ENCODING_SJIS =                                1;
const int32_t NSF_LK_ENCODING_AUTO =                                2;

const int32_t NSF_LK_FILE_TYPE_COUNT =                              6;
const int32_t NSF_LK_FILE_TYPE_NONE =                               0;
const int32_t NSF_LK_FILE_TYPE_NSF =                                1;
const int32_t NSF_LK_FILE_TYPE_NSF2 =                               2;
const int32_t NSF_LK_FILE_TYPE_NSFE =                               3;
const int32_t NSF_LK_FILE_TYPE_BIN =                                4;
const int32_t NSF_LK_FILE_TYPE_INVALID =                            5;

const int32_t NSF_LK_LOCALE_COUNT =                                 1;
const int32_t NSF_LK_LOCALE_ENGLISH =                               0;

const int32_t NSF_UNIT_COUNT =                                      8;
const int32_t NSF_UNIT_APU1 =                                       0;
const int32_t NSF_UNIT_APU2 =                                       1;
const int32_t NSF_UNIT_FDS =                                        2;
const int32_t NSF_UNIT_MMC5 =                                       3;
const int32_t NSF_UNIT_VRC6 =                                       4;
const int32_t NSF_UNIT_VRC7 =                                       5;
const int32_t NSF_UNIT_N163 =                                       6;
const int32_t NSF_UNIT_S5B =                                        7;

const int32_t NSF_GROUP_COUNT =                                    20;
const int32_t NSF_GROUP_MAIN =                                      0;
const int32_t NSF_GROUP_PLAY =                                      1;
const int32_t NSF_GROUP_AUDIO =                                     2;
const int32_t NSF_GROUP_EMU =                                       3;
const int32_t NSF_GROUP_APU1 =                                      4;
const int32_t NSF_GROUP_APU2 =                                      5;
const int32_t NSF_GROUP_FDS =                                       6;
const int32_t NSF_GROUP_MMC5 =                                      7;
const int32_t NSF_GROUP_VRC6 =                                      8;
const int32_t NSF_GROUP_VRC7 =                                      9;
const int32_t NSF_GROUP_N163 =                                     10;
const int32_t NSF_GROUP_S5B =                                      11;
const int32_t NSF_GROUP_WAVEOUT =                                  12;
const int32_t NSF_GROUP_ADVANCED =                                 13;
const int32_t NSF_GROUP_TEST =                                     14;
const int32_t NSF_GROUP_NSF =                                      15;
const int32_t NSF_GROUP_DATA =                                     16;
const int32_t NSF_GROUP_ACTIVE =                                   17;
const int32_t NSF_GROUP_PTEST =                                    18;
const int32_t NSF_GROUP_SONG =                                     19;

const int32_t NSF_CHANNEL_COUNT =                                  31;
const int32_t NSF_CHANNEL_APU1_SQU0 =                               0;
const int32_t NSF_CHANNEL_APU1_SQU1 =                               1;
const int32_t NSF_CHANNEL_APU2_TRI =                                2;
const int32_t NSF_CHANNEL_APU2_NSE =                                3;
const int32_t NSF_CHANNEL_APU2_DPCM =                               4;
const int32_t NSF_CHANNEL_FDS_FDS =                                 5;
const int32_t NSF_CHANNEL_MMC5_MMC5_0 =                             6;
const int32_t NSF_CHANNEL_MMC5_MMC5_1 =                             7;
const int32_t NSF_CHANNEL_MMC5_MMC5_PCM =                           8;
const int32_t NSF_CHANNEL_VRC6_VRC6_0 =                             9;
const int32_t NSF_CHANNEL_VRC6_VRC6_1 =                            10;
const int32_t NSF_CHANNEL_VRC6_VRC6_SAW =                          11;
const int32_t NSF_CHANNEL_VRC7_VRC7_0 =                            12;
const int32_t NSF_CHANNEL_VRC7_VRC7_1 =                            13;
const int32_t NSF_CHANNEL_VRC7_VRC7_2 =                            14;
const int32_t NSF_CHANNEL_VRC7_VRC7_3 =                            15;
const int32_t NSF_CHANNEL_VRC7_VRC7_4 =                            16;
const int32_t NSF_CHANNEL_VRC7_VRC7_5 =                            17;
const int32_t NSF_CHANNEL_N163_N163_0 =                            18;
const int32_t NSF_CHANNEL_N163_N163_1 =                            19;
const int32_t NSF_CHANNEL_N163_N163_2 =                            20;
const int32_t NSF_CHANNEL_N163_N163_3 =                            21;
const int32_t NSF_CHANNEL_N163_N163_4 =                            22;
const int32_t NSF_CHANNEL_N163_N163_5 =                            23;
const int32_t NSF_CHANNEL_N163_N163_6 =                            24;
const int32_t NSF_CHANNEL_N163_N163_7 =                            25;
const int32_t NSF_CHANNEL_S5B_S5B_0 =                              26;
const int32_t NSF_CHANNEL_S5B_S5B_1 =                              27;
const int32_t NSF_CHANNEL_S5B_S5B_2 =                              28;
const int32_t NSF_CHANNEL_S5B_S5B_N =                              29;
const int32_t NSF_CHANNEL_S5B_S5B_E =                              30;

const int32_t NSF_SET_COUNT =                                     200;
const int32_t NSF_SET_VOLUME =                                      0;
const int32_t NSF_SET_SAMPLERATE =                                  1; // List: SAMPLERATES
const int32_t NSF_SET_STEREO =                                      2; // List: ENABLE
const int32_t NSF_SET_BITS =                                        3; // List: BITS
const int32_t NSF_SET_TITLE_FORMAT =                                4; // String
const int32_t NSF_SET_LOCALE =                                      5; // List: LOCALE
const int32_t NSF_SET_CHANNEL_RESET =                               6; // List: ENABLE
const int32_t NSF_SET_PLAY_START =                                  7; // List: ENABLE
const int32_t NSF_SET_PLAY_NEXT =                                   8; // List: ENABLE
const int32_t NSF_SET_ALBUM_LOOP =                                  9; // List: ENABLE
const int32_t NSF_SET_PLAYLIST =                                   10; // List: ENABLE
const int32_t NSF_SET_PLAY_TIME =                                  11;
const int32_t NSF_SET_FADE_TIME =                                  12;
const int32_t NSF_SET_SILENT_TIMEOUT =                             13;
const int32_t NSF_SET_DETECT_LOOP =                                14; // List: ENABLE
const int32_t NSF_SET_LOOP_NUM =                                   15;
const int32_t NSF_SET_LPF =                                        16;
const int32_t NSF_SET_HPF =                                        17;
const int32_t NSF_SET_DOWNSAMPLE =                                 18; // List: DOWNSAMPLE
const int32_t NSF_SET_DOWNSAMPLE_INT =                             19; // List: ENABLE
const int32_t NSF_SET_REGION =                                     20; // List: REGION
const int32_t NSF_SET_CPU_NTSC =                                   21;
const int32_t NSF_SET_CPU_PAL =                                    22;
const int32_t NSF_SET_CPU_DENDY =                                  23;
const int32_t NSF_SET_FRAME_SPEED =                                24;
const int32_t NSF_SET_FRAME_IGNORE =                               25; // List: ENABLE
const int32_t NSF_SET_FRAME_NTSC =                                 26;
const int32_t NSF_SET_FRAME_PAL =                                  27;
const int32_t NSF_SET_FRAME_DENDY =                                28;
const int32_t NSF_SET_IRQ_ENABLE =                                 29; // List: ENABLE
const int32_t NSF_SET_APU1_NONLINEAR =                             30; // List: ENABLE
const int32_t NSF_SET_APU1_UNMUTE =                                31; // List: ENABLE
const int32_t NSF_SET_APU1_SWEEP_UNMUTE =                          32; // List: ENABLE
const int32_t NSF_SET_APU1_PHASE_RESET =                           33; // List: ENABLE
const int32_t NSF_SET_APU1_SWAP_DUTY =                             34; // List: ENABLE
const int32_t NSF_SET_SQU0_ON =                                    35; // List: ENABLE
const int32_t NSF_SET_SQU0_VOL =                                   36;
const int32_t NSF_SET_SQU0_PAN =                                   37;
const int32_t NSF_SET_SQU0_COL =                                   38;
const int32_t NSF_SET_SQU1_ON =                                    39; // List: ENABLE
const int32_t NSF_SET_SQU1_VOL =                                   40;
const int32_t NSF_SET_SQU1_PAN =                                   41;
const int32_t NSF_SET_SQU1_COL =                                   42;
const int32_t NSF_SET_APU2_NONLINEAR =                             43; // List: ENABLE
const int32_t NSF_SET_APU2_UNMUTE =                                44; // List: ENABLE
const int32_t NSF_SET_TRI_MUTE0 =                                  45; // List: ENABLE
const int32_t NSF_SET_TRI_RANDOM =                                 46; // List: ENABLE
const int32_t NSF_SET_NSE_PERIODIC =                               47; // List: ENABLE
const int32_t NSF_SET_NSE_RANDOM =                                 48; // List: ENABLE
const int32_t NSF_SET_DPCM_4011 =                                  49; // List: ENABLE
const int32_t NSF_SET_DPCM_REVERSE =                               50; // List: ENABLE
const int32_t NSF_SET_TRI_ON =                                     51; // List: ENABLE
const int32_t NSF_SET_TRI_VOL =                                    52;
const int32_t NSF_SET_TRI_PAN =                                    53;
const int32_t NSF_SET_TRI_COL =                                    54;
const int32_t NSF_SET_NSE_ON =                                     55; // List: ENABLE
const int32_t NSF_SET_NSE_VOL =                                    56;
const int32_t NSF_SET_NSE_PAN =                                    57;
const int32_t NSF_SET_NSE_COL =                                    58;
const int32_t NSF_SET_DPCM_ON =                                    59; // List: ENABLE
const int32_t NSF_SET_DPCM_VOL =                                   60;
const int32_t NSF_SET_DPCM_PAN =                                   61;
const int32_t NSF_SET_DPCM_COL =                                   62;
const int32_t NSF_SET_FDS_LOWPASS =                                63;
const int32_t NSF_SET_FDS_MOD_RESET =                              64; // List: ENABLE
const int32_t NSF_SET_FDS_WRITE_PROTECT =                          65; // List: ENABLE
const int32_t NSF_SET_FDS_ON =                                     66; // List: ENABLE
const int32_t NSF_SET_FDS_VOL =                                    67;
const int32_t NSF_SET_FDS_PAN =                                    68;
const int32_t NSF_SET_FDS_COL =                                    69;
const int32_t NSF_SET_MMC5_NONLINEAR =                             70; // List: ENABLE
const int32_t NSF_SET_MMC5_PHASE_RESET =                           71; // List: ENABLE
const int32_t NSF_SET_MMC5_0_ON =                                  72; // List: ENABLE
const int32_t NSF_SET_MMC5_0_VOL =                                 73;
const int32_t NSF_SET_MMC5_0_PAN =                                 74;
const int32_t NSF_SET_MMC5_0_COL =                                 75;
const int32_t NSF_SET_MMC5_1_ON =                                  76; // List: ENABLE
const int32_t NSF_SET_MMC5_1_VOL =                                 77;
const int32_t NSF_SET_MMC5_1_PAN =                                 78;
const int32_t NSF_SET_MMC5_1_COL =                                 79;
const int32_t NSF_SET_MMC5_PCM_ON =                                80; // List: ENABLE
const int32_t NSF_SET_MMC5_PCM_VOL =                               81;
const int32_t NSF_SET_MMC5_PCM_PAN =                               82;
const int32_t NSF_SET_MMC5_PCM_COL =                               83;
const int32_t NSF_SET_VRC6_0_ON =                                  84; // List: ENABLE
const int32_t NSF_SET_VRC6_0_VOL =                                 85;
const int32_t NSF_SET_VRC6_0_PAN =                                 86;
const int32_t NSF_SET_VRC6_0_COL =                                 87;
const int32_t NSF_SET_VRC6_1_ON =                                  88; // List: ENABLE
const int32_t NSF_SET_VRC6_1_VOL =                                 89;
const int32_t NSF_SET_VRC6_1_PAN =                                 90;
const int32_t NSF_SET_VRC6_1_COL =                                 91;
const int32_t NSF_SET_VRC6_SAW_ON =                                92; // List: ENABLE
const int32_t NSF_SET_VRC6_SAW_VOL =                               93;
const int32_t NSF_SET_VRC6_SAW_PAN =                               94;
const int32_t NSF_SET_VRC6_SAW_COL =                               95;
const int32_t NSF_SET_VRC7_OPLL =                                  96; // List: ENABLE
const int32_t NSF_SET_VRC7_PATCH =                                 97; // List: OPLL
const int32_t NSF_SET_OPLL_PATCH =                                 98; // List: OPLL
const int32_t NSF_SET_VRC7_0_ON =                                  99; // List: ENABLE
const int32_t NSF_SET_VRC7_0_VOL =                                100;
const int32_t NSF_SET_VRC7_0_PAN =                                101;
const int32_t NSF_SET_VRC7_0_COL =                                102;
const int32_t NSF_SET_VRC7_1_ON =                                 103; // List: ENABLE
const int32_t NSF_SET_VRC7_1_VOL =                                104;
const int32_t NSF_SET_VRC7_1_PAN =                                105;
const int32_t NSF_SET_VRC7_1_COL =                                106;
const int32_t NSF_SET_VRC7_2_ON =                                 107; // List: ENABLE
const int32_t NSF_SET_VRC7_2_VOL =                                108;
const int32_t NSF_SET_VRC7_2_PAN =                                109;
const int32_t NSF_SET_VRC7_2_COL =                                110;
const int32_t NSF_SET_VRC7_3_ON =                                 111; // List: ENABLE
const int32_t NSF_SET_VRC7_3_VOL =                                112;
const int32_t NSF_SET_VRC7_3_PAN =                                113;
const int32_t NSF_SET_VRC7_3_COL =                                114;
const int32_t NSF_SET_VRC7_4_ON =                                 115; // List: ENABLE
const int32_t NSF_SET_VRC7_4_VOL =                                116;
const int32_t NSF_SET_VRC7_4_PAN =                                117;
const int32_t NSF_SET_VRC7_4_COL =                                118;
const int32_t NSF_SET_VRC7_5_ON =                                 119; // List: ENABLE
const int32_t NSF_SET_VRC7_5_VOL =                                120;
const int32_t NSF_SET_VRC7_5_PAN =                                121;
const int32_t NSF_SET_VRC7_5_COL =                                122;
const int32_t NSF_SET_N163_MULTIPLEX =                            123; // List: ENABLE
const int32_t NSF_SET_N163_PHASE_PROTECT =                        124; // List: ENABLE
const int32_t NSF_SET_N163_SHORT_WAVE =                           125; // List: ENABLE
const int32_t NSF_SET_N163_0_ON =                                 126; // List: ENABLE
const int32_t NSF_SET_N163_0_VOL =                                127;
const int32_t NSF_SET_N163_0_PAN =                                128;
const int32_t NSF_SET_N163_0_COL =                                129;
const int32_t NSF_SET_N163_1_ON =                                 130; // List: ENABLE
const int32_t NSF_SET_N163_1_VOL =                                131;
const int32_t NSF_SET_N163_1_PAN =                                132;
const int32_t NSF_SET_N163_1_COL =                                133;
const int32_t NSF_SET_N163_2_ON =                                 134; // List: ENABLE
const int32_t NSF_SET_N163_2_VOL =                                135;
const int32_t NSF_SET_N163_2_PAN =                                136;
const int32_t NSF_SET_N163_2_COL =                                137;
const int32_t NSF_SET_N163_3_ON =                                 138; // List: ENABLE
const int32_t NSF_SET_N163_3_VOL =                                139;
const int32_t NSF_SET_N163_3_PAN =                                140;
const int32_t NSF_SET_N163_3_COL =                                141;
const int32_t NSF_SET_N163_4_ON =                                 142; // List: ENABLE
const int32_t NSF_SET_N163_4_VOL =                                143;
const int32_t NSF_SET_N163_4_PAN =                                144;
const int32_t NSF_SET_N163_4_COL =                                145;
const int32_t NSF_SET_N163_5_ON =                                 146; // List: ENABLE
const int32_t NSF_SET_N163_5_VOL =                                147;
const int32_t NSF_SET_N163_5_PAN =                                148;
const int32_t NSF_SET_N163_5_COL =                                149;
const int32_t NSF_SET_N163_6_ON =                                 150; // List: ENABLE
const int32_t NSF_SET_N163_6_VOL =                                151;
const int32_t NSF_SET_N163_6_PAN =                                152;
const int32_t NSF_SET_N163_6_COL =                                153;
const int32_t NSF_SET_N163_7_ON =                                 154; // List: ENABLE
const int32_t NSF_SET_N163_7_VOL =                                155;
const int32_t NSF_SET_N163_7_PAN =                                156;
const int32_t NSF_SET_N163_7_COL =                                157;
const int32_t NSF_SET_S5B_MIXER =                                 158; // List: ENABLE
const int32_t NSF_SET_S5B_0_ON =                                  159; // List: ENABLE
const int32_t NSF_SET_S5B_0_VOL =                                 160;
const int32_t NSF_SET_S5B_0_PAN =                                 161;
const int32_t NSF_SET_S5B_0_COL =                                 162;
const int32_t NSF_SET_S5B_1_ON =                                  163; // List: ENABLE
const int32_t NSF_SET_S5B_1_VOL =                                 164;
const int32_t NSF_SET_S5B_1_PAN =                                 165;
const int32_t NSF_SET_S5B_1_COL =                                 166;
const int32_t NSF_SET_S5B_2_ON =                                  167; // List: ENABLE
const int32_t NSF_SET_S5B_2_VOL =                                 168;
const int32_t NSF_SET_S5B_2_PAN =                                 169;
const int32_t NSF_SET_S5B_2_COL =                                 170;
const int32_t NSF_SET_S5B_N_ON =                                  171; // List: ENABLE
const int32_t NSF_SET_S5B_N_COL =                                 172;
const int32_t NSF_SET_S5B_E_ON =                                  173; // List: ENABLE
const int32_t NSF_SET_S5B_E_COL =                                 174;
const int32_t NSF_SET_WAVEOUT_SAMPLERATE =                        175;
const int32_t NSF_SET_WAVEOUT_BITS =                              176; // List: WAVEOUT_BITS
const int32_t NSF_SET_WAVEOUT_STEREO =                            177; // List: ENABLE_AUTO
const int32_t NSF_SET_SAMPLERATE_OVERRIDE =                       178;
const int32_t NSF_SET_SHIFT_JIS =                                 179; // List: ENCODING
const int32_t NSF_SET_EXPANSION_FDS =                             180; // List: ENABLE_AUTO
const int32_t NSF_SET_EXPANSION_MMC5 =                            181; // List: ENABLE_AUTO
const int32_t NSF_SET_EXPANSION_VRC6 =                            182; // List: ENABLE_AUTO
const int32_t NSF_SET_EXPANSION_VRC7 =                            183; // List: ENABLE_AUTO
const int32_t NSF_SET_EXPANSION_N163 =                            184; // List: ENABLE_AUTO
const int32_t NSF_SET_EXPANSION_S5B =                             185; // List: ENABLE_AUTO
const int32_t NSF_SET_EXPANSION_VT02 =                            186; // List: ENABLE_AUTO
const int32_t NSF_SET_OVERRIDE_NSF_SONG =                         187;
const int32_t NSF_SET_TEST_INT =                                  188;
const int32_t NSF_SET_TEST_STR =                                  189; // String
const int32_t NSF_SET_TEST_LIST =                                 190; // List: ENABLE
const int32_t NSF_SET_TEST_HEX8 =                                 191;
const int32_t NSF_SET_TEST_HEX16 =                                192;
const int32_t NSF_SET_TEST_HEX32 =                                193;
const int32_t NSF_SET_TEST_COLOR =                                194;
const int32_t NSF_SET_TEST_MSEC =                                 195;
const int32_t NSF_SET_TEST_MILL =                                 196;
const int32_t NSF_SET_TEST_DMILL =                                197;
const int32_t NSF_SET_TEST_KEY =                                  198;
const int32_t NSF_SET_TEST_PRECISE =                              199;
const int32_t NSF_SETSTR_COUNT =                                    2;

const int32_t NSF_PROP_COUNT =                                     73;
const int32_t NSF_PROP_FILE_TYPE =                                  0;
const int32_t NSF_PROP_NSF_SONG_COUNT =                             1; // NSF header data, use ACTIVE_SONG_COUNT with NSFPlay interface
const int32_t NSF_PROP_NSF_SONG_START =                             2; // NSF header data, use ACTIVE_SONG_START with NSFPlay interface
const int32_t NSF_PROP_NSF_VERSION =                                3;
const int32_t NSF_PROP_LOAD_ADDR =                                  4;
const int32_t NSF_PROP_INIT_ADDR =                                  5;
const int32_t NSF_PROP_PLAY_ADDR =                                  6;
const int32_t NSF_PROP_TITLE =                                      7;
const int32_t NSF_PROP_ARTIST =                                     8;
const int32_t NSF_PROP_COPYRIGHT =                                  9;
const int32_t NSF_PROP_RIPPER =                                    10;
const int32_t NSF_PROP_SPEED_NTSC =                                11;
const int32_t NSF_PROP_SPEED_PAL =                                 12;
const int32_t NSF_PROP_SPEED_DENDY =                               13;
const int32_t NSF_PROP_BANKSWITCH =                                14;
const int32_t NSF_PROP_REGION_NTSC =                               15;
const int32_t NSF_PROP_REGION_PAL =                                16;
const int32_t NSF_PROP_REGION_DENDY =                              17;
const int32_t NSF_PROP_REGION_PREFER =                             18;
const int32_t NSF_PROP_EXPANSION_FDS =                             19;
const int32_t NSF_PROP_EXPANSION_MMC5 =                            20;
const int32_t NSF_PROP_EXPANSION_VRC6 =                            21;
const int32_t NSF_PROP_EXPANSION_VRC7 =                            22;
const int32_t NSF_PROP_EXPANSION_N163 =                            23;
const int32_t NSF_PROP_EXPANSION_S5B =                             24;
const int32_t NSF_PROP_EXPANSION_VT02 =                            25;
const int32_t NSF_PROP_NSF2 =                                      26;
const int32_t NSF_PROP_NSF2_METADATA_OFF =                         27;
const int32_t NSF_PROP_NSF2_IRQ =                                  28;
const int32_t NSF_PROP_NSF2_INIT_NORETURN =                        29;
const int32_t NSF_PROP_NSF2_NOPLAY =                               30;
const int32_t NSF_PROP_NSF2_MANDATORY =                            31;
const int32_t NSF_PROP_NSF_HEADER =                                32;
const int32_t NSF_PROP_NSFE_INFO =                                 33;
const int32_t NSF_PROP_NSFE_BANK =                                 34;
const int32_t NSF_PROP_NSFE_RATE =                                 35;
const int32_t NSF_PROP_NSFE_NSF2 =                                 36;
const int32_t NSF_PROP_NSFE_VRC7 =                                 37;
const int32_t NSF_PROP_NSFE_PLST =                                 38;
const int32_t NSF_PROP_NSFE_PSFX =                                 39;
const int32_t NSF_PROP_NSFE_TIME =                                 40;
const int32_t NSF_PROP_NSFE_FADE =                                 41;
const int32_t NSF_PROP_NSFE_TLBL =                                 42;
const int32_t NSF_PROP_NSFE_TAUT =                                 43;
const int32_t NSF_PROP_NSFE_TEXT =                                 44;
const int32_t NSF_PROP_NSFE_MIXE =                                 45;
const int32_t NSF_PROP_NSFE_REGN =                                 46;
const int32_t NSF_PROP_ACTIVE_SONG =                               47; // Current song for NSFPlay interface (includes PLAYLIST override)
const int32_t NSF_PROP_ACTIVE_SONG_COUNT =                         48; // Song count for NSFPlay interface (includes PLAYLIST override)
const int32_t NSF_PROP_ACTIVE_SONG_START =                         49; // Song start for NSFPlay interface (includes PLAYLIST override)
const int32_t NSF_PROP_ACTIVE_PLAYLIST =                           50; // Indicates whether a playlist is overriding the NSF song list
const int32_t NSF_PROP_ACTIVE_SONG_NSF =                           51; // Current song in the NSF song list, use ACTIVE_SONG with NSFPlay interface
const int32_t NSF_PROP_ACTIVE_CPU_FREQ =                           52; // Current CPU frequency
const int32_t NSF_PROP_ACTIVE_BANKS =                              53; // Current banks: 8 bytes, 10 for FDS expansion
const int32_t NSF_PROP_ACTIVE_EMU_FRAME_CY =                       54; // Current cycle within frame
const int32_t NSF_PROP_TEST_INT =                                  55;
const int32_t NSF_PROP_TEST_LONG =                                 56;
const int32_t NSF_PROP_TEST_STR =                                  57;
const int32_t NSF_PROP_TEST_LINES =                                58;
const int32_t NSF_PROP_TEST_BLOB =                                 59;
const int32_t NSF_PROP_TEST_LIST =                                 60;
const int32_t NSF_PROP_TEST_HEX8 =                                 61;
const int32_t NSF_PROP_TEST_HEX16 =                                62;
const int32_t NSF_PROP_TEST_HEX32 =                                63;
const int32_t NSF_PROP_TEXT_HEX64 =                                64;
const int32_t NSF_PROP_TEST_COLOR =                                65;
const int32_t NSF_PROP_TEST_MSEC =                                 66;
const int32_t NSF_PROP_TEST_MILL =                                 67;
const int32_t NSF_PROP_TEST_HZ =                                   68;
const int32_t NSF_PROP_TEST_KEY =                                  69;
const int32_t NSF_PROP_TEST_PRECISE =                              70;
const int32_t NSF_PROP_TEST_PRDMILL =                              71;
const int32_t NSF_PROP_SONG_TITLE =                                72;

const int32_t NSF_TEXT_MENU_FILE =                                600;
const int32_t NSF_TEXT_MENU_VIEW =                                601;
const int32_t NSF_TEXT_MENU_OPTIONS =                             602;
const int32_t NSF_TEXT_MENU_ABOUT =                               603;
const int32_t NSF_TEXT_MENU_CHANNELS =                            604;
const int32_t NSF_TEXT_MENU_MEMORY =                              605;
const int32_t NSF_TEXT_MENU_LOG =                                 606;
const int32_t NSF_TEXT_MENU_KEYBOARD =                            607;
const int32_t NSF_TEXT_MENU_DEBUGGER =                            608;
const int32_t NSF_TEXT_MENU_OPEN =                                609;
const int32_t NSF_TEXT_MENU_INI_LOAD =                            610;
const int32_t NSF_TEXT_MENU_INI_SAVE =                            611;
const int32_t NSF_ERROR_RAW_ERROR =                               612;
const int32_t NSF_ERROR_SET_INVALID =                             613;
const int32_t NSF_ERROR_SET_TYPE =                                614;
const int32_t NSF_ERROR_SETINT_RANGE =                            615;
const int32_t NSF_ERROR_INI_NOTEXT =                              616;
const int32_t NSF_ERROR_INI_NO_EQUALS =                           617;
const int32_t NSF_ERROR_INI_BAD_KEY =                             618;
const int32_t NSF_ERROR_INI_BAD_INT =                             619;
const int32_t NSF_ERROR_INI_BAD_RANGE =                           620;
const int32_t NSF_ERROR_INI_BAD_LIST_KEY =                        621;
const int32_t NSF_ERROR_INI_BAD_WRITE =                           622;
const int32_t NSF_ERROR_BIN_BAD =                                 623;
const int32_t NSF_ERROR_NSF_HEAD_BAD =                            624;
const int32_t NSF_ERROR_NSF_VERSION_BAD =                         625;
const int32_t NSF_ERROR_NSF2_META_BAD =                           626;
const int32_t NSF_ERROR_NSF_HEAD_TEXT_BAD =                       627;
const int32_t NSF_ERROR_NSFE_CHUNK_BAD =                          628;
const int32_t NSF_ERROR_NSFE_MANDATORY =                          629;
const int32_t NSF_ERROR_NSFE_NO_MANDATORY =                       630;
const int32_t NSF_TEXT_COUNT =                                    631;

const int32_t NSF_LOCALE_COUNT =                                    1;
const int32_t NSF_LOCALE_ENGLISH =                                  0;
// end of file
