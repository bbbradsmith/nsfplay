#ifndef KM65C02_H_
#define KM65C02_H_

#include "kmconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if USE_USERPOINTER
typedef Uword (Callback *ReadHandler)(void *user, Uword adr);
typedef void (Callback *WriteHandler)(void *user, Uword adr, Uword value);
#else
typedef Uword (Callback *ReadHandler)(Uword adr);
typedef void (Callback *WriteHandler)(Uword adr, Uword value);
#endif

struct K65C02_Context {
	Uword A;			/* Accumulator */
	Uword P;			/* Status register */
	Uword X;			/* X register */
	Uword Y;			/* Y register */
	Uword S;			/* Stack pointer */
	Uword PC;			/* Program Counter */

	Uword iRequest;		/* interrupt request */
	Uword iMask;		/* interrupt mask */
	Uword clock;		/* (incremental)cycle counter */
	Uword lastcode;
	void *user;			/* pointer to user area */

#if USE_CALLBACK
	/* pointer to callback functions */
#if USE_INLINEMMC
	ReadHandler ReadByte[1 << (16 - USE_INLINEMMC)];
	WriteHandler WriteByte[1 << (16 - USE_INLINEMMC)];
#else
	ReadHandler ReadByte;
	WriteHandler WriteByte;
#endif
#endif

#if USE_DIRECT_ZEROPAGE
	Ubyte *zeropage;	/* pointer to zero page */
#endif
};

enum K65C02_FLAGS {
	K65C02_C_FLAG = 0x01,
	K65C02_Z_FLAG = 0x02,
	K65C02_I_FLAG = 0x04,
	K65C02_D_FLAG = 0x08,
	K65C02_B_FLAG = 0x10,
	K65C02_R_FLAG = 0x20,
	K65C02_V_FLAG = 0x40,
	K65C02_N_FLAG = 0x80
};

enum K65C02_IRQ {
	K65C02_INIT	= 1,
	K65C02_RESET	= 2,
	K65C02_NMI	= 4,
	K65C02_BRK	= 8,
	K65C02_INT	= 16
};

#ifdef STATIC_CONTEXT65C02
External void K6502_Exec(void);
#else
External void K6502_Exec(struct K65C02_Context *pc);
#endif

#if !USE_CALLBACK
#if USE_USERPOINTER
External Uword K65C02_ReadByte(void *user, Uword adr);
External void K65C02_WriteByte(void *user, Uword adr, Uword value);
#else
External Uword K65C02_ReadByte(Uword adr);
External void K65C02_WriteByte(Uword adr, Uword value);
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif
