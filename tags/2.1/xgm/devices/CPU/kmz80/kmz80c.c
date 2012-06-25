/*
  KMZ80 common(Z80/R800/DMG-CPU/HD64180)
  by Mamiya
*/

#include "kmz80i.h"

#define DEBUGX2 0
#if DEBUGX2
#define DUMP_MAX 65536
#include <stdio.h>
void dox(int pc, int i, int a, int hl, int bc)
{
	static int p = 0;
	static FILE *fp = NULL;
	static int stop = 0;
	if (i == -1000)
	{
		stop = 1;
		return;
	}
	if (i == -10000)
	{
		stop = 0;
		return;
	}
	if (stop) return;
	if (p >= DUMP_MAX)
	{
		if (fp) { fclose(fp); fp = 0; }
		return;
	}
	if (!fp) fp = fopen("kmz80d.out", "wb");
	if (fp) { 
		fputc(pc, fp);
		fputc(pc >> 8, fp);
		fputc(i, fp);
		fputc(a, fp);
		fputc(hl, fp);
		fputc(hl >> 8, fp);
		fputc(bc, fp);
		fputc(bc >> 8, fp);
		p++;
	}
}
#define DEBUG_OUTPUT(i) dox(TPC, i, A, HL, BC)
#else
#define DEBUG_OUTPUT(i) {}
#endif

extern Uint8 flagtable[0x200] = {
/*
  0 <= i <= 0x1ff
  CF(bit0) (i > 0xff)
  NF(bit1) always(1)
  PF(bit2) parity(i)
  XF(bit3) !!(i & 0x008)
  HF(bit4) always(1)
  YF(bit5) !!(i & 0x020)
  ZF(bit6) !(i & 0x0ff)
  SF(bit7) !!(i & 0x080)
*/
	0x56, 0x12, 0x12, 0x16, 0x12, 0x16, 0x16, 0x12,
	0x1a, 0x1e, 0x1e, 0x1a, 0x1e, 0x1a, 0x1a, 0x1e,
	0x12, 0x16, 0x16, 0x12, 0x16, 0x12, 0x12, 0x16,
	0x1e, 0x1a, 0x1a, 0x1e, 0x1a, 0x1e, 0x1e, 0x1a,
	0x32, 0x36, 0x36, 0x32, 0x36, 0x32, 0x32, 0x36,
	0x3e, 0x3a, 0x3a, 0x3e, 0x3a, 0x3e, 0x3e, 0x3a,
	0x36, 0x32, 0x32, 0x36, 0x32, 0x36, 0x36, 0x32,
	0x3a, 0x3e, 0x3e, 0x3a, 0x3e, 0x3a, 0x3a, 0x3e,
	0x12, 0x16, 0x16, 0x12, 0x16, 0x12, 0x12, 0x16,
	0x1e, 0x1a, 0x1a, 0x1e, 0x1a, 0x1e, 0x1e, 0x1a,
	0x16, 0x12, 0x12, 0x16, 0x12, 0x16, 0x16, 0x12,
	0x1a, 0x1e, 0x1e, 0x1a, 0x1e, 0x1a, 0x1a, 0x1e,
	0x36, 0x32, 0x32, 0x36, 0x32, 0x36, 0x36, 0x32,
	0x3a, 0x3e, 0x3e, 0x3a, 0x3e, 0x3a, 0x3a, 0x3e,
	0x32, 0x36, 0x36, 0x32, 0x36, 0x32, 0x32, 0x36,
	0x3e, 0x3a, 0x3a, 0x3e, 0x3a, 0x3e, 0x3e, 0x3a,
	0x92, 0x96, 0x96, 0x92, 0x96, 0x92, 0x92, 0x96,
	0x9e, 0x9a, 0x9a, 0x9e, 0x9a, 0x9e, 0x9e, 0x9a,
	0x96, 0x92, 0x92, 0x96, 0x92, 0x96, 0x96, 0x92,
	0x9a, 0x9e, 0x9e, 0x9a, 0x9e, 0x9a, 0x9a, 0x9e,
	0xb6, 0xb2, 0xb2, 0xb6, 0xb2, 0xb6, 0xb6, 0xb2,
	0xba, 0xbe, 0xbe, 0xba, 0xbe, 0xba, 0xba, 0xbe,
	0xb2, 0xb6, 0xb6, 0xb2, 0xb6, 0xb2, 0xb2, 0xb6,
	0xbe, 0xba, 0xba, 0xbe, 0xba, 0xbe, 0xbe, 0xba,
	0x96, 0x92, 0x92, 0x96, 0x92, 0x96, 0x96, 0x92,
	0x9a, 0x9e, 0x9e, 0x9a, 0x9e, 0x9a, 0x9a, 0x9e,
	0x92, 0x96, 0x96, 0x92, 0x96, 0x92, 0x92, 0x96,
	0x9e, 0x9a, 0x9a, 0x9e, 0x9a, 0x9e, 0x9e, 0x9a,
	0xb2, 0xb6, 0xb6, 0xb2, 0xb6, 0xb2, 0xb2, 0xb6,
	0xbe, 0xba, 0xba, 0xbe, 0xba, 0xbe, 0xbe, 0xba,
	0xb6, 0xb2, 0xb2, 0xb6, 0xb2, 0xb6, 0xb6, 0xb2,
	0xba, 0xbe, 0xbe, 0xba, 0xbe, 0xba, 0xba, 0xbe,
	0x57, 0x13, 0x13, 0x17, 0x13, 0x17, 0x17, 0x13,
	0x1b, 0x1f, 0x1f, 0x1b, 0x1f, 0x1b, 0x1b, 0x1f,
	0x13, 0x17, 0x17, 0x13, 0x17, 0x13, 0x13, 0x17,
	0x1f, 0x1b, 0x1b, 0x1f, 0x1b, 0x1f, 0x1f, 0x1b,
	0x33, 0x37, 0x37, 0x33, 0x37, 0x33, 0x33, 0x37,
	0x3f, 0x3b, 0x3b, 0x3f, 0x3b, 0x3f, 0x3f, 0x3b,
	0x37, 0x33, 0x33, 0x37, 0x33, 0x37, 0x37, 0x33,
	0x3b, 0x3f, 0x3f, 0x3b, 0x3f, 0x3b, 0x3b, 0x3f,
	0x13, 0x17, 0x17, 0x13, 0x17, 0x13, 0x13, 0x17,
	0x1f, 0x1b, 0x1b, 0x1f, 0x1b, 0x1f, 0x1f, 0x1b,
	0x17, 0x13, 0x13, 0x17, 0x13, 0x17, 0x17, 0x13,
	0x1b, 0x1f, 0x1f, 0x1b, 0x1f, 0x1b, 0x1b, 0x1f,
	0x37, 0x33, 0x33, 0x37, 0x33, 0x37, 0x37, 0x33,
	0x3b, 0x3f, 0x3f, 0x3b, 0x3f, 0x3b, 0x3b, 0x3f,
	0x33, 0x37, 0x37, 0x33, 0x37, 0x33, 0x33, 0x37,
	0x3f, 0x3b, 0x3b, 0x3f, 0x3b, 0x3f, 0x3f, 0x3b,
	0x93, 0x97, 0x97, 0x93, 0x97, 0x93, 0x93, 0x97,
	0x9f, 0x9b, 0x9b, 0x9f, 0x9b, 0x9f, 0x9f, 0x9b,
	0x97, 0x93, 0x93, 0x97, 0x93, 0x97, 0x97, 0x93,
	0x9b, 0x9f, 0x9f, 0x9b, 0x9f, 0x9b, 0x9b, 0x9f,
	0xb7, 0xb3, 0xb3, 0xb7, 0xb3, 0xb7, 0xb7, 0xb3,
	0xbb, 0xbf, 0xbf, 0xbb, 0xbf, 0xbb, 0xbb, 0xbf,
	0xb3, 0xb7, 0xb7, 0xb3, 0xb7, 0xb3, 0xb3, 0xb7,
	0xbf, 0xbb, 0xbb, 0xbf, 0xbb, 0xbf, 0xbf, 0xbb,
	0x97, 0x93, 0x93, 0x97, 0x93, 0x97, 0x97, 0x93,
	0x9b, 0x9f, 0x9f, 0x9b, 0x9f, 0x9b, 0x9b, 0x9f,
	0x93, 0x97, 0x97, 0x93, 0x97, 0x93, 0x93, 0x97,
	0x9f, 0x9b, 0x9b, 0x9f, 0x9b, 0x9f, 0x9f, 0x9b,
	0xb3, 0xb7, 0xb7, 0xb3, 0xb7, 0xb3, 0xb3, 0xb7,
	0xbf, 0xbb, 0xbb, 0xbf, 0xbb, 0xbf, 0xbf, 0xbb,
	0xb7, 0xb3, 0xb3, 0xb7, 0xb3, 0xb7, 0xb7, 0xb3,
	0xbb, 0xbf, 0xbf, 0xbb, 0xbf, 0xbb, 0xbb, 0xbf,
};
#define FLAGTBL(x) flagtable[x]

static Uint32 CheckNZ(KMZ80_CONTEXT *context) { return !(F & ZF); }
static Uint32 CheckZ(KMZ80_CONTEXT *context) { return (F & ZF); }
static Uint32 CheckNC(KMZ80_CONTEXT *context) { return !(F & CF); }
static Uint32 CheckC(KMZ80_CONTEXT *context) { return (F & CF); }
static Uint32 (*CCTABLE[4])(KMZ80_CONTEXT *context) = {
	CheckNZ,	CheckZ,	CheckNC,	CheckC,
};
#define CHECKCC(cc) CCTABLE[cc](context)
static Uint32 CheckPO(KMZ80_CONTEXT *context) { return !(F & PF); }
static Uint32 CheckPE(KMZ80_CONTEXT *context) { return (F & PF); }
static Uint32 CheckP(KMZ80_CONTEXT *context) { return !(F & SF); }
static Uint32 CheckM(KMZ80_CONTEXT *context) { return (F & SF); }
static Uint32 (*CXTABLE[4])(KMZ80_CONTEXT *context) = {
	CheckPO,	CheckPE,	CheckP,	CheckM,
};
#define CHECKCX(cx) CXTABLE[cx](context)
#define FETCH(v) { v = SYSMEMFETCH(context); }

static Uint32 kmz80_fetch_normal(KMZ80_CONTEXT *context) {
	Uint32 op;
	op = MEMREAD(TPC);
	TPC = RTO16(TPC + 1);
	return op;
}

static Uint32 kmz80_fetch_im0(KMZ80_CONTEXT *context) {
	Uint32 op;
	CYCLEIO;	/* ??? */
	op = BUSREAD(0);
	if (op & 0x100) SYSMEMFETCH = kmz80_fetch_normal;
	if (op & 0x200) op = kmz80_fetch_normal(context);
	return RTO8(op);
}

void kmz80_reset_common(KMZ80_CONTEXT *context) {
	B = C = D = E = F = H = L = A = F = IXH = IXL = IYH = IYL = 0;
	R = R7 = I = IFF1 = IFF2 = IMODE = NMIREQ = INTREQ = HALTED = STATE = FDMG = 0;
	M1CYCLE = 0;
	MEMCYCLE = 3;
	IOCYCLE = 4;
	PC = SP = SAF = SBC = SDE = SHL = 0;
	TFL = TDX = TPC = TOP = TAD = 0;
	CYCLE = 0;
	MEMPAGE = 0;
	RSTBASE = 0;
	EXFLAG = 0;
	SYSMEMFETCH = kmz80_fetch_normal;
	context->kmevent = 0;
	CHECKBREAK = 0;
	PATCHEDFE = 0;
}


#define M_ADC_(a, fs) \
{ \
	Uint32 tmp; \
	tmp = TOP ^ a; \
	TOP = TOP + a + (TFL & CF); \
	TFL = (tmp ^ TOP) & HF; \
	TFL += ((~tmp & (TOP ^ a)) >> 5) & PF; \
	TFL += FLAGTBL(TOP) & (fs); \
}
#if 0	/* マクロ */
#define M_ADC(a, fs) M_ADC_(a, fs)
#else	/* 関数 */
static void M_ADC_FUNC(KMZ80_CONTEXT *context, Uint32 a, Uint32 fs) M_ADC_(a,fs)
#define M_ADC(a, fs) M_ADC_FUNC(context, a, fs)
#endif

#define M_ADC16_(fs) \
{ \
	Uint32 tmp; \
	TDX = TAD; \
	tmp = TOP ^ TAD; \
	TOP = TOP + TAD + (TFL & CF); \
	TFL = ((tmp ^ TOP) >> 8) & HF; \
	TFL += ((~tmp & (TOP ^ TAD)) >> (8 + 5)) & PF; \
	TFL += FLAGTBL(TOP >> 8) & fs; \
}
#define M_LDX_(dir) \
{ \
	Uint32 tmp; \
	TFL = F & (SF | ZF | CF); \
	TOP = MEMREAD(HL); \
	MEMWRITE(DE, TOP); \
	TOP += A; \
	tmp = HL + dir; L = RTO8(tmp); H = RTO8(tmp >> 8); \
	tmp = DE + dir; E = RTO8(tmp); D = RTO8(tmp >> 8); \
	tmp = BC - 1; C = RTO8(tmp); B = RTO8(tmp >> 8); \
	F = (F & (SF | ZF | CF)) + (BC ? PF : 0) + (TOP & XF) + ((TOP << 4) & YF); \
}
#define M_CPX_(dir) \
{ \
	Uint32 tmp; \
	TFL = CF; \
	TOP = MEMREAD(HL) ^ 0xff; \
	M_ADC(A, SF | ZF | NF); \
	TOP -= ((TFL & HF) >> 4); \
	tmp = HL + dir; L = RTO8(tmp); H = RTO8(tmp >> 8); \
	tmp = BC - 1; C = RTO8(tmp); B = RTO8(tmp >> 8); \
	F = (F & CF) + (TFL & ~PF) + (BC ? PF : 0) + (TOP & XF) + ((TOP << 4) & YF); \
}
#define M_INX_(d) \
{ \
		CYCLEIO; \
		TOP = IOREAD(TAD); \
		MEMWRITE(HL, TOP); \
}
#define M_OUTX_(d) \
{ \
		TOP = MEMREAD(HL); \
		CYCLEIO; \
		IOWRITE(TAD, TOP); \
}
#define M_POSTIOX_(dir) \
{ \
	Uint32 tmp; \
	tmp = HL + dir; L = RTO8(tmp); H = RTO8(tmp >> 8); \
	B = RTO8(B - 1); \
	TFL = ((RTO8(C + dir) + TOP) >> 8) & CF; \
	TFL += TFL << 4; \
	TFL += ((TOP >> 6) & NF) + (FLAGTBL(B) & (SF | ZF | XF | YF)); \
	tmp = (dir > 0) ? (0xa492 << 2) : (0x6da4 << 2); \
	tmp >>= (((C & 3) << 2) + (TOP & 3)); \
	tmp ^= FLAGTBL(B) & PF; \
	if (B & 0x0F) \
		tmp ^= (B << 2) | ((~B << 1) & B); \
	else \
		tmp ^= (B >> 2) | (~(B >> 3) & (B >> 4)); \
	tmp ^= C ^ TOP; \
	F = TFL = TFL + (tmp & PF); \
}
#define M_OTXM_(d) \
{ \
		TOP = MEMREAD(HL); \
		CYCLEIO; \
		IOWRITE(TAD, TOP); \
}
#define M_POSTOTXM_(dir) \
{ \
	Uint32 tmp; \
	TFL = F & (SF | ZF | CF); \
	tmp = HL + dir; L = RTO8(tmp); H = RTO8(tmp >> 8); \
	C = RTO8(C + 1); \
	tmp = (B + (0xff ^ 1) + 1) ^ 0x100; \
	TFL = (tmp ^ B ^ HF) & HF; \
	B = RTO8(tmp); \
	F = TFL = TFL + ((TOP >> 6) & NF) + (FLAGTBL(B) & (SF | ZF | XF | YF | PF | CF)); \
}

#if 0	/* マクロ */
#define M_ADC16(fs) M_ADC16_(fs)
#define M_LDX(dir) M_LDX_(dir)
#define M_CPX(dir) M_CPX_(dir)
#define M_INX(d) M_INX_(d)
#define M_OUTX(d) M_OUTX_(d)
#define M_POSTIOX(dir) M_POSTIOX_(dir)
#define M_OTXM(d) M_OTXM_(d)
#define M_POSTOTXM(dir) M_POSTOTXM_(dir)
#else	/* 関数 */
static void M_ADC16_FUNC(KMZ80_CONTEXT *context, Uint32 fs) M_ADC16_(fs)
static void M_LDX_FUNC(KMZ80_CONTEXT *context, int dir) M_LDX_(dir)
static void M_CPX_FUNC(KMZ80_CONTEXT *context, int dir) M_CPX_(dir)
static void M_INX_FUNC(KMZ80_CONTEXT *context) M_INX_(d)
static void M_OUTX_FUNC(KMZ80_CONTEXT *context) M_OUTX_(d)
static void M_POSTIOX_FUNC(KMZ80_CONTEXT *context, int dir) M_POSTIOX_(dir)
static void M_OTXM_FUNC(KMZ80_CONTEXT *context) M_OTXM_(d)
static void M_POSTOTXM_FUNC(KMZ80_CONTEXT *context, int dir) M_POSTOTXM_(dir)
#define M_ADC16(fs) M_ADC16_FUNC(context, fs)
#define M_LDX(dir) M_LDX_FUNC(context, dir)
#define M_CPX(dir) M_CPX_FUNC(context, dir)
#define M_INX(d) M_INX_FUNC(context)
#define M_OUTX(d) M_OUTX_FUNC(context)
#define M_POSTIOX(dir) M_POSTIOX_FUNC(context, dir)
#define M_OTXM(d) M_OTXM_FUNC(context)
#define M_POSTOTXM(dir) M_POSTOTXM_FUNC(context, dir)
#endif
#define M_DECODE(opt, cyt) \
{ \
	CYCLEM1; \
	FETCH(op0); \
	CYCLE += CYT[cyt + op0]; \
	DEBUG_OUTPUT(op0); \
	opp = opt + op0; \
}

Uint32 kmz80_exec(KMZ80_CONTEXT *context, Uint32 cycles)
{
	const OPT_ITEM *opp;
	OPT_ITEM opcb;
	Uint32 op0, kmecycle;

	kmecycle = CYCLE = 0;
	while (CYCLE < cycles)
	{
		if (CHECKBREAK && CHECKBREAK(context->user, context)) break;
		TPC = PC;
		STATE = 0;
		M_DECODE(OPT, 0);
		do
		{
			STATE &= ~ST_LOCKINT;
			switch (opp->adr)
			{
				case ADR_NONE: break;
				case ADR_AN:
					FETCH(TAD);
					TAD += (A << 8);
					break;
				case ADR_NN:
					{
						Uint32 tmp;
						FETCH(TAD);
						FETCH(tmp);
						TAD += (tmp << 8);
					}
					break;
				case ADR_BC:
					TAD = BC;
					break;
				case ADR_DE:
					TAD = DE;
					break;
				case ADR_HL:
					TAD = HL;
					break;
				case ADR_IX:
					TAD = (STATE & ST_DD) ? IX : ((STATE & ST_FD) ? IY : HL);
					break;
				case ADR_ID:
					if (STATE & ST_DD)
					{
						Uint32 tmp;
						FETCH(tmp);
						TAD = TDX = RTO16(IX + (tmp ^ 0x80) - 0x80);
					}
					else if (STATE & ST_FD)
					{
						Uint32 tmp;
						FETCH(tmp);
						TAD = TDX = RTO16(IY + (tmp ^ 0x80) - 0x80);
					}
					else
						TAD = HL;
					break;
				case ADR_N:
					FETCH(TAD);
					break;
				case ADR_C:
					TAD = C;
					break;
				case ADR_HI:
					TAD = HL;
					L = RTO8(TAD + 1);
					H = RTO8((TAD + 1) >> 8);
					break;
				case ADR_HD:
					TAD = HL;
					L = RTO8(TAD - 1);
					H = RTO8((TAD - 1) >> 8);
					break;
				case ADR_HN:
					FETCH(TAD);
					TAD += (0xff << 8);
					break;
				case ADR_HC:
					TAD = C + (0xff << 8);
					break;
				case ADR_SP:
					TAD = SP;
					break;
			}
			switch (opp->pre)
			{
				case LDO_NONE: break;
				case LDO_B:	case LDO_C:	case LDO_D:	case LDO_E:
				case LDO_H:	case LDO_L:	case LDO_A:
					TOP = context->regs8[opp->pre - LDO_B];
					break;
				case LDO_M:
					TOP = MEMREAD(TAD);
					break;
				case LDO_BC: TOP = BC;	break;
				case LDO_DE: TOP = DE;	break;
				case LDO_HL: TOP = HL;	break;
				case LDO_AF: TOP = AF;	break;
				case LDO_SP: TOP = SP;	break;
				case LDO_MM:
					TOP = MEMREAD(TAD);
					TOP += MEMREAD(RTO16(TAD + 1)) << 8;
					break;
				case LDO_N:
					FETCH(TOP);
					break;
				case LDO_NN:
					{
						Uint32 tmp;
						FETCH(TOP);
						FETCH(tmp);
						TOP += tmp << 8;
					}
					break;
				case LDO_IX:
					TOP = (STATE & ST_DD) ? IX : ((STATE & ST_FD) ? IY : HL);
					break;
				case LDO_IH:
					TOP = (STATE & ST_DD) ? IXH : ((STATE & ST_FD) ? IYH : H);
					break;
				case LDO_IL:
					TOP = (STATE & ST_DD) ? IXL : ((STATE & ST_FD) ? IYL : L);
					break;
				case LDO_I:
					TOP = I;
					F = TFL = (F & CF) + (FLAGTBL(TOP) & (SF | ZF | XF | YF)) + (IFF2 ? PF : 0);
					break;
				case LDO_R:
					TOP = (R & 0x7f) + (R7 & 0x80);
					F = TFL = (F & CF) + (FLAGTBL(TOP) & (SF | ZF | XF | YF)) + (IFF2 ? PF : 0);
					break;
				case LDO_Z: TOP = 0;	break;
				case LDO_ST:
					TOP = MEMREAD(SP);
					TOP += MEMREAD(RTO16(SP + 1)) << 8;
					SP = RTO16(SP + 2);
					break;
				case LDO_SN:
					FETCH(TOP);
					TOP = RTO16((TOP ^ 0x80) - 0x80);
					break;
				case LDO_AFDMG:
					TOP = RTO16((A << 8) + (FDMG & 0xF));
					if (F & ZF) TOP += 0x80;
					if (F & NF) TOP += 0x40;
					if (F & HF) TOP += 0x20;
					if (F & CF) TOP += 0x10;
					break;
			}
			switch (opp->op)
			{
				case OP_NOP: break;
				/* 交換命令 */
				case OP_EX_AF_AF:
					{
						Uint32 tmp;
						tmp = TOP; TOP = SAF; SAF = tmp;
					}
					break;
				case OP_EXX:
					{
						Uint32 tmp;
						tmp = BC; C = RTO8(SBC); B = RTO8(SBC >> 8); SBC = tmp;
						tmp = DE; E = RTO8(SDE); D = RTO8(SDE >> 8); SDE = tmp;
						tmp = HL; L = RTO8(SHL); H = RTO8(SHL >> 8); SHL = tmp;
					}
					break;
				case OP_EX_DE_HL:
					E = L;
					D = H;
					L = RTO8(TOP);
					H = RTO8(TOP >> 8);
					break;
				case OP_EX_MSP16_HL_:
					{
						Uint32 tmp;
						tmp = MEMREAD(SP);
						tmp += MEMREAD(RTO16(SP + 1)) << 8;
						MEMWRITE(SP, RTO8(TOP));
						MEMWRITE(RTO16(SP + 1), RTO8(TOP >> 8));
						TOP = tmp;
					}
					break;

				/* 8ビット演算命令 */
				case OP_ADD:
					TFL = 0;
					M_ADC(A, SF | ZF | XF | YF | CF);
					F = TFL;
					break;
				case OP_ADC:
					TFL = F & CF;
					M_ADC(A, SF | ZF | XF | YF | CF);
					F = TFL;
					break;
				case OP_SUB:
					TFL = CF;
					TOP ^= 0xff;
					M_ADC(A, SF | ZF | XF | YF | NF | CF);
					TFL ^= CF;
					F = TFL;
					break;
				case OP_SBC:
					TFL = ((~F) & CF);
					TOP ^= 0xff;
					M_ADC(A, SF | ZF | XF | YF | NF | CF);
					TFL ^= CF;
					F = TFL;
					break;
				case OP_AND:
					TOP = TOP & A;
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | HF | YF | PF);
					break;
				case OP_OR:
					TOP = TOP | A;
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF);
					break;
				case OP_XOR:
					TOP = TOP ^ A;
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF);
					break;
				case OP_CP:
					TFL = CF;
					TOP ^= 0xff;
					M_ADC(A, SF | ZF | NF | CF);
					TFL ^= CF;
					F = TFL + (A & (XF | YF));
					break;
				case OP_INC:
					{
						TFL = 0;
						M_ADC(1, SF | ZF | XF | YF | CF);
						F = (F & CF) + (TFL & ~CF);
					}
					break;
				case OP_DEC:
					{
						Uint32 decsrc;
						TFL = CF;
						decsrc = TOP;
						TOP = 1 ^ 0xff;
						M_ADC(decsrc, SF | ZF | XF | YF | NF | CF);
						TFL ^= CF;
						F = (F & CF) + (TFL & ~CF);
					}
					break;
				case OP_DAA:
					/* HD64180に暗黙のキャリーフラグは存在しない */
					if (!ICEXIST) TFL = F;
					if (F & NF)
					{
						Uint32 ic;
						ic = TFL & CF;	/* 暗黙のキャリーフラグ */
						TFL = NF;
						if ((F & HF) || ((TOP & 0x0f) > 0x09))
						{
							TOP = (TOP + 0x100 - 0x006) ^ 0x100;
							TFL += HF;
						}
						if (ic || TOP > 0x99)
						{
							TOP = (TOP + 0x100 - 0x060) ^ 0x100;
							TFL += CF;
						}
					}
					else
					{
						Uint32 ic;
						ic = TFL & CF;	/* 暗黙のキャリーフラグ */
						TFL = 0;
						if ((F & HF) || ((TOP & 0x0f) > 0x09))
						{
							TOP += 0x06;
							TFL += HF;
						}
						if (ic || TOP > 0x99)
						{
							TOP += 0x60;
							TFL += CF;
						}
					}
					F = TFL + (FLAGTBL(TOP) & (SF | ZF | XF | YF | PF));
					break;
				case OP_CPL:
					TOP ^= 0xff;
					F = TFL = (F & (SF | ZF | PF | CF)) + (FLAGTBL(TOP) & (XF | HF | YF | NF));
					break;
				case OP_NEG:
					TFL = CF;
					TOP ^= 0xff;
					A = 0;
					M_ADC(A, SF | ZF | XF | YF | NF | CF);
					TFL ^= CF;
					F = TFL;
					break;
				/* 16ビット演算命令 */
				case OP_ADD16:
					TFL = 0;
					M_ADC16(XF | YF | CF);
					F = TFL = (F & (SF | ZF | PF)) + (TFL & (XF | HF | YF | NF | CF));
					break;
				case OP_ADC16:
					TFL = F & CF;
					M_ADC16(SF | XF | YF | CF);
					F = TFL = TFL + (RTO16(TOP) ? 0 : ZF);
					break;
				case OP_SBC16:
					TFL = ((~F) & CF);
					TOP ^= 0xffff;
					M_ADC16(SF | XF | YF | NF | CF);
					TFL ^= CF;
					F = TFL = TFL + (RTO16(TOP) ? 0 : ZF);
					break;
				case OP_INC16:
					TOP = RTO16(TOP + 1);
					break;
				case OP_DEC16:
					TOP = RTO16(TOP - 1);
					break;

				/* ブロック転送命令 */
				case OP_LDI:
					M_LDX(+1);
					break;
				case OP_LDD:
					M_LDX(-1);
					break;
				case OP_CPI:
					M_CPX(+1);
					break;
				case OP_CPD:
					M_CPX(-1);
					break;

				/* 分岐命令 */
				case OP_JP:
					TPC = TOP;
					break;
				case OP_JPCC:
					if (!CHECKCC((op0 >> 3) & 3)) break;
					CYCLE += CYT[0x506];
					TPC = TOP;
					break;
				case OP_JPCX:
					if (!CHECKCX((op0 >> 3) & 3)) break;
					CYCLE += CYT[0x506];
					TPC = TOP;
					break;
				case OP_JR:
					TPC = TDX = RTO16(TPC + (TOP ^ 0x80) - 0x80);
					break;
				case OP_JRCC:
					if (!CHECKCC((op0 >> 3) & 3)) break;
					CYCLE += CYT[0x504];
					TPC = TDX = RTO16(TPC + (TOP ^ 0x80) - 0x80);
					break;
				case OP_DJNZ:
					B = RTO8(B - 1);
					if (!B) break;
					CYCLE += CYT[0x500];
					TPC = TDX = RTO16(TPC + (TOP ^ 0x80) - 0x80);
					break;
				case OP_CALL:
					SP = RTO16(SP - 2);
					MEMWRITE(SP, RTO8(TPC));
					MEMWRITE(RTO16(SP + 1), RTO8(TPC >> 8));
					TPC = TOP;
					break;
				case OP_CALLCC:
					if (!CHECKCC((op0 >> 3) & 3)) break;
					CYCLE += CYT[0x502];
					SP = RTO16(SP - 2);
					MEMWRITE(SP, RTO8(TPC));
					MEMWRITE(RTO16(SP + 1), RTO8(TPC >> 8));
					TPC = TOP;
					break;
				case OP_CALLCX:
					if (!CHECKCX((op0 >> 3) & 3)) break;
					CYCLE += CYT[0x502];
					SP = RTO16(SP - 2);
					MEMWRITE(SP, RTO8(TPC));
					MEMWRITE(RTO16(SP + 1), RTO8(TPC >> 8));
					TPC = TOP;
					break;
				case OP_RETCC:
					if (!CHECKCC((op0 >> 3) & 3)) break;
					CYCLE += CYT[0x508];
					TPC = MEMREAD(SP);
					TPC += MEMREAD(RTO16(SP + 1)) << 8;
					SP = RTO16(SP + 2);
					break;
				case OP_RETCX:
					if (!CHECKCX((op0 >> 3) & 3)) break;
					CYCLE += CYT[0x508];
					TPC = MEMREAD(SP);
					TPC += MEMREAD(RTO16(SP + 1)) << 8;
					SP = RTO16(SP + 2);
					break;
				case OP_RETI:
				case OP_RETN:
					TPC = TOP;
					IFF1 = IFF2;
					break;
				case OP_RST:
					SP = RTO16(SP - 2);
					MEMWRITE(SP, RTO8(TPC));
					MEMWRITE(RTO16(SP + 1), RTO8(TPC >> 8));
					TPC = RSTBASE + (op0 & 0x38);
					break;

				/* ビット循環命令*/
				case OP_RLCA:
					TOP = (TOP << 1) + ((TOP >> 7) & 1);
					F = TFL = (F & (SF | ZF | HF | PF)) + (FLAGTBL(TOP) & (XF | YF | CF));
					break;
				case OP_RRCA:
					TOP = (TOP >> 1) + ((TOP & 1) << 7) + ((TOP & 1) << 8);
					F = TFL = (F & (SF | ZF | HF | PF)) + (FLAGTBL(TOP) & (XF | YF | CF));
					break;
				case OP_RLA:
					TOP = (TOP << 1) + (F & 1);
					F = TFL = (F & (SF | ZF | HF | PF)) + (FLAGTBL(TOP) & (XF | YF | CF));
					break;
				case OP_RRA:
					TOP = (TOP >> 1) + ((F & 1) << 7) + ((TOP & 1) << 8);
					F = TFL = (F & (SF | ZF | HF | PF)) + (FLAGTBL(TOP) & (XF | YF | CF));
					break;
				case OP_RLC:
					TOP = (TOP << 1) + ((TOP >> 7) & 1);
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF | CF);
					break;
				case OP_RRC:
					TOP = (TOP >> 1) + ((TOP & 1) << 7) + ((TOP & 1) << 8);
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF | CF);
					break;
				case OP_RL:
					TOP = (TOP << 1) + (F & 1);
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF | CF);
					break;
				case OP_RR:
					TOP = (TOP >> 1) + ((F & 1) << 7) + ((TOP & 1) << 8);
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF | CF);
					break;
				case OP_SLA:
					TOP = (TOP << 1);
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF | CF);
					break;
				case OP_SRA:
					TOP = (TOP >> 1) + (TOP & 0x80) + ((TOP & 1) << 8);
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF | CF);
					break;
				case OP_SLL:
					TOP = (TOP << 1) + 1;
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF | CF);
					break;
				case OP_SRL:
					TOP = (TOP >> 1) + ((TOP & 1) << 8);
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | YF | PF | CF);
					break;
				case OP_RLD:
					TOP = (TOP << 4) + (A & 0x0f);
					A = RTO8((A & 0xf0) + ((TOP >> 8) & 0x0f));
					TOP = RTO8(TOP);
					F = TFL = (F & CF) + (FLAGTBL(A) & (SF | ZF | XF | YF | PF));
					break;
				case OP_RRD:
					TOP = TOP + (A << 8);
					A = RTO8((A & 0xf0) + (TOP & 0x0f));
					TOP = RTO8(TOP >> 4);
					F = TFL = (F & CF) + (FLAGTBL(A) & (SF | ZF | XF | YF | PF));
					break;
				/* ビット操作命令*/
				case OP_BIT:
					{
						Uint32 tmp;
						tmp = TOP & (1 << ((op0 >> 3) & 7));
						TFL = (F & CF) + (tmp ? 0 : (ZF | PF));
						if (opp->pre == LDO_M)
							F = TFL + (tmp & SF) + ((TDX >> 8) & (XF | YF));
						else
							F = TFL + (tmp & (SF | XF | YF));
					}
					break;
				case OP_RES:
					{
						TOP &= ~(1 << ((op0 >> 3) & 7));
					}
					break;
				case OP_SET:
					{
						TOP |= 1 << ((op0 >> 3) & 7);
					}
					break;
				/* 入出力命令 */
				case OP_IN:
					CYCLEIO;
					TOP = IOREAD(TAD);
					if (opp->adr != ADR_AN)
						F = TFL = (F & CF) + (FLAGTBL(TOP) & (SF | ZF | XF | YF | PF));
					break;
				case OP_OUT:
					CYCLEIO;
					IOWRITE(TAD, TOP);
					break;

				/* ブロック入出力命令 */
				case OP_INI:
					M_INX(0);
					M_POSTIOX(+1);
					break;
				case OP_IND:
					M_INX(0);
					M_POSTIOX(-1);
					break;
				case OP_OUTI:
					M_OUTX(0);
					M_POSTIOX(+1);
					break;
				case OP_OUTD:
					M_OUTX(0);
					M_POSTIOX(-1);
					break;

				/* CPU制御命令 */
				case OP_CCF:
					F = TFL = ((F & (SF | ZF | PF | CF)) + (A & (XF | YF)) + ((F << 4) & HF)) ^ CF;
					break;
				case OP_SCF:
					F = TFL = (F & (SF | ZF | PF)) + (A & (XF | YF)) + CF;
					break;
				case OP_HALT:
					TPC = PC;
					HALTED = 1;
					break;
				case OP_HALT2:
					TPC = PC;
					HALTED = 2;
					break;
				case OP_DI:
					IFF1 = IFF2 = 0;
					PC = TPC;
					STATE = ST_LOCKINT;
					M_DECODE(OPT, 0);
					continue;
				case OP_EI:
					IFF1 = IFF2 = 1;
					PC = TPC;
					STATE = ST_LOCKINT;
					M_DECODE(OPT, 0);
					continue;
				case OP_IM0:
					if (IMODE < 3) IMODE = 0;
					break;
				case OP_IM1:
					if (IMODE < 3) IMODE = 1;
					break;
				case OP_IM2:
					if (IMODE < 3) IMODE = 2;
					break;
				case OP_PREFIX_CB:
					if (STATE & ST_DD)
					{
						Uint32 tmp;
						FETCH(tmp);
						TAD = TDX = RTO16(IX + (tmp ^ 0x80) - 0x80);
						FETCH(op0);
						CYCLE += CYT[0x300 + op0];
						DEBUG_OUTPUT(op0);
						opcb.adr = ADR_NONE;
						opcb.pre = LDO_M;
						opcb.op = OPTCB[((op0 >> 3) & 0x1f)];
						opcb.post = (opcb.op == OP_BIT) ? STO_NONE : ((op0 & 7) + STO_B);
						if (opcb.post && opcb.post != STO_M) STATE |= ST_CB;
					}
					else if (STATE & ST_FD)
					{
						Uint32 tmp;
						FETCH(tmp);
						TAD = TDX = RTO16(IY + (tmp ^ 0x80) - 0x80);
						FETCH(op0);
						CYCLE += CYT[0x300 + op0];
						DEBUG_OUTPUT(op0);
						opcb.adr = ADR_NONE;
						opcb.pre = LDO_M;
						opcb.op = OPTCB[((op0 >> 3) & 0x1f)];
						opcb.post = (opcb.op == OP_BIT) ? STO_NONE : ((op0 & 7) + STO_B);
						if (opcb.post && opcb.post != STO_M) STATE |= ST_CB;
					}
					else
					{
						TAD = HL;
						CYCLEM1;
						FETCH(op0);
						CYCLE += CYT[0x200 + op0];
						DEBUG_OUTPUT(op0);
						opcb.adr = ADR_NONE;
						opcb.pre = (op0 & 7) + LDO_B;
						opcb.op = OPTCB[((op0 >> 3) & 31)];
						opcb.post = (opcb.op == OP_BIT) ? STO_NONE : ((op0 & 7) + STO_B);
					}
					opp = &opcb;
					STATE |= ST_LOCKINT;
					continue;
				case OP_PREFIX_DD:
					PC = RTO16(TPC - 1);
					STATE = ST_DD + ST_LOCKINT;
					M_DECODE(OPT, 0x100);
					continue;
				case OP_PREFIX_ED:
					PC = RTO16(TPC - 1);
					STATE = ST_LOCKINT;
					M_DECODE(OPTED, 0x400);
					continue;
				case OP_PREFIX_FD:
					PC = RTO16(TPC - 1);
					STATE = ST_FD | ST_LOCKINT;
					M_DECODE(OPT, 0x100);
					continue;
				/* maratZ80命令 */
				case OP_PATCH:
					if (PATCHEDFE) PATCHEDFE(context->user, context);
					break;
				/* HD64180命令 */
				case OP_TST:
					TOP = TOP & A;
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | HF | YF | PF);
					break;
				case OP_TSTIO:
					CYCLEIO;
					TOP = TOP & IOREAD(TAD);
					F = TFL = FLAGTBL(TOP) & (SF | ZF | XF | HF | YF | PF);
					break;
				case OP_OTIM:
					M_OTXM(0);
					M_POSTOTXM(+1);
					break;
				case OP_OTDM:
					M_OTXM(0);
					M_POSTOTXM(-1);
					break;
				/* HD64180乗算命令 */
				case OP_MLT:
					TOP = RTO8(TOP) * RTO8(TOP >> 8);
					break;
				/* R800乗算命令 */
				case OP_MULUB:
					TOP = TOP * A;
					F = TFL = (TOP > 0xFF) + ((TOP == 0) << 6) + (F & (HF | PF));
					break;
				case OP_MULUW:
					TOP = TOP * TAD;
					F = TFL = (TOP > 0xFFFF) + ((TOP == 0) << 6) + (F & (HF | PF));
					break;
				/* DMG命令 */
				case OP_SWAP:
					TOP = RTO8(((TOP >> 4) & 0xf) + (TOP << 4));
					F = TFL = TOP ? 0 : ZF;
					break;
#if 0
				case OP_ADDSP:
					TOP = RTO16(TOP + TAD);
					break;
#endif
			}
			DEBUG_OUTPUT(-10000);
			switch (opp->post)
			{
				case STO_NONE: break;
				case STO_B:	case STO_C:	case STO_D:	case STO_E:
				case STO_H: case STO_L: case STO_A:
					context->regs8[opp->post - STO_B] = RTO8(TOP);
					break;
				case STO_M:
					MEMWRITE(TAD, TOP);
					break;
				case STO_BC: C = RTO8(TOP);	B = RTO8(TOP >> 8);	break;
				case STO_DE: E = RTO8(TOP);	D = RTO8(TOP >> 8);	break;
				case STO_HL: L = RTO8(TOP);	H = RTO8(TOP >> 8);	break;
				case STO_AF: F = RTO8(TOP);	A = RTO8(TOP >> 8);	break;
				case STO_SP: SP = RTO16(TOP);	break;
				case STO_MM:
					MEMWRITE(TAD, RTO8(TOP));
					MEMWRITE(RTO16(TAD + 1), RTO8(TOP >> 8));
					break;
				case STO_IX:
					if (STATE & ST_DD)
					{
						IXL = RTO8(TOP);	IXH = RTO8(TOP >> 8);
					}
					else if (STATE & ST_FD)
					{
						IYL = RTO8(TOP);	IYH = RTO8(TOP >> 8);
					}
					else
					{
						L = RTO8(TOP);	H = RTO8(TOP >> 8);
					}
					break;
				case STO_IH:
					if (STATE & ST_DD)
					{
						IXH = RTO8(TOP);
					}
					else if (STATE & ST_FD)
					{
						IYH = RTO8(TOP);
					}
					else
					{
						H = RTO8(TOP);
					}
					break;
				case STO_IL:
					if (STATE & ST_DD)
					{
						IXL = RTO8(TOP);
					}
					else if (STATE & ST_FD)
					{
						IYL = RTO8(TOP);
					}
					else
					{
						L = RTO8(TOP);
					}
					break;
				case STO_I: I = RTO8(TOP);	break;
				case STO_R:
					R = RTO8(TOP & 0x7f);
					R7 = RTO8(TOP & 0x80);
					break;
				case STO_R0:
					if (B)
					{
						if (NMIREQ || (IFF1 && INTREQ)) break;
						DEBUG_OUTPUT(-1000);
						TPC = PC;
					}
					else
					{
						F = TFL = (F & (XF | NF | YF)) + ZF + PF;
					}
					break;
				case STO_R1:
					if (B)
					{
						DEBUG_OUTPUT(-1000);
						TPC = PC;
						CYCLE += CYT[0x50A];
					}
					break;
				case STO_R2:
					if (BC)
					{
						DEBUG_OUTPUT(-1000);
						TPC = PC;
						CYCLE += CYT[0x50A];
					}
					break;
				case STO_R3:
					if (BC && !(F & ZF))
					{
						DEBUG_OUTPUT(-1000);
						TPC = PC;
						CYCLE += CYT[0x50A];
					}
					break;
				case STO_ST:
					SP = RTO16(SP - 2);
					MEMWRITE(SP, RTO8(TOP));
					MEMWRITE(RTO16(SP + 1), RTO8(TOP >> 8));
					break;
				case STO_AFDMG:
					 FDMG = RTO8(TOP & 0x0f);
					 A = RTO8(TOP >> 8);
					 TFL = 0;
					 if (TOP & 0x80) TFL += ZF;
					 if (TOP & 0x40) TFL += NF;
					 if (TOP & 0x20) TFL += HF;
					 if (TOP & 0x10) TFL += CF;
					 F = TFL;
					break;
			}
			if (STATE & (ST_CB))
			{
				STATE &= ~ST_CB;
				MEMWRITE(TAD, RTO8(TOP));
			}
			PC = TPC;
		} while (STATE & ST_LOCKINT);
		/* ユーザー定義イベントタイマ実行 */
		if (context->kmevent)
		{
#if 1
			/* HALT時は次のイベントまで一気に進める */
			if (HALTED && !NMIREQ && !INTREQ)
			{
				Uint32 nextcount;
				if (kmevent_gettimer(context->kmevent, 0, &nextcount))
				{
					/* イベント有り */
					if (CYCLE + nextcount < cycles)
						CYCLE += nextcount;
					else
						CYCLE = cycles;
				}
				else
				{
					/* イベント無し */
					CYCLE = cycles;
				}
			}
#endif
			kmevent_process(context->kmevent, CYCLE - kmecycle);
		}
		kmecycle = CYCLE;
		/* 割り込み受付 */
		if (NMIREQ)
		{
			if (HALTED)
			{
				PC = RTO16(PC + HALTED);
				HALTED = 0;
			}
			NMIREQ &= ~1;
			IFF2 = IFF1;
			IFF1 = 0;
			SP = RTO16(SP - 2);
			MEMWRITE(SP, RTO8(TOP));
			MEMWRITE(RTO16(SP + 1), RTO8(TOP >> 8));
			PC = 0x66;
			/*  M1cycle + RSTcost */
			CYCLEM1;
			CYCLE += MEMCYCLE + CYT[0xFF];
		}
		else if (IFF1 && INTREQ)
		{
			if (HALTED)
			{
				PC = RTO16(PC + HALTED);
				HALTED = 0;
			}
			switch (IMODE)
			{
				case 0:	/* Z80 IM 0 */
					if (AUTOIRQCLEAR) INTREQ &= ~1;
					IFF2 = IFF1 = 0;
					SYSMEMFETCH = kmz80_fetch_im0;
					CYCLE += 2;
					break;
				case 1:	/* Z80 IM 1 */
					if (AUTOIRQCLEAR) INTREQ &= ~1;
					IFF2 = IFF1 = 0;
					SP = RTO16(SP - 2);
					MEMWRITE(SP, RTO8(PC));
					MEMWRITE(RTO16(SP + 1), RTO8(PC >> 8));
					PC = 0x38;
					/*  M1cycle + RSTcost + 2 */
					CYCLEM1;
					CYCLE += MEMCYCLE + CYT[0xFF] + 2;
					break;
				case 2:	/* Z80 IM 2 */
					if (AUTOIRQCLEAR) INTREQ &= ~1;
					IFF2 = IFF1 = 0;
					SP = RTO16(SP - 2);
					MEMWRITE(SP, RTO8(PC));
					MEMWRITE(RTO16(SP + 1), RTO8(PC >> 8));
					TAD = (I << 8) + BUSREAD(2);
					PC = MEMREAD(TAD);
					PC += MEMREAD(RTO16(TAD + 1)) << 8;
					/*  M1cycle + RSTcost + 2 */
					CYCLEM1;
					CYCLE += MEMCYCLE + CYT[0xFF] + 2;
					break;
				case 3:	/* DMG */
					IFF2 = IFF1 = 0;
					SP = RTO16(SP - 2);
					MEMWRITE(SP, RTO8(PC));
					MEMWRITE(RTO16(SP + 1), RTO8(PC >> 8));
					{
						Uint32 i;
						for (i = 0; i < 5; i++)
						{
							if (INTREQ & (1 << i))
							{
								if (AUTOIRQCLEAR) INTREQ &= ~(1 << i);
								PC = 0x40 + (i << 3);
								break;
							}
						}
					}
					break;
				case 4:	/* VECTOR INT for special */
					IFF2 = IFF1 = 0;
				case 5:	/* VECTOR CALL for special */
					SP = RTO16(SP - 2);
					MEMWRITE(SP, RTO8(PC));
					MEMWRITE(RTO16(SP + 1), RTO8(PC >> 8));
				case 6:	/* VECTOR JP for special */
					{
						Uint32 i;
						for (i = 0; i < 5; i++)
						{
							if (INTREQ & (1 << i))
							{
								if (AUTOIRQCLEAR) INTREQ &= ~(1 << i);
								PC = VECTOR[i];
								break;
							}
						}
					}
					break;
			}
		}
	}
	kmecycle = CYCLE;
	CYCLE = 0;
	return kmecycle;
}
