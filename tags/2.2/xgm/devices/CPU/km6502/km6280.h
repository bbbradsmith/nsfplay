#ifndef KM6280_H_
#define KM6280_H_

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

struct K6280_Context {
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

	Uword lowClockMode;

#if USE_CALLBACK
	/* pointer to callback functions */
#if USE_INLINEMMC
	ReadHandler ReadByte[1 << (16 - USE_INLINEMMC)];
	WriteHandler WriteByte[1 << (16 - USE_INLINEMMC)];
#else
	ReadHandler ReadByte;
	WriteHandler WriteByte;
#endif
	ReadHandler ReadMPR;
	WriteHandler WriteMPR;
	WriteHandler Write6270;
#endif

#if USE_DIRECT_ZEROPAGE
	Ubyte *zeropage;	/* pointer to zero page */
#endif
};

enum K6280_FLAGS {
	K6280_C_FLAG = 0x01,
	K6280_Z_FLAG = 0x02,
	K6280_I_FLAG = 0x04,
	K6280_D_FLAG = 0x08,
	K6280_B_FLAG = 0x10,
	K6280_T_FLAG = 0x20,
	K6280_V_FLAG = 0x40,
	K6280_N_FLAG = 0x80
};

enum K6280_IRQ {
	K6280_INIT	= 1,
	K6280_RESET	= 2,
	K6280_NMI	= 4,
	K6280_BRK	= 8,
	K6280_TIMER	= 16,
	K6280_INT1	= 32,
	K6280_INT2	= 64
};

#ifdef STATIC_CONTEXT6280
External void K6280_Exec(void);
#else
External void K6280_Exec(struct K6280_Context *pc);
#endif

#if !USE_CALLBACK
#if USE_USERPOINTER
External Uword CCall K6280_ReadByte(void *user, Uword adr);
External void CCall K6280_WriteByte(void *user, Uword adr, Uword value);
External Uword CCall K6280_ReadMPR(void *user, Uword adr);
External void CCall K6280_WriteMPR(void *user, Uword adr, Uword value);
External void CCall K6280_Write6270(void *user, Uword adr, Uword value);
#else
External Uword CCall K6280_ReadByte(Uword adr);
External void CCall K6280_WriteByte(Uword adr, Uword value);
External Uword CCall K6280_ReadMPR(Uword adr);
External void CCall K6280_WriteMPR(Uword adr, Uword value);
External void CCall K6280_Write6270(Uword adr, Uword value);
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif
