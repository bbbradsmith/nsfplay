/* demo application for
 * 1. loading an NSF/NSFe or playlist line
 * 2. probing track/time info
 * 3. converting to WAV
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../xgm/xgm.h"

#define SAMPLE_RATE 48000
#define CHANNELS 2

static void pack_int16le(uint8_t *d, int16_t n);
static void pack_uint16le(uint8_t *d, uint16_t n);
static void pack_uint32le(uint8_t *d, uint32_t n);
static void pack_frames(uint8_t *d, int16_t *s, unsigned int frameCount);
static int write_wav_header(FILE *f, uint64_t totalFrames);
static int write_frames(FILE *f, uint8_t *d, unsigned int frameCount);

int main(int argc, char *argv[]) {
    int i;
    int m; /* max track counter */
    uint8_t nsfe_i; /* nsfe playlist track number */
    int16_t *buf; /* audio samples, native machine format */
    uint8_t *pac; /* audio samples, little-endian format */
    int fc; /* current # of frames to decode */
    FILE *f;
    int32_t length;  /* length in ms */
    int32_t fade;    /* fade in ms */
    uint64_t frames; /* total pcm frames */

    xgm::NSF nsf;
    xgm::NSFPlayer player;
    xgm::NSFPlayerConfig config;

    if(argc < 2) {
        fprintf(stderr,"Usage: %s (/path/to/nsf[e] | m3u entry) [tracknum] [out.wav]\n",argv[0]);
        return 1;
    }

    buf = (int16_t *)malloc(sizeof(int16_t) * 4096 * 2);
    pac = (uint8_t *)malloc(sizeof(int16_t) * 4096 * 2);

    if(!nsf.LoadFile(argv[1])) {
        fprintf(stderr,"Error loading NSF: %s\n",nsf.LoadError());
        return 1;
    }

    if(argc < 3) {
        /* dump info */
        /* use playlist order, if available */

        printf("Title: %s\n",nsf.title);
        printf("Artist: %s\n",nsf.artist);
        printf("Copyright: %s\n",nsf.copyright);
        printf("Ripper: %s\n",nsf.ripper);

        if(nsf.playlist_mode) {
            printf("Track %03d: %s\n",nsf.song+1,nsf.GetTitleString());
            return 0;
        }

        if(nsf.nsfe_plst_size > 0) {
            m = nsf.nsfe_plst_size;
        } else {
            m = nsf.total_songs;
        }
        for(i=0;i<m;i++) {
            if(nsf.nsfe_plst) {
                nsfe_i = nsf.nsfe_plst[i];
            } else {
                nsfe_i = i;
            }
            printf("Track %03d: %s\n",i+1,
              nsf.nsfe_entry[nsfe_i].tlbl);
        }
        return 0;
    }


    if(nsf.playlist_mode) {
        i = nsf.song;
    }
    else {
        i = atoi(argv[2]);
        if(i == 0) {
            fprintf(stderr,"error: use 1-based track number\n");
            return 1;
        }
        i--;
    }

    /* dump info for single track */
    if (!nsf.playlist_mode && nsf.nsfe_plst) {
        nsfe_i = nsf.nsfe_plst[i];
    } else {
        nsfe_i = i;
    }
    length = (!nsf.playlist_mode && nsf.nsfe_entry[nsfe_i].time >= 0) ? nsf.nsfe_entry[nsfe_i].time : nsf.time_in_ms < 0 ? nsf.default_playtime : nsf.time_in_ms;
    fade = (!nsf.playlist_mode && nsf.nsfe_entry[nsfe_i].fade >= 0) ? nsf.nsfe_entry[nsfe_i].fade : nsf.fade_in_ms < 0 ? nsf.default_fadetime : nsf.fade_in_ms;

    printf("Track %03d: %s\n",i+1,
      (!nsf.playlist_mode && nsf.nsfe_entry[nsfe_i].tlbl[0] != '\0') ?
      nsf.nsfe_entry[nsfe_i].tlbl :
      nsf.GetTitleString("%L",i));
    printf("  length: %d ms\n", length);
    printf("    fade: %d ms\n", fade);
    if(argc < 4) return 0;

    config["MASTER_VOLUME"] = 256; /* default volume = 128 */

    player.SetConfig(&config);
    if(!player.Load(&nsf)) {
        fprintf(stderr,"Error with player load\n");
        return 1;
    }

    player.SetPlayFreq(SAMPLE_RATE);
    player.SetChannels(CHANNELS);
    player.SetSong(i);
    player.Reset();

    frames  = (uint64_t)length * SAMPLE_RATE;
    frames += (uint64_t)fade * SAMPLE_RATE;
    frames /= 1000;

    f = fopen(argv[3],"wb");
    if(f == NULL) {
        fprintf(stderr,"Error opening %s\n",argv[3]);
        return 1;
    }
    write_wav_header(f,frames);

    while(frames) {
        fc = frames < 4096 ? frames : 4096;
        player.Render(buf,fc);
        pack_frames(pac,buf,fc);
        write_frames(f,pac,fc);
        frames -= fc;
    }

    fclose(f);
    free(buf);
    free(pac);

    return 0;
}

static void pack_int16le(uint8_t *d, int16_t n) {
    d[0] = (uint8_t)(n      );
    d[1] = (uint8_t)(n >> 8 );
}

static void pack_uint16le(uint8_t *d, uint16_t n) {
    d[0] = (uint8_t)(n      );
    d[1] = (uint8_t)(n >> 8 );
}

static void pack_uint32le(uint8_t *d, uint32_t n) {
    d[0] = (uint8_t)(n      );
    d[1] = (uint8_t)(n >> 8 );
    d[2] = (uint8_t)(n >> 16);
    d[3] = (uint8_t)(n >> 24);
}

static int write_wav_header(FILE *f, uint64_t totalFrames) {
    unsigned int dataSize = totalFrames * sizeof(int16_t) * CHANNELS;
    uint8_t tmp[4];
    if(fwrite("RIFF",1,4,f) != 4) return 0;
    pack_uint32le(tmp,dataSize + 44 - 8);
    if(fwrite(tmp,1,4,f) != 4) return 0;

    if(fwrite("WAVE",1,4,f) != 4) return 0;
    if(fwrite("fmt ",1,4,f) != 4) return 0;

    pack_uint32le(tmp,16); /*fmtSize */
    if(fwrite(tmp,1,4,f) != 4) return 0;

    pack_uint16le(tmp,1); /* audioFormat */
    if(fwrite(tmp,1,2,f) != 2) return 0;

    pack_uint16le(tmp,CHANNELS); /* numChannels */
    if(fwrite(tmp,1,2,f) != 2) return 0;

    pack_uint32le(tmp,SAMPLE_RATE);
    if(fwrite(tmp,1,4,f) != 4) return 0;

    pack_uint32le(tmp,SAMPLE_RATE * CHANNELS * sizeof(int16_t));
    if(fwrite(tmp,1,4,f) != 4) return 0;

    pack_uint16le(tmp,CHANNELS * sizeof(int16_t));
    if(fwrite(tmp,1,2,f) != 2) return 0;

    pack_uint16le(tmp,sizeof(int16_t) * 8);
    if(fwrite(tmp,1,2,f) != 2) return 0;

    if(fwrite("data",1,4,f) != 4) return 0;

    pack_uint32le(tmp,dataSize);
    if(fwrite(tmp,1,4,f) != 4) return 0;

    return 1;
}

static void pack_frames(uint8_t *d, int16_t *s, unsigned int frameCount) {
    unsigned int i = 0;
    while(i<frameCount) {
        pack_int16le(&d[0],s[(i*2)+0]);
#if CHANNELS == 2
        pack_int16le(&d[sizeof(int16_t)],s[(i*2)+1]);
#endif
        i++;
        d += (sizeof(int16_t) * CHANNELS);
    }
}

static int write_frames(FILE *f, uint8_t *d, unsigned int frameCount) {
    return fwrite(d,sizeof(int16_t) * CHANNELS,frameCount,f) == frameCount;
}
