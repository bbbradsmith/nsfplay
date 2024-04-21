#pragma once
// generated by nsfplayenums.py
// Sun Apr 21 03:40:40 2024

#include "../include/nsfplayenums.h"

const int32_t NSFPD_LIST[NSFP_LIST_COUNT] = {
	   0,   2,
};

typedef struct {
	const char* key;
	int32_t text;
} NSFSetGroupData;
const NSFSetGroupData NSFPD_GROUP[NSFP_GROUP_COUNT] = {
	{                         "MAIN",   4 },
	{                         "APU0",   8 },
	{                         "APU1",  12 },
};

typedef struct {
	const char* key;
	int32_t unit, text;
} NSFChannelData;
const NSFChannelData NSFPD_CHANNEL[NSFP_CHANNEL_COUNT] = {
	{                         "SQU0", 0,  16 },
	{                         "SQU1", 0,  18 },
	{                          "TRI", 1,  20 },
	{                          "NSE", 1,  22 },
	{                         "DPCM", 1,  24 },
};

typedef struct {
	const char* key;
	int32_t group, text;
	int32_t default_int, min_int, max_int, list;
	const char* default_str;
} NSFSetData;
const NSFSetData NSFPD_SET[NSFP_SET_COUNT] = {
	{                       "VOLUME",  0,  26,    500,      0,   1000, -1,NULL },
	{                   "SAMPLERATE",  0,  30,  48000,   1000,4000000, -1,NULL },
	{                       "STEREO",  0,  34,      1,      0,      1,  0,NULL },
	{                 "TITLE_FORMAT",  0,  38,      0,      0,      0, -1,"%L (%n/%e) %T - %A" },
	{                       "LOCALE",  0,  42,      0,      0,      1,  1,NULL },
	{                      "SQU0_ON",  1,  46,      1,      0,      1,  0,NULL },
	{                     "SQU0_VOL",  1,  50,    500,      0,   1000, -1,NULL },
	{                     "SQU0_PAN",  1,  54,    500,      0,   1000, -1,NULL },
	{                      "SQU1_ON",  1,  58,      1,      0,      1,  0,NULL },
	{                     "SQU1_VOL",  1,  62,    500,      0,   1000, -1,NULL },
	{                     "SQU1_PAN",  1,  66,    500,      0,   1000, -1,NULL },
	{                       "TRI_ON",  2,  70,      1,      0,      1,  0,NULL },
	{                      "TRI_VOL",  2,  74,    500,      0,   1000, -1,NULL },
	{                      "TRI_PAN",  2,  78,    500,      0,   1000, -1,NULL },
	{                       "NSE_ON",  2,  82,      1,      0,      1,  0,NULL },
	{                      "NSE_VOL",  2,  86,    500,      0,   1000, -1,NULL },
	{                      "NSE_PAN",  2,  90,    500,      0,   1000, -1,NULL },
	{                      "DPCM_ON",  2,  94,      1,      0,      1,  0,NULL },
	{                     "DPCM_VOL",  2,  98,    500,      0,   1000, -1,NULL },
	{                     "DPCM_PAN",  2, 102,    500,      0,   1000, -1,NULL },
};

typedef struct {
	const char* key;
	int32_t type, text;
} NSFPropData;
const NSFPropData NSFPD_PROP[NSFP_PROP_COUNT] = {
	{                    "SONGCOUNT",1, 106 },
	{                         "LONG",2, 108 },
	{                        "TITLE",3, 110 },
	{                         "INFO",4, 112 },
	{                         "BLOB",5, 114 },
};

typedef struct {
	const char* key;
	int32_t type, text;
} NSFSongPropData;
const NSFSongPropData NSFPD_SONGPROP[NSFP_SONGPROP_COUNT] = {
	{                          "INT",1, 116 },
	{                     "SONGTEST",1, 118 },
	{                         "LONG",2, 120 },
	{                        "TITLE",3, 122 },
	{                         "INFO",4, 124 },
	{                         "BLOB",5, 126 },
};

const int32_t NSFPD_TEXT[NSFP_LIST_COUNT] = {
	 128, 130,
};

const int32_t NSFPD_LOCAL_TEXT[NSFP_LOCALE_COUNT][132] = {
{
	0x000000,0x000000,0x000008,0x000008,0x00001B,0x000020,0x00001B,0x000020,0x00002E,0x000033,0x00002E,0x000033,0x00002E,0x00002E,0x00002E,0x00002E,
	0x000057,0x000057,0x00005C,0x00005C,0x000061,0x000061,0x000065,0x000065,0x000069,0x000069,0x00006E,0x000075,0x00006E,0x000075,0x00007C,0x000087,
	0x00007C,0x000087,0x000092,0x000099,0x000092,0x000099,0x0000A0,0x0000AD,0x0000A0,0x0000AD,0x0000C3,0x0000C3,0x0000C3,0x0000C3,0x0000CA,0x0000CA,
	0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D1,
	0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,
	0x0000D8,0x0000D8,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000E4,0x0000E4,
	0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000EB,0x0000EB,0x0000F6,0x0000F6,0x0000FB,0x0000FB,
	0x000101,0x000101,0x000106,0x000106,0x00010B,0x00010B,0x00010F,0x00010F,0x0000F6,0x0000F6,0x000119,0x000119,0x000101,0x000101,0x000106,0x000106,
	0x000124,0x000124,0x000129,0x000129,
},
{
	0x000000,0x000000,0x000008,0x000008,0x00001B,0x000020,0x00001B,0x000020,0x00002E,0x000033,0x00002E,0x000033,0x00002E,0x00002E,0x00002E,0x00002E,
	0x000057,0x000057,0x00005C,0x00005C,0x000061,0x000061,0x000065,0x000065,0x000069,0x000069,0x00006E,0x000075,0x00006E,0x000075,0x00007C,0x000087,
	0x00007C,0x000087,0x000092,0x000099,0x000092,0x000099,0x0000A0,0x0000AD,0x0000A0,0x0000AD,0x0000C3,0x0000C3,0x0000C3,0x0000C3,0x0000CA,0x0000CA,
	0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000CA,0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D1,
	0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D1,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,0x0000D8,
	0x0000D8,0x0000D8,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000DE,0x0000E4,0x0000E4,
	0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000E4,0x0000EB,0x0000EB,0x0000F6,0x0000F6,0x0000FB,0x0000FB,
	0x000101,0x000101,0x000106,0x000106,0x00010B,0x00010B,0x00010F,0x00010F,0x0000F6,0x0000F6,0x000119,0x000119,0x000101,0x000101,0x000106,0x000106,
	0x000124,0x000124,0x000129,0x000129,
},
};

const uint8_t NSFPD_LOCAL_TEXT_DATA[0x000135] = {
	0x4F,0x66,0x66,0x00,0x4F,0x6E,0x00,0x00,0x45,0x6E,0x67,0x6C,0x69,0x73,0x68,0x00,0xE6,0x97,0xA5,0xE6,0x9C,0xAC,0xE8,0xAA,0x9E,0x00,0x00,0x4D,0x61,0x69,0x6E,0x00,
	0x4D,0x61,0x69,0x6E,0x20,0x53,0x65,0x74,0x74,0x69,0x6E,0x67,0x73,0x00,0x41,0x50,0x55,0x31,0x00,0x42,0x75,0x69,0x6C,0x74,0x2D,0x69,0x6E,0x20,0x74,0x72,0x69,0x61,
	0x6E,0x67,0x6C,0x65,0x2C,0x20,0x6E,0x6F,0x69,0x73,0x65,0x2C,0x20,0x61,0x6E,0x64,0x20,0x44,0x50,0x43,0x4D,0x2E,0x00,0x53,0x51,0x55,0x30,0x00,0x53,0x51,0x55,0x31,
	0x00,0x54,0x52,0x49,0x00,0x4E,0x53,0x45,0x00,0x44,0x50,0x43,0x4D,0x00,0x56,0x4F,0x4C,0x55,0x4D,0x45,0x00,0x56,0x6F,0x6C,0x75,0x6D,0x65,0x00,0x53,0x41,0x4D,0x50,
	0x4C,0x45,0x52,0x41,0x54,0x45,0x00,0x53,0x61,0x6D,0x70,0x6C,0x65,0x72,0x61,0x74,0x65,0x00,0x53,0x54,0x45,0x52,0x45,0x4F,0x00,0x53,0x74,0x65,0x72,0x65,0x6F,0x00,
	0x54,0x49,0x54,0x4C,0x45,0x5F,0x46,0x4F,0x52,0x4D,0x41,0x54,0x00,0x4E,0x53,0x46,0x20,0x73,0x6F,0x6E,0x67,0x20,0x74,0x69,0x74,0x6C,0x65,0x20,0x66,0x6F,0x72,0x6D,
	0x61,0x74,0x00,0x4C,0x4F,0x43,0x41,0x4C,0x45,0x00,0x53,0x51,0x55,0x30,0x4F,0x4E,0x00,0x53,0x51,0x55,0x31,0x4F,0x4E,0x00,0x54,0x52,0x49,0x4F,0x4E,0x00,0x4E,0x53,
	0x45,0x4F,0x4E,0x00,0x44,0x50,0x43,0x4D,0x4F,0x4E,0x00,0x53,0x6F,0x6E,0x67,0x20,0x63,0x6F,0x75,0x6E,0x74,0x00,0x4C,0x4F,0x4E,0x47,0x00,0x54,0x49,0x54,0x4C,0x45,
	0x00,0x49,0x4E,0x46,0x4F,0x00,0x42,0x4C,0x4F,0x42,0x00,0x49,0x4E,0x54,0x00,0x53,0x6F,0x6E,0x67,0x20,0x74,0x65,0x73,0x74,0x00,0x53,0x6F,0x6E,0x67,0x20,0x74,0x69,
	0x74,0x6C,0x65,0x00,0x54,0x45,0x58,0x54,0x00,0x45,0x52,0x52,0x4F,0x52,0x5F,0x45,0x52,0x52,0x4F,0x52,0x00,
};

// end of file
