/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: load_mtm.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

//////////////////////////////////////////////////////////
// MTM file support (import only)

#pragma pack(1)

typedef struct tagMTMSAMPLE
{
    char samplename[22];		// changed from CHAR
    uint32_t length;
    uint32_t reppos;
    uint32_t repend;
    int8_t finetune;
    uint8_t volume;
    uint8_t attribute;
} MTMSAMPLE;

typedef struct tagMTMHEADER
{
    char id[4];				// MTM file marker + version // changed from CHAR
    char songname[20];			// ASCIIZ songname  // changed from CHAR
    uint16_t numtracks;			// number of tracks saved
    uint8_t lastpattern;		// last pattern number saved
    uint8_t lastorder;			// last order number to play (songlength-1)
    uint16_t commentsize;		// length of comment field
    uint8_t numsamples;			// number of samples saved
    uint8_t attribute;			// attribute byte (unused)
    uint8_t beatspertrack;
    uint8_t numchannels;		// number of channels used
    uint8_t panpos[32];			// voice pan positions
} MTMHEADER;

#pragma pack()

int CSoundFile_ReadMTM(CSoundFile * that, const uint8_t * lpStream, uint32_t dwMemLength)
//-----------------------------------------------------------
{
    MTMHEADER *pmh = (MTMHEADER *) lpStream;
    uint32_t dwMemPos = 66;
    unsigned i;
    unsigned pat;
    const uint8_t *pTracks;
    const uint16_t *pSeq;

    if ((!lpStream) || (dwMemLength < 0x100))
	return 0;
    if ((strncmp(pmh->id, "MTM", 3)) || (pmh->numchannels > 32)
	|| (pmh->numsamples >= MAX_SAMPLES) || (!pmh->numsamples)
	|| (!pmh->numtracks) || (!pmh->numchannels)
	|| (!pmh->lastpattern) || (pmh->lastpattern > MAX_PATTERNS))
	return 0;
    strncpy(that->m_szNames[0], pmh->songname, 20);
    that->m_szNames[0][20] = 0;
    if (dwMemPos + 37 * pmh->numsamples + 128 + 192 * pmh->numtracks +
	64 * (pmh->lastpattern + 1) + pmh->commentsize >= dwMemLength)
	return 0;
    that->m_nType = MOD_TYPE_MTM;
    that->m_nSamples = pmh->numsamples;
    that->m_nChannels = pmh->numchannels;
    // Reading instruments
    for (i = 1; i <= that->m_nSamples; i++) {
	uint32_t len;
	MTMSAMPLE *pms = (MTMSAMPLE *) (lpStream + dwMemPos);

	strncpy(that->m_szNames[i], pms->samplename, 22);
	that->m_szNames[i][22] = 0;
	that->Ins[i].nVolume = pms->volume << 2;
	that->Ins[i].nGlobalVol = 64;
	len = pms->length;

	if ((len > 4) && (len <= MAX_SAMPLE_LENGTH)) {
	    that->Ins[i].nLength = len;
	    that->Ins[i].nLoopStart = pms->reppos;
	    that->Ins[i].nLoopEnd = pms->repend;
	    if (that->Ins[i].nLoopEnd > that->Ins[i].nLength)
		that->Ins[i].nLoopEnd = that->Ins[i].nLength;
	    if (that->Ins[i].nLoopStart + 4 >= that->Ins[i].nLoopEnd)
		that->Ins[i].nLoopStart = that->Ins[i].nLoopEnd = 0;
	    if (that->Ins[i].nLoopEnd)
		that->Ins[i].uFlags |= CHN_LOOP;
	    that->Ins[i].nFineTune = MOD2XMFineTune(pms->finetune);
	    if (pms->attribute & 0x01) {
		that->Ins[i].uFlags |= CHN_16BIT;
		that->Ins[i].nLength >>= 1;
		that->Ins[i].nLoopStart >>= 1;
		that->Ins[i].nLoopEnd >>= 1;
	    }
	    that->Ins[i].nPan = 128;
	}
	dwMemPos += 37;
    }
    // Setting Channel Pan Position
    for (i = 0; i < that->m_nChannels; i++) {
	that->ChnSettings[i].nPan = ((pmh->panpos[i] & 0x0F) << 4) + 8;
	that->ChnSettings[i].nVolume = 64;
    }
    // Reading pattern order
    memcpy(that->Order, lpStream + dwMemPos, pmh->lastorder + 1);
    dwMemPos += 128;
    // Reading Patterns
    pTracks = lpStream + dwMemPos;

    dwMemPos += 192 * pmh->numtracks;
    pSeq = (uint16_t *) (lpStream + dwMemPos);

    for (pat = 0; pat <= pmh->lastpattern; pat++) {
	unsigned n;

	that->PatternSize[pat] = 64;
	if ((that->Patterns[pat] =
		CSoundFile_AllocatePattern(64, that->m_nChannels)) == NULL)
	    break;
	for (n = 0; n < 32; n++)
	    if ((pSeq[n]) && (pSeq[n] <= pmh->numtracks)
		&& (n < that->m_nChannels)) {
		const uint8_t *p = pTracks + 192 * (pSeq[n] - 1);
		MODCOMMAND *m = that->Patterns[pat] + n;

		for (i = 0; i < 64; i++, m += that->m_nChannels, p += 3) {
		    unsigned cmd;
		    unsigned param;

		    if (p[0] & 0xFC) {
			m->note = (p[0] >> 2) + 37;
		    }
		    m->instr = ((p[0] & 0x03) << 4) | (p[1] >> 4);
		    cmd = p[1] & 0x0F;
		    param = p[2];

		    if (cmd == 0x0A) {
			if (param & 0xF0) {
			    param &= 0xF0;
			} else {
			    param &= 0x0F;
			}
		    }
		    m->command = cmd;
		    m->param = param;
		    if (cmd || param) {
			CSoundFile_ConvertModCommand(that, m);
		    }
		}
	    }
	pSeq += 32;
    }

    dwMemPos += 64 * (pmh->lastpattern + 1);
    if ((pmh->commentsize) && (dwMemPos + pmh->commentsize < dwMemLength)) {
	unsigned n = pmh->commentsize;
	that->m_lpszSongComments = malloc(n + 1);

	if (that->m_lpszSongComments) {
	    memcpy(that->m_lpszSongComments, lpStream + dwMemPos, n);
	    that->m_lpszSongComments[n] = 0;
	    for (i = 0; i < n; i++) {
		if (!that->m_lpszSongComments[i]) {
		    that->m_lpszSongComments[i] = ((i + 1) % 40) ? 0x20 : 0x0D;
		}
	    }
	}
    }
    dwMemPos += pmh->commentsize;
    // Reading Samples
    for (i = 1; i <= that->m_nSamples; i++) {
	if (dwMemPos >= dwMemLength) {
	    break;
	}
	dwMemPos +=
	    CSoundFile_ReadSample(that,&that->Ins[i],
	    (that->Ins[i].uFlags & CHN_16BIT) ? RS_PCM16U : RS_PCM8U,
	    (char *)(lpStream + dwMemPos), dwMemLength - dwMemPos);
    }
    that->m_nMinPeriod = 64;
    that->m_nMaxPeriod = 32767;
    return 1;
}
