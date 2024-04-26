// nsfplaycore.cpp
//   Contains the public interface.

#include "core.h"
#include <cstring> // std::strlen

NSFCore* nsfplay_create(const char* ini_data)
{
	NSFCore* core = NSFCore::create();
	core->set_ini(ini_data);
	return core;
}

NSFCore* nsfplay_create_init(const NSFSetInit* init)
{
	NSFCore* core = NSFCore::create();
	core->set_init(init);
	return core;
}

void nsfplay_destroy(NSFCore* core)
{
	NSFCore::destroy(core);
}

void nsfplay_set_error_log(void (*error_callback_)(const char* msg))
{
	nsf::error_callback = error_callback_;
}

void nsfplay_set_debug_print(void (*debug_print_callback_)(const char* msg))
{
	nsf::debug_print_callback = debug_print_callback_;
}

void nsfplay_set_fatal(void (*fatal_callback_)(const char* msg))
{
	nsf::fatal_callback = fatal_callback_;
}

const char* nsfplay_last_error(const NSFCore* core)
{
	return core->last_error();
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

bool nsfplay_set_ini_line(NSFCore* core, const char* ini_line)
{
	bool result = core->parse_ini_line(ini_line,(int)(std::strlen(ini_line)),-1);
	core->set_apply();
	return result;
}

const char* nsfplay_ini_line(const NSFCore* core, int32_t setenum)
{
	return core->ini_line(setenum);
}

void nsfplay_ini_write(const NSFCore* core, FILE* f)
{
	return core->ini_write(f);
}

bool nsfplay_set_int(NSFCore* core, int32_t setenum, int32_t value)
{
	bool result = core->set_int(setenum,value);
	core->set_apply();
	return result;
}

bool nsfplay_set_str(NSFCore* core, int32_t setenum, const char* value)
{
	bool result = core->set_str(setenum,value);
	core->set_apply();
	return result;
}

int32_t nsfplay_get_int(const NSFCore* core, int32_t setenum)
{
	return core->get_int(setenum);
}

const char* nsfplay_get_str(const NSFCore* core, int32_t setenum)
{
	return core->get_str(setenum);
}

bool nsfplay_set_key_int(NSFCore* core, const char* key, int32_t value)
{
	sint32 setenum = core->set_enum(key);
	bool result = core->set_int(setenum,value);
	core->set_apply();
	return result;
}

bool nsfplay_set_key_str(NSFCore* core, const char* key, const char* value)
{
	sint32 setenum = core->set_enum(key);
	bool result = core->set_str(setenum,value);
	core->set_apply();
	return result;
}

int32_t nsfplay_get_key_int(const NSFCore* core, const char* key)
{
	sint32 setenum = core->set_enum(key);
	return core->get_int(setenum);
}

const char* nsfplay_get_key_str(const NSFCore* core, const char* key)
{
	sint32 setenum = core->set_enum(key);
	return core->get_str(setenum);
}

NSFSetInfo nsfplay_set_info(const NSFCore* core, int32_t setenum)
{
	return core->set_info(setenum);
}

NSFGroupInfo nsfplay_set_group_info(const NSFCore* core, int32_t group)
{
	return core->group_info(group);
}

int32_t nsfplay_set_enum(const char* key)
{
	return NSFCore::set_enum(key);
}

int32_t nsfplay_group_enum(const char* key)
{
	return NSFCore::group_enum(key);
}

bool nsfplay_load(NSFCore* core, const void* nsf_data, uint32_t nsf_size, bool assume)
{
	return core->load((uint8*)(nsf_data),nsf_size,assume,false);
}

bool nsfplay_load_bin(NSFCore* core, const void* bin_data, uint32_t bin_size, bool assume)
{
	return core->load((uint8*)(bin_data),bin_size,assume,true);
}

uint32_t nsfplay_song_count(const NSFCore* core)
{
	return core->prop_int(NSF_PROP_ACTIVE_SONG_COUNT);
}

uint32_t nsfplay_song_current(const NSFCore* core)
{
	return core->prop_int(NSF_PROP_ACTIVE_SONG);
}

bool nsfplay_song(NSFCore* core, uint8_t song)
{
	NSF_UNUSED(core);
	NSF_UNUSED(song);
	// TODO
	return false;
}

void nsfplay_song_play(NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
}

void nsfplay_seek(NSFCore* core, uint64_t samples)
{
	NSF_UNUSED(core);
	NSF_UNUSED(samples);
	// TODO
}

uint64_t nsfplay_samples_played(const NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
	return 0;
}

uint32_t nsfplay_render(NSFCore* core, uint32_t samples, int16_t* stereo_output)
{
	NSF_UNUSED(core);
	NSF_UNUSED(samples);
	NSF_UNUSED(stereo_output);
	// TODO
	return 0;
}

uint32_t nsfplay_render32(NSFCore* core, uint32_t samples, int32_t* stereo_output)
{
	NSF_UNUSED(core);
	NSF_UNUSED(samples);
	NSF_UNUSED(stereo_output);
	// TODO
	return 0;
}

uint8_t nsfplay_emu_peek(const NSFCore* core, uint16_t address)
{
	NSF_UNUSED(core);
	NSF_UNUSED(address);
	// TODO
	return 0;
}

uint8_t nsfplay_emu_read(NSFCore* core, uint16_t address)
{
	NSF_UNUSED(core);
	NSF_UNUSED(address);
	// TODO
	return 0;
}

void nsfplay_emu_poke(NSFCore* core, uint16_t address, uint8_t value)
{
	NSF_UNUSED(core);
	NSF_UNUSED(address);
	NSF_UNUSED(value);
	// TODO
}

void nsfplay_emu_reg_set(NSFCore* core, char reg, uint16_t value)
{
	NSF_UNUSED(core);
	NSF_UNUSED(reg);
	NSF_UNUSED(value);
	// TODO
}

uint16_t nsfplay_emu_reg_get(const NSFCore* core, char reg)
{
	NSF_UNUSED(core);
	NSF_UNUSED(reg);
	// TODO
	return 0;
}

void nsfplay_emu_init(NSFCore* core, uint8_t song)
{
	NSF_UNUSED(core);
	NSF_UNUSED(song);
	// TODO
}

void nsfplay_emu_run(NSFCore* core, uint32_t cycles)
{
	NSF_UNUSED(core);
	NSF_UNUSED(cycles);
	// TODO
}

uint32_t nsfplay_emu_run_frame(NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
	return 0;
}

uint32_t nsfplay_emu_run_instruction(NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
	return 0;
}

const char* nsfplay_emu_trace(const NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
	return NULL;
}

void nsfplay_emu_gamepad(NSFCore* core, int32_t pad, uint32_t report)
{
	NSF_UNUSED(core);
	NSF_UNUSED(pad);
	NSF_UNUSED(report);
	// TODO
	return;
}

uint32_t nsfplay_emu_samples_pending(const NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
	return 0;
}

void nsfplay_emu_cancel_pending(NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
}

uint64_t nsfplay_emu_cycles(const NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
	return 0;
}

uint32_t nsfplay_emu_cycles_to_next_sample(const NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
	return 0;
}

NSFOpcode nsfplay_emu_opcode(uint8 op)
{
	NSF_UNUSED(op);
	// TODO
	return {0};
}

NSFPropInfo nsfplay_prop_info(const NSFCore* core, int32_t prop)
{
	return core->prop_info(prop);
}

bool nsfplay_prop_exists(const NSFCore* core, int32_t prop, int32_t song)
{
	return core->prop_exists(prop,song);
}

int32_t nsfplay_prop_int(const NSFCore* core, int32_t prop, int32_t song)
{
	return core->prop_int(prop,song);
}

int64_t nsfplay_prop_long(const NSFCore* core, int32_t prop, int32_t song)
{
	return core->prop_long(prop,song);
}

const char* nsfplay_prop_str(const NSFCore* core, int32_t prop, int32_t song)
{
	return core->prop_str(prop,song);
}

int32_t nsfplay_prop_lines(const NSFCore* core, int32_t prop, int32_t song)
{
	return core->prop_lines(prop,song);
}

const char* nsfplay_prop_line(const NSFCore* core)
{
	return core->prop_line();
}

const void* nsfplay_prop_blob(const NSFCore* core, uint32_t* blob_size, int32_t prop, int32_t song)
{
	return core->prop_blob(blob_size,prop,song);
}

NSFPropMulti nsfplay_prop_multi(const NSFCore* core, int32_t prop, int32_t song)
{
	NSFPropMulti pm = {{0},0};
	pm.info = core->prop_info(prop);
	pm.exists = core->prop_exists(prop,song);
	pm.ival = core->prop_int(prop,song);
	pm.lval = core->prop_long(prop,song);
	pm.str = core->prop_str(prop,song);
	pm.lines = core->prop_lines(prop,song);
	pm.blob = static_cast<const void*>(core->prop_blob(&pm.blob_size,prop,song));
	return pm;
}

const void* nsfplay_chunk(const NSFCore* core, const char* fourcc, uint32_t* chunk_size)
{
	return core->nsfe_chunk(fourcc,chunk_size);
}

NSFChannelUnit nsfplay_channel_unit(const NSFCore* core, int32_t unit)
{
	NSF_UNUSED(core);
	NSF_UNUSED(unit);
	// TODO
	return {};
}

NSFChannelInfo nsfplay_channel_info(int32_t global_channel)
{
	NSF_UNUSED(global_channel);
	// TODO
	return {};
}

int32_t nsfplay_channel_count(const NSFCore* core)
{
	NSF_UNUSED(core);
	// TODO
	return 0;
}

NSFChannelState nsfplay_channel_state(const NSFCore* core, int32_t active_channel)
{
	NSF_UNUSED(core);
	NSF_UNUSED(active_channel);
	// TODO
	return {};
}

uint32_t nsfplay_channel_state_ex(const NSFCore* core, int32_t active_channel, void* data, uint32_t data_size)
{
	NSF_UNUSED(core);
	NSF_UNUSED(active_channel);
	NSF_UNUSED(data);
	NSF_UNUSED(data_size);
	// TODO
	return 0;
}

uint64_t nsfplay_time_to_samples(const NSFCore* core, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds)
{
	NSF_UNUSED(core);
	NSF_UNUSED(hours);
	NSF_UNUSED(minutes);
	NSF_UNUSED(seconds);
	NSF_UNUSED(milliseconds);
	// TODO
	return 0;
}

uint64_t nsfplay_time_to_cycles(const NSFCore* core, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds)
{
	NSF_UNUSED(core);
	NSF_UNUSED(hours);
	NSF_UNUSED(minutes);
	NSF_UNUSED(seconds);
	NSF_UNUSED(milliseconds);
	// TODO
	return 0;
}

void nsfplay_samples_to_time(const NSFCore* core, uint64_t samples, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds)
{
	NSF_UNUSED(core);
	NSF_UNUSED(samples);
	NSF_UNUSED(hours);
	NSF_UNUSED(minutes);
	NSF_UNUSED(seconds);
	NSF_UNUSED(milliseconds);
	// TODO
}

void nsfplay_cycles_to_time(const NSFCore* core, uint64_t cycles, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds)
{
	NSF_UNUSED(core);
	NSF_UNUSED(cycles);
	NSF_UNUSED(hours);
	NSF_UNUSED(minutes);
	NSF_UNUSED(seconds);
	NSF_UNUSED(milliseconds);
	// TODO
}

const char* nsfplay_local_text(const NSFCore* core, int32_t textenum)
{
	if (!core) return NSFCore::local_text(textenum,0); // default locale
	return core->local_text(textenum);
}
