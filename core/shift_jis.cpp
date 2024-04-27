// shift_jis.cpp
//   Detection and conversion of Shift-JIS (CP932) to UTF-8

#include "core.h"

#if !NSF_NOTEXT

bool valid_utf8(const uint8* s) // true if s is valid UTF-8
{
	unsigned int multibyte = 0;
	while (*s)
	{
		uint8 c = *s;
		++s;
		if (multibyte)
		{
			if ((c & 0xC0) != 0x80) return false; // multibyte continuations always have 10xxxxxx
			--multibyte;
		}
		else
		{
			if ((c & 0x80) != 0x00) // high bit marks start of multibyte
			{
				if      ((c & 0xE0) == 0xC0) multibyte = 1; // 110xxxxx
				else if ((c & 0xF0) == 0xE0) multibyte = 2; // 1110xxxx
				else if ((c & 0xF8) == 0xF0) multibyte = 3; // 11110xxx
				else return false;
			}
		}
	}
	return true;
}

#include "shift_jis_table.h"
//   CP932_00_00 - Single byte mappings  00-FF
//   CP932_81_9F - Double byte group 8100-9FFF
//   CP932_E0_EE - Double byte group E000-EEFF
//   CP932_FA_FC - Double byte group FA00-FCFF

void sjis_to_utf8(const uint8* sjis, uint16 unmapped, uint8* output, uint32 output_size)
{
	uint32 db = 0;
	uint32 pos = 0;
	while (*sjis && pos < (output_size-1))
	{
		uint16 unicode = 0;
		uint8 c = *sjis;
		++sjis;
		if (db == 0) // first byte
		{
			if ((c >= 0x81 && c <= 0x9F) ||
				(c >= 0xE0 && c <= 0xFC))
			{
				db = c << 8; // begin double byte
			}
			else // single byte
			{
				unicode = CP932_00_00[c];
			}
		}
		else // second byte of double-byte
		{
			db |= c;
			unicode = 0xFFFF; // unmapped
			if      (db >= 0x8100 && db <= 0x9FFF) unicode = CP932_81_9F[db-0x8100];
			else if (db >= 0xE000 && db <= 0xEEFF) unicode = CP932_E0_EE[db-0xE000];
			else if (db >= 0xFA00 && db <= 0xFCFF) unicode = CP932_FA_FC[db-0xFA00];
			db = 0;
		}
		// emit unicode
		if (unicode)
		{
			if (unicode == 0xFFFF) unicode = unmapped;
			if (unicode < 0x80)
			{
				output[pos] = uint8(unicode); ++pos;
			}
			else if (unicode < 0x800)
			{
				if ((pos+1) >= (output_size-1)) break;
				output[pos+0] = 0xC0 | ((unicode >>  6) & 0x1F);
				output[pos+1] = 0x80 | ((unicode >>  0) & 0x3F);
				pos += 2;
			}
			else //if (unicode < 0x10000)
			{
				if ((pos+2) >= (output_size-1)) break;
				output[pos+0] = 0xE0 | ((unicode >> 12) & 0x0F);
				output[pos+1] = 0x80 | ((unicode >>  6) & 0x3F);
				output[pos+2] = 0x80 | ((unicode >>  0) & 0x3F);
				pos += 3;
			}
			// there are no 4-byte mappings in this set
		}
	}
	output[pos] = 0;
}

#else // NSF_NOTEXT

// NOTEXT won't convert, just assume UTF-8
bool valid_utf8(const uint8* s)
{
	(void)s;
	return true;
}

// NOTEXT don't convert, just copy
void sjis_to_utf8(const uint8* sjis, uint16 unmapped, uint8* output, uint32 output_size)
{
	(void)unmapped;
	for (uint32 i=0; i<(output_size-1); ++i)
	{
		output[i] = *sjis;
		if (*sjis == 0) break;
		++sjis;
	}
	output[output_size-1] = 0;
}

#endif
