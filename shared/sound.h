#pragma once
// shared/sound.h
//   common hardware audio interface abstraction
//
// Expects external functions for debug output and error logging:
//   extern void out_printf(const char* fmt, ...);
//   extern void err_printf(const char* fmt, ...);
// (Calls to these will always end with \n)

struct NSFCore_;
typedef struct NSFCore_ NSFCore;

typedef struct
{
	enum { NAME_LEN = 256 };
	char name[NAME_LEN];
	int max_channels;
	int samplerate; // default samplerate
} SoundDeviceInfo;

typedef struct
{
	int samplerate;
	int latency; // approximate latency in samples
	int bits;
	int channels;
} SoundStreamInfo;

const char* sound_error_text(); // describes the last error (no newline)
const char* sound_debug_text(); // describes the current audio stream (multiple lines, ends with \n)

int sound_device_count();
SoundDeviceInfo sound_device_info(int device);
SoundStreamInfo sound_stream_info();

uint32_t sound_buffer_get(void** buffer); // returns a number of samples that can be sent, with a pointer to their buffer (can ignore result if we don't want to fill it yet), 0 if full
void sound_buffer_send(); // sends the buffer last retrieved via sound_buffer_get (invalidates buffer from sound_buffer_get)
void sound_buffer_flush(); // discards unplayed samples down to chosen latency amount (invalidates last sound_buffer_get)

void sound_mark_time(uint32_t mark_index); // the next sent buffer will mark its time when it begins playback
int64_t sound_play_time(uint32_t* mark_index); // get current playback sample, relative to mark time (may return a previous mark if the latest hasn't been called back yet)
void sound_pause(bool pause); // pause playback

bool sound_setup(const NSFCore* core);
void sound_shutdown();
