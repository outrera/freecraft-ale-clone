/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: load_okt.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

//////////////////////////////////////////////
// Oktalyzer (OKT) module loader	    //
//////////////////////////////////////////////

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

typedef struct OKTFILEHEADER
{
    uint32_t okta;			// "OKTA"
    uint32_t song;			// "SONG"
    uint32_t cmod;			// "CMOD"
    uint32_t fixed8;
    uint8_t chnsetup[8];
    uint32_t samp;			// "SAMP"
    uint32_t samplen;
} OKTFILEHEADER;

typedef struct OKTSAMPLE
{
    int8_t name[20];
    uint32_t length;
    uint16_t loopstart;
    uint16_t looplen;
    uint8_t pad1;
    uint8_t volume;
    uint8_t pad2;
    uint8_t pad3;
} OKTSAMPLE;

int CSoundFile_ReadOKT(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//---------------------------------------------------------------
{
    OKTFILEHEADER *pfh;
    uint32_t dwMemPos;
    unsigned nsamples;
    unsigned npatterns;
    unsigned norders;
    unsigned npat;
    unsigned i;

    if ((!lpStream) || (dwMemLength < 1024)) {
	return 0;
    }
    pfh = (OKTFILEHEADER *) lpStream;
    if ((pfh->okta != 0x41544B4F) || (pfh->song != 0x474E4F53)
	    || (pfh->cmod != 0x444F4D43) || (pfh->chnsetup[0])
	    || (pfh->chnsetup[2])
	    || (pfh->chnsetup[4]) || (pfh->chnsetup[6])
	    || (pfh->fixed8 != 0x08000000)
	    || (pfh->samp != 0x504D4153)) {
	return 0;
    }
    nsamples = 0;
    npatterns = 0;
    norders = 0;
    dwMemPos = sizeof(OKTFILEHEADER);
    that->m_nType = MOD_TYPE_OKT;
    that->m_nChannels =
	4 + pfh->chnsetup[1] + pfh->chnsetup[3] + pfh->chnsetup[5] +
	pfh->chnsetup[7];
    if (that->m_nChannels > MAX_CHANNELS) {
	that->m_nChannels = MAX_CHANNELS;
    }
    nsamples = bswapBE32(pfh->samplen) >> 5;
    that->m_nSamples = nsamples;
    if (that->m_nSamples >= MAX_SAMPLES) {
	that->m_nSamples = MAX_SAMPLES - 1;
    }
    // Reading samples
    for (i = 1; i <= nsamples; i++) {
	if (dwMemPos >= dwMemLength)
	    return 1;
	if (i < MAX_SAMPLES) {
	    OKTSAMPLE *psmp = (OKTSAMPLE *) (lpStream + dwMemPos);
	    MODINSTRUMENT *pins = &that->Ins[i];

	    memcpy(that->m_szNames[i], psmp->name, 20);
	    pins->uFlags = 0;
	    pins->nLength = bswapBE32(psmp->length) & ~1;
	    pins->nLoopStart = bswapBE16(psmp->loopstart);
	    pins->nLoopEnd = pins->nLoopStart + bswapBE16(psmp->looplen);
	    if (pins->nLoopStart + 2 < pins->nLoopEnd)
		pins->uFlags |= CHN_LOOP;
	    pins->nGlobalVol = 64;
	    pins->nVolume = psmp->volume << 2;
	    pins->nC4Speed = 8363;
	}
	dwMemPos += sizeof(OKTSAMPLE);
    }
    // SPEE
    if (dwMemPos >= dwMemLength)
	return 1;
    if (*((uint32_t *) (lpStream + dwMemPos)) == 0x45455053) {
	that->m_nDefaultSpeed = lpStream[dwMemPos + 9];
	dwMemPos += bswapBE32(*((uint32_t *) (lpStream + dwMemPos + 4))) + 8;
    }
    // SLEN
    if (dwMemPos >= dwMemLength)
	return 1;
    if (*((uint32_t *) (lpStream + dwMemPos)) == 0x4E454C53) {
	npatterns = lpStream[dwMemPos + 9];
	dwMemPos += bswapBE32(*((uint32_t *) (lpStream + dwMemPos + 4))) + 8;
    }
    // PLEN
    if (dwMemPos >= dwMemLength) {
	return 1;
    }
    if (*((uint32_t *) (lpStream + dwMemPos)) == 0x4E454C50) {
	norders = lpStream[dwMemPos + 9];
	dwMemPos += bswapBE32(*((uint32_t *) (lpStream + dwMemPos + 4))) + 8;
    }
    // PATT
    if (dwMemPos >= dwMemLength) {
	return 1;
    }
    if (*((uint32_t *) (lpStream + dwMemPos)) == 0x54544150) {
	unsigned orderlen = norders;

	if (orderlen >= MAX_ORDERS) {
	    orderlen = MAX_ORDERS - 1;
	}
	for (i = 0; i < orderlen; i++) {
	    that->Order[i] = lpStream[dwMemPos + 10 + i];
	}
	for (i = orderlen; i > 1; i--) {
	    if (that->Order[i - 1]) {
		break;
	    }
	    that->Order[i - 1] = 0xFF;
	}
	dwMemPos += bswapBE32(*((uint32_t *) (lpStream + dwMemPos + 4))) + 8;
    }
    // PBOD
    npat = 0;

    while ((dwMemPos + 10 < dwMemLength)
	&& (*((uint32_t *) (lpStream + dwMemPos)) == 0x444F4250)) {
	uint32_t dwPos = dwMemPos + 10;
	unsigned rows = lpStream[dwMemPos + 9];

	if (!rows)
	    rows = 64;
	if (npat < MAX_PATTERNS) {
	    MODCOMMAND *m;
	    unsigned imax;

	    if ((that->Patterns[npat] =
		    CSoundFile_AllocatePattern(rows,
			that->m_nChannels)) == NULL) {
		return 1;
	    }
	    m = that->Patterns[npat];

	    that->PatternSize[npat] = rows;
	    imax = that->m_nChannels * rows;

	    for (i = 0; i < imax; i++, m++, dwPos += 4) {
		const uint8_t *p;
		unsigned note;
		unsigned command;
		unsigned param;

		if (dwPos + 4 > dwMemLength) {
		    break;
		}
		p = lpStream + dwPos;
		note = p[0];

		if (note) {
		    m->note = note + 48;
		    m->instr = p[1] + 1;
		}
		command = p[2];
		param = p[3];

		m->param = param;
		switch (command) {
			// 0: no effect
		    case 0:
			break;
			// 1: Portamento Up
		    case 1:
		    case 17:
		    case 30:
			if (param)
			    m->command = CMD_PORTAMENTOUP;
			break;
			// 2: Portamento Down
		    case 2:
		    case 13:
		    case 21:
			if (param)
			    m->command = CMD_PORTAMENTODOWN;
			break;
			// 10: Arpeggio
		    case 10:
		    case 11:
		    case 12:
			m->command = CMD_ARPEGGIO;
			break;
			// 15: Filter
		    case 15:
			m->command = CMD_MODCMDEX;
			m->param = param & 0x0F;
			break;
			// 25: Position Jump
		    case 25:
			m->command = CMD_POSITIONJUMP;
			break;
			// 28: Set Speed
		    case 28:
			m->command = CMD_SPEED;
			break;
			// 31: Volume Control
		    case 31:
			if (param <= 0x40)
			    m->command = CMD_VOLUME;
			else if (param <= 0x50) {
			    m->command = CMD_VOLUMESLIDE;
			    m->param &= 0x0F;
			    if (!m->param)
				m->param = 0x0F;
			} else if (param <= 0x60) {
			    m->command = CMD_VOLUMESLIDE;
			    m->param = (param & 0x0F) << 4;
			    if (!m->param)
				m->param = 0xF0;
			} else if (param <= 0x70) {
			    m->command = CMD_MODCMDEX;
			    m->param = 0xB0 | (param & 0x0F);
			    if (!(param & 0x0F))
				m->param = 0xBF;
			} else if (param <= 0x80) {
			    m->command = CMD_MODCMDEX;
			    m->param = 0xA0 | (param & 0x0F);
			    if (!(param & 0x0F))
				m->param = 0xAF;
			}
			break;
		}
	    }
	}
	npat++;
	dwMemPos += bswapBE32(*((uint32_t *) (lpStream + dwMemPos + 4))) + 8;
    }
    // SBOD
    for( i = 1; (dwMemPos + 10 < dwMemLength)
	    && (*((uint32_t *) (lpStream + dwMemPos)) == 0x444F4253); i++ ) {
	if (i < MAX_SAMPLES) {
	    CSoundFile_ReadSample(that,&that->Ins[i], RS_PCM8S,
		(char *)(lpStream + dwMemPos + 8), dwMemLength - dwMemPos - 8);
	}
	dwMemPos += bswapBE32(*((uint32_t *) (lpStream + dwMemPos + 4))) + 8;
    }
    return 1;
}
