// cmd.cpp
//   Main entry point for nsfplac.

#include <nsfplaycore.h>
#include <cstdio> // std::fprintf
#include <cstdlib> // std::exit, std::atexit
#include <cstring> // std::strlen
#include <cstddef> // NULL
#include <cstdarg> // va_list, va_start

// platform specific abstractions (platform.cpp)
void platform_setup(int argc, char** argv);
void platform_shutdown();
int platform_argc();
const char* platform_argv(int index);
FILE* platform_fopen(const char* path, const char* mode);

//
// logging functions
//

void error_log(const NSFCore* core, int32_t code, const char* msg)
{
	(void)core;
	std::fprintf(stderr,"Error(%d): %s\n",code,msg);
}

void debug_print(const char* msg)
{
	std::fprintf(stdout,"Debug: %s\n",msg);
}

void fatal_log(const char* msg)
{
	std::fprintf(stderr,"Fatal: %s\n",msg);
	platform_shutdown();
	std::exit(-1);
}

//
// load a file
//

void* load_file(const char* filename, const char* mode, size_t& filesize)
{
	filesize = 0;
	// open file
	FILE* f = platform_fopen(filename,mode);
	if (f == NULL)
	{
		std::fprintf(stderr,"Could not open: %s\n",filename);
		return NULL;
	}
	// get size
	std::fseek(f,0,SEEK_END);
	size_t fs = std::ftell(f);
	std::fseek(f,0,SEEK_SET);
	// allocate memory
	void* fd = std::malloc(fs);
	if (fd == NULL)
	{
		std::fprintf(stderr,"Out of memory loading: %s\n",filename);
		std::fclose(f);
		return NULL;
	}
	// read data
	size_t frs = std::fread(fd,1,fs,f);
	if (frs != fs)
	{
		std::fprintf(stderr,"Read error in: %s\n",filename);
		std::fclose(f);
		std::free(fd);
		return NULL;
	}
	std::fclose(f);
	filesize = fs;
	return fd;
}

//
// command line parsing
//

struct
{
	int input = -1;
	int waveout = -1;
	int unit_test = -1;
	int save_default_ini = -1;
	bool help = false;
} arg;

int parse_commandline(NSFCore* core) // returns -1 on success, otherwise is index of bad argument
{
	bool implied_ini = false;
	arg.input = -1;
	arg.waveout = -1;
	arg.unit_test = -1;
	arg.save_default_ini = -1;

	#ifdef DEBUG
		for (int i=0; i<platform_argc(); ++i) std::printf("Debug: arg[%d] = \"%s\"\n",i,platform_argv(i));
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
			case 'i': ++i;
				if (arg.input >= 0) return i; // too many input files
				arg.input = i; break;
			case 'w': ++i; arg.waveout = i; break;
			case 'u': ++i; arg.unit_test = i; break;
			case 'd': ++i; arg.save_default_ini = i; break;
			case 'n': implied_ini = false; break;
			case 'a': ++i; implied_ini = false; break; // any commandline ini disables the implied ini
			//case 's': TODO solo channel by key
			//case 'm': TODO mute channel by key
			//case 'p': TODO fade time override
			//case 't': TODO time override
			default: return i; // unknown single character argument
			}
		}
		else { } // assume, core setting, do in second pass
	}

	// load implied ini file
	if (implied_ini)
	{
		// TODO
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
			case 'w': ++i; break;
			case 'u': ++i; break;
			case 'd': ++i; break;
			case 'n': break;
			case 'a':
				++i;
				// TODO load the ini
				break;
			//case 's':
			//case 'm':
			//case 'p':
			//case 'f':
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
	"options:\n"
	"  -h        print command line help\n"
	"  -i file   set input file (useful if filename begins with -)\n"
	"  -w file   wave output to file\n"
	"  -v        wave output all tracks in file, use TITLE_FORMAT as filename\n"
	"  -u file   run unit test file\n"
	"  -d file   save default settings to an ini file\n"
	"  -n        don't automatically load the user ini\n"
	"  -a file   use additional ini file (implies -n)\n"
	"  -s chan   solo channel\n"
	"  -m chan   mute channel\n"
	"  -p smps   play time override in samples\n"
	"  -f smps   fade time override in samples\n"
	"can also use ini file settings, examples:\n"
	"  -CPU_NTSC=1789772\n"
	"  -REGION=DENDY\n"
	"  \"-TITLE_FORMAT=my title format\"\n"
	"  -OVERRIDE_NSF_SONG=$2A\n"
	"  \"-WAVEOUT_SAMPLERATE = 9000\"\n"
	;

//
// main
//

int main(int argc, char** argv)
{
	platform_setup(argc,argv);
	nsfplay_set_error_log(error_log);
	nsfplay_set_debug_print(debug_print);
	nsfplay_set_fatal(fatal_log);

	NSFCore* core = nsfplay_create();
	if (core == NULL) fatal_log("Out of memory.");

	// parse command line
	{
		int bad_arg = parse_commandline(core);
		if (bad_arg >= 0)
		{
			std::fprintf(stderr,"Invalid argument %d: %s\n",bad_arg,platform_argv(bad_arg));
			std::exit(-1);
		}
	}

	// print help
	if (arg.help)
	{
		std::printf(HELP_TEXT);
		// TODO print channel shortname list?
		nsfplay_destroy(core);
		platform_shutdown();
		std::exit(0);
	}

	// load input file
	if (arg.input >= 0)
	{
		size_t fs;
		void* fd = load_file(platform_argv(arg.input),"rb",fs);
		nsfplay_load(core,fd,uint32_t(fs));
		std::free(fd);
	}

	/*
	// test info
	printf("GROUPS:\n");
	for (int i=0;i<NSF_GROUP_COUNT;++i)
	{
		NSFGroupInfo info = nsfplay_set_group_info(core,i);
		printf("%s %s - %s\n",info.key,info.name,info.desc);
	}
	printf("SETS:\n");
	for (int i=0;i<NSF_SET_COUNT;++i)
	{
		NSFSetInfo info = nsfplay_set_info(core,i);
		printf("%s %s %s - %s %d %d %d %d %s\n",
			nsfplay_set_group_info(core,info.group).key,
			info.key,info.name,info.desc,
			int(info.is_string),info.default_int,info.min_int,info.max_int,
			info.default_str ? info.default_str : "<NULL>");
		if (info.list)
		{
			printf("List:");
			const char* e = info.list;
			for (int j=info.min_int; j<=info.max_int; ++j)
			{
				printf(" %d=[%s]",j,e);
				e += std::strlen(e)+1;
			}
			printf("\n");
		}
	}
	*/

	// test props
	printf("PROPS:\n");
	int32_t last_group = -1;
	for (int i=0;i<NSF_PROP_COUNT;++i)
	{
		NSFPropInfo info = nsfplay_prop_info(core,i);
		if (info.group != last_group)
		{
			last_group = info.group;
			NSFGroupInfo ginfo = nsfplay_group_info(core,last_group);
			printf("PROP GROUP: %s\n",ginfo.name);
		}
		//printf("%s %s %d\n",info.key,info.name,info.type);
		if (nsfplay_prop_exists(core,i))
		{
			if (info.type == NSF_PROP_TYPE_INT || info.type == NSF_PROP_TYPE_LIST)
				printf("%s: %d\n",info.key,nsfplay_prop_int(core,i));
			else if (info.type == NSF_PROP_TYPE_STR)
				printf("%s: %s\n",info.key,nsfplay_prop_str(core,i));
			else if (info.type == NSF_PROP_TYPE_BLOB)
			{
				uint32_t blob_size = 0;
				const uint8_t* blob = reinterpret_cast<const uint8_t*>(nsfplay_prop_blob(core,&blob_size,i));
				printf("%s: %d bytes\n",info.key,blob_size);
				const int COLS = 16;
				for (uint32_t j=0; j<blob_size; j+=COLS) // print rows of hex + ascii
				{
					for (uint32_t k=0; k<COLS; ++k) if ((j+k)<(blob_size)) { printf(" %02X",blob[j+k]); } else { printf("   "); }
					printf(" ");
					for (uint32_t k=0; k<COLS; ++k)
					{
						if ((j+k)<(blob_size))
						{
							uint8_t c = blob[j+k];
							if (c > 0x20 && c < 0x7F) printf("%c",c);
							else printf("."); // unprintable
						}
						else printf(" ");
					}
					printf("\n");
				}
			}
			else if (info.type == NSF_PROP_TYPE_LINES)
			{
				int32_t lines = nsfplay_prop_lines(core,i);
				printf("%s: %d lines\n",info.key,lines);
				const char* line;
				while ((line = nsfplay_prop_line(core)) != NULL)
					printf(" %s\n",line);
			}
			else
				printf("%s (Type: %d)\n",info.key,info.type);
		}
	}

	// test ini generation
	for (int i=0;i<NSF_SET_COUNT;++i)
		printf("%s\n",nsfplay_ini_line(core,i));

	/*
	// test ini write
	nsfplay_ini_write(core,stdout);
	*/

	//nsfplay_destroy(core);
	platform_shutdown();
	return 0;
}
