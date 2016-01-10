/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: load_far.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

////////////////////////////////////////
// Farandole (FAR) module loader      //
////////////////////////////////////////

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

#define FARFILEMAGIC	0xFE524146	// "FAR"

#pragma pack(1)

typedef struct FARHEADER1
{
    uint32_t id;			// file magic FAR=
    int8_t songname[40];		// songname
    int8_t magic2[3];			// 13,10,26
    uint16_t headerlen;			// remaining length of header in bytes
    uint8_t version;			// 0xD1
    uint8_t onoff[16];
    uint8_t edit1[9];
    uint8_t speed;
    uint8_t panning[16];
    uint8_t edit2[4];
    uint16_t stlen;
} FARHEADER1;

typedef struct FARHEADER2
{
    uint8_t orders[256];
    uint8_t numpat;
    uint8_t snglen;
    uint8_t loopto;
    uint16_t patsiz[256];
} FARHEADER2;

typedef struct FARSAMPLE
{
    int8_t samplename[32];
    uint32_t length;
    uint8_t finetune;
    uint8_t volume;
    uint32_t reppos;
    uint32_t repend;
    uint8_t type;
    uint8_t loop;
} FARSAMPLE;

#pragma pack()

int CSoundFile_ReadFAR(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//---------------------------------------------------------------
{
    FARHEADER1 *pmh1 = (FARHEADER1 *) lpStream;
    FARHEADER2 *pmh2;
    uint32_t dwMemPos = sizeof(FARHEADER1);
    unsigned headerlen;
    uint8_t samplemap[8];
    unsigned i;
    uint16_t *patsiz;
    MODINSTRUMENT *pins;

    if ((!lpStream) || (dwMemLength < 1024) || (pmh1->id != FARFILEMAGIC)
	|| (pmh1->magic2[0] != 13) || (pmh1->magic2[1] != 10)
	|| (pmh1->magic2[2] != 26)) {
	return 0;
    }

    headerlen = pmh1->headerlen;
    if ((headerlen >= dwMemLength)
	|| (dwMemPos + pmh1->stlen + sizeof(FARHEADER2) >= dwMemLength))
	return 0; {
    }

    // Globals
    that->m_nType = MOD_TYPE_FAR;
    that->m_nChannels = 16;
    that->m_nInstruments = 0;
    that->m_nSamples = 0;
    that->m_nSongPreAmp = 0x20;
    that->m_nDefaultSpeed = pmh1->speed;
    that->m_nDefaultTempo = 80;
    that->m_nDefaultGlobalVolume = 256;

    memcpy(that->m_szNames[0], pmh1->songname, 32);
    // Channel Setting
    for (i = 0; i < 16; i++) {
	that->ChnSettings[i].dwFlags = 0;
	that->ChnSettings[i].nPan =
	    ((pmh1->panning[i] & 0x0F) << 4) + 8;
	that->ChnSettings[i].nVolume = 64;
    }
    // Reading comment
    if (pmh1->stlen) {
	unsigned szLen = pmh1->stlen;

	if (szLen > dwMemLength - dwMemPos) {
	    szLen = dwMemLength - dwMemPos;
	}
	if ( (that->m_lpszSongComments = malloc(szLen + 1)) ) {
	    memcpy(that->m_lpszSongComments, lpStream + dwMemPos, szLen);
	    that->m_lpszSongComments[szLen] = 0;
	}
	dwMemPos += pmh1->stlen;
    }
    // Reading orders
    pmh2 = (FARHEADER2 *) (lpStream + dwMemPos);
    dwMemPos += sizeof(FARHEADER2);
    if (dwMemPos >= dwMemLength) {
	return 1;
    }

    for (i = 0; i < MAX_ORDERS; i++) {
	// FIXME: better code
	that->Order[i] = (i <= pmh2->snglen) ? pmh2->orders[i] : 0xFF;
    }
    that->m_nRestartPos = pmh2->loopto;
    // Reading Patterns
    dwMemPos += headerlen - (869 + pmh1->stlen);
    if (dwMemPos >= dwMemLength) {
	return 1;
    }

    patsiz = (uint16_t *) pmh2->patsiz;

    for (i = 0; i < 256; i++)
	if (patsiz[i]) {
	    unsigned rows;
	    unsigned patlen = patsiz[i];
	    MODCOMMAND *m;
	    unsigned patbrk;
	    const uint8_t *p;
	    unsigned max;
	    unsigned len;

	    if ((i >= MAX_PATTERNS) || (patsiz[i] < 2)) {
		dwMemPos += patlen;
		continue;
	    }
	    if (dwMemPos + patlen >= dwMemLength) {
		return 1;
	    }
	    rows = (patlen - 2) >> 6;

	    if (!rows) {
		dwMemPos += patlen;
		continue;
	    }
	    if (rows > 256) {
		rows = 256;
	    }
	    if (rows < 16) {
		rows = 16;
	    }
	    that->PatternSize[i] = rows;
	    if ((that->Patterns[i] = CSoundFile_AllocatePattern(rows,
			that->m_nChannels)) == NULL) {
		return 1;
	    }
	    m = that->Patterns[i];
	    patbrk = lpStream[dwMemPos];
	    p = lpStream + dwMemPos + 2;
	    max = rows * 16 * 4;

	    if (max > patlen - 2)
		max = patlen - 2;
	    for (len = 0; len < max; len += 4, m++) {
		uint8_t note = p[len];
		uint8_t ins = p[len + 1];
		uint8_t vol = p[len + 2];
		uint8_t eff = p[len + 3];

		if (note) {
		    m->instr = ins + 1;
		    m->note = note + 36;
		}
		if (vol & 0x0F) {
		    m->volcmd = VOLCMD_VOLUME;
		    m->vol = (vol & 0x0F) << 2;
		    if (m->vol <= 4)
			m->vol = 0;
		}
		switch (eff & 0xF0) {
			// 1.x: Portamento Up
		    case 0x10:
			m->command = CMD_PORTAMENTOUP;
			m->param = eff & 0x0F;
			break;
			// 2.x: Portamento Down
		    case 0x20:
			m->command = CMD_PORTAMENTODOWN;
			m->param = eff & 0x0F;
			break;
			// 3.x: Tone-Portamento
		    case 0x30:
			m->command = CMD_TONEPORTAMENTO;
			m->param = (eff & 0x0F) << 2;
			break;
			// 4.x: Retrigger
		    case 0x40:
			m->command = CMD_RETRIG;
			m->param = 6 / (1 + (eff & 0x0F)) + 1;
			break;
			// 5.x: Set Vibrato Depth
		    case 0x50:
			m->command = CMD_VIBRATO;
			m->param = (eff & 0x0F);
			break;
			// 6.x: Set Vibrato Speed
		    case 0x60:
			m->command = CMD_VIBRATO;
			m->param = (eff & 0x0F) << 4;
			break;
			// 7.x: Vol Slide Up
		    case 0x70:
			m->command = CMD_VOLUMESLIDE;
			m->param = (eff & 0x0F) << 4;
			break;
			// 8.x: Vol Slide Down
		    case 0x80:
			m->command = CMD_VOLUMESLIDE;
			m->param = (eff & 0x0F);
			break;
			// A.x: Port to vol
		    case 0xA0:
			m->volcmd = VOLCMD_VOLUME;
			m->vol = ((eff & 0x0F) << 2) + 4;
			break;
			// B.x: Set Balance
		    case 0xB0:
			m->command = CMD_PANNING8;
			m->param = (eff & 0x0F) << 4;
			break;
			// F.x: Set Speed
		    case 0xF0:
			m->command = CMD_SPEED;
			m->param = eff & 0x0F;
			break;
		    default:
			if ((patbrk) && (patbrk + 1 == (len >> 6))
			    && (patbrk + 1 != rows - 1)) {
			    m->command = CMD_PATTERNBREAK;
			    patbrk = 0;
			}
		}
	    }
	    dwMemPos += patlen;
	}
    // Reading samples
    if (dwMemPos + 8 >= dwMemLength)
	return 1;
    memcpy(samplemap, lpStream + dwMemPos, 8);
    dwMemPos += 8;
    pins = &that->Ins[1];

    for (i = 0; i < 64; i++, pins++) {
	if (samplemap[i >> 3] & (1 << (i & 7))) {
	    FARSAMPLE *pfs;

	    if (dwMemPos + sizeof(FARSAMPLE) > dwMemLength) {
		return 1;
	    }
	    pfs = (FARSAMPLE *) (lpStream + dwMemPos);

	    dwMemPos += sizeof(FARSAMPLE);
	    that->m_nSamples = i + 1;
	    memcpy(that->m_szNames[i + 1], pfs->samplename, 32);
	    pins->nLength = pfs->length;
	    pins->nLoopStart = pfs->reppos;
	    pins->nLoopEnd = pfs->repend;
	    pins->nFineTune = 0;
	    pins->nC4Speed = 8363 * 2;
	    pins->nGlobalVol = 64;
	    pins->nVolume = pfs->volume << 4;
	    pins->uFlags = 0;
	    if ((pins->nLength > 3) && (dwMemPos + 4 < dwMemLength)) {
		if (pfs->type & 1) {
		    pins->uFlags |= CHN_16BIT;
		    pins->nLength >>= 1;
		    pins->nLoopStart >>= 1;
		    pins->nLoopEnd >>= 1;
		}
		if ((pfs->loop & 8) && (pins->nLoopEnd > 4))
		    pins->uFlags |= CHN_LOOP;
		CSoundFile_ReadSample(that,pins,
		    (pins->uFlags & CHN_16BIT) ? RS_PCM16S : RS_PCM8S,
		    (char *)(lpStream + dwMemPos), dwMemLength - dwMemPos);
	    }
	    dwMemPos += pfs->length;
	}
    }
    return 1;
}
