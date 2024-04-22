// stub

#include <nsfplaycore.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void error_log(const char* msg) { std::fprintf(stderr,"Error: %s\n",msg); }
void debug_print(const char* msg) { std::fprintf(stdout,"Debug: %s\n",msg); }
void fatal_log(const char* msg) { std::fprintf(stderr,"Fatal: %s\n",msg); std::exit(-1); }

int main()
{
	nsfplay_set_error_log(error_log);
	nsfplay_set_debug_print(debug_print);
	nsfplay_set_fatal(fatal_log);
	
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
		{NSFP_SET_DPCM_ON,0,NULL},
		{NSFP_SET_DPCM_ON,-2,NULL}, // out of range
		{NSFP_SET_DPCM_ON,2,NULL}, // out of range
		{NSFP_SET_DPCM_ON,0,"string"}, // wrong type
		{NSFP_SET_TITLE_FORMAT,0,"string"},
		{NSFP_SET_TITLE_FORMAT,3,NULL}, // wrong type
		{-1,0,NULL},
	};
	NSFCore* core = nsfplay_create(TEST_INI);
	nsfplay_set_init(core,TEST_INIT);
	nsfplay_set_key_int(core,"TRI_ON",0);
	nsfplay_set_key_str(core,"TITLE_FORMAT","keyed");

	// test info
	for (int i=0;i<NSFP_GROUP_COUNT;++i)
	{
		NSFSetGroupInfo info = nsfplay_set_group_info(core,i);
		printf("%s %s - %s\n",info.key,info.name,info.desc);
	}
	for (int i=0;i<NSFP_SET_COUNT;++i)
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

	// test ini generation
	for (int i=0;i<NSFP_SET_COUNT;++i)
		printf("%s\\n\n",nsfplay_ini_line(core,i));

	nsfplay_destroy(core);

	return 0;
}
