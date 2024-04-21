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

#ifdef DEBUG
#define NSFP_DEBUG(...) { nsfp::debug_printf(__VA_ARGS__); }
#else
#define NSFP_DEBUG(...) {}
#endif

// NSFCore structure, code members defined in core.cpp

typedef struct NSFCore_
{
	sint32 set[NSFP_SET_COUNT]; // all integer settings (values for string settings will index set_str)
	const char* set_str[NSFP_SETSTR_COUNT];
	bool set_str_free[NSFP_SETSTR_COUNT]; // true if string setting

	static NSFCore* create(); // After create: ->set_... then ->finalize.
	static void destroy(NSFCore* core); // Calls ->release before freeing the core.
	void finalize(); // finishes creation after create and initial settings
	void release(); // called by destroy, releases all owned allocations

	void set_default();
	bool set_ini(const char* ini);
	bool set_init(const NSFSetInit* init);

} NSFCore;

namespace nsfp {

// core.cpp

extern "C" {
extern void (*debug_print_callback)(const char* msg);
extern void (*fatal_callback)(const char* msg);
}

void* alloc(size_t size);
void free(void* ptr);
void debug_printf(const char* fmt,...); // only works if DEBUG defined
void debug(const char* msg);
void fatal(const char* msg);

} // namespace nsfp

#endif // __CORE_PCH__
