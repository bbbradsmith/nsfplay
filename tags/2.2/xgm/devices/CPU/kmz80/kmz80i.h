/*
  KMZ80 internal header
  by Mamiya
*/

#include "kmz80.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	ST_LOCKINT = (1 << 0),
	ST_DD = (1 << 1),	/* ix */
	ST_FD = (1 << 2),	/* iy */
	ST_CB = (1 << 3)	/* DDCB FDCB (ii+d) write back */
} KMZ80_STATE;

enum {
	EXF_ICEXIST = (1 << 0),
	EXF_AUTOIRQCLEAR = (1 << 1)
} KMZ80_EXFLAG;

enum {
	OP_NOP,
	/* 交換命令 */
	OP_EX_AF_AF,
	OP_EXX,
	OP_EX_DE_HL,
	OP_EX_MSP16_HL_,
	/* 8ビット演算命令 */
	OP_ADD,
	OP_ADC,
	OP_SUB,
	OP_SBC,
	OP_AND,
	OP_OR,
	OP_XOR,
	OP_CP,
	OP_INC,
	OP_DEC,
	OP_DAA,
	OP_CPL,
	OP_NEG,
	/* 16ビット演算命令 */
	OP_ADD16,
	OP_ADC16,
	OP_SBC16,
	OP_INC16,
	OP_DEC16,
	/* ブロック転送命令 */
	OP_LDI,
	OP_LDD,
	OP_CPI,
	OP_CPD,
	/* 分岐命令 */
	OP_JP,
	OP_JPCC,
	OP_JPCX,
	OP_JR,
	OP_JRCC,
	OP_DJNZ,
	OP_CALL,
	OP_CALLCC,
	OP_CALLCX,
	OP_RETCC,
	OP_RETCX,
	OP_RETI,
	OP_RETN,
	OP_RST,
	/* ビット循環命令*/
	OP_RLCA,
	OP_RRCA,
	OP_RLA,
	OP_RRA,
	OP_RLC,
	OP_RRC,
	OP_RL,
	OP_RR,
	OP_SLA,
	OP_SRA,
	OP_SLL,
	OP_SRL,
	OP_RLD,
	OP_RRD,
	/* ビット操作命令*/
	OP_BIT,
	OP_RES,
	OP_SET,
	/* 入出力命令 */
	OP_IN,
	OP_OUT,
	/* ブロック入出力命令 */
	OP_INI,
	OP_OUTI,
	OP_IND,
	OP_OUTD,
	/* CPU制御命令 */
	OP_CCF,
	OP_SCF,
	OP_HALT,
	OP_HALT2,
	OP_DI,
	OP_EI,
	OP_IM0,
	OP_IM1,
	OP_IM2,
	OP_PREFIX_CB,
	OP_PREFIX_DD,
	OP_PREFIX_ED,
	OP_PREFIX_FD,
	/* maratZ80命令 */
	OP_PATCH,
	/* HD64180命令 */
	OP_TST,
	OP_TSTIO,
	OP_OTIM,
	OP_OTDM,
	/* HD64180乗算命令 */
	OP_MLT,
	/* R800乗算命令 */
	OP_MULUB,
	OP_MULUW,
	/* DMG命令 */
	OP_SWAP,
	OP_MAX,
	/* alias */
	OP_LD = OP_NOP,
	OP_PUSH = OP_NOP,
	OP_POP = OP_NOP,
	OP_RET = OP_JP,
	OP_ADDSP = OP_ADD16,
	/* DMGCPU制御命令 */
	OP_STOP = OP_HALT,
	/* HD64180CPU制御命令 */
	OP_SLP = OP_HALT2
} KMZ80_OP_CODE;

enum {
	STO_NONE,
	STO_B,		/* b */
	STO_C,		/* c */
	STO_D,		/* d */
	STO_E,		/* e */
	STO_H,		/* force h */
	STO_L,		/* force l */
	STO_M,		/* (ea) */
	STO_A,		/* a */
	/* ここまで順序固定 */
	STO_BC,		/* bc */
	STO_DE,		/* de */
	STO_HL,		/* force hl */
	STO_AF,		/* af */
	STO_SP,		/* sp */
	STO_MM,		/* (ea).w */
	STO_IX,		/* hl ix iy */
	STO_IH,		/* h ixh iyh */
	STO_IL,		/* l ixl iyl */
	STO_I,		/* i */
	STO_R,		/* r */
	STO_R0,		/* repeat OTIM OTDM */
	STO_R1,		/* repeat INIR INDR OTIR OTDR */
	STO_R2,		/* repeat LDIR LDDR */
	STO_R3,		/* repeat CPIR CPDR */
	STO_ST,	/* (-sp).w */
	STO_AFDMG,	/* af(dmg) */
	STO_MAX
} KMZ80_POSTOP_CODE;

enum {
	LDO_NONE,
	LDO_B,		/* b */
	LDO_C,		/* c */
	LDO_D,		/* d */
	LDO_E,		/* e */
	LDO_H,		/* force h */
	LDO_L,		/* force l */
	LDO_M,		/* (ea) */
	LDO_A,		/* a */
	/* ここまで順序固定 */
	LDO_BC,		/* bc */
	LDO_DE,		/* de */
	LDO_HL,		/* force hl */
	LDO_AF,		/* af */
	LDO_SP,		/* sp */
	LDO_MM,		/* (ea).w */
	LDO_N,		/* n */
	LDO_NN,		/* nn */
	LDO_IX,		/* hl ix iy*/
	LDO_IH,		/* h ixh iyh */
	LDO_IL,		/* l ixl iyl */
	LDO_I,		/* i */
	LDO_R,		/* r */
	LDO_Z,		/* 0 */
	LDO_ST,		/* (sp+).w */
	LDO_SN,		/* signed n */
	LDO_AFDMG,	/* af(dmg) */
	LDO_MAX
} KMZ80_PREOP_CODE;

enum {
	/* Z80 R800 HD64180 DMG */
	ADR_NONE,
	ADR_AN,		/* (An) */
	ADR_NN,		/* (nn) */
	ADR_BC,		/* (bc) */
	ADR_DE,		/* (de) */
	ADR_HL,		/* force (hl) */
	/* Z80 R800 HD64180 */
	ADR_IX,		/* (hl) (ix) (iy) */
	ADR_ID,		/* (hl) (ix+d) (iy+d) */
	/* HD64180 */
	ADR_N,		/* (n) */
	ADR_C,		/* (C) */
	/* DMG */
	ADR_HI,		/* (hl+) */
	ADR_HD,		/* (hl-) */
	ADR_HN,		/* (ff00h+n) */
	ADR_HC,		/* (ff00h+c) */
	ADR_SP,		/* (sp+n) */
	ADR_MAX
} KMZ80_OP_ADR;

typedef struct {
	Uint8 post;
	Uint8 pre;
	Uint8 adr;
	Uint8 op;
} OPT_ITEM;
#define OPTABLE(post,pre,adr,op) {post,pre,adr,op}
#define OPTABLENOP OPTABLE(STO_NONE,LDO_NONE,ADR_NONE,OP_NOP)
#define OPTABLENOP4 OPTABLENOP,OPTABLENOP,OPTABLENOP,OPTABLENOP
#define OPTABLENOP16 OPTABLENOP4,OPTABLENOP4,OPTABLENOP4,OPTABLENOP4

#define RTO8(d) (d & 0xff)
#define RTO16(d) (d & 0xffff)

#define BC (C + (B << 8))
#define B (context->regs8[REGID_B])
#define C (context->regs8[REGID_C])
#define DE (E + (D << 8))
#define D (context->regs8[REGID_D])
#define E (context->regs8[REGID_E])
#define HL (L + (H << 8))
#define H (context->regs8[REGID_H])
#define L (context->regs8[REGID_L])
#define AF (F + (A << 8))
#define F (context->regs8[REGID_F])
#define A (context->regs8[REGID_A])
#define IX (IXL + (IXH << 8))
#define IXH (context->regs8[REGID_IXH])
#define IXL (context->regs8[REGID_IXL])
#define IY (IYL + (IYH << 8))
#define IYH (context->regs8[REGID_IYH])
#define IYL (context->regs8[REGID_IYL])
#define R (context->regs8[REGID_R])
#define R7 (context->regs8[REGID_R7])
#define I (context->regs8[REGID_R7])
#define IFF1 (context->regs8[REGID_IFF1])
#define IFF2 (context->regs8[REGID_IFF2])
#define IMODE (context->regs8[REGID_IMODE])
#define NMIREQ (context->regs8[REGID_NMIREQ])
#define INTREQ (context->regs8[REGID_INTREQ])
#define HALTED (context->regs8[REGID_HALTED])
#define M1CYCLE (context->regs8[REGID_M1CYCLE])
#define MEMCYCLE (context->regs8[REGID_MEMCYCLE])
#define IOCYCLE (context->regs8[REGID_IOCYCLE])
#define STATE (context->regs8[REGID_STATE])
#define FDMG (context->regs8[REGID_FDMG])
#define SP (context->sp)
#define PC (context->pc)
#define SAF (context->saf)
#define SBC (context->sbc)
#define SDE (context->sde)
#define SHL (context->shl)
#define TFL (context->t_fl)
#define TDX (context->t_dx)
#define TPC (context->t_pc)
#define TOP (context->t_op)
#define TAD (context->t_ad)
#define CYCLE (context->cycle)
#define OPT ((const OPT_ITEM *)context->opt)
#define OPTCB ((const Uint8 *)context->optcb)
#define OPTED ((const OPT_ITEM *)context->opted)
#define CYT ((const Uint8 *)context->cyt)
#define MEMPAGE (context->mempage)
#define VECTOR (context->vector)
#define RSTBASE (context->rstbase)
#define EXFLAG (context->exflag)
#define ICEXIST (EXFLAG & EXF_ICEXIST)
#define AUTOIRQCLEAR (EXFLAG & EXF_AUTOIRQCLEAR)
#define SYSMEMFETCH context->sysmemfetch
#define SYSMEMREAD context->sysmemread
#define SYSMEMWRITE context->sysmemwrite
#if 1
#define MEMREAD(a) (context->sysmemread(context, a))
#define MEMWRITE(a,d) (context->sysmemwrite(context, a, d))
#else
#define MEMREAD(a) (context->memread(context->user, a))
#define MEMWRITE(a,d) (context->memwrite(context->user, a, d))
#endif
#define IOREAD(a) (context->ioread(context->user, a))
#define IOWRITE(a,d) (context->iowrite(context->user, a, d))
#define BUSREAD(a) (context->busread(context->user, a))
#define CHECKBREAK (context->checkbreak)
#define PATCHEDFE (context->patchedfe)
#define CYCLEM1 { R++; CYCLE += M1CYCLE; }
#define CYCLEMEM { CYCLE += MEMCYCLE; }
#define CYCLEIO { CYCLE += IOCYCLE; }

#define CF (1 << 0)
#define NF (1 << 1)
#define PF (1 << 2)
#define XF (1 << 3)
#define HF (1 << 4)
#define YF (1 << 5)
#define ZF (1 << 6)
#define SF (1 << 7)

#ifdef __cplusplus
}
#endif
