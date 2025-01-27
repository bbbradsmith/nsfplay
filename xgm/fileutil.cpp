#include "fileutil.h"
#if defined(_WIN32)
#include <windows.h>
#else
#include <iconv.h>
#endif

FILE* fopen_utf8(const char* filename, const char* mode)
{
#if defined(_WIN32)
	const int MAX_WPATH = 2048;
	const int MAX_WMODE = 16;
	wchar_t wfilename[MAX_WPATH];
	wchar_t wmode[MAX_WMODE];

	utf8_file(filename,wfilename,MAX_WPATH);
	utf8_file(mode,wmode,MAX_WMODE);
	return _wfopen(wfilename,wmode);
#else
	return fopen(filename, mode); // assume non-Windows OS has UTF-8 path names
#endif
}

#if !defined(_WIN32)
int mbs_to_utf8(char* outbuf, size_t outsize, const char* incharset, const char* inbuf, size_t insize)
{
	char *inptr = (char *) inbuf;
	char *outptr = outbuf;
	iconv_t cd = iconv_open("UTF-8", incharset);

	if (cd == (iconv_t) -1)
	{
		outbuf[0] = '\0';
		return -1;
	}

	while (insize > 0)
	{
		if (iconv(cd, &inptr, &insize, &outptr, &outsize) == (size_t) -1)
			break;
	}
	if (outsize >= 1)
		*outptr = '\0';

	iconv_close(cd);
	return outptr - outbuf;
}
#endif
