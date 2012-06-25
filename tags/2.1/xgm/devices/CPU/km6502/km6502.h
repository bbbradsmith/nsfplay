#ifndef KM6502_H_
#define KM6502_H_

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

struct K6502_Context {
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

enum K6502_FLAGS {
	K6502_C_FLAG = 0x01,
	K6502_Z_FLAG = 0x02,
	K6502_I_FLAG = 0x04,
	K6502_D_FLAG = 0x08,
	K6502_B_FLAG = 0x10,
	K6502_R_FLAG = 0x20,
	K6502_V_FLAG = 0x40,
	K6502_N_FLAG = 0x80
};

enum K6502_IRQ {
	K6502_INIT	= 1,
	K6502_RESET	= 2,
	K6502_NMI	= 4,
	K6502_BRK	= 8,
	K6502_INT	= 16
};

#ifdef STATIC_CONTEXT6502
External void K6502_Exec(void);
#else
External void K6502_Exec(struct K6502_Context *pc);
#endif

#if !USE_CALLBACK
#if USE_USERPOINTER
External Uword K6502_ReadByte(void *user, Uword adr);
External void K6502_WriteByte(void *user, Uword adr, Uword value);
#else
External Uword K6502_ReadByte(Uword adr);
External void K6502_WriteByte(Uword adr, Uword value);
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif
