/*
  KMxxx event timer header
  by Mamiya
*/

#ifndef KMEVENT_H_
#define KMEVENT_H_

#include "kmtypes.h"
#ifdef __cplusplus
extern "C" {
#endif

#define KMEVENT_ITEM_MAX 31 /* MAX 255 */

typedef struct KMEVENT_TAG KMEVENT;
typedef struct KMEVENT_ITEM_TAG KMEVENT_ITEM;
typedef Uint32 KMEVENT_ITEM_ID;
struct KMEVENT_ITEM_TAG {
	/* メンバ直接アクセス禁止 */
	void *user;
	void (*proc)(KMEVENT *event, KMEVENT_ITEM_ID curid, void *user);
	Uint32 count;	/* イベント発生時間 */
	Uint8 prev;		/* 双方向リンクリスト */
	Uint8 next;		/* 双方向リンクリスト */
	Uint8 sysflag;	/* 内部状態フラグ */
	Uint8 flag2;	/* 未使用 */
};
struct KMEVENT_TAG {
	/* メンバ直接アクセス禁止 */
	KMEVENT_ITEM item[KMEVENT_ITEM_MAX + 1];
};

void kmevent_init(KMEVENT *kme);
KMEVENT_ITEM_ID kmevent_alloc(KMEVENT *kme);
void kmevent_free(KMEVENT *kme, KMEVENT_ITEM_ID curid);
void kmevent_settimer(KMEVENT *kme, KMEVENT_ITEM_ID curid, Uint32 time);
Uint32 kmevent_gettimer(KMEVENT *kme, KMEVENT_ITEM_ID curid, Uint32 *time);
void kmevent_setevent(KMEVENT *kme, KMEVENT_ITEM_ID curid, void (*proc)(), void *user);
void kmevent_process(KMEVENT *kme, Uint32 cycles);

#ifdef __cplusplus
}
#endif
#endif
