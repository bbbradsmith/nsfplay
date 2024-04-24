#pragma once
#ifndef __CORE_PCH__
#define __CORE_PCH__
// core.h
//   All internal core shared functions and data should be defined here.
//   Most modules can define their interface functions here,
//   preferring one central header over individual module headers,
//   but modules with a state struct (audio units, etc.)
//   may have their own header included here.
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
	#define DEBUG_ALLOC 1
#else
	#define NSFP_DEBUG(...) {}
	#define DEBUG_ALLOC 0
#endif

#ifndef NSFP_NOTEXT
	// define NSFP_NOTEXT=1 to strip all unnecessary text from the build
	//   this disables ini parsing, error messages will become blank strings, and list keys will also become blank
	#define NSFP_NOTEXT 0
#endif

// convenient accessors
#define SETTING(setenum_) (setting[NSFP_SET_##setenum_])
#define PROP(propenum_) (nsf_prop_int(NSFP_PROP_##propenum_))
#define PROPS(propenum_) (nsf_prop_str(NSFP_PROP_##propenum_))
#define SONGPROP(songpropenum_) (nsf_prop_int(NSFP_SONGPROP_$$songpropenum_,song_current))
#define SONGPROPS(songpropenum_) (nsf_prop_str(NSFP_SONGPROP_$$songpropenum_,song_current))

// NSFCore structure, code members defined in core.cpp unless otherwised marked

typedef struct NSFCore_
{
	// core state

	// general rule of thumb:
	//   Try not to store unnecessary state.
	//   Use a setting directly instead of copying it, where possible.
	//   Use prop/songprop to look up instead of store values.
	//   Can make exceptions for performance:
	//     Volume applies per-cycle/sample, should store that with fixed point adjustments.
	//     Square phase reset only applies per emulated-write, just use the setting directly.
	//     Settings are fast, props are slow, but props are generally only needed at song start.
	//   Consider whether setting changes can apply immediately (and add to set_apply if they do).

	// text and error output buffers
	mutable const char* error_last;
	mutable char error_last_buffer[256]; // error_last may point to this for formatted errors
	mutable char temp_text[1024]; // used for returned text information
	mutable const char* prop_lines;

	// settings
	sint32 setting[NSFP_SET_COUNT]; // integer settings (can read directly, write with set_int)
	const char* setting_str[NSFP_SETSTR_COUNT]; // string settings, indexed by value in setting[], (use set/get_str to access)
	bool setting_str_free[NSFP_SETSTR_COUNT]; // true if string setting was allocated (not default), managed by set_str/destroy

	// emulation
	const uint8* nsf;
	uint32 nsf_size;
	bool nsf_free; // true if nsf was allocated (load rather than assume)
	bool nsf_bin; // binary file at $6000 instead of NSF

	uint8 ram0000[0x00800-0x0000]; // $0000-07FF 2k of on-board RAM
	uint8 ram6000[0x10000-0x6000]; // $6000-FFFF 40k of RAM or other workspace
	const uint8* rom; // pointer to read only NSF data, aligned for padding (first/last banks may be masked by pad0/pad1)
	uint8* pad0; // 4k first bank, if it needs padding due to LOAD address (may reuse ram6000 at E000)
	uint8* pad1; // 4k last bank, if it needs padding due to not filling a complete bank (may reuse ram6000 at F000)
	unsigned int bank_last; // last bank in data (pad1 if not NULL)
	const uint8* rpage[16]; // currently assigned 4k bank pages for reading (null for open bus)
	uint8* wpage[16]; // currently assigned 4k bank pages for writing

	// playback
	uint8 song_current;

	// audio units
	// TODO these should be static POD structures, each with a pointer to the core
	// arrays for each:
	//   read function (flags resync if needed)
	//     probably need a read-sync stack (to sync before a read)
	//   write function (flags resync if needed)
	//     probably need a write-sync stack to sync before a write
	//     then do sync, then the write stack
	//   render function
	//   (these can have variants, e.g. render to aux buffer for side mixing)
	//   audio filters could go in the render stack, if okay to operate at high frequency

	// audio filters
	// TODO
	// stack of function pointers (allows processes to swap out, reorder) to operate at output frequency, after resample)

	// audio rendering
	// TODO
	// output buffer (32-bit), secondary output buffer (these can be small by default, might need more for direct emu drive instead of render out)
	// cycle buffer (32-bit), secondary (needs to be wide enough for resampling, but not any larger)
	// resample kernel
	//   maybe audio units can have a memory setup interface where each says how much it needs, then it's allocated, then the memory is handed out
	// resampler function pointer

	// interface

	static NSFCore* create();
	static void destroy(NSFCore* core); // Calls ->release before freeing the core.
	void release(); // internal: called by destroy, releases all owned allocations

	const char* last_error() const; // returns last error message, NULL if none since last check
	void set_error(sint32 textenum,...) const; // sets last error and generates error callback
	void set_ini_error(int linenum, sint32 textenum,...) const; // for ini files (-1 if no ini)
	void set_error_raw(const char* fmt,...) const; // set_error with localized errors is preferred, but this can send raw text errors for debug purposes

	void set_default(); // restore default settings
	bool set_ini(const char* ini);
	bool set_init(const NSFSetInit* init);
	bool set_int(sint32 setenum, sint32 value); // integer setting (sets error if false)
	bool set_str(sint32 setenum, const char* value, sint32 len=-1); // string setting, len truncates, len<0 will strlen (sets error if false)
	void set_apply(); // call to apply changed settings now
	sint32 get_int(sint32 setenum) const;
	const char* get_str(sint32 setenum) const; // use this instead of manually de-indexing setting_str

	static sint32 set_enum(const char* key, int len=-1); // len truncates, len<0 uses strlen
	static sint32 group_enum(const char* key, int len=-1);
	NSFSetInfo set_info(sint32 setenum) const;
	NSFSetGroupInfo group_info(sint32 group) const;

	const char* ini_line(sint32 setenum) const;
	void ini_write(FILE* f) const;
	bool parse_ini_line(const char* line, int len, int linenum); // linenum=-1 to parse a line with no INI file context

	bool load(const uint8* data, uint32 size, bool assume, bool bin=false);
	NSFPropInfo prop_info(sint32 prop, bool song=false) const; // if song, prop is a SONGPROP

	const char* local_text(sint32 textenum) const; // NSFP_TEXT_x for curent locale (local_text(0) is a default error string)
	static const char* local_text(sint32 textenum, sint32 locale); // NSFP_TEXT_x for specific locale

	// nsf.cpp
	bool nsf_parse(bool bin);
	const uint8* nsfe_chunk(uint32 fourcc, uint32* chunk_size) const; // fourcc is packed little-endian into uint32
	const uint8* nsfe_chunk(const char* fourcc, uint32* chunk_size) const;
	bool nsf_prop_exists(sint32 prop, sint32 song=-1) const; // if song>=0 prop is a SONGPROP
	sint32 nsf_prop_int(sint32 prop, sint32 song=-1) const;
	sint64 nsf_prop_long(sint32 prop, sint32 song=-1) const;
	const char* nsf_prop_str(sint32 prop, sint32 song=-1) const;
	sint32 nsf_prop_lines(sint32 prop, sint32 song=-1) const;
	const char* nsf_prop_line() const;
	const uint8* nsf_prop_blob(uint32* blob_size, sint32 prop, sint32 song=-1) const;

} NSFCore;

namespace nsfp {

// core.cpp

extern "C" {
extern void (*error_callback)(const char* msg);
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
