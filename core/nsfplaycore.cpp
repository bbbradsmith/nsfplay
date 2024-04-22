// nsfplaycore.cpp
//   Contains the public interface.

#include "core.h"

NSFCore* nsfplay_create(const char* ini_data)
{
	NSFCore* core = NSFCore::create();
	core->set_ini(ini_data);
	core->finalize();
	return core;
}

NSFCore* nsfplay_create_init(const NSFSetInit* init)
{
	NSFCore* core = NSFCore::create();
	core->set_init(init);
	core->finalize();
	return core;
}

void nsfplay_destroy(NSFCore* core)
{
	NSFCore::destroy(core);
}

const char* nsfplay_last_error(const NSFCore* core)
{
	return core->last_error();
}

void nsfplay_set_error_log(void (*error_callback_)(const char* msg))
{
	nsfp::error_callback = error_callback_;
}

void nsfplay_set_debug_print(void (*debug_print_callback_)(const char* msg))
{
	nsfp::debug_print_callback = debug_print_callback_;
}

void nsfplay_set_fatal(void (*fatal_callback_)(const char* msg))
{
	nsfp::fatal_callback = fatal_callback_;
}

void nsfplay_set_default(NSFCore* core)
{
	core->set_default();
	core->set_apply();
}

bool nsfplay_set_ini(NSFCore* core, const char* ini_data)
{
	bool result = core->set_ini(ini_data);
	core->set_apply();
	return result;
}

bool nsfplay_set_init(NSFCore* core, const NSFSetInit* init)
{
	bool result = core->set_init(init);
	core->set_apply();
	return result;
}

const char* nsfplay_ini_line(const NSFCore* core, int32_t setenum)
{
	return core->ini_line(setenum);
}

bool nsfplay_set_int(NSFCore* core, int32_t setenum, int32_t value)
{
	(void)core;
	(void)setenum;
	(void)value;
	return false;
}

bool nsfplay_set_str(NSFCore* core, int32_t setenum, const char* value)
{
	(void)core;
	(void)setenum;
	(void)value;
	return false;
}

int32_t nsfplay_get_int(const NSFCore* core, int32_t setenum)
{
	(void)core;
	(void)setenum;
	return 0;
}

const char* nsfplay_get_str(const NSFCore* core, int32_t setenum)
{
	(void)core;
	(void)setenum;
	return NULL;
}

bool nsfplay_set_key_int(NSFCore* core, const char* key, int32_t value)
{
	(void)core;
	(void)key;
	(void)value;
	return false;
}

bool nsfplay_set_key_str(NSFCore* core, const char* key, const char* value)
{
	(void)core;
	(void)key;
	(void)value;
	return false;
}

int32_t nsfplay_get_key_int(const NSFCore* core, const char* key)
{
	(void)core;
	(void)key;
	return 0;
}

const char* nsfplay_get_key_str(const NSFCore* core, const char* key)
{
	(void)core;
	(void)key;
	return NULL;
}

NSFSetInfo nsfplay_set_info(int32_t setenum)
{
	(void)setenum;
	return {};
}

NSFSetGroupInfo nsfplay_set_group_info(int32_t group)
{
	(void)group;
	return {};
}

int32_t nsfplay_set_enum(const char* key)
{
	return NSFCore::set_enum(key);
}

int32_t nsfplay_group_enum(const char* key)
{
	return NSFCore::group_enum(key);
}

bool nsfplay_load(NSFCore* core, const void* nsf_data, uint32_t nsf_size)
{
	(void)core;
	(void)nsf_data;
	(void)nsf_size;
	return false;
}

bool nsfplay_assume(NSFCore* core, const void* nsf_data, uint32_t nsf_size)
{
	(void)core;
	(void)nsf_data;
	(void)nsf_size;
	return false;
}

uint32_t nsfplay_song_count(const NSFCore* core)
{
	(void)core;
	return 0;
}

bool nsfplay_song(NSFCore* core, uint8_t song)
{
	(void)core;
	(void)song;
	return false;
}

void nsfplay_song_play(NSFCore* core)
{
	(void)core;
}

void nsfplay_seek(NSFCore* core, uint64_t samples)
{
	(void)core;
	(void)samples;
}

uint64_t nsfplay_samples_played(const NSFCore* core)
{
	(void)core;
	return 0;
}

uint32_t nsfplay_render(NSFCore* core, uint32_t samples, int16_t* stereo_output)
{
	(void)core;
	(void)samples;
	(void)stereo_output;
	return 0;
}

uint8_t nsfplay_emu_peek(const NSFCore* core, uint16_t address)
{
	(void)core;
	(void)address;
	return 0;
}

uint8_t nsfplay_emu_read(NSFCore* core, uint16_t address)
{
	(void)core;
	(void)address;
	return 0;
}

void nsfplay_emu_poke(NSFCore* core, uint16_t address, uint8_t value)
{
	(void)core;
	(void)address;
	(void)value;
}

void nsfplay_emu_reg_set(NSFCore* core, char reg, uint16_t value)
{
	(void)core;
	(void)reg;
	(void)value;
}

uint16_t nsfplay_emu_reg_get(const NSFCore* core, char reg)
{
	(void)core;
	(void)reg;
	return 0;
}

void nsfplay_emu_init(NSFCore* core, uint8_t song)
{
	(void)core;
	(void)song;
}

void nsfplay_emu_run(NSFCore* core, uint32_t cycles)
{
	(void)core;
	(void)cycles;
}

uint32_t nsfplay_emu_run_frame(NSFCore* core)
{
	(void)core;
	return 0;
}

uint32_t nsfplay_emu_run_instruction(NSFCore* core)
{
	(void)core;
	return 0;
}

const char* nsfplay_emu_trace(const NSFCore* core)
{
	(void)core;
	return NULL;
}

uint32_t nsfplay_emu_samples_pending(const NSFCore* core)
{
	(void)core;
	return 0;
}

void nsfplay_emu_cancel_pending(NSFCore* core)
{
	(void)core;
}

uint64_t nsfplay_emu_cycles(const NSFCore* core)
{
	(void)core;
	return 0;
}

uint32_t nsfplay_emu_cycles_to_next_sample(const NSFCore* core)
{
	(void)core;
	return 0;
}

NSFPropInfo nsfplay_prop_info(const NSFCore* core, int32_t prop)
{
	(void)core;
	(void)prop;
	return {};
}

NSFPropInfo nsfplay_prop_song_info(const NSFCore* core, int32_t song, int32_t prop)
{
	(void)core;
	(void)song;
	(void)prop;
	return {};
}

int32_t nsfplay_prop_int(const NSFCore* core, int32_t prop)
{
	(void)core;
	(void)prop;
	return 0;
}

int64_t nsfplay_prop_long(const NSFCore* core, int32_t prop)
{
	(void)core;
	(void)prop;
	return 0;
}

const char* nsfplay_prop_str(const NSFCore* core, int32_t prop)
{
	(void)core;
	(void)prop;
	return NULL;
}

int32_t nsfplay_prop_lines(const NSFCore* core, int32_t prop)
{
	(void)core;
	(void)prop;
	return 0;
}

const char* nsfplay_prop_line(const NSFCore* core)
{
	(void)core;
	return NULL;
}

const void* nsfplay_prop_blob(const NSFCore* core, uint32_t* blob_size)
{
	(void)core;
	(void)blob_size;
	return NULL;
}

int32_t nsfplay_prop_song_type(const NSFCore* core, int32_t song, int32_t prop)
{
	(void)core;
	(void)song;
	(void)prop;
	return 0;
}

int32_t nsfplay_prop_song_int(const NSFCore* core, int32_t song, int32_t prop)
{
	(void)core;
	(void)song;
	(void)prop;
	return 0;
}

int64_t nsfplay_prop_song_long(const NSFCore* core, int32_t song, int32_t prop)
{
	(void)core;
	(void)song;
	(void)prop;
	return 0;
}

const char* nsfplay_prop_song_str(const NSFCore* core, int32_t song, int32_t prop)
{
	(void)core;
	(void)song;
	(void)prop;
	return NULL;
}

int32_t nsfplay_prop_song_lines(const NSFCore* core, int32_t song, int32_t prop)
{
	(void)core;
	(void)song;
	(void)prop;
	return 0;
}

const void* nsfplay_prop_song_blob(const NSFCore* core, int32_t song, uint32_t* blob_size)
{
	(void)core;
	(void)song;
	(void)blob_size;
	return NULL;
}

const void* nsfplay_chunk(const NSFCore* core, const char* fourcc, uint32_t* chunk_size)
{
	(void)core;
	(void)fourcc;
	(void)chunk_size;
	return NULL;
}

NSFChannelUnit nsfplay_channel_unit(const NSFCore* core, int32_t unit)
{
	(void)core;
	(void)unit;
	return {};
}

NSFChannelInfo nsfplay_channel_info(int32_t global_channel)
{
	(void)global_channel;
	return {};
}

int32_t nsfplay_channel_count(const NSFCore* core)
{
	(void)core;
	return 0;
}

NSFChannelState nsfplay_channel_state(const NSFCore* core, int32_t active_channel)
{
	(void)core;
	(void)active_channel;
	return {};
}

uint32_t nsfplay_channel_state_ex(const NSFCore* core, int32_t active_channel, void* data, uint32_t data_size)
{
	(void)core;
	(void)active_channel;
	(void)data;
	(void)data_size;
	return 0;
}

uint64_t nsfplay_time_to_samples(const NSFCore* core, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds)
{
	(void)core;
	(void)hours;
	(void)minutes;
	(void)seconds;
	(void)milliseconds;
	return 0;
}

uint64_t nsfplay_time_to_cycles(const NSFCore* core, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds)
{
	(void)core;
	(void)hours;
	(void)minutes;
	(void)seconds;
	(void)milliseconds;
	return 0;
}

void nsfplay_samples_to_time(const NSFCore* core, uint64_t samples, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds)
{
	(void)core;
	(void)samples;
	(void)hours;
	(void)minutes;
	(void)seconds;
	(void)milliseconds;
}

void nsfplay_cycles_to_time(const NSFCore* core, uint64_t cycles, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds)
{
	(void)core;
	(void)cycles;
	(void)hours;
	(void)minutes;
	(void)seconds;
	(void)milliseconds;
}

const char* nsfplay_local_text(int32_t key)
{
	(void)key;
	return NULL;
}
