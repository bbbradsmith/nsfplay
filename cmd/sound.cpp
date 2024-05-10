#include <nsfplaycore.h>
#include <cstdio> // std::printf
#include <cstddef> // NULL

#ifndef NSF_NOSOUND
	// define NSF_NOSOUND=1 to remove audio output, allow WAV output only.
	#define NSF_NOSOUND 0
#endif

#if !NSF_NOSOUND

#include <portaudio.h>

#include <cmath> // TODO test

static bool pa_initialized = false;
static PaError pa_last_error = 0;
static PaStream* pa_stream = NULL;

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
	return 0;
}

const char* sound_error_text()
{
	return Pa_GetErrorText(pa_last_error);
}

static inline void sound_debug(const char* prefix)
{
	(void)prefix;
	#ifdef DEBUG
		std::printf("Sound: %s %d (%s)\n",prefix,pa_last_error,sound_error_text());
	#endif
}

// TODO sound_time - PaTime Pa_GetStreamTime
// TODO sound_ready - buffers ready to fill
// TODO sound_add_buffer - add new buffer
// TODO enumerate devices

bool sound_setup()
{
	if (!pa_initialized)
	{
		pa_last_error = Pa_Initialize();
		sound_debug("Pa_Initialize");
		if (pa_last_error != paNoError) return false;
		pa_initialized = true;
	}

	#ifdef DEBUG
		std::printf("Sound Library Version: %s\n",Pa_GetVersionText());
	#endif

	if (pa_stream)
	{
		pa_last_error = Pa_StopStream(pa_stream);
		sound_debug("Pa_StopStream");
		pa_last_error = Pa_CloseStream(pa_stream);
		sound_debug("Pa_CloseStream");
		pa_stream = NULL;
	}

	// TODO settings/format (override DefaultStream)
	pa_last_error = Pa_OpenDefaultStream(
		&pa_stream,
		0, // no input
		2, // stereo output
		paInt16, // sample format
		48000, // samplerate
		paFramesPerBufferUnspecified, // callback buffer size
		pa_callback, // callback
		NULL ); // userdata
	sound_debug("Pa_OpenDefaultStream");
	if (pa_last_error != paNoError) return false;

	pa_last_error = Pa_StartStream(pa_stream);
	sound_debug("Pa_StartStream");
	if (pa_last_error != paNoError) return false;

	return true;
}

void sound_shutdown()
{
	if (pa_stream)
	{
		pa_last_error = Pa_StopStream(pa_stream);
		sound_debug("Pa_StopStream");
		pa_last_error = Pa_CloseStream(pa_stream);
		sound_debug("Pa_CloseStream");
		pa_stream = NULL;
	}

	if (pa_initialized)
	{
		pa_last_error = Pa_Terminate();
		sound_debug("Pa_Terminate");
		pa_initialized = false;
	}
}

#else // NSF_NOSOUND

const char* sound_error_text()
{
	return "Sound not implemented.";
}

bool sound_setup()
{
	return false;
}

void sound_shutdown()
{
}

#endif
