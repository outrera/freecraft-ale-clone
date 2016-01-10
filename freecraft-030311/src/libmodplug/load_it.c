/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: load_it.c,v 1.2 2002/09/05 21:50:55 jsalmon3 Exp $
*/

#include "stdafx.h"
#include "sndfile.h"
#include "it_defs.h"

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif

static uint8_t autovibit2xm[8] =
{ 0, 3, 1, 4, 2, 0, 0, 0 };

#ifndef MODPLUG_NO_FILESAVE
static uint8_t autovibxm2it[8] =
{ 0, 2, 4, 1, 3, 0, 0, 0 };
#endif

//////////////////////////////////////////////////////////
// Impulse Tracker IT file support (import only)

static inline unsigned ConvertVolParam(unsigned value)
//--------------------------------------------
{
    return (value > 9) ? 9 : value;
}

static int ITInstrToMPT(const void *p, INSTRUMENTHEADER * penv,
    unsigned trkvers)
//------------------------------------------------------------------------------
{
    unsigned j;
    unsigned ev;

    if (trkvers < 0x0200) {
	const ITOLDINSTRUMENT *pis = (const ITOLDINSTRUMENT *)p;

	memcpy(penv->Name, pis->Name, 26);
	memcpy(penv->Filename, pis->Filename, 12);
	penv->nFadeOut = bswapLE16(pis->fadeout) << 6;
	penv->nGlobalVol = 64;
	for (j = 0; j < 120; j++) {
	    unsigned note = pis->keyboard[j * 2];
	    unsigned ins = pis->keyboard[j * 2 + 1];

	    if (ins < MAX_SAMPLES) {
		penv->Keyboard[j] = ins;
	    }
	    if (note < 128) {
		penv->NoteMap[j] = note + 1;
	    } else if (note >= 0xFE) {
		penv->NoteMap[j] = note;
	    }
	}
	if (pis->flags & 0x01) {
	    penv->dwFlags |= ENV_VOLUME;
	}
	if (pis->flags & 0x02) {
	    penv->dwFlags |= ENV_VOLLOOP;
	}
	if (pis->flags & 0x04) {
	    penv->dwFlags |= ENV_VOLSUSTAIN;
	}
	penv->nVolLoopStart = pis->vls;
	penv->nVolLoopEnd = pis->vle;
	penv->nVolSustainBegin = pis->sls;
	penv->nVolSustainEnd = pis->sle;
	penv->nVolEnv = 25;
	for (ev = 0; ev < 25; ev++) {
	    if ((penv->VolPoints[ev] = pis->nodes[ev * 2]) == 0xFF) {
		penv->nVolEnv = ev;
		break;
	    }
	    penv->VolEnv[ev] = pis->nodes[ev * 2 + 1];
	}
	penv->nNNA = pis->nna;
	penv->nDCT = pis->dnc;
	penv->nPan = 0x80;
    } else {
	const ITINSTRUMENT *pis = (const ITINSTRUMENT *)p;

	memcpy(penv->Name, pis->Name, 26);
	memcpy(penv->Filename, pis->Filename, 12);
	penv->nMidiProgram = pis->mpr;
	penv->nMidiChannel = pis->mch;
	penv->wMidiBank = bswapLE16(pis->mbank);
	penv->nFadeOut = bswapLE16(pis->fadeout) << 5;
	penv->nGlobalVol = pis->gbv >> 1;
	if (penv->nGlobalVol > 64) {
	    penv->nGlobalVol = 64;
	}
	for (j = 0; j < 120; j++) {
	    unsigned note = pis->keyboard[j * 2];
	    unsigned ins = pis->keyboard[j * 2 + 1];

	    if (ins < MAX_SAMPLES)
		penv->Keyboard[j] = ins;
	    if (note < 128)
		penv->NoteMap[j] = note + 1;
	    else if (note >= 0xFE)
		penv->NoteMap[j] = note;
	}
	// Volume Envelope
	if (pis->volenv.flags & 1)
	    penv->dwFlags |= ENV_VOLUME;
	if (pis->volenv.flags & 2)
	    penv->dwFlags |= ENV_VOLLOOP;
	if (pis->volenv.flags & 4)
	    penv->dwFlags |= ENV_VOLSUSTAIN;
	if (pis->volenv.flags & 8)
	    penv->dwFlags |= ENV_VOLCARRY;
	penv->nVolEnv = pis->volenv.num;
	if (penv->nVolEnv > 25)
	    penv->nVolEnv = 25;

	penv->nVolLoopStart = pis->volenv.lpb;
	penv->nVolLoopEnd = pis->volenv.lpe;
	penv->nVolSustainBegin = pis->volenv.slb;
	penv->nVolSustainEnd = pis->volenv.sle;
	// Panning Envelope
	if (pis->panenv.flags & 1)
	    penv->dwFlags |= ENV_PANNING;
	if (pis->panenv.flags & 2)
	    penv->dwFlags |= ENV_PANLOOP;
	if (pis->panenv.flags & 4)
	    penv->dwFlags |= ENV_PANSUSTAIN;
	if (pis->panenv.flags & 8)
	    penv->dwFlags |= ENV_PANCARRY;
	penv->nPanEnv = pis->panenv.num;
	if (penv->nPanEnv > 25)
	    penv->nPanEnv = 25;
	penv->nPanLoopStart = pis->panenv.lpb;
	penv->nPanLoopEnd = pis->panenv.lpe;
	penv->nPanSustainBegin = pis->panenv.slb;
	penv->nPanSustainEnd = pis->panenv.sle;
	// Pitch Envelope
	if (pis->pitchenv.flags & 1)
	    penv->dwFlags |= ENV_PITCH;
	if (pis->pitchenv.flags & 2)
	    penv->dwFlags |= ENV_PITCHLOOP;
	if (pis->pitchenv.flags & 4)
	    penv->dwFlags |= ENV_PITCHSUSTAIN;
	if (pis->pitchenv.flags & 8)
	    penv->dwFlags |= ENV_PITCHCARRY;
	if (pis->pitchenv.flags & 0x80)
	    penv->dwFlags |= ENV_FILTER;
	penv->nPitchEnv = pis->pitchenv.num;
	if (penv->nPitchEnv > 25)
	    penv->nPitchEnv = 25;
	penv->nPitchLoopStart = pis->pitchenv.lpb;
	penv->nPitchLoopEnd = pis->pitchenv.lpe;
	penv->nPitchSustainBegin = pis->pitchenv.slb;
	penv->nPitchSustainEnd = pis->pitchenv.sle;
	// Envelopes Data
	for (ev = 0; ev < 25; ev++) {
	    penv->VolEnv[ev] = pis->volenv.data[ev * 3];
	    penv->VolPoints[ev] =
		(pis->volenv.data[ev * 3 +
		    2] << 8) | (pis->volenv.data[ev * 3 + 1]);
	    penv->PanEnv[ev] = pis->panenv.data[ev * 3] + 32;
	    penv->PanPoints[ev] =
		(pis->panenv.data[ev * 3 +
		    2] << 8) | (pis->panenv.data[ev * 3 + 1]);
	    penv->PitchEnv[ev] = pis->pitchenv.data[ev * 3] + 32;
	    penv->PitchPoints[ev] =
		(pis->pitchenv.data[ev * 3 +
		    2] << 8) | (pis->pitchenv.data[ev * 3 + 1]);
	}
	penv->nNNA = pis->nna;
	penv->nDCT = pis->dct;
	penv->nDNA = pis->dca;
	penv->nPPS = pis->pps;
	penv->nPPC = pis->ppc;
	penv->nIFC = pis->ifc;
	penv->nIFR = pis->ifr;
	penv->nVolSwing = pis->rv;
	penv->nPanSwing = pis->rp;
	penv->nPan = (pis->dfp & 0x7F) << 2;
	if (penv->nPan > 256) {
	    penv->nPan = 128;
	}
	if (pis->dfp < 0x80) {
	    penv->dwFlags |= ENV_SETPANNING;
	}
    }
    if ((penv->nVolLoopStart >= 25) || (penv->nVolLoopEnd >= 25)) {
	penv->dwFlags &= ~ENV_VOLLOOP;
    }
    if ((penv->nVolSustainBegin >= 25) || (penv->nVolSustainEnd >= 25)) {
	penv->dwFlags &= ~ENV_VOLSUSTAIN;
    }
    return 1;
}

int CSoundFile_ReadIT(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//--------------------------------------------------------------
{
    ITFILEHEADER pifh = *(ITFILEHEADER *) lpStream;
    uint32_t dwMemPos = sizeof(ITFILEHEADER);
    uint32_t inspos[MAX_INSTRUMENTS];
    uint32_t smppos[MAX_SAMPLES];
    uint32_t patpos[MAX_PATTERNS];
    uint8_t chnmask[64], channels_used[64];
    MODCOMMAND lastvalue[64];
    int ipan;
    int tmp;
    unsigned j;
    unsigned npatterns;

    pifh.id = bswapLE32(pifh.id);
    pifh.reserved1 = bswapLE16(pifh.reserved1);
    pifh.ordnum = bswapLE16(pifh.ordnum);
    pifh.insnum = bswapLE16(pifh.insnum);
    pifh.smpnum = bswapLE16(pifh.smpnum);
    pifh.patnum = bswapLE16(pifh.patnum);
    pifh.cwtv = bswapLE16(pifh.cwtv);
    pifh.cmwt = bswapLE16(pifh.cmwt);
    pifh.flags = bswapLE16(pifh.flags);
    pifh.special = bswapLE16(pifh.special);
    pifh.msglength = bswapLE16(pifh.msglength);
    pifh.msgoffset = bswapLE32(pifh.msgoffset);
    pifh.reserved2 = bswapLE32(pifh.reserved2);

    if ((!lpStream) || (dwMemLength < 0x100))
	return 0;
    if ((pifh.id != 0x4D504D49) || (pifh.insnum >= MAX_INSTRUMENTS)
	|| (!pifh.smpnum) || (pifh.smpnum >= MAX_INSTRUMENTS)
	|| (!pifh.ordnum))
	return 0;
    if (dwMemPos + pifh.ordnum + pifh.insnum * 4 + pifh.smpnum * 4 +
	pifh.patnum * 4 > dwMemLength)
	return 0;
    that->m_nType = MOD_TYPE_IT;
    if (pifh.flags & 0x08)
	that->m_dwSongFlags |= SONG_LINEARSLIDES;
    if (pifh.flags & 0x10)
	that->m_dwSongFlags |= SONG_ITOLDEFFECTS;
    if (pifh.flags & 0x20)
	that->m_dwSongFlags |= SONG_ITCOMPATMODE;
    if (pifh.flags & 0x80)
	that->m_dwSongFlags |= SONG_EMBEDMIDICFG;
    if (pifh.flags & 0x1000)
	that->m_dwSongFlags |= SONG_EXFILTERRANGE;
    memcpy(that->m_szNames[0], pifh.songname, 26);
    that->m_szNames[0][26] = 0;
    // Global Volume
    if (pifh.globalvol) {
	that->m_nDefaultGlobalVolume = pifh.globalvol << 1;
	if (!that->m_nDefaultGlobalVolume)
	    that->m_nDefaultGlobalVolume = 256;
	if (that->m_nDefaultGlobalVolume > 256)
	    that->m_nDefaultGlobalVolume = 256;
    }
    if (pifh.speed)
	that->m_nDefaultSpeed = pifh.speed;
    if (pifh.tempo)
	that->m_nDefaultTempo = pifh.tempo;
    that->m_nSongPreAmp = pifh.mv & 0x7F;
    // Reading Channels Pan Positions
    for (ipan = 0; ipan < 64; ipan++) {
	if (pifh.chnpan[ipan] != 0xFF) {
	    unsigned n;

	    that->ChnSettings[ipan].nVolume = pifh.chnvol[ipan];
	    that->ChnSettings[ipan].nPan = 128;
	    if (pifh.chnpan[ipan] & 0x80) {
		that->ChnSettings[ipan].dwFlags |= CHN_MUTE;
	    }
	    n = pifh.chnpan[ipan] & 0x7F;

	    if (n <= 64)
		that->ChnSettings[ipan].nPan = n << 2;
	    if (n == 100)
		that->ChnSettings[ipan].dwFlags |= CHN_SURROUND;
	}
    }
    if (that->m_nChannels < 4) {
	that->m_nChannels = 4;
    }
    // Reading Song Message
    if ((pifh.special & 0x01) && (pifh.msglength)
	&& (pifh.msgoffset + pifh.msglength < dwMemLength)) {
	that->m_lpszSongComments = (char*)malloc(pifh.msglength + 1);

	if (that->m_lpszSongComments) {
	    memcpy(that->m_lpszSongComments, lpStream + pifh.msgoffset,
		pifh.msglength);
	    that->m_lpszSongComments[pifh.msglength] = 0;
	}
    }
    // Reading orders

    tmp = pifh.ordnum;
    if (tmp > MAX_ORDERS)
	tmp = MAX_ORDERS;
    memcpy(that->Order, lpStream + dwMemPos, tmp);

    dwMemPos += pifh.ordnum;
    // Reading Instrument Offsets
    memset(inspos, 0, sizeof(inspos));
    tmp = pifh.insnum;

    if (tmp > MAX_INSTRUMENTS)
	tmp = MAX_INSTRUMENTS;
    tmp <<= 2;
    memcpy(inspos, lpStream + dwMemPos, tmp);
    for (j = 0; j < ((unsigned)tmp >> 2); j++) {
	inspos[j] = bswapLE32(inspos[j]);
    }
    dwMemPos += pifh.insnum * 4;
    // Reading Samples Offsets
    memset(smppos, 0, sizeof(smppos));
    tmp = pifh.smpnum;

    if (tmp > MAX_SAMPLES)
	tmp = MAX_SAMPLES;
    tmp <<= 2;
    memcpy(smppos, lpStream + dwMemPos, tmp);
    for (j = 0; j < ((unsigned)tmp >> 2); j++) {
	smppos[j] = bswapLE32(smppos[j]);
    }
    dwMemPos += pifh.smpnum * 4;

    // Reading Patterns Offsets
    memset(patpos, 0, sizeof(patpos));
    tmp = pifh.patnum;

    if (tmp > MAX_PATTERNS) {
	tmp = MAX_PATTERNS;
    }
    tmp <<= 2;
    memcpy(patpos, lpStream + dwMemPos, tmp);
    for (j = 0; j < ((unsigned)tmp >> 2); j++) {
	patpos[j] = bswapLE32(patpos[j]);
    }
    dwMemPos += pifh.patnum * 4;
    // Reading IT Extra Info
    if (dwMemPos + 2 < dwMemLength) {
	unsigned nflt = bswapLE16(*((uint16_t *) (lpStream + dwMemPos)));

	dwMemPos += 2;
	if (dwMemPos + nflt * 8 < dwMemLength) {
	    dwMemPos += nflt * 8;
	}
    }
    // Reading Midi Output & Macros
    if (that->m_dwSongFlags & SONG_EMBEDMIDICFG) {
	if (dwMemPos + sizeof(MODMIDICFG) < dwMemLength) {
	    memcpy(&that->m_MidiCfg, lpStream + dwMemPos, sizeof(MODMIDICFG));
	    dwMemPos += sizeof(MODMIDICFG);
	}
    }
    // Read pattern names: "PNAM"
    if ((dwMemPos + 8 < dwMemLength)
	&& (bswapLE32(*((uint32_t *) (lpStream + dwMemPos))) == 0x4d414e50)) {
	unsigned len = bswapLE32(*((uint32_t *) (lpStream + dwMemPos + 4)));

	dwMemPos += 8;
	if ((dwMemPos + len <= dwMemLength)
	    && (len <= MAX_PATTERNS * MAX_PATTERNNAME)
	    && (len >= MAX_PATTERNNAME)) {

	    that->m_lpszPatternNames = malloc(len);
	    if (that->m_lpszPatternNames) {
		that->m_nPatternNames = len / MAX_PATTERNNAME;
		memcpy(that->m_lpszPatternNames, lpStream + dwMemPos, len);
	    }
	    dwMemPos += len;
	}
    }
    // 4-channels minimum
    that->m_nChannels = 4;
    // Read channel names: "CNAM"
    if ((dwMemPos + 8 < dwMemLength)
	&& (bswapLE32(*((uint32_t *) (lpStream + dwMemPos))) == 0x4d414e43)) {
	unsigned len = bswapLE32(*((uint32_t *) (lpStream + dwMemPos + 4)));

	dwMemPos += 8;
	if ((dwMemPos + len <= dwMemLength) && (len <= 64 * MAX_CHANNELNAME)) {
	    unsigned n = len / MAX_CHANNELNAME;
	    unsigned i;

	    if (n > that->m_nChannels)
		that->m_nChannels = n;
	    for (i = 0; i < n; i++) {
		memcpy(that->ChnSettings[i].szName,
		    (lpStream + dwMemPos + i * MAX_CHANNELNAME),
		    MAX_CHANNELNAME);
		that->ChnSettings[i].szName[MAX_CHANNELNAME - 1] = 0;
	    }
	    dwMemPos += len;
	}
    }
    // Read mix plugins information
    if (dwMemPos + 8 < dwMemLength) {
	dwMemPos +=
	    CSoundFile_LoadMixPlugins(that,lpStream + dwMemPos,
	    dwMemLength - dwMemPos);
    }
    // Checking for unused channels
    npatterns = pifh.patnum;

    if (npatterns > MAX_PATTERNS)
	npatterns = MAX_PATTERNS;
    for (j = 0; j < npatterns; j++) {
	unsigned i;
	const uint8_t *p;
	unsigned nrow;
	unsigned len;
	unsigned rows;

	memset(chnmask, 0, sizeof(chnmask));
	if ((!patpos[j])
		|| ((uint32_t) patpos[j] + 4 >= dwMemLength)) {
	    continue;
	}
	len = bswapLE16(*((uint16_t *) (lpStream + patpos[j])));
	rows = bswapLE16(*((uint16_t *) (lpStream + patpos[j] + 2)));

	if ((rows < 4) || (rows > 256)) {
	    continue;
	}
	if (patpos[j] + 8 + len > dwMemLength) {
	    continue;
	}
	i = 0;
	p = lpStream + patpos[j] + 8;
	nrow = 0;

	while (nrow < rows) {
	    uint8_t b;
	    unsigned ch;

	    if (i >= len) {
		break;
	    }
	    b = p[i++];

	    if (!b) {
		nrow++;
		continue;
	    }
	    ch = b & 0x7F;

	    if (ch) {
		ch = (ch - 1) & 0x3F;
	    }
	    if (b & 0x80) {
		if (i >= len) {
		    break;
		}
		chnmask[ch] = p[i++];
	    }
	    // Channel used
	    if (chnmask[ch] & 0x0F) {
		if ((ch >= that->m_nChannels) && (ch < 64)) {
		    that->m_nChannels = ch + 1;
		}
	    }
	    // Note
	    if (chnmask[ch] & 1) {
		i++;
	    }
	    // Instrument
	    if (chnmask[ch] & 2) {
		i++;
	    }
	    // Volume
	    if (chnmask[ch] & 4) {
		i++;
	    }
	    // Effect
	    if (chnmask[ch] & 8) {
		i += 2;
	    }
	    if (i >= len) {
		break;
	    }
	}
    }
    // Reading Instruments
    that->m_nInstruments = 0;
    if (pifh.flags & 0x04)
	that->m_nInstruments = pifh.insnum;
    if (that->m_nInstruments >= MAX_INSTRUMENTS)
	that->m_nInstruments = MAX_INSTRUMENTS - 1;

    for (j = 0; j < that->m_nInstruments; j++) {
	if ((inspos[j] > 0)
		&& (inspos[j] < dwMemLength - sizeof(ITOLDINSTRUMENT))) {
	    INSTRUMENTHEADER *penv = malloc(sizeof(INSTRUMENTHEADER));

	    if (!penv) {
		continue;
	    }
	    memset(penv, 0, sizeof(INSTRUMENTHEADER));

	    that->Headers[j + 1] = penv;
	    ITInstrToMPT(lpStream + inspos[j], penv, pifh.cmwt);
	}
    }

    // Reading Samples
    that->m_nSamples = pifh.smpnum;
    if (that->m_nSamples >= MAX_SAMPLES) {
	that->m_nSamples = MAX_SAMPLES - 1;
    }

    for (j = 0; j < pifh.smpnum; j++) {
	if ((smppos[j])
	    && (smppos[j] + sizeof(ITSAMPLESTRUCT) <= dwMemLength)) {
	    ITSAMPLESTRUCT pis = *(ITSAMPLESTRUCT *) (lpStream + smppos[j]);

	    pis.id = bswapLE32(pis.id);
	    pis.length = bswapLE32(pis.length);
	    pis.loopbegin = bswapLE32(pis.loopbegin);
	    pis.loopend = bswapLE32(pis.loopend);
	    pis.C5Speed = bswapLE32(pis.C5Speed);
	    pis.susloopbegin = bswapLE32(pis.susloopbegin);
	    pis.susloopend = bswapLE32(pis.susloopend);
	    pis.samplepointer = bswapLE32(pis.samplepointer);

	    if (pis.id == 0x53504D49) {
		MODINSTRUMENT *pins = &that->Ins[j + 1];

		memcpy(pins->Name, pis.Filename, 12);
		pins->uFlags = 0;
		pins->nLength = 0;
		pins->nLoopStart = pis.loopbegin;
		pins->nLoopEnd = pis.loopend;
		pins->nSustainStart = pis.susloopbegin;
		pins->nSustainEnd = pis.susloopend;
		pins->nC4Speed = pis.C5Speed;
		if (!pins->nC4Speed)
		    pins->nC4Speed = 8363;
		if (pis.C5Speed < 256)
		    pins->nC4Speed = 256;
		pins->nVolume = pis.vol << 2;
		if (pins->nVolume > 256)
		    pins->nVolume = 256;
		pins->nGlobalVol = pis.gvl;
		if (pins->nGlobalVol > 64)
		    pins->nGlobalVol = 64;
		if (pis.flags & 0x10)
		    pins->uFlags |= CHN_LOOP;
		if (pis.flags & 0x20)
		    pins->uFlags |= CHN_SUSTAINLOOP;
		if (pis.flags & 0x40)
		    pins->uFlags |= CHN_PINGPONGLOOP;
		if (pis.flags & 0x80)
		    pins->uFlags |= CHN_PINGPONGSUSTAIN;
		pins->nPan = (pis.dfp & 0x7F) << 2;
		if (pins->nPan > 256)
		    pins->nPan = 256;
		if (pis.dfp & 0x80)
		    pins->uFlags |= CHN_PANNING;
		pins->nVibType = autovibit2xm[pis.vit & 7];
		pins->nVibRate = pis.vis;
		pins->nVibDepth = pis.vid & 0x7F;
		pins->nVibSweep = (pis.vir + 3) / 4;
		if ((pis.samplepointer) && (pis.samplepointer < dwMemLength)
		    && (pis.length)) {
		    unsigned flags;

		    pins->nLength = pis.length;
		    if (pins->nLength > MAX_SAMPLE_LENGTH) {
			pins->nLength = MAX_SAMPLE_LENGTH;
		    }

		    flags = (pis.cvt & 1) ? RS_PCM8S : RS_PCM8U;
		    if (pis.flags & 2) {
			flags += 5;
			if (pis.flags & 4)
			    flags |= RSF_STEREO;
			pins->uFlags |= CHN_16BIT;
			// IT 2.14 16-bit packed sample ?
			if (pis.flags & 8)
			    flags = ((pifh.cmwt >= 0x215)
				&& (pis.cvt & 4)) ? RS_IT21516 : RS_IT21416;
		    } else {
			if (pis.flags & 4)
			    flags |= RSF_STEREO;
			if (pis.cvt == 0xFF)
			    flags = RS_ADPCM4;
			else
			    // IT 2.14 8-bit packed sample ?
			if (pis.flags & 8)
			    flags = ((pifh.cmwt >= 0x215)
				&& (pis.cvt & 4)) ? RS_IT2158 : RS_IT2148;
		    }
		    CSoundFile_ReadSample(that,&that->Ins[j + 1], flags,
			(char *)(lpStream + pis.samplepointer),
			dwMemLength - pis.samplepointer);
		}
	    }
	    memcpy(that->m_szNames[j + 1], pis.Name, 26);
	}
    }

    // Reading Patterns
    for (j = 0; j < npatterns; j++) {
	unsigned len;
	unsigned rows;
	MODCOMMAND *m;
	unsigned i;
	const uint8_t *p;
	unsigned nrow;

	if ((!patpos[j]) || ((uint32_t) patpos[j] + 4 >= dwMemLength)) {
	    that->PatternSize[j] = 64;
	    that->Patterns[j] =
		CSoundFile_AllocatePattern(64, that->m_nChannels);
	    continue;
	}

	len = bswapLE16(*((uint16_t *) (lpStream + patpos[j])));
	rows = bswapLE16(*((uint16_t *) (lpStream + patpos[j] + 2)));
	if ((rows < 4) || (rows > 256))
	    continue;
	if (patpos[j] + 8 + len > dwMemLength)
	    continue;
	that->PatternSize[j] = rows;
	if ((that->Patterns[j] =
		CSoundFile_AllocatePattern(rows, that->m_nChannels)) == NULL)
	    continue;
	memset(lastvalue, 0, sizeof(lastvalue));
	memset(chnmask, 0, sizeof(chnmask));

	m = that->Patterns[j];
	i = 0;
	p = lpStream + patpos[j] + 8;
	nrow = 0;

	while (nrow < rows) {
	    uint8_t b;
	    unsigned ch;

	    if (i >= len) {
		break;
	    }

	    b = p[i++];
	    if (!b) {
		nrow++;
		m += that->m_nChannels;
		continue;
	    }

	    ch = b & 0x7F;
	    if (ch) {
		ch = (ch - 1) & 0x3F;
	    }
	    if (b & 0x80) {
		if (i >= len) {
		    break;
		}
		chnmask[ch] = p[i++];
	    }
	    if ((chnmask[ch] & 0x10) && (ch < that->m_nChannels)) {
		m[ch].note = lastvalue[ch].note;
	    }
	    if ((chnmask[ch] & 0x20) && (ch < that->m_nChannels)) {
		m[ch].instr = lastvalue[ch].instr;
	    }
	    if ((chnmask[ch] & 0x40) && (ch < that->m_nChannels)) {
		m[ch].volcmd = lastvalue[ch].volcmd;
		m[ch].vol = lastvalue[ch].vol;
	    }
	    if ((chnmask[ch] & 0x80) && (ch < that->m_nChannels)) {
		m[ch].command = lastvalue[ch].command;
		m[ch].param = lastvalue[ch].param;
	    }
	    if (chnmask[ch] & 1) {	// Note
		unsigned note;

		if (i >= len) {
		    break;
		}
		note = p[i++];

		if (ch < that->m_nChannels) {
		    if (note < 0x80)
			note++;
		    m[ch].note = note;
		    lastvalue[ch].note = note;
		    channels_used[ch] = 1;
		}
	    }
	    if (chnmask[ch] & 2) {
		unsigned instr;

		if (i >= len) {
		    break;
		}
		instr = p[i++];

		if (ch < that->m_nChannels) {
		    m[ch].instr = instr;
		    lastvalue[ch].instr = instr;
		}
	    }
	    if (chnmask[ch] & 4) {
		unsigned vol;

		if (i >= len) {
		    break;
		}
		vol = p[i++];

		if (ch < that->m_nChannels) {
		    // 0-64: Set Volume
		    if (vol <= 64) {
			m[ch].volcmd = VOLCMD_VOLUME;
			m[ch].vol = vol;
		    } else
			// 128-192: Set Panning
		    if ((vol >= 128) && (vol <= 192)) {
			m[ch].volcmd = VOLCMD_PANNING;
			m[ch].vol = vol - 128;
		    } else
			// 65-74: Fine Volume Up
		    if (vol < 75) {
			m[ch].volcmd = VOLCMD_FINEVOLUP;
			m[ch].vol = vol - 65;
		    } else
			// 75-84: Fine Volume Down
		    if (vol < 85) {
			m[ch].volcmd = VOLCMD_FINEVOLDOWN;
			m[ch].vol = vol - 75;
		    } else
			// 85-94: Volume Slide Up
		    if (vol < 95) {
			m[ch].volcmd = VOLCMD_VOLSLIDEUP;
			m[ch].vol = vol - 85;
		    } else
			// 95-104: Volume Slide Down
		    if (vol < 105) {
			m[ch].volcmd = VOLCMD_VOLSLIDEDOWN;
			m[ch].vol = vol - 95;
		    } else
			// 105-114: Pitch Slide Up
		    if (vol < 115) {
			m[ch].volcmd = VOLCMD_PORTADOWN;
			m[ch].vol = vol - 105;
		    } else
			// 115-124: Pitch Slide Down
		    if (vol < 125) {
			m[ch].volcmd = VOLCMD_PORTAUP;
			m[ch].vol = vol - 115;
		    } else
			// 193-202: Portamento To
		    if ((vol >= 193) && (vol <= 202)) {
			m[ch].volcmd = VOLCMD_TONEPORTAMENTO;
			m[ch].vol = vol - 193;
		    } else
			// 203-212: Vibrato
		    if ((vol >= 203) && (vol <= 212)) {
			m[ch].volcmd = VOLCMD_VIBRATOSPEED;
			m[ch].vol = vol - 203;
		    }
		    lastvalue[ch].volcmd = m[ch].volcmd;
		    lastvalue[ch].vol = m[ch].vol;
		}
	    }
	    // Reading command/param
	    if (chnmask[ch] & 8) {
		unsigned cmd;
		unsigned param;

		if (i > len - 2) {
		    break;
		}
		cmd = p[i++];
		param = p[i++];

		if (ch < that->m_nChannels) {
		    if (cmd) {
			m[ch].command = cmd;
			m[ch].param = param;
			CSoundFile_S3MConvert(&m[ch], 1);
			lastvalue[ch].command = m[ch].command;
			lastvalue[ch].param = m[ch].param;
		    }
		}
	    }
	}
    }
    for (j = 0; j < MAX_BASECHANNELS; j++) {
	if (j >= that->m_nChannels) {
	    that->ChnSettings[j].nVolume = 64;
	    that->ChnSettings[j].dwFlags &= ~CHN_MUTE;
	}
    }
    that->m_nMinPeriod = 8;
    that->m_nMaxPeriod = 0xF000;
    return 1;
}

#ifndef MODPLUG_NO_FILESAVE
//#define MODPLUG_SAVEITTIMESTAMP
#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

int CSoundFile_SaveIT(CSoundFile * that, const char *lpszFileName,
    unsigned nPacking)
//---------------------------------------------------------
{
    uint32_t dwPatNamLen, dwChnNamLen;
    ITFILEHEADER header;
    ITINSTRUMENT iti;
    ITSAMPLESTRUCT itss;
    uint8_t smpcount[MAX_SAMPLES];
    uint32_t inspos[MAX_INSTRUMENTS];
    uint32_t patpos[MAX_PATTERNS];
    uint32_t smppos[MAX_SAMPLES];
    uint32_t dwPos = 0, dwHdrPos = 0, dwExtra = 2;
    uint16_t patinfo[4];
    uint8_t chnmask[64];
    uint8_t buf[512];
    MODCOMMAND lastvalue[64];
    FILE *f;
    unsigned i;
    unsigned j;

    if ((!lpszFileName) || ((f = fopen(lpszFileName, "wb")) == NULL)) {
	return 0;
    }
    memset(inspos, 0, sizeof(inspos));
    memset(patpos, 0, sizeof(patpos));
    memset(smppos, 0, sizeof(smppos));
    // Writing Header
    memset(&header, 0, sizeof(header));
    dwPatNamLen = 0;
    dwChnNamLen = 0;
    header.id = 0x4D504D49;
    strncpy((char *)header.songname, that->m_szNames[0], 27);
    header.reserved1 = 0x1004;
    header.ordnum = 0;
    while ((header.ordnum < MAX_ORDERS) && (that->Order[header.ordnum] < 0xFF))
	header.ordnum++;
    if (header.ordnum < MAX_ORDERS) {
	that->Order[header.ordnum++] = 0xFF;
    }
    header.insnum = that->m_nInstruments;
    header.smpnum = that->m_nSamples;
    header.patnum = MAX_PATTERNS;
    while ((header.patnum > 0) && (!that->Patterns[header.patnum - 1]))
	header.patnum--;
    header.cwtv = 0x217;
    header.cmwt = 0x200;
    header.flags = 0x0001;
    header.special = 0x0006;
    if (that->m_nInstruments)
	header.flags |= 0x04;
    if (that->m_dwSongFlags & SONG_LINEARSLIDES)
	header.flags |= 0x08;
    if (that->m_dwSongFlags & SONG_ITOLDEFFECTS)
	header.flags |= 0x10;
    if (that->m_dwSongFlags & SONG_ITCOMPATMODE)
	header.flags |= 0x20;
    if (that->m_dwSongFlags & SONG_EXFILTERRANGE)
	header.flags |= 0x1000;
    header.globalvol = that->m_nDefaultGlobalVolume >> 1;
    header.mv = that->m_nSongPreAmp;
    if (header.mv < 0x20)
	header.mv = 0x20;
    if (header.mv > 0x7F)
	header.mv = 0x7F;
    header.speed = that->m_nDefaultSpeed;
    header.tempo = that->m_nDefaultTempo;
    header.sep = 128;
    dwHdrPos = sizeof(header) + header.ordnum;
    // Channel Pan and Volume
    memset(header.chnpan, 0xFF, 64);
    memset(header.chnvol, 64, 64);
    for (i = 0; i < that->m_nChannels; i++) {
	header.chnpan[i] = that->ChnSettings[i].nPan >> 2;
	if (that->ChnSettings[i].dwFlags & CHN_SURROUND)
	    header.chnpan[i] = 100;
	header.chnvol[i] = that->ChnSettings[i].nVolume;
	if (that->ChnSettings[i].dwFlags & CHN_MUTE)
	    header.chnpan[i] |= 0x80;
	if (that->ChnSettings[i].szName[0]) {
	    dwChnNamLen = (i + 1) * MAX_CHANNELNAME;
	}
    }
    if (dwChnNamLen)
	dwExtra += dwChnNamLen + 8;
#ifdef MODPLUG_SAVEITTIMESTAMP
    dwExtra += 8;			// Time Stamp
#endif
    if (that->m_dwSongFlags & SONG_EMBEDMIDICFG) {
	header.flags |= 0x80;
	header.special |= 0x08;
	dwExtra += sizeof(MODMIDICFG);
    }
    // Pattern Names
    if ((that->m_nPatternNames) && (that->m_lpszPatternNames)) {
	dwPatNamLen = that->m_nPatternNames * MAX_PATTERNNAME;
	while ((dwPatNamLen >= MAX_PATTERNNAME)
	    && (!that->m_lpszPatternNames[dwPatNamLen - MAX_PATTERNNAME]))
	    dwPatNamLen -= MAX_PATTERNNAME;
	if (dwPatNamLen < MAX_PATTERNNAME)
	    dwPatNamLen = 0;
	if (dwPatNamLen)
	    dwExtra += dwPatNamLen + 8;
    }
    // Mix Plugins
    dwExtra += CSoundFile_SaveMixPlugins(that,NULL, 1);
    // Comments
    if (that->m_lpszSongComments) {
	header.special |= 1;
	header.msglength = strlen(that->m_lpszSongComments) + 1;
	header.msgoffset =
	    dwHdrPos + dwExtra + header.insnum * 4 + header.patnum * 4 +
	    header.smpnum * 4;
    }
    // Write file header
    fwrite(&header, 1, sizeof(header), f);
    fwrite(that->Order, 1, header.ordnum, f);
    if (header.insnum)
	fwrite(inspos, 4, header.insnum, f);
    if (header.smpnum)
	fwrite(smppos, 4, header.smpnum, f);
    if (header.patnum)
	fwrite(patpos, 4, header.patnum, f);
    // Writing editor history information
    {
#ifdef MODPLUG_SAVEITTIMESTAMP
	SYSTEMTIME systime;
	FILETIME filetime;
	uint16_t timestamp[4];
	uint16_t nInfoEx = 1;

	memset(timestamp, 0, sizeof(timestamp));
	fwrite(&nInfoEx, 1, 2, f);
	GetSystemTime(&systime);
	SystemTimeToFileTime(&systime, &filetime);
	FileTimeToDosDateTime(&filetime, &timestamp[0], &timestamp[1]);
	fwrite(timestamp, 1, 8, f);
#else
	uint16_t nInfoEx = 0;

	fwrite(&nInfoEx, 1, 2, f);
#endif
    }
    // Writing midi cfg
    if (header.flags & 0x80) {
	fwrite(&that->m_MidiCfg, 1, sizeof(MODMIDICFG), f);
    }

    // Writing pattern names
    if (dwPatNamLen) {
	uint32_t d = 0x4d414e50;

	fwrite(&d, 1, 4, f);
	fwrite(&dwPatNamLen, 1, 4, f);
	fwrite(that->m_lpszPatternNames, 1, dwPatNamLen, f);
    }

    // Writing channel Names
    if (dwChnNamLen) {
	uint32_t d = 0x4d414e43;
	unsigned nChnNames;

	fwrite(&d, 1, 4, f);
	fwrite(&dwChnNamLen, 1, 4, f);
	nChnNames = dwChnNamLen / MAX_CHANNELNAME;

	for (i = 0; i < nChnNames; i++) {
	    fwrite(that->ChnSettings[i].szName, 1, MAX_CHANNELNAME, f);
	}
    }

    // Writing mix plugins info
    CSoundFile_SaveMixPlugins(that,f, 0);
    // Writing song message
    dwPos = dwHdrPos + dwExtra + (header.insnum + header.smpnum +
	header.patnum) * 4;
    if (header.special & 1) {
	dwPos += strlen(that->m_lpszSongComments) + 1;
	fwrite(that->m_lpszSongComments, 1,
	    strlen(that->m_lpszSongComments) + 1, f);
    }

    // Writing instruments
    for (i = 1; i <= header.insnum; i++) {
	memset(&iti, 0, sizeof(iti));
	iti.id = 0x49504D49;		// "IMPI"
	iti.trkvers = 0x211;
	if (that->Headers[i]) {
	    INSTRUMENTHEADER *penv = that->Headers[i];
	    unsigned ev;

	    memset(smpcount, 0, sizeof(smpcount));
	    memcpy(iti.Filename, penv->Filename, 12);
	    memcpy(iti.Name, penv->Name, 26);
	    iti.mbank = penv->wMidiBank;
	    iti.mpr = penv->nMidiProgram;
	    iti.mch = penv->nMidiChannel;
	    iti.nna = penv->nNNA;
	    iti.dct = penv->nDCT;
	    iti.dca = penv->nDNA;
	    iti.fadeout = penv->nFadeOut >> 5;
	    iti.pps = penv->nPPS;
	    iti.ppc = penv->nPPC;
	    iti.gbv = (uint8_t) (penv->nGlobalVol << 1);
	    iti.dfp = (uint8_t) penv->nPan >> 2;
	    if (!(penv->dwFlags & ENV_SETPANNING))
		iti.dfp |= 0x80;
	    iti.rv = penv->nVolSwing;
	    iti.rp = penv->nPanSwing;
	    iti.ifc = penv->nIFC;
	    iti.ifr = penv->nIFR;
	    iti.nos = 0;
	    for (j = 0; j < 120; j++)
		if (penv->Keyboard[j] < MAX_SAMPLES) {
		    unsigned smp = penv->Keyboard[j];

		    if ((smp) && (!smpcount[smp])) {
			smpcount[smp] = 1;
			iti.nos++;
		    }
		    iti.keyboard[j * 2] = penv->NoteMap[j] - 1;
		    iti.keyboard[j * 2 + 1] = smp;
		}
	    // Writing Volume envelope
	    if (penv->dwFlags & ENV_VOLUME)
		iti.volenv.flags |= 0x01;
	    if (penv->dwFlags & ENV_VOLLOOP)
		iti.volenv.flags |= 0x02;
	    if (penv->dwFlags & ENV_VOLSUSTAIN)
		iti.volenv.flags |= 0x04;
	    if (penv->dwFlags & ENV_VOLCARRY)
		iti.volenv.flags |= 0x08;
	    iti.volenv.num = (uint8_t) penv->nVolEnv;
	    iti.volenv.lpb = (uint8_t) penv->nVolLoopStart;
	    iti.volenv.lpe = (uint8_t) penv->nVolLoopEnd;
	    iti.volenv.slb = penv->nVolSustainBegin;
	    iti.volenv.sle = penv->nVolSustainEnd;
	    // Writing Panning envelope
	    if (penv->dwFlags & ENV_PANNING)
		iti.panenv.flags |= 0x01;
	    if (penv->dwFlags & ENV_PANLOOP)
		iti.panenv.flags |= 0x02;
	    if (penv->dwFlags & ENV_PANSUSTAIN)
		iti.panenv.flags |= 0x04;
	    if (penv->dwFlags & ENV_PANCARRY)
		iti.panenv.flags |= 0x08;
	    iti.panenv.num = (uint8_t) penv->nPanEnv;
	    iti.panenv.lpb = (uint8_t) penv->nPanLoopStart;
	    iti.panenv.lpe = (uint8_t) penv->nPanLoopEnd;
	    iti.panenv.slb = penv->nPanSustainBegin;
	    iti.panenv.sle = penv->nPanSustainEnd;
	    // Writing Pitch Envelope
	    if (penv->dwFlags & ENV_PITCH)
		iti.pitchenv.flags |= 0x01;
	    if (penv->dwFlags & ENV_PITCHLOOP)
		iti.pitchenv.flags |= 0x02;
	    if (penv->dwFlags & ENV_PITCHSUSTAIN)
		iti.pitchenv.flags |= 0x04;
	    if (penv->dwFlags & ENV_PITCHCARRY)
		iti.pitchenv.flags |= 0x08;
	    if (penv->dwFlags & ENV_FILTER)
		iti.pitchenv.flags |= 0x80;
	    iti.pitchenv.num = (uint8_t) penv->nPitchEnv;
	    iti.pitchenv.lpb = (uint8_t) penv->nPitchLoopStart;
	    iti.pitchenv.lpe = (uint8_t) penv->nPitchLoopEnd;
	    iti.pitchenv.slb = (uint8_t) penv->nPitchSustainBegin;
	    iti.pitchenv.sle = (uint8_t) penv->nPitchSustainEnd;
	    // Writing Envelopes data
	    for (ev = 0; ev < 25; ev++) {
		iti.volenv.data[ev * 3] = penv->VolEnv[ev];
		iti.volenv.data[ev * 3 + 1] = penv->VolPoints[ev] & 0xFF;
		iti.volenv.data[ev * 3 + 2] = penv->VolPoints[ev] >> 8;
		iti.panenv.data[ev * 3] = penv->PanEnv[ev] - 32;
		iti.panenv.data[ev * 3 + 1] = penv->PanPoints[ev] & 0xFF;
		iti.panenv.data[ev * 3 + 2] = penv->PanPoints[ev] >> 8;
		iti.pitchenv.data[ev * 3] = penv->PitchEnv[ev] - 32;
		iti.pitchenv.data[ev * 3 + 1] = penv->PitchPoints[ev] & 0xFF;
		iti.pitchenv.data[ev * 3 + 2] = penv->PitchPoints[ev] >> 8;
	    }
	    // Save Empty Instrument
	} else {
	    for (j = 0; j < 120; j++) {
		iti.keyboard[j * 2] = j;
	    }
	    iti.ppc = 5 * 12;
	    iti.gbv = 128;
	    iti.dfp = 0x20;
	    iti.ifc = 0xFF;
	}
	if (!iti.nos) {
	    iti.trkvers = 0;
	}
	// Writing instrument
	inspos[i - 1] = dwPos;
	dwPos += sizeof(ITINSTRUMENT);
	fwrite(&iti, 1, sizeof(ITINSTRUMENT), f);
    }

    // Writing sample headers
    memset(&itss, 0, sizeof(itss));
    for (i = 0; i < header.smpnum; i++) {
	smppos[i] = dwPos;
	dwPos += sizeof(ITSAMPLESTRUCT);
	fwrite(&itss, 1, sizeof(ITSAMPLESTRUCT), f);
    }
    // Writing Patterns
    for (i = 0; i < header.patnum; i++) {
	uint32_t dwPatPos = dwPos;
	unsigned len;
	unsigned row;
	MODCOMMAND *m;

	if (!that->Patterns[i])
	    continue;
	patpos[i] = dwPos;
	patinfo[0] = 0;
	patinfo[1] = that->PatternSize[i];
	patinfo[2] = 0;
	patinfo[3] = 0;
	// Check for empty pattern
	if (that->PatternSize[i] == 64) {
	    MODCOMMAND *pzc = that->Patterns[i];
	    unsigned nz = that->PatternSize[i] * that->m_nChannels;
	    unsigned iz;

	    for (iz = 0; iz < nz; iz++) {
		if ((pzc[iz].note) || (pzc[iz].instr)
		    || (pzc[iz].volcmd) || (pzc[iz].command))
		    break;
	    }
	    if (iz == nz) {
		patpos[i] = 0;
		continue;
	    }
	}
	fwrite(patinfo, 8, 1, f);
	dwPos += 8;
	memset(chnmask, 0xFF, sizeof(chnmask));
	memset(lastvalue, 0, sizeof(lastvalue));
	m = that->Patterns[i];

	for (row = 0; row < that->PatternSize[i]; row++) {
	    unsigned ch;

	    len = 0;
	    for (ch = 0; ch < that->m_nChannels; ch++, m++) {
		uint8_t b = 0;
		unsigned command = m->command;
		unsigned param = m->param;
		unsigned vol = 0xFF;
		unsigned note = m->note;

		if (note)
		    b |= 1;
		if ((note) && (note < 0xFE))
		    note--;
		if (m->instr)
		    b |= 2;
		if (m->volcmd) {
		    unsigned volcmd = m->volcmd;

		    switch (volcmd) {
			case VOLCMD_VOLUME:
			    vol = m->vol;
			    if (vol > 64)
				vol = 64;
			    break;
			case VOLCMD_PANNING:
			    vol = m->vol + 128;
			    if (vol > 192)
				vol = 192;
			    break;
			case VOLCMD_VOLSLIDEUP:
			    vol = 85 + ConvertVolParam(m->vol);
			    break;
			case VOLCMD_VOLSLIDEDOWN:
			    vol = 95 + ConvertVolParam(m->vol);
			    break;
			case VOLCMD_FINEVOLUP:
			    vol = 65 + ConvertVolParam(m->vol);
			    break;
			case VOLCMD_FINEVOLDOWN:
			    vol = 75 + ConvertVolParam(m->vol);
			    break;
			case VOLCMD_VIBRATO:
			    vol = 203;
			    break;
			case VOLCMD_VIBRATOSPEED:
			    vol = 203 + ConvertVolParam(m->vol);
			    break;
			case VOLCMD_TONEPORTAMENTO:
			    vol = 193 + ConvertVolParam(m->vol);
			    break;
			case VOLCMD_PORTADOWN:
			    vol = 105 + ConvertVolParam(m->vol);
			    break;
			case VOLCMD_PORTAUP:
			    vol = 115 + ConvertVolParam(m->vol);
			    break;
			default:
			    vol = 0xFF;
		    }
		}
		if (vol != 0xFF)
		    b |= 4;
		if (command) {
		    CSoundFile_S3MSaveConvert(that, &command, &param, 1);
		    if (command)
			b |= 8;
		}
		// Packing information
		if (b) {
		    // Same note ?
		    if (b & 1) {
			if ((note == lastvalue[ch].note)
			    && (lastvalue[ch].volcmd & 1)) {
			    b &= ~1;
			    b |= 0x10;
			} else {
			    lastvalue[ch].note = note;
			    lastvalue[ch].volcmd |= 1;
			}
		    }
		    // Same instrument ?
		    if (b & 2) {
			if ((m->instr == lastvalue[ch].instr)
			    && (lastvalue[ch].volcmd & 2)) {
			    b &= ~2;
			    b |= 0x20;
			} else {
			    lastvalue[ch].instr = m->instr;
			    lastvalue[ch].volcmd |= 2;
			}
		    }
		    // Same volume column byte ?
		    if (b & 4) {
			if ((vol == lastvalue[ch].vol)
			    && (lastvalue[ch].volcmd & 4)) {
			    b &= ~4;
			    b |= 0x40;
			} else {
			    lastvalue[ch].vol = vol;
			    lastvalue[ch].volcmd |= 4;
			}
		    }
		    // Same command / param ?
		    if (b & 8) {
			if ((command == lastvalue[ch].command)
			    && (param == lastvalue[ch].param)
			    && (lastvalue[ch].volcmd & 8)) {
			    b &= ~8;
			    b |= 0x80;
			} else {
			    lastvalue[ch].command = command;
			    lastvalue[ch].param = param;
			    lastvalue[ch].volcmd |= 8;
			}
		    }
		    if (b != chnmask[ch]) {
			chnmask[ch] = b;
			buf[len++] = (ch + 1) | 0x80;
			buf[len++] = b;
		    } else {
			buf[len++] = ch + 1;
		    }
		    if (b & 1)
			buf[len++] = note;
		    if (b & 2)
			buf[len++] = m->instr;
		    if (b & 4)
			buf[len++] = vol;
		    if (b & 8) {
			buf[len++] = command;
			buf[len++] = param;
		    }
		}
	    }
	    buf[len++] = 0;
	    dwPos += len;
	    patinfo[0] += len;
	    fwrite(buf, 1, len, f);
	}
	fseek(f, dwPatPos, SEEK_SET);
	fwrite(patinfo, 8, 1, f);
	fseek(f, dwPos, SEEK_SET);
    }
    // Writing Sample Data
    for (i = 1; i <= header.smpnum; i++) {
	MODINSTRUMENT *psmp = &that->Ins[i];
	unsigned flags;

	memset(&itss, 0, sizeof(itss));
	memcpy(itss.Filename, psmp->Name, 12);
	memcpy(itss.Name, that->m_szNames[i], 26);
	itss.id = 0x53504D49;
	itss.gvl = (uint8_t) psmp->nGlobalVol;
	if (that->m_nInstruments) {
	    unsigned iu;

	    for (iu = 1; iu <= that->m_nInstruments; iu++)
		if (that->Headers[iu]) {
		    INSTRUMENTHEADER *penv = that->Headers[iu];
		    unsigned ju;

		    for (ju = 0; ju < 128; ju++) {
			if (penv->Keyboard[ju] == i) {
			    itss.flags = 0x01;
			    break;
			}
		    }
		}
	} else {
	    itss.flags = 0x01;
	}
	if (psmp->uFlags & CHN_LOOP)
	    itss.flags |= 0x10;
	if (psmp->uFlags & CHN_SUSTAINLOOP)
	    itss.flags |= 0x20;
	if (psmp->uFlags & CHN_PINGPONGLOOP)
	    itss.flags |= 0x40;
	if (psmp->uFlags & CHN_PINGPONGSUSTAIN)
	    itss.flags |= 0x80;
	itss.C5Speed = psmp->nC4Speed;
	if (!itss.C5Speed)
	    itss.C5Speed = 8363;
	itss.length = psmp->nLength;
	itss.loopbegin = psmp->nLoopStart;
	itss.loopend = psmp->nLoopEnd;
	itss.susloopbegin = psmp->nSustainStart;
	itss.susloopend = psmp->nSustainEnd;
	itss.vol = psmp->nVolume >> 2;
	itss.dfp = psmp->nPan >> 2;
	itss.vit = autovibxm2it[psmp->nVibType & 7];
	itss.vis = psmp->nVibRate;
	itss.vid = psmp->nVibDepth;
	itss.vir = (psmp->nVibSweep < 64) ? psmp->nVibSweep * 4 : 255;
	if (psmp->uFlags & CHN_PANNING)
	    itss.dfp |= 0x80;
	if ((psmp->pSample) && (psmp->nLength))
	    itss.cvt = 0x01;
	flags = RS_PCM8S;

#ifndef MODPLUG_NO_PACKING
	if (nPacking) {
	    if ((!(psmp->uFlags & (CHN_16BIT | CHN_STEREO)))
		&& (CSoundFile_CanPackSample(that,(char *)psmp->pSample,
			psmp->nLength, nPacking, NULL))) {
		flags = RS_ADPCM4;
		itss.cvt = 0xFF;
	    }
	} else
#endif // MODPLUG_NO_PACKING
	{
	    if (psmp->uFlags & CHN_STEREO) {
		flags = RS_STPCM8S;
		itss.flags |= 0x04;
	    }
	    if (psmp->uFlags & CHN_16BIT) {
		itss.flags |= 0x02;
		flags = (psmp->uFlags & CHN_STEREO) ? RS_STPCM16S : RS_PCM16S;
	    }
	}
	itss.samplepointer = dwPos;
	fseek(f, smppos[i - 1], SEEK_SET);
	fwrite(&itss, 1, sizeof(ITSAMPLESTRUCT), f);
	fseek(f, dwPos, SEEK_SET);
	if ((psmp->pSample) && (psmp->nLength)) {
	    dwPos += CSoundFile_WriteSample(that, f, psmp, flags, 0);
	}
    }
    // Updating offsets
    fseek(f, dwHdrPos, SEEK_SET);
    if (header.insnum)
	fwrite(inspos, 4, header.insnum, f);
    if (header.smpnum)
	fwrite(smppos, 4, header.smpnum, f);
    if (header.patnum)
	fwrite(patpos, 4, header.patnum, f);
    fclose(f);
    return 1;
}

#ifdef _MSC_VER
#pragma warning(default:4100)
#endif
#endif // MODPLUG_NO_FILESAVE

//////////////////////////////////////////////////////////////////////////////
// IT 2.14 compression

uint32_t CSoundFile_ITReadBits(uint32_t * bitbuf, unsigned * bitnum,
    uint8_t ** ibuf, int8_t n)
//-----------------------------------------------------------------
{
    uint32_t retval = 0;
    unsigned i = n;

    if (n > 0) {
	do {
	    if (!*bitnum) {
		*bitbuf = *(*ibuf)++;	// JOHNS: not sure? was &ibuf
		*bitnum = 8;
	    }
	    retval >>= 1;
	    retval |= *bitbuf << 31;
	    *bitbuf >>= 1;
	    (*bitnum)--;
	    i--;
	} while (i);
	i = n;
    }
    return (retval >> (32 - i));
}

#define IT215_SUPPORT
void CSoundFile_ITUnpack8Bit(int8_t * pSample, uint32_t dwLen,
    uint8_t * lpMemFile, uint32_t dwMemLength, int b215)
//-------------------------------------------------------------------------------------------
{
    int8_t *pDst = pSample;
    uint8_t *pSrc = lpMemFile;
    uint32_t wHdr = 0;
    uint32_t wCount = 0;
    uint32_t bitbuf = 0;
    unsigned bitnum = 0;
    uint8_t bLeft = 0, bTemp = 0, bTemp2 = 0;

    while (dwLen) {
	uint32_t d;
	uint32_t dwPos;

	if (!wCount) {
	    wCount = 0x8000;
	    wHdr = bswapLE16(*((uint16_t *) pSrc));
	    pSrc += 2;
	    bLeft = 9;
	    bTemp = bTemp2 = 0;
	    bitbuf = bitnum = 0;
	}
	d = wCount;

	if (d > dwLen)
	    d = dwLen;
	// Unpacking
	dwPos = 0;

	do {
	    uint16_t wBits = (uint16_t) CSoundFile_ITReadBits(&bitbuf,
			&bitnum, &pSrc, bLeft);

	    if (bLeft < 7) {
		uint32_t i = 1 << (bLeft - 1);
		uint32_t j = wBits & 0xFFFF;

		if (i != j)
		    goto UnpackByte;
		wBits =
		    (uint16_t) (CSoundFile_ITReadBits(&bitbuf, &bitnum, &pSrc,
			3) + 1) & 0xFF;
		bLeft =
		    ((uint8_t) wBits <
		    bLeft) ? (uint8_t) wBits : (uint8_t) ((wBits + 1) & 0xFF);
		goto Next;
	    }
	    if (bLeft < 9) {
		uint16_t i = (0xFF >> (9 - bLeft)) + 4;
		uint16_t j = i - 8;

		if ((wBits <= j) || (wBits > i))
		    goto UnpackByte;
		wBits -= j;
		bLeft =
		    ((uint8_t) (wBits & 0xFF) <
		    bLeft) ? (uint8_t) (wBits & 0xFF) : (uint8_t) ((wBits +
			1) & 0xFF);
		goto Next;
	    }
	    if (bLeft >= 10)
		goto SkipByte;
	    if (wBits >= 256) {
		bLeft = (uint8_t) (wBits + 1) & 0xFF;
		goto Next;
	    }
UnpackByte:
	    if (bLeft < 8) {
		uint8_t shift = 8 - bLeft;
		int8_t c = (int8_t) (wBits << shift);

		c >>= shift;
		wBits = (uint16_t) c;
	    }
	    wBits += bTemp;
	    bTemp = (uint8_t) wBits;
	    bTemp2 += bTemp;
#ifdef IT215_SUPPORT
	    pDst[dwPos] = (b215) ? bTemp2 : bTemp;
#else
	    pDst[dwPos] = bTemp;
#endif
SkipByte:
	    dwPos++;
Next:
	    if (pSrc >= lpMemFile + dwMemLength + 1)
		return;
	} while (dwPos < d);
	// Move On
	wCount -= d;
	dwLen -= d;
	pDst += d;
    }
}

void CSoundFile_ITUnpack16Bit(int8_t * pSample, uint32_t dwLen,
    uint8_t * lpMemFile, uint32_t dwMemLength, int b215)
//--------------------------------------------------------------------------------------------
{
    signed short *pDst = (signed short *)pSample;
    uint8_t *pSrc = lpMemFile;
    uint32_t wHdr = 0;
    uint32_t wCount = 0;
    uint32_t bitbuf = 0;
    unsigned bitnum = 0;
    uint8_t bLeft = 0;
    signed short wTemp = 0, wTemp2 = 0;

    while (dwLen) {
	uint32_t d;
	uint32_t dwPos;

	if (!wCount) {
	    wCount = 0x4000;
	    wHdr = bswapLE16(*((uint16_t *) pSrc));
	    pSrc += 2;
	    bLeft = 17;
	    wTemp = wTemp2 = 0;
	    bitbuf = bitnum = 0;
	}
	d = wCount;

	if (d > dwLen)
	    d = dwLen;
	// Unpacking
	dwPos = 0;

	do {
	    uint32_t dwBits =
		CSoundFile_ITReadBits(&bitbuf, &bitnum, &pSrc, bLeft);

	    if (bLeft < 7) {
		uint32_t i = 1 << (bLeft - 1);
		uint32_t j = dwBits;

		if (i != j)
		    goto UnpackByte;
		dwBits = CSoundFile_ITReadBits(&bitbuf, &bitnum, &pSrc, 4) + 1;
		bLeft =
		    ((uint8_t) (dwBits & 0xFF) <
		    bLeft) ? (uint8_t) (dwBits & 0xFF) : (uint8_t) ((dwBits +
			1) & 0xFF);
		goto Next;
	    }
	    if (bLeft < 17) {
		uint32_t i = (0xFFFF >> (17 - bLeft)) + 8;
		uint32_t j = (i - 16) & 0xFFFF;

		if ((dwBits <= j) || (dwBits > (i & 0xFFFF)))
		    goto UnpackByte;
		dwBits -= j;
		bLeft =
		    ((uint8_t) (dwBits & 0xFF) <
		    bLeft) ? (uint8_t) (dwBits & 0xFF) : (uint8_t) ((dwBits +
			1) & 0xFF);
		goto Next;
	    }
	    if (bLeft >= 18)
		goto SkipByte;
	    if (dwBits >= 0x10000) {
		bLeft = (uint8_t) (dwBits + 1) & 0xFF;
		goto Next;
	    }
UnpackByte:
	    if (bLeft < 16) {
		uint8_t shift = 16 - bLeft;
		signed short c = (signed short)(dwBits << shift);

		c >>= shift;
		dwBits = (uint32_t) c;
	    }
	    dwBits += wTemp;
	    wTemp = (signed short)dwBits;
	    wTemp2 += wTemp;
#ifdef IT215_SUPPORT
	    pDst[dwPos] = (b215) ? wTemp2 : wTemp;
#else
	    pDst[dwPos] = wTemp;
#endif
SkipByte:
	    dwPos++;
Next:
	    if (pSrc >= lpMemFile + dwMemLength + 1)
		return;
	} while (dwPos < d);
	// Move On
	wCount -= d;
	dwLen -= d;
	pDst += d;
	if (pSrc >= lpMemFile + dwMemLength) {
	    break;
	}
    }
}

unsigned CSoundFile_SaveMixPlugins(CSoundFile const * that, FILE * f, int bUpdate)
//----------------------------------------------------
{
    uint32_t chinfo[64];
    int8_t s[32];
    uint32_t nPluginSize;
    unsigned nTotalSize = 0;
    unsigned nChInfo = 0;
    unsigned i;

    for (i = 0; i < MAX_MIXPLUGINS; i++) {
	SNDMIXPLUGIN const *p = &that->m_MixPlugins[i];

	if ((p->Info.dwPluginId1) || (p->Info.dwPluginId2)) {
	    nPluginSize = sizeof(SNDMIXPLUGININFO) + 4;	// plugininfo+4 (datalen)
#ifndef MODPLUG_NO_IMIXPLUGIN
	    if ((p->pMixPlugin) && (bUpdate)) {
		p->pMixPlugin->SaveAllParameters();
	    }
#else
	    bUpdate = 0;
#endif
	    if (p->pPluginData) {
		nPluginSize += p->nPluginDataSize;
	    }
	    if (f) {
		s[0] = 'F';
		s[1] = 'X';
		s[2] = '0' + (i / 10);
		s[3] = '0' + (i % 10);
		fwrite(s, 1, 4, f);
		fwrite(&nPluginSize, 1, 4, f);
		fwrite(&p->Info, 1, sizeof(SNDMIXPLUGININFO), f);
		fwrite(&that->m_MixPlugins[i].nPluginDataSize, 1, 4, f);
		if (that->m_MixPlugins[i].pPluginData) {
		    fwrite(that->m_MixPlugins[i].pPluginData, 1,
			that->m_MixPlugins[i].nPluginDataSize, f);
		}
	    }
	    nTotalSize += nPluginSize + 8;
	}
    }
    for (i = 0; i < that->m_nChannels; i++) {
	if (i < 64) {
	    if ((chinfo[i] = that->ChnSettings[i].nMixPlugin) != 0) {
		nChInfo = i + 1;
	    }
	}
    }
    if (nChInfo) {
	if (f) {
	    nPluginSize = 0x58464843;
	    fwrite(&nPluginSize, 1, 4, f);
	    nPluginSize = nChInfo * 4;
	    fwrite(&nPluginSize, 1, 4, f);
	    fwrite(chinfo, 1, nPluginSize, f);
	}
	nTotalSize += nChInfo * 4 + 8;
    }
    return nTotalSize;
}

unsigned CSoundFile_LoadMixPlugins(CSoundFile * that, const void *pData,
    unsigned nLen)
//-----------------------------------------------------------
{
    const uint8_t *p = (const uint8_t *)pData;
    unsigned nPos = 0;

    while (nPos + 8 < nLen) {
	uint32_t nPluginSize;
	unsigned nPlugin;
	unsigned j;

	nPluginSize = bswapLE32(*(uint32_t *) (p + nPos + 4));
	if (nPluginSize > nLen - nPos - 8)
	    break;;
	if ((bswapLE32(*(uint32_t *) (p + nPos))) == 0x58464843) {
	    unsigned ch;

	    for (ch = 0; ch < 64; ch++)
		if (ch * 4 < nPluginSize) {
		    that->ChnSettings[ch].nMixPlugin =
			bswapLE32(*(uint32_t *) (p + nPos + 8 + ch * 4));
		}
	} else {
	    if ((p[nPos] != 'F') || (p[nPos + 1] != 'X')
		|| (p[nPos + 2] < '0') || (p[nPos + 3] < '0')) {
		break;
	    }
	    nPlugin = (p[nPos + 2] - '0') * 10 + (p[nPos + 3] - '0');
	    if ((nPlugin < MAX_MIXPLUGINS)
		&& (nPluginSize >= sizeof(SNDMIXPLUGININFO) + 4)) {
		uint32_t dwExtra =
		    bswapLE32(*(uint32_t *) (p + nPos + 8 +
			sizeof(SNDMIXPLUGININFO)));
		that->m_MixPlugins[nPlugin].Info =
		    *(const SNDMIXPLUGININFO *)(p + nPos + 8);
		that->m_MixPlugins[nPlugin].Info.dwPluginId1 =
		    bswapLE32(that->m_MixPlugins[nPlugin].Info.dwPluginId1);
		that->m_MixPlugins[nPlugin].Info.dwPluginId2 =
		    bswapLE32(that->m_MixPlugins[nPlugin].Info.dwPluginId2);
		that->m_MixPlugins[nPlugin].Info.dwInputRouting =
		    bswapLE32(that->m_MixPlugins[nPlugin].Info.dwInputRouting);
		that->m_MixPlugins[nPlugin].Info.dwOutputRouting =
		    bswapLE32(that->m_MixPlugins[nPlugin].Info.
		    dwOutputRouting);
		for (j = 0; j < 4; j++) {
		    that->m_MixPlugins[nPlugin].Info.dwReserved[j] =
			bswapLE32(that->m_MixPlugins[nPlugin].Info.
			dwReserved[j]);
		}
		if ((dwExtra)
		    && (dwExtra <=
			nPluginSize - sizeof(SNDMIXPLUGININFO) - 4)) {
		    that->m_MixPlugins[nPlugin].nPluginDataSize = 0;
		    that->m_MixPlugins[nPlugin].pPluginData =
			malloc(sizeof(int8_t) * dwExtra);
		    if (that->m_MixPlugins[nPlugin].pPluginData) {
			that->m_MixPlugins[nPlugin].nPluginDataSize = dwExtra;
			memcpy(that->m_MixPlugins[nPlugin].pPluginData,
			    p + nPos + 8 + sizeof(SNDMIXPLUGININFO) + 4,
			    dwExtra);
		    }
		}
	    }
	}
	nPos += nPluginSize + 8;
    }
    return nPos;
}
