/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: load_ams.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

//////////////////////////////////////////////
// AMS module loader			    //
//////////////////////////////////////////////

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

#pragma pack(1)

typedef struct AMSFILEHEADER
{
    int8_t szHeader[7];			// "Extreme"
    uint8_t verlo, verhi;		// 0x??,0x01
    uint8_t chncfg;
    uint8_t samples;
    uint16_t patterns;
    uint16_t orders;
    uint8_t vmidi;
    uint16_t extra;
} AMSFILEHEADER;

typedef struct AMSSAMPLEHEADER
{
    uint32_t length;
    uint32_t loopstart;
    uint32_t loopend;
    uint8_t finetune_and_pan;
    uint16_t samplerate;		// C-2 = 8363
    uint8_t volume;			// 0-127
    uint8_t infobyte;
} AMSSAMPLEHEADER;

#pragma pack()

/////////////////////////////////////////////////////////////////////
// AMS 2.2 loader

#pragma pack(1)

typedef struct AMS2FILEHEADER
{
    uint32_t dwHdr1;			// AMShdr
    uint16_t wHdr2;
    uint8_t b1A;			// 0x1A
    uint8_t titlelen;			// 30-bytes max
    int8_t szTitle[30];			// [titlelen]
} AMS2FILEHEADER;

typedef struct AMS2SONGHEADER
{
    uint16_t version;
    uint8_t instruments;
    uint16_t patterns;
    uint16_t orders;
    uint16_t bpm;
    uint8_t speed;
    uint8_t channels;
    uint8_t commands;
    uint8_t rows;
    uint16_t flags;
} AMS2SONGHEADER;

typedef struct AMS2INSTRUMENT
{
    uint8_t samples;
    uint8_t notemap[120];
} AMS2INSTRUMENT;

typedef struct AMS2ENVELOPE
{
    uint8_t speed;
    uint8_t sustain;
    uint8_t loopbegin;
    uint8_t loopend;
    uint8_t points;
    uint8_t info[3];
} AMS2ENVELOPE;

typedef struct AMS2SAMPLE
{
    uint32_t length;
    uint32_t loopstart;
    uint32_t loopend;
    uint16_t frequency;
    uint8_t finetune;
    uint16_t c4speed;
    int8_t transpose;
    uint8_t volume;
    uint8_t flags;
} AMS2SAMPLE;

#pragma pack()

static int ReadAMS2(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//------------------------------------------------------------
{
    AMS2FILEHEADER *pfh = (AMS2FILEHEADER *) lpStream;
    AMS2SONGHEADER *psh;
    uint32_t dwMemPos;
    uint8_t smpmap[16];
    uint8_t packedsamples[MAX_SAMPLES];
    unsigned i;
    unsigned j;

    if ((pfh->dwHdr1 != 0x68534D41) || (pfh->wHdr2 != 0x7264)
	|| (pfh->b1A != 0x1A) || (pfh->titlelen > 30)) {
	return 0;
    }
    dwMemPos = pfh->titlelen + 8;
    psh = (AMS2SONGHEADER *) (lpStream + dwMemPos);
    if (((psh->version & 0xFF00) != 0x0200) || (!psh->instruments)
	|| (psh->instruments > MAX_INSTRUMENTS) || (!psh->patterns)
	|| (!psh->orders)) {
	return 0;
    }
    dwMemPos += sizeof(AMS2SONGHEADER);
    if (pfh->titlelen) {
	memcpy(that->m_szNames, pfh->szTitle, pfh->titlelen);
	that->m_szNames[0][pfh->titlelen] = 0;
    }
    that->m_nType = MOD_TYPE_AMS;
    that->m_nChannels = 32;
    that->m_nDefaultTempo = psh->bpm >> 8;
    that->m_nDefaultSpeed = psh->speed;
    that->m_nInstruments = psh->instruments;
    that->m_nSamples = 0;
    if (psh->flags & 0x40) {
	that->m_dwSongFlags |= SONG_LINEARSLIDES;
    }
    for (i = 1; i <= that->m_nInstruments; i++) {
	unsigned insnamelen;
	int8_t *pinsname;
	AMS2INSTRUMENT *pins;
	AMS2ENVELOPE *volenv;
	AMS2ENVELOPE *panenv;
	AMS2ENVELOPE *pitchenv;
	INSTRUMENTHEADER *penv;
	unsigned pos;

	pins = (AMS2INSTRUMENT *) (lpStream + dwMemPos);
	insnamelen = lpStream[dwMemPos];
	pinsname = (int8_t *) (lpStream + dwMemPos + 1);
	dwMemPos += insnamelen + 1;

	dwMemPos += sizeof(AMS2INSTRUMENT);
	if (dwMemPos + 1024 >= dwMemLength) {
	    return 1;
	}

	volenv = (AMS2ENVELOPE *) (lpStream + dwMemPos);
	dwMemPos += 5 + volenv->points * 3;
	panenv = (AMS2ENVELOPE *) (lpStream + dwMemPos);
	dwMemPos += 5 + panenv->points * 3;
	pitchenv = (AMS2ENVELOPE *) (lpStream + dwMemPos);
	dwMemPos += 5 + pitchenv->points * 3;

	penv = malloc(sizeof(INSTRUMENTHEADER));
	if (!penv) {
	    return 1;
	}

	memset(penv, 0, sizeof(INSTRUMENTHEADER));
	memset(smpmap, 0, sizeof(smpmap));
	for (j = 0; j < pins->samples; j++) {
	    if ((j >= 16) || (that->m_nSamples + 1 >= MAX_SAMPLES)) {
		break;
	    }
	    that->m_nSamples++;
	    smpmap[j] = that->m_nSamples;
	}
	penv->nGlobalVol = 64;
	penv->nPan = 128;
	penv->nPPC = 60;
	that->Headers[i] = penv;
	if (insnamelen) {
	    if (insnamelen > 31) {
		insnamelen = 31;
	    }
	    memcpy(penv->Name, pinsname, insnamelen);
	    penv->Name[insnamelen] = 0;
	}
	for (j = 0; j < 120; j++) {
	    penv->NoteMap[j] = j + 1;
	    penv->Keyboard[j] = smpmap[pins->notemap[j] & 0x0F];
	}

	// Volume Envelope
	pos = 0;

	penv->nVolEnv = (volenv->points > 16) ? 16 : volenv->points;
	penv->nVolSustainBegin = penv->nVolSustainEnd = volenv->sustain;
	penv->nVolLoopStart = volenv->loopbegin;
	penv->nVolLoopEnd = volenv->loopend;
	for (j = 0; j < penv->nVolEnv; j++) {
	    penv->VolEnv[j] =
		(uint8_t) ((volenv->info[j * 3 + 2] & 0x7F) >> 1);
	    pos +=
		volenv->info[j * 3] + ((volenv->info[j * 3 + 1] & 1) << 8);
	    penv->VolPoints[j] = (uint16_t) pos;
	}
	penv->nFadeOut =
	    (((lpStream[dwMemPos + 2] & 0x0F) << 8) | (lpStream[dwMemPos +
		    1])) << 3;

	j = lpStream[dwMemPos + 3];
	if (j & 0x01) {
	    penv->dwFlags |= ENV_VOLLOOP;
	}
	if (j & 0x02) {
	    penv->dwFlags |= ENV_VOLSUSTAIN;
	}
	if (j & 0x04) {
	    penv->dwFlags |= ENV_VOLUME;
	}
	dwMemPos += 5;
	// Read Samples
	for (j = 0; j < pins->samples; j++) {
	    MODINSTRUMENT *psmp = ((j < 16)
		&& (smpmap[j])) ? &that->Ins[smpmap[j]] : NULL;
	    unsigned smpnamelen = lpStream[dwMemPos];

	    if ((psmp) && (smpnamelen) && (smpnamelen <= 22)) {
		memcpy(that->m_szNames[smpmap[j]], lpStream + dwMemPos + 1,
		    smpnamelen);
	    }
	    dwMemPos += smpnamelen + 1;
	    if (psmp) {
		AMS2SAMPLE *pams = (AMS2SAMPLE *) (lpStream + dwMemPos);

		psmp->nGlobalVol = 64;
		psmp->nPan = 128;
		psmp->nLength = pams->length;
		psmp->nLoopStart = pams->loopstart;
		psmp->nLoopEnd = pams->loopend;
		psmp->nC4Speed = pams->c4speed;
		psmp->RelativeTone = pams->transpose;
		psmp->nVolume = pams->volume / 2;
		packedsamples[smpmap[j]] = pams->flags;
		if (pams->flags & 0x04)
		    psmp->uFlags |= CHN_16BIT;
		if (pams->flags & 0x08)
		    psmp->uFlags |= CHN_LOOP;
		if (pams->flags & 0x10)
		    psmp->uFlags |= CHN_PINGPONGLOOP;
	    }
	    dwMemPos += sizeof(AMS2SAMPLE);
	}
    }
    if (dwMemPos + 256 >= dwMemLength) {
	return 1;
    }

    // Comments
    {
	unsigned composernamelen = lpStream[dwMemPos];

	if (composernamelen) {
	    that->m_lpszSongComments = malloc(composernamelen + 1);
	    if (that->m_lpszSongComments) {
		memcpy(that->m_lpszSongComments, lpStream + dwMemPos + 1,
		    composernamelen);
		that->m_lpszSongComments[composernamelen] = '\0';
	    }
	}
	dwMemPos += composernamelen + 1;
	// channel names
	for (j = 0; j < 32; j++) {
	    unsigned chnnamlen = lpStream[dwMemPos];

	    if ((chnnamlen) && (chnnamlen < MAX_CHANNELNAME)) {
		memcpy(that->ChnSettings[j].szName, lpStream + dwMemPos + 1,
		    chnnamlen);
	    }
	    dwMemPos += chnnamlen + 1;
	    if (dwMemPos + chnnamlen + 256 >= dwMemLength) {
		return 1;
	    }
	}
	// packed comments (ignored) songtextlen
	j  = *((uint32_t *) (lpStream + dwMemPos));

	dwMemPos += j;
	if (dwMemPos + 256 >= dwMemLength) {
	    return 1;
	}
    }

    // Order List
    for (i = 0; i < MAX_ORDERS; i++) {
	that->Order[i] = 0xFF;
	if (dwMemPos + 2 >= dwMemLength) {
	    return 1;
	}
	if (i < psh->orders) {
	    that->Order[i] = lpStream[dwMemPos];
	    dwMemPos += 2;
	}
    }

    // Pattern Data
    for (i = 0; i < psh->patterns; i++) {
	unsigned packedlen;
	unsigned numrows;
	unsigned patnamlen;

	if (dwMemPos + 8 >= dwMemLength) {
	    return 1;
	}
	packedlen = *((uint32_t *) (lpStream + dwMemPos));
	numrows = 1 + (unsigned)(lpStream[dwMemPos + 4]);
	patnamlen = lpStream[dwMemPos + 6];

	dwMemPos += 4;
	if ((i < MAX_PATTERNS) && (packedlen < dwMemLength - dwMemPos)
	    && (numrows >= 8)) {
	    const uint8_t *psrc;
	    unsigned pos;
	    unsigned row;

	    if ((patnamlen) && (patnamlen < MAX_PATTERNNAME)) {
		char s[MAX_PATTERNNAME];	// changed from CHAR

		memcpy(s, lpStream + dwMemPos + 3, patnamlen);
		s[patnamlen] = 0;
		CSoundFile_SetPatternName(that,i, s);
	    }
	    that->PatternSize[i] = numrows;
	    that->Patterns[i] =
		CSoundFile_AllocatePattern(numrows, that->m_nChannels);
	    if (!that->Patterns[i]) {
		return 1;
	    }
	    // Unpack Pattern Data
	    psrc = lpStream + dwMemPos;
	    pos = 3 + patnamlen;
	    row = 0;

	    while ((pos < packedlen) && (row < numrows)) {
		MODCOMMAND *m = that->Patterns[i] + row * that->m_nChannels;
		unsigned byte1 = psrc[pos++];
		unsigned ch = byte1 & 0x1F;

		// Read Note + Instr
		if (!(byte1 & 0x40)) {
		    unsigned byte2 = psrc[pos++];
		    unsigned note = byte2 & 0x7F;

		    if (note)
			m[ch].note = (note > 1) ? (note - 1) : 0xFF;
		    m[ch].instr = psrc[pos++];
		    // Read Effect
		    while (byte2 & 0x80) {
			byte2 = psrc[pos++];
			if (byte2 & 0x40) {
			    m[ch].volcmd = VOLCMD_VOLUME;
			    m[ch].vol = byte2 & 0x3F;
			} else {
			    unsigned command = byte2 & 0x3F;
			    unsigned param = psrc[pos++];

			    if (command == 0x0C) {
				m[ch].volcmd = VOLCMD_VOLUME;
				m[ch].vol = param / 2;
			    } else if (command < 0x10) {
				m[ch].command = command;
				m[ch].param = param;
				CSoundFile_ConvertModCommand(that, &m[ch]);
			    } else {
				// TODO: AMS effects
			    }
			}
		    }
		}
		if (byte1 & 0x80) {
		    row++;
		}
	    }
	}
	dwMemPos += packedlen;
    }

    // Read Samples
    for (i = 1; i <= that->m_nSamples; i++) {
	if (that->Ins[i].nLength) {
	    unsigned flags;

	    if (dwMemPos >= dwMemLength - 9) {
		return 1;
	    }

	    if (packedsamples[i] & 0x03) {
		flags =
		    (that->Ins[i].uFlags & CHN_16BIT) ? RS_AMS16 : RS_AMS8;
	    } else {
		flags =
		    (that->Ins[i].
		    uFlags & CHN_16BIT) ? RS_PCM16S : RS_PCM8S;
	    }
	    dwMemPos +=
		CSoundFile_ReadSample(that,&that->Ins[i], flags,
		(char *)(lpStream + dwMemPos), dwMemLength - dwMemPos);
	}
    }
    return 1;
}

/////////////////////////////////////////////////////////////////////
// AMS loader

int CSoundFile_ReadAMS(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//-----------------------------------------------------------
{
    uint8_t pkinf[MAX_SAMPLES];
    AMSFILEHEADER *pfh = (AMSFILEHEADER *) lpStream;
    uint32_t dwMemPos;
    unsigned tmp, tmp2;
    unsigned i;

    if ((!lpStream) || (dwMemLength < 1024)) {
	return 0;
    }
    if ((pfh->verhi != 0x01) || (strncmp(pfh->szHeader, "Extreme", 7))
	|| (!pfh->patterns) || (!pfh->orders) || (!pfh->samples)
	|| (pfh->samples > MAX_SAMPLES)
	|| (pfh->patterns > MAX_PATTERNS) || (pfh->orders > MAX_ORDERS)) {
	return ReadAMS2(that, lpStream, dwMemLength);
    }
    dwMemPos = sizeof(AMSFILEHEADER) + pfh->extra;
    if (dwMemPos + pfh->samples * sizeof(AMSSAMPLEHEADER) + 256 >= dwMemLength)
	return 0;
    that->m_nType = MOD_TYPE_AMS;
    that->m_nInstruments = 0;
    that->m_nChannels = (pfh->chncfg & 0x1F) + 1;
    that->m_nSamples = pfh->samples;
    for (i = 1; i <= that->m_nSamples;
	i++, dwMemPos += sizeof(AMSSAMPLEHEADER)) {
	AMSSAMPLEHEADER *psh = (AMSSAMPLEHEADER *) (lpStream + dwMemPos);
	MODINSTRUMENT *pins = &that->Ins[i];

	pins->nLength = psh->length;
	pins->nLoopStart = psh->loopstart;
	pins->nLoopEnd = psh->loopend;
	pins->nGlobalVol = 64;
	pins->nVolume = psh->volume << 1;
	pins->nC4Speed = psh->samplerate;
	pins->nPan = (psh->finetune_and_pan & 0xF0);
	if (pins->nPan < 0x80)
	    pins->nPan += 0x10;
	pins->nFineTune = MOD2XMFineTune(psh->finetune_and_pan & 0x0F);
	pins->uFlags = (psh->infobyte & 0x80) ? CHN_16BIT : 0;
	if ((pins->nLoopEnd <= pins->nLength)
	    && (pins->nLoopStart + 4 <= pins->nLoopEnd))
	    pins->uFlags |= CHN_LOOP;
	pkinf[i] = psh->infobyte;
    }

    // Read Song Name
    tmp = lpStream[dwMemPos++];
    if (dwMemPos + tmp + 1 >= dwMemLength) {
	return 1;
    }
    tmp2 = (tmp < 32) ? tmp : 31;
    if (tmp2) {
	memcpy(that->m_szNames[0], lpStream + dwMemPos, tmp2);
    }
    that->m_szNames[0][tmp2] = 0;
    dwMemPos += tmp;
    // Read sample names
    for (i = 1; i <= that->m_nSamples; i++) {
	if (dwMemPos + 32 >= dwMemLength) {
	    return 1;
	}
	tmp = lpStream[dwMemPos++];
	tmp2 = (tmp < 32) ? tmp : 31;
	if (tmp2) {
	    memcpy(that->m_szNames[i], lpStream + dwMemPos, tmp2);
	}
	dwMemPos += tmp;
    }
    // Skip Channel names
    for (i = 0; i < that->m_nChannels; i++) {
	if (dwMemPos + 32 >= dwMemLength) {
	    return 1;
	}
	tmp = lpStream[dwMemPos++];
	dwMemPos += tmp;
    }

    // Read Pattern Names
    that->m_lpszPatternNames = malloc(sizeof(*that->m_lpszPatternNames)
	* pfh->patterns * 32);
    if (!that->m_lpszPatternNames) {
	return 1;
    }
    memset(that->m_lpszPatternNames, 0, pfh->patterns * 32);
    that->m_nPatternNames = pfh->patterns;

    for (i = 0; i < that->m_nPatternNames; i++) {
	if (dwMemPos + 32 >= dwMemLength) {
	    return 1;
	}
	tmp = lpStream[dwMemPos++];
	tmp2 = (tmp < 32) ? tmp : 31;
	if (tmp2) {
	    memcpy(that->m_lpszPatternNames + i * 32, lpStream + dwMemPos,
		tmp2);
	}
	dwMemPos += tmp;
    }

    // Read Song Comments
    tmp = *((uint16_t *) (lpStream + dwMemPos));
    dwMemPos += 2;
    if (dwMemPos + tmp >= dwMemLength) {
	return 1;
    }
    if (tmp) {
	that->m_lpszSongComments = malloc(tmp + 1);
	if (!that->m_lpszSongComments) {
	    return 1;
	}
	memcpy(that->m_lpszSongComments, lpStream + dwMemPos, tmp);
	that->m_lpszSongComments[tmp] = '\0';;
	dwMemPos += tmp;
    }

    // Read Order List
    for (i = 0; i < pfh->orders; i++, dwMemPos += 2) {
	unsigned n = *((uint16_t *) (lpStream + dwMemPos));

	that->Order[i] = (uint8_t) n;
    }

    // Read Patterns
    for (i = 0; i < pfh->patterns; i++) {
	unsigned len;
	MODCOMMAND *m;
	const uint8_t *p;
	unsigned row;
	unsigned j;

	if (dwMemPos + 4 >= dwMemLength) {
	    return 1;
	}
	len = *((uint32_t *) (lpStream + dwMemPos));

	dwMemPos += 4;
	if ((len >= dwMemLength) || (dwMemPos + len > dwMemLength)) {
	    return 1;
	}
	that->PatternSize[i] = 64;
	m = CSoundFile_AllocatePattern(that->PatternSize[i], that->m_nChannels);
	if (!m) {
	    return 1;
	}
	that->Patterns[i] = m;

	p = lpStream + dwMemPos;
	row = 0;
	j = 0;
	while ((row < that->PatternSize[i]) && (j + 2 < len)) {
	    uint8_t b0 = p[j++];
	    uint8_t b1 = p[j++];
	    uint8_t b2 = 0;
	    unsigned ch = b0 & 0x3F;

	    // Note+Instr
	    if (!(b0 & 0x40)) {
		b2 = p[j++];
		if (ch < that->m_nChannels) {
		    if (b1 & 0x7F)
			m[ch].note = (b1 & 0x7F) + 25;
		    m[ch].instr = b2;
		}
		if (b1 & 0x80) {
		    b0 |= 0x40;
		    b1 = p[j++];
		}
	    }
	    // Effect
	    if (b0 & 0x40) {
anothercommand:
		if (b1 & 0x40) {
		    if (ch < that->m_nChannels) {
			m[ch].volcmd = VOLCMD_VOLUME;
			m[ch].vol = b1 & 0x3F;
		    }
		} else {
		    b2 = p[j++];
		    if (ch < that->m_nChannels) {
			unsigned cmd = b1 & 0x3F;

			if (cmd == 0x0C) {
			    m[ch].volcmd = VOLCMD_VOLUME;
			    m[ch].vol = b2 >> 1;
			} else if (cmd == 0x0E) {
			    if (!m[ch].command) {
				unsigned command = CMD_S3MCMDEX;
				unsigned param = b2;

				switch (param & 0xF0) {
				    case 0x00:
					if (param & 0x08) {
					    param &= 0x07;
					    param |= 0x90;
					} else {
					    command = param = 0;
					}
					break;
				    case 0x10:
					command = CMD_PORTAMENTOUP;
					param |= 0xF0;
					break;
				    case 0x20:
					command = CMD_PORTAMENTODOWN;
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
					command = CMD_RETRIG;
					param &= 0x0F;
					break;
				    case 0xA0:
					if (param & 0x0F) {
					    command = CMD_VOLUMESLIDE;
					    param = (param << 4) | 0x0F;
					} else
					    command = param = 0;
					break;
				    case 0xB0:
					if (param & 0x0F) {
					    command = CMD_VOLUMESLIDE;
					    param |= 0xF0;
					} else
					    command = param = 0;
					break;
				}
				m[ch].command = command;
				m[ch].param = param;
			    }
			} else {
			    m[ch].command = cmd;
			    m[ch].param = b2;
			    CSoundFile_ConvertModCommand(that, &m[ch]);
			}
		    }
		}
		if (b1 & 0x80) {
		    b1 = p[j++];
		    if (j <= len) {
			goto anothercommand;
		    }
		}
	    }
	    if (b0 & 0x80) {
		row++;
		m += that->m_nChannels;
	    }
	}
	dwMemPos += len;
    }

    // Read Samples
    for (i = 1; i <= that->m_nSamples; i++) {
	if (that->Ins[i].nLength) {
	    unsigned flags;

	    if (dwMemPos >= dwMemLength - 9) {
		return 1;
	    }
	    flags = (that->Ins[i].uFlags & CHN_16BIT) ? RS_AMS16 : RS_AMS8;
	    dwMemPos +=
		CSoundFile_ReadSample(that,&that->Ins[i], flags,
		(char *)(lpStream + dwMemPos), dwMemLength - dwMemPos);
	}
    }
    return 1;
}

/////////////////////////////////////////////////////////////////////
// AMS Sample unpacking

void CSoundFile_AMSUnpack(const char *psrc, unsigned inputlen, char *pdest,
    unsigned dmax, char packcharacter)
{
    unsigned tmplen;
    int8_t *amstmp;

    tmplen = dmax;
    amstmp = alloca(sizeof(int8_t)*tmplen);
    if (!amstmp) {
	return;
    }

    // Unpack Loop
    {
	int8_t *p = amstmp;
	unsigned i = 0, j = 0;

	while ((i < inputlen) && (j < tmplen)) {
	    int8_t ch = psrc[i++];

	    if (ch == packcharacter) {
		uint8_t ch2 = psrc[i++];

		if (ch2) {
		    ch = psrc[i++];
		    while (ch2--) {
			p[j++] = ch;
			if (j >= tmplen)
			    break;
		    }
		} else
		    p[j++] = packcharacter;
	    } else
		p[j++] = ch;
	}
    }
    // Bit Unpack Loop
    {
	int8_t *p = amstmp;
	unsigned bitcount = 0x80;
	unsigned dh;
	unsigned k;
	unsigned i;

	for (k = i = 0; i < dmax; i++) {
	    uint8_t al = *p++;
	    unsigned count;

	    dh = 0;
	    for (count = 0; count < 8; count++) {
		unsigned bl = al & bitcount;

		bl = ((bl | (bl << 8)) >> ((dh + 8 - count) & 7)) & 0xFF;
		bitcount = ((bitcount | (bitcount << 8)) >> 1) & 0xFF;
		pdest[k++] |= bl;
		if (k >= dmax) {
		    k = 0;
		    dh++;
		}
	    }
	    bitcount = ((bitcount | (bitcount << 8)) >> dh) & 0xFF;
	}
    }
    // Delta Unpack
    {
	int8_t old;
	unsigned i;

	for (old = i = 0; i < dmax; i++) {
	    int pos = ((uint8_t *) pdest)[i];

	    if ((pos != 128) && (pos & 0x80))
		pos = -(pos & 0x7F);
	    old -= (int8_t) pos;
	    pdest[i] = old;
	}
    }
}
