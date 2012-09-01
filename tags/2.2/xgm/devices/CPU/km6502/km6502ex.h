External void K_EXEC(__CONTEXT)
{
	if (__THIS__.iRequest)
	{
		if (__THIS__.iRequest & IRQ_INIT)
		{
#if BUILD_HUC6280
			__THIS__.lowClockMode = 1;
#endif
			__THIS__.A = 0;
			__THIS__.X = 0;
			__THIS__.Y = 0;
			__THIS__.S = 0xFF;
			__THIS__.P = Z_FLAG | R_FLAG | I_FLAG;
			__THIS__.iRequest = 0;
			__THIS__.iMask = ~0;
			KI_ADDCLOCK(__THISP_ 7);
			return;
		}
		else if (__THIS__.iRequest & IRQ_RESET)
		{
#if BUILD_HUC6280
			__THIS__.lowClockMode = 1;
			K_WRITEMPR(__THISP_ 0x80, 0x00);	/* IPL(TOP OF ROM) */
#endif
			__THIS__.A = 0;
			__THIS__.X = 0;
			__THIS__.Y = 0;
			__THIS__.S = 0xFF;
			__THIS__.P = Z_FLAG | R_FLAG | I_FLAG;
			__THIS__.PC = KI_READWORD(__THISP_ VEC_RESET);
			__THIS__.iRequest = 0;
			__THIS__.iMask = ~0;
		}
		else if (__THIS__.iRequest & IRQ_NMI)
		{
			KM_PUSH(__THISP_ RTO8(__THIS__.PC >> 8));
			KM_PUSH(__THISP_ RTO8(__THIS__.PC));
			KM_PUSH(__THISP_ (Uword)(__THIS__.P | R_FLAG | B_FLAG));
#if BUILD_M65C02 || BUILD_HUC6280
			__THIS__.P = (__THIS__.P & ~(D_FLAG | T_FLAG)) | I_FLAG;
			__THIS__.iRequest &= ~IRQ_NMI;
#else
			__THIS__.P = (__THIS__.P & ~T_FLAG) | I_FLAG;	/* 6502 bug */
			__THIS__.iRequest &= ~(IRQ_NMI | IRQ_BRK);
#endif
			__THIS__.PC  =  KI_READWORD(__THISP_ VEC_NMI);
			KI_ADDCLOCK(__THISP_ 7);
		}
		else if (__THIS__.iRequest & IRQ_BRK)
		{
			KM_PUSH(__THISP_ RTO8(__THIS__.PC >> 8));
			KM_PUSH(__THISP_ RTO8(__THIS__.PC));
			KM_PUSH(__THISP_ (Uword)(__THIS__.P | R_FLAG | B_FLAG));
#if BUILD_M65C02 || BUILD_HUC6280
			__THIS__.P = (__THIS__.P & ~(D_FLAG | T_FLAG)) | I_FLAG;
#else
			__THIS__.P = (__THIS__.P & ~T_FLAG) | I_FLAG;	/* 6502 bug */
#endif
			__THIS__.iRequest &= ~IRQ_BRK;
			__THIS__.PC = KI_READWORD(__THISP_ VEC_BRK);
			KI_ADDCLOCK(__THISP_ 7);
		}
		else if (__THIS__.P & I_FLAG)
		{
			/* interrupt disabled */
		}
#if BUILD_HUC6280
		else if (__THIS__.iMask & __THIS__.iRequest & IRQ_INT1)
		{
			KM_PUSH(__THISP_ RTO8(__THIS__.PC >> 8));
			KM_PUSH(__THISP_ RTO8(__THIS__.PC));
			KM_PUSH(__THISP_ (Uword)(__THIS__.P | R_FLAG | B_FLAG));
			__THIS__.P = (__THIS__.P & ~(D_FLAG | T_FLAG)) | I_FLAG;
			__THIS__.PC = KI_READWORD(__THISP_ VEC_INT1);
			KI_ADDCLOCK(__THISP_ 7);
		}
		else if (__THIS__.iMask & __THIS__.iRequest & IRQ_TIMER)
		{
			KM_PUSH(__THISP_ RTO8(__THIS__.PC >> 8));
			KM_PUSH(__THISP_ RTO8(__THIS__.PC));
			KM_PUSH(__THISP_ (Uword)(__THIS__.P | R_FLAG | B_FLAG));
			__THIS__.P = (__THIS__.P & ~(D_FLAG | T_FLAG)) | I_FLAG;
			__THIS__.PC = KI_READWORD(__THISP_ VEC_TIMER);
			KI_ADDCLOCK(__THISP_ 7);
		}
		else if (__THIS__.iMask & __THIS__.iRequest & IRQ_INT)
		{
			KM_PUSH(__THISP_ RTO8(__THIS__.PC >> 8));
			KM_PUSH(__THISP_ RTO8(__THIS__.PC));
			KM_PUSH(__THISP_ (Uword)((__THIS__.P | R_FLAG) & ~B_FLAG));
			__THIS__.P = (__THIS__.P & ~(D_FLAG | T_FLAG)) | I_FLAG;
			__THIS__.PC = KI_READWORD(__THISP_ VEC_INT);
			KI_ADDCLOCK(__THISP_ 7);
		}
#else
		else if (__THIS__.iMask & __THIS__.iRequest & IRQ_INT)
		{
			KM_PUSH(__THISP_ RTO8(__THIS__.PC >> 8));
			KM_PUSH(__THISP_ RTO8(__THIS__.PC));
			KM_PUSH(__THISP_ (Uword)((__THIS__.P | R_FLAG) & ~B_FLAG));
#if BUILD_M65C02
			__THIS__.P = (__THIS__.P & ~(D_FLAG | T_FLAG)) | I_FLAG;
#else
			__THIS__.P = (__THIS__.P & ~T_FLAG) | I_FLAG;	/* 6502 bug */
#endif
			__THIS__.iRequest &= ~IRQ_INT;
			__THIS__.PC = KI_READWORD(__THISP_ VEC_INT);
			KI_ADDCLOCK(__THISP_ 7);
		}
#endif
	}
	K_OPEXEC(__THISP);
}
