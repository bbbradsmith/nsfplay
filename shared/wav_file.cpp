// wav_file.cpp
//   WAV file output

#include "wav_file.h"
//#include <cstdio> // FILE, std::fwrite, std::ftell, std::fseek

static inline void headfc(uint8_t* head, int pos, const char* fc) { for (int i=0;i<4;++i) head[pos+i] = fc[i]; }
static inline void head16( uint8_t* head, int pos, uint16_t v)    { for (int i=0;i<2;++i) head[pos+i] = (v>>(8*i))&0xFF; }
static inline void head32( uint8_t* head, int pos, uint32_t v)    { for (int i=0;i<4;++i) head[pos+i] = (v>>(8*i))&0xFF; }

bool wave_file_begin(FILE* f, uint32_t samplerate, uint16_t channels, uint16_t bits)
{
	uint8_t head[0x2C];
	// RIFF master chunk
	headfc(head,0x00,"RIFF");
	head32(head,0x04,0); // (file size - 12)
	headfc(head,0x08,"WAVE");
	// fmt chunk
	headfc(head,0x0C,"fmt ");
	head32(head,0x10,16); // fmt chunk size
	head16(head,0x14,0x01); // PCM data
	head16(head,0x16,channels);
	head32(head,0x18,samplerate);
	head32(head,0x1C,samplerate * channels * (bits/8));
	head16(head,0x20,channels * (bits/8));
	head16(head,0x22,bits);
	// data chunk
	headfc(head,0x24,"data");
	head32(head,0x28,0); // data chunk size
	return 0x2C == std::fwrite(head,1,0x2C,f);
}

bool wave_file_finish(FILE* f)
{
	uint8_t head[4];
	bool result = true;
	long fsize = std::ftell(f);
	if (fsize & 1) { result &= (0 == std::fputc(0,f)); ++fsize; } // padding byte needed for odd data chunk size
	// RIFF chunk size
	result &= (0 == std::fseek(f,0x04,SEEK_SET));
	head32(head,0,fsize-12);
	result &= (4 == std::fwrite(head,1,4,f));
	// data chunk size
	result &= (0 == std::fseek(f,0x28,SEEK_SET));
	head32(head,0,fsize-(0x2C));
	result &= (4 == std::fwrite(head,1,4,f));
	// return to end of file
	std::fseek(f,0,SEEK_END);
	return result;
}
