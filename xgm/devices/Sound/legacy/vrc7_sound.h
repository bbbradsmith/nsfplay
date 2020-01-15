
/*
 _     __   _____     ____   _______
| |   / /  / ___ \   / ___\ /___   /    #####    ###    ##    #  ##    #  #####
| |  / /  / /__/ /  / /       __/ /   ##       ##   #  ##    #  ###   #  ##    #
| | / /  / _   _/  / /       /_  _/   ####    ##   #  ##    #  ## #  #  ##    #
| |/ /  / / | |   / /___      / /        ##  ##   #  ##    #  ##  # #  ##    #
|___/  /_/  |_|   \____/     /_/    #####     ###     ####   ##    #  ######


VRC7 Audio emulator by Delphi1024

Copyright 2019 Jonas Rinke

===REFERENCES===
VRC7 die shot by digshadow:
	https://siliconpr0n.org/archive/doku.php?id=digshadow:konami:vrc_vii_053982

YM2413 (OPLL) Datasheet:
	http://www.smspower.org/maxim/Documents/YM2413ApplicationManual

VRC7 audio on Nesdev Wiki:
	https://wiki.nesdev.com/w/index.php/VRC7_audio

OPLx logSin/exp/ksl tables by Olli Niemitalo and Matthew Gambrell:
	http://yehar.com/blog/?p=665
	https://docs.google.com/document/d/18IGx18NQY_Q1PJVZ-bHywao9bhsDoAqoIn1rIm42nwo/edit

Instrument ROM Dump by NukeYKT:
	https://siliconpr0n.org/archive/doku.php?id=vendor:yamaha:opl2#ym2413_instruments

Lagrange point filter analysis by kevtris:
	http://forums.nesdev.com/viewtopic.php?t=4709&p=41523
*/

// <!>  Uncomment the following line if you want to use vrc7_sound from DLL  <!>
// #define VRC7_SOUND_IMPORTING

// <!>  Uncomment the following line when you want to have the VRC7's TEST register enabled <!>
// #define VRC7_SOUND_TEST_REG

#ifndef VRC7_SOUND_H
#define VRC7_SOUND_H

#include <stdint.h>
#include <stdbool.h>

#ifdef VRC7_SOUND_EXPORTING
#define VRC7SOUND_API __declspec(dllexport)
#elif VRC7_SOUND_IMPORTING
#define VRC7SOUND_API __declspec(dllimport)
#else
#define VRC7SOUND_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define VRC7_NUM_PATCHES 16
#define VRC7_NUM_CHANNELS 6

#define VRC7_SIGNAL_CHUNK_LENGTH 72

#define VRC7_DEFAULT_CLOCK_RATE 3579545.0
#define VRC7_DEFAULT_SAMPLE_RATE 48000.0

#define MODULATOR 0
#define CARRIER 1

#define STEREO_LEFT 0
#define STEREO_RIGHT 1

enum patch_sets {
	VRC7_NUKE_TONE = 0,
	VRC7_RW_TONE,
	VRC7_FT36_TONE,
	VRC7_FT35_TONE,
	VRC7_MO_TONE,
	VRC7_KT2_TONE,
	VRC7_KT1_TONE,
	OPLL_2413_TONE,
	OPLL_281B_TONE
};

struct vrc7_patch {
	uint32_t feedback;
	uint32_t total_level;
	uint32_t mult[2];
	bool vibrato[2];
	bool tremolo[2];
	bool sustained[2];
	bool rect[2];
	bool key_scale_rate[2];
	uint32_t key_scale_level[2];

	uint32_t attack_rate[2];
	uint32_t decay_rate[2];
	uint32_t sustain_level[2];
	uint32_t release_rate[2];
};

struct vrc7_slot {
	uint32_t type;
	int32_t sample;
	int32_t sample_prev;

	uint32_t phase;
	uint32_t phase_inc;

	uint32_t ksl_val;

	uint32_t env_rate_high;
	uint32_t env_rate_low;
	uint32_t env_stage;
	uint32_t env_value;
	bool env_enabled;
	bool restart_env;
};

struct vrc7_channel {
	uint32_t instrument;
	uint32_t fNum;
	uint32_t octave;
	uint32_t volume;
	bool sustain;
	bool trigger;

	struct vrc7_slot *slots[2];
};

/*
This is the main object. You can/have to change some properties directly via this struct. These are:
-- channel_mask:	Bit field that enables or disables some channels of the VRC7. Setting a bit to 1 will disable that channel.
					Bit 0 corresponds to channel 0, bit 1 to channel 1 etc. The default value is 0 (all channels enabled).
-- filter:			Filter function that is applied to the raw output of the VRC7. It can be set to any of the vrc7_filter_* functions below.
					The default is vrc7_filter_lagrange_point_fast.
-- stereo_volume:	Volume control for each channel. This is a 2d-array. The first dimension selects the stereo channel with either STEREO_LEFT or STEREO_RIGHT.
					The second dimension selects the VRC7 channel (from 0 to VRC7_NUM_CHANNELS). The default value is 1.0 (full volume)
					for every side and channel.

-- signal:			The output signal of the VRC7. This is an array of length VRC7_SIGNAL_CHUNK_LENGTH and contains the audio signal sampled at the clock rate.
*/
struct vrc7_sound {
	//Read & Write:
	uint32_t channel_mask;
	void(*filter)(struct vrc7_sound *vrc7_s);
	double stereo_volume[2][VRC7_NUM_CHANNELS];

	//Read only:
	int16_t *signal[2];

	//private:
	struct vrc7_channel *channels[VRC7_NUM_CHANNELS];
	struct vrc7_patch *patches[VRC7_NUM_PATCHES];
	double clock_rate;
	double sample_rate;
	double sample_length;
	double current_time;
	uint32_t vibrato_counter;
	uint32_t tremolo_value;
	int32_t tremolo_inc;
	uint32_t envelope_counter;
	uint32_t zero_count;
	uint32_t mini_counter;
	int patch_set;
	uint32_t address;

	float fir_coeff;
	float iir_coeff;
	float fir_coeff_fast;
	float iir_coeff_fast;

	bool test_envelope;
	bool test_reset_fmam;
	bool test_halt_phase;
	bool test_counters;

};

/*
=============  VRC7 Sound Management  ==============
*/

/*
Creates and resets a new vrc7_sound object.
*/
VRC7SOUND_API struct vrc7_sound *vrc7_new(void);

/*
Deletes a vrc7_sound object.
*/
VRC7SOUND_API void vrc7_delete(struct vrc7_sound *vrc7_s);

/*
Manually resets a vrc7_sound object to it's default values.
*/
VRC7SOUND_API void vrc7_reset(struct vrc7_sound *vrc7_s);

/*
Clears a vrc7_sound object. This function is different from vrc7_reset in that it does not reset everything (e.g. the vibrato phase does not get reset)
but clears the values that would be cleared by disabling the expansion sound (setting bit 6 of $E000 to 1).
*/
VRC7SOUND_API void vrc7_clear(struct vrc7_sound *vrc7_s);

/*
Sets the clock rate of the vrc7. This will also change the sample rate of vrc7_sound->signal. 
The default is 3579545.0 Hz (VRC7_DEFAULT_CLOCK_RATE).
*/
VRC7SOUND_API void vrc7_set_clock_rate(struct vrc7_sound *vrc7_s, double clock_rate);

/*
Sets the sample rate of the vrc7_sound object. This function only has an effect if you are using vrc7_fetch_sample. This is not to be
confused with the sample rate of the signal variable of the vrc7_sound object which is set by vrc7_set_clock_rate.
*/
VRC7SOUND_API void vrc7_set_sample_rate(struct vrc7_sound *vrc7_s, double sample_rate);

/*
Sets the instrument data for the vrc7's build-in patches. This can be any value from the patch_sets enum. The default is VRC7_NUKE_TONE.
*/
VRC7SOUND_API void vrc7_set_patch_set(struct vrc7_sound *vrc7_s, int patch_set);

/*
Updates the vrc7. This function has to be called at 1/72th of the clock rate set by vrc7_set_clock_rate. After calling this function,
the signal variable of the vrc7_sound object will contain new data. If you are using vrc7_fetch_sample, you should not call this function.
*/
VRC7SOUND_API void vrc7_tick(struct vrc7_sound *vrc7_s);

/*
Fetches a single sample and updates the vrc7_sound object. This function internally calls vrc7_tick, so you should not call it manually when using
vrc7_fetch_sample. This function has to be called at the sample rate set by vrc7_set_sample_rate.
*/
VRC7SOUND_API void vrc7_fetch_sample(struct vrc7_sound *vrc7_s, int16_t *sample);

/*
=============  VRC7 Sound IO  ==============
*/

/*
Sets the address that subsequent write operations with vrc7_write_data will write to.
*/
VRC7SOUND_API void vrc7_write_addr(struct vrc7_sound *vrc7_s, uint32_t addr);

/*
Writes a value to the vrc7 register selected by vrc7_write_addr.
*/
VRC7SOUND_API void vrc7_write_data(struct vrc7_sound *vrc7_s, uint32_t data);

/*
=============  VRC7 Patch Utility  ==============
*/

/*
Utility function to convert a vrc7_patch object to a register value.
*/
VRC7SOUND_API void vrc7_patch_to_reg(struct vrc7_patch *patch, uint8_t *reg);

/*
Utility function to convert a vrc7 register value to a vrc7_patch object.
*/
VRC7SOUND_API void vrc7_reg_to_patch(const uint8_t *reg, struct vrc7_patch *patch);

/*
Loads a default patch into the vrc7_patch object.
*/
VRC7SOUND_API void vrc7_get_default_patch(int set, uint32_t index, struct vrc7_patch *patch);

/*
=============  VRC7 Filter Functions  ==============
*/

/*
Filter function that leaves the raw output from the vrc7 in the signal variable. In other words, this function does nothing.
*/
VRC7SOUND_API void vrc7_filter_raw(struct vrc7_sound *vrc7_s);

/*
Filter function that converts the pulse output of the vrc7 into a more usable signal, but does not apply any filters to it.
The output signal is filled with the sum of each of the channels outputs.
*/
VRC7SOUND_API void vrc7_filter_no_filter(struct vrc7_sound *vrc7_s);

/*
Filter function that replicates the low pass filter of the lagrange point cartridge.
*/
VRC7SOUND_API void vrc7_filter_lagrange_point(struct vrc7_sound *vrc7_s);

/*
This function also replicates the lagrange point filter, but first sums up the channel outputs similar to vrc7_filter_no_filter. This speeds
up the computation of the low pass filter significantly.
*/
VRC7SOUND_API void vrc7_filter_lagrange_point_fast(struct vrc7_sound *vrc7_s);

#ifdef __cplusplus
}
#endif

#endif