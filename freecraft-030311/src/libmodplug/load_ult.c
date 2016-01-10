/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: load_ult.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

#define ULT_16BIT   0x04
#define ULT_LOOP    0x08
#define ULT_BIDI    0x10

#pragma pack(1)

// Raw ULT header struct:
typedef struct tagULTHEADER
{
    char id[15];			// changed from CHAR
    char songtitle[32];			// changed from CHAR
    uint8_t reserved;
} ULTHEADER;

// Raw ULT sampleinfo struct:
typedef struct tagULTSAMPLE
{
    int8_t samplename[32];
    int8_t dosname[12];
    long loopstart;
    long loopend;
    long sizestart;
    long sizeend;
    uint8_t volume;
    uint8_t flags;
    uint16_t finetune;
} ULTSAMPLE;

#pragma pack()

int CSoundFile_ReadUlt(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//---------------------------------------------------------------
{
    ULTHEADER *pmh = (ULTHEADER *) lpStream;
    ULTSAMPLE *pus;
    unsigned nos, nop;
    uint32_t dwMemPos;
    unsigned i;
    unsigned smpsize;

    // try to read module header
    if (!lpStream || dwMemLength < 0x100) {
	return 0;
    }
    if (strncmp(pmh->id, "MAS_UTrack_V00", 14)) {
	return 0;
    }
    // Warning! Not supported ULT format, trying anyway
    // if ((pmh->id[14] < '1') || (pmh->id[14] > '4')) return 0;
    that->m_nType = MOD_TYPE_ULT;
    that->m_nDefaultSpeed = 6;
    that->m_nDefaultTempo = 125;
    memcpy(that->m_szNames[0], pmh->songtitle, 32);
    // read songtext
    dwMemPos = sizeof(ULTHEADER);
    if ((pmh->reserved) && (dwMemPos + pmh->reserved * 32 < dwMemLength)) {
	unsigned len = pmh->reserved * 32;
	unsigned l;

	that->m_lpszSongComments = malloc(len + 1 + pmh->reserved);

	if (that->m_lpszSongComments) {
	    for (l = 0; l < pmh->reserved; l++) {
		memcpy(that->m_lpszSongComments + l * 33,
		    lpStream + dwMemPos + l * 32, 32);
		that->m_lpszSongComments[l * 33 + 32] = 0x0D;
	    }
	    that->m_lpszSongComments[len] = 0;
	}
	dwMemPos += len;
    }
    if (dwMemPos >= dwMemLength)
	return 1;
    nos = lpStream[dwMemPos++];
    that->m_nSamples = nos;
    if (that->m_nSamples >= MAX_SAMPLES) {
	that->m_nSamples = MAX_SAMPLES - 1;
    }
    smpsize = 64;

    if (pmh->id[14] >= '4') {
	smpsize += 2;
    }
    if (dwMemPos + nos * smpsize + 256 + 2 > dwMemLength) {
	return 1;
    }
    for (i = 1; i <= nos; i++, dwMemPos += smpsize) {
	if (i <= that->m_nSamples) {
	    MODINSTRUMENT *pins;

	    pus = (ULTSAMPLE *) (lpStream + dwMemPos);
	    pins = &that->Ins[i];

	    memcpy(that->m_szNames[i], pus->samplename, 32);
	    memcpy(pins->Name, pus->dosname, 12);
	    pins->nLoopStart = pus->loopstart;
	    pins->nLoopEnd = pus->loopend;
	    pins->nLength = pus->sizeend - pus->sizestart;
	    pins->nVolume = pus->volume;
	    pins->nGlobalVol = 64;
	    pins->nC4Speed = 8363;
	    if (pmh->id[14] >= '4') {
		pins->nC4Speed = pus->finetune;
	    }
	    if (pus->flags & ULT_LOOP)
		pins->uFlags |= CHN_LOOP;
	    if (pus->flags & ULT_BIDI)
		pins->uFlags |= CHN_PINGPONGLOOP;
	    if (pus->flags & ULT_16BIT) {
		pins->uFlags |= CHN_16BIT;
		pins->nLoopStart >>= 1;
		pins->nLoopEnd >>= 1;
	    }
	}
    }
    memcpy(that->Order, lpStream + dwMemPos, 256);
    dwMemPos += 256;
    that->m_nChannels = lpStream[dwMemPos] + 1;
    nop = lpStream[dwMemPos + 1] + 1;
    dwMemPos += 2;
    if (that->m_nChannels > 32)
	that->m_nChannels = 32;
    // Default channel settings
    for (i = 0; i < that->m_nChannels; i++) {
	that->ChnSettings[i].nVolume = 64;
	that->ChnSettings[i].nPan = (i & 1) ? 0x40 : 0xC0;
    }
    // read pan position table for v1.5 and higher
    if (pmh->id[14] >= '3') {
	if (dwMemPos + that->m_nChannels > dwMemLength)
	    return 1;
	for (i = 0; i < that->m_nChannels; i++) {
	    that->ChnSettings[i].nPan = (lpStream[dwMemPos++] << 4) + 8;
	    if (that->ChnSettings[i].nPan > 256) {
		that->ChnSettings[i].nPan = 256;
	    }
	}
    }
    // Allocating Patterns
    for (i = 0; i < nop; i++) {
	if (i < MAX_PATTERNS) {
	    that->PatternSize[i] = 64;
	    that->Patterns[i] =
		CSoundFile_AllocatePattern(64, that->m_nChannels);
	}
    }
    // Reading Patterns
    for (i = 0; i < that->m_nChannels; i++) {
	unsigned nPat;

	for (nPat = 0; nPat < nop; nPat++) {
	    unsigned row;
	    MODCOMMAND *pat = NULL;

	    if (nPat < MAX_PATTERNS) {
		pat = that->Patterns[nPat];
		if (pat) {
		    pat += i;
		}
	    }

	    row = 0;
	    while (row < 64) {
		unsigned rep;
		unsigned note;
		unsigned instr;
		unsigned eff;
		unsigned dat1;
		unsigned dat2;
		unsigned cmd1;
		unsigned cmd2;


		if (dwMemPos + 6 > dwMemLength) {
		    return 1;
		}
		rep = 1;
		note = lpStream[dwMemPos++];

		if (note == 0xFC) {
		    rep = lpStream[dwMemPos];
		    note = lpStream[dwMemPos + 1];
		    dwMemPos += 2;
		}
		instr = lpStream[dwMemPos++];
		eff = lpStream[dwMemPos++];
		dat1 = lpStream[dwMemPos++];
		dat2 = lpStream[dwMemPos++];
		cmd1 = eff & 0x0F;
		cmd2 = eff >> 4;

		if (cmd1 == 0x0C)
		    dat1 >>= 2;
		else if (cmd1 == 0x0B) {
		    cmd1 = dat1 = 0;
		}
		if (cmd2 == 0x0C)
		    dat2 >>= 2;
		else if (cmd2 == 0x0B) {
		    cmd2 = dat2 = 0;
		}
		while ((rep != 0) && (row < 64)) {
		    if (pat) {
			pat->instr = instr;
			if (note)
			    pat->note = note + 36;
			if (cmd1 | dat1) {
			    if (cmd1 == 0x0C) {
				pat->volcmd = VOLCMD_VOLUME;
				pat->vol = dat1;
			    } else {
				pat->command = cmd1;
				pat->param = dat1;
				CSoundFile_ConvertModCommand(that, pat);
			    }
			}
			if (cmd2 == 0x0C) {
			    pat->volcmd = VOLCMD_VOLUME;
			    pat->vol = dat2;
			} else if ((cmd2 | dat2) && (!pat->command)) {
			    pat->command = cmd2;
			    pat->param = dat2;
			    CSoundFile_ConvertModCommand(that, pat);
			}
			pat += that->m_nChannels;
		    }
		    row++;
		    rep--;
		}
	    }
	}
    }
    // Reading Instruments
    for (i = 1; i <= that->m_nSamples; i++) {
	if (that->Ins[i].nLength) {
	    unsigned flags;
	    if (dwMemPos >= dwMemLength) {
		return 1;
	    }
	    flags =
		(that->Ins[i].uFlags & CHN_16BIT) ? RS_PCM16S : RS_PCM8S;
	    dwMemPos +=
		CSoundFile_ReadSample(that,&that->Ins[i], flags,
		(char *)(lpStream + dwMemPos), dwMemLength - dwMemPos);
	}
    }
    return 1;
}
