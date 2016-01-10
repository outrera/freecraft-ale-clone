/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: load_mt2.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

//#define MT2DEBUG

#pragma pack(1)

typedef struct _MT2FILEHEADER
{
    uint32_t dwMT20;			// 0x3032544D "MT20"
    uint32_t dwSpecial;
    uint16_t wVersion;
    int8_t szTrackerName[32];		// "MadTracker 2.0"
    int8_t szSongName[64];
    uint16_t nOrders;
    uint16_t wRestart;
    uint16_t wPatterns;
    uint16_t wChannels;
    uint16_t wSamplesPerTick;
    uint8_t bTicksPerLine;
    uint8_t bLinesPerBeat;
    uint32_t fulFlags;			// b0=packed patterns
    uint16_t wInstruments;
    uint16_t wSamples;
    uint8_t Orders[256];
} MT2FILEHEADER;

typedef struct _MT2PATTERN
{
    uint16_t wLines;
    uint32_t wDataLen;
} MT2PATTERN;

typedef struct _MT2COMMAND
{
    uint8_t note;			// 0=nothing, 97=note off
    uint8_t instr;
    uint8_t vol;
    uint8_t pan;
    uint8_t fxcmd;
    uint8_t fxparam1;
    uint8_t fxparam2;
} MT2COMMAND;

typedef struct _MT2DRUMSDATA
{
    uint16_t wDrumPatterns;
    uint16_t wDrumSamples[8];
    uint8_t DrumPatternOrder[256];
} MT2DRUMSDATA;

typedef struct _MT2AUTOMATION
{
    uint32_t dwFlags;
    uint32_t dwEffectId;
    uint32_t nEnvPoints;
} MT2AUTOMATION;

typedef struct _MT2INSTRUMENT
{
    int8_t szName[32];
    uint32_t dwDataLen;
    uint16_t wSamples;
    uint8_t GroupsMapping[96];
    uint8_t bVibType;
    uint8_t bVibSweep;
    uint8_t bVibDepth;
    uint8_t bVibRate;
    uint16_t wFadeOut;
    uint16_t wNNA;
    uint16_t wInstrFlags;
    uint16_t wEnvFlags1;
    uint16_t wEnvFlags2;
} MT2INSTRUMENT;

typedef struct _MT2ENVELOPE
{
    uint8_t nFlags;
    uint8_t nPoints;
    uint8_t nSustainPos;
    uint8_t nLoopStart;
    uint8_t nLoopEnd;
    uint8_t bReserved[3];
    uint8_t EnvData[64];
} MT2ENVELOPE;

typedef struct _MT2SYNTH
{
    uint8_t nSynthId;
    uint8_t nFxId;
    uint16_t wCutOff;
    uint8_t nResonance;
    uint8_t nAttack;
    uint8_t nDecay;
    uint8_t bReserved[25];
} MT2SYNTH;

typedef struct _MT2SAMPLE
{
    int8_t szName[32];
    uint32_t dwDataLen;
    uint32_t dwLength;
    uint32_t dwFrequency;
    uint8_t nQuality;
    uint8_t nChannels;
    uint8_t nFlags;
    uint8_t nLoop;
    uint32_t dwLoopStart;
    uint32_t dwLoopEnd;
    uint16_t wVolume;
    uint8_t nPan;
    uint8_t nBaseNote;
    uint16_t wSamplesPerBeat;
} MT2SAMPLE;

typedef struct _MT2GROUP
{
    uint8_t nSmpNo;
    uint8_t nVolume;			// 0-128
    uint8_t nFinePitch;
    uint8_t Reserved[5];
} MT2GROUP;

#pragma pack()

static void ConvertMT2Command(CSoundFile const * that, MODCOMMAND * m,
    MT2COMMAND * p)
//---------------------------------------------------------------------------
{
    // Note
    m->note = 0;
    if (p->note) {
	m->note = (p->note > 96) ? 0xFF : p->note + 12;
    }
    // Instrument
    m->instr = p->instr;
    // Volume Column
    if ((p->vol >= 0x10) && (p->vol <= 0x90)) {
	m->volcmd = VOLCMD_VOLUME;
	m->vol = (p->vol - 0x10) >> 1;
    } else if ((p->vol >= 0xA0) && (p->vol <= 0xAF)) {
	m->volcmd = VOLCMD_VOLSLIDEDOWN;
	m->vol = (p->vol & 0x0f);
    } else if ((p->vol >= 0xB0) && (p->vol <= 0xBF)) {
	m->volcmd = VOLCMD_VOLSLIDEUP;
	m->vol = (p->vol & 0x0f);
    } else if ((p->vol >= 0xC0) && (p->vol <= 0xCF)) {
	m->volcmd = VOLCMD_FINEVOLDOWN;
	m->vol = (p->vol & 0x0f);
    } else if ((p->vol >= 0xD0) && (p->vol <= 0xDF)) {
	m->volcmd = VOLCMD_FINEVOLUP;
	m->vol = (p->vol & 0x0f);
    } else {
	m->volcmd = 0;
	m->vol = 0;
    }
    // Effects
    m->command = 0;
    m->param = 0;
    if ((p->fxcmd) || (p->fxparam1) || (p->fxparam2)) {
	if (!p->fxcmd) {
	    m->command = p->fxparam2;
	    m->param = p->fxparam1;
	    CSoundFile_ConvertModCommand(that, m);
	} else {
	    // TODO: MT2 Effects
	}
    }
}

int CSoundFile_ReadMT2(CSoundFile * that, const uint8_t * lpStream, uint32_t dwMemLength)
//-----------------------------------------------------------
{
    MT2FILEHEADER *pfh = (MT2FILEHEADER *) lpStream;
    uint32_t dwMemPos, dwDrumDataPos, dwExtraDataPos;
    unsigned nDrumDataLen, nExtraDataLen;
    MT2DRUMSDATA *pdd;
    MT2INSTRUMENT *InstrMap[255];
    MT2SAMPLE *SampleMap[256];
    unsigned i;
    unsigned j;

    if ((!lpStream) || (dwMemLength < sizeof(MT2FILEHEADER))
	|| (pfh->dwMT20 != 0x3032544D)
	|| (pfh->wVersion < 0x0200) || (pfh->wVersion >= 0x0300)
	|| (pfh->wChannels < 4) || (pfh->wChannels > 64)) {
	return 0;
    }
    pdd = NULL;
    that->m_nType = MOD_TYPE_MT2;
    that->m_nChannels = pfh->wChannels;
    that->m_nRestartPos = pfh->wRestart;
    that->m_nDefaultSpeed = pfh->bTicksPerLine;
    that->m_nDefaultTempo = 125;
    if ((pfh->wSamplesPerTick > 100) && (pfh->wSamplesPerTick < 5000)) {
	that->m_nDefaultTempo = 110250 / pfh->wSamplesPerTick;
    }
    for (i = 0; i < MAX_ORDERS; i++) {
	that->Order[i] =
	    (uint8_t) ((i < pfh->nOrders) ? pfh->Orders[i] : 0xFF);
    }
    memcpy(that->m_szNames[0], pfh->szSongName, 32);
    that->m_szNames[0][31] = 0;
    dwMemPos = sizeof(MT2FILEHEADER);
    nDrumDataLen = *(uint16_t *) (lpStream + dwMemPos);
    dwDrumDataPos = dwMemPos + 2;
    if (nDrumDataLen >= 2)
	pdd = (MT2DRUMSDATA *) (lpStream + dwDrumDataPos);
    dwMemPos += 2 + nDrumDataLen;
#ifdef MT2DEBUG

    Log("MT2 v%03X: \"%s\" (flags=%04X)\n", pfh->wVersion, that->m_szNames[0],
	pfh->fulFlags);
    Log("%d Channels, %d Patterns, %d Instruments, %d Samples\n",
	pfh->wChannels, pfh->wPatterns, pfh->wInstruments, pfh->wSamples);
    Log("Drum Data: %d bytes @%04X\n", nDrumDataLen, dwDrumDataPos);
#endif
    if (dwMemPos >= dwMemLength - 12)
	return 1;
    if (!*(uint32_t *) (lpStream + dwMemPos))
	dwMemPos += 4;
    if (!*(uint32_t *) (lpStream + dwMemPos))
	dwMemPos += 4;
    nExtraDataLen = *(uint32_t *) (lpStream + dwMemPos);
    dwExtraDataPos = dwMemPos + 4;
    dwMemPos += 4;
#ifdef MT2DEBUG
    Log("Extra Data: %d bytes @%04X\n", nExtraDataLen, dwExtraDataPos);
#endif
    if (dwMemPos + nExtraDataLen >= dwMemLength)
	return 1;
    while (dwMemPos + 8 < dwExtraDataPos + nExtraDataLen) {
	uint32_t dwId = *(uint32_t *) (lpStream + dwMemPos);
	uint32_t dwLen = *(uint32_t *) (lpStream + dwMemPos + 4);

	dwMemPos += 8;
	if (dwMemPos + dwLen > dwMemLength)
	    return 1;
#ifdef MT2DEBUG
	int8_t s[5];

	memcpy(s, &dwId, 4);
	s[4] = 0;
	Log("pos=0x%04X: %s: %d bytes\n", dwMemPos - 8, s, dwLen);
#endif
	switch (dwId) {
		// MSG
	    case 0x0047534D:
		if ((dwLen > 3) && (!that->m_lpszSongComments)) {
		    uint32_t nTxtLen = dwLen;

		    if (nTxtLen > 32000)
			nTxtLen = 32000;
		    that->m_lpszSongComments = malloc(nTxtLen);

		    if (that->m_lpszSongComments) {
			memcpy(that->m_lpszSongComments,
			    lpStream + dwMemPos + 1, nTxtLen - 1);
			that->m_lpszSongComments[nTxtLen - 1] = 0;
		    }
		}
		break;
		// SUM -> author name (or "Unregistered")
		// TMAP
		// TRKS
	    case 0x534b5254:
		break;
	}
	dwMemPos += dwLen;
    }
    // Load Patterns
    dwMemPos = dwExtraDataPos + nExtraDataLen;
    for (i = 0; i < pfh->wPatterns; i++)
	if (dwMemPos < dwMemLength - 6) {
	    MT2PATTERN *pmp = (MT2PATTERN *) (lpStream + dwMemPos);
	    unsigned wDataLen = (pmp->wDataLen + 1) & ~1;
	    unsigned nLines;

	    dwMemPos += 6;
	    if (dwMemPos + wDataLen > dwMemLength) {
		break;
	    }

	    nLines = pmp->wLines;
	    if ((i < MAX_PATTERNS) && (nLines > 0) && (nLines <= 256)) {
		MODCOMMAND *m;
		unsigned len;
#ifdef MT2DEBUG
		Log("Pattern #%d @%04X: %d lines, %d bytes\n", i,
		    dwMemPos - 6, nLines, pmp->wDataLen);
#endif
		that->PatternSize[i] = nLines;
		that->Patterns[i] =
		    CSoundFile_AllocatePattern(nLines, that->m_nChannels);
		if (!that->Patterns[i])
		    return 1;
		m = that->Patterns[i];
		len = wDataLen;

		if (pfh->fulFlags & 1)	// Packed Patterns
		{
		    uint8_t *p = (uint8_t *) (lpStream + dwMemPos);
		    unsigned pos = 0, row = 0, ch = 0;

		    while (pos < len) {
			MT2COMMAND cmd;
			unsigned infobyte = p[pos++];
			unsigned rptcount = 0;

			if (infobyte == 0xff) {
			    rptcount = p[pos++];
			    infobyte = p[pos++];
#if 0
			    Log("(%d.%d) FF(%02X).%02X\n", row, ch, rptcount,
				infobyte);
			} else {
			    Log("(%d.%d) %02X\n", row, ch, infobyte);
#endif
			}
			if (infobyte & 0x7f) {
			    unsigned patpos = row * that->m_nChannels + ch;

			    cmd.note = cmd.instr = cmd.vol = cmd.pan =
				cmd.fxcmd = cmd.fxparam1 = cmd.fxparam2 = 0;
			    if (infobyte & 1)
				cmd.note = p[pos++];
			    if (infobyte & 2)
				cmd.instr = p[pos++];
			    if (infobyte & 4)
				cmd.vol = p[pos++];
			    if (infobyte & 8)
				cmd.pan = p[pos++];
			    if (infobyte & 16)
				cmd.fxcmd = p[pos++];
			    if (infobyte & 32)
				cmd.fxparam1 = p[pos++];
			    if (infobyte & 64)
				cmd.fxparam2 = p[pos++];
#ifdef MT2DEBUG
			    if (cmd.fxcmd) {
				Log("(%d.%d) MT2 FX=%02X.%02X.%02X\n", row, ch,
				    cmd.fxcmd, cmd.fxparam1, cmd.fxparam2);
			    }
#endif
			    ConvertMT2Command(that, &m[patpos], &cmd);
			}
			row += rptcount + 1;
			while (row >= nLines) {
			    row -= nLines;
			    ch++;
			}
			if (ch >= that->m_nChannels)
			    break;
		    }
		} else {
		    MT2COMMAND *p = (MT2COMMAND *) (lpStream + dwMemPos);
		    unsigned n = 0;

		    while ((len > sizeof(MT2COMMAND))
			&& (n < that->m_nChannels * nLines)) {
			ConvertMT2Command(that, m, p);
			len -= sizeof(MT2COMMAND);
			n++;
			p++;
			m++;
		    }
		}
	    }
	    dwMemPos += wDataLen;
	}
    // Skip Drum Patterns
    if (pdd) {
#ifdef MT2DEBUG
	Log("%d Drum Patterns at offset 0x%08X\n", pdd->wDrumPatterns,
	    dwMemPos);
#endif
	for (i = 0; i < pdd->wDrumPatterns; i++) {
	    unsigned nLines;

	    if (dwMemPos > dwMemLength - 2) {
		return 1;
	    }

	    nLines = *(uint16_t *) (lpStream + dwMemPos);
#ifdef MT2DEBUG
	    if (nLines != 64)
		Log("Drum Pattern %d: %d Lines @%04X\n", i, nLines,
		    dwMemPos);
#endif
	    dwMemPos += 2 + nLines * 32;
	}
    }
    // Automation
    if (pfh->fulFlags & 2) {
	unsigned nAutoCount = that->m_nChannels;

#ifdef MT2DEBUG
	Log("Automation at offset 0x%08X\n", dwMemPos);
#endif

	if (pfh->fulFlags & 0x10)
	    nAutoCount++;		// Master Automation
	if ((pfh->fulFlags & 0x08) && (pdd))
	    nAutoCount += 8;		// Drums Automation
	nAutoCount *= pfh->wPatterns;
	for (i = 0; i < nAutoCount; i++) {
	    MT2AUTOMATION *pma;

	    if (dwMemPos + 12 >= dwMemLength) {
		return 1;
	    }

	    pma = (MT2AUTOMATION *) (lpStream + dwMemPos);
	    dwMemPos += (pfh->wVersion <= 0x201) ? 4 : 8;
	    for (j = 0; j < 14; j++) {
		if (pma->dwFlags & (1 << j)) {
#ifdef MT2DEBUG
		    unsigned nPoints = *(uint32_t *) (lpStream + dwMemPos);

		    Log("  Env[%d/%d] %04X @%04X: %d points\n", i,
			nAutoCount, 1 << j, dwMemPos - 8, nPoints);
#endif
		    dwMemPos += 260;
		}
	    }
	}
    }
    // Load Instruments
#ifdef MT2DEBUG
    Log("Loading instruments at offset 0x%08X\n", dwMemPos);
#endif
    memset(InstrMap, 0, sizeof(InstrMap));
    that->m_nInstruments =
	(pfh->wInstruments <
	MAX_INSTRUMENTS) ? pfh->wInstruments : MAX_INSTRUMENTS - 1;
    for (i = 1; i <= 255; i++) {
	MT2INSTRUMENT *pmi;
	INSTRUMENTHEADER *penv;

	if (dwMemPos + 36 > dwMemLength) {
	    return 1;
	}

	pmi = (MT2INSTRUMENT *) (lpStream + dwMemPos);
	if (i <= that->m_nInstruments) {
	    penv = malloc(sizeof(INSTRUMENTHEADER));
	    if (penv) {
		that->Headers[i] = penv;
		memset(penv, 0, sizeof(INSTRUMENTHEADER));
		memcpy(penv->Name, pmi->szName, 32);
		penv->nGlobalVol = 64;
		penv->nPan = 128;
		for (j = 0; j < 120; j++) {
		    penv->NoteMap[j] = j + 1;
		}
	    }
	} else {
	    penv = NULL;
	}
#ifdef MT2DEBUG
	if (i <= pfh->wInstruments)
	    Log("  Instrument #%d at offset %04X: %d bytes\n", i, dwMemPos,
		pmi->dwDataLen);
#endif
	if (((long)pmi->dwDataLen > 0)
	    && (dwMemPos + pmi->dwDataLen + 40 <= dwMemLength)) {
	    InstrMap[i - 1] = pmi;
	    if (penv) {
		MT2ENVELOPE *pehdr[4];
		uint16_t *pedata[4];
		unsigned iEnv;

		penv->nFadeOut = pmi->wFadeOut;
		penv->nNNA = pmi->wNNA & 3;
		penv->nDCT = (pmi->wNNA >> 8) & 3;
		penv->nDNA = (pmi->wNNA >> 12) & 3;

		if (pfh->wVersion <= 0x201) {
		    uint32_t dwEnvPos = dwMemPos + sizeof(MT2INSTRUMENT) - 4;

		    pehdr[0] = (MT2ENVELOPE *) (lpStream + dwEnvPos);
		    pehdr[1] = (MT2ENVELOPE *) (lpStream + dwEnvPos + 8);
		    pehdr[2] = pehdr[3] = NULL;
		    pedata[0] = (uint16_t *) (lpStream + dwEnvPos + 16);
		    pedata[1] = (uint16_t *) (lpStream + dwEnvPos + 16 + 64);
		    pedata[2] = pedata[3] = NULL;
		} else {
		    uint32_t dwEnvPos = dwMemPos + sizeof(MT2INSTRUMENT);

		    for (j = 0; j < 4; j++) {
			if (pmi->wEnvFlags1 & (1 << j)) {
			    pehdr[j] = (MT2ENVELOPE *) (lpStream + dwEnvPos);
			    pedata[j] = (uint16_t *) pehdr[j]->EnvData;
			    dwEnvPos += sizeof(MT2ENVELOPE);
			} else {
			    pehdr[j] = NULL;
			    pedata[j] = NULL;
			}
		    }
		}
		// Load envelopes
		for (iEnv = 0; iEnv < 4; iEnv++) {
		    if (pehdr[iEnv]) {
			MT2ENVELOPE *pme = pehdr[iEnv];
			uint16_t *pEnvPoints = NULL;
			uint8_t *pEnvData = NULL;

#ifdef MT2DEBUG
			Log("  Env %d.%d @%04X: %d points\n", i, iEnv,
			    (unsigned)(((uint8_t *) pme) - lpStream),
			    pme->nPoints);
#endif
			switch (iEnv) {
				// Volume Envelope
			    case 0:
				if (pme->nFlags & 1)
				    penv->dwFlags |= ENV_VOLUME;
				if (pme->nFlags & 2)
				    penv->dwFlags |= ENV_VOLSUSTAIN;
				if (pme->nFlags & 4)
				    penv->dwFlags |= ENV_VOLLOOP;
				penv->nVolEnv =
				    (pme->nPoints > 16) ? 16 : pme->nPoints;
				penv->nVolSustainBegin = penv->nVolSustainEnd =
				    pme->nSustainPos;
				penv->nVolLoopStart = pme->nLoopStart;
				penv->nVolLoopEnd = pme->nLoopEnd;
				pEnvPoints = penv->VolPoints;
				pEnvData = penv->VolEnv;
				break;

				// Panning Envelope
			    case 1:
				if (pme->nFlags & 1)
				    penv->dwFlags |= ENV_PANNING;
				if (pme->nFlags & 2)
				    penv->dwFlags |= ENV_PANSUSTAIN;
				if (pme->nFlags & 4)
				    penv->dwFlags |= ENV_PANLOOP;
				penv->nPanEnv =
				    (pme->nPoints > 16) ? 16 : pme->nPoints;
				penv->nPanSustainBegin = penv->nPanSustainEnd =
				    pme->nSustainPos;
				penv->nPanLoopStart = pme->nLoopStart;
				penv->nPanLoopEnd = pme->nLoopEnd;
				pEnvPoints = penv->PanPoints;
				pEnvData = penv->PanEnv;
				break;

				// Pitch/Filter envelope
			    default:
				if (pme->nFlags & 1)
				    penv->dwFlags |=
					(iEnv ==
					3) ? (ENV_PITCH | ENV_FILTER) :
					ENV_PITCH;
				if (pme->nFlags & 2)
				    penv->dwFlags |= ENV_PITCHSUSTAIN;
				if (pme->nFlags & 4)
				    penv->dwFlags |= ENV_PITCHLOOP;
				penv->nPitchEnv =
				    (pme->nPoints > 16) ? 16 : pme->nPoints;
				penv->nPitchSustainBegin =
				    penv->nPitchSustainEnd = pme->nSustainPos;
				penv->nPitchLoopStart = pme->nLoopStart;
				penv->nPitchLoopEnd = pme->nLoopEnd;
				pEnvPoints = penv->PitchPoints;
				pEnvData = penv->PitchEnv;
			}
			// Envelope data
			if ((pEnvPoints) && (pEnvData) && (pedata[iEnv])) {
			    uint16_t *psrc = pedata[iEnv];

			    for (j = 0; j < 16; j++) {
				pEnvPoints[j] = psrc[j * 2];
				pEnvData[j] = (uint8_t) psrc[j * 2 + 1];
			    }
			}
		    }
		}
	    }
	    dwMemPos += pmi->dwDataLen + 36;
	    if (pfh->wVersion > 0x201)
		dwMemPos += 4;		// ?
	} else {
	    dwMemPos += 36;
	}
    }
#ifdef MT2DEBUG
    Log("Loading samples at offset 0x%08X\n", dwMemPos);
#endif
    memset(SampleMap, 0, sizeof(SampleMap));
    that->m_nSamples =
	(pfh->wSamples < MAX_SAMPLES) ? pfh->wSamples : MAX_SAMPLES - 1;
    for (i = 1; i <= 256; i++) {
	MT2SAMPLE *pms;

	if (dwMemPos + 36 > dwMemLength) {
	    return 1;
	}

	pms = (MT2SAMPLE *) (lpStream + dwMemPos);

#ifdef MT2DEBUG
	if (i <= that->m_nSamples)
	    Log("  Sample #%d at offset %04X: %d bytes\n", i, dwMemPos,
		pms->dwDataLen);
#endif
	if (i < MAX_SAMPLES) {
	    memcpy(that->m_szNames[i], pms->szName, 32);
	}
	if (pms->dwDataLen > 0) {
	    SampleMap[i - 1] = pms;
	    if (i < MAX_SAMPLES) {
		MODINSTRUMENT *psmp = &that->Ins[i];

		psmp->nGlobalVol = 64;
		psmp->nVolume = (pms->wVolume >> 7);
		psmp->nPan = (pms->nPan == 0x80) ? 128 : (pms->nPan ^ 0x80);
		psmp->nLength = pms->dwLength;
		psmp->nC4Speed = pms->dwFrequency;
		psmp->nLoopStart = pms->dwLoopStart;
		psmp->nLoopEnd = pms->dwLoopEnd;
		CSoundFile_FrequencyToTranspose_Instrument(psmp);
		psmp->RelativeTone -= pms->nBaseNote - 49;
		psmp->nC4Speed =
		    CSoundFile_TransposeToFrequency(psmp->RelativeTone, psmp->nFineTune);
		if (pms->nQuality == 2) {
		    psmp->uFlags |= CHN_16BIT;
		    psmp->nLength >>= 1;
		}
		if (pms->nChannels == 2) {
		    psmp->nLength >>= 1;
		}
		if (pms->nLoop == 1)
		    psmp->uFlags |= CHN_LOOP;
		if (pms->nLoop == 2)
		    psmp->uFlags |= CHN_LOOP | CHN_PINGPONGLOOP;
	    }
	    dwMemPos += pms->dwDataLen + 36;
	} else {
	    dwMemPos += 36;
	}
    }
#ifdef MT2DEBUG
    Log("Loading groups at offset 0x%08X\n", dwMemPos);
#endif
    for (i = 0; i < 255; i++) {
	if (InstrMap[i]) {
	    unsigned iGrp;
	    MT2INSTRUMENT *pmi;
	    INSTRUMENTHEADER *penv;

	    if (dwMemPos + 8 > dwMemLength) {
		return 1;
	    }
	    if (i < that->m_nInstruments) {
		penv = that->Headers[i + 1];
	    } else {
		penv = NULL;
	    }
	    pmi = InstrMap[i];
	    for (iGrp = 0; iGrp < pmi->wSamples; iGrp++) {
		if (penv) {
		    MT2GROUP *pmg = (MT2GROUP *) (lpStream + dwMemPos);

		    for (j = 0; j < 96; j++) {
			if (pmi->GroupsMapping[j] == iGrp) {
			    unsigned nSmp = pmg->nSmpNo + 1;

			    penv->Keyboard[j + 12] = (uint8_t) nSmp;
			    if (nSmp <= that->m_nSamples) {
				that->Ins[nSmp].nVibType = pmi->bVibType;
				that->Ins[nSmp].nVibSweep = pmi->bVibSweep;
				that->Ins[nSmp].nVibDepth = pmi->bVibDepth;
				that->Ins[nSmp].nVibRate = pmi->bVibRate;
			    }
			}
		    }
		}
		dwMemPos += 8;
	    }
	}
    }
#ifdef MT2DEBUG
    Log("Loading sample data at offset 0x%08X\n", dwMemPos);
#endif
    for (i = 0; i < 256; i++) {
	if ((i < that->m_nSamples) && (SampleMap[i])) {
	    MT2SAMPLE *pms = SampleMap[i];
	    MODINSTRUMENT *psmp = &that->Ins[i + 1];

	    if (!(pms->nFlags & 5)) {
		if (psmp->nLength > 0) {
#ifdef MT2DEBUG
		    Log("  Reading sample #%d at offset 0x%04X (len=%d)\n",
			i + 1, dwMemPos, psmp->nLength);
#endif
		    unsigned rsflags;

		    if (pms->nChannels == 2)
			rsflags =
			    (psmp->
			    uFlags & CHN_16BIT) ? RS_STPCM16D : RS_STPCM8D;
		    else
			rsflags =
			    (psmp->uFlags & CHN_16BIT) ? RS_PCM16D : RS_PCM8D;

		    dwMemPos +=
			CSoundFile_ReadSample(that,psmp, rsflags,
			(const char *)(lpStream + dwMemPos),
			dwMemLength - dwMemPos);
		}
	    } else if (dwMemPos + 4 < dwMemLength) {
		unsigned nNameLen = *(uint32_t *) (lpStream + dwMemPos);

		dwMemPos += nNameLen + 16;
	    }
	    if (dwMemPos + 4 >= dwMemLength) {
		break;
	    }
	}
    }
    return 1;
}
