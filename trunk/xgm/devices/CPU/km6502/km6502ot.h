#define OP__(i) \
	case 0x##i##: \
		Opcode##i##(__THISP); \
		break;

/* BS - option to disable decimal execution for NES */
#if DISABLE_DECIMAL
#define OP_d OP__
#else
#define OP_d(i) \
	case 0x##i##: \
		if (__THIS__.P & D_FLAG) \
			D_Opco##i##(__THISP); \
		else \
			Opcode##i##(__THISP); \
		break;
#endif

#if BUILD_HUC6280
#define OPtd(i) \
	case 0x##i##: \
		if (__THIS__.P & T_FLAG) \
			if (__THIS__.P & D_FLAG) \
				TD_Opc##i##(__THISP); \
			else \
				T_Opco##i##(__THISP); \
		else \
			if (__THIS__.P & D_FLAG) \
				D_Opco##i##(__THISP); \
			else \
				Opcode##i##(__THISP); \
		break;
#define OPt_(i) \
	case 0x##i##: \
		if (__THIS__.P & T_FLAG) \
			T_Opco##i##(__THISP); \
		else \
			Opcode##i##(__THISP); \
		break;
#else
#define OPtd OP_d
#define OPt_ OP__
#endif

/* BS - allowing illegal opcode implementation */
#if ILLEGAL_OPCODES
#define OPxx OP__
#else
#define OPxx(i)
#endif

static void Inline K_OPEXEC(__CONTEXT)
{
	Uword opcode = __THIS__.lastcode = K_READNP(__THISP_ KAI_IMM(__THISP));
	KI_ADDCLOCK(__THISP_ cl_table[opcode]);
	switch(opcode)
	{
		OP__(00)	OPt_(01)	OPxx(02)	OPxx(03)	OPxx(04)	OPt_(05)	OP__(06)	OPxx(07)
		OP__(08)	OPt_(09)	OP__(0A)	OPxx(0B)	OPxx(0C)	OPt_(0D)	OP__(0E)	OPxx(0F)
		OP__(10)	OPt_(11)	OPxx(12)	OPxx(13)	OPxx(14)	OPt_(15)	OP__(16)	OPxx(17)
		OP__(18)	OPt_(19)	OPxx(1A)	OPxx(1B)	OPxx(1C)	OPt_(1D)	OP__(1E)	OPxx(1F)
		OP__(20)	OP__(21)	OPxx(22)	OPxx(23)	OP__(24)	OP__(25)	OP__(26)	OPxx(27)
		OP__(28)	OP__(29)	OP__(2A)	OPxx(2B)	OP__(2C)	OP__(2D)	OP__(2E)	OPxx(2F)
		OP__(30)	OPt_(31)	OPxx(32)	OPxx(33)	OPxx(34)	OPt_(35)	OP__(36)	OPxx(37)
		OP__(38)	OPt_(39)	OPxx(3A)	OPxx(3B)	OPxx(3C)	OPt_(3D)	OP__(3E)	OPxx(3F)
		OP__(40)	OPt_(41)	OPxx(42)	OPxx(43)	OPxx(44)	OPt_(45)	OP__(46)	OPxx(47)
		OP__(48)	OPt_(49)	OP__(4A)	OPxx(4B)	OP__(4C)	OPt_(4D)	OP__(4E)	OPxx(4F)
		OP__(50)	OPt_(51)	OPxx(52)	OPxx(53)	OPxx(54)	OPt_(55)	OP__(56)	OPxx(57)
		OP__(58)	OPt_(59)	OPxx(5A)	OPxx(5B)	OPxx(5C)	OPt_(5D)	OP__(5E)	OPxx(5F)
		OP__(60)	OPtd(61)	OPxx(62)	OPxx(63)	OPxx(64)	OPtd(65)	OP__(66)	OPxx(67)
		OP__(68)	OPtd(69)	OP__(6A)	OPxx(6B)	OP__(6C)	OPtd(6D)	OP__(6E)	OPxx(6F)
		OP__(70)	OPtd(71)	OPxx(72)	OPxx(73)	OPxx(74)	OPtd(75)	OP__(76)	OPxx(77)
		OP__(78)	OPtd(79)	OPxx(7A)	OPxx(7B)	OPxx(7C)	OPtd(7D)	OP__(7E)	OPxx(7F)

		OPxx(80)	OP__(81)	OPxx(82)	OPxx(83)	OP__(84)	OP__(85)	OP__(86)	OPxx(87)
		OP__(88)	OPxx(89)	OP__(8A)	OPxx(8B)	OP__(8C)	OP__(8D)	OP__(8E)	OPxx(8F)
		OP__(90)	OP__(91)	OPxx(92)	OPxx(93)	OP__(94)	OP__(95)	OP__(96)	OPxx(97)
		OP__(98)	OP__(99)	OP__(9A)	OPxx(9B)	OPxx(9C)	OP__(9D)	OPxx(9E)	OPxx(9F)
		OP__(A0)	OP__(A1)	OP__(A2)	OPxx(A3)	OP__(A4)	OP__(A5)	OP__(A6)	OPxx(A7)
		OP__(A8)	OP__(A9)	OP__(AA)	OPxx(AB)	OP__(AC)	OP__(AD)	OP__(AE)	OPxx(AF)
		OP__(B0)	OP__(B1)	OPxx(B2)	OPxx(B3)	OP__(B4)	OP__(B5)	OP__(B6)	OPxx(B7)
		OP__(B8)	OP__(B9)	OP__(BA)	OPxx(BB)	OP__(BC)	OP__(BD)	OP__(BE)	OPxx(BF)
		OP__(C0)	OP__(C1)	OPxx(C2)	OPxx(C3)	OP__(C4)	OP__(C5)	OP__(C6)	OPxx(C7)
		OP__(C8)	OP__(C9)	OP__(CA)	OPxx(CB)	OP__(CC)	OP__(CD)	OP__(CE)	OPxx(CF)
		OP__(D0)	OP__(D1)	OPxx(D2)	OPxx(D3)	OPxx(D4)	OP__(D5)	OP__(D6)	OPxx(D7)
		OP__(D8)	OP__(D9)	OPxx(DA)	OPxx(DB)	OPxx(DC)	OP__(DD)	OP__(DE)	OPxx(DF)
		OP__(E0)	OP_d(E1)	OPxx(E2)	OPxx(E3)	OP__(E4)	OP_d(E5)	OP__(E6)	OPxx(E7)
		OP__(E8)	OP_d(E9)	OP__(EA)	OPxx(EB)	OP__(EC)	OP_d(ED)	OP__(EE)	OPxx(EF)
		OP__(F0)	OP_d(F1)	OPxx(F2)	OPxx(F3)	OPxx(F4)	OP_d(F5)	OP__(F6)	OPxx(F7)
		OP__(F8)	OP_d(F9)	OPxx(FA)	OPxx(FB)	OPxx(FC)	OP_d(FD)	OP__(FE)	OPxx(FF)

#if BUILD_HUC6280 || BUILD_M65C02
		OP__(34)	/* 34 - BIT - Zero Page,X */
		OP__(3C)	/* 3C - BIT - Absolute,X */
		OP__(80)	/* 80 - BRA */
		OP__(3A)	/* 3A - DEA */
		OP__(1A)	/* 1A - INA */



		OP__(89)	/* 89 - BIT - Immediate */

		OP__(04)	OP__(0C)	/* TSB */
		OP__(14)	OP__(1C)	/* TRB */

		OPt_(12)	/* 12 - ORA - (Indirect) */
		OPt_(32)	/* 32 - AND - (Indirect) */
		OPt_(52)	/* 52 - EOR - (Indirect) */
		OPtd(72)	/* 72 - ADC - (Indirect) */
		OP__(92)	/* 92 - STA - (Indirect) */
		OP__(B2)	/* B2 - LDA - (Indirect) */
		OP__(D2)	/* D2 - CMP - (Indirect) */
		OP_d(F2)	/* F2 - SBC - (Indirect) */

		OP__(DA)	OP__(5A)	OP__(FA)	OP__(7A)	/* PHX PHY PLX PLY */
		OP__(64)	OP__(9C)	OP__(74)	OP__(9E)	/* STZ */
		OP__(7C)	/* 7C - JMP - Absolute,X */
#endif
#if 0 && BUILD_M65C02
		OP__(CB)	/* WAI */
		OP__(DB)	/* STP */
#endif
#if BUILD_HUC6280
		OP__(0F)	OP__(1F)	OP__(2F)	OP__(3F)	/* BBRi */
		OP__(4F)	OP__(5F)	OP__(6F)	OP__(7F)
		OP__(8F)	OP__(9F)	OP__(AF)	OP__(BF)	/* BBSi*/
		OP__(CF)	OP__(DF)	OP__(EF)	OP__(FF)
		OP__(44)	/* 44 - BSR */
		OP__(62)	OP__(82)	OP__(C2)	/* CLA CLX CLY */
		OP__(07)	OP__(17)	OP__(27)	OP__(37)	/* RMBi */
		OP__(47)	OP__(57)	OP__(67)	OP__(77)
		OP__(87)	OP__(97)	OP__(A7)	OP__(B7)	/* SMBi */
		OP__(C7)	OP__(D7)	OP__(E7)	OP__(F7)
		OP__(02)	OP__(22)	OP__(42)	/* SXY SAX SAY */
		OP__(F4)	/* F4 - SET */
		OP__(03)	OP__(13)	OP__(23)	/* ST0 ST1 ST2 */
		OP__(43)	OP__(53)	/* TMAi TAMi */
		OP__(83)	OP__(93)	OP__(A3)	OP__(B3)	/* TST */
		OP__(73)	OP__(C3)	OP__(D3)	OP__(E3)	OP__(F3)	/* block */
		OP__(54)	OP__(D4)	/* CSL CSH */
#endif
	}
}
