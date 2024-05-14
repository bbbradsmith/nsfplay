#pragma once
// shared/wav_file.h
//   WAV file output

#include <cstdint>
#include <cstdio> // FILE

// 1. Open FILE.
// 2. Write header:
bool wave_file_begin(FILE* f, uint32_t samplerate, uint16_t channels, uint16_t bits);
// 3. Write sample data to FILE.
// 4. Finish FILE by updating header with data sizes:
bool wave_file_finish(FILE* f);
// 5. Close FILE.

// This creates a minimal RIFF WAVE (.WAV) file with no extra data fields.
// The total file size must be less than 2GB or the header field sizes will be incorrect.
