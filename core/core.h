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
	mutable const char* error_last;
	char error_last_buffer[256]; // error_last may point to this for formatted errors
	char temp_text[1024]; // used for returned text information

	sint32 setting[NSFP_SET_COUNT]; // integer settings (can read directly, write with set_int)
	const char* setting_str[NSFP_SETSTR_COUNT]; // string settings, indexed by value in setting[], (use set/get_str to access)
	bool setting_str_free[NSFP_SETSTR_COUNT]; // true if string setting (managed by set_str/destroy)

	static NSFCore* create(); // After create: ->set_... then ->finalize.
	static void destroy(NSFCore* core); // Calls ->release before freeing the core.
	void finalize(); // finishes creation after create and initial settings
	void release(); // called by destroy, releases all owned allocations

	const char* last_error() const; // returns last error message, NULL if none since last check
	void set_last_error(sint32 textenum,...);

	void set_default(); // restore default settings
	bool parse_ini_line(const char* line, int len, int linenum); // used by set_ini
	bool set_ini(const char* ini);
	bool set_init(const NSFSetInit* init);
	bool set_int(sint32 setenum, sint32 value); // integer setting (sets error if false)
	bool set_str(sint32 setenum, const char* value, sint32 len=-1); // string setting, len truncates, len<0 will strlen (sets error if false)
	sint32 get_int(sint32 setenum) const;
	const char* get_str(sint32 setenum) const; // use this instead of manually de-indexing setting_str

	static sint32 set_enum(const char* key, int len=-1); // len truncates, len<0 uses strlen
	static sint32 group_enum(const char* key, int len=-1);

	// TODO after changing settings, they need to be applied at some point, some might be immediate

	const char* local_text(sint32 textenum) const; // NSFP_TEXT_x for curent locale
	static const char* local_text(sint32 textenum, sint32 locale); // NSFP_TEXT_x for specific locale

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
