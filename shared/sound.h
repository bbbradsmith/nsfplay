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

bool sound_setup(const NSFCore* core);
void sound_shutdown();

// TODO
//   uint32_t sound_buffer_get(void** buffer) returns number of samples to fill (ring buffer can be split with this)
//   sound_buffer_send(); // sends the buffer just requested by sound_buffer_get
//   sound_buffer_flush(); // redact unplayed buffers (leaving 1x latency extra)
//   (setup should allocate at least 10 buffers, maybe minimum 1 second worth)
