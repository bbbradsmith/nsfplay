#pragma once
#ifndef __CORE_PCH__
#define __CORE_PCH__
// core.h
//   All internal core shared functions and data should be defined here,
//   preferring one central header over individual module headers,
//   unless they are deliberately self-contained, like an external library.
//
//   core.h should be included as the first code line in every core source file.
//   Some builds will force-include it as a precompiled header.
//   The include guards allow GCC to use the precompiled version in another directory
//   without redundantly including this one.

#include <nsfplaycore.h>

typedef int8_t    sint8;
typedef int16_t   sint16;
typedef int32_t   sint32;
typedef int64_t   sint64;

typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;

#endif // __CORE_PCH__
