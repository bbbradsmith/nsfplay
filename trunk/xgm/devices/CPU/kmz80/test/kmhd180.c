/*
  KMZ80 HD64180(not supported)
  by Mamiya
*/

#include "kmz80i.h"

const static OPT_ITEM kmhd180_ot_edxx[0x100] = {
/* ED 00-07  00000??? */
	OPTABLE(STO_B,	0,		ADR_N,	OP_IN),
	OPTABLE(0,		LDO_B,	ADR_N,	OP_OUT),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		LDO_B,	0,		OP_TST),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 08-0F  00001??? */
	OPTABLE(STO_C,	0,		ADR_N,	OP_IN),
	OPTABLE(0,		LDO_C,	ADR_N,	OP_OUT),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		LDO_C,	0,		OP_TST),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 10-17  00010??? */
	OPTABLE(STO_D,	0,		ADR_N,	OP_IN),
	OPTABLE(0,		LDO_D,	ADR_N,	OP_OUT),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		LDO_D,	0,		OP_TST),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 18-1F  00011??? */
	OPTABLE(STO_E,	0,		ADR_N,	OP_IN),
	OPTABLE(0,		LDO_E,	ADR_N,	OP_OUT),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		LDO_E,	0,		OP_TST),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 20-27  00100??? */
	OPTABLE(STO_H,	0,		ADR_N,	OP_IN),
	OPTABLE(0,		LDO_H,	ADR_N,	OP_OUT),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		LDO_H,	0,		OP_TST),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 28-2F  00101??? */
	OPTABLE(STO_L,	0,		ADR_N,	OP_IN),
	OPTABLE(0,		LDO_L,	ADR_N,	OP_OUT),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		LDO_L,	0,		OP_TST),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 30-37  00110??? */
	OPTABLE(0,		0,		ADR_N,	OP_IN),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		LDO_M,	ADR_HL,	OP_TST),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 38-3F  00111??? */
	OPTABLE(STO_A,	0,		ADR_N,	OP_IN),
	OPTABLE(0,		LDO_A,	ADR_N,	OP_OUT),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		LDO_A,	0,		OP_TST),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),

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
	OPTABLE(STO_BC,	LDO_BC,	0,		OP_MLT),
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
	OPTABLE(STO_DE,	LDO_DE,	0,		OP_MLT),
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
	OPTABLE(STO_HL,	LDO_HL,	0,		OP_MLT),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_IM0),
	OPTABLE(STO_M,	LDO_M,	ADR_HL,	OP_RLD),
/* ED 70-77  01110??? */
	OPTABLE(0,		0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_Z,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_SP,	ADR_HL,	OP_SBC16),
	OPTABLE(STO_MM,	LDO_SP,	ADR_NN,	OP_LD),
	OPTABLE(0,		LDO_N,	ADR_C,	OP_TSTIO),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_SLP),
	OPTABLE(0,		0,		0,		OP_NOP),
/* ED 78-7F  01111??? */
	OPTABLE(STO_A,	0,		ADR_BC,	OP_IN),
	OPTABLE(0,		LDO_A,	ADR_BC,	OP_OUT),
	OPTABLE(STO_HL,	LDO_SP,	ADR_HL,	OP_ADC16),
	OPTABLE(STO_SP,	LDO_MM,	ADR_NN,	OP_LD),
	OPTABLE(STO_SP,	LDO_SP,	0,		OP_MLT),
	OPTABLE(0,		LDO_ST,	0,		OP_RETN),
	OPTABLE(0,		0,		0,		OP_IM2),
	OPTABLE(0,		0,		0,		OP_NOP),
/* ED 80-87  10000??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		0,		ADR_C,	OP_OTIM),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 88-8F  10001??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0,		0,		ADR_C,	OP_OTDM),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 90-97  10010??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_R0,	0,		ADR_C,	OP_OTIM),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
/* ED 98-9F  10011??? */
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(STO_R0,	0,		ADR_C,	OP_OTDM),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
	OPTABLE(0, 0, 0, OP_NOP),
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

#if 0
/* HD64180 */ 
/* XX       0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 1 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 2 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 3 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 4 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 5 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 6 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 7 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 8 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 9 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* A */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* B */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* C */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* D */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* E */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* F */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,

/* CBXX       0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 1 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 2 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 3 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 4 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 5 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 6 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 7 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 8 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 9 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* A */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* B */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* C */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* D */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* E */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* F */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,

/* CBXX       0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 1 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 2 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 3 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 4 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 5 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 6 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 7 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 8 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 9 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* A */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* B */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* C */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* D */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* E */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* F */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,

/* DDXX       0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 1 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 2 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 3 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 4 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 5 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 6 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 7 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 8 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 9 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* A */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* B */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* C */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* D */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* E */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* F */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,

/* DDCBXX   0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 1 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 2 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 3 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 4 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 5 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 6 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 7 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 8 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 9 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* A */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* B */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* C */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* D */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* E */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* F */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,

/* EDXX     0 1 2 3 4 5 6 7  8 9 A B C D E F */
/* 0 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 1 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 2 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 3 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 4 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 5 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 6 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 7 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 8 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* 9 */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* A */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* B */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* C */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* D */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* E */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
/* F */		*,*,*,*,*,*,*,*, *,*,*,*,*,*,*,*,
#endif

static Uint32 kmhd180_memread(KMZ80_CONTEXT *context, Uint32 a)
{
	CYCLEMEM;
	return context->memread(context->user, a);
}

static void kmhd180_memwrite(KMZ80_CONTEXT *context, Uint32 a, Uint32 d)
{
	CYCLEMEM;
	context->memwrite(context->user, a, d);
}

void kmhd180_reset(KMZ80_CONTEXT *context) {
	extern const OPT_ITEM kmz80_ot_xx[0x100];
	extern const Uint8 kmz80_ot_cbxx[0x20];
	extern void kmz80_reset_common(KMZ80_CONTEXT *context);
	kmz80_reset_common(context);
	EXFLAG = 0;
	M1CYCLE = 0;
	MEMCYCLE = 3;
	IOCYCLE = 4;
	OPT = kmz80_ot_xx;
	OPTCB = kmz80_ot_cbxx;
	OPTED = kmhd180_ot_edxx;
	SYSMEMREAD = kmhd180_memread;
	SYSMEMWRITE = kmhd180_memwrite;
}
