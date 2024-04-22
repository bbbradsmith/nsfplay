// core.cpp
//   High level central operation of core functions.
//   Implementation of most NSFCore members.
//   Global utilities.

#include "core.h"
#include "enums_data.h"

#include <cstdio> // std::fprintf, std::snprintf, std::vsnprintf, stdout, stderr
#include <cstdlib> // std::malloc, std::free, std::exit, std::strtol
#include <cstring> // std:;memset, std::memcpy, std::strlen
#include <cstdarg> // va_list, va_start
#include <cerrno> // errno

#if DEBUG_ALLOC
#include <map> // std::map
std::map<void*,size_t> debug_alloc;
size_t debug_alloc_total = 0;
#endif

// key_compare
// - key_test can be any case, len can truncate it (len<0 will use the whole string)
// - key_reference is uppercase only
// - returns true if equal (ignoring case)
inline static bool key_match(const char* key_test, int len, const char* key_reference)
{
	if (len < 0) len = 256; // greater than maximum possible key_reference length
	while ((len==0 || *key_test != 0) && *key_reference != 0)
	{
		char c = *key_test;
		if (c >= 'a' && c <= 'z') c = (c - 'a') + 'A';
		if (c != *key_reference) return false;
		++key_test;
		++key_reference;
		--len;
	}
	return (*key_reference == 0);
}

namespace nsfp {

void (*error_callback)(const char* msg) = NULL;
void (*debug_print_callback)(const char* msg) = NULL;
void (*fatal_callback)(const char* msg) = NULL;

void* alloc(size_t size)
{
	void* a = std::malloc(size);
	if (a == NULL) nsfp::fatal("Out of memory.");
	#if DEBUG_ALLOC
		debug_alloc[a] = size;
		debug_alloc_total += size;
		NSFP_DEBUG("alloc(%7zu) total %7zu in %3zu",size,debug_alloc_total,debug_alloc.size());
	#else
		NSFP_DEBUG("alloc(%7zu)",size);
	#endif
	return a;
}

void free(void* ptr)
{
	std::free(ptr);
	#if DEBUG_ALLOC
		size_t size = debug_alloc[ptr];
		debug_alloc_total -= size;
		debug_alloc.erase(ptr);
		NSFP_DEBUG("free(-%7zu) total %7zu in %3zu",size,debug_alloc_total,debug_alloc.size());
	#else
		NSFP_DEBUG("free()");
	#endif
}

void debug(const char* msg)
{
	if (debug_print_callback) debug_print_callback(msg);
	else std::fprintf(stdout,"%s\n",msg);
}

void debug_printf(const char* fmt,...)
{
#ifdef DEBUG
	static char msg[2048];
	va_list args;
	va_start(args,fmt);
	std::vsnprintf(msg,sizeof(msg),fmt,args);
	msg[sizeof(msg)-1] = 0;
	debug(msg);
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
	NSFP_DEBUG("create()");
	std::memset(core,0,sizeof(NSFCore));
	core->set_default();
	return core;
}

void NSFCore::destroy(NSFCore* core)
{
	NSFP_DEBUG("destroy()");
	core->release();
	nsfp::free(core);
}

void NSFCore::finalize()
{
	ram8000 = memory_master;
	ram0000 = ram8000 + 0x8000;
	pad0    = ram0000 + 0x800;
	pad1    = pad0    + 0x1000;

	// TODO make any allocations needed based on the settings
	// resample kernel, etc.
	set_apply();
}

void NSFCore::release()
{
	// release any allocations held
	if (nsf_free)
		nsfp::free(const_cast<uint8*>(nsf));
	for (int si=0; si<NSFP_SETSTR_COUNT; ++si)
		if (setting_str_free[si])
			nsfp::free(const_cast<char*>(setting_str[si]));
}

const char* NSFCore::last_error() const
{
	const char* r = error_last;
	error_last = NULL;
	return r;
}

void NSFCore::set_error(sint32 textenum,...)
{
	const char* fmt = local_text(textenum);
	// skip formatting if not needed
	bool direct = true;
	for (const char* c = fmt; *c; ++c)
	{
		if (*c == '%')
		{
			direct = false;
			break;
		}
	}
	if (direct)
	{
		error_last = fmt;
		if (nsfp::error_callback) nsfp::error_callback(error_last);
		else { NSFP_DEBUG("ERROR: %s",error_last); } // send errors to debug if not logged
		return;
	}
	// format
	va_list args;
	va_start(args,textenum);
	std::vsnprintf(error_last_buffer,sizeof(error_last_buffer),fmt,args);
	error_last_buffer[sizeof(error_last_buffer)-1] = 0;
	error_last = error_last_buffer;
	if (nsfp::error_callback) nsfp::error_callback(error_last);
	else { NSFP_DEBUG("ERROR: %s",error_last); }
}

void NSFCore::set_error_raw(const char* fmt,...)
{
	va_list args;
	va_start(args,fmt);
	std::vsnprintf(error_last_buffer,sizeof(error_last_buffer),fmt,args);
	error_last_buffer[sizeof(error_last_buffer)-1] = 0;
	error_last = error_last_buffer;
	if (nsfp::error_callback) nsfp::error_callback(error_last);
	else { NSFP_DEBUG("ERROR: %s",error_last); }
}

void NSFCore::set_default()
{
	for (int i=0; i<NSFP_SET_COUNT; ++i)
	{
		setting[i] = NSFPD_SET[i].default_int;
		if (NSFPD_SET[i].default_str != NULL)
		{
			sint32 si = setting[i];
			if (setting_str_free[si])
				nsfp::free(const_cast<char*>(setting_str[si]));
			setting_str[si] = NSFPD_SET[i].default_str;
			setting_str_free[si] = false;
		}
	}
}

bool NSFCore::set_ini(const char* ini)
{
	if (ini == NULL) return true;
	// skip UTF-8 BOM if it exists
	if (ini[0] == 0xEF && ini[1] == 0xBB && ini[2] == 0xBF) ini += 3;
	// parse line by line
	int linenum = 1;
	bool result = true;
	while (*ini) // ini is the start of a line
	{
		int eol = 0;
		while (ini[eol] != 10 && ini[eol] != 13 && ini[eol] != 0)
			++eol;
		result &= parse_ini_line(ini,eol,linenum);
		if (ini[eol] == 0) break;
		if ((ini[eol] == 10 && ini[eol+1] == 13) || // LF CR (obscure)
		    (ini[eol] == 13 && ini[eol+1] == 10)) // CR LF (windows)
			++eol;
		linenum += 1;
		ini += (eol+1);
	}
	return result;
}

bool NSFCore::set_init(const NSFSetInit* init)
{
	if (init == NULL) return true;
	bool result = true;
	for (int i=0; init[i].setenum >= 0; ++i)
	{
		if (init[i].val_str != NULL)
			result &= set_str(init[i].setenum,init[i].val_str);
		else
			result &= set_int(init[i].setenum,init[i].val_int);
	}
	return result;
}

bool NSFCore::set_int(sint32 setenum, sint32 value)
{
	if (setenum < 0 || setenum > NSFP_SET_COUNT)
	{
		set_error(NSFP_ERROR_SET_INVALID);
		return false;
	}
	const NSFSetData& SD = NSFPD_SET[setenum];
	if (SD.default_str != NULL)
	{
		set_error(NSFP_ERROR_SET_TYPE);
		return false;
	}
	if (value < SD.min_int || value > SD.max_int)
	{
		set_error(NSFP_ERROR_SETINT_RANGE,value,SD.min_int,SD.max_int);
		return false;
	}
	setting[setenum] = value;
	return true;
}

bool NSFCore::set_str(sint32 setenum, const char* value, sint32 len)
{
	if (setenum < 0 || setenum > NSFP_SET_COUNT)
	{
		set_error(NSFP_ERROR_SET_INVALID);
		return false;
	}
	const NSFSetData& SD = NSFPD_SET[setenum];
	if (SD.default_str == NULL)
	{
		set_error(NSFP_ERROR_SET_TYPE);
		return false;
	}
	sint32 si = setting[setenum];

	// auto-calculate 
	if (len < 0) len = sint32(std::strlen(value));

	// don't reallocate if the setting is redundant
	bool match = true;
	for (int i=0; i<len; ++i)
	{
		if (setting_str[si][i] != value[i])
		{
			match = false;
			break;
		}
	}
	if (match && setting_str[si][len] == 0)
		return true;

	// allocate and copy
	if (setting_str_free[si])
		nsfp::free(const_cast<char*>(setting_str[si]));
	char* new_str = (char*)(nsfp::alloc(size_t(len)+1));
	std::memcpy(new_str,value,len);
	new_str[len] = 0;
	setting_str[si] = new_str;
	setting_str_free[si] = true;
	return true;
}

void NSFCore::set_apply()
{
	// TODO apply settings that have changed
}

sint32 NSFCore::get_int(sint32 setenum) const
{
	if (setenum < 0 || setenum > NSFP_SET_COUNT) return 0;
	if (NSFPD_SET[setenum].default_str != NULL) return 0;
	return setting[setenum];
}

const char* NSFCore::get_str(sint32 setenum) const
{
	static const char* INVALID = "<INVALID>";
	if (setenum < 0 || setenum > NSFP_SET_COUNT) return INVALID;
	if (NSFPD_SET[setenum].default_str == NULL) return INVALID;
	sint32 si = setting[setenum];
	return setting_str[si];
}

sint32 NSFCore::set_enum(const char* key, int len)
{
	for (sint32 i=0; i<NSFP_SET_COUNT; ++i)
	{
		if (key_match(key,len,NSFPD_SET[i].key))
			return i;
	}
	return -1;
}

sint32 NSFCore::group_enum(const char* key, int len)
{
	for (sint32 i=0; i<NSFP_GROUP_COUNT; ++i)
	{
		if (key_match(key,len,NSFPD_GROUP[i].key))
			return i;
	}
	return -1;
}

NSFSetInfo NSFCore::set_info(sint32 setenum) const
{
	NSFSetInfo info = {0}; info.group = -1;
	if (setenum < 0 || setenum > NSFP_SET_COUNT) return info;
	const NSFSetData& SD = NSFPD_SET[setenum];
	info.group = SD.group;
	info.key = SD.key;
	info.name = local_text(SD.text+0);
	info.desc = local_text(SD.text+1);
	info.is_string = (SD.default_str != NULL);
	info.default_int = SD.default_int;
	info.min_int = SD.min_int;
	info.max_int = SD.max_int;
	info.list = (SD.list >= 0) ? (local_text(NSFPD_LIST_TEXT[SD.list])) : NULL; 
	info.default_str = SD.default_str;
	return info;
}

NSFSetGroupInfo NSFCore::group_info(sint32 group) const
{
	NSFSetGroupInfo info = {0};
	if (group < 0 || group > NSFP_GROUP_COUNT) return info;
	const NSFSetGroupData& GD = NSFPD_GROUP[group];
	info.key = GD.key;
	info.name = local_text(GD.text+0);
	info.desc = local_text(GD.text+1);
	return info;
}

const char* NSFCore::ini_line(sint32 setenum) const
{
	if (setenum < 0 || setenum >= NSFP_SET_COUNT) return "";
	const NSFSetData& SD = NSFPD_SET[setenum];
	if (SD.default_str == NULL)
		std::snprintf(temp_text,sizeof(temp_text),"%s=%d",SD.key,setting[setenum]);
	else
		std::snprintf(temp_text,sizeof(temp_text),"%s=%s",SD.key,get_str(setenum));
	temp_text[sizeof(temp_text)-1] = 0;
	return temp_text;
}

void NSFCore::ini_write(FILE* f) const
{
	sint32 last_group = -1;
	std::fprintf(f,"# NSFPlay INI settings file\n");
	for (sint32 i=0; i<NSFP_SET_COUNT; ++i)
	{
		// print a group comment each time it changes
		sint32 group = NSFPD_SET[i].group;
		if (group != last_group)
		{
			last_group = group;
			std::fprintf(f,"# [%s]\n",NSFPD_GROUP[last_group].key);
		}
		std::fprintf(f,"%s\n",ini_line(i));
	}
	std::fprintf(f,"# end of settings\n");
}

bool NSFCore::parse_ini_line(const char* line, int len, int linenum)
{
	// trim leading whitespace
	while (line[0]==' ' || line[0] == '\t') { ++line; --len; }
	// truncate for comments
	for (int i=0; i<len; ++i) { if (line[i]=='#') { len = i; break; } }
	// trim trailing whitespace
	while (len > 0 && (line[len-1]==' ' || line[len-1]=='\t')) { --len; }
	// reject empty line
	if (len < 1) return true;
	// find equals
	int equals = -1;
	for (int i=0; i<len; ++i) { if (line[i]=='=') { equals=i; break; } }
	if (equals < 0)
	{
		set_error(NSFP_ERROR_INI_NO_EQUALS,linenum);
		return false;
	}
	// line starts key, find end of key
	int keylen = equals;
	while (keylen > 0 && (line[keylen-1]==' ' || line[keylen-1]=='\t')) { --keylen; }
	// get enum from key
	sint32 se = set_enum(line,keylen);
	if (se < 0)
	{
		set_error(NSFP_ERROR_INI_BAD_KEY,linenum);
		return false;
	}
	// find start of value
	int valpos = equals+1;
	while (valpos < len && (line[valpos]==' ' || line[valpos]=='\t')) { ++valpos; }
	line += valpos;	len -= valpos;
	// se is our setting
	// line, len are now the value
	const NSFSetData& SD = NSFPD_SET[se];
	if (SD.default_str != NULL)
	{
		if (!set_str(se,line,len) && error_last != NULL)
		{
			NSFP_DEBUG("Unexpected set_str error at INI line %d: %s",linenum,error_last);
		}
		return true;
	}
	const char* value_end = line + len;
	char* strtol_end = const_cast<char*>(value_end);
	errno=0;
	sint32 value = std::strtol(line,&strtol_end,10);
	if (errno || strtol_end != value_end)
	{
		set_error(NSFP_ERROR_INI_BAD_INT,linenum);
		return false;
	}
	if (value < SD.min_int || value > SD.max_int)
	{
		set_error(NSFP_ERROR_INI_BAD_RANGE,linenum,value,SD.min_int,SD.max_int);
		return false;
	}
	if (!set_int(se,value) && error_last != NULL)
	{
		NSFP_DEBUG("Unexpected set_int error at INI line %d: %s",linenum,error_last);
	}
	return true;
}

bool NSFCore::load(const uint8* data, uint32 size, bool assume)
{
	if (nsf_free)
		nsfp::free(const_cast<uint8*>(nsf));
	nsf = NULL;
	nsf_free = false;

	if (data != NULL)
	{
		if (assume)
			nsf = data;
		else
		{
			nsf = (uint8*)(nsfp::alloc(size));
			std::memcpy(const_cast<uint8*>(nsf),data,size);
			nsf_free = true;
		}
	}

	bool result = nsf_parse();
	// TODO play song (reset/rebuilds audio stacks) which may & with result
	return result;
}

NSFPropInfo NSFCore::prop_info(sint32 prop, bool song) const
{
	NSFPropInfo info = {0};
	info.type = NSFP_PROP_TYPE_INVALID;
	const NSFPropData* PD;
	if (!song)
	{
		if (prop < 0 || prop > NSFP_PROP_COUNT) return info;
		PD = &NSFPD_PROP[prop];
	}
	else
	{
		if (prop < 0 || prop > NSFP_SONGPROP_COUNT) return info;
		PD = &NSFPD_SONGPROP[prop];
	}
	info.key = PD->key;
	info.name = local_text(PD->text);
	info.type = PD->type;
	return info;
}

const char* NSFCore::local_text(sint32 textenum) const
{
	return NSFCore::local_text(textenum,setting[NSFP_SET_LOCALE]);
}

const char* NSFCore::local_text(sint32 textenum, sint32 locale)
{
	if (locale < 0 || locale >= NSFP_LOCALE_COUNT || textenum < 0 || textenum >= NSFP_TEXT_COUNT)
	{
		// text 0 is a default <MISSING TEXT> value
		locale = 0;
		textenum = 0;
	}
	return (const char*)(NSFPD_LOCAL_TEXT_DATA + NSFPD_LOCAL_TEXT[locale][textenum]);
}
