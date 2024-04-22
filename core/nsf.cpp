// nsf.cpp
//   Parsing of the loaded NSF file (or other file types).
//   Fetching of properties and song properties.

#include "core.h"
#include <cstring> // std::memset

bool NSFCore::nsf_parse()
{
	prop_lines = NULL;
	std::memset(memory_master,0,sizeof(memory_master));
	current_song = 0;
	// TODO reset all stored properties (e.g. expansions)
	// 
	// TODO parse the file
	// TODO nsf file, nsfe file, nsf2 file
	// TODO bin file (loaded at 8000, no expansions?)
	//
	// Don't store much information,
	// all property queries should be expected "slow" (human instant)
	// so it's fine to just reparse them through the prop interface whenever needed.
	// This way we don't store unnecessary or potentially divergent state.
	//
	return false;
}

const void* NSFCore::nsf_chunk(const char* fourcc, uint32* chunk_size) const
{
	(void)fourcc;
	(void)chunk_size;
	// TODO
	return NULL;
}

bool NSFCore::nsf_prop_exists(sint32 prop, sint32 song) const
{
	(void)prop;
	(void)song;
	// TODO
	return false;
}

sint32 NSFCore::nsf_prop_int(sint32 prop, sint32 song) const
{
	(void)prop;
	(void)song;
	// TODO
	return 0;
}

sint64 NSFCore::nsf_prop_long(sint32 prop, sint32 song) const
{
	(void)prop;
	(void)song;
	// TODO
	return 0;
}

const char* NSFCore::nsf_prop_str(sint32 prop, sint32 song) const
{
	(void)prop;
	(void)song;
	// TODO
	return NULL;
}

sint32 NSFCore::nsf_prop_lines(sint32 prop, sint32 song) const
{
	(void)prop;
	(void)song;
	// TODO
	prop_lines = NULL;
	return 0;
}

const char* NSFCore::nsf_prop_line() const
{
	// TODO
	// return prop_lines, advance to next line
	return NULL;
}

const void* NSFCore::nsf_prop_blob(uint32* blob_size, sint32 prop, sint32 song) const
{
	(void)blob_size;
	(void)prop;
	(void)song;
	// TODO
	return NULL;
}
