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

static_assert(sizeof(char)==sizeof(uint8),"char must be byte sized, because UTF-8 is assumed");

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

// skip UTF-8 BOM if it exists
inline const char* utf8_bom_skip(const char* text)
{
	if ((unsigned char)(text[0]) == 0xEF &&
	    (unsigned char)(text[1]) == 0xBB &&
	    (unsigned char)(text[2]) == 0xBF)
		return text+3;
	return text;
}

namespace nsf {

void* (*custom_alloc)(size_t size) = NULL;
void* (*custom_free)(void* ptr) = NULL;
void (*error_callback)(const char* msg) = NULL;
void (*debug_print_callback)(const char* msg) = NULL;
void (*fatal_callback)(const char* msg) = NULL;

void* alloc(size_t size)
{
	void* a;
	if (custom_alloc) a = custom_alloc(size);
	else              a = std::malloc(size);
	if (a == NULL) nsf::fatal("Out of memory.");
	#if DEBUG_ALLOC
		debug_alloc[a] = size;
		debug_alloc_total += size;
		NSF_DEBUG("alloc(%7zu) total %7zu in %3zu",size,debug_alloc_total,debug_alloc.size());
	#else
		NSF_DEBUG("alloc(%7zu)",size);
	#endif
	return a;
}

void free(void* ptr)
{
	if (custom_free) custom_free(ptr);
	else             std::free(ptr);
	#if DEBUG_ALLOC
		size_t size = debug_alloc[ptr];
		debug_alloc_total -= size;
		debug_alloc.erase(ptr);
		NSF_DEBUG("free(-%7zu) total %7zu in %3zu",size,debug_alloc_total,debug_alloc.size());
	#else
		NSF_DEBUG("free()");
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
	NSF_UNUSED(fmt);
#endif
}

void fatal(const char* msg)
{
	if (fatal_callback) fatal_callback(msg);
	std::fprintf(stderr,"%s\n",msg);
	std::exit(-1);
}

} // namespace nsf

NSFCore* NSFCore::create()
{
	NSFCore* core = (NSFCore*)nsf::alloc(sizeof(NSFCore));
	NSF_DEBUG("create()");
	std::memset(core,0,sizeof(NSFCore));
	core->set_default();
	return core;
}

void NSFCore::destroy(NSFCore* core)
{
	NSF_DEBUG("destroy()");
	core->release();
	nsf::free(core);
}

void NSFCore::release()
{
	// release any allocations held
	if (nsf_free)
		nsf::free(const_cast<uint8*>(nsf));
	for (int si=0; si<NSF_SETSTR_COUNT; ++si)
		if (setting_str_free[si])
			nsf::free(const_cast<char*>(setting_str[si]));
}

const char* NSFCore::last_error() const
{
	const char* r = error_last;
	error_last = NULL;
	return r;
}

void NSFCore::set_error(sint32 textenum,...) const
{
#if !(NSF_NOTEXT)
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
		if (nsf::error_callback) nsf::error_callback(error_last);
		else { NSF_DEBUG("ERROR: %s",error_last); } // send errors to debug if not logged
		return;
	}
	// format
	va_list args;
	va_start(args,textenum);
	std::vsnprintf(error_last_buffer,sizeof(error_last_buffer),fmt,args);
	error_last_buffer[sizeof(error_last_buffer)-1] = 0;
	error_last = error_last_buffer;
#else
	NSF_UNUSED(textenum);
	error_last = "";
#endif
	if (nsf::error_callback) nsf::error_callback(error_last);
	else { NSF_DEBUG("ERROR: %s",error_last); }
}

void NSFCore::set_ini_error(int linenum, sint32 textenum,...) const
{
#if !(NSF_NOTEXT)
	const char* fmt = local_text(textenum);
	// line number prefix if linenum < 0
	if (linenum < 0) error_last_buffer[0] = 0;
	else
	{
		std::snprintf(error_last_buffer,sizeof(error_last_buffer),"INI line %d: ",linenum);
		error_last_buffer[sizeof(error_last_buffer)-1] = 0;
	}
	size_t start = std::strlen(error_last_buffer);
	// append format
	va_list args;
	va_start(args,textenum);
	std::vsnprintf(error_last_buffer+start,sizeof(error_last_buffer)-start,fmt,args);
	error_last_buffer[sizeof(error_last_buffer)-1] = 0;
	error_last = error_last_buffer;
#else
	NSF_UNUSED(linenum);
	NSF_UNUSED(textenum);
	error_last = "";
#endif
	if (nsf::error_callback) nsf::error_callback(error_last);
	else { NSF_DEBUG("ERROR: %s",error_last); }
}

void NSFCore::set_error_raw(const char* fmt,...) const
{
	va_list args;
	va_start(args,fmt);
	std::vsnprintf(error_last_buffer,sizeof(error_last_buffer),fmt,args);
	error_last_buffer[sizeof(error_last_buffer)-1] = 0;
	error_last = error_last_buffer;
	if (nsf::error_callback) nsf::error_callback(error_last);
	else { NSF_DEBUG("ERROR: %s",error_last); }
}

void NSFCore::set_default()
{
	for (int i=0; i<NSF_SET_COUNT; ++i)
	{
		setting[i] = NSFD_SET[i].default_int;
		if (NSFD_SET[i].default_str != NULL)
		{
			sint32 si = setting[i];
			if (setting_str_free[si])
				nsf::free(const_cast<char*>(setting_str[si]));
			setting_str[si] = NSFD_SET[i].default_str;
			setting_str_free[si] = false;
		}
	}
}

bool NSFCore::set_ini(const char* ini)
{
#if !(NSF_NOTEXT)
	if (ini == NULL) return true;
	// skip UTF-8 BOM if present
	ini = utf8_bom_skip(ini);
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
#else
	NSF_UNUSED(ini);
	return false;
#endif
}

bool NSFCore::set_init(const NSFSetInit* init, bool assume_str)
{
	if (init == NULL) return true;
	bool result = true;
	for (int i=0; init[i].setenum >= 0; ++i)
	{
		if (init[i].val_str != NULL)
			result &= set_str(init[i].setenum,init[i].val_str,assume_str);
		else
			result &= set_int(init[i].setenum,init[i].val_int);
	}
	return result;
}

bool NSFCore::set_int(sint32 setenum, sint32 value)
{
	if (setenum < 0 || setenum > NSF_SET_COUNT)
	{
		set_error(NSF_ERROR_SET_INVALID);
		return false;
	}
	const NSFSetData& SD = NSFD_SET[setenum];
	if (SD.default_str != NULL)
	{
		set_error(NSF_ERROR_SET_TYPE);
		return false;
	}
	if (value < SD.min_int || value > SD.max_int)
	{
		set_error(NSF_ERROR_SETINT_RANGE,value,SD.min_int,SD.max_int);
		return false;
	}
	setting[setenum] = value;
	return true;
}

bool NSFCore::set_str(sint32 setenum, const char* value, bool assume, sint32 len)
{
	if (setenum < 0 || setenum > NSF_SET_COUNT)
	{
		set_error(NSF_ERROR_SET_INVALID);
		return false;
	}
	const NSFSetData& SD = NSFD_SET[setenum];
	if (SD.default_str == NULL)
	{
		set_error(NSF_ERROR_SET_TYPE);
		return false;
	}
	sint32 si = setting[setenum];

	// assume directly
	if (assume)
	{
		if (setting_str_free[si]) nsf::free(const_cast<char*>(setting_str[si]));
		setting_str[si] = value;
		setting_str_free[si] = false;
		return true;
	}

	// auto-calculate length
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
	if (setting_str_free[si]) nsf::free(const_cast<char*>(setting_str[si]));
	char* new_str = static_cast<char*>(nsf::alloc(size_t(len)+1));
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
	if (setenum < 0 || setenum > NSF_SET_COUNT) return 0;
	if (NSFD_SET[setenum].default_str != NULL) return 0;
	return setting[setenum];
}

const char* NSFCore::get_str(sint32 setenum) const
{
	static const char* INVALID = "<INVALID>";
	if (setenum < 0 || setenum > NSF_SET_COUNT) return INVALID;
	if (NSFD_SET[setenum].default_str == NULL) return INVALID;
	sint32 si = setting[setenum];
	return setting_str[si];
}

sint32 NSFCore::set_enum(const char* key, int len)
{
	for (sint32 i=0; i<NSF_SET_COUNT; ++i)
	{
		if (key_match(key,len,NSFD_SET[i].key))
			return i;
	}
	return -1;
}

sint32 NSFCore::group_enum(const char* key, int len)
{
	for (sint32 i=0; i<NSF_GROUP_COUNT; ++i)
	{
		if (key_match(key,len,NSFD_GROUP[i].key))
			return i;
	}
	return -1;
}

NSFSetInfo NSFCore::set_info(sint32 setenum) const
{
	NSFSetInfo info = {0}; info.group = -1;
	info.key = info.name = info.desc = local_text(0);
	if (setenum < 0 || setenum > NSF_SET_COUNT) return info;
	const NSFSetData& SD = NSFD_SET[setenum];
	info.group = SD.group;
	info.key = SD.key;
	info.name = local_text(SD.text+0);
	info.desc = local_text(SD.text+1);
	info.is_string = (SD.default_str != NULL);
	info.default_str = SD.default_str;
	info.default_int = SD.default_int;
	info.min_int = SD.min_int;
	info.max_int = SD.max_int;
	info.min_hint = SD.min_hint;
	info.max_hint = SD.max_hint;
	info.list      = (SD.list >= 0) ? (local_text(NSFD_LIST_TEXT[SD.list]+1)) : NULL;
	info.list_keys = (SD.list >= 0) ? (local_text(NSFD_LIST_TEXT[SD.list]+0)) : NULL;
	info.display = SD.display;
	return info;
}

NSFGroupInfo NSFCore::group_info(sint32 group) const
{
	NSFGroupInfo info = {0};
	info.key = info.name = info.desc = local_text(0);
	if (group < 0 || group > NSF_GROUP_COUNT) return info;
	const NSFGroupData& GD = NSFD_GROUP[group];
	info.key = GD.key;
	info.name = local_text(GD.text+0);
	info.desc = local_text(GD.text+1);
	info.type = GD.type;
	return info;
}

const char* NSFCore::ini_line(sint32 setenum) const
{
#if !(NSF_NOTEXT)
	if (setenum < 0 || setenum >= NSF_SET_COUNT) return "";
	const NSFSetData& SD = NSFD_SET[setenum];
	if (SD.default_str == NULL)
	{
		switch (SD.display)
		{
		case NSF_DISPLAY_LIST:
			{
				const char* list_key = local_text(NSFD_LIST_TEXT[SD.list]+0);
				for (int i=0; i<setting[setenum]; ++i)
				{
					while(*list_key) ++list_key;
					++list_key;
				}
				std::snprintf(temp_text,sizeof(temp_text),"%s=%s",SD.key,list_key);
			} break;
		case NSF_DISPLAY_HEX8:
			std::snprintf(temp_text,sizeof(temp_text),"%s=$%02X",SD.key,setting[setenum]); break;
		case NSF_DISPLAY_HEX16:
			std::snprintf(temp_text,sizeof(temp_text),"%s=$%04X",SD.key,setting[setenum]); break;
		case NSF_DISPLAY_HEX32:
			std::snprintf(temp_text,sizeof(temp_text),"%s=$%08X",SD.key,setting[setenum]); break;
		case NSF_DISPLAY_COLOR:
			std::snprintf(temp_text,sizeof(temp_text),"%s=$%06X",SD.key,setting[setenum]); break;
		default:
			std::snprintf(temp_text,sizeof(temp_text),"%s=%d",   SD.key,setting[setenum]); break;
		};
	}
	else
	{
		std::snprintf(temp_text,sizeof(temp_text),"%s=%s",SD.key,get_str(setenum));
	}
	temp_text[sizeof(temp_text)-1] = 0;
	return temp_text;
#else
	NSF_UNUSED(setenum);
	return "";
#endif
}

void NSFCore::ini_write(FILE* f) const
{
#if !(NSF_NOTEXT)
	sint32 last_group = -1;
	std::fprintf(f,"# NSFPlay INI settings file\n");
	for (sint32 i=0; i<NSF_SET_COUNT; ++i)
	{
		// print a group comment each time it changes
		sint32 group = NSFD_SET[i].group;
		if (group != last_group)
		{
			last_group = group;
			std::fprintf(f,"# [%s]\n",NSFD_GROUP[last_group].key);
		}
		std::fprintf(f,"%s\n",ini_line(i));
	}
	std::fprintf(f,"# end of settings\n");
#else
	NSF_UNUSED(f);
#endif
}

bool NSFCore::parse_ini_line(const char* line, int len, int linenum)
{
#if !(NSF_NOTEXT)
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
		set_ini_error(linenum,NSF_ERROR_INI_NO_EQUALS);
		return false;
	}
	// line starts key, find end of key
	int keylen = equals;
	while (keylen > 0 && (line[keylen-1]==' ' || line[keylen-1]=='\t')) { --keylen; }
	// get enum from key
	sint32 se = set_enum(line,keylen);
	if (se < 0)
	{
		set_ini_error(linenum,NSF_ERROR_INI_BAD_KEY);
		return false;
	}
	// find start of value
	int valpos = equals+1;
	while (valpos < len && (line[valpos]==' ' || line[valpos]=='\t')) { ++valpos; }
	line += valpos;	len -= valpos;
	// se is our setting
	// line, len are now the value
	const NSFSetData& SD = NSFD_SET[se];
	if (SD.default_str != NULL)
	{
		if (!set_str(se,line,false,len) && error_last != NULL)
		{
			NSF_DEBUG("Unexpected set_str error at INI line %d: %s",linenum,error_last);
		}
		return true;
	}
	// not a string, consider integer
	const char* value_end = line + len;
	int radix = 10;
	if (*line == '$') // $ prefix selects hexadecimal integer
	{
		radix = 16;
		line += 1;
	}
	else if (SD.list >= 0 && (*line < '0' || *line > '9')) // isn't a number, could be a list key
	{
		const char* list_key = local_text(NSFD_LIST_TEXT[SD.list]+0);
		for (int i=0; i<=SD.max_int; ++i)
		{
			if (key_match(line,len,list_key))
			{
				if (!set_int(se,i) && error_last != NULL)
				{
					NSF_DEBUG("Unexpected set_int error at INI line %d: %s",linenum,error_last);
				}
				return true;
			}
			while (*list_key) ++list_key;
			++list_key;
		}
		set_ini_error(linenum,NSF_ERROR_INI_BAD_LIST_KEY,SD.key);
		return false;
	}
	char* strtol_end = const_cast<char*>(value_end);
	errno=0;
	sint32 value = std::strtol(line,&strtol_end,radix);
	if (errno || strtol_end != value_end)
	{
		set_ini_error(linenum,NSF_ERROR_INI_BAD_INT);
		return false;
	}
	if (value < SD.min_int || value > SD.max_int)
	{
		set_ini_error(linenum,NSF_ERROR_INI_BAD_RANGE,value,SD.min_int,SD.max_int);
		return false;
	}
	if (!set_int(se,value) && error_last != NULL)
	{
		NSF_DEBUG("Unexpected set_int error at INI line %d: %s",linenum,error_last);
	}
	return true;
#else
	NSF_UNUSED(line);
	NSF_UNUSED(len);
	NSF_UNUSED(linenum);
	return false;
#endif
}

bool NSFCore::load(const uint8* data, uint32 size, bool assume, bool bin)
{
	NSF_DEBUG("NSFCore::load(%s,%d,%d,%d)",data?"*":"NULL",size,assume,bin);
	if (nsf_free)
		nsf::free(const_cast<uint8*>(nsf));
	nsf = NULL;
	nsf_size = 0;
	nsf_free = false;

	// drop all potential references to the former nsf
	rom = NULL;
	pad0 = NULL;
	pad1 = NULL;
	for (int i=0; i<16; ++i)
	{
		rpage[i] = 0;
		wpage[i] = 0;
	}
	bank_last = 0;

	if (data != NULL)
	{
		if (assume)
			nsf = data;
		else
		{
			nsf = (uint8*)(nsf::alloc(size));
			std::memcpy(const_cast<uint8*>(nsf),data,size);
			nsf_free = true;
		}
		nsf_size = size;
	}

	bool result = nsf_parse(bin);
	// TODO play song (reset/rebuilds audio stacks) which may & with result
	return result;
}

NSFPropInfo NSFCore::prop_info(sint32 prop) const
{
	NSFPropInfo info = {0};
	info.type = NSF_PROP_TYPE_INVALID;
	info.key = info.name = local_text(0);
	const NSFPropData& PD = NSFD_PROP[prop];
	info.key       = PD.key;
	info.name      = local_text(PD.text);
	info.max_list  = PD.max_list;
	info.group     = PD.group;
	info.list      = (PD.list >= 0) ? (local_text(NSFD_LIST_TEXT[PD.list]+1)) : NULL;
	info.list_keys = (PD.list >= 0) ? (local_text(NSFD_LIST_TEXT[PD.list]+0)) : NULL;
	info.type      = PD.type;
	info.display   = PD.display;
	return info;
}

const char* NSFCore::local_text(sint32 textenum) const
{
#if !(NSF_NOTEXT)
	return NSFCore::local_text(textenum,SETTING(LOCALE));
#else
	NSF_UNUSED(textenum);
	return reinterpret_cast<const char*>(NSFD_NOTEXT_LIST_KEY);
#endif
}

const char* NSFCore::local_text(sint32 textenum, sint32 locale)
{
#if !(NSF_NOTEXT)
	if (locale < 0 || locale >= NSF_LOCALE_COUNT || textenum < 0 || textenum >= NSF_TEXT_COUNT)
	{
		// text 0 is a default <MISSING TEXT> value
		locale = 0;
		textenum = 0;
	}
	return reinterpret_cast<const char*>(NSFD_LOCAL_TEXT_DATA + NSFD_LOCAL_TEXT[locale][textenum]);
#else
	NSF_UNUSED(textenum);
	NSF_UNUSED(locale);
	return reinterpret_cast<const char*>(NSFD_NOTEXT_LIST_KEY);
#endif
}
