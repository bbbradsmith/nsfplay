// nsfplaycore.cpp
//   Contains the public interface.

#include "core.h"
#include <cstring> // std::strlen

#if NSF_MUTEX
	#include <mutex>
	static std::mutex nsfplay_mutex;
	#define NSF_MUTEX_GUARD() const std::lock_guard<std::mutex> nsfplay_guard(nsfplay_mutex)
#else
	#define NSF_MUTEX_GUARD() {}
#endif

NSFCore* nsfplay_create(const char* ini_data)
{
	NSF_MUTEX_GUARD();
	NSFCore* core = NSFCore::create();
	if (core) core->set_ini(ini_data);
	return core;
}

NSFCore* nsfplay_create_init(const NSFSetInit* init, bool assume_str)
{
	NSF_MUTEX_GUARD();
	NSFCore* core = NSFCore::create();
	if (core) core->set_init(init,assume_str);
	return core;
}

void nsfplay_destroy(NSFCore* core)
{
	NSF_MUTEX_GUARD();
	NSFCore::destroy(core); // destroy(NULL) is safe
}

void nsfplay_set_alloc(void* (*custom_alloc_)(size_t size),void* (*custom_free_)(void* ptr))
{
	NSF_MUTEX_GUARD();
	nsf::custom_alloc = custom_alloc_;
	nsf::custom_free = custom_free_;
}

void nsfplay_set_error_log(void (*error_callback_)(const NSFCore* core, int32_t code, const char* msg))
{
	NSF_MUTEX_GUARD();
	nsf::error_callback = error_callback_;
}

void nsfplay_set_debug_print(void (*debug_print_callback_)(const char* msg))
{
	NSF_MUTEX_GUARD();
	nsf::debug_print_callback = debug_print_callback_;
}

void nsfplay_set_fatal(void (*fatal_callback_)(const char* msg))
{
	NSF_MUTEX_GUARD();
	nsf::fatal_callback = fatal_callback_;
}

const char* nsfplay_last_error(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (core) return core->last_error();
	return NULL;
}

int32_t nsfplay_last_error_code(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (core) return core->last_error_code();
	return -1;
}

void nsfplay_set_default(NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (core)
	{
		core->set_default();
		core->set_apply();
	}
}

bool nsfplay_set_ini(NSFCore* core, const char* ini_data)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	bool result = core->set_ini(ini_data);
	core->set_apply();
	return result;
}

bool nsfplay_set_init(NSFCore* core, const NSFSetInit* init, bool assume_str)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	bool result = core->set_init(init,assume_str);
	core->set_apply();
	return result;
}

bool nsfplay_set_ini_line(NSFCore* core, const char* ini_line)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	bool result = core->parse_ini_line(ini_line,(int)(std::strlen(ini_line)),-1);
	core->set_apply();
	return result;
}

const char* nsfplay_ini_line(const NSFCore* core, int32_t setenum)
{
	NSF_MUTEX_GUARD();
	if (!core) return "";
	return core->ini_line(setenum);
}

bool nsfplay_ini_write(const NSFCore* core, FILE* f)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	return core->ini_write(f);
}

bool nsfplay_set_int(NSFCore* core, int32_t setenum, int32_t value)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	bool result = core->set_int(setenum,value);
	core->set_apply();
	return result;
}

bool nsfplay_set_str(NSFCore* core, int32_t setenum, const char* value, bool assume)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	bool result = core->set_str(setenum,value,assume);
	core->set_apply();
	return result;
}

int32_t nsfplay_get_int(const NSFCore* core, int32_t setenum)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	return core->get_int(setenum);
}

const char* nsfplay_get_str(const NSFCore* core, int32_t setenum)
{
	NSF_MUTEX_GUARD();
	if (!core) return "";
	return core->get_str(setenum);
}

bool nsfplay_set_key_int(NSFCore* core, const char* key, int32_t value)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	sint32 setenum = core->set_enum(key);
	bool result = core->set_int(setenum,value);
	core->set_apply();
	return result;
}

bool nsfplay_set_key_str(NSFCore* core, const char* key, const char* value)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	sint32 setenum = core->set_enum(key);
	bool result = core->set_str(setenum,value);
	core->set_apply();
	return result;
}

int32_t nsfplay_get_key_int(const NSFCore* core, const char* key)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	sint32 setenum = core->set_enum(key);
	return core->get_int(setenum);
}

const char* nsfplay_get_key_str(const NSFCore* core, const char* key)
{
	NSF_MUTEX_GUARD();
	if (!core) return "";
	sint32 setenum = core->set_enum(key);
	return core->get_str(setenum);
}

NSFSetInfo nsfplay_set_info(const NSFCore* core, int32_t setenum)
{
	NSF_MUTEX_GUARD();
	if (!core)
	{
		NSFSetInfo info = {0};
		info.group = -1;
		info.key = info.name = info.desc = "";
		return info;
	}
	return core->set_info(setenum);
}

NSFGroupInfo nsfplay_group_info(const NSFCore* core, int32_t group)
{
	NSF_MUTEX_GUARD();
	if (!core)
	{
		NSFGroupInfo info = {0};
		info.key = info.name = info.desc = "";
		return info;
	}
	return core->group_info(group);
}

int32_t nsfplay_set_enum(const char* key)
{
	// no mutex needed, just a static lookup
	return NSFCore::set_enum(key);
}

int32_t nsfplay_group_enum(const char* key)
{
	// no mutex needed, just a static lookup
	return NSFCore::group_enum(key);
}

bool nsfplay_load(NSFCore* core, const void* nsf_data, uint32_t nsf_size, bool assume)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	return core->load(reinterpret_cast<const uint8*>(nsf_data),nsf_size,assume,false);
}

bool nsfplay_load_bin(NSFCore* core, const void* bin_data, uint32_t bin_size, bool assume)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	return core->load(reinterpret_cast<const uint8*>(bin_data),bin_size,assume,true);
}

int32_t nsfplay_song_count(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	return core->prop_int(NSF_PROP_ACTIVE_SONG_COUNT);
}

int32_t nsfplay_song_current(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	return core->prop_int(NSF_PROP_ACTIVE_SONG);
}

bool nsfplay_song(NSFCore* core, uint8_t song)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	NSF_UNUSED(song);
	// TODO
	return false;
}

void nsfplay_song_play(NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return;
	NSF_UNUSED(core);
	// TODO
}

void nsfplay_seek(NSFCore* core, uint64_t samples)
{
	NSF_MUTEX_GUARD();
	if (!core) return;
	NSF_UNUSED(core);
	NSF_UNUSED(samples);
	// TODO
}

uint64_t nsfplay_samples_played(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	NSF_UNUSED(core);
	// TODO
	return 0;
}

uint32_t nsfplay_render32(NSFCore* core, uint32_t samples, int32_t* stereo_output)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	NSF_UNUSED(core);
	NSF_UNUSED(samples);
	NSF_UNUSED(stereo_output);
	// TODO
	return 0;
}

uint32_t nsfplay_render(NSFCore* core, uint32_t samples, int16_t* stereo_output)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	NSF_UNUSED(core);
	NSF_UNUSED(samples);
	NSF_UNUSED(stereo_output);
	// TODO
	return 0;
}

void nsfplay_ready(NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return;
	NSF_UNUSED(core);
	// TODO
}

uint8_t nsfplay_emu_peek(const NSFCore* core, uint16_t address)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	NSF_UNUSED(core);
	NSF_UNUSED(address);
	// TODO
	return 0;
}

uint8_t nsfplay_emu_read(NSFCore* core, uint16_t address)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	NSF_UNUSED(core);
	NSF_UNUSED(address);
	// TODO
	return 0;
}

void nsfplay_emu_poke(NSFCore* core, uint16_t address, uint8_t value)
{
	NSF_MUTEX_GUARD();
	if (!core) return;
	NSF_UNUSED(core);
	NSF_UNUSED(address);
	NSF_UNUSED(value);
	// TODO
}

void nsfplay_emu_reg_set(NSFCore* core, char reg, uint16_t value)
{
	NSF_MUTEX_GUARD();
	if (!core) return;
	NSF_UNUSED(core);
	NSF_UNUSED(reg);
	NSF_UNUSED(value);
	// TODO
}

uint16_t nsfplay_emu_reg_get(const NSFCore* core, char reg)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	NSF_UNUSED(core);
	NSF_UNUSED(reg);
	// TODO
	return 0;
}

void nsfplay_emu_init(NSFCore* core, uint8_t song)
{
	NSF_MUTEX_GUARD();
	if (!core) return;
	NSF_UNUSED(core);
	NSF_UNUSED(song);
	// TODO
}

void nsfplay_emu_run(NSFCore* core, uint32_t cycles)
{
	NSF_MUTEX_GUARD();
	if (!core) return;
	NSF_UNUSED(core);
	NSF_UNUSED(cycles);
	// TODO
}

uint32_t nsfplay_emu_run_frame(NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	NSF_UNUSED(core);
	// TODO
	return 0;
}

uint32_t nsfplay_emu_run_instruction(NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	NSF_UNUSED(core);
	// TODO
	return 0;
}

const char* nsfplay_emu_trace(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return "";
	NSF_UNUSED(core);
	// TODO
	return NULL;
}

void nsfplay_emu_gamepad(NSFCore* core, int32_t pad, uint32_t report)
{
	NSF_MUTEX_GUARD();
	if (!core) return;
	NSF_UNUSED(core);
	NSF_UNUSED(pad);
	NSF_UNUSED(report);
	// TODO
	return;
}

uint32_t nsfplay_emu_samples_pending(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	return core->prop_int(NSF_PROP_EMU_PENDING);
}

void nsfplay_emu_cancel_pending(NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return;
	NSF_UNUSED(core);
	// TODO
}

uint64_t nsfplay_emu_cycles(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	return core->prop_long(NSF_PROP_EMU_CYCLES);
}

uint32_t nsfplay_emu_cycles_to_next_sample(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	return core->prop_int(NSF_PROP_EMU_NEXT_CYCLES);
}

NSFOpcode nsfplay_emu_opcode(uint8 op)
{
	// no mutex needed, just a static lookup
	NSF_UNUSED(op);
	// TODO
	return {0};
}

uint32_t nsfplay_savestate_size(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	NSF_UNUSED(core);
	// TODO
	return 0;
}

void nsfplay_savestate_save(const NSFCore* core, uint8_t* state)
{
	NSF_MUTEX_GUARD();
	NSF_UNUSED(core);
	NSF_UNUSED(state);
	// TODO
}

bool nsfplay_savestate_restore(NSFCore* core, const uint8_t* state)
{
	NSF_MUTEX_GUARD();
	NSF_UNUSED(core);
	NSF_UNUSED(state);
	// TODO
	return false;
}

NSFPropInfo nsfplay_prop_info(const NSFCore* core, int32_t prop)
{
	NSF_MUTEX_GUARD();
	if (!core)
	{
		NSFPropInfo info = {0};
		info.type = NSF_PROP_TYPE_INVALID;
		info.key = info.name = "";
		return info;
	}
	return core->prop_info(prop);
}

bool nsfplay_prop_exists(const NSFCore* core, int32_t prop, int32_t song)
{
	NSF_MUTEX_GUARD();
	if (!core) return false;
	return core->prop_exists(prop,song);
}

int32_t nsfplay_prop_int(const NSFCore* core, int32_t prop, int32_t song)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	return core->prop_int(prop,song);
}

int64_t nsfplay_prop_long(const NSFCore* core, int32_t prop, int32_t song)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	return core->prop_long(prop,song);
}

const char* nsfplay_prop_str(const NSFCore* core, int32_t prop, int32_t song)
{
	NSF_MUTEX_GUARD();
	if (!core) return "";
	return core->prop_str(prop,song);
}

int32_t nsfplay_prop_lines(const NSFCore* core, int32_t prop, int32_t song)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	return core->prop_lines(prop,song);
}

const char* nsfplay_prop_line(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return "";
	return core->prop_line();
}

const void* nsfplay_prop_blob(const NSFCore* core, uint32_t* blob_size, int32_t prop, int32_t song)
{
	NSF_MUTEX_GUARD();
	if (!core)
	{
		if(blob_size) *blob_size = 0;
		return NULL;
	}
	return core->prop_blob(blob_size,prop,song);
}

NSFPropMulti nsfplay_prop_multi(const NSFCore* core, int32_t prop, int32_t song)
{
	NSF_MUTEX_GUARD();
	NSFPropMulti pm = {{0},0};
	if (core)
	{
		pm.info = core->prop_info(prop);
		pm.exists = core->prop_exists(prop,song);
		pm.ival = core->prop_int(prop,song);
		pm.lval = core->prop_long(prop,song);
		pm.str = core->prop_str(prop,song);
		pm.lines = core->prop_lines(prop,song);
		pm.blob = static_cast<const void*>(core->prop_blob(&pm.blob_size,prop,song));
	}
	return pm;
}

const void* nsfplay_chunk(const NSFCore* core, const char* fourcc, uint32_t* chunk_size)
{
	NSF_MUTEX_GUARD();
	if (!core)
	{
		if (chunk_size) *chunk_size = 0;
		return NULL;
	}
	return core->nsfe_chunk(fourcc,chunk_size);
}

NSFUnitInfo nsfplay_unit_info(const NSFCore* core, int32_t unit)
{
	NSF_MUTEX_GUARD();
	if (!core)
	{
		NSFUnitInfo info = {0};
		info.key = info.name = info.desc = "";
		return info;
	}
	return core->unit_info(unit);
}

NSFChannelInfo nsfplay_channel_info(const NSFCore* core, int32_t global_channel)
{
	NSF_MUTEX_GUARD();
	if (!core)
	{
		NSFChannelInfo info = {0};
		info.unit = -1;
		info.key = info.short_name = info.name = "";
		return info;
	}
	return core->channel_info(global_channel);
}

int32_t nsfplay_channel_count(const NSFCore* core)
{
	NSF_MUTEX_GUARD();
	if (!core) return 0;
	NSF_UNUSED(core);
	// TODO
	return 0;
}

NSFChannelState nsfplay_channel_state(const NSFCore* core, int32_t active_channel)
{
	NSF_MUTEX_GUARD();
	NSF_UNUSED(core);
	NSF_UNUSED(active_channel);
	// TODO
	return {0};
}

uint32_t nsfplay_channel_state_ex(const NSFCore* core, int32_t active_channel, void* data, uint32_t data_size)
{
	NSF_MUTEX_GUARD();
	NSF_UNUSED(core);
	NSF_UNUSED(active_channel);
	NSF_UNUSED(data);
	NSF_UNUSED(data_size);
	// TODO
	return 0;
}

uint64_t nsfplay_time_to_samples(const NSFCore* core, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds)
{
	NSF_MUTEX_GUARD();
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
	NSF_MUTEX_GUARD();
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
	NSF_MUTEX_GUARD();
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
	NSF_MUTEX_GUARD();
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
	NSF_MUTEX_GUARD();
	return core->local_text(textenum);
}
