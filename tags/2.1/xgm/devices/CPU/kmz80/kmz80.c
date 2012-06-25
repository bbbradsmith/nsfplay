/*
  KMZ80 Z80
  by Mamiya
*/

#include "kmz80i.h"



const static OPT_ITEM kmz80_ot_edxx[0x100] = {
/* ED 00-0F  0000???? */	OPTABLENOP16,
/* ED 10-1F  0001???? */	OPTABLENOP16,
/* ED 20-2F  0010???? */	OPTABLENOP16,
/* ED 30-3F  0011???? */	OPTABLENOP16,
/* ED 40-47  01000??? */
	OPTABLE(STO_B,	0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_B,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_BC,	ADR_HL,	OP_SBC16),
	OPTABLE(STO_MM,	LDO_BC,	ADR_NN,	OP_LD),
	OPTABLE(STO_A,	LDO_A,	0,		OP_NEG),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_IM0),
	OPTABLE(STO_I,	LDO_A,	0,		OP_LD),
/* ED 48-4F  01001??? */
	OPTABLE(STO_C,	0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_C,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_BC,	ADR_HL,	OP_ADC16),
	OPTABLE(STO_BC,	LDO_MM,	ADR_NN,	OP_LD),
	OPTABLE(STO_A,	LDO_A,	0,		OP_NEG),
	OPTABLE(0,		LDO_ST,	0,		OP_RETI),
	OPTABLE(0,		0,		0,		OP_IM0),
	OPTABLE(STO_R,	LDO_A,	0,		OP_LD),
/* ED 50-57  01010??? */
	OPTABLE(STO_D,	0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_D,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_DE,	ADR_HL,	OP_SBC16),
	OPTABLE(STO_MM,	LDO_DE,	ADR_NN,	OP_LD),
	OPTABLE(STO_A,	LDO_A,	0,		OP_NEG),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_IM1),
	OPTABLE(STO_A,	LDO_I,	0,		OP_LD),
/* ED 58-5F  01011??? */
	OPTABLE(STO_E,	0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_E,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_DE,	ADR_HL,	OP_ADC16),
	OPTABLE(STO_DE,	LDO_MM,	ADR_NN,	OP_LD),
	OPTABLE(STO_A,	LDO_A,	0,		OP_NEG),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_IM2),
	OPTABLE(STO_A,	LDO_R,	0,		OP_LD),
/* ED 60-67  01100??? */
	OPTABLE(STO_H,	0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_H,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_HL,	ADR_HL,	OP_SBC16),
	OPTABLE(STO_MM,	LDO_HL,	ADR_NN,	OP_LD),
	OPTABLE(STO_A,	LDO_A,	0,		OP_NEG),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_IM0),
	OPTABLE(STO_M,	LDO_M,	ADR_HL,	OP_RRD),
/* ED 68-6F  01101??? */
	OPTABLE(STO_L,	0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_L,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_HL,	ADR_HL,	OP_ADC16),
	OPTABLE(STO_HL,	LDO_MM,	ADR_NN,	OP_LD),
	OPTABLE(STO_A,	LDO_A,	0,		OP_NEG),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_IM0),
	OPTABLE(STO_M,	LDO_M,	ADR_HL,	OP_RLD),
/* ED 70-77  01110??? */
	OPTABLE(0,		0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_Z,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_SP,	ADR_HL,	OP_SBC16),
	OPTABLE(STO_MM,	LDO_SP,	ADR_NN,	OP_LD),
	OPTABLE(STO_A,	LDO_A,	0,		OP_NEG),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_IM1),
	OPTABLE(0,		0,		0,		OP_NOP),
/* ED 78-7F  01111??? */
	OPTABLE(STO_A,	0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_A,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_SP,	ADR_HL,	OP_ADC16),
	OPTABLE(STO_SP,	LDO_MM,	ADR_NN,	OP_LD),
	OPTABLE(STO_A,	LDO_A,	0,		OP_NEG),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_IM2),
	OPTABLE(0,		0,		0,		OP_NOP),
/* ED 80-8F  1000???? */	OPTABLENOP16,
/* ED 90-9F  1001???? */	OPTABLENOP16,
/* ED A0-A7  10100??? */
	OPTABLE(0,		0,		0,		OP_LDI),
	OPTABLE(0,		0,		0,		OP_CPI),
	OPTABLE(0,		0,		ADR_BC,	OP_INI),
	OPTABLE(0,		0,		ADR_BC,	OP_OUTI),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED A8-AF  10101??? */
	OPTABLE(0,		0,		0,		OP_LDD),
	OPTABLE(0,		0,		0,		OP_CPD),
	OPTABLE(0,		0,		ADR_BC,	OP_IND),
	OPTABLE(0,		0,		ADR_BC,	OP_OUTD),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED B0-B7  10110??? */
	OPTABLE(STO_R2,	0,		0,		OP_LDI),
	OPTABLE(STO_R3,	0,		0,		OP_CPI),
	OPTABLE(STO_R1,	0,		ADR_BC,	OP_INI),
	OPTABLE(STO_R1,	0,		ADR_BC,	OP_OUTI),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED B8-BF  10111??? */
	OPTABLE(STO_R2,	0,		0,		OP_LDD),
	OPTABLE(STO_R3,	0,		0,		OP_CPD),
	OPTABLE(STO_R1,	0,		ADR_BC,	OP_IND),
	OPTABLE(STO_R1,	0,		ADR_BC,	OP_OUTD),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED C0-CF  1100???? */	OPTABLENOP16,
/* ED D0-DF  1101???? */	OPTABLENOP16,
/* ED E0-EF  1110???? */	OPTABLENOP16,
/* ED F0-FF  1111???? */
	OPTABLENOP4,OPTABLENOP4,OPTABLENOP4,
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_PATCH),
	OPTABLE(0, 0, 0, OP_NOP),
};

const static Uint8 kmz80_ct[0x510] = {
/* Z80 追加クロック */ 
/* XX       0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		0,0,0,2,0,0,0,0, 0,7,0,2,0,0,0,0,
/* 1 */		1,0,0,2,0,0,0,0, 5,7,0,2,0,0,0,0,
/* 2 */		0,0,0,2,0,0,0,0, 0,7,0,2,0,0,0,0,
/* 3 */		0,0,0,2,1,1,0,0, 0,7,0,2,0,0,0,0,
/* 4 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 5 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 6 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 7 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 8 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 9 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* A */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* B */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* C */		1,0,0,0,0,1,0,1, 1,0,0,0,0,1,0,1,
/* D */		1,0,0,0,0,1,0,1, 1,0,0,0,0,0,0,1,
/* E */		1,0,0,3,0,1,0,1, 1,0,0,0,0,0,0,1,
/* F */		1,0,0,0,0,1,0,1, 1,2,0,0,0,0,0,1,

/* DDXX     0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		0,0,0,2,0,0,0,0, 0,7,0,2,0,0,0,0,
/* 1 */		1,0,0,2,0,0,0,0, 5,7,0,2,0,0,0,0,
/* 2 */		0,0,0,2,0,0,0,0, 0,7,0,2,0,0,0,0,
/* 3 */		0,0,0,2,5,5,2,0, 0,7,0,2,0,0,0,0,
/* 4 */		0,0,0,0,0,0,5,0, 0,0,0,0,0,0,5,0,
/* 5 */		0,0,0,0,0,0,5,0, 0,0,0,0,0,0,5,0,
/* 6 */		0,0,0,0,0,0,5,0, 0,0,0,0,0,0,5,0,
/* 7 */		5,5,5,5,5,5,0,5, 0,0,0,0,0,0,5,0,
/* 8 */		0,0,0,0,0,0,5,0, 0,0,0,0,0,0,5,0,
/* 9 */		0,0,0,0,0,0,5,0, 0,0,0,0,0,0,5,0,
/* A */		0,0,0,0,0,0,5,0, 0,0,0,0,0,0,5,0,
/* B */		0,0,0,0,0,0,5,0, 0,0,0,0,0,0,5,0,
/* C */		1,0,0,0,0,1,0,1, 1,0,0,0,0,1,0,1,
/* D */		1,0,0,0,0,1,0,1, 1,0,0,0,0,0,0,1,
/* E */		1,0,0,3,0,1,0,1, 1,0,0,0,0,0,0,1,
/* F */		1,0,0,0,0,1,0,1, 1,2,0,0,0,0,0,1,

/* CBXX     0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 1 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 2 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 3 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 4 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 5 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 6 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 7 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 8 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 9 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* A */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* B */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* C */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* D */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* E */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* F */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,

/* DDCBXX   0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* 1 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* 2 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* 3 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* 4 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* 5 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* 6 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* 7 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* 8 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* 9 */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* A */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* B */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* C */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* D */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* E */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
/* F */		3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,

/* EDXX     0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 1 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 2 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 3 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 4 */		1,0,7,0,0,0,0,1, 1,0,7,0,0,0,0,1,
/* 5 */		1,0,7,0,0,0,0,1, 1,0,7,0,0,0,0,1,
/* 6 */		1,0,7,0,0,0,0,4, 1,0,7,0,0,0,0,4,
/* 7 */		1,0,7,0,0,0,0,0, 1,0,7,0,0,0,0,0,
/* 8 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 9 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* A */		2,5,1,1,0,0,0,0, 2,5,1,1,0,0,0,0,
/* B */		2,5,1,1,0,0,0,0, 2,5,1,1,0,0,0,0,
/* C */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* D */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* E */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* F */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	5,	/* 0x500 DJNZ */
	0,	/* 0x501  不成立 */
	1,	/* 0x502 CALL cc */
	0,	/* 0x503  不成立 */
	5,	/* 0x504 JR cc */
	0,	/* 0x505  不成立 */
	0,	/* 0x506 JP cc */
	0,	/* 0x507  不成立 */
	0,	/* 0x508 RET cc */
	0,	/* 0x509  不成立 */
	5,	/* 0x50A CPDR CPIR INDR INIR LDDR LDIR ODIR OTIR */
	0,	/* 0x50B  不成立 */
	5,	/* 0x50C */
	0,	/* 0x50D  不成立 */
	0,	/* 0x50E OTIMR OTDMR */
	0,	/* 0x50F  不成立 */
};

static Uint32 kmz80_memread(KMZ80_CONTEXT *context, Uint32 a)
{
	CYCLEMEM;
	return context->memread(context->user, a);
}

static void kmz80_memwrite(KMZ80_CONTEXT *context, Uint32 a, Uint32 d)
{
	CYCLEMEM;
	context->memwrite(context->user, a, d);
}

void kmz80_reset(KMZ80_CONTEXT *context) {
	extern const OPT_ITEM kmz80_ot_xx[0x100];
	extern const Uint8 kmz80_ot_cbxx[0x20];
	extern void kmz80_reset_common(KMZ80_CONTEXT *context);
	kmz80_reset_common(context);
	EXFLAG = EXF_ICEXIST;
	M1CYCLE = 1;	/* MSXなら2 */
	MEMCYCLE = 3;
	IOCYCLE = 4;
	OPT = kmz80_ot_xx;
	OPTCB = kmz80_ot_cbxx;
	OPTED = kmz80_ot_edxx;
	CYT = kmz80_ct;
	SYSMEMREAD = kmz80_memread;
	SYSMEMWRITE = kmz80_memwrite;
}
