#pragma once

// NSFPlay stores all filenames internally as char.
// Windows uses the native wchar filenames, and convert them to/from utf8 before storing/using.
// Other platforms that use utf8 directly might bypass these conversions.

#include <cstdio>

// open a file given utf8 parameters
extern FILE* fopen_utf8(const char* filename, const char* mode);

// convert a wchar_t filename to utf8, returns length
// use utf8=NULL, utf8_len=0 to query length without writing the conversion to utf8
#define file_utf8(wfile,utf8,utf8_len) WideCharToMultiByte(CP_UTF8,0,(wfile),-1,(utf8),(utf8_len),NULL,NULL)

// convert a utf8 filename to wchar_t, returns length
// use wfile=NULL, wfile_len=0 to query length without writing the conversion to wfile
#define utf8_file(utf8,wfile,wfile_len) MultiByteToWideChar(CP_UTF8,0,(utf8),-1,(wfile),(wfile_len))

#ifndef _WIN32
// convert multibyte charset string to utf-8 string using iconv
// returns number converted characters or -1 on error
extern int mbs_to_utf8(char* outbuf, size_t outsize, const char* incharset, const char* inbuf, size_t insize);
#endif
