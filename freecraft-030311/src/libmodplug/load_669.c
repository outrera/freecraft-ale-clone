/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: load_669.c,v 1.2 2002/12/22 15:19:42 ariclone Exp $
*/

////////////////////////////////////////////////////////////
// 669 Composer / UNIS 669 module loader
////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

typedef struct tagFILEHEADER669
{
    uint16_t sig;			// 'if' or 'JN'
    int8_t songmessage[108];		// Song Message
    uint8_t samples;			// number of samples (1-64)
    uint8_t patterns;			// number of patterns (1-128)
    uint8_t restartpos;
    uint8_t orders[128];
    uint8_t tempolist[128];
    uint8_t breaks[128];
} FILEHEADER669;

typedef struct tagSAMPLE669
{
    uint8_t filename[13];
    uint8_t length[4];	// when will somebody think about uint32_t align ???
    uint8_t loopstart[4];
    uint8_t loopend[4];
} SAMPLE669;

//----------------------------------------------------------------------------

int CSoundFile_Read669(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
{
    int b669Ext;
    const FILEHEADER669 *pfh;
    const SAMPLE669 *psmp;
    uint32_t dwMemPos;
    unsigned i;
    unsigned n;

    if (!lpStream || dwMemLength < sizeof(FILEHEADER669)) {
	return 0;
    }
    pfh = (const FILEHEADER669 *)lpStream;
    i = bswapLE16(pfh->sig);
    if (i != 0x6669 && i != 0x4E4A) {
	return 0;
    }
    if (!pfh->samples || pfh->samples > 64 || pfh->restartpos >= 128
	|| !pfh->patterns || pfh->patterns > 128) {
	return 0;
    }
    b669Ext = i == 0x4E4A ? 1 : 0;

    // Check if the index are all valid
    n = 0x1F1 + pfh->samples * sizeof(SAMPLE669) + pfh->patterns * 0x600;
    if (n > dwMemLength) {
	return 0;
    }
    psmp = (const SAMPLE669 *)(lpStream + 0x1F1);
    for (i = 0; i < pfh->samples; i++) {
	uint32_t len = bswapLE32(*((uint32_t *) (&psmp[i].length)));

	n += len;
    }
    if (n > dwMemLength + 32) {		// 32 needed by a song file
#ifdef DEBUG
	PrintFunction();
	fprintf(stdout,"%d - %d = %d\n", n, dwMemLength,
	    n - dwMemLength);
#endif
	return 0;
    }
    // That should be enough checking: this must be a 669 module.
    that->m_nType = MOD_TYPE_669;
    that->m_dwSongFlags |= SONG_LINEARSLIDES;
    that->m_nMinPeriod = 28 << 2;
    that->m_nMaxPeriod = 1712 << 3;
    that->m_nDefaultTempo = 125;
    that->m_nDefaultSpeed = 6;
    that->m_nChannels = 8;
    memcpy(that->m_szNames[0], pfh->songmessage, 16);
    that->m_nSamples = pfh->samples;

    for (i = 1; i <= that->m_nSamples; i++, psmp++) {
	uint32_t len = bswapLE32(*((uint32_t *) (&psmp->length)));
	uint32_t loopstart = bswapLE32(*((uint32_t *) (&psmp->loopstart)));
	uint32_t loopend = bswapLE32(*((uint32_t *) (&psmp->loopend)));

	if (len > MAX_SAMPLE_LENGTH) {
	    len = MAX_SAMPLE_LENGTH;
	}
	if ((loopend > len) && (!loopstart)) {
	    loopend = 0;
	}
	if (loopend > len) {
	    loopend = len;
	}
	if (loopstart + 4 >= loopend) {
	    loopstart = loopend = 0;
	}
	that->Ins[i].nLength = len;
	that->Ins[i].nLoopStart = loopstart;
	that->Ins[i].nLoopEnd = loopend;
	if (loopend) {
	    that->Ins[i].uFlags |= CHN_LOOP;
	}
	memcpy(that->m_szNames[i], psmp->filename, 13);
	that->Ins[i].nVolume = 256;
	that->Ins[i].nGlobalVol = 64;
	that->Ins[i].nPan = 128;
    }

    // Song Message
    that->m_lpszSongComments = malloc(sizeof(char) * 109);
    memcpy(that->m_lpszSongComments, pfh->songmessage, 108);
    that->m_lpszSongComments[108] = 0;

    // Reading Orders
    memcpy(that->Order, pfh->orders, 128);
    that->m_nRestartPos = pfh->restartpos;
    if (that->Order[that->m_nRestartPos] >= pfh->patterns)
	that->m_nRestartPos = 0;
    // Reading Pattern Break Locations
    for (i = 0; i < 8; i++) {
	that->ChnSettings[i].nPan = (i & 1) ? 0x30 : 0xD0;
	that->ChnSettings[i].nVolume = 64;
    }
    // Reading Patterns
    dwMemPos = 0x1F1 + pfh->samples * 25;
    for (i = 0; i < pfh->patterns; i++) {
	unsigned row;
	MODCOMMAND *m;
	const uint8_t *p;

	that->Patterns[i] = CSoundFile_AllocatePattern(64, that->m_nChannels);
	if (!that->Patterns[i]) {
	    break;
	}
	that->PatternSize[i] = 64;
	m = that->Patterns[i];
	p = lpStream + dwMemPos;

	for (row = 0; row < 64; row++) {
	    MODCOMMAND *mspeed = m;

	    if ((row == pfh->breaks[i]) && (row != 63)) {
		for (n = 0; n < 8; n++) {
		    m[n].command = CMD_PATTERNBREAK;
		    m[n].param = 0;
		}
	    }
	    for (n = 0; n < 8; n++, m++, p += 3) {
		unsigned note = p[0] >> 2;
		unsigned instr = ((p[0] & 0x03) << 4) | (p[1] >> 4);
		unsigned vol = p[1] & 0x0F;

		if (p[0] < 0xFE) {
		    m->note = note + 37;
		    m->instr = instr + 1;
		}
		if (p[0] <= 0xFE) {
		    m->volcmd = VOLCMD_VOLUME;
		    m->vol = (vol << 2) + 2;
		}
		if (p[2] != 0xFF) {
		    unsigned command = p[2] >> 4;
		    unsigned param = p[2] & 0x0F;

		    switch (command) {
			case 0x00:
			    command = CMD_PORTAMENTOUP;
			    break;
			case 0x01:
			    command = CMD_PORTAMENTODOWN;
			    break;
			case 0x02:
			    command = CMD_TONEPORTAMENTO;
			    break;
			case 0x03:
			    command = CMD_MODCMDEX;
			    param |= 0x50;
			    break;
			case 0x04:
			    command = CMD_VIBRATO;
			    param |= 0x40;
			    break;
			case 0x05:
			    if (param)
				command = CMD_SPEED;
			    else
				command = 0;
			    param += 2;
			    break;
			case 0x06:
			    if (param == 0) {
				command = CMD_PANNINGSLIDE;
				param = 0xFE;
			    } else if (param == 1) {
				command = CMD_PANNINGSLIDE;
				param = 0xEF;
			    } else
				command = 0;
			    break;
			default:
			    command = 0;
		    }
		    if (command) {
			if (command == CMD_SPEED)
			    mspeed = NULL;
			m->command = command;
			m->param = param;
		    }
		}
	    }
	    if ((!row) && (mspeed)) {
		for (n = 0; n < 8; n++)
		    if (!mspeed[n].command) {
			mspeed[n].command = CMD_SPEED;
			mspeed[n].param = pfh->tempolist[i] + 2;
			break;
		    }
	    }
	}
	dwMemPos += 0x600;
    }

    // Reading Samples
    for (i = 1; i <= that->m_nSamples; i++) {
	unsigned len = that->Ins[i].nLength;

	if (dwMemPos >= dwMemLength) {
	    break;
	}
	if (len > 4) {
	    CSoundFile_ReadSample(that, &that->Ins[i], RS_PCM8U,
		(char *)(lpStream + dwMemPos), dwMemLength - dwMemPos);
	}
	dwMemPos += len;
    }
    return 1;
}
