/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: load_amf.c,v 1.1 2001/11/07 23:36:13 johns Exp $
 */

///////////////////////////////////////////////////
//
// AMF module loader
//
// There is 2 types of AMF files:
// - ASYLUM Music Format
// - Advanced Music Format(DSM)
//
///////////////////////////////////////////////////
#include "stdafx.h"
#include "sndfile.h"

//#define AMFLOG

//#pragma warning(disable:4244)

#pragma pack(1)

typedef struct _AMFFILEHEADER
{
    uint8_t szAMF[3];
    uint8_t version;
    int8_t title[32];
    uint8_t numsamples;
    uint8_t numorders;
    uint16_t numtracks;
    uint8_t numchannels;
} AMFFILEHEADER;

typedef struct _AMFSAMPLE
{
    uint8_t type;
    int8_t  samplename[32];
    int8_t  filename[13];
    uint32_t offset;
    uint32_t length;
    uint16_t c2spd;
    uint8_t volume;
} AMFSAMPLE;


#pragma pack()


#ifdef AMFLOG
extern void Log(const char*, ...);
#endif

//-----------------------------------------------------------------------------
static void AMF_Unpack(MODCOMMAND * pPat, const uint8_t * pTrack,
    unsigned nRows, unsigned nChannels)
{
    unsigned lastinstr = 0;
    unsigned nTrkSize = *(uint16_t *) pTrack;

    nTrkSize += (unsigned)pTrack[2] << 16;
    pTrack += 3;
    while (nTrkSize--) {
	unsigned row;
	unsigned cmd;
	unsigned arg;
	MODCOMMAND *m;

	row = pTrack[0];
	if (row >= nRows) {
	    break;
	}

	cmd = pTrack[1];
	arg = pTrack[2];
	m = pPat + row * nChannels;
	if (cmd < 0x7F) {		// note+vol
	    m->note = cmd + 1;
	    if (!m->instr) {
		m->instr = lastinstr;
	    }
	    m->volcmd = VOLCMD_VOLUME;
	    m->vol = arg;
	} else if (cmd == 0x7F) {	// duplicate row

	    int8_t rdelta = (int8_t) arg;
	    int rowsrc = (int)row + (int)rdelta;

	    if ((rowsrc >= 0) && (rowsrc < (int)nRows)) {
		*m = pPat[rowsrc * nChannels];
	    }
	} else if (cmd == 0x80) {	// instrument

	    m->instr = arg + 1;
	    lastinstr = m->instr;
	} else if (cmd == 0x83) {	// volume

	    m->volcmd = VOLCMD_VOLUME;
	    m->vol = arg;
	} else {			// effect
	    unsigned command = cmd & 0x7F;
	    unsigned param = arg;

	    switch (command) {
		    // 0x01: Set Speed
		case 0x01:
		    command = CMD_SPEED;
		    break;
		    // 0x02: Volume Slide
		case 0x02:
		    command = CMD_VOLUMESLIDE;
		    // 0x0A: Tone Porta + Vol Slide
		case 0x0A:
		    if (command == 0x0A)
			command = CMD_TONEPORTAVOL;
		    // 0x0B: Vibrato + Vol Slide
		case 0x0B:
		    if (command == 0x0B)
			command = CMD_VIBRATOVOL;
		    if (param & 0x80)
			param = (-(int8_t) param) & 0x0F;
		    else
			param = (param & 0x0F) << 4;
		    break;
		    // 0x04: Porta Up/Down
		case 0x04:
		    if (param & 0x80) {
			command = CMD_PORTAMENTOUP;
			param = -(int8_t) param;
		    } else {
			command = CMD_PORTAMENTODOWN;
		    }
		    break;
		    // 0x06: Tone Portamento
		case 0x06:
		    command = CMD_TONEPORTAMENTO;
		    break;
		    // 0x07: Tremor
		case 0x07:
		    command = CMD_TREMOR;
		    break;
		    // 0x08: Arpeggio
		case 0x08:
		    command = CMD_ARPEGGIO;
		    break;
		    // 0x09: Vibrato
		case 0x09:
		    command = CMD_VIBRATO;
		    break;
		    // 0x0C: Pattern Break
		case 0x0C:
		    command = CMD_PATTERNBREAK;
		    break;
		    // 0x0D: Position Jump
		case 0x0D:
		    command = CMD_POSITIONJUMP;
		    break;
		    // 0x0F: Retrig
		case 0x0F:
		    command = CMD_RETRIG;
		    break;
		    // 0x10: Offset
		case 0x10:
		    command = CMD_OFFSET;
		    break;
		    // 0x11: Fine Volume Slide
		case 0x11:
		    if (param) {
			command = CMD_VOLUMESLIDE;
			if (param & 0x80)
			    param = 0xF0 | ((-(int8_t) param) & 0x0F);
			else
			    param = 0x0F | ((param & 0x0F) << 4);
		    } else
			command = 0;
		    break;
		    // 0x12: Fine Portamento
		    // 0x16: Extra Fine Portamento
		case 0x12:
		case 0x16:
		    if (param) {
			int mask = (command == 0x16) ? 0xE0 : 0xF0;

			command =
			    (param & 0x80) ? CMD_PORTAMENTOUP :
			    CMD_PORTAMENTODOWN;
			if (param & 0x80)
			    param = mask | ((-(int8_t) param) & 0x0F);
			else
			    param |= mask;
		    } else
			command = 0;
		    break;
		    // 0x13: Note Delay
		case 0x13:
		    command = CMD_S3MCMDEX;
		    param = 0xD0 | (param & 0x0F);
		    break;
		    // 0x14: Note Cut
		case 0x14:
		    command = CMD_S3MCMDEX;
		    param = 0xC0 | (param & 0x0F);
		    break;
		    // 0x15: Set Tempo
		case 0x15:
		    command = CMD_TEMPO;
		    break;
		    // 0x17: Panning
		case 0x17:
		    param = (param + 64) & 0x7F;
		    if (m->command) {
			if (!m->volcmd) {
			    m->volcmd = VOLCMD_PANNING;
			    m->vol = param / 2;
			}
			command = 0;
		    } else {
			command = CMD_PANNING8;
		    }
		    // Unknown effects
		default:
		    command = param = 0;
	    }
	    if (command) {
		m->command = command;
		m->param = param;
	    }
	}
	pTrack += 3;
    }
}

static int CSoundFile_ReadAsylum(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
{
    uint32_t dwMemPos;
    unsigned numorders;
    unsigned numpats;
    unsigned numsamples;
    unsigned i;
    unsigned iPat;

    dwMemPos = 32;
    numpats = lpStream[dwMemPos + 3];
    numorders = lpStream[dwMemPos + 4];
    numsamples = 64;
    dwMemPos += 6;
    if ((!numpats) || (numpats > MAX_PATTERNS) || (!numorders)
	    || (numpats * 64 * 32 + 294 + 37 * 64 >= dwMemLength)) {
	return 0;
    }
    that->m_nType = MOD_TYPE_AMF0;
    that->m_nChannels = 8;
    that->m_nInstruments = 0;
    that->m_nSamples = 31;
    that->m_nDefaultTempo = 125;
    that->m_nDefaultSpeed = 6;
    for (i = 0; i < MAX_ORDERS; i++) {
	that->Order[i] = (i < numorders) ? lpStream[dwMemPos + i] : 0xFF;
	// FIXME: no optimal code
    }
    dwMemPos = 294;			// ???
    for (i = 0; i < numsamples; i++) {
	MODINSTRUMENT *psmp = &that->Ins[i + 1];

	memcpy(that->m_szNames[i + 1], lpStream + dwMemPos, 22);
	psmp->nFineTune = MOD2XMFineTune(lpStream[dwMemPos + 22]);
	psmp->nVolume = lpStream[dwMemPos + 23];
	psmp->nGlobalVol = 64;
	if (psmp->nVolume > 0x40) {
	    psmp->nVolume = 0x40;
	}
	psmp->nVolume <<= 2;
	psmp->nLength = *((uint32_t *) (lpStream + dwMemPos + 25));
	psmp->nLoopStart = *((uint32_t *) (lpStream + dwMemPos + 29));
	psmp->nLoopEnd =
	    psmp->nLoopStart + *((uint32_t *) (lpStream + dwMemPos + 33));
	if ((psmp->nLoopEnd > psmp->nLoopStart)
	    && (psmp->nLoopEnd <= psmp->nLength)) {
	    psmp->uFlags = CHN_LOOP;
	} else {
	    psmp->nLoopStart = psmp->nLoopEnd = 0;
	}
	if ((psmp->nLength) && (i > 31)) {
	    that->m_nSamples = i + 1;
	}
	dwMemPos += 37;
    }
    for (iPat = 0; iPat < numpats; iPat++) {
	MODCOMMAND *p = CSoundFile_AllocatePattern(64, that->m_nChannels);
	const uint8_t *pin;

	if (!p) {
	    break;
	}
	that->Patterns[iPat] = p;
	that->PatternSize[iPat] = 64;
	pin = lpStream + dwMemPos;

	for (i = 0; i < 8 * 64; i++) {
	    p->note = 0;

	    if (pin[0]) {
		p->note = pin[0] + 13;
	    }
	    p->instr = pin[1];
	    p->command = pin[2];
	    p->param = pin[3];
	    if (p->command > 0x0F) {
#ifdef AMFLOG
		Log("0x%02X.0x%02X ?", p->command, p->param);
#endif
		p->command = 0;
	    }
	    CSoundFile_ConvertModCommand(that, p);
	    pin += 4;
	    p++;
	}
	dwMemPos += 64 * 32;
    }
    // Read samples
    for (i = 0; i < that->m_nSamples; i++) {
	MODINSTRUMENT *psmp = &that->Ins[i + 1];

	if (psmp->nLength) {
	    dwMemPos +=
		CSoundFile_ReadSample(that,psmp, RS_PCM8S,
		(const char *)(lpStream + dwMemPos), dwMemLength);
	}
    }
    return 1;
}

static int CSoundFile_ReadDSMAMF(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
{
    uint32_t dwMemPos;
    uint16_t *ptracks[MAX_PATTERNS];
    uint32_t sampleseekpos[MAX_SAMPLES];
    AMFFILEHEADER *pfh;
    unsigned i;
    unsigned maxsampleseekpos;
    uint16_t *pTrackMap;
    unsigned realtrackcnt;
    uint8_t **pTrackData;

    ////////////////////////////
    // DSM/AMF

    pfh = (AMFFILEHEADER *) lpStream;
    if ((pfh->szAMF[0] != 'A') || (pfh->szAMF[1] != 'M')
	|| (pfh->szAMF[2] != 'F')
	|| (pfh->version < 10) || (pfh->version > 14) || (!pfh->numtracks)
	|| (!pfh->numorders) || (pfh->numorders > MAX_PATTERNS)
	|| (!pfh->numsamples) || (pfh->numsamples > MAX_SAMPLES)
	|| (pfh->numchannels < 4) || (pfh->numchannels > 32)) {
	return 0;
    }
    memcpy(that->m_szNames[0], pfh->title, 32);
    dwMemPos = sizeof(AMFFILEHEADER);

    that->m_nType = MOD_TYPE_AMF;
    that->m_nChannels = pfh->numchannels;
    that->m_nSamples = pfh->numsamples;
    that->m_nInstruments = 0;

    // Setup Channel Pan Positions
    if (pfh->version >= 11) {
	int8_t *panpos = (int8_t *) (lpStream + dwMemPos);
	unsigned nchannels = (pfh->version >= 13) ? 32 : 16;

	for (i = 0; i < nchannels; i++) {
	    int pan = (panpos[i] + 64) * 2;

	    if (pan < 0) {
		pan = 0;
	    } else if (pan > 256) {
		pan = 128;
		that->ChnSettings[i].dwFlags |= CHN_SURROUND;
	    }
	    that->ChnSettings[i].nPan = pan;
	}
	dwMemPos += nchannels;
    } else {
	for (i = 0; i < 16; i++) {
	    that->ChnSettings[i].nPan =
		(lpStream[dwMemPos + i] & 1) ? 0x30 : 0xD0;
	}
	dwMemPos += 16;
    }
    // Get Tempo/Speed
    that->m_nDefaultTempo = 125;
    that->m_nDefaultSpeed = 6;
    if (pfh->version >= 13) {
	if (lpStream[dwMemPos] >= 32) {
	    that->m_nDefaultTempo = lpStream[dwMemPos];
	}
	if (lpStream[dwMemPos + 1] <= 32) {
	    that->m_nDefaultSpeed = lpStream[dwMemPos + 1];
	}
	dwMemPos += 2;
    }
    // Setup sequence list
    for (i = 0; i < MAX_ORDERS; i++) {
	that->Order[i] = 0xFF;
	if (i < pfh->numorders) {
	    that->Order[i] = i;
	    that->PatternSize[i] = 64;
	    if (pfh->version >= 14) {
		that->PatternSize[i] = *(uint16_t *) (lpStream + dwMemPos);
		dwMemPos += 2;
	    }
	    ptracks[i] = (uint16_t *) (lpStream + dwMemPos);
	    dwMemPos += that->m_nChannels * sizeof(uint16_t);
	}
    }
    if (dwMemPos + that->m_nSamples * (sizeof(AMFSAMPLE) + 8) > dwMemLength) {
	return 1;
    }
    // Read Samples
    maxsampleseekpos = 0;

    for (i = 0; i < that->m_nSamples; i++) {
	MODINSTRUMENT *pins = &that->Ins[i + 1];
	AMFSAMPLE *psh = (AMFSAMPLE *) (lpStream + dwMemPos);

	dwMemPos += sizeof(AMFSAMPLE);
	memcpy(that->m_szNames[i + 1], psh->samplename, 32);
	memcpy(pins->Name, psh->filename, 13);
	pins->nLength = psh->length;
	pins->nC4Speed = psh->c2spd;
	pins->nGlobalVol = 64;
	pins->nVolume = psh->volume * 4;
	if (pfh->version >= 11) {
	    pins->nLoopStart = *(uint32_t *) (lpStream + dwMemPos);
	    pins->nLoopEnd = *(uint32_t *) (lpStream + dwMemPos + 4);
	    dwMemPos += 8;
	} else {
	    pins->nLoopStart = *(uint16_t *) (lpStream + dwMemPos);
	    pins->nLoopEnd = pins->nLength;
	    dwMemPos += 2;
	}
	sampleseekpos[i] = 0;
	if ((psh->type) && (psh->offset < dwMemLength - 1)) {
	    sampleseekpos[i] = psh->offset;
	    if (psh->offset > maxsampleseekpos)
		maxsampleseekpos = psh->offset;
	    if ((pins->nLoopEnd > pins->nLoopStart + 2)
		&& (pins->nLoopEnd <= pins->nLength))
		pins->uFlags |= CHN_LOOP;
	}
    }

    // Read Track Mapping Table
    pTrackMap = (uint16_t *) (lpStream + dwMemPos);
    realtrackcnt = 0;

    dwMemPos += pfh->numtracks * sizeof(uint16_t);
    for (i = 0; i < pfh->numtracks; i++) {
	if (realtrackcnt < pTrackMap[i]) {
	    realtrackcnt = pTrackMap[i];
	}
    }
    // Store tracks positions
    pTrackData = alloca(sizeof(uint8_t *) * realtrackcnt);
    memset(pTrackData, 0, sizeof(sizeof(uint8_t *) * realtrackcnt));

    for (i = 0; i < realtrackcnt; i++) {
	if (dwMemPos + 3 <= dwMemLength) {
	    unsigned nTrkSize = *(uint16_t *) (lpStream + dwMemPos);

	    nTrkSize += (unsigned)lpStream[dwMemPos + 2] << 16;
	    if (dwMemPos + nTrkSize * 3 + 3 <= dwMemLength) {
		pTrackData[i] = (uint8_t *) (lpStream + dwMemPos);
	    }
	    dwMemPos += nTrkSize * 3 + 3;
	}
    }
    // Create the patterns from the list of tracks
    for (i = 0; i < pfh->numorders; i++) {
	unsigned j;
	MODCOMMAND *p = CSoundFile_AllocatePattern(that->PatternSize[i],
	    that->m_nChannels);

	if (!p) {
	    break;
	}
	that->Patterns[i] = p;
	for (j = 0; j < that->m_nChannels; j++) {
	    unsigned nTrack = ptracks[i][j];

	    if ((nTrack) && (nTrack <= pfh->numtracks)) {
		unsigned realtrk = pTrackMap[nTrack - 1];

		if (realtrk) {
		    realtrk--;
		    if ((realtrk < realtrackcnt) && (pTrackData[realtrk])) {
			AMF_Unpack(p + j, pTrackData[realtrk],
			    that->PatternSize[i], that->m_nChannels);
		    }
		}
	    }
	}
    }
    // delete pTrackData;

    // Read Sample Data
    for (i = 1; i <= maxsampleseekpos; i++) {
	unsigned j;

	if (dwMemPos >= dwMemLength) {
	    break;
	}
	for (j = 0; j < that->m_nSamples; j++) {
	    if (i == sampleseekpos[j]) {
		MODINSTRUMENT *pins = &that->Ins[j + 1];

		dwMemPos += CSoundFile_ReadSample(that,pins, RS_PCM8U,
		    (const char *)(lpStream + dwMemPos),
		    dwMemLength - dwMemPos);
		break;
	    }
	}
    }
    return 1;
}

int CSoundFile_ReadAMF(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//-----------------------------------------------------------
{
    if ((!lpStream) || (dwMemLength < 2048)) {
	return 0;
    }

    if ((!strncmp((const char *)lpStream, "ASYLUM Music Format V1.0", 25))
	&& (dwMemLength > 4096)) {
	return CSoundFile_ReadAsylum(that, lpStream, dwMemLength);
    }

    return CSoundFile_ReadDSMAMF(that, lpStream, dwMemLength);
}
