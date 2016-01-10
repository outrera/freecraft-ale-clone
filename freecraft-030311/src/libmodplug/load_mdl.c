/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
*/

//////////////////////////////////////////////
// DigiTracker (MDL) module loader	    //
//////////////////////////////////////////////

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

typedef struct MDLSONGHEADER
{
    uint32_t id;			// "DMDL" = 0x4C444D44
    uint8_t version;
} MDLSONGHEADER;

typedef struct MDLINFOBLOCK
{
    int8_t songname[32];
    int8_t composer[20];
    uint16_t norders;
    uint16_t repeatpos;
    uint8_t globalvol;
    uint8_t speed;
    uint8_t tempo;
    uint8_t channelinfo[32];
    uint8_t seq[256];
} MDLINFOBLOCK;

typedef struct MDLPATTERNDATA
{
    uint8_t channels;
    uint8_t lastrow;			// nrows = lastrow+1
    int8_t name[16];
    uint16_t data[1];
} MDLPATTERNDATA;

static void ConvertMDLCommand(MODCOMMAND * m, unsigned eff, unsigned data)
//--------------------------------------------------------
{
    unsigned command = 0;
    unsigned param = data;

    switch (eff) {
	case 0x01:
	    command = CMD_PORTAMENTOUP;
	    break;
	case 0x02:
	    command = CMD_PORTAMENTODOWN;
	    break;
	case 0x03:
	    command = CMD_TONEPORTAMENTO;
	    break;
	case 0x04:
	    command = CMD_VIBRATO;
	    break;
	case 0x05:
	    command = CMD_ARPEGGIO;
	    break;
	case 0x07:
	    command = (param < 0x20) ? CMD_SPEED : CMD_TEMPO;
	    break;
	case 0x08:
	    command = CMD_PANNING8;
	    param <<= 1;
	    break;
	case 0x0B:
	    command = CMD_POSITIONJUMP;
	    break;
	case 0x0C:
	    command = CMD_GLOBALVOLUME;
	    break;
	case 0x0D:
	    command = CMD_PATTERNBREAK;
	    param = (data & 0x0F) + (data >> 4) * 10;
	    break;
	case 0x0E:
	    command = CMD_S3MCMDEX;
	    switch (data & 0xF0) {
		case 0x00:
		    command = 0;
		    break;		// What is E0x in MDL (there is a bunch) ?
		case 0x10:
		    if (param & 0x0F) {
			param |= 0xF0;
			command = CMD_PANNINGSLIDE;
		    } else
			command = 0;
		    break;
		case 0x20:
		    if (param & 0x0F) {
			param = (param << 4) | 0x0F;
			command = CMD_PANNINGSLIDE;
		    } else
			command = 0;
		    break;
		case 0x30:
		    param = (data & 0x0F) | 0x10;
		    break;		// glissando
		case 0x40:
		    param = (data & 0x0F) | 0x30;
		    break;		// vibrato waveform
		case 0x60:
		    param = (data & 0x0F) | 0xB0;
		    break;
		case 0x70:
		    param = (data & 0x0F) | 0x40;
		    break;		// tremolo waveform
		case 0x90:
		    command = CMD_RETRIG;
		    param &= 0x0F;
		    break;
		case 0xA0:
		    param = (data & 0x0F) << 4;
		    command = CMD_GLOBALVOLSLIDE;
		    break;
		case 0xB0:
		    param = data & 0x0F;
		    command = CMD_GLOBALVOLSLIDE;
		    break;
		case 0xF0:
		    param = ((data >> 8) & 0x0F) | 0xA0;
		    break;
	    }
	    break;
	case 0x0F:
	    command = CMD_SPEED;
	    break;
	case 0x10:
	    if ((param & 0xF0) != 0xE0) {
		command = CMD_VOLUMESLIDE;
		if ((param & 0xF0) == 0xF0)
		    param = ((param << 4) | 0x0F);
		else
		    param >>= 2;
	    }
	    break;
	case 0x20:
	    if ((param & 0xF0) != 0xE0) {
		command = CMD_VOLUMESLIDE;
		if ((param & 0xF0) != 0xF0)
		    param >>= 2;
	    }
	    break;
	case 0x30:
	    command = CMD_RETRIG;
	    break;
	case 0x40:
	    command = CMD_TREMOLO;
	    break;
	case 0x50:
	    command = CMD_TREMOR;
	    break;
	case 0xEF:
	    if (param > 0xFF)
		param = 0xFF;
	    command = CMD_OFFSET;
	    break;
    }
    if (command) {
	m->command = command;
	m->param = param;
    }
}

static void UnpackMDLTrack(MODCOMMAND * pat, unsigned nChannels, unsigned nRows,
    unsigned nTrack, const uint8_t * lpTracks)
//-------------------------------------------------------------------------------------------------
{
    MODCOMMAND cmd, *m = pat;
    unsigned len = *((uint16_t *) lpTracks);
    unsigned pos = 0, row = 0;
    unsigned i;

    lpTracks += 2;
    for (i = 1; i < nTrack; i++) {
	lpTracks += len;
	len = *((uint16_t *) lpTracks);
	lpTracks += 2;
    }
    cmd.note = cmd.instr = 0;
    cmd.volcmd = cmd.vol = 0;
    cmd.command = cmd.param = 0;
    while ((row < nRows) && (pos < len)) {
	unsigned xx;
	uint8_t b = lpTracks[pos++];

	xx = b >> 2;
	switch (b & 0x03) {
	    case 0x01:
		for (i = 0; i <= xx; i++) {
		    if (row)
			*m = *(m - nChannels);
		    m += nChannels;
		    row++;
		    if (row >= nRows)
			break;
		}
		break;

	    case 0x02:
		if (xx < row)
		    *m = pat[nChannels * xx];
		m += nChannels;
		row++;
		break;

	    case 0x03:
	    {
		unsigned volume;
		unsigned commands;
		unsigned command1;
		unsigned command2;
		unsigned param1;
		unsigned param2;

		cmd.note = (xx & 0x01) ? lpTracks[pos++] : 0;
		cmd.instr = (xx & 0x02) ? lpTracks[pos++] : 0;
		cmd.volcmd = cmd.vol = 0;
		cmd.command = cmd.param = 0;
		if ((cmd.note < 120 - 12) && (cmd.note)) {
		    cmd.note += 12;
		}
		volume = (xx & 0x04) ? lpTracks[pos++] : 0;
		commands = (xx & 0x08) ? lpTracks[pos++] : 0;
		command1 = commands & 0x0F;
		command2 = commands & 0xF0;
		param1 = (xx & 0x10) ? lpTracks[pos++] : 0;
		param2 = (xx & 0x20) ? lpTracks[pos++] : 0;

		if (command1 == 0x0E && (param1 & 0xF0) == 0xF0
		    && !command2) {
		    param1 = ((param1 & 0x0F) << 8) | param2;
		    command1 = 0xEF;
		    command2 = param2 = 0;
		}
		if (volume) {
		    cmd.volcmd = VOLCMD_VOLUME;
		    cmd.vol = (volume + 1) >> 2;
		}
		ConvertMDLCommand(&cmd, command1, param1);
		if ((cmd.command != CMD_SPEED)
		    && (cmd.command != CMD_TEMPO)
		    && (cmd.command != CMD_PATTERNBREAK))
		    ConvertMDLCommand(&cmd, command2, param2);
		*m = cmd;
		m += nChannels;
		row++;
	    }
		break;

		// Empty Slots
	    default:
		row += xx + 1;
		m += (xx + 1) * nChannels;
		if (row >= nRows)
		    break;
	}
    }
}

int CSoundFile_ReadMDL(CSoundFile* that, const uint8_t * lpStream, uint32_t dwMemLength)
//---------------------------------------------------------------
{
    uint32_t dwMemPos, dwPos, blocklen, dwTrackPos;
    const MDLSONGHEADER *pmsh = (const MDLSONGHEADER *)lpStream;
    MDLINFOBLOCK *pmib;
    MDLPATTERNDATA *pmpd;
    unsigned i, j, norders = 0, npatterns = 0, ntracks = 0;
    unsigned ninstruments = 0, nsamples = 0;
    uint16_t block;
    uint16_t patterntracks[MAX_PATTERNS * 32];
    uint8_t smpinfo[MAX_SAMPLES];
    uint8_t insvolenv[MAX_INSTRUMENTS];
    uint8_t inspanenv[MAX_INSTRUMENTS];
    const uint8_t *pvolenv;
    const uint8_t *ppanenv;
    const uint8_t *ppitchenv;
    unsigned nvolenv, npanenv, npitchenv;

    if ((!lpStream) || (dwMemLength < 1024))
	return 0;
    if ((pmsh->id != 0x4C444D44) || ((pmsh->version & 0xF0) > 0x10))
	return 0;
    memset(patterntracks, 0, sizeof(patterntracks));
    memset(smpinfo, 0, sizeof(smpinfo));
    memset(insvolenv, 0, sizeof(insvolenv));
    memset(inspanenv, 0, sizeof(inspanenv));
    dwMemPos = 5;
    dwTrackPos = 0;
    pvolenv = ppanenv = ppitchenv = NULL;
    nvolenv = npanenv = npitchenv = 0;
    that->m_nSamples = that->m_nInstruments = 0;
    while (dwMemPos + 6 < dwMemLength) {
	block = *((uint16_t *) (lpStream + dwMemPos));
	blocklen = *((uint32_t *) (lpStream + dwMemPos + 2));
	dwMemPos += 6;
	if (dwMemPos + blocklen > dwMemLength) {
	    if (dwMemPos == 11)
		return 0;
	    break;
	}
	switch (block) {
		// IN: infoblock
	    case 0x4E49:
		pmib = (MDLINFOBLOCK *) (lpStream + dwMemPos);
		memcpy(that->m_szNames[0], pmib->songname, 32);
		norders = pmib->norders;
		if (norders > MAX_ORDERS)
		    norders = MAX_ORDERS;
		that->m_nRestartPos = pmib->repeatpos;
		that->m_nDefaultGlobalVolume = pmib->globalvol;
		that->m_nDefaultTempo = pmib->tempo;
		that->m_nDefaultSpeed = pmib->speed;
		that->m_nChannels = 4;
		for (i = 0; i < 32; i++) {
		    that->ChnSettings[i].nVolume = 64;
		    that->ChnSettings[i].nPan = (pmib->channelinfo[i] & 0x7F) << 1;
		    if (pmib->channelinfo[i] & 0x80)
			that->ChnSettings[i].dwFlags |= CHN_MUTE;
		    else
			that->m_nChannels = i + 1;
		}
		for (i = 0; i < norders; i++) {
		    that->Order[i] = pmib->seq[i];
		}
		break;
		// ME: song message
	    case 0x454D:
		if (blocklen) {
		    if (that->m_lpszSongComments) {
			free(that->m_lpszSongComments);
		    }
		    that->m_lpszSongComments = (char*)malloc(blocklen);

		    if (that->m_lpszSongComments) {
			memcpy(that->m_lpszSongComments, lpStream + dwMemPos,
			    blocklen);
			that->m_lpszSongComments[blocklen - 1] = 0;
		    }
		}
		break;
		// PA: Pattern Data
	    case 0x4150:
		npatterns = lpStream[dwMemPos];
		if (npatterns > MAX_PATTERNS)
		    npatterns = MAX_PATTERNS;
		dwPos = dwMemPos + 1;
		for (i = 0; i < npatterns; i++) {
		    if (dwPos + 18 >= dwMemLength)
			break;
		    pmpd = (MDLPATTERNDATA *) (lpStream + dwPos);
		    if (pmpd->channels > 32)
			break;
		    that->PatternSize[i] = pmpd->lastrow + 1;
		    if (that->m_nChannels < pmpd->channels)
			that->m_nChannels = pmpd->channels;
		    dwPos += 18 + 2 * pmpd->channels;
		    for (j = 0; j < pmpd->channels; j++) {
			patterntracks[i * 32 + j] = pmpd->data[j];
		    }
		}
		break;
		// TR: Track Data
	    case 0x5254:
		if (dwTrackPos)
		    break;
		ntracks = *((uint16_t *) (lpStream + dwMemPos));
		dwTrackPos = dwMemPos + 2;
		break;
		// II: Instruments
	    case 0x4949:
		ninstruments = lpStream[dwMemPos];
		dwPos = dwMemPos + 1;
		for (i = 0; i < ninstruments; i++) {
		    unsigned nins = lpStream[dwPos];

		    if ((nins >= MAX_INSTRUMENTS) || (!nins))
			break;
		    if (that->m_nInstruments < nins)
			that->m_nInstruments = nins;
		    if (!that->Headers[nins]) {
			INSTRUMENTHEADER *penv;
			unsigned note;

			if ((that->Headers[nins]
				= malloc(sizeof(INSTRUMENTHEADER))) == NULL) {
			    break;
			}
			// FIXME: johns: use calloc
			penv = that->Headers[nins];
			memset(penv, 0, sizeof(INSTRUMENTHEADER));
			note = 12;

			memcpy(penv->Name, lpStream + dwPos + 2, 32);
			penv->nGlobalVol = 64;
			penv->nPPC = 5 * 12;
			for (j = 0; j < lpStream[dwPos + 1]; j++) {
			    const uint8_t *ps = lpStream + dwPos + 34 + 14 * j;

			    while ((note < (unsigned)(ps[1] + 12))
				&& (note < 120)) {
				penv->NoteMap[note] = note + 1;
				if (ps[0] < MAX_SAMPLES) {
				    int ismp = ps[0];

				    penv->Keyboard[note] = ps[0];
				    that->Ins[ismp].nVolume = ps[2];
				    that->Ins[ismp].nPan = ps[4] << 1;
				    that->Ins[ismp].nVibType = ps[11];
				    that->Ins[ismp].nVibSweep = ps[10];
				    that->Ins[ismp].nVibDepth = ps[9];
				    that->Ins[ismp].nVibRate = ps[8];
				}
				penv->nFadeOut = (ps[7] << 8) | ps[6];
				if (penv->nFadeOut == 0xFFFF)
				    penv->nFadeOut = 0;
				note++;
			    }
			    // Use volume envelope ?
			    if (ps[3] & 0x80) {
				penv->dwFlags |= ENV_VOLUME;
				insvolenv[nins] = (ps[3] & 0x3F) + 1;
			    }
			    // Use panning envelope ?
			    if (ps[5] & 0x80) {
				penv->dwFlags |= ENV_PANNING;
				inspanenv[nins] = (ps[5] & 0x3F) + 1;
			    }
			}
		    }
		    dwPos += 34 + 14 * lpStream[dwPos + 1];
		}
		for (j = 1; j <= that->m_nInstruments; j++)
		    if (!that->Headers[j]) {
			// FIXME: johns: use calloc
			that->Headers[j] = malloc(sizeof(INSTRUMENTHEADER));
			if (that->Headers[j]) {
			    memset(that->Headers[j], 0, sizeof(INSTRUMENTHEADER));
			}
		    }
		break;
		// VE: Volume Envelope
	    case 0x4556:
		if ((nvolenv = lpStream[dwMemPos]) == 0)
		    break;
		if (dwMemPos + nvolenv * 32 + 1 <= dwMemLength)
		    pvolenv = lpStream + dwMemPos + 1;
		break;
		// PE: Panning Envelope
	    case 0x4550:
		if ((npanenv = lpStream[dwMemPos]) == 0)
		    break;
		if (dwMemPos + npanenv * 32 + 1 <= dwMemLength)
		    ppanenv = lpStream + dwMemPos + 1;
		break;
		// FE: Pitch Envelope
	    case 0x4546:
		if ((npitchenv = lpStream[dwMemPos]) == 0)
		    break;
		if (dwMemPos + npitchenv * 32 + 1 <= dwMemLength)
		    ppitchenv = lpStream + dwMemPos + 1;
		break;
		// IS: Sample Infoblock
	    case 0x5349:
		nsamples = lpStream[dwMemPos];
		dwPos = dwMemPos + 1;
		for (i = 0; i < nsamples; i++, dwPos += 59) {
		    unsigned nins = lpStream[dwPos];
		    MODINSTRUMENT *pins;

		    if (nins >= MAX_SAMPLES || !nins) {
			continue;
		    }
		    if (that->m_nSamples < nins) {
			that->m_nSamples = nins;
		    }
		    pins = &that->Ins[nins];

		    memcpy(that->m_szNames[nins], lpStream + dwPos + 1, 32);
		    memcpy(pins->Name, lpStream + dwPos + 33, 8);
		    pins->nC4Speed = *((uint32_t *) (lpStream + dwPos + 41));
		    pins->nLength = *((uint32_t *) (lpStream + dwPos + 45));
		    pins->nLoopStart = *((uint32_t *) (lpStream + dwPos + 49));
		    pins->nLoopEnd =
			pins->nLoopStart + *((uint32_t *) (lpStream + dwPos +
			    53));
		    if (pins->nLoopEnd > pins->nLoopStart) {
			pins->uFlags |= CHN_LOOP;
		    }
		    pins->nGlobalVol = 64;
		    if (lpStream[dwPos + 58] & 0x01) {
			pins->uFlags |= CHN_16BIT;
			pins->nLength >>= 1;
			pins->nLoopStart >>= 1;
			pins->nLoopEnd >>= 1;
		    }
		    if (lpStream[dwPos + 58] & 0x02) {
			pins->uFlags |= CHN_PINGPONGLOOP;
		    }
		    smpinfo[nins] = (lpStream[dwPos + 58] >> 2) & 3;
		}
		break;
		// SA: Sample Data
	    case 0x4153:
		dwPos = dwMemPos;
		for (i = 1; i <= that->m_nSamples; i++)
		    if ((that->Ins[i].nLength) && (!that->Ins[i].pSample)
			&& (smpinfo[i] != 3) && (dwPos < dwMemLength)) {
			MODINSTRUMENT *pins = &that->Ins[i];
			unsigned flags =
			    (pins->uFlags & CHN_16BIT) ? RS_PCM16S : RS_PCM8S;
			if (!smpinfo[i]) {
			    dwPos +=
				CSoundFile_ReadSample(that,pins, flags,
				(char *)(lpStream + dwPos),
				dwMemLength - dwPos);
			} else {
			    uint32_t dwLen =
				*((uint32_t *) (lpStream + dwPos));
			    dwPos += 4;
			    if ((dwPos + dwLen <= dwMemLength) && (dwLen > 4)) {
				flags =
				    (pins->
				    uFlags & CHN_16BIT) ? RS_MDL16 : RS_MDL8;
				CSoundFile_ReadSample(that,pins, flags,
				    (char *)(lpStream + dwPos), dwLen);
			    }
			    dwPos += dwLen;
			}
		    }
		break;
	}
	dwMemPos += blocklen;
    }
    // Unpack Patterns
    if ((dwTrackPos) && (npatterns) && (that->m_nChannels) && (ntracks)) {
	for (i = 0; i < npatterns; i++) {
	    if ((that->Patterns[i] =
		    CSoundFile_AllocatePattern(that->PatternSize[i],
			that->m_nChannels)) == NULL) {
		break;
	    }
	    for (j = 0; j < that->m_nChannels; j++)
		if ((patterntracks[i * 32 + j])
		    && (patterntracks[i * 32 + j] <= ntracks)) {
		    MODCOMMAND *m = that->Patterns[i] + j;

		    UnpackMDLTrack(m, that->m_nChannels, that->PatternSize[i],
			patterntracks[i * 32 + j], lpStream + dwTrackPos);
		}
	}
    }

    // Set up envelopes
    for (i = 1; i <= that->m_nInstruments; i++) {
	if (that->Headers[i]) {
	    INSTRUMENTHEADER *penv = that->Headers[i];

	    // Setup volume envelope
	    if ((nvolenv) && (pvolenv) && (insvolenv[i])) {
		const uint8_t *pve = pvolenv;

		for (j = 0; j < nvolenv; j++, pve += 33)
		    if (pve[0] + 1 == insvolenv[i]) {
			uint16_t vtick = 1;
			unsigned iv;

			penv->nVolEnv = 15;
			for (iv = 0; iv < 15; iv++) {
			    if (iv) {
				vtick += pve[iv * 2 + 1];
			    }
			    penv->VolPoints[iv] = vtick;
			    penv->VolEnv[iv] = pve[iv * 2 + 2];
			    if (!pve[iv * 2 + 1]) {
				penv->nVolEnv = iv + 1;
				break;
			    }
			}
			penv->nVolSustainBegin = penv->nVolSustainEnd =
			    pve[31] & 0x0F;
			if (pve[31] & 0x10)
			    penv->dwFlags |= ENV_VOLSUSTAIN;
			if (pve[31] & 0x20)
			    penv->dwFlags |= ENV_VOLLOOP;
			penv->nVolLoopStart = pve[32] & 0x0F;
			penv->nVolLoopEnd = pve[32] >> 4;
		    }
	    }
	    // Setup panning envelope
	    if ((npanenv) && (ppanenv) && (inspanenv[i])) {
		const uint8_t *ppe = ppanenv;

		for (j = 0; j < npanenv; j++, ppe += 33)
		    if (ppe[0] + 1 == inspanenv[i]) {
			unsigned iv;
			uint16_t vtick = 1;

			penv->nPanEnv = 15;
			for (iv = 0; iv < 15; iv++) {
			    if (iv)
				vtick += ppe[iv * 2 + 1];
			    penv->PanPoints[iv] = vtick;
			    penv->PanEnv[iv] = ppe[iv * 2 + 2];
			    if (!ppe[iv * 2 + 1]) {
				penv->nPanEnv = iv + 1;
				break;
			    }
			}
			if (ppe[31] & 0x10)
			    penv->dwFlags |= ENV_PANSUSTAIN;
			if (ppe[31] & 0x20)
			    penv->dwFlags |= ENV_PANLOOP;
			penv->nPanLoopStart = ppe[32] & 0x0F;
			penv->nPanLoopEnd = ppe[32] >> 4;
		    }
	    }
	}
    }
    that->m_dwSongFlags |= SONG_LINEARSLIDES;
    that->m_nType = MOD_TYPE_MDL;
    return 1;
}

/////////////////////////////////////////////////////////////////////////
// MDL Sample Unpacking

// MDL Huffman ReadBits compression
uint16_t MDLReadBits(uint32_t * bitbuf, unsigned *bitnum, uint8_t ** ibuf,
    int8_t n)
//-----------------------------------------------------------------
{
    uint16_t v = (uint16_t) (*bitbuf & ((1 << n) - 1));

    *bitbuf >>= n;
    *bitnum -= n;
    if (*bitnum <= 24) {
	*bitbuf |= (((uint32_t) *((*ibuf)++))) << *bitnum;
	*bitnum += 8;
    }
    return v;
}
