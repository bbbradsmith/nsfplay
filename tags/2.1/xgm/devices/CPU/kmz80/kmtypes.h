/*
  KMxxx common header
  by Mamiya
*/

#ifndef KMTYPES_H_
#define KMTYPES_H_
#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned int Uint32;
typedef signed int Int32;
typedef unsigned char Uint8;
typedef signed char Int8;

#if defined(_MSC_VER)
#elif defined(__BORLANDC__)
#elif defined(__GNUC__)
#ifndef __inline
#define __inline __inline__
#endif
#else
#ifndef __inline
#define __inline static
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif
