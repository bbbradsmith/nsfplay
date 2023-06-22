#include "fileutil.h"

#include <windows.h>

FILE* fopen_utf8(const char* filename, const char* mode)
{
	const int MAX_WPATH = 2048;
	const int MAX_WMODE = 16;
	wchar_t wfilename[MAX_WPATH];
	wchar_t wmode[MAX_WMODE];

	utf8_file(filename,wfilename,MAX_WPATH);
	utf8_file(mode,wmode,MAX_WMODE);
	return _wfopen(wfilename,wmode);
}
