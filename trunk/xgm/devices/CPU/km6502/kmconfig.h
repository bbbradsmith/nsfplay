#ifndef KMCONFIG_H_
#define KMCONFIG_H_

/* general setting */
#if !defined(USE_USERPOINTER)
#define USE_USERPOINTER	1
#endif

#if !defined(USE_INLINEMMC)
#define USE_INLINEMMC	12	/* mmc page bits */
#endif

#if USE_INLINEMMC
#if defined(USE_CALLBACK)
#undef USE_CALLBACK
#endif
#define USE_CALLBACK	1
#else

#if !defined(USE_CALLBACK)
#define USE_CALLBACK	0
#endif

#endif

#if !defined(BUILD_FOR_SIZE) && !defined(BUILD_FOR_SPEED)
#define BUILD_FOR_SIZE	1
#define BUILD_FOR_SPEED	0
#endif

#if defined(_MSC_VER)
typedef unsigned int Uword;				/* (0-0xFFFF) */
typedef unsigned char Ubyte;			/* unsigned 8bit integer for table */
#ifndef Inline
#define Inline __inline
#endif
#define CCall __cdecl
#define FastCall __fastcall
#define RTO16(w) ((Uword)(w & 0xFFFF))	/* Round to 16bit integer */
#define RTO8(w) ((Uword)(w & 0xFF))		/* Round to  8bit integer */
#elif defined(__BORLANDC__)
typedef unsigned int Uword;				/* (0-0xFFFF) */
typedef unsigned char Ubyte;			/* unsigned 8bit integer for table */
#ifndef Inline
#define Inline __inline
#endif
#define CCall __cdecl
#define FastCall
#define RTO16(w) ((Uword)(w & 0xFFFF))	/* Round to 16bit integer */
#define RTO8(w) ((Uword)(w & 0xFF))		/* Round to  8bit integer */
#elif defined(__GNUC__)
typedef unsigned int Uword;				/* (0-0xFFFF) */
typedef unsigned char Ubyte;			/* unsigned 8bit integer for table */
#ifndef Inline
#define Inline __inline__
#endif
#define CCall
#define FastCall /* __attribute__((regparm(2))) */
#define RTO16(w) ((Uword)(w & 0xFFFF))	/* Round to 16bit integer */
#define RTO8(w) ((Uword)(w & 0xFF))		/* Round to  8bit integer */
#else
typedef unsigned int Uword;				/* (0-0xFFFF) */
typedef unsigned char Ubyte;			/* unsigned 8bit integer for table */
#ifndef Inline
#define Inline
#endif
#ifndef CCall
#define CCall
#endif
#ifndef FastCall
#define FastCall
#endif
#define RTO16(w) ((Uword)(w & 0xFFFF))	/* Round to 16bit integer */
#define RTO8(w) ((Uword)(w & 0xFF))		/* Round to  8bit integer */
#endif

#define Callback FastCall
#ifndef External
#define External extern
#endif
#ifndef USE_DIRECT_ZEROPAGE
#define USE_DIRECT_ZEROPAGE 0
#endif

/* advanced setting */

#if !BUILD_FOR_SIZE && !BUILD_FOR_SPEED
#define USE_FL_TABLE	1				/* Use table(512bytes) for flag */
#define OpsubCall CCall					/* OP code sub */
#define MasubCall CCall					/* addressing sub */
#endif

/* auto setting */

#if BUILD_FOR_SIZE
#define USE_FL_TABLE 1
#define OpsubCall FastCall
#define MasubCall FastCall
#define OpcodeCall Inline
#elif BUILD_FOR_SPEED
#define USE_FL_TABLE 1
#define OpsubCall Inline
#define MasubCall FastCall
#define OpcodeCall Inline
#else
#define OpcodeCall Inline
#endif

#endif	/* KMCONFIG_H_ */
