// sound.cpp
//   audio interface abstraction

#include <nsfplaycore.h>
#include "sound.h"

#include <cstdlib> // std::malloc, std::free
#include <cstddef> // NULL
#include <cstdio> // std::snprintf

#ifndef NSF_NOSOUND
	// define NSF_NOSOUND=1 to remove audio output, allow WAV output only.
	#define NSF_NOSOUND 0
#endif

extern void out_printf(const char* fmt, ...); // print to stdout
extern void err_printf(const char* fmt, ...); // print to stderr

#if !NSF_NOSOUND

#include <portaudio.h>

#include <cmath> // TODO test

static bool pa_initialized = false;
static PaError pa_last_error = 0;
static PaStream* pa_stream = NULL;
static int pa_device = -1;
static SoundStreamInfo pa_stream_info = { 0 };

void* pa_buffer = NULL;
size_t pa_buffer_size = 0; // total size in bytes
size_t pa_buffer_play_pos = 0; // next play position for callback
size_t pa_buffer_send_pos = 0; // next send position for write
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
	// TODO test
	(void)input;
	(void)time_info;
	(void)status_flags;
	(void)user_data;
	int16_t* data = reinterpret_cast<int16_t*>(output);
	for (unsigned int i=0; i<frame_count; ++i)
	{
		static int phase_l = 0;
		static int phase_r = 0;
		const double PERIOD_L = int(48000.0 / 220.0);
		const double PERIOD_R = int((3.0 * 48000.0) / (4.0 * 220.0));
		data[0] = int16_t(5000.0 * std::sin(double(phase_l) * (2.0 * 3.14158 / PERIOD_L)));
		data[1] = int16_t(5000.0 * std::sin(double(phase_r) * (2.0 * 3.14158 / PERIOD_R)));
		++phase_l;
		++phase_r;
		data += 2;
	}
	// TODO this callback should just memcpy an available ready buffer into the target
	// TODO if nothing is ready just copy zeroes
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

bool sound_setup(const NSFCore* core)
{
	pa_device = -1;
	pa_stream_info = {0};

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

	if (pa_stream)
	{
		pa_last_error = Pa_StopStream(pa_stream);
		pa_result_check("Pa_StopStream");
		pa_last_error = Pa_CloseStream(pa_stream);
		pa_result_check("Pa_CloseStream");
		pa_stream = NULL;
	}

	// reset buffers
	pa_buffer_play_pos = 0;
	pa_buffer_send_pos = 0;

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
	case NSF_LK_SAMPLERATES_SR_48000:  samplerate = 48000;  break;
	case NSF_LK_SAMPLERATES_SR_96000:  samplerate = 96000;  break;
	case NSF_LK_SAMPLERATES_SR_192000: samplerate = 192000; break;
	default: break;
	}
	switch (nsfplay_get_int(core,NSF_SET_BITS))
	{
	case NSF_LK_BITS_BIT_8:  bits = 8; break;
	case NSF_LK_BITS_BIT_16: bits = 16; break;
	case NSF_LK_BITS_BIT_32: bits = 32; break;
	default: break;
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
	case 16: pa_params.sampleFormat = paInt16; break;
	case 24: pa_params.sampleFormat = paInt24; break;
	case 32: pa_params.sampleFormat = paInt32; break;
	}
	int latency = 64; // desired latency in samples
	for (; sound_latency > 0; --sound_latency) latency <<= 1;
	pa_params.suggestedLatency = double(latency) / double(samplerate);
	pa_params.hostApiSpecificStreamInfo = NULL;

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
	const PaStreamInfo* pa_info = Pa_GetStreamInfo(pa_stream);
	if (pa_info)
	{
		pa_stream_info.samplerate = int(pa_info->sampleRate);
		pa_stream_info.latency = int(pa_info->outputLatency * pa_info->sampleRate);
	}

	// TODO allocate pa_buffer (if too small)
	// is there a convenient pa error for out of memory?

	return true;
}

void sound_shutdown()
{
	if (pa_stream)
	{
		pa_last_error = Pa_StopStream(pa_stream);
		pa_result_check("Pa_StopStream");
		pa_last_error = Pa_CloseStream(pa_stream);
		pa_result_check("Pa_CloseStream");
		pa_stream = NULL;
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

bool sound_setup(const NSFCore* core)
{
	return false;
}

void sound_shutdown()
{
}

#endif
