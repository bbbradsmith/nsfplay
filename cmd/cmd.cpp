// cmd.cpp
//   Main entry point for nsfplac.

#define DEFAULT_INI      "nsfplay.ini"
#define DEFAULT_INI_ENV  "NSFPLAY_INI"

#include <nsfplaycore.h>
#include <cstdio> // std::vfprintf, std::snprintf
#include <cstdlib> // std::exit, std::atexit, std::strtol, std::malloc, std::free
#include <cstring> // std::strlen, std::memset
#include <cstddef> // NULL
#include <cstdarg> // va_list, va_start
#include <cerrno> // cerrno
#include <thread> // std::this_thread::sleep_for
#include "../shared/sound.h"

// platform specific abstractions (platform.cpp)
void platform_setup(int argc, char** argv);
void platform_shutdown();
int platform_argc();
const char* platform_argv(int index); // note: return only valid until next argv/getenv
const char* platform_getenv(const char* name); // note: return only valid until next argv/getenv, name limit of 63 chars
FILE* platform_fopen(const char* path, const char* mode);
int platform_nonblock_getc(); // get a keypress, 0 if none waiting

// unit testing (unit_test.cpp)
int unit_test(const char* path);

//
// global data
//

NSFCore* core = NULL;

//
// logging functions
//

void err_printf(const char* fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	std::vfprintf(stderr,fmt,args);
}

void out_printf(const char* fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	std::vfprintf(stdout,fmt,args);
}

void error_log(const NSFCore* core_, int32_t code, const char* msg)
{
	(void)core_;
	err_printf("Error(%d): %s\n",code,msg);
}

void debug_print(const char* msg)
{
	out_printf("Debug: %s\n",msg);
}

void fatal_log(const char* msg)
{
	err_printf("Fatal: %s\n",msg);
	platform_shutdown();
	std::exit(-1);
}

//
// load files
//

static void* load_file(const char* path, const char* mode, size_t& filesize, bool silent_notfound=false)
{
	filesize = 0;
	// open file
	FILE* f = platform_fopen(path,mode);
	if (f == NULL)
	{
		if (!silent_notfound) err_printf("Could not open: %s\n",path);
		return NULL;
	}
	// get size
	std::fseek(f,0,SEEK_END);
	size_t fs = std::ftell(f);
	std::fseek(f,0,SEEK_SET);
	// allocate memory
	uint8_t* fd = reinterpret_cast<uint8_t*>(std::malloc(fs+1));
	if (fd == NULL)
	{
		err_printf("Out of memory loading: %s\n",path);
		std::fclose(f);
		return NULL;
	}
	fd[fs] = 0; // terminating zero, needed for some purposes (e.g. ini file)
	// read data
	size_t frs = std::fread(fd,1,fs,f);
	if (frs != fs)
	{
		err_printf("Read error in: %s\n",path);
		std::fclose(f);
		std::free(fd);
		return NULL;
	}
	std::fclose(f);
	filesize = fs;
	return fd;
}

static bool load_ini(const char* path, bool silent_notfound=false)
{
	size_t filesize;
	char* ini = reinterpret_cast<char*>(load_file(path,"rt",filesize,silent_notfound));
	if (!ini) return false;
	nsfplay_set_ini(core,ini);
	out_printf("Loaded INI: %s\n",path);
	std::free(ini);
	return true;
}

//
// command line parsing
//

static bool parse_i32(const char* s, int32_t& v)
{
	int radix = 10;
	if (*s == '$') { radix=16; ++s; } // $ prefix selects hexadecimal integer
	errno=0;
	v = std::strtol(s,NULL,radix);
	if (errno) return false;
	return true;
}

static bool parse_i64(const char* s, int64_t& v)
{
	int radix = 10;
	if (*s == '$') { radix=16; ++s; }
	errno=0;
	v = std::strtoll(s,NULL,radix);
	if (errno) return false;
	return true;
}

static int32_t parse_chan(const char* s, bool ch[NSF_CHANNEL_COUNT])
{
	for (int32_t i=0; i<NSF_CHANNEL_COUNT; ++i)
	{
		const char* key = nsfplay_channel_info(core,i).key;
		const char* sm = s;
		while (*key && *sm)
		{
			char ck = *key;
			char cs = *sm;
			if (ck >= 'a' && ck <= 'z') ck = (ck - 'a') + 'A';
			if (cs >= 'a' && cs <= 'z') cs = (cs - 'a') + 'A';
			if (ck != cs) break;
		}
		if (*key == 0 && *sm == 0)
		{
			ch[i] = true;
			return i;
		}
	}
	return -1;
}

struct
{
	int input = -1;
	int waveout = -1;
	int waveout_multi = -1;
	int unit_test = -1;
	int save_default_ini = -1;
	int32_t track = 0;
	int32_t fade = -1;
	int64_t time = -1;
	bool help = false;
	bool solo[NSF_CHANNEL_COUNT] = {0};
	bool mute[NSF_CHANNEL_COUNT] = {0};
} arg;

int parse_commandline() // returns -1 on success, otherwise is index of bad argument
{
	bool implied_ini = true;
	arg.input = -1;
	arg.waveout = -1;
	arg.waveout_multi = -1;
	arg.unit_test = -1;
	arg.save_default_ini = -1;
	arg.track = 0;
	arg.fade = -1;
	arg.time = -1;
	arg.help = false;
	std::memset(arg.solo,0,sizeof(arg.solo));
	std::memset(arg.mute,0,sizeof(arg.mute));

	#ifdef DEBUG
		for (int i=0; i<platform_argc(); ++i) out_printf("Debug: arg[%d] = \"%s\"\n",i,platform_argv(i));
	#endif

	// start with default settings
	nsfplay_set_default(core);

	// first pass does not change any settings
	for (int i=1; i<platform_argc(); ++i)
	{
		const char* v = platform_argv(i);
		if (v[0] != '-') // input file
		{
			if (arg.input >= 0) return i; // too many input files
			arg.input =  i;
			continue;
		}
		if (v[1] != 0 && v[2] == 0) // single character argument
		{
			char c = v[1]; if (c >= 'A' && c <= 'Z') c += 'a'-'A';
			switch(v[1])
			{
			case 'h': arg.help = true; break;
			case 'i': ++i; { if (arg.input >= 0) return i; } arg.input = i; break;
			case 't': ++i; if (!parse_i32(platform_argv(i),arg.track)) return i; break;
			case 'w': ++i; arg.waveout = i; break;
			case 'v': ++i; arg.waveout_multi = i; break;
			case 'd': ++i; arg.save_default_ini = i; break;
			case 'a': ++i; implied_ini = false; break; // any commandline ini disables the implied ini
			case 'n': implied_ini = false; break;
			case 's': ++i; if (parse_chan(platform_argv(i),arg.solo) < 0) return i; break;
			case 'm': ++i; if (parse_chan(platform_argv(i),arg.mute) < 0) return i; break;
			case 'p': ++i; if (!parse_i64(platform_argv(i),arg.time)) return i; break;
			case 'f': ++i; if (!parse_i32(platform_argv(i),arg.fade)) return i; break;
			case 'u': ++i; arg.unit_test = i; break;
			default: return i; // unknown single character argument
			}
		}
		else { } // assume, core setting, do in second pass
	}

	// load implied default INI file
	if (implied_ini)
	{
		if (!load_ini(DEFAULT_INI,true)) // look in current directory
		{
			static_assert(sizeof(DEFAULT_INI_ENV)<=64,"platform_getenv has a 64 character limit");
			const char* path = platform_getenv(DEFAULT_INI_ENV);
			if (!path || !load_ini(path,true))
			{
				out_printf("No default INI file found.\n");
			}
		}
	}

	// second pass apply settings
	for (int i=1; i<platform_argc(); ++i)
	{
		const char* v = platform_argv(i);
		if (v[0] != '-') continue;
		if (v[1] != 0 && v[2] == 0)
		{
			char c = v[1]; if (c >= 'A' && c <= 'Z') c += 'a'-'A';
			switch(v[1])
			{
			case 'h': ++i; break;
			case 'i': ++i; break;
			case 't': ++i; break;
			case 'w': ++i; break;
			case 'v': ++i; break;
			case 'd': ++i; break;
			case 'a': ++i; if(!load_ini(platform_argv(i))) return i; break;
			case 'n': break;
			case 's': ++i; break;
			case 'm': ++i; break;
			case 'p': ++i; break;
			case 'f': ++i; break;
			case 'u': ++i; break;
			default: return i;
			}
		}
		else // apply as core setting
		{
			if (!nsfplay_set_ini_line(core,v+1))
				return i; // invalid core setting
		}
	}
	return -1;
}

const char HELP_TEXT[] =
	"nsfplac command line help:\n"
	"  Any argument beginning with - is an option, otherwise it gives the input file.\n"
	"  Only one input files is permitted.\n"
	"  A default INI settings file \"" DEFAULT_INI "\" will be applied from the current directory.\n"
	"    If this INI file is not found, the \"" DEFAULT_INI_ENV "\" environment variable will be used\n"
	"    to look for another INI file, and applied if found. -n disables this default INI.\n"
	"options:\n"
	"  -h        print command line help\n"
	"  -i file   set input file (useful if filename begins with -)\n"
	"  -t num    set starting track (1-256)\n"
	"  -w file   wave output to file\n"
	"  -v path   wave output all tracks in path, appending TITLE_FORMAT + .wav as filename\n"
	"  -d file   create a new INI file with default settings\n"
	"  -a file   apply INI file (implies -n)\n"
	"  -n        don't automatically load the default INI\n"
	"  -s chan   solo channel (can use multiple -s arguments to get more than 1 channel)\n"
	"  -m chan   mute channel\n"
	"  -p smps   play time override in samples\n"
	"  -f smps   fade time override in samples\n"
	"  -u file   run unit test file\n"
	"INI file settings may also be used, examples:\n"
	"  -CPU_NTSC=1789772\n"
	"  -REGION=DENDY\n"
	"  \"-TITLE_FORMAT=my title format\"\n"
	"  -OVERRIDE_NSF_SONG=$2A\n"
	"  \"-WAVEOUT_SAMPLERATE = 9000\"\n"
	;

void help_chans()
{
	out_printf("channels:");
	int32_t last_unit = -1;
	for (int32_t i=0; i<NSF_CHANNEL_COUNT; ++i)
	{
		NSFChannelInfo info = nsfplay_channel_info(core,i);
		if (info.unit != last_unit)
		{
			out_printf("\n ");
			last_unit = info.unit;
		}
		out_printf(" %s",info.short_name);
	}
	out_printf("\n");
}

//
// WAVE file output
//

static bool waveout(const char* path)
{
	(void)path;
	// TODO
	// open file, print error and return fale if failed
	// print song info brief
	// print a progress bar [----]\r
	// fill in the progress one character at a time (so a log will just make 2 lines)
	// fixup WAV header for file length
	return false;
}

//
// main
//

int run()
{
	// parse command line
	{
		int bad_arg = parse_commandline();
		if (bad_arg >= 0)
		{
			err_printf("Invalid argument %d: %s\n",bad_arg,platform_argv(bad_arg));
			out_printf("Try -h for command line usage help.\n");
			return -1;
		}
	}

	// print help
	if (arg.help)
	{
		out_printf(HELP_TEXT);
		help_chans();
		return 0;
	}

	// save default ini
	if (arg.save_default_ini >= 0)
	{
		const char* path = platform_argv(arg.save_default_ini);
		out_printf("Generate default INI file: %s\n",path);
		FILE* f = platform_fopen(path,"wt");
		if (f == NULL)
		{
			err_printf("Could not open: %s\n",path);
			return -1;
		}
		nsfplay_set_default(core);
		bool result = nsfplay_ini_write(core,f);
		std::fclose(f);
		if (!result)
		{
			err_printf("Error writitng to file: %s\n",path);
			return -1;
		}
		out_printf("Success.\n");
		return 0;
	}

	// unit test
	if (arg.unit_test >= 0)
	{
		return unit_test(platform_argv(arg.unit_test));
	}

	// load input file
	if (arg.input >= 0)
	{
		size_t fs;
		void* fd = load_file(platform_argv(arg.input),"rb",fs);
		nsfplay_load(core,fd,uint32_t(fs));
		std::free(fd);
	}
	// TODO print NSF info brief
	
	// set track if requested
	if (arg.track > 0) nsfplay_song(core,uint8_t(arg.track-1));

	// WAV file output
	if (arg.waveout >= 0)
	{
		if (!waveout(platform_argv(arg.waveout))) return -1;
		out_printf("Success.\n");
		return 0;
	}
	if (arg.waveout_multi >= 0)
	{
		if (nsfplay_song_count(core) < 1)
		{
			err_printf("No songs in input file.\n");
			return -1;
		}
		for (int32_t i=0; i<nsfplay_song_count(core); ++i)
		{
			nsfplay_song(core,uint8_t(i));
			const int PATH_SIZE = 2048;
			char path[PATH_SIZE];
			std::snprintf(path,PATH_SIZE,"%s%s.wav",platform_argv(arg.waveout_multi),nsfplay_prop_str(core,NSF_PROP_SONG_TITLE));
			if (!waveout(path)) return -1;
		}
		out_printf("Success.\n");
		return 0;
	}

	// TODO regular playback with prompt
	// print song brief
	// enter prompt with playback loop (if autoplay, otherwise just prompt)
	// prompt is a branching menu

	// TODO  the rest of this function is test code I am keeping as an example for later menus

	sound_setup(core);
	out_printf(sound_debug_text());

	// test of non blocking getc
	out_printf("Q to quit...\n");
	int kc = -1;
	while (kc != 'q' && kc != 'Q')
	{
		out_printf("check?\n");
		int kn = platform_nonblock_getc();
		if (kn)
		{
			kc = kn;
			out_printf("key %3d $%02X '%c'\n",kc,kc,char(kc));
		}
		else
		{
			out_printf("Waiting...\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

	sound_shutdown();

	/*
	// test info
	out_printf("GROUPS:\n");
	for (int i=0;i<NSF_GROUP_COUNT;++i)
	{
		NSFGroupInfo info = nsfplay_set_group_info(core,i);
		out_printf("%s %s - %s\n",info.key,info.name,info.desc);
	}
	out_printf("SETS:\n");
	for (int i=0;i<NSF_SET_COUNT;++i)
	{
		NSFSetInfo info = nsfplay_set_info(core,i);
		out_printf("%s %s %s - %s %d %d %d %d %s\n",
			nsfplay_set_group_info(core,info.group).key,
			info.key,info.name,info.desc,
			int(info.is_string),info.default_int,info.min_int,info.max_int,
			info.default_str ? info.default_str : "<NULL>");
		if (info.list)
		{
			out_printf("List:");
			const char* e = info.list;
			for (int j=info.min_int; j<=info.max_int; ++j)
			{
				out_printf(" %d=[%s]",j,e);
				e += std::strlen(e)+1;
			}
			out_printf("\n");
		}
	}

	// test props
	out_printf("PROPS:\n");
	int32_t last_group = -1;
	for (int i=0;i<NSF_PROP_COUNT;++i)
	{
		NSFPropInfo info = nsfplay_prop_info(core,i);
		if (info.group != last_group)
		{
			last_group = info.group;
			NSFGroupInfo ginfo = nsfplay_group_info(core,last_group);
			out_printf("PROP GROUP: %s\n",ginfo.name);
		}
		//out_printf("%s %s %d\n",info.key,info.name,info.type);
		if (nsfplay_prop_exists(core,i))
		{
			if (info.type == NSF_PROP_TYPE_INT || info.type == NSF_PROP_TYPE_LIST)
				out_printf("%s: %d\n",info.key,nsfplay_prop_int(core,i));
			else if (info.type == NSF_PROP_TYPE_STR)
				out_printf("%s: %s\n",info.key,nsfplay_prop_str(core,i));
			else if (info.type == NSF_PROP_TYPE_BLOB)
			{
				uint32_t blob_size = 0;
				const uint8_t* blob = reinterpret_cast<const uint8_t*>(nsfplay_prop_blob(core,&blob_size,i));
				out_printf("%s: %d bytes\n",info.key,blob_size);
				const int COLS = 16;
				for (uint32_t j=0; j<blob_size; j+=COLS) // print rows of hex + ascii
				{
					for (uint32_t k=0; k<COLS; ++k) if ((j+k)<(blob_size)) { out_printf(" %02X",blob[j+k]); } else { out_printf("   "); }
					out_printf(" ");
					for (uint32_t k=0; k<COLS; ++k)
					{
						if ((j+k)<(blob_size))
						{
							uint8_t c = blob[j+k];
							if (c > 0x20 && c < 0x7F) out_printf("%c",c);
							else out_printf("."); // unprintable
						}
						else out_printf(" ");
					}
					out_printf("\n");
				}
			}
			else if (info.type == NSF_PROP_TYPE_LINES)
			{
				int32_t lines = nsfplay_prop_lines(core,i);
				out_printf("%s: %d lines\n",info.key,lines);
				const char* line;
				while ((line = nsfplay_prop_line(core)) != NULL)
					out_printf(" %s\n",line);
			}
			else
				out_printf("%s (Type: %d)\n",info.key,info.type);
		}
	}

	// test ini generation
	for (int i=0;i<NSF_SET_COUNT;++i)
		out_printf("%s\n",nsfplay_ini_line(core,i));

	// test ini write
	nsfplay_ini_write(core,stdout);
	*/

	return 0;
}

int main(int argc, char** argv)
{
	platform_setup(argc,argv);
	nsfplay_set_error_log(error_log);
	nsfplay_set_debug_print(debug_print);
	nsfplay_set_fatal(fatal_log);

	core = nsfplay_create();
	if (core == NULL) fatal_log("Out of memory.");

	int result = run();

	nsfplay_destroy(core);
	platform_shutdown();
	return result;
}
