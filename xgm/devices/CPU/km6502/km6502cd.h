static void Inline KI_ADDCLOCK(__CONTEXT_ Uword cycle)
{
#if BUILD_HUC6280
	if (__THIS__.lowClockMode)
	{
#if 0
		cycle += (cycle << 2);	/* x5 */
#else
		cycle += cycle + cycle;	/*    */
		cycle += cycle;			/* x6 */
#endif
	}
	__THIS__.clock += cycle;
#else
	__THIS__.clock += cycle;
#endif
}
static Uword Inline KI_READWORD(__CONTEXT_ Uword adr)
{
	Uword ret = K_READNP(__THISP_ adr);
	return ret + (K_READNP(__THISP_ (Uword)(RTO16(adr + 1))) << 8);
}
static Uword Inline KI_READWORDZP(__CONTEXT_ Uword adr)
{
	Uword ret = K_READZP(__THISP_ (Uword)(BASE_OF_ZERO + adr));
	return ret + (K_READZP(__THISP_ (Uword)(BASE_OF_ZERO + RTO8(adr + 1))) << 8);
}

static Uword Inline KAI_IMM(__CONTEXT)
{
	Uword ret = __THIS__.PC;
	__THIS__.PC = RTO16(__THIS__.PC + 1);
	return ret;
}
static Uword Inline KAI_IMM16(__CONTEXT)
{
	Uword ret = __THIS__.PC;
	__THIS__.PC = RTO16(__THIS__.PC + 2);
	return ret;
}
static Uword Inline KAI_ABS(__CONTEXT)
{
	return KI_READWORD(__THISP_ KAI_IMM16(__THISP));
}
static Uword Inline KAI_ABSX(__CONTEXT)
{
	return RTO16(KAI_ABS(__THISP) + __THIS__.X);
}
static Uword Inline KAI_ABSY(__CONTEXT)
{
	return RTO16(KAI_ABS(__THISP) + __THIS__.Y);
}
static Uword Inline KAI_ZP(__CONTEXT)
{
	return K_READNP(__THISP_ KAI_IMM(__THISP));
}
static Uword Inline KAI_ZPX(__CONTEXT)
{
	return RTO8(KAI_ZP(__THISP) + __THIS__.X);
}
static Uword Inline KAI_INDY(__CONTEXT)
{
	return RTO16(KI_READWORDZP(__THISP_ KAI_ZP(__THISP)) + __THIS__.Y);
}

static Uword MasubCall KA_IMM(__CONTEXT)
{
	Uword ret = __THIS__.PC;
	__THIS__.PC = RTO16(__THIS__.PC + 1);
	return ret;
}
static Uword MasubCall KA_IMM16(__CONTEXT)
{
	Uword ret = __THIS__.PC;
	__THIS__.PC = RTO16(__THIS__.PC + 2);
	return ret;
}
static Uword MasubCall KA_ABS(__CONTEXT)
{
	return KI_READWORD(__THISP_ KAI_IMM16(__THISP));
}
static Uword MasubCall KA_ABSX(__CONTEXT)
{
	return RTO16(KAI_ABS(__THISP) + __THIS__.X);
}
static Uword MasubCall KA_ABSY(__CONTEXT)
{
	return RTO16(KAI_ABS(__THISP) + __THIS__.Y);
}
static Uword MasubCall KA_ZP(__CONTEXT)
{
	return BASE_OF_ZERO + K_READNP(__THISP_ KAI_IMM(__THISP));
}
static Uword MasubCall KA_ZPX(__CONTEXT)
{
	return BASE_OF_ZERO + RTO8(KAI_ZP(__THISP) + __THIS__.X);
}
static Uword MasubCall KA_ZPY(__CONTEXT)
{
	return BASE_OF_ZERO + RTO8(KAI_ZP(__THISP) + __THIS__.Y);
}
static Uword MasubCall KA_INDX(__CONTEXT)
{
	return KI_READWORDZP(__THISP_ KAI_ZPX(__THISP));
}
static Uword MasubCall KA_INDY(__CONTEXT)
{
	return RTO16(KI_READWORDZP(__THISP_ KAI_ZP(__THISP)) + __THIS__.Y);
}

#if BUILD_HUC6280 || BUILD_M65C02
static Uword MasubCall KA_IND(__CONTEXT)
{
	return KI_READWORDZP(__THISP_ KAI_ZP(__THISP));
}
#else
static Uword Inline KI_READWORDBUG(__CONTEXT_ Uword adr)
{
	Uword ret = K_READNP(__THISP_ adr);
	return ret + (K_READNP(__THISP_ (Uword)((adr & 0xFF00) + RTO8(adr + 1))) << 8);
}
#endif

#if BUILD_HUC6280
#define KA_ABSX_ KA_ABSX
#define KA_ABSY_ KA_ABSY
#define KA_INDY_ KA_INDY
#else
static Uword MasubCall KA_ABSX_(__CONTEXT)
{
	if (RTO8(__THIS__.PC) == 0xFF) KI_ADDCLOCK(__THISP_ 1);	/* page break */
	return KAI_ABSX(__THISP);
}
static Uword MasubCall KA_ABSY_(__CONTEXT)
{
	if (RTO8(__THIS__.PC) == 0xFF) KI_ADDCLOCK(__THISP_ 1);	/* page break */
	return KAI_ABSY(__THISP);
}
static Uword MasubCall KA_INDY_(__CONTEXT)
{
	Uword adr = KAI_INDY(__THISP);
	if (RTO8(adr) == 0xFF) KI_ADDCLOCK(__THISP_ 1);	/* page break */
	return adr;
}
#endif

static void OpsubCall KM_ALUADDER(__CONTEXT_ Uword src)
{
	Uword w = __THIS__.A + src + (__THIS__.P & C_FLAG);
	__THIS__.P &= ~(N_FLAG | V_FLAG | Z_FLAG | C_FLAG);
	__THIS__.P += FLAG_NZC(w)
		+ ((((~__THIS__.A ^ src) & (__THIS__.A ^ w)) >> 1) & V_FLAG);
	__THIS__.A = RTO8(w);
}

#if 0
static void OpsubCall KM_ALUADDER_S(__CONTEXT_ Uword src)
{
	Uword w = __THIS__.A + src + (__THIS__.P & C_FLAG);
	__THIS__.P &= ~(N_FLAG | V_FLAG | Z_FLAG | C_FLAG);
	__THIS__.P += FLAG_NZC(w)
		+ (((__THIS__.A ^ ~src) & (__THIS__.A ^ w)) & V_FLAG);
	__THIS__.A = RTO8(w);
}
#endif

static void OpsubCall KM_ALUADDER_D(__CONTEXT_ Uword src)
{
	Uword wl = (__THIS__.A & 0x0F) + (src & 0x0F) + (__THIS__.P & C_FLAG);
	Uword w = __THIS__.A + src + (__THIS__.P & C_FLAG);
#if BUILD_HUC6280 || BUILD_M65C02
	__THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
#else
	__THIS__.P &= ~C_FLAG;
#endif
	if (wl > 0x9) w += 0x6;
	if (w > 0x9F)
	{
		__THIS__.P += C_FLAG;
		w += 0x60;
	}
#if BUILD_HUC6280 || BUILD_M65C02
	__THIS__.P += FLAG_NZ(w);
#endif
	__THIS__.A = RTO8(w);
	 KI_ADDCLOCK(__THISP_ 1);
}

static void Inline KMI_ADC(__CONTEXT_ Uword src)
{
	KM_ALUADDER(__THISP_ src);
}
static void Inline KMI_ADC_D(__CONTEXT_ Uword src)
{
	KM_ALUADDER_D(__THISP_ src);
}
static void Inline KMI_SBC(__CONTEXT_ Uword src)
{
	KM_ALUADDER(__THISP_ src ^ 0xFF);
}
static void Inline KMI_SBC_D(__CONTEXT_ Uword src)
{
	KM_ALUADDER_D(__THISP_ RTO8((src ^ 0xFF) + (0x100 - 0x66)));
}

static void OpsubCall KM_CMP(__CONTEXT_ Uword src)
{
	Uword w = __THIS__.A + (src ^ 0xFF) + 1;
	__THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
	__THIS__.P += FLAG_NZC(w);
}
static void OpsubCall KM_CPX(__CONTEXT_ Uword src)
{
	Uword w = __THIS__.X + (src ^ 0xFF) + 1;
	__THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
	__THIS__.P += FLAG_NZC(w);
}
static void OpsubCall KM_CPY(__CONTEXT_ Uword src)
{
	Uword w = __THIS__.Y + (src ^ 0xFF) + 1;
	__THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
	__THIS__.P += FLAG_NZC(w);
}

static void OpsubCall KM_BIT(__CONTEXT_ Uword src)
{
	Uword w = __THIS__.A & src;
	__THIS__.P &= ~(N_FLAG | V_FLAG | Z_FLAG);
	__THIS__.P += (src & (N_FLAG | V_FLAG)) + (w ? 0 : Z_FLAG);
}

static void OpsubCall KM_AND(__CONTEXT_ Uword src)
{
	__THIS__.A &= src;
	__THIS__.P &= ~(N_FLAG | Z_FLAG);
	__THIS__.P += FLAG_NZ(__THIS__.A);
}

static void OpsubCall KM_ORA(__CONTEXT_ Uword src)
{
	__THIS__.A |= src;
	__THIS__.P &= ~(N_FLAG | Z_FLAG);
	__THIS__.P += FLAG_NZ(__THIS__.A);
}

static void OpsubCall KM_EOR(__CONTEXT_ Uword src)
{
	__THIS__.A ^= src;
	__THIS__.P &= ~(N_FLAG | Z_FLAG);
	__THIS__.P += FLAG_NZ(__THIS__.A);
}

static Uword OpsubCall KM_DEC(__CONTEXT_ Uword des)
{
	Uword w = des - 1;
	__THIS__.P &= ~(N_FLAG | Z_FLAG);
	__THIS__.P += FLAG_NZ(w);
	return RTO8(w);
}

static Uword OpsubCall KM_INC(__CONTEXT_ Uword des)
{
	Uword w = des + 1;
	__THIS__.P &= ~(N_FLAG | Z_FLAG);
	__THIS__.P += FLAG_NZ(w);
	return RTO8(w);
}

static Uword OpsubCall KM_ASL(__CONTEXT_ Uword des)
{
	Uword w = des << 1;
	__THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
	__THIS__.P += FLAG_NZ(w) + ((des >> 7)/* & C_FLAG*/);
	return RTO8(w);
}

static Uword OpsubCall KM_LSR(__CONTEXT_ Uword des)
{
	Uword w = des >> 1;
	__THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
	__THIS__.P += FLAG_NZ(w) + (des & C_FLAG);
	return w;
}

static Uword OpsubCall KM_LD(__CONTEXT_ Uword src)
{
	__THIS__.P &= ~(N_FLAG | Z_FLAG);
	__THIS__.P += FLAG_NZ(src);
	return src;
}


static Uword OpsubCall KM_ROL(__CONTEXT_ Uword des)
{
	Uword w = (des << 1) + (__THIS__.P & C_FLAG);
	__THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
	__THIS__.P += FLAG_NZ(w) + ((des >> 7)/* & C_FLAG*/);
	return RTO8(w);
}

static Uword OpsubCall KM_ROR(__CONTEXT_ Uword des)
{
	Uword w = (des >> 1) + ((__THIS__.P & C_FLAG) << 7);
	__THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
	__THIS__.P += FLAG_NZ(w) + (des & C_FLAG);
	return RTO8(w);
}

static void OpsubCall KM_BRA(__CONTEXT_ Uword rel)
{
#if BUILD_HUC6280
	__THIS__.PC = RTO16(__THIS__.PC + (rel ^ 0x80) - 0x80);
	KI_ADDCLOCK(__THISP_ 2);
#else
	Uword oldPage = __THIS__.PC & 0xFF00;
	__THIS__.PC = RTO16(__THIS__.PC + (rel ^ 0x80) - 0x80);
	 KI_ADDCLOCK(__THISP_ (Uword)(1 + (oldPage != (__THIS__.PC & 0xFF00))));
#endif
}

static void OpsubCall KM_PUSH(__CONTEXT_ Uword src)
{
	K_WRITENP(__THISP_ (Uword)(BASE_OF_ZERO + 0x100 + __THIS__.S), src);
	__THIS__.S = RTO8(__THIS__.S - 1);
}
static Uword OpsubCall KM_POP(__CONTEXT)
{
	__THIS__.S = RTO8(__THIS__.S + 1);
	return K_READNP(__THISP_ (Uword)(BASE_OF_ZERO + 0x100  + __THIS__.S));
}
#if BUILD_HUC6280 || BUILD_M65C02
static Uword OpsubCall KM_TSB(__CONTEXT_ Uword mem)
{
	Uword w = __THIS__.A | mem;
	__THIS__.P &= ~(N_FLAG | V_FLAG | Z_FLAG);
	__THIS__.P += (mem & (N_FLAG | V_FLAG)) + (w ? 0 : Z_FLAG);
	return w;
}
static Uword OpsubCall KM_TRB(__CONTEXT_ Uword mem)
{
	Uword w = (__THIS__.A ^ 0xFF) & mem;
	__THIS__.P &= ~(N_FLAG | V_FLAG | Z_FLAG);
	__THIS__.P += (mem & (N_FLAG | V_FLAG)) + (w ? 0 : Z_FLAG);
	return w;
}
#endif
#if BUILD_HUC6280
static Uword Inline KMI_PRET(__CONTEXT)
{
	Uword saveA = __THIS__.A;
	__THIS__.A = K_READZP(__THISP_ (Uword)(BASE_OF_ZERO + __THIS__.X));
	return saveA;
}
static void Inline KMI_POSTT(__CONTEXT_ Uword saveA)
{
	K_WRITEZP(__THISP_ (Uword)(BASE_OF_ZERO + __THIS__.X), __THIS__.A);
	__THIS__.A = saveA;
	 KI_ADDCLOCK(__THISP_ 3);
}
static void OpsubCall KM_TST(__CONTEXT_ Uword imm, Uword mem)
{
	Uword w = imm & mem;
	__THIS__.P &= ~(N_FLAG | V_FLAG | Z_FLAG);
	__THIS__.P += (mem & (N_FLAG | V_FLAG)) + (w ? 0 : Z_FLAG);
}
#endif

/* --- ADC ---  */
#if BUILD_HUC6280
#define DEF_ADC(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KMI_ADC(__THISP_ K_READ##p##(__THISP_ a(__THISP))); } \
static void OpcodeCall D_Opco##i##(__CONTEXT) \
{ KMI_ADC_D(__THISP_ K_READ##p##(__THISP_ a(__THISP))); } \
static void OpcodeCall T_Opco##i##(__CONTEXT) \
{ \
	Uword saveA = KMI_PRET(__THISP); \
	KMI_ADC(__THISP_ K_READ##p##(__THISP_ a(__THISP))); \
	KMI_POSTT(__THISP_ saveA); \
} \
static void OpcodeCall TD_Opc##i##(__CONTEXT) \
{ \
	Uword saveA = KMI_PRET(__THISP); \
	KMI_ADC_D(__THISP_ K_READ##p##(__THISP_ a(__THISP))); \
	KMI_POSTT(__THISP_ saveA); \
}
#else
#define DEF_ADC(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KMI_ADC(__THISP_ K_READ##p##(__THISP_ a(__THISP))); } \
static void OpcodeCall D_Opco##i##(__CONTEXT) \
{ KMI_ADC_D(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
#endif
DEF_ADC(61,NP,KA_INDX)	/* 61 - ADC - (Indirect,X) */
DEF_ADC(65,ZP,KA_ZP)	/* 65 - ADC - Zero Page */
DEF_ADC(69,NP,KA_IMM)	/* 69 - ADC - Immediate */
DEF_ADC(6D,NP,KA_ABS)	/* 6D - ADC - Absolute */
DEF_ADC(71,NP,KA_INDY_)	/* 71 - ADC - (Indirect),Y */
DEF_ADC(75,ZP,KA_ZPX)	/* 75 - ADC - Zero Page,X */
DEF_ADC(79,NP,KA_ABSY_)	/* 79 - ADC - Absolute,Y */
DEF_ADC(7D,NP,KA_ABSX_)	/* 7D - ADC - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_ADC(72,NP,KA_IND)	/* 72 - ADC - (Indirect) */
#endif

/* --- AND ---  */
#if BUILD_HUC6280
#define DEF_AND(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_AND(__THISP_ K_READ##p##(__THISP_ a(__THISP))); } \
static void OpcodeCall T_Opco##i##(__CONTEXT) \
{ \
	Uword saveA = KMI_PRET(__THISP); \
	KM_AND(__THISP_ K_READ##p##(__THISP_ a(__THISP))); \
	KMI_POSTT(__THISP_ saveA); \
}
#else
#define DEF_AND(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_AND(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
#endif
DEF_AND(21,NP,KA_INDX)	/* 21 - AND - (Indirect,X) */
DEF_AND(25,ZP,KA_ZP)	/* 25 - AND - Zero Page */
DEF_AND(29,NP,KA_IMM)	/* 29 - AND - Immediate */
DEF_AND(2D,NP,KA_ABS)	/* 2D - AND - Absolute */
DEF_AND(31,NP,KA_INDY_)	/* 31 - AND - (Indirect),Y */
DEF_AND(35,ZP,KA_ZPX)	/* 35 - AND - Zero Page,X */
DEF_AND(39,NP,KA_ABSY_)	/* 39 - AND - Absolute,Y */
DEF_AND(3D,NP,KA_ABSX_)	/* 3D - AND - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_AND(32,NP,KA_IND)	/* 32 - AND - (Indirect) */
#endif

/* --- ASL ---  */
#define DEF_ASL(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = a(__THISP); \
	K_WRITE##p##(__THISP_ adr, KM_ASL(__THISP_ K_READ##p##(__THISP_ adr))); \
}
DEF_ASL(06,ZP,KA_ZP)	/* 06 - ASL - Zero Page */
DEF_ASL(0E,NP,KA_ABS)	/* 0E - ASL - Absolute */
DEF_ASL(16,ZP,KA_ZPX)	/* 16 - ASL - Zero Page,X */
DEF_ASL(1E,NP,KA_ABSX)	/* 1E - ASL - Absolute,X */
static void OpcodeCall Opcode0A(__CONTEXT)	/* 0A - ASL - Accumulator */
{ __THIS__.A = KM_ASL(__THISP_ __THIS__.A); }

#if BUILD_HUC6280
/* --- BBRi --- */
#define DEF_BBR(i,y) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = KA_ZP(__THISP); \
	Uword rel = K_READNP(__THISP_ KA_IMM(__THISP)); \
	if ((K_READZP(__THISP_ adr) & (1 << y)) == 0) KM_BRA(__THISP_ rel); \
}
DEF_BBR(0F,0)			/* 0F - BBR0 */
DEF_BBR(1F,1)			/* 1F - BBR1 */
DEF_BBR(2F,2)			/* 2F - BBR2 */
DEF_BBR(3F,3)			/* 3F - BBR3 */
DEF_BBR(4F,4)			/* 4F - BBR4 */
DEF_BBR(5F,5)			/* 5F - BBR5 */
DEF_BBR(6F,6)			/* 6F - BBR6 */
DEF_BBR(7F,7)			/* 7F - BBR7 */
/* --- BBSi --- */
#define DEF_BBS(i,y) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = KA_ZP(__THISP); \
	Uword rel = K_READNP(__THISP_ KA_IMM(__THISP)); \
	if ((K_READZP(__THISP_ adr) & (1 << y)) != 0) KM_BRA(__THISP_ rel); \
}
DEF_BBS(8F,0)			/* 8F - BBS0 */
DEF_BBS(9F,1)			/* 9F - BBS1 */
DEF_BBS(AF,2)			/* AF - BBS2 */
DEF_BBS(BF,3)			/* BF - BBS3 */
DEF_BBS(CF,4)			/* CF - BBS4 */
DEF_BBS(DF,5)			/* DF - BBS5 */
DEF_BBS(EF,6)			/* EF - BBS6 */
DEF_BBS(FF,7)			/* FF - BBS7 */
#endif

/* --- BIT ---  */
#define DEF_BIT(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_BIT(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
DEF_BIT(24,ZP,KA_ZP)	/* 24 - BIT - Zero Page */
DEF_BIT(2C,NP,KA_ABS)	/* 2C - BIT - Absolute */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_BIT(34,ZP,KA_ZPX)	/* 34 - BIT - Zero Page,X */
DEF_BIT(3C,NP,KA_ABSX_)	/* 3C - BIT - Absolute,X */
DEF_BIT(89,NP,KA_IMM)	/* 89 - BIT - Immediate */
#endif

/* --- Bcc ---  */
#define DEF_BRA(i,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword rel = K_READNP(__THISP_ KA_IMM(__THISP)); \
	if (a) KM_BRA(__THISP_ rel); \
}
DEF_BRA(10,((__THIS__.P & N_FLAG) == 0))	/* 10 - BPL */
DEF_BRA(30,((__THIS__.P & N_FLAG) != 0))	/* 30 - BMI */
DEF_BRA(50,((__THIS__.P & V_FLAG) == 0))	/* 50 - BVC */
DEF_BRA(70,((__THIS__.P & V_FLAG) != 0))	/* 70 - BVS */
DEF_BRA(90,((__THIS__.P & C_FLAG) == 0))	/* 90 - BCC */
DEF_BRA(B0,((__THIS__.P & C_FLAG) != 0))	/* B0 - BCS */
DEF_BRA(D0,((__THIS__.P & Z_FLAG) == 0))	/* D0 - BNE */
DEF_BRA(F0,((__THIS__.P & Z_FLAG) != 0))	/* F0 - BEQ */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_BRA(80,1)								/* 80 - BRA */
#endif

/* --- BRK --- */
static void OpcodeCall Opcode00(__CONTEXT)	/* 00 - BRK */
{
	__THIS__.PC = RTO16(__THIS__.PC + 1);
	__THIS__.iRequest |= IRQ_BRK;
}
#if BUILD_HUC6280
/* --- BSR --- */
static void OpcodeCall Opcode44(__CONTEXT)	/* 44 - BSR */
{
	KM_PUSH(__THISP_ RTO8(__THIS__.PC >> 8));	/* !!! PC = NEXT - 1; !!! */
	KM_PUSH(__THISP_ RTO8(__THIS__.PC));
	KM_BRA(__THISP_ K_READNP(__THISP_ KA_IMM(__THISP)));
}
/* --- CLA --- */
static void OpcodeCall Opcode62(__CONTEXT)	/* 62 - CLA */
{ __THIS__.A = 0; }
/* --- CLX --- */
static void OpcodeCall Opcode82(__CONTEXT)	/* 82 - CLX */
{ __THIS__.X = 0; }
/* --- CLY --- */
static void OpcodeCall OpcodeC2(__CONTEXT)	/* C2 - CLY */
{ __THIS__.Y = 0; }
#endif
/* --- CLC --- */
static void OpcodeCall Opcode18(__CONTEXT)	/* 18 - CLC */
{ __THIS__.P &= ~C_FLAG; }
/* --- CLD --- */
static void OpcodeCall OpcodeD8(__CONTEXT)	/* D8 - CLD */
{
	__THIS__.P &= ~D_FLAG;
}
/* --- CLI --- */
static void OpcodeCall Opcode58(__CONTEXT)	/* 58 - CLI */
{ __THIS__.P &= ~I_FLAG; }
/* --- CLV --- */
static void OpcodeCall OpcodeB8(__CONTEXT)	/* B8 - CLV */
{ __THIS__.P &= ~V_FLAG; }

/* --- CMP --- */
#define DEF_CMP(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_CMP(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
DEF_CMP(C1,NP,KA_INDX)	/* C1 - CMP - (Indirect,X) */
DEF_CMP(C5,ZP,KA_ZP)	/* C5 - CMP - Zero Page */
DEF_CMP(C9,NP,KA_IMM)	/* C9 - CMP - Immediate */
DEF_CMP(CD,NP,KA_ABS)	/* CD - CMP - Absolute */
DEF_CMP(D1,NP,KA_INDY_)	/* D1 - CMP - (Indirect),Y */
DEF_CMP(D5,ZP,KA_ZPX)	/* D5 - CMP - Zero Page,X */
DEF_CMP(D9,NP,KA_ABSY_)	/* D9 - CMP - Absolute,Y */
DEF_CMP(DD,NP,KA_ABSX_)	/* DD - CMP - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_CMP(D2,NP,KA_IND)	/* D2 - CMP - (Indirect) */
#endif

/* --- CPX --- */
#define DEF_CPX(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_CPX(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
DEF_CPX(E0,NP,KA_IMM)	/* E0 - CPX - Immediate */
DEF_CPX(E4,ZP,KA_ZP)	/* E4 - CPX - Zero Page */
DEF_CPX(EC,NP,KA_ABS)	/* EC - CPX - Absolute */

/* --- CPY --- */
#define DEF_CPY(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_CPY(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
DEF_CPY(C0,NP,KA_IMM)	/* C0 - CPY - Immediate */
DEF_CPY(C4,ZP,KA_ZP)	/* C4 - CPY - Zero Page */
DEF_CPY(CC,NP,KA_ABS)	/* CC - CPY - Absolute */

/* --- DEC ---  */
#define DEF_DEC(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = a(__THISP); \
	K_WRITE##p##(__THISP_ adr, KM_DEC(__THISP_ K_READ##p##(__THISP_ adr))); \
}
DEF_DEC(C6,ZP,KA_ZP)	/* C6 - DEC - Zero Page */
DEF_DEC(CE,NP,KA_ABS)	/* CE - DEC - Absolute */
DEF_DEC(D6,ZP,KA_ZPX)	/* D6 - DEC - Zero Page,X */
DEF_DEC(DE,NP,KA_ABSX)	/* DE - DEC - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
static void OpcodeCall Opcode3A(__CONTEXT)	/* 3A - DEA */
{ __THIS__.A = KM_DEC(__THISP_ __THIS__.A); }
#endif
static void OpcodeCall OpcodeCA(__CONTEXT)	/* CA - DEX */
{ __THIS__.X = KM_DEC(__THISP_ __THIS__.X); }
static void OpcodeCall Opcode88(__CONTEXT)	/* 88 - DEY */
{ __THIS__.Y = KM_DEC(__THISP_ __THIS__.Y); }

/* --- EOR ---  */
#if BUILD_HUC6280
#define DEF_EOR(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_EOR(__THISP_ K_READ##p##(__THISP_ a(__THISP))); } \
static void OpcodeCall T_Opco##i##(__CONTEXT) \
{ \
	Uword saveA = KMI_PRET(__THISP); \
	KM_EOR(__THISP_ K_READ##p##(__THISP_ a(__THISP))); \
	KMI_POSTT(__THISP_ saveA); \
}
#else
#define DEF_EOR(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_EOR(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
#endif
DEF_EOR(41,NP,KA_INDX)	/* 41 - EOR - (Indirect,X) */
DEF_EOR(45,ZP,KA_ZP)	/* 45 - EOR - Zero Page */
DEF_EOR(49,NP,KA_IMM)	/* 49 - EOR - Immediate */
DEF_EOR(4D,NP,KA_ABS)	/* 4D - EOR - Absolute */
DEF_EOR(51,NP,KA_INDY_)	/* 51 - EOR - (Indirect),Y */
DEF_EOR(55,ZP,KA_ZPX)	/* 55 - EOR - Zero Page,X */
DEF_EOR(59,NP,KA_ABSY_)	/* 59 - EOR - Absolute,Y */
DEF_EOR(5D,NP,KA_ABSX_)	/* 5D - EOR - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_EOR(52,NP,KA_IND)	/* 52 - EOR - (Indirect) */
#endif

/* --- INC ---  */
#define DEF_INC(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = a(__THISP); \
	K_WRITE##p##(__THISP_ adr, KM_INC(__THISP_ K_READ##p##(__THISP_ adr))); \
}
DEF_INC(E6,ZP,KA_ZP)	/* E6 - INC - Zero Page */
DEF_INC(EE,NP,KA_ABS)	/* EE - INC - Absolute */
DEF_INC(F6,ZP,KA_ZPX)	/* F6 - INC - Zero Page,X */
DEF_INC(FE,NP,KA_ABSX)	/* FE - INC - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
static void OpcodeCall Opcode1A(__CONTEXT)	/* 1A - INA */
{ __THIS__.A = KM_INC(__THISP_ __THIS__.A); }
#endif
static void OpcodeCall OpcodeE8(__CONTEXT)	/* E8 - INX */
{ __THIS__.X = KM_INC(__THISP_ __THIS__.X); }
static void OpcodeCall OpcodeC8(__CONTEXT)	/* C8 - INY */
{ __THIS__.Y = KM_INC(__THISP_ __THIS__.Y); }

/* --- JMP ---  */
#define DEF_JMP(i,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ __THIS__.PC = KI_READWORD(__THISP_ a(__THISP)); }
#if BUILD_HUC6280 || BUILD_M65C02
#define DEF_JMPBUG(i,a) DEF_JMP(i,a)
#else
#define DEF_JMPBUG(i,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ __THIS__.PC = KI_READWORDBUG(__THISP_ a(__THISP)); }
#endif
DEF_JMP(4C,KA_IMM16)	/* 4C - JMP - Immediate */
DEF_JMPBUG(6C,KA_ABS)	/* 6C - JMP - Absolute */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_JMP(7C,KA_ABSX)	/* 7C - JMP - Absolute,X */
#endif

/* --- JSR --- */
static void OpcodeCall Opcode20(__CONTEXT)	/* 20 - JSR */
{
	Uword adr = KA_IMM(__THISP);
	KM_PUSH(__THISP_ RTO8(__THIS__.PC >> 8));	/* !!! PC = NEXT - 1; !!! */
	KM_PUSH(__THISP_ RTO8(__THIS__.PC));
	__THIS__.PC = KI_READWORD(__THISP_ adr);
}

/* --- LDA --- */
#define DEF_LDA(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ __THIS__.A = KM_LD(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
DEF_LDA(A1,NP,KA_INDX)	/* A1 - LDA - (Indirect,X) */
DEF_LDA(A5,ZP,KA_ZP)	/* A5 - LDA - Zero Page */
DEF_LDA(A9,NP,KA_IMM)	/* A9 - LDA - Immediate */
DEF_LDA(AD,NP,KA_ABS)	/* AD - LDA - Absolute */
DEF_LDA(B1,NP,KA_INDY_)	/* B1 - LDA - (Indirect),Y */
DEF_LDA(B5,ZP,KA_ZPX)	/* B5 - LDA - Zero Page,X */
DEF_LDA(B9,NP,KA_ABSY_)	/* B9 - LDA - Absolute,Y */
DEF_LDA(BD,NP,KA_ABSX_)	/* BD - LDA - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_LDA(B2,NP,KA_IND)	/* B2 - LDA - (Indirect) */
#endif

/* --- LDX ---  */
#define DEF_LDX(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ __THIS__.X = KM_LD(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
DEF_LDX(A2,NP,KA_IMM)	/* A2 - LDX - Immediate */
DEF_LDX(A6,ZP,KA_ZP)	/* A6 - LDX - Zero Page */
DEF_LDX(AE,NP,KA_ABS)	/* AE - LDX - Absolute */
DEF_LDX(B6,ZP,KA_ZPY)	/* B6 - LDX - Zero Page,Y */
DEF_LDX(BE,NP,KA_ABSY_)	/* BE - LDX - Absolute,Y */

/* --- LDY ---  */
#define DEF_LDY(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ __THIS__.Y = KM_LD(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
DEF_LDY(A0,NP,KA_IMM)	/* A0 - LDY - Immediate */
DEF_LDY(A4,ZP,KA_ZP)	/* A4 - LDY - Zero Page */
DEF_LDY(AC,NP,KA_ABS)	/* AC - LDY - Absolute */
DEF_LDY(B4,ZP,KA_ZPX)	/* B4 - LDY - Zero Page,X */
DEF_LDY(BC,NP,KA_ABSX_)	/* BC - LDY - Absolute,X */

/* --- LSR ---  */
#define DEF_LSR(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = a(__THISP); \
	K_WRITE##p##(__THISP_ adr, KM_LSR(__THISP_ K_READ##p##(__THISP_ adr))); \
}
DEF_LSR(46,ZP,KA_ZP)	/* 46 - LSR - Zero Page */
DEF_LSR(4E,NP,KA_ABS)	/* 4E - LSR - Absolute */
DEF_LSR(56,ZP,KA_ZPX)	/* 56 - LSR - Zero Page,X */
DEF_LSR(5E,NP,KA_ABSX)	/* 5E - LSR - Absolute,X */
static void OpcodeCall Opcode4A(__CONTEXT)	/* 4A - LSR - Accumulator */
{ __THIS__.A = KM_LSR(__THISP_ __THIS__.A); }

/* --- NOP ---  */
static void OpcodeCall OpcodeEA(__CONTEXT)	/* EA - NOP */
{
}

/* --- ORA ---  */
#if BUILD_HUC6280
#define DEF_ORA(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_ORA(__THISP_ K_READ##p##(__THISP_ a(__THISP))); } \
static void OpcodeCall T_Opco##i##(__CONTEXT) \
{ \
	Uword saveA = KMI_PRET(__THISP); \
	KM_ORA(__THISP_ K_READ##p##(__THISP_ a(__THISP))); \
	KMI_POSTT(__THISP_ saveA); \
}
#else
#define DEF_ORA(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KM_ORA(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
#endif
DEF_ORA(01,NP,KA_INDX)	/* 01 - ORA - (Indirect,X) */
DEF_ORA(05,ZP,KA_ZP)	/* 05 - ORA - Zero Page */
DEF_ORA(09,NP,KA_IMM)	/* 09 - ORA - Immediate */
DEF_ORA(0D,NP,KA_ABS)	/* 0D - ORA - Absolute */
DEF_ORA(11,NP,KA_INDY_)	/* 11 - ORA - (Indirect),Y */
DEF_ORA(15,ZP,KA_ZPX)	/* 15 - ORA - Zero Page,X */
DEF_ORA(19,NP,KA_ABSY_)	/* 19 - ORA - Absolute,Y */
DEF_ORA(1D,NP,KA_ABSX_)	/* 1D - ORA - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_ORA(12,NP,KA_IND)	/* 12 - ORA - (Indirect) */
#endif

/* --- PHr PLr  --- */
static void OpcodeCall Opcode48(__CONTEXT)	/* 48 - PHA */
{ KM_PUSH(__THISP_ __THIS__.A); }
static void OpcodeCall Opcode08(__CONTEXT)	/* 08 - PHP */
{ KM_PUSH(__THISP_ (Uword)((__THIS__.P | B_FLAG | R_FLAG) & ~T_FLAG)); }
static void OpcodeCall Opcode68(__CONTEXT)	/* 68 - PLA */
{ __THIS__.A = KM_LD(__THISP_ KM_POP(__THISP)); }
static void OpcodeCall Opcode28(__CONTEXT)	/* 28 - PLP */
{ __THIS__.P = KM_POP(__THISP) & ~T_FLAG; }
#if BUILD_HUC6280 || BUILD_M65C02
static void OpcodeCall OpcodeDA(__CONTEXT)	/* DA - PHX */
{ KM_PUSH(__THISP_ __THIS__.X); }
static void OpcodeCall Opcode5A(__CONTEXT)	/* 5A - PHY */
{ KM_PUSH(__THISP_ __THIS__.Y); }
static void OpcodeCall OpcodeFA(__CONTEXT)	/* FA - PLX */
{ __THIS__.X = KM_LD(__THISP_ KM_POP(__THISP)); }
static void OpcodeCall Opcode7A(__CONTEXT)	/* 7A - PLY */
{ __THIS__.Y = KM_LD(__THISP_ KM_POP(__THISP)); }
#endif

#if BUILD_HUC6280
/* --- RMBi --- */
#define DEF_RMB(i,y) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = KA_ZP(__THISP); \
	K_WRITEZP(__THISP_ adr, (Uword)(K_READZP(__THISP_ adr) & (~(1 << y)))); \
}
DEF_RMB(07,0)	/* 07 - RMB0 */
DEF_RMB(17,1)	/* 17 - RMB1 */
DEF_RMB(27,2)	/* 27 - RMB2 */
DEF_RMB(37,3)	/* 37 - RMB3 */
DEF_RMB(47,4)	/* 47 - RMB4 */
DEF_RMB(57,5)	/* 57 - RMB5 */
DEF_RMB(67,6)	/* 67 - RMB6 */
DEF_RMB(77,7)	/* 77 - RMB7 */
/* --- SMBi --- */
#define DEF_SMB(i,y) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = KA_ZP(__THISP); \
	K_WRITEZP(__THISP_ adr, (Uword)(K_READZP(__THISP_ adr) | (1 << y))); \
}
DEF_SMB(87,0)	/* 87 - SMB0 */
DEF_SMB(97,1)	/* 97 - SMB1 */
DEF_SMB(A7,2)	/* A7 - SMB2 */
DEF_SMB(B7,3)	/* B7 - SMB3 */
DEF_SMB(C7,4)	/* C7 - SMB4 */
DEF_SMB(D7,5)	/* D7 - SMB5 */
DEF_SMB(E7,6)	/* E7 - SMB6 */
DEF_SMB(F7,7)	/* F7 - SMB7 */
#endif

/* --- ROL ---  */
#define DEF_ROL(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = a(__THISP); \
	K_WRITE##p##(__THISP_ adr, KM_ROL(__THISP_ K_READ##p##(__THISP_ adr))); \
}
DEF_ROL(26,ZP,KA_ZP)	/* 26 - ROL - Zero Page */
DEF_ROL(2E,NP,KA_ABS)	/* 2E - ROL - Absolute */
DEF_ROL(36,ZP,KA_ZPX)	/* 36 - ROL - Zero Page,X */
DEF_ROL(3E,NP,KA_ABSX)	/* 3E - ROL - Absolute,X */
static void OpcodeCall Opcode2A(__CONTEXT)	/* 2A - ROL - Accumulator */
{ __THIS__.A = KM_ROL(__THISP_ __THIS__.A); }

/* --- ROR ---  */
#define DEF_ROR(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = a(__THISP); \
	K_WRITE##p##(__THISP_ adr, KM_ROR(__THISP_ K_READ##p##(__THISP_ adr))); \
}
DEF_ROR(66,ZP,KA_ZP)	/* 66 - ROR - Zero Page */
DEF_ROR(6E,NP,KA_ABS)	/* 6E - ROR - Absolute */
DEF_ROR(76,ZP,KA_ZPX)	/* 76 - ROR - Zero Page,X */
DEF_ROR(7E,NP,KA_ABSX)	/* 7E - ROR - Absolute,X */
static void OpcodeCall Opcode6A(__CONTEXT)	/* 6A - ROR - Accumulator */
{ __THIS__.A = KM_ROR(__THISP_ __THIS__.A); }

static void OpcodeCall Opcode40(__CONTEXT)	/* 40 - RTI */
{
	__THIS__.P = KM_POP(__THISP);
	__THIS__.PC  = KM_POP(__THISP);
	__THIS__.PC += KM_POP(__THISP) << 8;
}
static void OpcodeCall Opcode60(__CONTEXT)	/* 60 - RTS */
{
	__THIS__.PC  = KM_POP(__THISP);
	__THIS__.PC += KM_POP(__THISP) << 8;
	__THIS__.PC  = RTO16(__THIS__.PC + 1);
}

#if BUILD_HUC6280
static void OpcodeCall Opcode22(__CONTEXT)	/* 22 - SAX */
{
	Uword temp = __THIS__.A;
	__THIS__.A = __THIS__.X;
	__THIS__.X = temp;
}
static void OpcodeCall Opcode42(__CONTEXT)	/* 42 - SAY */
{
	Uword temp = __THIS__.A;
	__THIS__.A = __THIS__.Y;
	__THIS__.Y = temp;
}
static void OpcodeCall Opcode02(__CONTEXT)	/* 02 - SXY */
{
	Uword temp = __THIS__.Y;
	__THIS__.Y = __THIS__.X;
	__THIS__.X = temp;
}
#endif

/* --- SBC ---  */
#define DEF_SBC(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ KMI_SBC(__THISP_ K_READ##p##(__THISP_ a(__THISP))); } \
static void OpcodeCall D_Opco##i##(__CONTEXT) \
{ KMI_SBC_D(__THISP_ K_READ##p##(__THISP_ a(__THISP))); }
DEF_SBC(E1,NP,KA_INDX)	/* E1 - SBC - (Indirect,X) */
DEF_SBC(E5,ZP,KA_ZP)	/* E5 - SBC - Zero Page */
DEF_SBC(E9,NP,KA_IMM)	/* E9 - SBC - Immediate */
DEF_SBC(ED,NP,KA_ABS)	/* ED - SBC - Absolute */
DEF_SBC(F1,NP,KA_INDY_)	/* F1 - SBC - (Indirect),Y */
DEF_SBC(F5,ZP,KA_ZPX)	/* F5 - SBC - Zero Page,X */
DEF_SBC(F9,NP,KA_ABSY_)	/* F9 - SBC - Absolute,Y */
DEF_SBC(FD,NP,KA_ABSX_)	/* FD - SBC - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_SBC(F2,NP,KA_IND)	/* F2 - SBC - (Indirect) */
#endif

/* --- SEC --- */
static void OpcodeCall Opcode38(__CONTEXT)	/* 38 - SEC */
{ __THIS__.P |= C_FLAG; }
/* --- SED --- */
static void OpcodeCall OpcodeF8(__CONTEXT)	/* F8 - SED */
{ __THIS__.P |= D_FLAG; }
/* --- SEI --- */
static void OpcodeCall Opcode78(__CONTEXT)	/* 78 - SEI */
{ __THIS__.P |= I_FLAG; }

#if BUILD_HUC6280
/* --- SET --- */
static void OpcodeCall OpcodeF4(__CONTEXT)	/* F4 - SET */
{ __THIS__.P |= T_FLAG; }
#endif

#if BUILD_HUC6280
static void OpcodeCall Opcode03(__CONTEXT)	/* 03 - ST0 */
{ K_WRITE6270(__THISP_ 0, K_READNP(__THISP_ KA_IMM(__THISP))); }
static void OpcodeCall Opcode13(__CONTEXT)	/* 13 - ST1 */
{ K_WRITE6270(__THISP_ 2, K_READNP(__THISP_ KA_IMM(__THISP))); }
static void OpcodeCall Opcode23(__CONTEXT)	/* 23 - ST2 */
{ K_WRITE6270(__THISP_ 3, K_READNP(__THISP_ KA_IMM(__THISP))); }
#endif

/* --- STA --- */
#define DEF_STA(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ K_WRITE##p##(__THISP_ a(__THISP), __THIS__.A); }
DEF_STA(81,NP,KA_INDX)	/* 81 - STA - (Indirect,X) */
DEF_STA(85,ZP,KA_ZP)	/* 85 - STA - Zero Page */
DEF_STA(8D,NP,KA_ABS)	/* 8D - STA - Absolute */
DEF_STA(91,NP,KA_INDY)	/* 91 - STA - (Indirect),Y */
DEF_STA(95,ZP,KA_ZPX)	/* 95 - STA - Zero Page,X */
DEF_STA(99,NP,KA_ABSY)	/* 99 - STA - Absolute,Y */
DEF_STA(9D,NP,KA_ABSX)	/* 9D - STA - Absolute,X */
#if BUILD_HUC6280 || BUILD_M65C02
DEF_STA(92,NP,KA_IND)	/* 92 - STA - (Indirect) */
#endif

/* --- STX ---  */
#define DEF_STX(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ K_WRITE##p##(__THISP_ a(__THISP), __THIS__.X); }
DEF_STX(86,ZP,KA_ZP)	/* 86 - STX - Zero Page */
DEF_STX(8E,NP,KA_ABS)	/* 8E - STX - Absolute */
DEF_STX(96,ZP,KA_ZPY)	/* 96 - STX - Zero Page,Y */

/* --- STY ---  */
#define DEF_STY(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ K_WRITE##p##(__THISP_ a(__THISP), __THIS__.Y); }
DEF_STY(84,ZP,KA_ZP)	/* 84 - STY - Zero Page */
DEF_STY(8C,NP,KA_ABS)	/* 8C - STY - Absolute */
DEF_STY(94,ZP,KA_ZPX)	/* 94 - STY - Zero Page,X */

#if BUILD_HUC6280 || BUILD_M65C02
/* --- STZ ---  */
#define DEF_STZ(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ K_WRITE##p##(__THISP_ a(__THISP), 0); }
DEF_STZ(64,ZP,KA_ZP)	/* 64 - STZ - Zero Page */
DEF_STZ(9C,NP,KA_ABS)	/* 9C - STZ - Absolute */
DEF_STZ(74,ZP,KA_ZPX)	/* 74 - STZ - Zero Page,X */
DEF_STZ(9E,NP,KA_ABSX)	/* 9E - STZ - Absolute,X */
#endif

#if BUILD_HUC6280
/* --- TAMi ---  */
static void OpcodeCall Opcode53(__CONTEXT)	/* 53 - TAMi */
{ K_WRITEMPR(__THISP_ K_READNP(__THISP_ KA_IMM(__THISP)), __THIS__.A); }
/* --- TMAi ---  */
static void OpcodeCall Opcode43(__CONTEXT)	/* 43 - TMAi */
{ __THIS__.A = K_READMPR(__THISP_ K_READNP(__THISP_ KA_IMM(__THISP))); }
#endif

#if BUILD_HUC6280 || BUILD_M65C02
/* --- TRB --- */
#define DEF_TRB(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = a(__THISP); \
	K_WRITE##p##(__THISP_ adr, KM_TRB(__THISP_ K_READ##p##(__THISP_ adr))); \
}
DEF_TRB(14,ZP,KA_ZP)	/* 14 - TRB - Zero Page */
DEF_TRB(1C,NP,KA_ABS)	/* 1C - TRB - Absolute */
/* --- TSB --- */
#define DEF_TSB(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword adr = a(__THISP); \
	K_WRITE##p##(__THISP_ adr, KM_TSB(__THISP_ K_READ##p##(__THISP_ adr))); \
}
DEF_TSB(04,ZP,KA_ZP)	/* 04 - TSB - Zero Page */
DEF_TSB(0C,NP,KA_ABS)	/* 0C - TSB - Absolute */
#endif

#if BUILD_HUC6280
/* --- TST --- */
#define DEF_TST(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
	Uword imm = K_READNP(__THISP_ KA_IMM(__THISP)); \
	KM_TST(__THISP_ imm, K_READ##p##(__THISP_ a(__THISP))); \
}
DEF_TST(83,ZP,KA_ZP)	/* 83 - TST - Zero Page */
DEF_TST(93,NP,KA_ABS)	/* 93 - TST - Absolute */
DEF_TST(A3,ZP,KA_ZPX)	/* A3 - TST - Zero Page,X */
DEF_TST(B3,NP,KA_ABSX)	/* B3 - TST - Absolute,X */
#endif

/* --- TAX ---  */
static void OpcodeCall OpcodeAA(__CONTEXT)	/* AA - TAX */
{ __THIS__.X = KM_LD(__THISP_ __THIS__.A); }
/* --- TAY ---  */
static void OpcodeCall OpcodeA8(__CONTEXT)	/* A8 - TAY */
{ __THIS__.Y = KM_LD(__THISP_ __THIS__.A); }
/* --- TSX ---  */
static void OpcodeCall OpcodeBA(__CONTEXT)	/* BA - TSX */
{ __THIS__.X = KM_LD(__THISP_ __THIS__.S); }
/* --- TXA ---  */
static void OpcodeCall Opcode8A(__CONTEXT)	/* 8A - TXA */
{ __THIS__.A = KM_LD(__THISP_ __THIS__.X); }
/* --- TXS ---  */
static void OpcodeCall Opcode9A(__CONTEXT)	/* 9A - TXS */
{ __THIS__.S = __THIS__.X; }
/* --- TYA ---  */
static void OpcodeCall Opcode98(__CONTEXT)	/* 98 - TYA */
{ __THIS__.A = KM_LD(__THISP_ __THIS__.Y); }

#if BUILD_HUC6280
static void OpcodeCall Opcode73(__CONTEXT)	/* 73 - TII */
{
	Uword src,des,len;
	src = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	des = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	len = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	KI_ADDCLOCK(__THISP_ (Uword)(len ? len * 6 : (Uword)0x60000));
	do
	{
		K_WRITENP(__THISP_ des, K_READNP(__THISP_ src));
		src = RTO16(src + 1);
		des = RTO16(des + 1);
		len = RTO16(len - 1);
	} while (len != 0);
}
static void OpcodeCall OpcodeC3(__CONTEXT)	/* C3 - TDD */
{
	Uword src,des,len;
	src = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	des = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	len = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	KI_ADDCLOCK(__THISP_ (Uword)(len ? len * 6 : (Uword)0x60000));
	do
	{
		K_WRITENP(__THISP_ des, K_READNP(__THISP_ src));
		src = RTO16(src - 1);
		des = RTO16(des - 1);
		len = RTO16(len - 1);
	} while (len != 0);
}
static void OpcodeCall OpcodeD3(__CONTEXT)	/* D3 - TIN */
{
	Uword src,des,len;
	src = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	des = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	len = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	KI_ADDCLOCK(__THISP_ (Uword)(len ? len * 6 : (Uword)0x60000));
	do
	{
		K_WRITENP(__THISP_ des, K_READNP(__THISP_ src));
		src = RTO16(src + 1);
		len = RTO16(len - 1);
	} while (len != 0);
}
static void OpcodeCall OpcodeE3(__CONTEXT)	/* E3 - TIA */
{
	int add = +1;
	Uword src,des,len;
	src = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	des = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	len = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	KI_ADDCLOCK(__THISP_ (Uword)(len ? len * 6 : (Uword)0x60000));
	do
	{
		K_WRITENP(__THISP_ des, K_READNP(__THISP_ src));
		src = RTO16(src + 1);
		des = RTO16(des + add);
		add = -add;
		len = RTO16(len - 1);
	} while (len != 0);
}
static void OpcodeCall OpcodeF3(__CONTEXT)	/* F3 - TAI */
{
	int add = +1;
	Uword src,des,len;
	src = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	des = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	len = KI_READWORD(__THISP_ KA_IMM16(__THISP));
	KI_ADDCLOCK(__THISP_ (Uword)(len ? len * 6 : (Uword)0x60000));
	do
	{
		K_WRITENP(__THISP_ des, K_READNP(__THISP_ src));
		src = RTO16(src + add);
		des = RTO16(des + 1);
		add = -add;
		len = RTO16(len - 1);
	} while (len != 0);
}
static void OpcodeCall Opcode54(__CONTEXT)	/* 54 - CSL */
{ __THIS__.lowClockMode = 1; }
static void OpcodeCall OpcodeD4(__CONTEXT)	/* D4 - CSH */
{ __THIS__.lowClockMode = 0; }
#endif

/* BS - implementing all illegal opcodes */
#if ILLEGAL_OPCODES

/* --- KIL ---  */
/* halts CPU */
#define DEF_KIL(i) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
    __THIS__.PC = RTO16(__THIS__.PC - 1); \
    __THIS__.P |= I_FLAG; /* disable interrupt */ \
}
/* opcodes */
DEF_KIL(02)
DEF_KIL(12)
DEF_KIL(22)
DEF_KIL(32)
DEF_KIL(42)
DEF_KIL(52)
DEF_KIL(62)
DEF_KIL(72)
DEF_KIL(92)
DEF_KIL(B2)
DEF_KIL(D2)
DEF_KIL(F2)

/* --- NOP ---  */
/* does nothing */
#define DEF_NOP(i) static void OpcodeCall Opcode##i##(__CONTEXT) \
{}
/* fetches operands but does not use them, issues dummy reads (may have page boundary cycle penalty) */
#define DEF_NOP_A(i,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ a(__THISP); }
/* opcodes */
DEF_NOP_A(80,KAI_IMM);
DEF_NOP_A(82,KAI_IMM);
DEF_NOP_A(C2,KAI_IMM);
DEF_NOP_A(E2,KAI_IMM);
DEF_NOP_A(04,KAI_ZP);
DEF_NOP_A(14,KAI_ZPX);
DEF_NOP_A(34,KAI_ZPX);
DEF_NOP_A(44,KAI_ZP);
DEF_NOP_A(54,KAI_ZPX);
DEF_NOP_A(64,KAI_ZP);
DEF_NOP_A(74,KAI_ZPX);
DEF_NOP_A(D4,KAI_ZPX);
DEF_NOP_A(F4,KAI_ZPX);
DEF_NOP_A(89,KAI_IMM);
DEF_NOP(1A);
DEF_NOP(3A);
DEF_NOP(5A);
DEF_NOP(7A);
DEF_NOP(DA);
DEF_NOP(FA);
DEF_NOP_A(0C,KAI_ABS);
DEF_NOP_A(1C,KA_ABSX_);
DEF_NOP_A(3C,KA_ABSX_);
DEF_NOP_A(5C,KA_ABSX_);
DEF_NOP_A(7C,KA_ABSX_);
DEF_NOP_A(DC,KA_ABSX_);
DEF_NOP_A(FC,KA_ABSX_);

/* --- SLO ---  */
/* shift left, OR result */
static Uword OpsubCall KM_SLO(__CONTEXT_ Uword src)
{
    Uword w = src << 1;
    __THIS__.A |= RTO8(w);
    __THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.A);
    __THIS__.P += src >> 7; /* C_FLAG */
    return __THIS__.A;
}
/* macro */
#define DEF_SLO(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
    Uword adr = a(__THISP); \
    Uword src = K_READ##p##(__THISP_ adr); \
    K_WRITE##p##(__THISP_ adr, KM_SLO(__THISP_ src)); \
}
/* opcodes */
DEF_SLO(03,NP,KA_INDX);
DEF_SLO(13,NP,KA_INDY);
DEF_SLO(07,ZP,KA_ZP);
DEF_SLO(17,ZP,KA_ZPX);
DEF_SLO(1B,NP,KA_ABSY);
DEF_SLO(0F,NP,KA_ABS);
DEF_SLO(1F,NP,KA_ABSX);

/* --- RLA ---  */
/* rotate left, AND result */
static Uword OpsubCall KM_RLA(__CONTEXT_ Uword src)
{
    Uword w = (src << 1) + (__THIS__.P & C_FLAG);
    __THIS__.A &= RTO8(w);
    __THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.A);
    __THIS__.P += src >> 7; /* C_FLAG */
    return __THIS__.A;
}
/* macro */
#define DEF_RLA(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
    Uword adr = a(__THISP); \
    Uword src = K_READ##p##(__THISP_ adr); \
    K_WRITE##p##(__THISP_ adr, KM_RLA(__THISP_ src)); \
}
/* opcodes */
DEF_RLA(23,NP,KA_INDX);
DEF_RLA(33,NP,KA_INDY);
DEF_RLA(27,ZP,KA_ZP);
DEF_RLA(37,ZP,KA_ZPX);
DEF_RLA(3B,NP,KA_ABSY);
DEF_RLA(2F,NP,KA_ABS);
DEF_RLA(3F,NP,KA_ABSX);

/* --- SRE ---  */
/* shift right, EOR result */
static Uword OpsubCall KM_SRE(__CONTEXT_ Uword src)
{
    Uword w = src >> 1;
    __THIS__.A ^= RTO8(w);
    __THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.A);
    __THIS__.P += src & 1; /* C_FLAG */
    return __THIS__.A;
}
/* macro */
#define DEF_SRE(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
    Uword adr = a(__THISP); \
    Uword src = K_READ##p##(__THISP_ adr); \
    K_WRITE##p##(__THISP_ adr, KM_SRE(__THISP_ src)); \
}
/* opcodes */
DEF_SRE(43,NP,KA_INDX);
DEF_SRE(53,NP,KA_INDY);
DEF_SRE(47,ZP,KA_ZP);
DEF_SRE(57,ZP,KA_ZPX);
DEF_SRE(5B,NP,KA_ABSY);
DEF_SRE(4F,NP,KA_ABS);
DEF_SRE(5F,NP,KA_ABSX);

/* --- RRA ---  */
/* rotate right, ADC result */
static Uword OpsubCall KM_RRA(__CONTEXT_ Uword src)
{
    Uword w = (src >> 1) + ((__THIS__.P & C_FLAG) << 7);
    __THIS__.P &= ~(C_FLAG);
    __THIS__.P += (src >> 7); /* C_FLAG */
    KMI_ADC(__THISP_ RTO8(w));
    return __THIS__.A;
}
/* macro */
#define DEF_RRA(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
    Uword adr = a(__THISP); \
    Uword src = K_READ##p##(__THISP_ adr); \
    K_WRITE##p##(__THISP_ adr, KM_RRA(__THISP_ src)); \
}
/* opcodes */
DEF_RRA(63,NP,KA_INDX);
DEF_RRA(73,NP,KA_INDY);
DEF_RRA(67,ZP,KA_ZP);
DEF_RRA(77,ZP,KA_ZPX);
DEF_RRA(7B,NP,KA_ABSY);
DEF_RRA(6F,NP,KA_ABS);
DEF_RRA(7F,NP,KA_ABSX);

/* --- DCP ---  */
/* decrement, CMP */
static Uword OpsubCall KM_DCP(__CONTEXT_ Uword src)
{
    Uword w = RTO8(src - 1);
    KM_CMP(__THISP_ w);
    return w;
}
/* macro */
#define DEF_DCP(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
    Uword adr = a(__THISP); \
    Uword src = K_READ##p##(__THISP_ adr); \
    K_WRITE##p##(__THISP_ adr, KM_DCP(__THISP_ src)); \
}
/* opcodes */
DEF_DCP(C3,NP,KA_INDX);
DEF_DCP(D3,NP,KA_INDY);
DEF_DCP(C7,ZP,KA_ZP);
DEF_DCP(D7,ZP,KA_ZPX);
DEF_DCP(DB,NP,KA_ABSY);
DEF_DCP(CF,NP,KA_ABS);
DEF_DCP(DF,NP,KA_ABSX);

/* --- ISC ---  */
/* increment, SBC */
static Uword OpsubCall KM_ISC(__CONTEXT_ Uword src)
{
    Uword w = RTO8(src + 1);
    KMI_SBC(__THISP_ w);
    return __THIS__.A;
}
/* macro */
#define DEF_ISC(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
    Uword adr = a(__THISP); \
    Uword src = K_READ##p##(__THISP_ adr); \
    K_WRITE##p##(__THISP_ adr, KM_ISC(__THISP_ src)); \
}
/* opcodes */
DEF_ISC(E3,NP,KA_INDX);
DEF_ISC(F3,NP,KA_INDY);
DEF_ISC(E7,ZP,KA_ZP);
DEF_ISC(F7,ZP,KA_ZPX);
DEF_ISC(FB,NP,KA_ABSY);
DEF_ISC(EF,NP,KA_ABS);
DEF_ISC(FF,NP,KA_ABSX);

/* --- LAX ---  */
/* load A and X */
static void OpsubCall KM_LAX(__CONTEXT_ Uword src)
{
    __THIS__.A = src;
    __THIS__.X = src;
    __THIS__.P &= ~(N_FLAG | Z_FLAG);
    __THIS__.P += FLAG_NZ(src);
}
/* macro */
#define DEF_LAX(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
    Uword adr = a(__THISP); \
    Uword src = K_READ##p##(__THISP_ adr); \
    KM_LAX(__THISP_ src); \
}
/* opcodes */
DEF_LAX(A3,NP,KA_INDX);
DEF_LAX(B3,NP,KA_INDY_);
DEF_LAX(A7,ZP,KA_ZP);
DEF_LAX(B7,ZP,KA_ZPX);
DEF_LAX(AB,NP,KA_IMM); /* this one is unstable on hardware */
DEF_LAX(AF,NP,KA_ABS);
DEF_LAX(BF,NP,KA_ABSY_);

/* --- SAX ---  */
/* store A AND X */
#define DEF_SAX(i,p,a) static void OpcodeCall Opcode##i##(__CONTEXT) \
{ \
    K_WRITE##p##(__THISP_ a(__THISP), (__THIS__.A & __THIS__.X) ); \
}
/* opcodes */
DEF_SAX(83,NP,KA_INDX);
DEF_SAX(87,ZP,KA_ZP);
DEF_SAX(97,ZP,KA_ZPY);
DEF_SAX(8F,NP,KA_ABS);

/* --- AHX ---  */
/* store A AND X AND high address (somewhat unstable) */
static void OpcodeCall Opcode93(__CONTEXT)
{
    Uword adr = KA_ZPY(__THISP);
    K_WRITEZP(__THISP_ adr, RTO8(__THIS__.A & __THIS__.X & ((adr >> 8) + 1)) );
}
static void OpcodeCall Opcode9F(__CONTEXT)
{
    Uword adr = KA_ABSY(__THISP);
    K_WRITENP(__THISP_ adr, RTO8(__THIS__.A & __THIS__.X & ((adr >> 8) + 1)) );
}

/* --- TAS --- */
/* transfer A AND X to S, store A AND X AND high address */
static void OpcodeCall Opcode9B(__CONTEXT)
{
    Uword adr = KA_ABSY(__THISP);
    __THIS__.S = __THIS__.A & __THIS__.X;
    K_WRITENP(__THISP_ adr, RTO8(__THIS__.S & ((adr >> 8) + 1)) );
}

/* --- SHY --- */
/* store Y AND high address (somewhat unstable) */
static void OpcodeCall Opcode9C(__CONTEXT)
{
    Uword adr = KA_ABSX(__THISP);
    K_WRITENP(__THISP_ adr, RTO8(__THIS__.Y & ((adr >> 8) + 1)) );
}

/* --- SHX --- */
/* store X AND high address (somewhat unstable) */
static void OpcodeCall Opcode9E(__CONTEXT)
{
    Uword adr = KA_ABSY(__THISP);
    K_WRITENP(__THISP_ adr, RTO8(__THIS__.X & ((adr >> 8) + 1)) );
}

/* --- ANC --- */
/* a = A AND immediate */
static void OpcodeCall Opcode0B(__CONTEXT)
{
    Uword adr = KA_IMM(__THISP);
    __THIS__.A = RTO8(__THIS__.A & K_READNP(__THISP_ adr));
    __THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.A);
    __THIS__.P += (__THIS__.A >> 7); /* C_FLAG */
}
static void OpcodeCall Opcode2B(__CONTEXT)
{
    Uword adr = KA_IMM(__THISP);
    __THIS__.A = RTO8(__THIS__.A & K_READNP(__THISP_ adr));
    __THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.A);
    __THIS__.P += (__THIS__.A >> 7); /* C_FLAG */
}

/* --- XAA --- */
/* a = X AND immediate (unstable) */
static void OpcodeCall Opcode8B(__CONTEXT)
{
    Uword adr = KA_IMM(__THISP);
    __THIS__.A = RTO8(__THIS__.X & K_READNP(__THISP_ adr));
    __THIS__.P &= ~(N_FLAG | Z_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.A);
}

/* --- ALR --- */
/* A AND immediate (unstable), shift right */
static void OpcodeCall Opcode4B(__CONTEXT)
{
    Uword adr = KA_IMM(__THISP);
    Uword res = RTO8(__THIS__.A & K_READNP(__THISP_ adr));
    __THIS__.A = res >> 1;
    __THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.A);
    __THIS__.P += (res & 1); /* C_FLAG */
}

/* --- ARR --- */
/* A AND immediate (unstable), rotate right, weird carry */
static void OpcodeCall Opcode6B(__CONTEXT)
{
    Uword adr = KA_IMM(__THISP);
    Uword res = RTO8(__THIS__.A & K_READNP(__THISP_ adr));
    __THIS__.A = (res >> 1) + ((__THIS__.P & C_FLAG) << 7);
    __THIS__.P &= ~(N_FLAG | V_FLAG | Z_FLAG | C_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.A);
    __THIS__.P += (res ^ (res >> 1)) & V_FLAG;
    __THIS__.P += (res >> 7); /* C_FLAG */
}

/* --- LAS --- */
/* stack AND immediate, copy to A and X */
static void OpcodeCall OpcodeBB(__CONTEXT)
{
    Uword adr = KA_ABSY_(__THISP);
    __THIS__.S &= RTO8(K_READNP(__THISP_ adr));
    __THIS__.A = __THIS__.S;
    __THIS__.X = __THIS__.S;
    __THIS__.P &= ~(N_FLAG | Z_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.A);
}

/* --- AXS --- */
/* (A & X) - immediate, result in X */
static void OpcodeCall OpcodeCB(__CONTEXT)
{
    Uword adr = KA_IMM(__THISP);
    Uword res = (__THIS__.A & __THIS__.X) - RTO8(K_READNP(__THISP_ adr));
    __THIS__.X = RTO8(res);
    __THIS__.P &= ~(N_FLAG | Z_FLAG | C_FLAG);
    __THIS__.P += FLAG_NZ(__THIS__.X);
    __THIS__.P += (res <= 0xFF) ? C_FLAG : 0;
}

/* --- SBC --- */
/* EB is alternate opcode for SBC E9 */
static void OpcodeCall OpcodeEB(__CONTEXT)
{
    OpcodeE9(__THISP);
}

#endif