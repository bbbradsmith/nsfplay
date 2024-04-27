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
// main
//

int arg_input = -1;
const char* arg_input_file = NULL;
bool arg_ini;

int parse_commandline(NSFCore* core) // returns -1 on success, otherwise is index of bad argument
{
	#ifdef DEBUG
		for (int i=0; i<platform_argc(); ++i) std::printf("Debug: arg[%d] = \"%s\"\n",i,platform_argv(i));
	#endif
	for (int i=1; i<platform_argc(); ++i)
	{
		const char* v = platform_argv(i);

		if (v[0] != '-') // input file
		{
			if (arg_input >= 0) return i; // too many input files
			arg_input =  i;
			continue;
		}
		if (v[1] != 0 && v[2] == 0) // single character argument
		{
			switch(v[1])
			{
			// TODO
			case 'w': // TODO
			default:
				return i; // unknown argument
			}
		}
		else // core setting
		{
			if (!nsfplay_set_ini_line(core,v+1))
				return i; // invalid core setting
		}
		// TODO we want arguments that involve loading ini or core settings to apply in-order, but we also want to load default settings first IF no ini is loaded. two passes?
	}
	return -1;
}

void help_commandline()
{
	// TODO
}

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
			fprintf(stderr,"Invalid argument %d: %s\n",bad_arg,platform_argv(bad_arg));
			exit(-1);
		}
	}

	const char* TEST_INI = NULL;
	const NSFSetInit* TEST_INIT = NULL;
	/*
	const char* TEST_INI =
		"# test comment\n"
		"SAMPLERATE=12345\n"
		"    STEREO_ENABLE     =      0      \r\n"
		" error no equals\n"
		"TITLE_format =      a b   c d     \n\r"
		"\tTITLE_FORMAT=\t1234\n"
		"TITLE_FORMAT=1234\n"
		"\tTITLE_FORMAT =    1234     \n"
		"TRI_VOL =  12#34   \n"
		"error bad key  = 5\n"
		"NSE_VOL=string # error bad int\n"
		"DPCM_VOL=-3 # error range\n"
		"\n"
		"VOLUME=368";
	const NSFSetInit TEST_INIT[] = {
		{NSF_SET_DPCM_ON,0,NULL},
		{NSF_SET_DPCM_ON,-2,NULL}, // out of range
		{NSF_SET_DPCM_ON,2,NULL}, // out of range
		{NSF_SET_DPCM_ON,0,"string"}, // wrong type
		{NSF_SET_TITLE_FORMAT,0,"string"},
		{NSF_SET_TITLE_FORMAT,3,NULL}, // wrong type
		{-1,0,NULL},
	};
	*/
	nsfplay_set_ini(core,TEST_INI);
	nsfplay_set_init(core,TEST_INIT);
	/*
	//nsfplay_set_key_int(core,"TRI_ON",0);
	//nsfplay_set_key_str(core,"TITLE_FORMAT","keyed");
	nsfplay_set_ini_line(core,"SQU0_ON = off ");
	nsfplay_set_ini_line(core,"SQU1_ON=$-1");
	nsfplay_set_ini_line(core,"TRI_ON=$F");
	nsfplay_set_ini_line(core,"NSE_ON=ON");
	nsfplay_set_ini_line(core,"DPCM_ON=false");
	*/

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

	//FILE* f = platform_fopen("moon8.nsfe","rb");
	FILE* f = platform_fopen("moon8.nsf","rb");
	//FILE* f = platform_fopen("LunaAscension.nsf","rb");
	//FILE* f = platform_fopen("katoken_sky_v01.nsf","rb");
	//FILE* f = platform_fopen("ct109_v01.nsf","rb");
	if (f == NULL) fatal_log("file not found");
	fseek(f,0,SEEK_END);
	int fs = ftell(f);
	fseek(f,0,SEEK_SET);
	void* fd = malloc(fs);
	if (fd == NULL) fatal_log("out of memory");
	fread(fd,1,fs,f);
	fclose(f);
	nsfplay_load(core,fd,fs,false);
	//nsfplay_load(core,fd,fs,true);

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

	/*
	// test ini generation
	for (int i=0;i<NSF_SET_COUNT;++i)
		printf("%s\\n\n",nsfplay_ini_line(core,i));
	*/

	// test ini write
	//nsfplay_ini_write(core,stdout);

	nsfplay_destroy(core);

	platform_shutdown();
	return 0;
}
