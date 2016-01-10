/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: load_s3m.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

//////////////////////////////////////////////////////
// ScreamTracker S3M file support

typedef struct tagS3MSAMPLESTRUCT
{
    uint8_t type;
    int8_t dosname[12];
    uint8_t hmem;
    uint16_t memseg;
    uint32_t length;
    uint32_t loopbegin;
    uint32_t loopend;
    uint8_t vol;
    uint8_t bReserved;
    uint8_t pack;
    uint8_t flags;
    uint32_t finetune;
    uint32_t dwReserved;
    uint16_t intgp;
    uint16_t int512;
    uint32_t lastused;
    int8_t Name[28];
    int8_t scrs[4];
} S3MSAMPLESTRUCT;

typedef struct tagS3MFILEHEADER
{
    int8_t Name[28];
    uint8_t b1A;
    uint8_t type;
    uint16_t reserved1;
    uint16_t ordnum;
    uint16_t insnum;
    uint16_t patnum;
    uint16_t flags;
    uint16_t cwtv;
    uint16_t version;
    uint32_t scrm;			// "SCRM" = 0x4D524353
    uint8_t globalvol;
    uint8_t speed;
    uint8_t tempo;
    uint8_t mastervol;
    uint8_t ultraclicks;
    uint8_t panning_present;
    uint8_t reserved2[8];
    uint16_t special;
    uint8_t channels[32];
} S3MFILEHEADER;

void CSoundFile_S3MConvert(MODCOMMAND * m, int bIT)
//--------------------------------------------------------
{
    unsigned command = m->command;
    unsigned param = m->param;

    switch (command + 0x40) {
	case 'A':
	    command = CMD_SPEED;
	    break;
	case 'B':
	    command = CMD_POSITIONJUMP;
	    break;
	case 'C':
	    command = CMD_PATTERNBREAK;
	    if (!bIT)
		param = (param >> 4) * 10 + (param & 0x0F);
	    break;
	case 'D':
	    command = CMD_VOLUMESLIDE;
	    break;
	case 'E':
	    command = CMD_PORTAMENTODOWN;
	    break;
	case 'F':
	    command = CMD_PORTAMENTOUP;
	    break;
	case 'G':
	    command = CMD_TONEPORTAMENTO;
	    break;
	case 'H':
	    command = CMD_VIBRATO;
	    break;
	case 'I':
	    command = CMD_TREMOR;
	    break;
	case 'J':
	    command = CMD_ARPEGGIO;
	    break;
	case 'K':
	    command = CMD_VIBRATOVOL;
	    break;
	case 'L':
	    command = CMD_TONEPORTAVOL;
	    break;
	case 'M':
	    command = CMD_CHANNELVOLUME;
	    break;
	case 'N':
	    command = CMD_CHANNELVOLSLIDE;
	    break;
	case 'O':
	    command = CMD_OFFSET;
	    break;
	case 'P':
	    command = CMD_PANNINGSLIDE;
	    break;
	case 'Q':
	    command = CMD_RETRIG;
	    break;
	case 'R':
	    command = CMD_TREMOLO;
	    break;
	case 'S':
	    command = CMD_S3MCMDEX;
	    break;
	case 'T':
	    command = CMD_TEMPO;
	    break;
	case 'U':
	    command = CMD_FINEVIBRATO;
	    break;
	case 'V':
	    command = CMD_GLOBALVOLUME;
	    break;
	case 'W':
	    command = CMD_GLOBALVOLSLIDE;
	    break;
	case 'X':
	    command = CMD_PANNING8;
	    break;
	case 'Y':
	    command = CMD_PANBRELLO;
	    break;
	case 'Z':
	    command = CMD_MIDI;
	    break;
	default:
	    command = 0;
    }
    m->command = command;

    m->param = param;
}

void CSoundFile_S3MSaveConvert(CSoundFile const *that, unsigned *pcmd,
    unsigned *pprm, int bIT)
//---------------------------------------------------------------------
{
    unsigned command = *pcmd;
    unsigned param = *pprm;

    switch (command) {
	case CMD_SPEED:
	    command = 'A';
	    break;
	case CMD_POSITIONJUMP:
	    command = 'B';
	    break;
	case CMD_PATTERNBREAK:
	    command = 'C';
	    if (!bIT)
		param = ((param / 10) << 4) + (param % 10);
	    break;
	case CMD_VOLUMESLIDE:
	    command = 'D';
	    break;
	case CMD_PORTAMENTODOWN:
	    command = 'E';
	    if ((param >= 0xE0)
		&& (that->m_nType & (MOD_TYPE_MOD | MOD_TYPE_XM)))
		param = 0xDF;
	    break;
	case CMD_PORTAMENTOUP:
	    command = 'F';
	    if ((param >= 0xE0)
		&& (that->m_nType & (MOD_TYPE_MOD | MOD_TYPE_XM)))
		param = 0xDF;
	    break;
	case CMD_TONEPORTAMENTO:
	    command = 'G';
	    break;
	case CMD_VIBRATO:
	    command = 'H';
	    break;
	case CMD_TREMOR:
	    command = 'I';
	    break;
	case CMD_ARPEGGIO:
	    command = 'J';
	    break;
	case CMD_VIBRATOVOL:
	    command = 'K';
	    break;
	case CMD_TONEPORTAVOL:
	    command = 'L';
	    break;
	case CMD_CHANNELVOLUME:
	    command = 'M';
	    break;
	case CMD_CHANNELVOLSLIDE:
	    command = 'N';
	    break;
	case CMD_OFFSET:
	    command = 'O';
	    break;
	case CMD_PANNINGSLIDE:
	    command = 'P';
	    break;
	case CMD_RETRIG:
	    command = 'Q';
	    break;
	case CMD_TREMOLO:
	    command = 'R';
	    break;
	case CMD_S3MCMDEX:
	    command = 'S';
	    break;
	case CMD_TEMPO:
	    command = 'T';
	    break;
	case CMD_FINEVIBRATO:
	    command = 'U';
	    break;
	case CMD_GLOBALVOLUME:
	    command = 'V';
	    break;
	case CMD_GLOBALVOLSLIDE:
	    command = 'W';
	    break;
	case CMD_PANNING8:
	    command = 'X';
	    if ((bIT) && (that->m_nType != MOD_TYPE_IT)
		&& (that->m_nType != MOD_TYPE_XM)) {
		if (param == 0xA4) {
		    command = 'S';
		    param = 0x91;
		} else if (param <= 0x80) {
		    param <<= 1;
		    if (param > 255)
			param = 255;
		} else
		    command = param = 0;
	    } else if ((!bIT) && ((that->m_nType == MOD_TYPE_IT)
		    || (that->m_nType == MOD_TYPE_XM))) {
		param >>= 1;
	    }
	    break;
	case CMD_PANBRELLO:
	    command = 'Y';
	    break;
	case CMD_MIDI:
	    command = 'Z';
	    break;
	case CMD_XFINEPORTAUPDOWN:
	    if (param & 0x0F)
		switch (param & 0xF0) {
		    case 0x10:
			command = 'F';
			param = (param & 0x0F) | 0xE0;
			break;
		    case 0x20:
			command = 'E';
			param = (param & 0x0F) | 0xE0;
			break;
		    case 0x90:
			command = 'S';
			break;
		    default:
			command = param = 0;
	    } else
		command = param = 0;
	    break;
	case CMD_MODCMDEX:
	    command = 'S';
	    switch (param & 0xF0) {
		case 0x00:
		    command = param = 0;
		    break;
		case 0x10:
		    command = 'F';
		    param |= 0xF0;
		    break;
		case 0x20:
		    command = 'E';
		    param |= 0xF0;
		    break;
		case 0x30:
		    param = (param & 0x0F) | 0x10;
		    break;
		case 0x40:
		    param = (param & 0x0F) | 0x30;
		    break;
		case 0x50:
		    param = (param & 0x0F) | 0x20;
		    break;
		case 0x60:
		    param = (param & 0x0F) | 0xB0;
		    break;
		case 0x70:
		    param = (param & 0x0F) | 0x40;
		    break;
		case 0x90:
		    command = 'Q';
		    param &= 0x0F;
		    break;
		case 0xA0:
		    if (param & 0x0F) {
			command = 'D';
			param = (param << 4) | 0x0F;
		    } else
			command = param = 0;
		    break;
		case 0xB0:
		    if (param & 0x0F) {
			command = 'D';
			param |= 0xF0;
		    } else
			command = param = 0;
		    break;
	    }
	    break;
	default:
	    command = param = 0;
    }
    command &= ~0x40;
    *pcmd = command;
    *pprm = param;
}

int CSoundFile_ReadS3M(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//---------------------------------------------------------------
{
    unsigned insnum, patnum, nins, npat;
    uint32_t insfile[128];
    uint16_t ptr[256];
    uint8_t s[1024];
    uint32_t dwMemPos;
    uint8_t insflags[128], inspack[128];
    S3MFILEHEADER psfh = *(S3MFILEHEADER *) lpStream;
    unsigned i;

    psfh.reserved1 = bswapLE16(psfh.reserved1);
    psfh.ordnum = bswapLE16(psfh.ordnum);
    psfh.insnum = bswapLE16(psfh.insnum);
    psfh.patnum = bswapLE16(psfh.patnum);
    psfh.flags = bswapLE16(psfh.flags);
    psfh.cwtv = bswapLE16(psfh.cwtv);
    psfh.version = bswapLE16(psfh.version);
    psfh.scrm = bswapLE32(psfh.scrm);
    psfh.special = bswapLE16(psfh.special);

    if ((!lpStream)
	|| (dwMemLength <=
	    sizeof(S3MFILEHEADER) + sizeof(S3MSAMPLESTRUCT) + 64))
	return 0;
    if (psfh.scrm != 0x4D524353)
	return 0;
    dwMemPos = 0x60;
    that->m_nType = MOD_TYPE_S3M;
    memset(that->m_szNames, 0, sizeof(that->m_szNames));
    memcpy(that->m_szNames[0], psfh.Name, 28);
    // Speed
    that->m_nDefaultSpeed = psfh.speed;
    if (that->m_nDefaultSpeed < 1)
	that->m_nDefaultSpeed = 6;
    if (that->m_nDefaultSpeed > 0x1F)
	that->m_nDefaultSpeed = 0x1F;
    // Tempo
    that->m_nDefaultTempo = psfh.tempo;
    if (that->m_nDefaultTempo < 40)
	that->m_nDefaultTempo = 40;
    if (that->m_nDefaultTempo > 240)
	that->m_nDefaultTempo = 240;
    // Global Volume
    that->m_nDefaultGlobalVolume = psfh.globalvol << 2;
    if ((!that->m_nDefaultGlobalVolume)
	|| (that->m_nDefaultGlobalVolume > 256))
	that->m_nDefaultGlobalVolume = 256;
    that->m_nSongPreAmp = psfh.mastervol & 0x7F;
    // Channels
    that->m_nChannels = 4;
    for (i = 0; i < 32; i++) {
	that->ChnSettings[i].nPan = 128;
	that->ChnSettings[i].nVolume = 64;

	that->ChnSettings[i].dwFlags = CHN_MUTE;
	if (psfh.channels[i] != 0xFF) {
	    unsigned b;

	    that->m_nChannels = i + 1;
	    b = psfh.channels[i] & 0x0F;

	    that->ChnSettings[i].nPan = (b & 8) ? 0xC0 : 0x40;
	    that->ChnSettings[i].dwFlags = 0;
	}
    }
    if (that->m_nChannels < 4) {
	that->m_nChannels = 4;
    }
    if ((psfh.cwtv < 0x1320) || (psfh.flags & 0x40)) {
	that->m_dwSongFlags |= SONG_FASTVOLSLIDES;
    }

    // Reading pattern order
    i = psfh.ordnum;

    if (i < 1) {
	i = 1;
    }
    if (i > MAX_ORDERS) {
	i = MAX_ORDERS;
    }
    if (i) {
	memcpy(that->Order, lpStream + dwMemPos, i);
	dwMemPos += i;
    }
    if ((i & 1) && (lpStream[dwMemPos] == 0xFF)) {
	dwMemPos++;
    }

    // Reading file pointers
    insnum = nins = psfh.insnum;
    if (insnum >= MAX_SAMPLES)
	insnum = MAX_SAMPLES - 1;
    that->m_nSamples = insnum;
    patnum = npat = psfh.patnum;
    if (patnum > MAX_PATTERNS)
	patnum = MAX_PATTERNS;
    memset(ptr, 0, sizeof(ptr));
    if (nins + npat) {
	unsigned j;

	memcpy(ptr, lpStream + dwMemPos, 2 * (nins + npat));
	dwMemPos += 2 * (nins + npat);
	for (j = 0; j < (nins + npat); ++j) {
	    ptr[j] = bswapLE16(ptr[j]);
	}
	if (psfh.panning_present == 252) {
	    const uint8_t *chnpan = lpStream + dwMemPos;

	    for (j = 0; j < 32; j++)
		if (chnpan[j] & 0x20) {
		    that->ChnSettings[j].nPan = ((chnpan[j] & 0x0F) << 4) + 8;
		}
	}
    }
    if (!that->m_nChannels)
	return 1;
    // Reading instrument headers
    memset(insfile, 0, sizeof(insfile));
    for (i = 1; i <= insnum; i++) {
	unsigned nInd = ((uint32_t) ptr[i - 1]) * 16;

	if ((!nInd) || (nInd + 0x50 > dwMemLength))
	    continue;
	memcpy(s, lpStream + nInd, 0x50);
	memcpy(that->Ins[i].Name, s + 1, 12);
	insflags[i - 1] = s[0x1F];
	inspack[i - 1] = s[0x1E];
	s[0x4C] = 0;
	strcpy(that->m_szNames[i], (const char *)&s[0x30]);
	if ((s[0] == 1) && (s[0x4E] == 'R') && (s[0x4F] == 'S')) {
	    unsigned j = bswapLE32(*((uint32_t *) (s + 0x10)));

	    if (j > MAX_SAMPLE_LENGTH)
		j = MAX_SAMPLE_LENGTH;
	    if (j < 4)
		j = 0;
	    that->Ins[i].nLength = j;
	    j = bswapLE32(*((uint32_t *) (s + 0x14)));
	    if (j >= that->Ins[i].nLength)
		j = that->Ins[i].nLength - 1;
	    that->Ins[i].nLoopStart = j;
	    j = bswapLE32(*((uint32_t *) (s + 0x18)));
	    if (j > MAX_SAMPLE_LENGTH)
		j = MAX_SAMPLE_LENGTH;
	    if (j < 4)
		j = 0;
	    if (j > that->Ins[i].nLength)
		j = that->Ins[i].nLength;
	    that->Ins[i].nLoopEnd = j;
	    j = s[0x1C];
	    if (j > 64)
		j = 64;
	    that->Ins[i].nVolume = j << 2;
	    that->Ins[i].nGlobalVol = 64;
	    if (s[0x1F] & 1)
		that->Ins[i].uFlags |= CHN_LOOP;
	    j = bswapLE32(*((uint32_t *) (s + 0x20)));
	    if (!j)
		j = 8363;
	    if (j < 1024)
		j = 1024;
	    that->Ins[i].nC4Speed = j;
	    insfile[i] =
		((uint32_t) bswapLE16(*((uint16_t *) (s + 0x0E)))) << 4;
	    insfile[i] += ((uint32_t) (uint8_t) s[0x0D]) << 20;
	    if (insfile[i] > dwMemLength)
		insfile[i] &= 0xFFFF;
	    if ((that->Ins[i].nLoopStart >= that->Ins[i].nLoopEnd)
		|| (that->Ins[i].nLoopEnd - that->Ins[i].nLoopStart < 8))
		that->Ins[i].nLoopStart = that->Ins[i].nLoopEnd = 0;
	    that->Ins[i].nPan = 0x80;
	}
    }
    // Reading patterns
    for (i = 0; i < patnum; i++) {
	uint16_t len;
	uint8_t const *src;
	MODCOMMAND *p;
	unsigned row;
	unsigned j;
	unsigned nInd = ((uint32_t) ptr[nins + i]) << 4;

	if (nInd + 0x40 > dwMemLength) {
	    continue;
	}
	len = bswapLE16(*((uint16_t *) (lpStream + nInd)));

	nInd += 2;
	that->PatternSize[i] = 64;
	if ((!len) || (nInd + len > dwMemLength - 6)
	    || ((that->Patterns[i] =
		    CSoundFile_AllocatePattern(64,
			that->m_nChannels)) == NULL)) {
	    continue;
	}
	src = (uint8_t const *) (lpStream + nInd);

	// Unpacking pattern
	p = that->Patterns[i];
	row = 0;
	j = 0;

	while (j < len) {
	    uint8_t b = src[j++];

	    if (!b) {
		if (++row >= 64) {
		    break;
		}
	    } else {
		unsigned chn = b & 0x1F;

		if (chn < that->m_nChannels) {
		    MODCOMMAND *m = &p[row * that->m_nChannels + chn];

		    if (b & 0x20) {
			m->note = src[j++];
			if (m->note < 0xF0)
			    m->note =
				(m->note & 0x0F) + 12 * (m->note >> 4) + 13;
			else if (m->note == 0xFF)
			    m->note = 0;
			m->instr = src[j++];
		    }
		    if (b & 0x40) {
			unsigned vol = src[j++];

			if ((vol >= 128) && (vol <= 192)) {
			    vol -= 128;
			    m->volcmd = VOLCMD_PANNING;
			} else {
			    if (vol > 64)
				vol = 64;
			    m->volcmd = VOLCMD_VOLUME;
			}
			m->vol = vol;
		    }
		    if (b & 0x80) {
			m->command = src[j++];
			m->param = src[j++];
			if (m->command)
			    CSoundFile_S3MConvert(m, 0);
		    }
		} else {
		    if (b & 0x20)
			j += 2;
		    if (b & 0x40)
			j++;
		    if (b & 0x80)
			j += 2;
		}
		if (j >= len)
		    break;
	    }
	}
    }
    // Reading samples
    for (i = 1; i <= insnum; i++)
	if ((that->Ins[i].nLength) && (insfile[i])) {
	    unsigned flags = (psfh.version == 1) ? RS_PCM8S : RS_PCM8U;

	    if (insflags[i - 1] & 4)
		flags += 5;
	    if (insflags[i - 1] & 2)
		flags |= RSF_STEREO;
	    if (inspack[i - 1] == 4)
		flags = RS_ADPCM4;
	    dwMemPos = insfile[i];
	    dwMemPos +=
		CSoundFile_ReadSample(that,&that->Ins[i], flags,
		(char *)(lpStream + dwMemPos), dwMemLength - dwMemPos);
	}
    that->m_nMinPeriod = 64;
    that->m_nMaxPeriod = 32767;
    if (psfh.flags & 0x10) {
	that->m_dwSongFlags |= SONG_AMIGALIMITS;
    }
    return 1;
}

#ifndef MODPLUG_NO_FILESAVE
#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

static uint8_t S3MFiller[16] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
};

int CSoundFile_SaveS3M(CSoundFile *that, const char *lpszFileName,
    unsigned nPacking)
//----------------------------------------------------------
{
    FILE *f;
    uint8_t header[0x60];
    unsigned nbo;
    unsigned nbi;
    unsigned nbp;
    unsigned i;
    uint16_t patptr[128];
    uint16_t insptr[128];
    uint8_t buffer[5 * 1024];
    S3MSAMPLESTRUCT insex[128];
    unsigned ofs;
    unsigned ofs0;
    unsigned ofs1;

    if ((!that->m_nChannels) || (!lpszFileName))
	return 0;
    if ((f = fopen(lpszFileName, "wb")) == NULL)
	return 0;
    // Writing S3M header
    memset(header, 0, sizeof(header));
    memset(insex, 0, sizeof(insex));
    memcpy(header, that->m_szNames[0], 0x1C);
    header[0x1B] = 0;
    header[0x1C] = 0x1A;
    header[0x1D] = 0x10;
    nbo = (CSoundFile_GetNumPatterns(that) + 15) & 0xF0;
    if (!nbo)
	nbo = 16;
    header[0x20] = nbo & 0xFF;
    header[0x21] = nbo >> 8;
    nbi = that->m_nInstruments;
    if (!nbi)
	nbi = that->m_nSamples;
    if (nbi > 99)
	nbi = 99;
    header[0x22] = nbi & 0xFF;
    header[0x23] = nbi >> 8;
    nbp = 0;
    for (i = 0; that->Patterns[i]; i++) {
	nbp = i + 1;
	if (nbp >= MAX_PATTERNS)
	    break;
    }
    for (i = 0; i < MAX_ORDERS; i++)
	if ((that->Order[i] < MAX_PATTERNS) && (that->Order[i] >= nbp))
	    nbp = that->Order[i] + 1;
    header[0x24] = nbp & 0xFF;
    header[0x25] = nbp >> 8;
    if (that->m_dwSongFlags & SONG_FASTVOLSLIDES)
	header[0x26] |= 0x40;
    if ((that->m_nMaxPeriod < 20000)
	|| (that->m_dwSongFlags & SONG_AMIGALIMITS))
	header[0x26] |= 0x10;
    header[0x28] = 0x20;
    header[0x29] = 0x13;
    header[0x2A] = 0x02;		// Version = 1 => Signed samples
    header[0x2B] = 0x00;
    header[0x2C] = 'S';
    header[0x2D] = 'C';
    header[0x2E] = 'R';
    header[0x2F] = 'M';
    header[0x30] = that->m_nDefaultGlobalVolume >> 2;
    header[0x31] = that->m_nDefaultSpeed;
    header[0x32] = that->m_nDefaultTempo;
    header[0x33] = ((that->m_nSongPreAmp < 0x20) ? 0x20 : that->m_nSongPreAmp) | 0x80;	// Stereo
    header[0x35] = 0xFC;
    for (i = 0; i < 32; i++) {
	if (i < that->m_nChannels) {
	    unsigned tmp = (i & 0x0F) >> 1;

	    header[0x40 + i] = (i & 0x10) | ((i & 1) ? 8 + tmp : tmp);
	} else
	    header[0x40 + i] = 0xFF;
    }
    fwrite(header, 0x60, 1, f);
    fwrite(that->Order, nbo, 1, f);
    memset(patptr, 0, sizeof(patptr));
    memset(insptr, 0, sizeof(insptr));
    ofs0 = 0x60 + nbo;
    ofs1 = ((0x60 + nbo + nbi * 2 + nbp * 2 + 15) & 0xFFF0) + 0x20;
    ofs = ofs1;

    for (i = 0; i < nbi; i++)
	insptr[i] = (uint16_t) ((ofs + i * 0x50) / 16);
    for (i = 0; i < nbp; i++)
	patptr[i] = (uint16_t) ((ofs + nbi * 0x50) / 16);
    fwrite(insptr, nbi, 2, f);
    fwrite(patptr, nbp, 2, f);
    if (header[0x35] == 0xFC) {
	uint8_t chnpan[32];

	for (i = 0; i < 32; i++) {
	    chnpan[i] = 0x20 | (that->ChnSettings[i].nPan >> 4);
	}
	fwrite(chnpan, 0x20, 1, f);
    }
    if ((nbi * 2 + nbp * 2) & 0x0F) {
	fwrite(S3MFiller, 0x10 - ((nbi * 2 + nbp * 2) & 0x0F), 1, f);
    }
    ofs1 = ftell(f);
    fwrite(insex, nbi, 0x50, f);
    // Packing patterns
    ofs += nbi * 0x50;
    for (i = 0; i < nbp; i++) {
	uint16_t len = 64;

	memset(buffer, 0, sizeof(buffer));
	patptr[i] = ofs / 16;
	if (that->Patterns[i]) {
	    unsigned row;
	    MODCOMMAND *p = that->Patterns[i];
	    len = 2;

	    for (row = 0; row < 64; row++)
		if (row < that->PatternSize[i]) {
		    unsigned j;

		    for (j = 0; j < that->m_nChannels; j++) {
			unsigned b = j;
			MODCOMMAND *m = &p[row * that->m_nChannels + j];
			unsigned note = m->note;
			unsigned volcmd = m->volcmd;
			unsigned vol = m->vol;
			unsigned command = m->command;
			unsigned param = m->param;

			if ((note) || (m->instr))
			    b |= 0x20;
			if (!note)
			    note = 0xFF;
			else if (note >= 0xFE)
			    note = 0xFE;
			else if (note < 13)
			    note = 0;
			else
			    note -= 13;
			if (note < 0xFE)
			    note = (note % 12) + ((note / 12) << 4);
			if (command == CMD_VOLUME) {
			    command = 0;
			    if (param > 64)
				param = 64;
			    volcmd = VOLCMD_VOLUME;
			    vol = param;
			}
			if (volcmd == VOLCMD_VOLUME)
			    b |= 0x40;
			else if (volcmd == VOLCMD_PANNING) {
			    vol |= 0x80;
			    b |= 0x40;
			}
			if (command) {
			    CSoundFile_S3MSaveConvert(that, &command, &param, 0);
			    if (command)
				b |= 0x80;
			}
			if (b & 0xE0) {
			    buffer[len++] = b;
			    if (b & 0x20) {
				buffer[len++] = note;
				buffer[len++] = m->instr;
			    }
			    if (b & 0x40) {
				buffer[len++] = vol;
			    }
			    if (b & 0x80) {
				buffer[len++] = command;
				buffer[len++] = param;
			    }
			    if (len > sizeof(buffer) - 20)
				break;
			}
		    }
		    buffer[len++] = 0;
		    if (len > sizeof(buffer) - 20)
			break;
		}
	}
	buffer[0] = (len - 2) & 0xFF;
	buffer[1] = (len - 2) >> 8;
	len = (len + 15) & (~0x0F);
	fwrite(buffer, len, 1, f);
	ofs += len;
    }
    // Writing samples
    for (i = 1; i <= nbi; i++) {
	MODINSTRUMENT const *pins = &that->Ins[i];

	if (that->m_nInstruments) {
	    pins = that->Ins;
	    if (that->Headers[i]) {
		unsigned j;

		for (j = 0; j < 128; j++) {
		    unsigned n = that->Headers[i]->Keyboard[j];

		    if ((n) && (n < MAX_INSTRUMENTS)) {
			pins = &that->Ins[n];
			break;
		    }
		}
	    }
	}
	memcpy(insex[i - 1].dosname, pins->Name, 12);
	memcpy(insex[i - 1].Name, that->m_szNames[i], 28);
	memcpy(insex[i - 1].scrs, "SCRS", 4);
	insex[i - 1].hmem = (uint8_t) ((uint32_t) ofs >> 20);
	insex[i - 1].memseg = (uint16_t) ((uint32_t) ofs >> 4);
	if (pins->pSample) {
	    unsigned flags;
	    uint32_t len;

	    insex[i - 1].type = 1;
	    insex[i - 1].length = pins->nLength;
	    insex[i - 1].loopbegin = pins->nLoopStart;
	    insex[i - 1].loopend = pins->nLoopEnd;
	    insex[i - 1].vol = pins->nVolume / 4;
	    insex[i - 1].flags = (pins->uFlags & CHN_LOOP) ? 1 : 0;
	    if (pins->nC4Speed)
		insex[i - 1].finetune = pins->nC4Speed;
	    else
		insex[i - 1].finetune =
		    CSoundFile_TransposeToFrequency(pins->RelativeTone,
		    pins->nFineTune);
	    flags = RS_PCM8U;

#ifndef MODPLUG_NO_PACKING
	    if (nPacking) {
		if ((!(pins->uFlags & (CHN_16BIT | CHN_STEREO)))
		    && (CSoundFile_CanPackSample(that,(char *)pins->pSample,
			    pins->nLength, nPacking, NULL))) {
		    insex[i - 1].pack = 4;
		    flags = RS_ADPCM4;
		}
	    } else
#endif // MODPLUG_NO_PACKING
	    {
		if (pins->uFlags & CHN_16BIT) {
		    insex[i - 1].flags |= 4;
		    flags = RS_PCM16U;
		}
		if (pins->uFlags & CHN_STEREO) {
		    insex[i - 1].flags |= 2;
		    flags =
			(pins->uFlags & CHN_16BIT) ? RS_STPCM16U : RS_STPCM8U;
		}
	    }
	    len = CSoundFile_WriteSample(that, f, pins, flags, 0);

	    if (len & 0x0F) {
		fwrite(S3MFiller, 0x10 - (len & 0x0F), 1, f);
	    }
	    ofs += (len + 15) & (~0x0F);
	} else {
	    insex[i - 1].length = 0;
	}
    }
    // Updating parapointers
    fseek(f, ofs0, SEEK_SET);
    fwrite(insptr, nbi, 2, f);
    fwrite(patptr, nbp, 2, f);
    fseek(f, ofs1, SEEK_SET);
    fwrite(insex, 0x50, nbi, f);
    fclose(f);
    return 1;
}

#ifdef _MSC_VER
#pragma warning(default:4100)
#endif
#endif // MODPLUG_NO_FILESAVE
