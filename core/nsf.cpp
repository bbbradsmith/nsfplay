// nsf.cpp
//   Parsing of the loaded NSF file (or other file types).
//   Fetching of properties and song properties.

#include "core.h"
#include <cstring> // std::memset, std::memcpy

//
// file parsing helpers
//

static const char* MISSING_STR = "<?>";

inline static uint16 le16(const uint8* bin) { return bin[0] | (bin[1] << 8); }
inline static uint32 le24(const uint8* bin) { return bin[0] | (bin[1] << 8) | (bin[2] << 16); }
inline static uint32 le32(const uint8* bin) { return bin[0] | (bin[1] << 8) | (bin[2] << 16) | (bin[3] << 24); }

#define FOURCC(fcc_) ( (uint32(fcc_[0])) | ((uint32(fcc_[1])) << 8) | ((uint32(fcc_[2])) << 16) | ((uint32(fcc_[3])) << 24) )

inline static const char* fcc_string(uint32 fcc)
{
	static char fcc_str[5];
	fcc_str[0] = char((fcc >>  0) & 0xFF);
	fcc_str[1] = char((fcc >>  8) & 0xFF);
	fcc_str[2] = char((fcc >> 16) & 0xFF);
	fcc_str[3] = char((fcc >> 24) & 0xFF);
	fcc_str[4] = 0;
	return fcc_str;
}

inline static bool has0(const uint8* bin, int len) // at least 1 byte is 0
{
	for (int i=0;i<len;++i) if(bin[i]==0) return true;
	return false;
}

inline static bool all0(const uint8* bin, int len) // all bytes are 0
{
	for (int i=0;i<len;++i) if(bin[i]==0) return true;
	return false;
}

inline static bool nsfe_mandatory(uint32 fcc) // first letter of fcc is capital letter
{
	char a = char(fcc & 0x000000FF);
	return a >= 'A' && a <= 'Z';
}

inline static int count_strings(const uint8* chunk, uint32 chunk_size) // count null terminated strings within chunk
{
	int count = 0;
	while (chunk_size > 0)
	{
		if (*chunk == 0) ++count;
		++chunk;
		--chunk_size;
	}
	return count;
}

inline static const char* nth_string(const uint8* chunk, uint32 chunk_size, int n) // return the nth null terminated string (make sure n < count)
{
	if (!chunk_size) return MISSING_STR;
	while (n > 0)
	{
		if (*chunk == 0) --n;
		++chunk;
		--chunk_size;
	}
	return reinterpret_cast<const char*>(chunk);
}

inline static sint32 nsf_speed16(uint32 rate) // convert 1/10,000 Hz setting to NSF rate setting
{
	double fresult = 1000000.0 / (double(rate)/10000.0);
	sint64 result = sint64(fresult + 0.5);
	if (result > 0xFFFF) result = 0xFFFF;
	if (result < 0) result = 0;
	return sint32(result);
}

//
// core property helpers
//

static const sint32 FT_NONE    = NSF_LK_FILE_TYPE_NONE;
static const sint32 FT_NSF     = NSF_LK_FILE_TYPE_NSF;
static const sint32 FT_NSF2    = NSF_LK_FILE_TYPE_NSF2;
static const sint32 FT_NSFE    = NSF_LK_FILE_TYPE_NSFE;
static const sint32 FT_BIN     = NSF_LK_FILE_TYPE_BIN;
static const sint32 FT_INVALID = NSF_LK_FILE_TYPE_INVALID;

inline static sint32 nsf_type(const NSFCore* core)
{
	if (core->nsf == NULL) return FT_NONE;
	if (core->nsf_bin) return FT_BIN;
	if (core->nsf_size < 4) return FT_INVALID; // note enough data for NSFe tag
	if (le32(core->nsf)==FOURCC("NSFE")) return FT_NSFE;
	if (core->nsf_size < 0x80) return FT_INVALID; // not enough data for NSF header
	if (le32(core->nsf)!=FOURCC("NESM")) return FT_INVALID; // not NSF or NSF2
	if (core->nsf[0x04] != 0x1A) return FT_INVALID;
	if (core->nsf[0x05] > 1) return FT_NSF2; // version 2+
	return FT_NSF; // version 1
	// guarantee: if not INVALID nsf_size >= 4 for NSFe, and >= 0x80 for NSF/NSF2 (full header)
}

inline static bool has_nsf_header(const NSFCore* core)
{
	sint32 ft = nsf_type(core);
	return (ft == FT_NSF || ft == FT_NSF2);
}

inline static bool is_nsfx(const NSFCore* core)
{
	sint32 ft = nsf_type(core);
	return (ft == FT_NSF || ft == FT_NSF2 || ft == FT_NSFE);
}

inline static uint32 nsfe_data(const NSFCore* core)
{
	sint32 ft = nsf_type(core);
	if (ft == FT_NSFE) return 4; // first chunk after fourcc
	if (has_nsf_header(core)) // if metadata pointer is valid, first chunk there
	{
		uint32 nsf2_meta = le24(core->nsf+0x7D);
		if (nsf2_meta && ((nsf2_meta + 0x80) <= core->nsf_size))
			return nsf2_meta + 0x80;
	}
	return 0;
}

inline static uint32 active_playlist_len(const NSFCore* core)
{
	const int setplay = core->setting[NSF_SET_PLAYLIST];
	if (setplay != NSF_LK_PLAYLIST_OFF)
	{
		uint32 cs = 0;
		core->nsfe_chunk((setplay==NSF_LK_PLAYLIST_PSFX) ? FOURCC("psfx") : FOURCC("plst"),&cs);
		return cs;
	}
	return 0;
}

inline static sint32 resolve_nsf_song(const NSFCore* core, sint32 song)
{
	if (song < 0) song = core->active_song;
	if (song < 0) return 0; // invalid active_song?
	const int setplay = core->setting[NSF_SET_PLAYLIST];
	if (setplay != NSF_LK_PLAYLIST_OFF)
	{
		uint32 cs = 0;
		const uint8* chk = core->nsfe_chunk((setplay==NSF_LK_PLAYLIST_PSFX) ? FOURCC("psfx") : FOURCC("plst"),&cs);
		if (!chk) return song;
		if (cs <= (uint32)song) return 0; // outside of playlist?
		return chk[song];
	}
	return song; // no playlist
}

inline static uint8 nsfe_nsf_shared_bit(const NSFCore* core, uint32 nsfe_fcc, uint32 nsfe_offset, uint32 nsf_offset, uint8 bitmask)
{
	// return true if either a bit in an NSFe chunk byte is set, or the NSF header
	uint32 cs = 0;
	const uint8* chk = core->nsfe_chunk(nsfe_fcc,&cs);
	if (chk && cs > nsfe_offset && (      chk[nsfe_offset] & bitmask)) return true;
	if (has_nsf_header(core)    && (core->nsf[nsf_offset ] & bitmask)) return true;
	return false;
}

inline const char* legacy_string(const NSFCore* core, const uint8* data)
{
	NSF_UNUSED(core);
	return reinterpret_cast<const char*>(data);
	// TODO detect impossibility of shift-jis (ASCII only?) and return direct reinterpret
	//if (core->setting[SHIFT_JIS] == NSF_LK_ENABLE_AUTO_AUTO) // detect Shift-JIS, copy to temp_text
	//if (core->setting[SHIFT_JIS] == NSF_LK_ENABLE_ON) // force Shift-JIS, copy to temp_text
}

// check NSF type, NSFx = NSF/NSF2/NSFe
#define NSFTYPE() nsf_type(this)
#define NSFHDR() has_nsf_header(this)
#define ISNSFE() (NSF_TYPE()==FT_NSFE)
#define ISNSF2() (NSF_TYPE()==FT_NSF2)
#define ISNSFX() is_nsfx(this)

#define NSFE_NSF_BIT(fcc_,nsfe_off_,nsf_off_,bitmask_) nsfe_nsf_shared_bit(this,FOURCC(fcc_),nsfe_off_,nsf_off_,bitmask_)

// CHK: fetch NSFe chunk as chk and size cs, true if found
// CHKS: fetch NSFe chunk makes sure it has more than size_min_ bytes
#define CHK(fcc_str_) ((chk=nsfe_chunk(FOURCC(fcc_str_),&(cs)))!=NULL)
#define CHKS(fcc_str_,size_min_) (CHK(fcc_str_)&&(cs>(size_min_)))

// use at the start of each property fetch function
#define PROPSETUP() \
	if (song < 0) song = active_song; \
	const uint8* chk = NULL; /* resable chunk pointer */ \
	uint32 cs = 0; /* chunk size */ \
	sint32 nsf_song = resolve_nsf_song(this,song); \
	NSF_UNUSED(chk); \
	NSF_UNUSED(cs); \
	NSF_UNUSED(nsf_song);

//
// parse the loaded file
//

bool NSFCore::nsf_parse(bool bin)
{
	NSF_DEBUG("nsf_parse(%d)",bin);
	active_prop_lines = NULL;
	nsf_bin = false;
	active_song = 0;
	std::memset(ram0000,0,sizeof(ram0000));
	std::memset(ram6000,0,sizeof(ram6000));

	if (nsf == NULL) return true; // no file

	if (bin) // bin file
	{
		nsf_bin = true;
		uint32 bin_size = nsf_size;
		if (bin_size > (40*1024))
			bin_size = (40*1024);
		std::memcpy(ram6000,nsf,bin_size);
		if (bin_size != nsf_size) // bin was too big, but this is recoverable
			set_error(NSF_ERROR_BIN_BAD);
		return true;
	}

	// validate the file, report errors, unload if unrecoverable
	//
	// we try to continue as long as the error is recoverable,
	// but there is also some redundant parsing here to give validation warnings,
	// to be of help to those trying to debug their NSFs.

	const sint32 ft = NSFTYPE();
	if (ft == FT_INVALID)
	{
		set_error(NSF_ERROR_NSF_HEAD_BAD);
		load(NULL,0,false);
		return false;
	}
	// ensures at least 0x80 for NSF/NSF2 header, or at least 4 for NSFe fourcc

	const uint8* nsf_data = NULL;
	uint32 nsf_data_size = 0;
	bool header_found = false;
	uint16 load_addr = 0;
	bool mandatory = false;

	if (ft != FT_NSFE) // validate NSF/NSF2 header, check for metadata
	{
		header_found = true;
		uint8 nsf_version = nsf[0x05];
		load_addr = le16(nsf+0x08);
		NSF_DEBUG("NSF version: %d",nsf_version);

		nsf_data = nsf + 0x80; // data follows header
		nsf_data_size = nsf_size - 0x80;
		if (nsf_version < 1) set_error(NSF_ERROR_NSF_VERSION_BAD);
		if (nsf_version >= 2) // NSF1 may still use metadata but we will treat it as part of the NSF data if present
		{
			uint32 nsf2_meta = le24(nsf+0x7D);
			if (nsf2_meta)
			{
				if ((nsf2_meta + 0x80) > nsf_size) set_error(NSF_ERROR_NSF2_META_BAD);
				else
				{
					nsf_data_size = nsf2_meta;
					mandatory = (nsf[0x7C] & 0x80) != 0; // NSF2 has a mandatory metadata flag
				}
			}
		}
		// warning about bad header strings
		if (!has0(nsf+0x0E,32) || !has0(nsf+0x2E,32) || !has0(nsf+0x4E,32)) set_error(NSF_ERROR_NSF_HEAD_TEXT_BAD);
	}
	else mandatory = true;

	if (nsfe_data(this)) // acknowledge all known mandatory chunks, warn about malformed chunks
	{
		bool nend = false;
		uint32 offset = nsfe_data(this);
		while (!nend && offset < nsf_size)
		{
			if ((offset + 8) > nsf_size)
			{
				set_error(NSF_ERROR_NSFE_CHUNK_BAD,"NULL");
				break;
			}
			uint32 chunk_size = le32(nsf+offset+0);
			uint32 fcc        = le32(nsf+offset+4);
			if ((offset + chunk_size) > nsf_size)
			{
				set_error(NSF_ERROR_NSFE_CHUNK_BAD,fcc_string(fcc));
				break;
			}
			NSF_DEBUG("NSFE %08X + %08X: %s",offset+8,chunk_size,fcc_string(fcc));
			switch (fcc)
			{
			case FOURCC("INFO"):
				if (chunk_size < 0xA) // incomplete info chunk is unrecoverable
				{
					set_error(NSF_ERROR_NSFE_CHUNK_BAD,fcc_string(fcc));
					if (mandatory)
					{
						load(NULL,0,false);
						return false;
					}
				}
				load_addr = le16(nsf+offset+8+0x0);
				header_found = true;
				break;
			case FOURCC("DATA"): // found our data too
				nsf_data = nsf + offset + 8;
				nsf_data_size = chunk_size;
				break;
			case FOURCC("NEND"): // stop parsing
				nend = true;
				break;
			case FOURCC("BANK"): // mandatory chunks we don't need to store, we'll check for them when we start a song
			case FOURCC("RATE"):
			case FOURCC("NSF2"):
			case FOURCC("VRC7"):
				break;
			default:
				if (nsfe_mandatory(fcc)) // failing to handle a mandatory chunk is unrecoverable
				{
					set_error(NSF_ERROR_NSFE_MANDATORY,fcc_string(fcc));
					if (mandatory)
					{
						load(NULL,0,false);
						return false;
					}
				}
				break;
			}
			offset += 8 + chunk_size;
		}
	}
	// we need a header and data to proceed
	if (!header_found)
	{
		set_error(NSF_ERROR_NSFE_NO_MANDATORY,"INFO");
		load(NULL,0,false);
		return false;
	}
	if (nsf_data == NULL)
	{
		set_error(NSF_ERROR_NSFE_NO_MANDATORY,"DATA");
		load(NULL,0,false);
		return false;
	}

	NSF_DEBUG("nsf_data_size = %08X",nsf_data_size);
	NSF_DEBUG("load_addr     = %04X",load_addr);
	// setup pad0/pad1 banks if needed
	load_addr &= 0x0FFF;
	int load_pad = load_addr ? (0x10000 - load_addr) : 0;
	rom = nsf_data - load_pad;
	if (load_addr) // pad0 reuses ram6000 @ E000 to make a padded version of bank 0
	{
		int copy_size = (load_pad <= int(nsf_data_size)) ? load_pad : nsf_data_size;
		pad0 = ram6000 + (0xE000 - 0x6000);
		std::memcpy(pad0 + load_addr, nsf_data, copy_size);
		NSF_DEBUG("load_pad      = %04X",load_pad);
	}
	// determine index of last bank
	bank_last = 0;
	if ((load_pad + nsf_data_size) > 0)
		bank_last = (load_pad + nsf_data_size - 1) / 0x1000;
	// setup pad1 if needed
	int load_end = (load_pad + nsf_data_size) & 0x0FFF;
	if (load_end && bank_last) // pad1 reuses ram6000 @ F000 to make a padded version of bank_last
	{
		pad1 = ram6000 + (0xF000 - 0x6000);
		std::memcpy(pad1, nsf_data + (nsf_data_size - load_end), load_end);
		NSF_DEBUG("load_end      = %04X",load_end);
	}
	NSF_DEBUG("bank_last     = %02X",bank_last);

	// set starting song
	active_song = uint8(PROP(ACTIVE_SONG_START));
	NSF_DEBUG("active_song   = %d",active_song);
	return true;
}

const uint8* NSFCore::nsfe_chunk(uint32 fourcc, uint32* chunk_size_out) const
{
	bool nend = false;
	uint32 offset = nsfe_data(this);
	if (chunk_size_out) *chunk_size_out = 0;
	if (offset == 0) return NULL; // no NSFE data
	while (!nend && offset < nsf_size)
	{
		if ((offset + 8) > nsf_size) return NULL;
		uint32 chunk_size = le32(nsf+offset+0);
		uint32 chunk_fcc  = le32(nsf+offset+4);
		if (chunk_fcc == FOURCC("NEND")) return NULL;
		if (fourcc == chunk_fcc && (offset+8+chunk_size) <= nsf_size)
		{
			if (chunk_size_out) *chunk_size_out = chunk_size;
			return nsf+offset+8;
		}
		offset += 8 + chunk_size;
	}
	return NULL;
}

const uint8* NSFCore::nsfe_chunk(const char* fourcc, uint32* chunk_size) const
{
	return nsfe_chunk(FOURCC(fourcc), chunk_size);
}

bool NSFCore::prop_exists(sint32 prop, sint32 song) const
{
	PROPSETUP();
	switch(prop) // this switch should handle every PROP
	{
	case NSF_PROP_TEXT: return CHK("text");
	case NSF_PROP_FILE_TYPE: return true;
	case NSF_PROP_NSF_SONG_COUNT: return true;
	case NSF_PROP_NSF_SONG_START: return true;
	case NSF_PROP_NSF_VERSION: return NSFHDR();
	case NSF_PROP_LOAD_ADDR:
	case NSF_PROP_INIT_ADDR:
	case NSF_PROP_PLAY_ADDR:
		return true;
	case NSF_PROP_TITLE:
		if (NSFHDR()) return true;
		return (CHK("auth") && count_strings(chk,cs) >= 1);
	case NSF_PROP_ARTIST:
		if (NSFHDR()) return true;
		return (CHK("auth") && count_strings(chk,cs) >= 2);
	case NSF_PROP_COPYRIGHT:
		if (NSFHDR()) return true;
		return (CHK("auth") && count_strings(chk,cs) >= 3);
	case NSF_PROP_RIPPER:
		return (CHK("auth") && count_strings(chk,cs) >= 4);
	case NSF_PROP_SPEED_NTSC:
	case NSF_PROP_SPEED_PAL:
	case NSF_PROP_SPEED_DENDY:
		return true;
	case NSF_PROP_BANKSWITCH:
		if (CHK("BANK")) return true;
		return (NSFHDR() && !all0(nsf+0x70,8));
	case NSF_PROP_REGION_NTSC:
	case NSF_PROP_REGION_PAL:
	case NSF_PROP_REGION_DENDY:
		return ISNSFX();
	case NSF_PROP_REGION_PREFER:
		return (CHKS("regn",1) && (chk[1] < NSF_LK_REGIONLIST_COUNT));
	case NSF_PROP_EXPANSION_FDS:
	case NSF_PROP_EXPANSION_MMC5:
	case NSF_PROP_EXPANSION_VRC6:
	case NSF_PROP_EXPANSION_VRC7:
	case NSF_PROP_EXPANSION_N163:
	case NSF_PROP_EXPANSION_S5B:
	case NSF_PROP_EXPANSION_VT02:
		return true;
	case NSF_PROP_NSF2:
	case NSF_PROP_NSF2_METADATA_OFF:
	case NSF_PROP_NSF2_IRQ:
	case NSF_PROP_NSF2_INIT_NORETURN:
	case NSF_PROP_NSF2_NOPLAY:
	case NSF_PROP_NSF2_MANDATORY:
		return true;
	case NSF_PROP_NSF_HEADER: return NSFHDR();
	case NSF_PROP_NSFE_INFO: return CHK("INFO");
	case NSF_PROP_NSFE_BANK: return CHK("BANK");
	case NSF_PROP_NSFE_RATE: return CHK("RATE");
	case NSF_PROP_NSFE_NSF2: return CHK("NSF2");
	case NSF_PROP_NSFE_VRC7: return CHK("VRC7");
	case NSF_PROP_NSFE_PLST: return CHK("plst");
	case NSF_PROP_NSFE_PSFX: return CHK("psfx");
	case NSF_PROP_NSFE_TIME: return CHK("time");
	case NSF_PROP_NSFE_FADE: return CHK("fade");
	case NSF_PROP_NSFE_TLBL: return CHK("tlbl");
	case NSF_PROP_NSFE_TAUT: return CHK("taut");
	case NSF_PROP_NSFE_TEXT: return CHK("text");
	case NSF_PROP_NSFE_MIXE: return CHK("mixe");
	case NSF_PROP_NSFE_REGN: return CHK("regn");
	case NSF_PROP_ACTIVE_SONG:
	case NSF_PROP_ACTIVE_SONG_COUNT:
	case NSF_PROP_ACTIVE_SONG_START:
	case NSF_PROP_ACTIVE_PLAYLIST:
	case NSF_PROP_ACTIVE_SONG_NSF:
	case NSF_PROP_ACTIVE_CPU_FREQ:
		return true;
	case NSF_PROP_ACTIVE_BANKS: return prop_exists(NSF_PROP_BANKSWITCH);
	case NSF_PROP_ACTIVE_EMU_FRAME_CY: return true;
	// song props
	case NSF_PROP_SONG_TITLE:

	default:
		break;
	}

	return false;
}

sint32 NSFCore::prop_int(sint32 prop, sint32 song) const
{
	PROPSETUP();
	switch(prop)
	{
	case NSF_PROP_FILE_TYPE: return NSFTYPE();
	case NSF_PROP_NSF_SONG_COUNT:
		if (CHKS("INFO",8)) return chk[0x08];
		if (NSFHDR())       return nsf[0x06];
		return 0;
	case NSF_PROP_NSF_SONG_START:
		if (CHKS("INFO",9)) return chk[0x09];
		if (NSFHDR() && nsf[0x07]) return nsf[0x07]-1; // NSF header indexes first song as 1 (treating 0 also as first song)
		return 0;
	case NSF_PROP_NSF_VERSION:
		if(NSFHDR()) return nsf[0x05];
		return 0;
	case NSF_PROP_LOAD_ADDR:
		if (nsf_bin) return 0x6000;
		if (CHKS("INFO",1)) return le16(chk+0x00);
		if (NSFHDR())       return le16(nsf+0x08);
		return 0x8000;
	case NSF_PROP_INIT_ADDR:
		if (nsf_bin) return 0x6000;
		if (CHKS("INFO",3)) return le16(chk+0x02);
		if (NSFHDR())       return le16(nsf+0x0A);
		return 0x8000;
	case NSF_PROP_PLAY_ADDR:
		if (nsf_bin) return 0x6000;
		if (CHKS("INFO",5)) return le16(chk+0x04);
		if (NSFHDR())       return le16(nsf+0x0C);
		return 0x8000;
	case NSF_PROP_SPEED_NTSC:
		if (CHKS("RATE",1)) return le16(chk+0x00);
		if (NSFHDR())       return le16(nsf+0x6E);
		return nsf_speed16(SETTING(FRAME_NTSC));
	case NSF_PROP_SPEED_PAL:
		if (CHKS("RATE",3)) return le16(chk+0x02);
		if (NSFHDR())       return le16(nsf+0x78);
		return nsf_speed16(SETTING(FRAME_PAL));
	case NSF_PROP_SPEED_DENDY:
		if (CHKS("RATE",5)) return le16(chk+0x04);
		if (NSFHDR())       return PROP(SPEED_PAL); // NSF has no Dendy property, fallback to PAL
		return nsf_speed16(SETTING(FRAME_DENDY));
	case NSF_PROP_REGION_NTSC:
		if (CHKS("regn",0) && (chk[0x00] & 1)) return 1;
		if (CHKS("INFO",6) && (!(chk[0x06] & 1) || (chk[0x06] & 2))) return 1;
		if (NSFHDR()       && (!(nsf[0x7A] & 1) || (nsf[0x7A] & 2))) return 1;
		return 0;
	case NSF_PROP_REGION_PAL:
		if (CHKS("regn",0) && (chk[0x00] & 2)) return 1;
		if (CHKS("INFO",6) && ( (chk[0x06] & 1) || (chk[0x06] & 2))) return 1;
		if (NSFHDR()       && ( (nsf[0x7A] & 1) || (nsf[0x7A] & 2))) return 1;
		return 0;
	case NSF_PROP_REGION_DENDY:
		if (CHKS("regn",0) && (chk[0x00] & 4)) return 1;
		return 0;
	case NSF_PROP_REGION_PREFER:
		if (CHKS("regn",1) && chk[0x01] < NSF_LK_REGIONLIST_COUNT) return chk[0x01];
		return 0;
	case NSF_PROP_EXPANSION_FDS:      return NSFE_NSF_BIT("INFO",0x07,0x7B,0x04) ? 1: 0;
	case NSF_PROP_EXPANSION_MMC5:     return NSFE_NSF_BIT("INFO",0x07,0x7B,0x08) ? 1: 0;
	case NSF_PROP_EXPANSION_VRC6:     return NSFE_NSF_BIT("INFO",0x07,0x7B,0x01) ? 1: 0;
	case NSF_PROP_EXPANSION_VRC7:     return NSFE_NSF_BIT("INFO",0x07,0x7B,0x02) ? 1: 0;
	case NSF_PROP_EXPANSION_N163:     return NSFE_NSF_BIT("INFO",0x07,0x7B,0x10) ? 1: 0;
	case NSF_PROP_EXPANSION_S5B:      return NSFE_NSF_BIT("INFO",0x07,0x7B,0x20) ? 1: 0;
	case NSF_PROP_EXPANSION_VT02:     return NSFE_NSF_BIT("INFO",0x07,0x7B,0x40) ? 1: 0;
	case NSF_PROP_NSF2: return (NSFTYPE() == FT_NSF2) ? 1 : 0;
	case NSF_PROP_NSF2_METADATA_OFF:
		if (NSFHDR()) return le24(nsf+0x7D);
		return 0;
	case NSF_PROP_NSF2_IRQ:           return NSFE_NSF_BIT("NSF2",0x00,0x7C,0x10) ? 1: 0;
	case NSF_PROP_NSF2_INIT_NORETURN: return NSFE_NSF_BIT("NSF2",0x00,0x7C,0x20) ? 1: 0;
	case NSF_PROP_NSF2_NOPLAY:        return NSFE_NSF_BIT("NSF2",0x00,0x7C,0x40) ? 1: 0;
	case NSF_PROP_NSF2_MANDATORY:     return NSFE_NSF_BIT("NSF2",0x00,0x7C,0x80) ? 1: 0;

	case NSF_PROP_ACTIVE_SONG: return active_song;
	case NSF_PROP_ACTIVE_SONG_COUNT:
		{ uint32 l = active_playlist_len(this); if(l) return l; }
		return PROP(NSF_SONG_COUNT);
	case NSF_PROP_ACTIVE_SONG_START:
		if (active_playlist_len(this)) return 0; // playlist starts at the beginning
		return PROP(NSF_SONG_START);
	case NSF_PROP_ACTIVE_PLAYLIST: return active_playlist_len(this) ? 1 : 0;
	case NSF_PROP_ACTIVE_SONG_NSF: return resolve_nsf_song(this,active_song);
	case NSF_PROP_ACTIVE_CPU_FREQ: return 0; // TODO
	case NSF_PROP_ACTIVE_EMU_FRAME_CY: return 0; // TODO

	default:
		break;
	}
	return 0;
}

sint64 NSFCore::prop_long(sint32 prop, sint32 song) const
{
	PROPSETUP();
	switch(prop)
	{
	case 0: // TODO
	default:
		break;
	}
	return prop_int(prop,song); // fallback to prop int
}

const char* NSFCore::prop_str(sint32 prop, sint32 song) const
{
	PROPSETUP();
	switch(prop)
	{
	case NSF_PROP_TITLE:
		if (CHK("auth") && count_strings(chk,cs) >= 1) return nth_string(chk,cs,0);
		if (NSFHDR() && has0(nsf+0x0E,32)) return legacy_string(this,nsf+0x0E);
		break;
	case NSF_PROP_ARTIST:
		if (CHK("auth") && count_strings(chk,cs) >= 2) return nth_string(chk,cs,1);
		if (NSFHDR() && has0(nsf+0x2E,32)) return legacy_string(this,nsf+0x2E);
		break;
	case NSF_PROP_COPYRIGHT:
		if (CHK("auth") && count_strings(chk,cs) >= 3) return nth_string(chk,cs,2);
		if (NSFHDR() && has0(nsf+0x4E,32)) return legacy_string(this,nsf+0x4E);
		break;
	case NSF_PROP_RIPPER:
		if (CHK("auth") && count_strings(chk,cs) >= 4) return nth_string(chk,cs,3);
		break;
	case NSF_PROP_SONG_TITLE:
		return MISSING_STR; // TODO generate song title

	default:
		break;
	}
	return MISSING_STR;
}

sint32 NSFCore::prop_lines(sint32 prop, sint32 song) const
{
	PROPSETUP();
	active_prop_lines = NULL;
	active_prop_lines_len = 0;
	switch(prop)
	{
	case NSF_PROP_TEXT: if (CHK("text")) break;

	default:
		break;
	}
	if (chk)
	{
		// can handle data without a terminating zero
		// newline at end of data will still count as an extra line, it is not eliminated
		active_prop_lines = chk;
		active_prop_lines_len = cs;
		sint32 lines = 1;
		uint32 line_width = 0;
		while (cs > 0 && *chk)
		{
			uint8 c = *chk;
			++chk;
			--cs;
			if (c == 10 || c == 13)
			{
				line_width = 0;
				++lines;
				if ((c == 10 && *chk == 13) || // LF CR (obscure)
				    (c == 13 && *chk == 10))   // CR LF (windows)
				{
					++chk;
					--cs;
				}
			}
			else
			{
				++line_width;
				if (line_width >= (TEMP_TEXT_SIZE-1)) // break lines too long for temp_text
				{
					++lines;
					line_width = 0;
				}
			}
			++chk;
		}
		return lines;
	}
	return 0;
}

const char* NSFCore::prop_line() const
{
	if (active_prop_lines == NULL) return NULL;
	uint32 line_width = 0;
	bool newline = false;
	while (active_prop_lines_len > 0 && *active_prop_lines)
	{
		uint8 c = *active_prop_lines;
		++active_prop_lines;
		--active_prop_lines_len;
		if (c == 10 || c == 13)
		{
			newline = true;
			if ((c == 10 && *active_prop_lines == 13) || // LF CR
			    (c == 13 && *active_prop_lines == 10))   // CR LF
			{
				++active_prop_lines;
				if (active_prop_lines_len) --active_prop_lines_len;
			}
			break;
		}
		else
		{
			temp_text[line_width] = c;
			++line_width;
			if (line_width >= (TEMP_TEXT_SIZE-1)) break;
		}
	}
	temp_text[line_width] = 0;
	// if we don't break because of a newline, stop if it was from end of data or null terminator
	if (!newline && (active_prop_lines_len == 0 || *active_prop_lines == 0))
	{
		active_prop_lines = NULL;
		active_prop_lines_len = 0;
	}
	return temp_text;
}

const uint8* NSFCore::prop_blob(uint32* blob_size, sint32 prop, sint32 song) const
{
	PROPSETUP();
	const uint8* blob = NULL;
	uint32 bsize = 0;
	switch(prop)
	{
	case NSF_PROP_BANKSWITCH:
		if ((blob = nsfe_chunk(FOURCC("BANK"),&bsize)) != NULL) break;
		if (NSFHDR() && !all0(nsf+0x70,8)) { blob = nsf+0x70; bsize = 8; break; }
		break;

	case NSF_PROP_NSF_HEADER:
		if (NSFHDR()) { bsize = 0x80; blob = nsf; break; }
		break;
	case NSF_PROP_NSFE_INFO: blob = nsfe_chunk(FOURCC("INFO"),&bsize); break;
	case NSF_PROP_NSFE_BANK: blob = nsfe_chunk(FOURCC("BANK"),&bsize); break;
	case NSF_PROP_NSFE_RATE: blob = nsfe_chunk(FOURCC("RATE"),&bsize); break;
	case NSF_PROP_NSFE_NSF2: blob = nsfe_chunk(FOURCC("NSF2"),&bsize); break;
	case NSF_PROP_NSFE_VRC7: blob = nsfe_chunk(FOURCC("VRC7"),&bsize); break;
	case NSF_PROP_NSFE_PLST: blob = nsfe_chunk(FOURCC("plst"),&bsize); break;
	case NSF_PROP_NSFE_PSFX: blob = nsfe_chunk(FOURCC("psfx"),&bsize); break;
	case NSF_PROP_NSFE_TIME: blob = nsfe_chunk(FOURCC("time"),&bsize); break;
	case NSF_PROP_NSFE_FADE: blob = nsfe_chunk(FOURCC("fade"),&bsize); break;
	case NSF_PROP_NSFE_TLBL: blob = nsfe_chunk(FOURCC("tlbl"),&bsize); break;
	case NSF_PROP_NSFE_TAUT: blob = nsfe_chunk(FOURCC("taut"),&bsize); break;
	case NSF_PROP_NSFE_TEXT: blob = nsfe_chunk(FOURCC("text"),&bsize); break;
	case NSF_PROP_NSFE_MIXE: blob = nsfe_chunk(FOURCC("mixe"),&bsize); break;
	case NSF_PROP_NSFE_REGN: blob = nsfe_chunk(FOURCC("regn"),&bsize); break;

	case NSF_PROP_ACTIVE_BANKS:
		break; // TODO

	default:
		break;
	}

	if (!blob) bsize = 0;
	if (blob_size) *blob_size = bsize;
	return blob;
}
