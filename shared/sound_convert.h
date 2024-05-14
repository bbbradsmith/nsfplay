#pragma once
// shared/sound_convert.h
//   common sound render conversions
//   NSFPlay internal 28-bit stereo to various outputs
//   inlined to facilitate compiler optimizations, especially witha  constant number of samples

#include <cstdint>

inline void sound_convert_sint32s(const uint32_t samples, const int32_t* input, int32_t* output);
inline void sound_convert_sint32m(const uint32_t samples, const int32_t* input, int32_t* output);
inline void sound_convert_sint24s(const uint32_t samples, const int32_t* input, uint8_t* output);
inline void sound_convert_sint24m(const uint32_t samples, const int32_t* input, uint8_t* output);
inline void sound_convert_sint16s(const uint32_t samples, const int32_t* input, int16_t* output);
inline void sound_convert_sint16m(const uint32_t samples, const int32_t* input, int16_t* output);
inline void sound_convert_uint8s( const uint32_t samples, const int32_t* input, uint8_t* output);
inline void sound_convert_uint8m( const uint32_t samples, const int32_t* input, uint8_t* output);

//
// Inline Implementation
//

inline int32_t sound_convert_clamp(const int32_t s)
{
	static const int32_t SCMAX =  ((1<<27)-1);
	static const int32_t SCMIN = -((1<<27)-1);
	if      (s > SCMAX) return SCMAX;
	else if (s < SCMIN) return SCMIN;
	return s;
}

// single sample clamp and convert

inline int32_t sound_convert_clamp32(const int32_t s)
{
	return sound_convert_clamp(s) << 4;
}

inline int32_t sound_convert_clamp24(const int32_t s)
{
	return sound_convert_clamp(s) >> 4;
}

inline int16_t sound_convert_clamp16(const int32_t s)
{
	return int16_t(sound_convert_clamp(s) >> 12);
}

inline uint8_t sound_convert_clamp8(const int32_t s)
{
	return uint8_t((sound_convert_clamp(s) >> 20) + (1<<7));
}

// stereo to mono single sample clamp and convert

inline int32_t sound_convert_clamp32m(const int32_t s0, const int32_t s1)
{
	return sound_convert_clamp32((s0>>1)+(s1>>1)); // shifting first avoids reducing overhead
}

inline int32_t sound_convert_clamp24m(const int32_t s0, const int32_t s1)
{
	return sound_convert_clamp24((s0>>1)+(s1>>1));
}

inline int16_t sound_convert_clamp16m(const int32_t s0, const int32_t s1)
{
	return sound_convert_clamp16((s0>>1)+(s1>>1));
}

inline uint8_t sound_convert_clamp8m(const int32_t s0, const int32_t s1)
{
	return sound_convert_clamp8((s0>>1)+(s1>>1));
}

// batch conversion

inline void sound_convert_sint32s(const uint32_t samples, const int32_t* input, int32_t* output)
{
	for (uint32_t i=0; i<(samples*2); ++i)
		output[i] = sound_convert_clamp32(input[i]);
}

inline void sound_convert_sint32m(const uint32_t samples, const int32_t* input, int32_t* output)
{
	for (uint32_t i=0; i<samples; ++i)
		output[i] = sound_convert_clamp32m(input[(i*2)+0],input[(i*2)+1]);
}

inline void sound_convert_sint24s(const uint32_t samples, const int32_t* input, uint8_t* output)
{
	for (uint32_t i=0,j=0; i<(samples*3); i+=3,j+=1)
	{
		const int32_t s = sound_convert_clamp24(input[j]);
		output[i+0] = uint8_t((s >>  0) & 0xFF);
		output[i+1] = uint8_t((s >>  8) & 0xFF);
		output[i+2] = uint8_t((s >> 16) & 0xFF);
	}
}

inline void sound_convert_sint24m(const uint32_t samples, const int32_t* input, uint8_t* output)
{
	for (uint32_t i=0,j=0; i<(samples*3); i+=3,j+=2)
	{
		const int32_t s = sound_convert_clamp24m(input[j+0],input[j+1]);
		output[i+0] = uint8_t((s >>  0) & 0xFF);
		output[i+1] = uint8_t((s >>  8) & 0xFF);
		output[i+2] = uint8_t((s >> 16) & 0xFF);
	}
}

inline void sound_convert_sint16s(const uint32_t samples, const int32_t* input, int16_t* output)
{
	for (uint32_t i=0; i<(samples*2); ++i)
		output[i] = sound_convert_clamp16(input[i]);
}

inline void sound_convert_sint16m(const uint32_t samples, const int32_t* input, int16_t* output)
{
	for (uint32_t i=0; i<samples; ++i)
		output[i] = sound_convert_clamp16m(input[(i*2)+0],input[(i*2)+1]);
}

inline void sound_convert_uint8s(const uint32_t samples, const int32_t* input, uint8_t* output)
{
	for (uint32_t i=0; i<(samples*2); ++i)
		output[i] = sound_convert_clamp8(input[i]);
}

inline void sound_convert_uint8m(const uint32_t samples, const int32_t* input, uint8_t* output)
{
	for (uint32_t i=0; i<samples; ++i)
		output[i] = sound_convert_clamp8m(input[(i*2)+0],input[(i*2)+1]);
}
