// sound.cpp
//   audio interface abstraction

#include <nsfplaycore.h>
#include "sound.h"

#include <cstddef> // NULL

#ifndef NSF_NOSOUND
	// define NSF_NOSOUND=1 to remove audio output, allow WAV output only.
	#define NSF_NOSOUND 0
#endif

extern void out_printf(const char* fmt, ...); // print to stdout
extern void err_printf(const char* fmt, ...); // print to stderr

#if !NSF_NOSOUND

#include <cstdlib> // std::malloc, std::free
#include <cstdio> // std::snprintf
#include <cstring> // std::memcpy, std::memset
#include <mutex>
#include <portaudio.h>

static bool pa_initialized = false;
static PaError pa_last_error = 0;
static PaStream* pa_stream = NULL;
static int pa_device = -1;
static SoundStreamInfo pa_stream_info = { 0 };
static size_t pa_latency = 0;
static size_t pa_sample_size = 0;
static double pa_samplerate = 0;

static std::mutex pa_mutex;
static void* pa_buffer = NULL;
static size_t pa_buffer_size = 0; // total size in bytes
static size_t pa_buffer_allocated = 0; // may be larger than pa_buffer_size due to alignment
static size_t pa_buffer_play_pos = 0; // start of play position for next callback
static size_t pa_buffer_play_end = 0; // end of play buffer for next callback (usually same as play_pos, except when the callback is busy copying)
static size_t pa_buffer_send_pos = 0; // next send position for write
static size_t pa_buffer_send_get_size = 0; // bytes from sound_buffer_get to send at next sound_buffer_send
static bool pa_pause = false;
static uint32_t pa_mark_index = 0;
static double pa_mark_time = 0.0;
static bool pa_mark_active = false;
static uint32_t pa_mark_active_index = 0;
static size_t pa_mark_active_pos = 0;
static uint8_t pa_last_sample[8] = { 0 };

// This has to be longer than PortAudio's possible callback buffer size,
// but the size of that is unspecified. We also shouldn't make it huge,
// to prevent CPU spikes from overgeneration.
#define PA_BUFFER_MS   2000

//
// string utilities (because we've never solved a few cross-platform string problems...)
//

static void stringcpy(char* dst, size_t size, const char* src)
{
	while (*src && size>1) { *dst = *src; ++dst; ++src; }
	*dst = 0;
}

static void stringcat(char* dst, size_t size, const char* src)
{
	while (*dst && size) { ++dst; --size; }
	if (size) stringcpy(dst, size, src);
}

static void stringcatint(char* dst, size_t size, int i)
{
	char num[16];
	std::snprintf(num,16,"%d",i);
	stringcat(dst,size,num);
}

static bool stringicmp(const char* a, const char* b)
{
	while (*a && *b)
	{
		char ca = *a; ++a;
		char cb = *b; ++b;
		if (ca >= 'a' || ca <= 'z') ca = (ca - 'a') + 'A';
		if (cb >= 'a' || cb <= 'z') ca = (cb - 'a') + 'A';
		if (ca != cb) return true;
	}
	return *a != *b;
}

//
// static sound utilities
//

static int pa_callback(
	const void* input, void* output, unsigned long frame_count,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flags,
	void* user_data )
{
	(void)input;
	(void)status_flags;
	(void)user_data;

	size_t frame_bytes = size_t(frame_count) * pa_sample_size;
	const uint8_t* buffer_data = reinterpret_cast<uint8_t*>(pa_buffer);
	uint8_t* output_data = reinterpret_cast<uint8_t*>(output);
	bool mark_active_triggered = false;

	// lock send buffer while we adjust pa_buffer_play_end, and check for active mark
	pa_mutex.lock();

	// calculate sent bytes available in the ring buffer to play
	size_t segment0_size = pa_buffer_size - pa_buffer_play_pos;
	size_t available =
		(pa_buffer_send_pos >= pa_buffer_play_pos) ?
		(pa_buffer_send_pos - pa_buffer_play_pos) :
		(segment0_size + pa_buffer_send_pos);

	if (pa_pause) available = 0; // pause sound
	else if (available > frame_bytes) available = frame_bytes;

	size_t play_pos = pa_buffer_play_pos;
	pa_buffer_play_end = pa_buffer_play_pos + available;
	if (pa_buffer_play_end >= pa_buffer_size)
		pa_buffer_play_end -= pa_buffer_size;

	bool mark_active = pa_mark_active; // check if mark has an active update
	size_t mark_active_pos = pa_mark_active_pos;

	// with play_pos/play_end marking the buffer we're using, we're ready to copy
	pa_mutex.unlock();

	// copy first segment
	{
		size_t segment0 = (frame_bytes < segment0_size) ? frame_bytes : segment0_size;
		if (segment0 > available) segment0 = available;
		if (mark_active && play_pos <= mark_active_pos && (play_pos + segment0) > mark_active_pos)
			mark_active_triggered = true;
		std::memcpy(output_data, buffer_data + play_pos, segment0);
		if (segment0 >= pa_sample_size) std::memcpy(pa_last_sample, output_data + (segment0 - pa_sample_size), pa_sample_size);
		play_pos += segment0;
		if (play_pos >= pa_buffer_size) play_pos = 0;
		frame_bytes -= segment0;
		available -= segment0;
		output_data += segment0;
	}

	// copy second segment if we wrapped
	if (frame_bytes > 0)
	{
		size_t segment1 = (frame_bytes < available) ? frame_bytes : available;
		if (mark_active && play_pos <= mark_active_pos && (play_pos + segment1) > mark_active_pos)
			mark_active_triggered = true;
		std::memcpy(output_data, buffer_data + play_pos, segment1);
		if (segment1 >= pa_sample_size) std::memcpy(pa_last_sample, output_data + (segment1 - pa_sample_size), pa_sample_size);
		play_pos += segment1;
		if (play_pos >= pa_buffer_size) play_pos = 1;
		frame_bytes -= segment1;
		available -= segment1;
		output_data += segment1;
	}

	// lock send buffer while we adjust pa_buffer_play_pos, and mark if needed
	pa_mutex.lock();

	pa_buffer_play_pos = pa_buffer_play_end;

	if (mark_active_triggered)
	{
		pa_mark_active = false;
		pa_mark_index = pa_mark_active_index;
		pa_mark_time = time_info->outputBufferDacTime;
	}

	if (frame_bytes > 0) // if skipping bytes, adjust mark time to account for it
	{
		double skip_samples = double(frame_bytes / pa_sample_size);
		pa_mark_time += (skip_samples / pa_samplerate);
	}

	// finished with play/send buffer and mark
	pa_mutex.unlock();

	// if we didn't have enough data we need to fill the rest (don't need mutex for this)
	if (frame_bytes > 0)
	{
		switch (pa_sample_size)
		{
		case 1: while (frame_bytes > 0) { *output_data = pa_last_sample[0]; ++output_data; --frame_bytes; } break;
		case 2: while (frame_bytes >= 2) { std::memcpy(output_data, pa_last_sample, 2); output_data += 2; frame_bytes -= 2; } break;
		case 3: while (frame_bytes >= 3) { std::memcpy(output_data, pa_last_sample, 3); output_data += 3; frame_bytes -= 3; } break;
		case 4: while (frame_bytes >= 4) { std::memcpy(output_data, pa_last_sample, 4); output_data += 4; frame_bytes -= 4; } break;
		case 6: while (frame_bytes >= 6) { std::memcpy(output_data, pa_last_sample, 6); output_data += 6; frame_bytes -= 6; } break;
		case 8: while (frame_bytes >= 8) { std::memcpy(output_data, pa_last_sample, 8); output_data += 8; frame_bytes -= 8; } break;
		default: while(frame_bytes >= pa_sample_size) { std::memcpy(output_data, pa_last_sample, pa_sample_size); output_data += pa_sample_size; frame_bytes -= pa_sample_size; } break;
		}
	}

	// success
	return 0;
}

static void pa_result_check(const char* source, bool debug=true)
{
	if (pa_last_error < 0)
		err_printf("Sound Error: %s (%d, %s)\n",sound_error_text(),pa_last_error,source);
	#ifdef DEBUG
	else if (debug)
		out_printf("Sound Debug: %s (%d, %s)\n",sound_error_text(),pa_last_error,source);
	#endif
	(void)debug;
}

//
// public sound interface
//

const char* sound_error_text()
{
	if (!pa_initialized) return "Sound not initialized.";
	return Pa_GetErrorText(pa_last_error);
}

const char* sound_debug_text()
{
	const char* interface_info = "Sound not initialized.";
	if (pa_initialized) interface_info = Pa_GetVersionText();
	const int MSG_LEN = 2048;
	static char msg[MSG_LEN];
	SoundDeviceInfo dinfo = sound_device_info(pa_device);
	SoundStreamInfo sinfo = sound_stream_info();
	stringcpy(msg,MSG_LEN,"Sound Interface: ");
	stringcat(msg,MSG_LEN,interface_info);
	stringcat(msg,MSG_LEN,"\nDevice: ");
	stringcat(msg,MSG_LEN,dinfo.name);
	stringcat(msg,MSG_LEN," (");
	stringcatint(msg,MSG_LEN,pa_device);
	stringcat(msg,MSG_LEN,")\nOutput: ");
	stringcatint(msg,MSG_LEN,sinfo.samplerate);
	stringcat(msg,MSG_LEN," Hz, ");
	stringcatint(msg,MSG_LEN,sinfo.bits);
	stringcat(msg,MSG_LEN," bit, ");
	stringcatint(msg,MSG_LEN,sinfo.channels);
	stringcat(msg,MSG_LEN," channels\nLatency: ");
	stringcatint(msg,MSG_LEN,sinfo.latency);
	stringcat(msg,MSG_LEN," samples\n");
	return msg;
}

int sound_device_count()
{
	int count = pa_last_error = Pa_GetDeviceCount();
	if (pa_last_error < 0) count = 0;
	else pa_last_error = 0; // no error
	pa_result_check("Pa_GetDeviceCount",false);
	return count;
}

SoundDeviceInfo sound_device_info(int device)
{
	SoundDeviceInfo info = { "<none>", 0, 0 };
	if (!pa_initialized) return info;
	const PaDeviceInfo* pa_info = Pa_GetDeviceInfo(device);
	if (!pa_info) return info;
	const PaHostApiInfo* pa_host = Pa_GetHostApiInfo(pa_info->hostApi);

	// create combined name
	{
		info.name[0] = 0;
		if (pa_host && pa_host->name)
		{
			stringcat(info.name,SoundDeviceInfo::NAME_LEN,pa_host->name);
			stringcat(info.name,SoundDeviceInfo::NAME_LEN,":");
		}
		stringcat(info.name,SoundDeviceInfo::NAME_LEN,pa_info->name ? pa_info->name : "<unknown>");
		// remove characters that would cause trouble for the INI settings
		int i;
		for (i=0; info.name[i]; ++i)
		{
			char c = info.name[i];
			if (c == 10 || c == 13 || c == '\t') info.name[i] = ' ';
		}
		// remove trailing spaces
		for (;i > 1 && info.name[i-1] == ' ';--i) info.name[i-1] = 0;
		// remove leading spaces
		for (i=0; info.name[i] == ' '; ++i) {}
		if (i > 0) stringcpy(info.name,SoundDeviceInfo::NAME_LEN,info.name+i);
	}

	// get other info
	info.max_channels = pa_info->maxOutputChannels;
	info.samplerate = int(pa_info->defaultSampleRate);

	// debug info, but only if it's an output device
	#ifdef DEBUG
	if (pa_info->maxOutputChannels > 0)
	{
		out_printf("Pa_GetDeviceInfo(%d)\n",device);
		out_printf("\tname: [%s]\n",info.name);
		out_printf("\tmaxInputChannels: %2d  maxOutputChannels: %2d\n",pa_info->maxInputChannels,pa_info->maxOutputChannels);
		out_printf("\tdefaultLowInputLatency:  %8f (%6d)  defaultLowOutputLatency:  %8f (%6d)\n",
			pa_info->defaultLowInputLatency,   int(pa_info->defaultLowInputLatency   * pa_info->defaultSampleRate),
			pa_info->defaultLowOutputLatency,  int(pa_info->defaultLowOutputLatency  * pa_info->defaultSampleRate));
		out_printf("\tdefaultHighInputLatency: %8f (%6d)  defaultHighOutputLatency: %8f (%6d)\n",
			pa_info->defaultHighInputLatency,  int(pa_info->defaultHighInputLatency  * pa_info->defaultSampleRate),
			pa_info->defaultHighOutputLatency, int(pa_info->defaultHighOutputLatency * pa_info->defaultSampleRate));
		out_printf("\tdefaultSampleRate: %f\n",pa_info->defaultSampleRate);
	}
	#endif

	return info;
}

SoundStreamInfo sound_stream_info()
{
	return pa_stream_info;
}

uint32_t sound_buffer_get(void** buffer)
{
	// callback thread will only ever increase play_pos
	// so reading it here we can guarantee that the available
	// ring buffer space between send_pos and play_pos will not shrink
	pa_mutex.lock();
	size_t play_end = pa_buffer_play_end;
	size_t send_pos = pa_buffer_send_pos;
	pa_mutex.unlock();

	// calculate empty bytes available in the ring buffer,
	// split at end of buffer
	size_t empty =
		(play_end >= send_pos) ?
		(play_end - send_pos) :
		(pa_buffer_size - send_pos);

	// return a packet size and pointer to the buffer to use
	size_t max_packet = pa_latency * pa_sample_size;
	if (empty > max_packet) empty = max_packet;
	pa_buffer_send_get_size = empty;
	if (buffer)
		*buffer = reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(pa_buffer)+send_pos);
	return uint32_t(pa_buffer_send_get_size / pa_sample_size);
}

void sound_buffer_send()
{
	// update the send_pos to indicate the buffer is ready to play
	pa_mutex.lock();
	pa_buffer_send_pos += pa_buffer_send_get_size;
	if (pa_buffer_send_pos >= pa_buffer_size) pa_buffer_send_pos = 0;
	pa_mutex.unlock();

	pa_buffer_send_get_size = 0;
}

void sound_buffer_flush()
{
	size_t keep = pa_latency * pa_sample_size;

	// flush unsent data but keep up to pa_latency samples if available
	pa_mutex.lock();

	// calculate sent bytes available in the ring buffer to play
	size_t segment0_size = pa_buffer_size - pa_buffer_play_end;
	size_t available =
		(pa_buffer_send_pos >= pa_buffer_play_end) ?
		(pa_buffer_send_pos - pa_buffer_play_end) :
		(segment0_size + pa_buffer_send_pos);

	if (available > keep) // only discard samples if we have too many
	{
		pa_buffer_send_pos = pa_buffer_play_end + keep;
		if (pa_buffer_send_pos >= pa_buffer_size)
			pa_buffer_send_pos -= pa_buffer_size;
	}

	// safe to allow
	pa_mutex.unlock();

	// this was invalidated, sound_buffer_get again before writing any new samples
	pa_buffer_send_get_size = 0;
}

void sound_mark_time(uint32_t mark_index)
{
	pa_mutex.lock();
	pa_mark_active = true;
	pa_mark_active_index = mark_index;
	pa_mark_active_pos = pa_buffer_send_pos;
	pa_mutex.unlock();
}

int64_t sound_play_time(uint32_t* mark_index)
{
	if (!pa_initialized || !pa_stream) return 0;

	pa_mutex.lock();
	double mark_time = pa_mark_time;
	uint32_t mark_index_ = pa_mark_index;
	pa_mutex.unlock();

	double pa_time = Pa_GetStreamTime(pa_stream);
	if (mark_index) *mark_index = mark_index_;
	return int64_t((pa_samplerate*(pa_time-mark_time))+0.5);
}

bool sound_setup()
{
	if (!pa_initialized)
	{
		pa_last_error = Pa_Initialize();
		pa_result_check("Pa_Initialize");
		if (pa_last_error != paNoError) return false;
		pa_initialized = true;
	}

	out_printf("Sound Library Version: %s\n",Pa_GetVersionText());
	#ifdef DEBUG
		for (int i=0; i<sound_device_count(); ++i) sound_device_info(i);
	#endif

	return true;
}

bool sound_start(const NSFCore* core)
{
	pa_device = -1;
	pa_stream_info = {0};
	pa_sample_size = 0;
	pa_samplerate = 0.0;
	pa_latency = 0;
	pa_pause = false;
	pa_mark_index = 0;
	pa_mark_time = 0.0;
	pa_mark_active = false;
	pa_mark_active_index = 0;
	pa_mark_active_pos = 0;

	// make sure initialized and stopped
	if (!pa_initialized)
	{
		if (!sound_setup()) return false;
	}
	if (pa_stream) sound_stop();

	// reset buffers while stream is stopped
	pa_buffer_play_pos = 0;
	pa_buffer_play_end = 0;
	pa_buffer_send_pos = 0;
	pa_buffer_send_get_size = 0;
	std::memset(pa_last_sample, 0, sizeof(pa_last_sample));

	int samplerate = 48000;
	int bits = 16;
	int channels = 2;
	PaStreamParameters pa_params = {0};

	// MAIN group settings
	const char* device_name = nsfplay_get_str(core, NSF_SET_SOUND_DEVICE);
	int32_t device_int = nsfplay_get_int(core,NSF_SET_SOUND_DEVICE_NUM);
	switch (nsfplay_get_int(core, NSF_SET_SAMPLERATE))
	{
	case NSF_LK_SAMPLERATES_SR_8000:   samplerate = 8000;   break;
	case NSF_LK_SAMPLERATES_SR_11025:  samplerate = 11025;  break;
	case NSF_LK_SAMPLERATES_SR_22050:  samplerate = 22050;  break;
	case NSF_LK_SAMPLERATES_SR_44100:  samplerate = 44100;  break;
	default:
	case NSF_LK_SAMPLERATES_SR_48000:  samplerate = 48000;  break;
	case NSF_LK_SAMPLERATES_SR_96000:  samplerate = 96000;  break;
	case NSF_LK_SAMPLERATES_SR_192000: samplerate = 192000; break;
	}
	switch (nsfplay_get_int(core,NSF_SET_BITS))
	{
	case NSF_LK_BITS_BIT_8:  bits = 8; break;
	default:
	case NSF_LK_BITS_BIT_16: bits = 16; break;
	case NSF_LK_BITS_BIT_24: bits = 24; break;
	case NSF_LK_BITS_BIT_32: bits = 32; break;
	}
	if (!nsfplay_get_int(core,NSF_SET_STEREO)) channels = 1;
	// ADVANCED group settings
	int32_t samplerate_override = nsfplay_get_int(core, NSF_SET_SAMPLERATE_OVERRIDE);
	int32_t sound_latency = nsfplay_get_int(core, NSF_SET_SOUND_LATENCY);
	if (samplerate_override) samplerate = samplerate_override;

	// determine sound device
	if (*device_name)
	{
		// if both name and number match, we can just use device_int (perfect match)
		// otherwise, we'll search for a name match, and fallback to device_int if not found
		pa_device = device_int;
		if (device_int < 0 || stringicmp(device_name,sound_device_info(device_int).name))
		{
			for (int i=0; i<sound_device_count(); ++i)
			{
				if (!stringicmp(device_name,sound_device_info(i).name))
				{
					pa_device = i;
					break;
				}
			}
		}
	}
	else if (device_int >= 0) // no name, use the number
	{
		pa_device = device_int;
	}
	if (pa_device < 0) // if no device requested, use default
	{
		pa_device = Pa_GetDefaultOutputDevice();
	}

	// fill PortAudio parameters
	pa_params.device = pa_device;
	pa_params.channelCount = channels;
	switch (bits)
	{
	case 8:  pa_params.sampleFormat = paUInt8; break;
	default:
	case 16: pa_params.sampleFormat = paInt16; break;
	case 24: pa_params.sampleFormat = paInt24; break;
	case 32: pa_params.sampleFormat = paInt32; break;
	}
	int latency = 64; // desired latency in samples
	for (; sound_latency > 0; --sound_latency) latency <<= 1;
	pa_latency = latency;
	pa_params.suggestedLatency = double(latency) / double(samplerate);
	pa_params.hostApiSpecificStreamInfo = NULL;
	pa_sample_size = (bits * channels) / 8;
	if (bits == 8) pa_last_sample[0] = pa_last_sample[1] = 128;

	pa_last_error = Pa_OpenStream(
		&pa_stream,
		NULL, // no input
		&pa_params, // output parameters
		double(samplerate),
		paFramesPerBufferUnspecified, // callback buffer size may vary
		paNoFlag, // stream flags
		pa_callback,
		NULL ); // userdata
	pa_result_check("Pa_OpenStream");
	if (pa_last_error != paNoError) return false;

	pa_last_error = Pa_StartStream(pa_stream);
	pa_result_check("Pa_StartStream");
	if (pa_last_error != paNoError) return false;

	pa_stream_info.bits = bits;
	pa_stream_info.channels = channels;
	pa_samplerate = double(samplerate);
	const PaStreamInfo* pa_info = Pa_GetStreamInfo(pa_stream);
	if (pa_info)
	{
		pa_samplerate = pa_info->sampleRate;
		samplerate = int(pa_info->sampleRate + 0.5);
		pa_stream_info.samplerate = samplerate;
		pa_stream_info.latency = int(pa_info->outputLatency * pa_info->sampleRate);
	}

	size_t buffer_size = pa_sample_size * size_t((samplerate * PA_BUFFER_MS) / 1000);
	if (buffer_size < (pa_sample_size * 2 * pa_latency)) // need at least 2 buffers at the requested latency
		buffer_size = (pa_sample_size * 2 * pa_latency);
	if (pa_buffer && pa_buffer_allocated < buffer_size)
	{
		std::free(pa_buffer);
		pa_buffer = NULL;
		pa_buffer_size = 0;
		pa_buffer_allocated = 0;
	}
	if (!pa_buffer || pa_buffer_allocated < buffer_size)
	{
		pa_buffer = std::malloc(buffer_size);
		if (pa_buffer == NULL)
		{
			pa_last_error = Pa_StopStream(pa_stream);
			pa_result_check("Pa_StopStream");
			pa_last_error = Pa_CloseStream(pa_stream);
			pa_result_check("Pa_CloseStream");
			pa_stream = NULL;
			pa_last_error = paInsufficientMemory;
			pa_result_check("std::malloc, playback buffer");
			return false;
		}
		pa_buffer_allocated = buffer_size;
	}
	pa_buffer_size = buffer_size; // <= pa_buffer_allocated

	return true;
}

void sound_pause(bool pause)
{
	pa_mutex.lock();
	pa_pause = pause;
	pa_mutex.unlock();
}

void sound_stop()
{
	if (pa_stream)
	{
		pa_last_error = Pa_StopStream(pa_stream);
		pa_result_check("Pa_StopStream");
		pa_last_error = Pa_CloseStream(pa_stream);
		pa_result_check("Pa_CloseStream");
		pa_stream = NULL;
	}
}

void sound_shutdown()
{
	sound_stop();

	if (pa_buffer)
	{
		std::free(pa_buffer);
		pa_buffer = NULL;
		pa_buffer_size = 0;
		pa_buffer_allocated = 0;
	}

	if (pa_initialized)
	{
		pa_last_error = Pa_Terminate();
		pa_result_check("Pa_Terminate");
		pa_initialized = false;
	}
}

#else // NSF_NOSOUND

const char* sound_error_text()
{
	return "No sound library included.";
}

const char* sound_debug_text()
{
	return "No sound library included.\n";
}

int sound_device_count()
{
	return 0;
}

SoundDeviceInfo sound_device_info(int device)
{
	SoundDeviceInfo info = { "<none>", 0, 0 };
	return info;
}

uint32_t sound_buffer_get(void** buffer)
{
	if (buffer) *buffer = NULL;
	return 0;
}

void sound_buffer_send()
{
}

void sound_buffer_flush()
{
}

void sound_mark_time(uint32_t mark_index)
{
	(void)mark_index;
}

int64_t sound_play_time(uint32_t* mark_index)
{
	if (mark_index) *mark_index = 0;
	return 0;
}

bool sound_start()
{
	return false;
}

bool sound_setup(const NSFCore* core)
{
	(void)core;
	return false;
}

void sound_pause(bool pause)
{
	(void)pause;
}

void sound_stop()
{
}

void sound_shutdown()
{
}

#endif
