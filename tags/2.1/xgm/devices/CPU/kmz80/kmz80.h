/*
  KMZ80 common header
  by Mamiya
*/

#ifndef KMZ80_H_
#define KMZ80_H_

#include "kmtypes.h"
#include "kmevent.h"
#ifdef __cplusplus
extern "C" {
#endif

enum {
	REGID_B,
	REGID_C,
	REGID_D,
	REGID_E,
	REGID_H,
	REGID_L,
	REGID_F,
	REGID_A,
	REGID_IXL,
	REGID_IXH,
	REGID_IYL,
	REGID_IYH,
	REGID_R,
	REGID_R7,
	REGID_I,
	REGID_IFF1,
	REGID_IFF2,
	REGID_IMODE,
	REGID_NMIREQ,
	REGID_INTREQ,
	REGID_HALTED,
	REGID_M1CYCLE,
	REGID_MEMCYCLE,
	REGID_IOCYCLE,
	REGID_STATE,
	REGID_FDMG,
	REGID_MAX,
	REGID_REGS8SIZE = ((REGID_MAX + (sizeof(int) - 1)) & ~(sizeof(int) - 1))
};

typedef struct KMZ80_CONTEXT_TAG KMZ80_CONTEXT;
struct KMZ80_CONTEXT_TAG {
	Uint8 regs8[REGID_REGS8SIZE];
	Uint32 sp;
	Uint32 pc;
	/* 裏レジスタ */
	Uint32 saf;
	Uint32 sbc;
	Uint32 sde;
	Uint32 shl;
	/* テンポラリフラグレジスタ(暗黙のキャリーフラグ) */
	Uint32 t_fl;
	/* テンポラリデーターレジスタ(暗黙の35フラグ) */
	Uint32 t_dx;
	/* ここまでは保存するべき */
	/* テンポラリプログラムカウンタ */
	Uint32 t_pc;
	/* テンポラリオペランドレジスタ */
	Uint32 t_op;
	/* テンポラリアドレスレジスタ */
	Uint32 t_ad;
	/* サイクルカウンタ */
	Uint32 cycle;
	/* オペコードテーブル */
	void *opt;
	/* オペコードCBテーブル */
	void *optcb;
	/* オペコードEDテーブル */
	void *opted;
	/* 追加サイクルテーブル */
	void *cyt;
	/* R800メモリーページ(ページブレイクの確認用) */
	Uint32 mempage;
	/* 特殊用途割り込みベクタ */
	Uint32 vector[5];
	/* RST飛び先基本アドレス */
	Uint32 rstbase;
	/* 追加フラグ */
	/*   bit0: 暗黙のキャリー有効 */
	/*   bit1: 割り込み要求自動クリア */
	Uint32 exflag;
	/* 内部定義コールバック */
	Uint32 (*sysmemfetch)(KMZ80_CONTEXT *context);
	Uint32 (*sysmemread)(KMZ80_CONTEXT *context, Uint32 a);
	void (*sysmemwrite)(KMZ80_CONTEXT *context, Uint32 a, Uint32 d);
	/* ユーザーデーターポインタ */
	void *user;
	/* ユーザー定義コールバック */
	Uint32 (*memread)(void *u, Uint32 a);
	void (*memwrite)(void *u, Uint32 a, Uint32 d);
	Uint32 (*ioread)(void *u, Uint32 a);
	void (*iowrite)(void *u, Uint32 a, Uint32 d);
	Uint32 (*busread)(void *u, Uint32 mode);
	Uint32 (*checkbreak)(void *u, KMZ80_CONTEXT *context);
	Uint32 (*patchedfe)(void *u, KMZ80_CONTEXT *context);
	/* ユーザー定義イベントタイマ */
	KMEVENT *kmevent;
};

void kmz80_reset(KMZ80_CONTEXT *context);
void kmr800_reset(KMZ80_CONTEXT *context);
void kmdmg_reset(KMZ80_CONTEXT *context);
Uint32 kmz80_exec(KMZ80_CONTEXT *context, Uint32 cycle);

#ifdef __cplusplus
}
#endif
#endif
