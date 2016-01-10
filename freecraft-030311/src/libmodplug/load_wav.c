/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: load_wav.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

#ifndef WAVE_FORMAT_EXTENSIBLE
#define WAVE_FORMAT_EXTENSIBLE	0xFFFE
#endif

/////////////////////////////////////////////////////////////
// WAV file support

int CSoundFile_ReadWav(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//---------------------------------------------------------------
{
    uint32_t dwMemPos;
    WAVEFILEHEADER *phdr;
    WAVEFORMATHEADER *pfmt;
    WAVEDATAHEADER *pdata;
    MODCOMMAND *pcmd;
    uint32_t dwTime;
    uint32_t framesperrow;
    unsigned norders;
    unsigned samplesize;
    unsigned len;
    unsigned bytelen;
    unsigned i;
    unsigned j;

    if (!lpStream || dwMemLength < (uint32_t) sizeof(WAVEFILEHEADER)) {
	return 0;
    }
    phdr = (WAVEFILEHEADER *) lpStream;
    pfmt = (WAVEFORMATHEADER *) (lpStream + sizeof(WAVEFILEHEADER));
    if ((phdr->id_RIFF != IFFID_RIFF) || (phdr->id_WAVE != IFFID_WAVE)
	|| (pfmt->id_fmt != IFFID_fmt)) {
	return 0;
    }
    dwMemPos = sizeof(WAVEFILEHEADER) + 8 + pfmt->hdrlen;
    if ((dwMemPos + 8 >= dwMemLength)
	|| ((pfmt->format != WAVE_FORMAT_PCM)
	    && (pfmt->format != WAVE_FORMAT_EXTENSIBLE))
	|| (pfmt->channels > 4)
	|| !pfmt->channels
	|| !pfmt->freqHz
	|| (pfmt->bitspersample & 7)
	|| (pfmt->bitspersample < 8)
	|| (pfmt->bitspersample > 32)) {
	return 0;
    }

    for (;;) {
	pdata = (WAVEDATAHEADER *) (lpStream + dwMemPos);
	if (pdata->id_data == IFFID_data) {
	    break;
	}
	dwMemPos += pdata->length + 8;
	if (dwMemPos + 8 >= dwMemLength) {
	    return 0;
	}
    }
    that->m_nType = MOD_TYPE_WAV;
    that->m_nSamples = 0;
    that->m_nInstruments = 0;
    that->m_nChannels = 4;
    that->m_nDefaultSpeed = 8;
    that->m_nDefaultTempo = 125;
    that->m_dwSongFlags |= SONG_LINEARSLIDES;	// For no resampling
    that->Order[0] = 0;
    that->Order[1] = 0xFF;
    that->PatternSize[0] = that->PatternSize[1] = 64;
    if ((that->Patterns[0] = CSoundFile_AllocatePattern(64, 4)) == NULL) {
	return 1;
    }
    if ((that->Patterns[1] = CSoundFile_AllocatePattern(64, 4)) == NULL) {
	return 1;
    }
    samplesize = (pfmt->channels * pfmt->bitspersample) >> 3;
    len = pdata->length;

    if (dwMemPos + len > dwMemLength - 8) {
	len = dwMemLength - dwMemPos - 8;
    }
    len /= samplesize;
    bytelen = len;
    if (pfmt->bitspersample >= 16) {
	bytelen *= 2;
    }
    if (len > MAX_SAMPLE_LENGTH) {
	len = MAX_SAMPLE_LENGTH;
    }
    if (!len) {
	return 1;
    }
    // Setting up module length
    dwTime = ((len * 50) / pfmt->freqHz) + 1;
    framesperrow = (dwTime + 63) / 63;

    if (framesperrow < 4) {
	framesperrow = 4;
    }
    norders = 1;

    while (framesperrow >= 0x20) {
	that->Order[norders++] = 1;
	that->Order[norders] = 0xFF;
	framesperrow = (dwTime + (64 * norders - 1)) / (64 * norders);
	if (norders >= MAX_ORDERS - 1)
	    break;
    }
    that->m_nDefaultSpeed = framesperrow;
    for (i = 0; i < 4; i++) {
	that->ChnSettings[i].nPan = (i & 1) ? 256 : 0;
	that->ChnSettings[i].nVolume = 64;
	that->ChnSettings[i].dwFlags = 0;
    }

    // Setting up speed command
    pcmd = that->Patterns[0];
    pcmd[0].command = CMD_SPEED;
    pcmd[0].param = (uint8_t) that->m_nDefaultSpeed;
    pcmd[0].note = 5 * 12 + 1;
    pcmd[0].instr = 1;
    pcmd[1].note = pcmd[0].note;
    pcmd[1].instr = pcmd[0].instr;
    that->m_nSamples = pfmt->channels;
    // Support for Multichannel Wave
    for (i = 0; i < that->m_nSamples; i++) {
	MODINSTRUMENT *pins = &that->Ins[i + 1];

	pcmd[i].note = pcmd[0].note;
	pcmd[i].instr = (uint8_t) (i + 1);
	pins->nLength = len;
	pins->nC4Speed = pfmt->freqHz;
	pins->nVolume = 256;
	pins->nPan = 128;
	pins->nGlobalVol = 64;
	pins->uFlags =
	    (uint16_t) ((pfmt->bitspersample >= 16) ? CHN_16BIT : 0);
	pins->uFlags |= CHN_PANNING;
	if (that->m_nSamples > 1) {
	    switch (i) {
		case 0:
		    pins->nPan = 0;
		    break;
		case 1:
		    pins->nPan = 256;
		    break;
		case 2:
		    pins->nPan =
			(uint16_t) ((that->m_nSamples == 3) ? 128 : 64);
		    pcmd[i].command = CMD_S3MCMDEX;
		    pcmd[i].param = 0x91;
		    break;
		case 3:
		    pins->nPan = 192;
		    pcmd[i].command = CMD_S3MCMDEX;
		    pcmd[i].param = 0x91;
		    break;
		default:
		    pins->nPan = 128;
		    break;
	    }
	}
	if ((pins->pSample = CSoundFile_AllocateSample(bytelen + 8)) == NULL)
	    return 1;
	if (pfmt->bitspersample >= 16) {
	    int slsize = pfmt->bitspersample >> 3;
	    signed short *p = (signed short *)pins->pSample;
	    int8_t *psrc =
		(int8_t *) (lpStream + dwMemPos + 8 + i * slsize + slsize -
		2);
	    for (j = 0; j < len; j++) {
		p[j] = *((signed short *)psrc);
		psrc += samplesize;
	    }
	    p[len + 1] = p[len] = p[len - 1];
	} else {
	    int8_t *p = (int8_t *) pins->pSample;
	    int8_t *psrc = (int8_t *) (lpStream + dwMemPos + 8 + i);

	    for (j = 0; j < len; j++) {
		p[j] = (int8_t) ((*psrc) + 0x80);
		psrc += samplesize;
	    }
	    p[len + 1] = p[len] = p[len - 1];
	}
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////
// IMA ADPCM Support

#pragma pack(1)

typedef struct IMAADPCMBLOCK
{
    uint16_t sample;
    uint8_t index;
    uint8_t Reserved;
}
DVI_ADPCMBLOCKHEADER;

#pragma pack()

static const int gIMAUnpackTable[90] = {
    7, 8, 9, 10, 11, 12, 13, 14,
    16, 17, 19, 21, 23, 25, 28, 31,
    34, 37, 41, 45, 50, 55, 60, 66,
    73, 80, 88, 97, 107, 118, 130, 143,
    157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658,
    724, 796, 876, 963, 1060, 1166, 1282, 1411,
    1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
    3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
    7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
    32767, 0
};

int IMAADPCMUnpack16(signed short *pdest, unsigned nLen, uint8_t * psrc,
    uint32_t dwBytes, unsigned pkBlkAlign)
//------------------------------------------------------------------------------------------------
{
    static const int gIMAIndexTab[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };
    unsigned nPos;
    int value;
    unsigned i;

    if ((nLen < 4) || (!pdest) || (!psrc)
	|| (pkBlkAlign < 5) || (pkBlkAlign > dwBytes))
	return 0;
    nPos = 0;
    while ((nPos < nLen) && (dwBytes > 4)) {
	int nIndex;

	value = *((int16_t *) psrc);
	nIndex = psrc[2];
	psrc += 4;
	dwBytes -= 4;
	pdest[nPos++] = (int16_t) value;
	for (i = 0;
	    ((i < (pkBlkAlign - 4) * 2) && (nPos < nLen) && (dwBytes)); i++) {
	    uint8_t delta;
	    int v;

	    if (i & 1) {
		delta = (uint8_t) (((*(psrc++)) >> 4) & 0x0F);
		dwBytes--;
	    } else {
		delta = (uint8_t) ((*psrc) & 0x0F);
	    }
	    v = gIMAUnpackTable[nIndex] >> 3;

	    if (delta & 1) {
		v += gIMAUnpackTable[nIndex] >> 2;
	    }
	    if (delta & 2) {
		v += gIMAUnpackTable[nIndex] >> 1;
	    }
	    if (delta & 4) {
		v += gIMAUnpackTable[nIndex];
	    }
	    if (delta & 8) {
		value -= v;
	    } else {
		value += v;
	    }
	    nIndex += gIMAIndexTab[delta & 7];
	    if (nIndex < 0) {
		nIndex = 0;
	    } else if (nIndex > 88) {
		nIndex = 88;
	    }
	    if (value > 32767) {
		value = 32767;
	    } else if (value < -32768) {
		value = -32768;
	    }
	    pdest[nPos++] = (int16_t) value;
	}
    }
    return 1;
}
