#pragma once

// NSFPlay core library public interface

// General notes:
// - NSFCore assumes single threaded access, do not call multiple core state modifying functions concurrently.
// - All strings are UTF-8.
// - For all functions that return a const char*, this will point to an internal temporary buffer
//   which is only valid until the next nsfplay call that returns a const char*.
// - The song index begins at 0 for the interface, but when presented to the user a +1 should be used,
//   so that the first song in the NSF is song 1.

#include <stdint.h> // explicit size integer types
#include <stddef.h> // NULL
#include <stdio.h> // FILE

// auto-generated enumerations for settings and properties
#include "nsfplayenums.h"

extern "C"
{

// hidden implementation
struct NSFCore_;
typedef struct NSFCore_ NSFCore;


typedef struct
{
	int32_t setenum; // -1 to terminate an init array
	int32_t val_int;
	const char* val_str; // NULL if not string
} NSFSetInit;


// create or destroy an core instance
// - see set_init/set_init below for an explanation of the create parameters
// - a null ini_data/init will just use the default settings
NSFCore* nsfplay_create(const char* ini_data);
NSFCore* nsfplay_create_init(const NSFSetInit* init);
void nsfplay_destroy(NSFCore* core);


// logging and error handling

// set a callback to log errors
// - errors are silently sent to last_error by default,
//   but this provides a way to catch all of them as they happen.
// - error msg will not end with newline.
// - global, can be set before creating a core.
void nsfplay_set_error_log(void (*error_callback)(const char* msg));

// for debug builds, sets a custom debug output function for diagnostics
// - default will print to stdout.
// - debug msg will not end with newline.
// - global, can be set before creating a core.
void nsfplay_set_debug_print(void (*debug_print_callback)(const char* msg));

// set a callback to handle a fatal error
// - May be called if out of memory or some other irrecoverable error.
// - Allows you to report the error message in some other way before exiting, luck willing.
// - If no callback is provided, or if it returns, it will print msg to stderr,
//   then std::exit(-1) to close the application.
// - fatal msg will not end with newline.
// - global, can be set before creating a core.
void nsfplay_set_fatal(void (*fatal_callback)(const char* msg));

// returns a localized string describing the last error for this core
//  - NULL if there has been no logged error since the last call to nsfplay_test_error
//  - only remembers the latest error, use set_error_log if you need to catch them all
//  - once returned, the error state will be cleared, and subsequent calls will return NULL until another error is caught
const char* nsfplay_last_error(const NSFCore* core);


// reset all settings to default values
void nsfplay_set_default(NSFCore* core);
// apply ini file
// - returns false and reports error if any lines could not be parsed
// - ini_data is a null terminated string
// - settings are separated by a line ending, any combination of cr and/or lf is accepted
// - blank lines are ignored, anything after # will be ignored until the next line
// - each line has a setting key, an =, and the value
// - whitespace can be used before/after/around the key = and value
// - string values will have their left and right whitespace trimmed,
//   quotes are not treated as special, so a string cannot start or end with a space
bool nsfplay_set_ini(NSFCore* core, const char* ini_data);
// apply init array
// - easier way to apply many settings from code
// - an init array can be used instead of ini to initialize from code, terminated by an entry with setenum -1.
// - returns false and reports error if any entries could not be set
bool nsfplay_set_init(NSFCore* core, const NSFSetInit* init);

// generate ini file

// generate an ini line for a current settings
// - does not include newline
// - iterate from 0 to NSFP_SET_COUNT-1 to generate a complete ini file
const char* nsfplay_ini_line(const NSFCore* core, int32_t setenum);
// write a complete ini to an open file
// - uses fprintf and \n, and assumes the file is in "wt" mode for platform-appropriate line endings
// - does not close the file
void nsfplay_ini_write(const NSFCore* core, FILE* f);


// settings by setenum
// - use the provided NSFP_SET_x enumerations, because setenum values are subject to change
// - set returns false if setenum is out of bounds, or the wrong value type was used
// - get returns 0 or NULL if out of bounds, or wrong type
bool nsfplay_set_int(NSFCore* core, int32_t setenum, int32_t value);
bool nsfplay_set_str(NSFCore* core, int32_t setenum, const char* value);
int32_t nsfplay_get_int(const NSFCore* core, int32_t setenum);
const char* nsfplay_get_str(const NSFCore* core, int32_t setenum);

// settings by string key
bool nsfplay_set_key_int(NSFCore* core, const char* key, int32_t value);
bool nsfplay_set_key_str(NSFCore* core, const char* key, const char* value);
int32_t nsfplay_get_key_int(const NSFCore* core, const char* key);
const char* nsfplay_get_key_str(const NSFCore* core, const char* key);

// information about settings, useful for UI display

typedef struct
{
	// all const char* in this structure point to static strings, permanently available
	int32_t group;
	const char* key; // ini text key
	const char* name; // localized name, according to current language setting
	const char* desc; // localized description, according to current language setting
	bool is_string;
	int32_t default_int; // 0 if is_string
	int32_t min_int;
	int32_t max_int;
	const char* list; // if not NULL, contains a series of (1+max_int) localized null terminated strings naming each option
	const char* default_str; // NULL only if !is_string
	// TODO: slider min/max 
	// TODO: display hint: int, hex16, hex32, color, hotkey
} NSFSetInfo;

typedef struct
{
	const char* key; // not used by ini, but does give a non-localized permanent string key for this group
	const char* name; // localized name
	const char* desc; // localized description
} NSFSetGroupInfo;

NSFSetInfo nsfplay_set_info(const NSFCore* core, int32_t setenum);
NSFSetGroupInfo nsfplay_set_group_info(const NSFCore* core, int32_t group);
int32_t nsfplay_set_enum(const char* key); // -1 if not found
int32_t nsfplay_group_enum(const char* key); // -1 if not found


// load/change the current NSF file
// - load makes an internal copy of nsf_data
// - assume will instead use nsf_data directly, assuming the pointer will remain valid
// - returns false if nsf_data could not be parsed
// - a NULL nsf_data may be used to "unload" the current NSF,
//   but even without an NSF, the core may still be used via direct emulation access
bool nsfplay_load(NSFCore* core, const void* nsf_data, uint32_t nsf_size);
bool nsfplay_assume(NSFCore* core, const void* nsf_data, uint32_t nsf_size);


// song control
uint32_t nsfplay_song_count(const NSFCore* core); // number of songs in loaded NSF
bool nsfplay_song(NSFCore* core, uint8_t song); // set song, false if song out of bounds, automatically calls song_play
void nsfplay_song_play(NSFCore* core); // resets the song and executes its INIT routine
void nsfplay_seek(NSFCore* core, uint64_t samples);
uint64_t nsfplay_samples_played(const NSFCore* core); // samples since song_play

// advance emulation and render sound samples
// - every two elements of stereo_output alternates left channel, right channel (length must be 2*samples)
// - stereo_output can be NULL if the output isn't needed
// - returns number of samples rendered, may be less than samples if song is finished (will zero fill unused output samples)
uint32_t nsfplay_render(NSFCore* core, uint32_t samples, int16_t* stereo_output);
// internal mixing is done at 32-bits, this can be delivered if desired
// - ensure the output volume is low enough to prevent overflow (32-bit render is not able to clip out of range samples)
uint32_t nsfplay_render32(NSFCore* core, uint32_t samples, int32_t* stereo_output);


// direct emulation access
uint8_t nsfplay_emu_peek(const NSFCore* core, uint16_t address); // peek at memory, no read side effects
uint8_t nsfplay_emu_read(NSFCore* core, uint16_t address); // read memory, some registers have read side effects
void nsfplay_emu_poke(NSFCore* core, uint16_t address, uint8_t value); // write to memory
void nsfplay_emu_reg_set(NSFCore* core, char reg, uint16_t value); // reg is one of A, X, Y, S, P (flags), * (PC)
uint16_t nsfplay_emu_reg_get(const NSFCore* core, char reg);
void nsfplay_emu_init(NSFCore* core, uint8_t song); // set song and reset for play, set PC to beginning of INIT
void nsfplay_emu_run(NSFCore* core, uint32_t cycles);
uint32_t nsfplay_emu_run_frame(NSFCore* core); // runs until start of next frame, or end of INIT if just started, returns cycles elapsed
uint32_t nsfplay_emu_run_instruction(NSFCore* core); // runs to end of current instruction, returns cycles elapsed
const char* nsfplay_emu_trace(const NSFCore* core); // trace prints register state and next instruction
void nsfplay_emu_gamepad(const NSFCore* core, int32_t pad, uint32_t report); // send a gamepad report for debugging (LSB is read first), reset to 0 on init

uint32_t nsfplay_emu_samples_pending(const NSFCore* core); // number of sound samples due to emu_run that have not been rendered out yet
void nsfplay_emu_cancel_pending(NSFCore* core);
uint64_t nsfplay_emu_cycles(const NSFCore* core); // cycles since song_play
uint32_t nsfplay_emu_cycles_to_next_sample(const NSFCore* core); // cycles until next pending sample is generated


// NSF properties
// - prop parameter should use the NSFP_PROP_key enumerations, as the values are subject to change
// - song prop uses NSFP_SONG_PROP_key enumerations
// - NSFP_PROP_COUNT and NSFP_SONG_PROP_COUNT can be used iterate over all properties

#define NSFP_PROP_TYPE_INVALID   0
#define NSFP_PROP_TYPE_INT       1
#define NSFP_PROP_TYPE_LONG      2
#define NSFP_PROP_TYPE_STR       3
#define NSFP_PROP_TYPE_LINES     4 
#define NSFP_PROP_TYPE_BLOB      5
typedef struct
{
	const char* key; // permanent string ID
	const char* name; // localized name
	int32_t type;
	// TODO: display hint (shared with NSFSetInfo)
} NSFPropInfo;

bool nsfplay_prop_exists(const NSFCore* core, int32_t prop);
bool nsfplay_songprop_exists(const NSFCore* core, int32_t song, int32_t prop);
NSFPropInfo nsfplay_prop_info(const NSFCore* core, int32_t prop); // type will be INVALID if not present
NSFPropInfo nsfplay_songprop_info(const NSFCore* core, int32_t songprop);

int32_t nsfplay_prop_int(const NSFCore* core, int32_t prop);
int64_t nsfplay_prop_long(const NSFCore* core, int32_t prop);
const char* nsfplay_prop_str(const NSFCore* core, int32_t prop); // NULL if not found or wrong type
int32_t nsfplay_prop_lines(const NSFCore* core, int32_t prop); // returns line count (-1 if not found), prepares for first prop_line
const char* nsfplay_prop_line(const NSFCore* core); // returns next line  (NULL if no more lines)
const void* nsfplay_prop_blob(const NSFCore* core, uint32_t* blob_size, int32_t prop); // blob_size written if not NULL

// song -1 will use the current song
int32_t nsfplay_songprop_int(const NSFCore* core, int32_t prop, int32_t song=-1);
int64_t nsfplay_songprop_long(const NSFCore* core, int32_t prop, int32_t song=-1);
const char* nsfplay_songprop_str(const NSFCore* core, int32_t prop, int32_t song=-1);
int32_t nsfplay_songprop_lines(const NSFCore* core, int32_t prop, int32_t song=-1); // prepares nsfplay_prop_line
const void* nsfplay_songprop_blob(const NSFCore* core, uint32_t* blob_size, int32_t prop, int32_t song=-1);

// NSFe or NSF2 chunks can be fetched for manual inspection
// - fourcc does not need a terminating 0, only the first 4 characters will be used
// - chunk_size will be written if not NULL
const void* nsfplay_chunk(const NSFCore* core, const char* fourcc, uint32_t* chunk_size);


// Channel info

typedef struct // NSFP_UNIT_key (< NSFP_UNIT_COUNT)
{
	const char* key; // permanent string ID
	const char* name; // localized name
	bool active; // whether this unit is active for this NSF
} NSFChannelUnit;

typedef struct // NSFP_CHANNEL_key (< NSFP_CHANNEL_COUNT)
{
	int32_t unit; // unit
	const char* key; // permanent string ID
	const char* name; // localized name
} NSFChannelInfo;

typedef struct
{
	int32_t channel; // global channel index (-1 if invalid)
	float frequency; // calculated expected frequency
	int32_t volume; // volume, a register value or 0 if muted
	uint32_t reg_period; // register period or frequency value
	uint32_t reg_volume; // register volume valud
	uint32_t reg_extra[3]; // extra data (channel specific)
} NSFChannelState;

// every channel of every expansion audio unit has a unique global index
NSFChannelUnit nsfplay_channel_unit(const NSFCore* core, int32_t unit); // information about a unit
NSFChannelInfo nsfplay_channel_info(int32_t global_channel); // information about a global channel

// an NSF has a list of active channels, and their playback state can be queried
int32_t nsfplay_channel_count(const NSFCore* core); // number of active channels
NSFChannelState nsfplay_channel_state(const NSFCore* core, int32_t active_channel);
// some channels have an extended state (e.g. FDS or N163 waveform)
// - returns number of bytes of extended state
// - copies up to data_size bytes to data if data is not NULL
uint32_t nsfplay_channel_state_ex(const NSFCore* core, int32_t active_channel, void* data, uint32_t data_size);


// convenience functions for time conversion
uint64_t nsfplay_time_to_samples(const NSFCore* core, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds);
uint64_t nsfplay_time_to_cycles(const NSFCore* core, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds);
void nsfplay_samples_to_time(const NSFCore* core, uint64_t samples, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds);
void nsfplay_cycles_to_time(const NSFCore* core, uint64_t cycles, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds);

// other text strings adapted for the current locale, see: NSFP_TEXT_key
// - the returned string pointer is static and has permanent lifetime
const char* nsfplay_local_text(const NSFCore* core, int32_t textenum);


} // extern "C"
