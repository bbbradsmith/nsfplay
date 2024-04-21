// core.cpp
//   High level central operation of core functions.
//   Implementation of NSFCore members.
//   Global utilities.

#include "core.h"
#include "enums_data.h"

#include <cstdio> // std::fprintf, std::vsnprintf, stdout, stderr
#include <cstdlib> // std::malloc, std::free, std::exit
#include <cstring> // std:;memset, std::memcpy
#include <cstdarg> // va_list, va_start

namespace nsfp {

void (*debug_print_callback)(const char* msg) = NULL;
void (*fatal_callback)(const char* msg) = NULL;

void* alloc(size_t size)
{
	void* a = std::malloc(size);
	if (a == NULL) nsfp::fatal("Out of memory.");
	NSFP_DEBUG("alloc(%z)=%p",size,a);
	return a;
}

void free(void* ptr)
{
	NSFP_DEBUG("free(%p)",ptr);
	std::free(ptr);
}

void debug(const char* msg)
{
	if (debug_print_callback) debug_print_callback(msg);
	else std::fprintf(stdout,"%s\n",msg);
}

void debug_printf(const char* fmt,...)
{
#ifdef DEBUG
	static char msg[1024];
	va_list args;
	va_start(args,fmt);
	std::vsnprintf(msg,sizeof(msg),msg,args);
#else
	(void)fmt;
#endif
}

void fatal(const char* msg)
{
	if (fatal_callback) fatal_callback(msg);
	std::fprintf(stderr,"%s\n",msg);
	std::exit(-1);
}

} // namespace nsfp

NSFCore* NSFCore::create()
{
	NSFCore* core = (NSFCore*)nsfp::alloc(sizeof(NSFCore));
	NSFP_DEBUG("create()=%p",core);
	std::memset(core,0,sizeof(NSFCore));
	core->set_default();
	return core;
}

void NSFCore::destroy(NSFCore* core)
{
	NSFP_DEBUG("destroy(%p)",core);
	core->release();
	nsfp::free(core);
}

void NSFCore::finalize()
{
	// TODO make any allocations needed based on the settings
}

void NSFCore::release()
{
	// TODO release any allocations
}

void NSFCore::set_default()
{
	// TODO
}

bool NSFCore::set_ini(const char* ini)
{
	(void)ini;
	// TODO
	return false;
}

bool NSFCore::set_init(const NSFSetInit* init)
{
	(void)init;
	// TODO
	return false;
}
