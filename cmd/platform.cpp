// platform.cpp
//   sets up the console for the chosen platform

#if defined(_WIN32) || defined(_WIN64)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetConsoleOutputCP, SetConsoleOutputCP, WireCharToMultiByte, MultiByteToWideChar
#include <shellapi.h> // GetCommandLineW, CommandLineToArgW
#include <cstdlib> // std::malloc, std::free
#include <cstdio> // std::_wfopen

#if defined(_MSC_VER)
#pragma warning(disable:6387) // MSVC thinks store_wconvert could be NULL and warns about _wfopen_s
#endif

static UINT store_cp = 0;
static LPWSTR* store_argv;
static int store_argc;
static char* store_convert = NULL;
static int store_convert_size = 0;
static wchar_t* store_wconvert = NULL;
static int store_wconvert_size = 0;

UINT set_code_page(UINT cp)
{
	UINT old_cp = GetConsoleOutputCP();
	SetConsoleOutputCP(cp);
	#ifdef DEBUG
		UINT new_cp = GetConsoleOutputCP();
		printf("Console code page: %5d -> %5d\n",old_cp,new_cp);
	#endif
	return old_cp;
}

extern "C" {
void platform_atexit()
{
	set_code_page(store_cp);
}
}

void platform_setup(int argc, char** argv)
{
	// Ignore argc/argv from main and get the wide command line from Windows
	(void)argc;
	(void)argv;
	store_argv = CommandLineToArgvW(GetCommandLineW(), &store_argc);

	// Windows needs its console set to a UTF8 code page
	store_cp = set_code_page(CP_UTF8);
	std::atexit(platform_atexit);
}

void platform_shutdown()
{
	std::free(store_wconvert);
	std::free(store_convert);
	LocalFree(store_argv);
}

int platform_argc()
{
	return store_argc;
}

const char* platform_argv(int index)
{
	// convert wchar arguments to UTF-8
	int new_size = WideCharToMultiByte(CP_UTF8,0,store_argv[index],-1,NULL,0,NULL,NULL);
	if (new_size > store_convert_size)
	{
		free(store_convert);
		store_convert_size = new_size;
		store_convert = (char*)std::malloc(store_convert_size);
	}
	WideCharToMultiByte(CP_UTF8,0,store_argv[index],-1,store_convert,store_convert_size,NULL,NULL);
	return store_convert;
}

FILE* platform_fopen(const char* path, const char* mode)
{
	// Windows filesystem is wchar native, convert UTF8 paths and use native wide fopen
	#ifdef DEBUG
		printf("fopen(\"%s\",\"%s\")\n",path,mode);
	#endif
	static wchar_t wmode[16];
	MultiByteToWideChar(CP_UTF8,0,mode,-1,wmode,sizeof(wmode)/sizeof(wmode[0]));

	int new_size = MultiByteToWideChar(CP_UTF8,0,path,-1,NULL,0);
	if (new_size > store_wconvert_size)
	{
		free(store_wconvert);
		store_wconvert_size = new_size;
		store_wconvert = (wchar_t*)std::malloc(store_wconvert_size*sizeof(wchar_t));
	}
	MultiByteToWideChar(CP_UTF8,0,path,-1,store_wconvert,store_wconvert_size);

	FILE* f;
	if (0 == _wfopen_s(&f,store_wconvert,wmode)) return f;
	return NULL;
}

#else

#include <cstdio> // std::fopen

// other platforms assume a UTF-8 by default

static int store_argc;
static const char* const* store_argv;

void platform_setup(int argc, char** argv)
{
	store_argc = argc;
	store_argv = argv;
}

void platform_shutdown()
{
}

int platform_argc()
{
	return store_argc;
}

const char* platform_argv(int index)
{
	return store_argv[index];
}

FILE* platform_fopen(const char* path, const char* mode)
{
	#ifdef DEBUG
		printf("fopen(\"%s\",\"%s\")\n",path,mode);
	#endif
	return std::fopen(path,mode);
}

#endif
