/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: load_ptm.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

//////////////////////////////////////////////
// PTM PolyTracker module loader            //
//////////////////////////////////////////////

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

#pragma pack(1)

typedef struct PTMFILEHEADER
{
    int8_t songname[28];		// name of song, asciiz string
    int8_t eof;				// 26
    uint8_t version_lo;			// 03 version of file, currently 0203h
    uint8_t version_hi;			// 02
    uint8_t reserved1;			// reserved, set to 0
    uint16_t norders;			// number of orders (0..256)
    uint16_t nsamples;			// number of instruments (1..255)
    uint16_t npatterns;			// number of patterns (1..128)
    uint16_t nchannels;			// number of channels (voices) used (1..32)
    uint16_t fileflags;			// set to 0
    uint16_t reserved2;			// reserved, set to 0
    uint32_t ptmf_id;			// song identification, 'PTMF' or 0x464d5450
    uint8_t reserved3[16];		// reserved, set to 0
    uint8_t chnpan[32];			// channel panning settings, 0..15, 0 = left, 7 = middle, 15 = right
    uint8_t orders[256];		// order list, valid entries 0..nOrders-1
    uint16_t patseg[128];		// pattern offsets (*16)
} PTMFILEHEADER;

#define SIZEOF_PTMFILEHEADER	608

typedef struct PTMSAMPLE
{
    uint8_t sampletype;			// sample type (bit array)
    int8_t filename[12];		// name of external sample file
    uint8_t volume;			// default volume
    uint16_t nC4Spd;			// C4 speed
    uint16_t sampleseg;			// sample segment (used internally)
    uint16_t fileofs[2];		// offset of sample data
    uint16_t length[2];			// sample size (in bytes)
    uint16_t loopbeg[2];		// start of loop
    uint16_t loopend[2];		// end of loop
    uint16_t gusdata[8];
    char samplename[28];		// name of sample, asciiz  // changed from CHAR
    uint32_t ptms_id;			// sample identification, 'PTMS' or 0x534d5450
} PTMSAMPLE;

#define SIZEOF_PTMSAMPLE    80

#pragma pack()

int CSoundFile_ReadPTM(CSoundFile * that, const uint8_t * lpStream,
	uint32_t dwMemLength)
//---------------------------------------------------------------
{
    PTMFILEHEADER pfh = *(PTMFILEHEADER *) lpStream;
    uint32_t dwMemPos;
    unsigned nOrders;
    unsigned i;

    pfh.norders = bswapLE16(pfh.norders);
    pfh.nsamples = bswapLE16(pfh.nsamples);
    pfh.npatterns = bswapLE16(pfh.npatterns);
    pfh.nchannels = bswapLE16(pfh.nchannels);
    pfh.fileflags = bswapLE16(pfh.fileflags);
    pfh.reserved2 = bswapLE16(pfh.reserved2);
    pfh.ptmf_id = bswapLE32(pfh.ptmf_id);
    for (i = 0; i < 128; i++) {
	pfh.patseg[i] = bswapLE16(pfh.patseg[i]);
    }

    if ((!lpStream) || (dwMemLength < 1024))
	return 0;
    if ((pfh.ptmf_id != 0x464d5450) || (!pfh.nchannels)
	|| (pfh.nchannels > 32)
	|| (pfh.norders > 256) || (!pfh.norders)
	|| (!pfh.nsamples) || (pfh.nsamples > 255)
	|| (!pfh.npatterns) || (pfh.npatterns > 128)
	|| (SIZEOF_PTMFILEHEADER + pfh.nsamples * SIZEOF_PTMSAMPLE >=
	    (int)dwMemLength))
	return 0;
    memcpy(that->m_szNames[0], pfh.songname, 28);
    that->m_szNames[0][28] = 0;
    that->m_nType = MOD_TYPE_PTM;
    that->m_nChannels = pfh.nchannels;
    that->m_nSamples =
	(pfh.nsamples < MAX_SAMPLES) ? pfh.nsamples : MAX_SAMPLES - 1;
    dwMemPos = SIZEOF_PTMFILEHEADER;
    nOrders = (pfh.norders < MAX_ORDERS) ? pfh.norders : MAX_ORDERS - 1;
    memcpy(that->Order, pfh.orders, nOrders);
    for (i = 0; i < that->m_nChannels; i++) {
	that->ChnSettings[i].nVolume = 64;
	that->ChnSettings[i].nPan = ((pfh.chnpan[i] & 0x0F) << 4) + 4;
    }
    for (i = 0; i < that->m_nSamples; i++, dwMemPos += SIZEOF_PTMSAMPLE) {
	MODINSTRUMENT *pins = &that->Ins[i + 1];
	PTMSAMPLE *psmp = (PTMSAMPLE *) (lpStream + dwMemPos);

	strncpy(that->m_szNames[i + 1], psmp->samplename, 28);
	memcpy(pins->Name, psmp->filename, 12);
	pins->Name[12] = 0;
	pins->nGlobalVol = 64;
	pins->nPan = 128;
	pins->nVolume = psmp->volume << 2;
	pins->nC4Speed = bswapLE16(psmp->nC4Spd) << 1;
	pins->uFlags = 0;
	if ((psmp->sampletype & 3) == 1) {
	    unsigned smpflg = RS_PCM8D;
	    uint32_t samplepos;

	    pins->nLength = bswapLE32(*(uint32_t *) (psmp->length));
	    pins->nLoopStart = bswapLE32(*(uint32_t *) (psmp->loopbeg));
	    pins->nLoopEnd = bswapLE32(*(uint32_t *) (psmp->loopend));
	    samplepos = bswapLE32(*(uint32_t *) (&psmp->fileofs));
	    if (psmp->sampletype & 4)
		pins->uFlags |= CHN_LOOP;
	    if (psmp->sampletype & 8)
		pins->uFlags |= CHN_PINGPONGLOOP;
	    if (psmp->sampletype & 16) {
		pins->uFlags |= CHN_16BIT;
		pins->nLength >>= 1;
		pins->nLoopStart >>= 1;
		pins->nLoopEnd >>= 1;
		smpflg = RS_PTM8DTO16;
	    }
	    if ((pins->nLength) && (samplepos) && (samplepos < dwMemLength)) {
		CSoundFile_ReadSample(that,pins, smpflg,
		    (char *)(lpStream + samplepos), dwMemLength - samplepos);
	    }
	}
    }
    // Reading Patterns
    for (i = 0; i < pfh.npatterns; i++) {
	unsigned row;
	MODCOMMAND *m;

	dwMemPos = ((unsigned)pfh.patseg[i]) << 4;
	if ((!dwMemPos) || (dwMemPos >= dwMemLength))
	    continue;
	that->PatternSize[i] = 64;
	if ((that->Patterns[i] =
		CSoundFile_AllocatePattern(64, that->m_nChannels)) == NULL) {
	    break;
	}
	m = that->Patterns[i];

	for (row = 0; ((row < 64) && (dwMemPos < dwMemLength));) {
	    unsigned b = lpStream[dwMemPos++];

	    if (dwMemPos >= dwMemLength)
		break;
	    if (b) {
		unsigned nChn = b & 0x1F;

		if (b & 0x20) {
		    if (dwMemPos + 2 > dwMemLength)
			break;
		    m[nChn].note = lpStream[dwMemPos++];
		    m[nChn].instr = lpStream[dwMemPos++];
		}
		if (b & 0x40) {
		    if (dwMemPos + 2 > dwMemLength)
			break;
		    m[nChn].command = lpStream[dwMemPos++];
		    m[nChn].param = lpStream[dwMemPos++];
		    if ((m[nChn].command == 0x0E)
			&& ((m[nChn].param & 0xF0) == 0x80)) {
			m[nChn].command = CMD_S3MCMDEX;
		    } else if (m[nChn].command < 0x10) {
			CSoundFile_ConvertModCommand(that, &m[nChn]);
		    } else {
			switch (m[nChn].command) {
			    case 16:
				m[nChn].command = CMD_GLOBALVOLUME;
				break;
			    case 17:
				m[nChn].command = CMD_RETRIG;
				break;
			    case 18:
				m[nChn].command = CMD_FINEVIBRATO;
				break;
			    default:
				m[nChn].command = 0;
			}
		    }
		}
		if (b & 0x80) {
		    if (dwMemPos >= dwMemLength)
			break;
		    m[nChn].volcmd = VOLCMD_VOLUME;
		    m[nChn].vol = lpStream[dwMemPos++];
		}
	    } else {
		row++;
		m += that->m_nChannels;
	    }
	}
    }
    return 1;
}
