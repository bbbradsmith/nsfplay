/*
  KMZ80 R800
  by Mamiya
*/

#include "kmz80i.h"

const static OPT_ITEM kmr800_ot_edxx[0x100] = {
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
/* ED C0-C7  11000??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_B,	0,		OP_MULUB),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_BC, ADR_HL,	OP_MULUW),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED C8-CF  11001??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_C,	0,		OP_MULUB),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED D0-D7  11010??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_D,	0,		OP_MULUB),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_DE, ADR_HL,	OP_MULUW),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED D8-DF  11011??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_E,	0,		OP_MULUB),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED E0-E7  11100??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_H,	0,		OP_MULUB),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_HL, ADR_HL,	OP_MULUW),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED E8-EF  11101??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_L,	0,		OP_MULUB),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED F0-F7  11110??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_SP, ADR_HL,	OP_MULUW),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED F8-FF  11111??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_HL,	LDO_A,	0,		OP_MULUB),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_PATCH),
	OPTABLE(0, 0, 0, OP_NOP),
};

#if 0
add (hl)	+0cycle
add (ii+d)	+1cycle
PUSH	+1cycle
CPX		+1cycle
CPXR	+2cycle
JR		+1cycle
JR cc(成立)	+1cycle
DJNZ	+1cycle
RETI	+1cycle
RETN	+1cycle
RST		+1cycle
RLD		+1cycle
RRD		+1cycle
MULUB		+12cycle
MULUW		+34cycle
shr (hl)	+1cycle
shr (ii+d)	+1cycle
bit (hl)	+0cycle
bit (ii+d)	+0cycle
set (hl)	+1cycle
set (ii+d)	+1cycle
HALT	+1cycle
DI		+1cycle
IM?		+1cycle
#endif

const static Uint8 kmr800_ct[0x510] = {
/* Z80 追加クロック */ 
/* XX       0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 1 */		1,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,
/* 2 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 3 */		0,0,0,0,1,1,0,0, 0,0,0,0,0,0,0,0,
/* 4 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 5 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 6 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 7 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0,
/* 8 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 9 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* A */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* B */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* C */		0,0,0,0,0,1,0,1, 0,0,0,0,0,0,0,1,
/* D */		0,0,0,0,0,1,0,1, 0,0,0,0,0,0,0,1,
/* E */		0,0,0,0,0,1,0,1, 0,0,0,0,0,0,0,1,
/* F */		0,0,0,1,0,1,0,1, 0,0,0,0,0,0,0,1,

/* DDXX     0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 1 */		1,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,
/* 2 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 3 */		0,0,0,0,2,2,0,0, 0,0,0,0,0,0,0,0,
/* 4 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 5 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 6 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 7 */		1,1,1,1,1,1,1,1, 0,0,0,0,0,0,1,0,
/* 8 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* 9 */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* A */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* B */		0,0,0,0,0,0,1,0, 0,0,0,0,0,0,1,0,
/* C */		0,0,0,0,0,1,0,1, 0,0,0,0,0,0,0,1,
/* D */		0,0,0,0,0,1,0,1, 0,0,0,0,0,0,0,1,
/* E */		0,0,0,0,0,1,0,1, 0,0,0,0,0,0,0,1,
/* F */		0,0,0,1,0,1,0,1, 0,0,0,0,0,0,0,1,

/* CBXX     0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 1 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 2 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 3 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 4 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 5 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 6 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 7 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 8 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 9 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* A */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* B */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* C */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* D */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* E */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* F */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,

/* DDCBXX   0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 1 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 2 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 3 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 4 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 5 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 6 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 7 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 8 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* 9 */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* A */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* B */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* C */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* D */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* E */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
/* F */		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,

/* EDXX     0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 1 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 2 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 3 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 4 */		0,0,0,0,0,1,1,0, 0,0,0,0,0,1,1,0,
/* 5 */		0,0,0,0,0,1,1,0, 0,0,0,0,0,1,1,0,
/* 6 */		0,0,0,0,0,1,1,1, 0,0,0,0,0,1,1,1,
/* 7 */		0,0,0,0,0,1,1,0, 0,0,0,0,0,1,1,0,
/* 8 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 9 */		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* A */		0,1,0,0,0,0,0,0, 0,1,0,0,0,0,0,0,
/* B */		0,1,0,0,0,0,0,0, 0,1,0,0,0,0,0,0,
/* C */		0,12,0,34,0,0,0,0, 0,12,0,0,0,0,0,0,
/* D */		0,12,0,34,0,0,0,0, 0,12,0,0,0,0,0,0,
/* E */		0,12,0,34,0,0,0,0, 0,12,0,0,0,0,0,0,
/* F */		0, 0,0,34,0,0,0,0, 0,12,0,0,0,0,0,0,
	0,	/* 0x500 DJNZ */
	0,	/* 0x501  不成立 */
	0,	/* 0x502 CALL cc */
	0,	/* 0x503  不成立 */
	1,	/* 0x504 JR cc */
	0,	/* 0x505  不成立 */
	0,	/* 0x506 JP cc */
	0,	/* 0x507  不成立 */
	0,	/* 0x508 RET cc */
	0,	/* 0x509  不成立 */
	0,	/* 0x50A CPDR CPIR INDR INIR LDDR LDIR ODIR OTIR */
	0,	/* 0x50B  不成立 */
	0,	/* 0x50C */
	0,	/* 0x50D  不成立 */
	0,	/* 0x50E OTIMR OTDMR  */
	0,	/* 0x50F  不成立 */
};

static Uint32 kmr800_memread(KMZ80_CONTEXT *context, Uint32 a)
{
	CYCLEMEM;
	if (MEMPAGE != (a & 0xff00))
	{
		MEMPAGE = a & 0xff00;
		CYCLE += 1;
	}
	return context->memread(context->user, a);
}

static void kmr800_memwrite(KMZ80_CONTEXT *context, Uint32 a, Uint32 d)
{
	CYCLEMEM;
	if (MEMPAGE != (a & 0xff00))
	{
		MEMPAGE = a & 0xff00;
		CYCLE += 1;
	}
	context->memwrite(context->user, a, d);
}

void kmr800_reset(KMZ80_CONTEXT *context) {
	extern const OPT_ITEM kmz80_ot_xx[0x100];
	extern const Uint8 kmz80_ot_cbxx[0x20];
	extern void kmz80_reset_common(KMZ80_CONTEXT *context);
	kmz80_reset_common(context);
	EXFLAG = EXF_ICEXIST;
	M1CYCLE = 0;	/* MSXなら2 */
	MEMCYCLE = 1;
	IOCYCLE = 1;
	OPT = kmz80_ot_xx;
	OPTCB = kmz80_ot_cbxx;
	OPTED = kmr800_ot_edxx;
	CYT = kmr800_ct;
	SYSMEMREAD = kmr800_memread;
	SYSMEMWRITE = kmr800_memwrite;
}
