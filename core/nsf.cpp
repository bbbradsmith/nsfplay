// nsf.cpp
//   Parsing of the loaded NSF file (or other file types).
//   Fetching of properties and song properties.

#include "core.h"
#include <cstring> // std::memset, std::memcpy

//
// file parsing helpers
//

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

inline static bool has0(const uint8* bin, int len)
{
	for (int i=0;i<len;++i) if(bin[i]==0) return true;
	return false;
}

inline static bool nsfe_mandatory(uint32 fcc)
{
	char a = char(fcc & 0x000000FF);
	return a >= 'A' && a <= 'Z';
}

//
// internal core NSF parsing helpers
//

static const sint32 FT_NONE    = NSFP_LK_FILE_TYPE_NONE;
static const sint32 FT_NSF     = NSFP_LK_FILE_TYPE_NSF;
static const sint32 FT_NSF2    = NSFP_LK_FILE_TYPE_NSF2;
static const sint32 FT_NSFE    = NSFP_LK_FILE_TYPE_NSFE;
static const sint32 FT_BIN     = NSFP_LK_FILE_TYPE_BIN;
static const sint32 FT_INVALID = NSFP_LK_FILE_TYPE_INVALID;

inline sint32 NSFCore::nsf_type() const
{
	if (nsf == NULL) return FT_NONE;
	if (nsf_bin) return FT_BIN;
	if (nsf_size < 4) return FT_INVALID; // note enough data for NSFe tag
	if (le32(nsf)==FOURCC("NSFE")) return FT_NSFE;
	if (nsf_size < 0x80) return FT_INVALID; // not enough data for NSF header
	if (le32(nsf)!=FOURCC("NESM")) return FT_INVALID; // not NSF or NSF2
	if (nsf[0x04] != 0x1A) return FT_INVALID;
	if (nsf[0x05] > 1) return FT_NSF2; // version 2+
	return FT_NSF; // version 1
	// guarantee: if not INVALID nsf_size >= 4 for NSFe, and >= 0x80 for NSF/NSF2 (full header)
}

inline uint32 NSFCore::nsfe_data() const
{
	sint32 ft = nsf_type();
	if (ft == FT_NSFE) return 4; // first chunk after fourcc
	if (ft == FT_NSF2) // if metadata pointer is valid, first chunk there
	{
		uint32 nsf2_meta = le24(nsf+0x7D);
		if (nsf2_meta && ((nsf2_meta + 0x80) <= nsf_size))
			return nsf2_meta + 0x80;
	}
	return 0;
}

inline bool NSFCore::nsf_header_present() const
{
	sint32 ft = nsf_type();
	return (ft == FT_NSF || ft == FT_NSF2);
}

//
// validate a loaded file
//

bool NSFCore::nsf_parse(bool bin)
{
	NSFP_DEBUG("nsf_parse(%d)",bin);
	prop_lines = NULL;
	nsf_bin = false;
	song_current = 0;
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
			set_error(NSFP_ERROR_BIN_BAD);

		return true;
	}

	// validate the file, report errors, unload if unrecoverable

	const sint32 ft = nsf_type(); // ensures at least 0x80 for NSF/NSF2 header, or at least 4 for NSFe fourcc

	if (ft == FT_INVALID)
	{
		set_error(NSFP_ERROR_NSF_HEAD_BAD);
		load(NULL,0,false);
		return false;
	}

	const uint8* nsf_data = NULL;
	uint32 nsf_data_size = 0;
	bool header_found = false;
	uint16 load_addr = 0;

	if (ft != FT_NSFE) // validate NSF/NSF2 header, check for metadata
	{
		header_found = true;
		uint8 nsf_version = nsf[0x05];
		load_addr = le16(nsf+0x08);
		NSFP_DEBUG("NSF version: %d",nsf_version);

		nsf_data = nsf + 0x80; // data follows header
		nsf_data_size = nsf_size - 0x80;
		if (nsf_version < 1) set_error(NSFP_ERROR_NSF_VERSION_BAD);
		if (nsf_version >= 2)
		{
			uint32 nsf2_meta = le24(nsf+0x7D);
			if (nsf2_meta)
			{
				if ((nsf2_meta + 0x80) > nsf_size) set_error(NSFP_ERROR_NSF2_META_BAD);
				else nsf_data_size = nsf2_meta;
			}
		}
		// warning about bad header strings
		if (!has0(nsf+0x0E,32) || !has0(nsf+0x2E,32) || !has0(nsf+0x4E,32)) set_error(NSFP_ERROR_NSF_HEAD_TEXT_BAD);
	}

	if (nsfe_data()) // acknowledge all known mandatory chunks, warn about malformed chunks
	{
		bool nend = false;
		uint32 offset = nsfe_data();
		while (!nend && offset < nsf_size)
		{
			if ((offset + 8) > nsf_size)
			{
				set_error(NSFP_ERROR_NSFE_CHUNK_BAD,"NULL");
				break;
			}
			uint32 chunk_size = le32(nsf+offset+0);
			uint32 fcc        = le32(nsf+offset+4);
			if ((offset + chunk_size) > nsf_size)
			{
				set_error(NSFP_ERROR_NSFE_CHUNK_BAD,fcc_string(fcc));
				break;
			}
			NSFP_DEBUG("NSFE %08X + %08X: %s",offset+8,chunk_size,fcc_string(fcc));
			switch (fcc)
			{
			case FOURCC("INFO"):
				if (chunk_size < 0xA) // incomplete info chunk is unrecoverable
				{
					set_error(NSFP_ERROR_NSFE_CHUNK_BAD,fcc_string(fcc));
					load(NULL,0,false);
					return false;
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
					set_error(NSFP_ERROR_NSFE_MANDATORY,fcc_string(fcc));
					load(NULL,0,false);
					return false;
				}
				break;
			}
			offset += 8 + chunk_size;
		}
	}
	// we need a header and data to proceed
	if (!header_found)
	{
		set_error(NSFP_ERROR_NSFE_NO_MANDATORY,"INFO");
		load(NULL,0,false);
		return false;
	}
	if (nsf_data == NULL)
	{
		set_error(NSFP_ERROR_NSFE_NO_MANDATORY,"DATA");
		load(NULL,0,false);
		return false;
	}

	NSFP_DEBUG("nsf_data_size = %08X",nsf_data_size);
	NSFP_DEBUG("load_addr     = %04X",load_addr);
	// setup pad0/pad1 banks if needed
	load_addr &= 0x0FFF;
	int load_pad = load_addr ? (0x10000 - load_addr) : 0;
	rom = nsf_data - load_pad;
	if (load_addr) // pad0 reuses ram6000 @ E000 to make a padded version of bank 0
	{
		int copy_size = (load_pad <= int(nsf_data_size)) ? load_pad : nsf_data_size;
		pad0 = ram6000 + (0xE000 - 0x6000);
		std::memcpy(pad0 + load_addr, nsf_data, copy_size);
		NSFP_DEBUG("load_pad      = %04X",load_pad);
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
		NSFP_DEBUG("load_end      = %04X",load_end);
	}
	NSFP_DEBUG("bank_last     = %02X",bank_last);

	// set starting song
	song_current = uint8(nsf_prop_int(NSFP_PROP_SONG_START));
	NSFP_DEBUG("song_current  = %d",song_current);
	return true;
}

const uint8* NSFCore::nsfe_chunk(uint32 fourcc, uint32* chunk_size_out) const
{
	bool nend = false;
	uint32 offset = nsfe_data();
	if (chunk_size_out) *chunk_size_out = 0;
	if (offset == 0) return NULL; // no NSFE data
	while (!nend && offset < nsf_size)
	{
		if ((offset + 8) > nsf_size) return NULL;
		uint32 chunk_size = le32(nsf+offset+0);
		uint32 chunk_fcc  = le32(nsf+offset+4);
		if (chunk_fcc == FOURCC("NEND")) return NULL;
		if (fourcc == chunk_fcc)
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

bool NSFCore::nsf_prop_exists(sint32 prop, sint32 song) const
{
	if (song < 0)
	{
		switch(prop)
		{
		case NSFP_PROP_FILE_TYPE: return true;
		case NSFP_PROP_SONG_COUNT: return true;
		case NSFP_PROP_SONG_START: return true;
		default:
			break;
		}
	}
	else // songprop
	{
		switch(prop)
		{
		case 0: // TODO
		default:
			break;
		}
	}
	return false;
}

sint32 NSFCore::nsf_prop_int(sint32 prop, sint32 song) const
{
	const uint8* ck = NULL;
	uint32 cks = 0;

	if (song < 0)
	{
		switch(prop)
		{
		case NSFP_PROP_FILE_TYPE:
			return nsf_type();
		case NSFP_PROP_SONG_COUNT:
			ck = nsfe_chunk(FOURCC("INFO"),&cks);
			if (ck && cks > 0x8) return ck[0x8];
			if (nsf_header_present()) return nsf[0x06];
			return 0;
		case NSFP_PROP_SONG_START:
			ck = nsfe_chunk(FOURCC("INFO"),&cks);
			if (ck && cks > 0x9) return ck[0x9];
			if (nsf_header_present() && (nsf[0x07]>0)) return nsf[0x07]-1; // NSF header indexes first song as 1 (treating 0 also as first song)
			return 0;
		default:
			break;
		}
	}
	else // songprop
	{
		switch(prop)
		{
		case 0: // TODO
		default:
			break;
		}
	}
	return 0;
}

sint64 NSFCore::nsf_prop_long(sint32 prop, sint32 song) const
{
	(void)prop;
	(void)song;
	// TODO
	return nsf_prop_int(prop,song); // fallback to prop int
}

const char* NSFCore::nsf_prop_str(sint32 prop, sint32 song) const
{
	(void)prop;
	(void)song;
	// TODO
	return NULL;
}

sint32 NSFCore::nsf_prop_lines(sint32 prop, sint32 song) const
{
	(void)prop;
	(void)song;
	// TODO
	prop_lines = NULL;
	return 0;
}

const char* NSFCore::nsf_prop_line() const
{
	// TODO
	// return prop_lines, advance to next line
	return NULL;
}

const void* NSFCore::nsf_prop_blob(uint32* blob_size, sint32 prop, sint32 song) const
{
	(void)blob_size;
	(void)prop;
	(void)song;
	// TODO
	return NULL;
}
