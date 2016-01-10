/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: sndfile.c,v 1.3 2002/04/13 19:27:18 jsalmon3 Exp $
*/

#include <math.h>			//for GCCFIX
#include "stdafx.h"
#include "sndfile.h"

#define MAX_PACK_TABLES	    3

#ifndef MODPLUG_NO_PACKING
// Compression table
static int8_t UnpackTable[MAX_PACK_TABLES][16] =
//--------------------------------------------
{
    // CPU-generated dynamic table
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    // u-Law table
    {0, 1, 2, 4, 8, 16, 32, 64,
	-1, -2, -4, -8, -16, -32, -48, -64},
    // Linear table
    {0, 1, 2, 3, 5, 7, 12, 19,
	-1, -2, -3, -5, -7, -12, -19, -31}
};
#endif

//////////////////////////////////////////////////////////
// CSoundFile

void CSoundFile_CSoundFile(CSoundFile * that)
//----------------------
{
    that->m_nType = MOD_TYPE_NONE;
    that->m_dwSongFlags = 0;
    that->m_nChannels = 0;
    that->m_nMixChannels = 0;
    that->m_nSamples = 0;
    that->m_nInstruments = 0;
    that->m_nPatternNames = 0;
    that->m_lpszPatternNames = NULL;
    that->m_lpszSongComments = NULL;
    that->m_nFreqFactor = that->m_nTempoFactor = 128;
    that->m_nMasterVolume = 128;
    that->m_nMinPeriod = 0x20;
    that->m_nMaxPeriod = 0x7FFF;
    that->m_nRepeatCount = 0;
    memset(that->Chn, 0, sizeof(that->Chn));
    memset(that->ChnMix, 0, sizeof(that->ChnMix));
    memset(that->Ins, 0, sizeof(that->Ins));
    memset(that->ChnSettings, 0, sizeof(that->ChnSettings));
    memset(that->Headers, 0, sizeof(that->Headers));
    memset(that->Order, 0xFF, sizeof(that->Order));
    memset(that->Patterns, 0, sizeof(that->Patterns));
    memset(that->m_szNames, 0, sizeof(that->m_szNames));
    memset(that->m_MixPlugins, 0, sizeof(that->m_MixPlugins));
}

int CSoundFile_Create(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//----------------------------------------------------------
{
    int i;
    MODINSTRUMENT *pins;

    CSoundFile_CzCUBICSPLINE();
    CSoundFile_CzWINDOWEDFIR();

    that->m_nType = MOD_TYPE_NONE;
    that->m_dwSongFlags = 0;
    that->m_nChannels = 0;
    that->m_nMixChannels = 0;
    that->m_nSamples = 0;
    that->m_nInstruments = 0;
    that->m_nFreqFactor = that->m_nTempoFactor = 128;
    that->m_nMasterVolume = 128;
    that->m_nDefaultGlobalVolume = 256;
    that->m_nGlobalVolume = 256;
    that->m_nOldGlbVolSlide = 0;
    that->m_nDefaultSpeed = 6;
    that->m_nDefaultTempo = 125;
    that->m_nPatternDelay = 0;
    that->m_nFrameDelay = 0;
    that->m_nNextRow = 0;
    that->m_nRow = 0;
    that->m_nPattern = 0;
    that->m_nCurrentPattern = 0;
    that->m_nNextPattern = 0;
    that->m_nRestartPos = 0;
    that->m_nMinPeriod = 16;
    that->m_nMaxPeriod = 32767;
    that->m_nSongPreAmp = 0x30;
    that->m_nPatternNames = 0;
    that->m_nMaxOrderPosition = 0;
    that->m_lpszPatternNames = NULL;
    that->m_lpszSongComments = NULL;
    memset(that->Ins, 0, sizeof(that->Ins));
    memset(that->ChnMix, 0, sizeof(that->ChnMix));
    memset(that->Chn, 0, sizeof(that->Chn));
    memset(that->Headers, 0, sizeof(that->Headers));
    memset(that->Order, 0xFF, sizeof(that->Order));
    memset(that->Patterns, 0, sizeof(that->Patterns));
    memset(that->m_szNames, 0, sizeof(that->m_szNames));
    memset(that->m_MixPlugins, 0, sizeof(that->m_MixPlugins));
    CSoundFile_ResetMidiCfg(that);

    for (i = 0; i < MAX_PATTERNS; i++) {
	that->PatternSize[i] = 64;
    }
    for (i = 0; i < MAX_BASECHANNELS; i++) {
	that->ChnSettings[i].nPan = 128;
	that->ChnSettings[i].nVolume = 64;
	that->ChnSettings[i].dwFlags = 0;
	that->ChnSettings[i].szName[0] = 0;
    }
    if (lpStream) {
#ifdef MODPLUG_MMCMP_SUPPORT
	int bMMCmp = MMCMP_Unpack(&lpStream, &dwMemLength);
#endif
	if ((!CSoundFile_ReadXM(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadIT(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadS3M(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadWav(that, lpStream, dwMemLength))
#ifndef MODPLUG_BASIC_SUPPORT
	    && (!CSoundFile_ReadSTM(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadMed(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadMTM(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadMDL(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadDBM(that, lpStream, dwMemLength))
	    && (!CSoundFile_Read669(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadFAR(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadAMS(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadOKT(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadPTM(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadUlt(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadDMF(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadDSM(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadUMX(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadAMF(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadPSM(that, lpStream, dwMemLength))
	    && (!CSoundFile_ReadMT2(that, lpStream, dwMemLength))
#endif // MODPLUG_BASIC_SUPPORT
	    && (!CSoundFile_ReadMod(that, lpStream, dwMemLength)))
	    that->m_nType = MOD_TYPE_NONE;
#ifdef MODPLUG_MMCMP_SUPPORT
	if (bMMCmp) {
	    GlobalFreePtr(lpStream);
	    lpStream = NULL;
	}
#endif
    }

    // Adjust song names
    for (i = 0; i < MAX_SAMPLES; i++) {
	char *p = that->m_szNames[i];
	int j = 31;

	p[j] = 0;
	while ((j >= 0) && (p[j] <= ' '))
	    p[j--] = 0;
	while (j >= 0) {
	    if (((uint8_t) p[j]) < ' ')
		p[j] = ' ';
	    j--;
	}
    }

    // Adjust channels
    for (i = 0; i < MAX_BASECHANNELS; i++) {
	if (that->ChnSettings[i].nVolume > 64) {
	    that->ChnSettings[i].nVolume = 64;
	}
	if (that->ChnSettings[i].nPan > 256) {
	    that->ChnSettings[i].nPan = 128;
	}
	that->Chn[i].nPan = that->ChnSettings[i].nPan;
	that->Chn[i].nGlobalVol = that->ChnSettings[i].nVolume;
	that->Chn[i].dwFlags = that->ChnSettings[i].dwFlags;
	that->Chn[i].nVolume = 256;
	that->Chn[i].nCutOff = 0x7F;
    }

    // Checking instruments
    pins = that->Ins;
    for (i = 0; i < MAX_INSTRUMENTS; i++, pins++) {
	if (pins->pSample) {
	    if (pins->nLoopEnd > pins->nLength)
		pins->nLoopEnd = pins->nLength;
	    if (pins->nLoopStart + 3 >= pins->nLoopEnd) {
		pins->nLoopStart = 0;
		pins->nLoopEnd = 0;
	    }
	    if (pins->nSustainEnd > pins->nLength)
		pins->nSustainEnd = pins->nLength;
	    if (pins->nSustainStart + 3 >= pins->nSustainEnd) {
		pins->nSustainStart = 0;
		pins->nSustainEnd = 0;
	    }
	} else {
	    pins->nLength = 0;
	    pins->nLoopStart = 0;
	    pins->nLoopEnd = 0;
	    pins->nSustainStart = 0;
	    pins->nSustainEnd = 0;
	}
	if (!pins->nLoopEnd)
	    pins->uFlags &= ~CHN_LOOP;
	if (!pins->nSustainEnd)
	    pins->uFlags &= ~CHN_SUSTAINLOOP;
	if (pins->nGlobalVol > 64)
	    pins->nGlobalVol = 64;
    }
    // Check invalid instruments
    while ((that->m_nInstruments > 0)
	&& (!that->Headers[that->m_nInstruments]))
	that->m_nInstruments--;
    // Set default values
    if (that->m_nSongPreAmp < 0x20)
	that->m_nSongPreAmp = 0x20;
    if (that->m_nDefaultTempo < 32)
	that->m_nDefaultTempo = 125;
    if (!that->m_nDefaultSpeed)
	that->m_nDefaultSpeed = 6;
    that->m_nMusicSpeed = that->m_nDefaultSpeed;
    that->m_nMusicTempo = that->m_nDefaultTempo;
    that->m_nGlobalVolume = that->m_nDefaultGlobalVolume;
    that->m_nNextPattern = 0;
    that->m_nCurrentPattern = 0;
    that->m_nPattern = 0;
    that->m_nBufferCount = 0;
    that->m_nTickCount = that->m_nMusicSpeed;
    that->m_nNextRow = 0;
    that->m_nRow = 0;
    if ((that->m_nRestartPos >= MAX_ORDERS)
	|| (that->Order[that->m_nRestartPos] >= MAX_PATTERNS))
	that->m_nRestartPos = 0;
    // Load plugins
    if (CSoundFile_gpMixPluginCreateProc) {
	for (i = 0; i < MAX_MIXPLUGINS; i++) {
	    if ((that->m_MixPlugins[i].Info.dwPluginId1)
		|| (that->m_MixPlugins[i].Info.dwPluginId2)) {
		CSoundFile_gpMixPluginCreateProc(&that->m_MixPlugins[i]);
#ifndef MODPLUG_NO_IMIXPLUGIN
		if (that->m_MixPlugins[i].pMixPlugin) {
		    that->m_MixPlugins[i].pMixPlugin->
			RestoreAllParameters();
		}
#endif
	    }
	}
    }
    if (that->m_nType) {
	unsigned maxpreamp = 0x10 + (that->m_nChannels * 8);

	if (maxpreamp > 100)
	    maxpreamp = 100;
	if (that->m_nSongPreAmp > maxpreamp)
	    that->m_nSongPreAmp = maxpreamp;
	return 1;
    }
    return 0;
}

int CSoundFile_Destroy(CSoundFile * that)
//------------------------
{
    int i;

    for (i = 0; i < MAX_PATTERNS; i++) {
	if (that->Patterns[i]) {
	    CSoundFile_FreePattern(that->Patterns[i]);
	    that->Patterns[i] = NULL;
	}
    }
    that->m_nPatternNames = 0;
    if (that->m_lpszPatternNames) {
	free(that->m_lpszPatternNames);

	that->m_lpszPatternNames = NULL;
    }
    if (that->m_lpszSongComments) {
	free(that->m_lpszSongComments);
	that->m_lpszSongComments = NULL;
    }
    for (i = 1; i < MAX_SAMPLES; i++) {
	MODINSTRUMENT *pins = &that->Ins[i];

	if (pins->pSample) {
	    CSoundFile_FreeSample(pins->pSample);
	    pins->pSample = NULL;
	}
    }
    for (i = 0; i < MAX_INSTRUMENTS; i++) {
	if (that->Headers[i]) {
	    free(that->Headers[i]);

	    that->Headers[i] = NULL;
	}
    }
    for (i = 0; i < MAX_MIXPLUGINS; i++) {
	if ((that->m_MixPlugins[i].nPluginDataSize)
	    && (that->m_MixPlugins[i].pPluginData)) {
	    that->m_MixPlugins[i].nPluginDataSize = 0;
	    free(that->m_MixPlugins[i].pPluginData);
	    that->m_MixPlugins[i].pPluginData = NULL;
	}
	that->m_MixPlugins[i].pMixState = NULL;
#ifndef MODPLUG_NO_IMIXPLUGIN
	if (that->m_MixPlugins[i].pMixPlugin) {
	    that->m_MixPlugins[i].pMixPlugin->Release();
	    that->m_MixPlugins[i].pMixPlugin = NULL;
	}
#endif
    }
    that->m_nType = MOD_TYPE_NONE;
    that->m_nChannels = that->m_nSamples = that->m_nInstruments = 0;
    return 1;
}

//////////////////////////////////////////////////////////////////////////
// Memory Allocation

MODCOMMAND *CSoundFile_AllocatePattern(unsigned rows, unsigned nchns)
//------------------------------------------------------------
{
    return (MODCOMMAND *) calloc(rows * nchns, sizeof(MODCOMMAND));
}

void CSoundFile_FreePattern(MODCOMMAND * pat)
//--------------------------------------
{
    free(pat);
}

int8_t *CSoundFile_AllocateSample(unsigned nbytes)
//-------------------------------------------
{
    int8_t *p = (int8_t *) GlobalAllocPtr(GHND, (nbytes + 39) & ~7);

    if (p) {
	p += 16;
    }
    return p;
}

void CSoundFile_FreeSample(void *p)
//-----------------------------------
{
    if (p) {
	GlobalFreePtr(((char *)p) - 16);
    }
}

//////////////////////////////////////////////////////////////////////////
// Misc functions

void CSoundFile_ResetMidiCfg(CSoundFile * that)
//-----------------------------
{
    unsigned i;

    memset(&that->m_MidiCfg, 0, sizeof(that->m_MidiCfg));
    strcpy(&that->m_MidiCfg.szMidiGlb[MIDIOUT_START * 32], "FF");
    strcpy(&that->m_MidiCfg.szMidiGlb[MIDIOUT_STOP * 32], "FC");
    strcpy(&that->m_MidiCfg.szMidiGlb[MIDIOUT_NOTEON * 32], "9c n v");
    strcpy(&that->m_MidiCfg.szMidiGlb[MIDIOUT_NOTEOFF * 32], "9c n 0");
    strcpy(&that->m_MidiCfg.szMidiGlb[MIDIOUT_PROGRAM * 32], "Cc p");
    strcpy(&that->m_MidiCfg.szMidiSFXExt[0], "F0F000z");

    for (i = 0; i < 16; i++) {
	sprintf(&that->m_MidiCfg.szMidiZXXExt[i * 32], "F0F001%02X", i * 8);
    }
}

unsigned CSoundFile_GetNumChannels(CSoundFile const * that)
//-------------------------------------
{
    unsigned i;
    unsigned n;

    for (i = n = 0; i < that->m_nChannels; i++) {
	if (that->ChnSettings[i].nVolume) {
	    n++;
	}
    }
    return n;
}

unsigned CSoundFile_GetSongComments(CSoundFile const * that, char *s,
    unsigned len, unsigned linesize)
//----------------------------------------------------------------
{
    unsigned i;
    unsigned ln;
    const char *p = that->m_lpszSongComments;

    if (!p) {
	return 0;
    }

    i = 2;
    ln = 0;

    if ((len) && (s))
	s[0] = '\x0D';
    if ((len > 1) && (s))
	s[1] = '\x0A';
    while ((*p) && (i + 2 < len)) {
	uint8_t c = (uint8_t) * p++;

	if ((c == 0x0D) || ((c == ' ') && (ln >= linesize))) {
	    if (s) {
		s[i++] = '\x0D';
		s[i++] = '\x0A';
	    } else
		i += 2;
	    ln = 0;
	} else if (c >= 0x20) {
	    if (s)
		s[i++] = c;
	    else
		i++;
	    ln++;
	}
    }
    if (s)
	s[i] = 0;
    return i;
}

unsigned CSoundFile_GetRawSongComments(CSoundFile const * that, char *s,
    unsigned len, unsigned linesize)
//-------------------------------------------------------------------
{
    const char *p = that->m_lpszSongComments;
    unsigned i;
    unsigned ln;

    if (!p) {
	return 0;
    }
    i = 0;
    ln = 0;

    while ((*p) && (i < len - 1)) {
	uint8_t c = (uint8_t) * p++;

	if ((c == 0x0D) || (c == 0x0A)) {
	    if (ln) {
		while (ln < linesize) {
		    if (s) {
			s[i] = ' ';
		    }
		    i++;
		    ln++;
		}
		ln = 0;
	    }
	} else if ((c == ' ') && (!ln)) {
	    unsigned k = 0;

	    while ((p[k]) && (p[k] >= ' ')) {
		k++;
	    }
	    if (k <= linesize) {
		if (s) {
		    s[i] = ' ';
		}
		i++;
		ln++;
	    }
	} else {
	    if (s) {
		s[i] = c;
	    }
	    i++;
	    ln++;
	    if (ln == linesize) {
		ln = 0;
	    }
	}
    }
    if (ln) {
	while ((ln < linesize) && (i < len)) {
	    if (s) {
		s[i] = ' ';
	    }
	    i++;
	    ln++;
	}
    }
    if (s) {
	s[i] = 0;
    }
    return i;
}

int CSoundFile_SetWaveConfig(CSoundFile * that __attribute__((unused)),
    unsigned nRate, unsigned nBits, unsigned nChannels, int bMMX)
//----------------------------------------------------------------------------
{
    int bReset = 0;
    uint32_t d = CSoundFile_gdwSoundSetup & ~SNDMIX_ENABLEMMX;

    if (bMMX) {
	d |= SNDMIX_ENABLEMMX;
    }
    if ((CSoundFile_gdwMixingFreq != nRate)
	|| (CSoundFile_gnBitsPerSample != nBits)
	|| (CSoundFile_gnChannels != nChannels)
	|| (d != CSoundFile_gdwSoundSetup)) {
	bReset = 1;
    }
    CSoundFile_gnChannels = nChannels;
    CSoundFile_gdwSoundSetup = d;
    CSoundFile_gdwMixingFreq = nRate;
    CSoundFile_gnBitsPerSample = nBits;
    CSoundFile_InitPlayer(bReset);
    return 1;
}

/**
**	Set resampling mode
**
**	@param that	Unused, FIXME: should be removed.
**	@param mode	Wanted resampling mode, #SRCMODE_NEAREST,
**			#SRCMODE_LINEAR, #SRCMODE_SPLINE, #SRCMODE_POLYPHASE.
*/
int CSoundFile_SetResamplingMode(CSoundFile * that __attribute__((unused)),
    unsigned mode)
{
    uint32_t d =
	 CSoundFile_gdwSoundSetup & ~(SNDMIX_NORESAMPLING
	     | SNDMIX_HQRESAMPLER | SNDMIX_ULTRAHQSRCMODE);

    switch (mode) {
	case SRCMODE_NEAREST:
	    d |= SNDMIX_NORESAMPLING;
	    break;
	case SRCMODE_LINEAR:
	    break;
	case SRCMODE_SPLINE:
	    d |= SNDMIX_HQRESAMPLER;
	    break;
	case SRCMODE_POLYPHASE:
	    d |= (SNDMIX_HQRESAMPLER | SNDMIX_ULTRAHQSRCMODE);
	    break;
	default:
	    return 0;
    }
    CSoundFile_gdwSoundSetup = d;

    return 1;
}

int CSoundFile_SetMasterVolume(CSoundFile * that, unsigned nVol,
    int bAdjustAGC)
//----------------------------------------------------------
{
    if (nVol < 1) {
	nVol = 1;
    }
    if (nVol > 0x200) {
	nVol = 0x200;			// x4 maximum
    }
#ifndef MODPLUG_NO_AGC
    if ((nVol < that->m_nMasterVolume) && (nVol)
	    && (CSoundFile_gdwSoundSetup & SNDMIX_AGC) && (bAdjustAGC)) {
	CSoundFile_gnAGC = CSoundFile_gnAGC * that->m_nMasterVolume / nVol;
	if (CSoundFile_gnAGC > AGC_UNITY) {
	    CSoundFile_gnAGC = AGC_UNITY;
	}
    }
#else
    bAdjustAGC=bAdjustAGC;
#endif
    that->m_nMasterVolume = nVol;
    return 1;
}

void CSoundFile_SetAGC(int b)
//-----------------------------
{
    if (b) {
	if (!(CSoundFile_gdwSoundSetup & SNDMIX_AGC)) {
	    CSoundFile_gdwSoundSetup |= SNDMIX_AGC;
#ifndef MODPLUG_NO_AGC
	    CSoundFile_gnAGC = AGC_UNITY;
#endif
	}
    } else {
	CSoundFile_gdwSoundSetup &= ~SNDMIX_AGC;
    }
}

unsigned CSoundFile_GetNumPatterns(CSoundFile const *that)
//-------------------------------------
{
    unsigned i = 0;

    while ((i < MAX_ORDERS) && (that->Order[i] < 0xFF))
	i++;
    return i;
}

unsigned CSoundFile_GetNumInstruments(CSoundFile const *that)
//----------------------------------------
{
    unsigned i;
    unsigned n;

    for (i = n = 0; i < MAX_INSTRUMENTS; i++) {
	if (that->Ins[i].pSample) {
	    n++;
	}
    }
    return n;
}

unsigned CSoundFile_GetMaxPosition(CSoundFile const *that)
//-------------------------------------
{
    unsigned max = 0;
    unsigned i = 0;

    while ((i < MAX_ORDERS) && (that->Order[i] != 0xFF)) {
	if (that->Order[i] < MAX_PATTERNS) {
	    max += that->PatternSize[that->Order[i]];
	}
	i++;
    }
    return max;
}

unsigned CSoundFile_GetCurrentPos(CSoundFile const * that)
//------------------------------------
{
    unsigned pos;
    unsigned i;

    for (i = pos = 0; i < that->m_nCurrentPattern; i++) {
	if (that->Order[i] < MAX_PATTERNS) {
	    pos += that->PatternSize[that->Order[i]];
	}
    }
    return pos + that->m_nRow;
}

void CSoundFile_SetCurrentPos(CSoundFile * that, unsigned nPos)
//---------------------------------------
{
    unsigned nRow;
    unsigned i;
    unsigned nPattern;

    for (i = 0; i < MAX_CHANNELS; i++) {
	that->Chn[i].nNote = that->Chn[i].nNewNote = that->Chn[i].nNewIns = 0;
	that->Chn[i].pInstrument = NULL;
	that->Chn[i].pHeader = NULL;
	that->Chn[i].nPortamentoDest = 0;
	that->Chn[i].nCommand = 0;
	that->Chn[i].nPatternLoopCount = 0;
	that->Chn[i].nPatternLoop = 0;
	that->Chn[i].nFadeOutVol = 0;
	that->Chn[i].dwFlags |= CHN_KEYOFF | CHN_NOTEFADE;
	that->Chn[i].nTremorCount = 0;
    }
    if (!nPos) {
	for (i = 0; i < MAX_CHANNELS; i++) {
	    that->Chn[i].nPeriod = 0;
	    that->Chn[i].nPos = that->Chn[i].nLength = 0;
	    that->Chn[i].nLoopStart = 0;
	    that->Chn[i].nLoopEnd = 0;
	    that->Chn[i].nROfs = that->Chn[i].nLOfs = 0;
	    that->Chn[i].pSample = NULL;
	    that->Chn[i].pInstrument = NULL;
	    that->Chn[i].pHeader = NULL;
	    that->Chn[i].nCutOff = 0x7F;
	    that->Chn[i].nResonance = 0;
	    that->Chn[i].nLeftVol = that->Chn[i].nRightVol = 0;
	    that->Chn[i].nNewLeftVol = that->Chn[i].nNewRightVol = 0;
	    that->Chn[i].nLeftRamp = that->Chn[i].nRightRamp = 0;
	    that->Chn[i].nVolume = 256;
	    if (i < MAX_BASECHANNELS) {
		that->Chn[i].dwFlags = that->ChnSettings[i].dwFlags;
		that->Chn[i].nPan = that->ChnSettings[i].nPan;
		that->Chn[i].nGlobalVol = that->ChnSettings[i].nVolume;
	    } else {
		that->Chn[i].dwFlags = 0;
		that->Chn[i].nPan = 128;
		that->Chn[i].nGlobalVol = 64;
	    }
	}
	that->m_nGlobalVolume = that->m_nDefaultGlobalVolume;
	that->m_nMusicSpeed = that->m_nDefaultSpeed;
	that->m_nMusicTempo = that->m_nDefaultTempo;
    }
    that->m_dwSongFlags &=
	~(SONG_PATTERNLOOP | SONG_CPUVERYHIGH | SONG_FADINGSONG |
	SONG_ENDREACHED | SONG_GLOBALFADE);
    for (nPattern = 0; nPattern < MAX_ORDERS; nPattern++) {
	unsigned ord = that->Order[nPattern];

	if (ord == 0xFE)
	    continue;
	if (ord == 0xFF)
	    break;
	if (ord < MAX_PATTERNS) {
	    if (nPos < (unsigned)that->PatternSize[ord])
		break;
	    nPos -= that->PatternSize[ord];
	}
    }
    // Buggy position ?
    if ((nPattern >= MAX_ORDERS)
	|| (that->Order[nPattern] >= MAX_PATTERNS)
	|| (nPos >= that->PatternSize[that->Order[nPattern]])) {
	nPos = 0;
	nPattern = 0;
    }
    nRow = nPos;

    if ((nRow) && (that->Order[nPattern] < MAX_PATTERNS)) {
	MODCOMMAND *p = that->Patterns[that->Order[nPattern]];

	if ((p) && (nRow < that->PatternSize[that->Order[nPattern]])) {
	    int bOk = 0;

	    while ((!bOk) && (nRow > 0)) {
		unsigned n = nRow * that->m_nChannels;
		unsigned k;

		for (k = 0; k < that->m_nChannels; k++, n++) {
		    if (p[n].note) {
			bOk = 1;
			break;
		    }
		}
		if (!bOk)
		    nRow--;
	    }
	}
    }
    that->m_nNextPattern = nPattern;
    that->m_nNextRow = nRow;
    that->m_nTickCount = that->m_nMusicSpeed;
    that->m_nBufferCount = 0;
    that->m_nPatternDelay = 0;
    that->m_nFrameDelay = 0;
}

void CSoundFile_SetCurrentOrder(CSoundFile * that, unsigned nPos)
//-----------------------------------------
{
    unsigned j;

    while ((nPos < MAX_ORDERS) && (that->Order[nPos] == 0xFE)) {
	nPos++;
    }
    if ((nPos >= MAX_ORDERS) || (that->Order[nPos] >= MAX_PATTERNS)) {
	return;
    }
    for (j = 0; j < MAX_CHANNELS; j++) {
	that->Chn[j].nPeriod = 0;
	that->Chn[j].nNote = 0;
	that->Chn[j].nPortamentoDest = 0;
	that->Chn[j].nCommand = 0;
	that->Chn[j].nPatternLoopCount = 0;
	that->Chn[j].nPatternLoop = 0;
	that->Chn[j].nTremorCount = 0;
    }
    if (!nPos) {
	CSoundFile_SetCurrentPos(that, 0);
    } else {
	that->m_nNextPattern = nPos;
	that->m_nRow = that->m_nNextRow = 0;
	that->m_nPattern = 0;
	that->m_nTickCount = that->m_nMusicSpeed;
	that->m_nBufferCount = 0;
	that->m_nTotalCount = 0;
	that->m_nPatternDelay = 0;
	that->m_nFrameDelay = 0;
    }
    that->m_dwSongFlags &=
	~(SONG_PATTERNLOOP | SONG_CPUVERYHIGH | SONG_FADINGSONG |
	SONG_ENDREACHED | SONG_GLOBALFADE);
}

void CSoundFile_ResetChannels(CSoundFile * that)
//------------------------------
{
    unsigned i;

    that->m_dwSongFlags &= ~(SONG_CPUVERYHIGH | SONG_FADINGSONG
	    | SONG_ENDREACHED | SONG_GLOBALFADE);
    that->m_nBufferCount = 0;

    for (i = 0; i < MAX_CHANNELS; i++) {
	that->Chn[i].nROfs = that->Chn[i].nLOfs = 0;
    }
}

void CSoundFile_LoopPattern(CSoundFile * that, int nPat, int nRow)
//----------------------------------------------
{
    if ((nPat < 0) || (nPat >= MAX_PATTERNS) || (!that->Patterns[nPat])) {
	that->m_dwSongFlags &= ~SONG_PATTERNLOOP;
    } else {
	if ((nRow < 0) || (nRow >= that->PatternSize[nPat]))
	    nRow = 0;
	that->m_nPattern = nPat;
	that->m_nRow = that->m_nNextRow = nRow;
	that->m_nTickCount = that->m_nMusicSpeed;
	that->m_nPatternDelay = 0;
	that->m_nFrameDelay = 0;
	that->m_nBufferCount = 0;
	that->m_dwSongFlags |= SONG_PATTERNLOOP;
    }
}

unsigned CSoundFile_GetBestSaveFormat(CSoundFile * that)
//----------------------------------------
{
    if ((!that->m_nSamples) || (!that->m_nChannels))
	return MOD_TYPE_NONE;
    if (!that->m_nType)
	return MOD_TYPE_NONE;
    if (that->m_nType & (MOD_TYPE_MOD | MOD_TYPE_OKT))
	return MOD_TYPE_MOD;
    if (that->
	m_nType & (MOD_TYPE_S3M | MOD_TYPE_STM | MOD_TYPE_ULT | MOD_TYPE_FAR |
	    MOD_TYPE_PTM))
	return MOD_TYPE_S3M;
    if (that->
	m_nType & (MOD_TYPE_XM | MOD_TYPE_MED | MOD_TYPE_MTM | MOD_TYPE_MT2))
	return MOD_TYPE_XM;
    return MOD_TYPE_IT;
}

unsigned CSoundFile_GetSaveFormats(CSoundFile * that)
//-------------------------------------
{
    unsigned n = 0;

    if ((!that->m_nSamples) || (!that->m_nChannels)
	|| (that->m_nType == MOD_TYPE_NONE))
	return 0;
    switch (that->m_nType) {
	case MOD_TYPE_MOD:
	    n = MOD_TYPE_MOD;
	case MOD_TYPE_S3M:
	    n = MOD_TYPE_S3M;
    }
    n |= MOD_TYPE_XM | MOD_TYPE_IT;
    if (!that->m_nInstruments) {
	if (that->m_nSamples < 32)
	    n |= MOD_TYPE_MOD;
	n |= MOD_TYPE_S3M;
    }
    return n;
}

unsigned CSoundFile_GetSampleName(CSoundFile * that, unsigned nSample, char *s)
//--------------------------------------------------------
{
    char sztmp[40] = "";

    memcpy(sztmp, that->m_szNames[nSample], 32);
    sztmp[31] = 0;
    if (s)
	strcpy(s, sztmp);
    return strlen(sztmp);
}

unsigned CSoundFile_GetInstrumentName(CSoundFile * that, unsigned nInstr,
    char *s)
//-----------------------------------------------------------
{
    char buf[40];
    INSTRUMENTHEADER *penv;

    if ((nInstr >= MAX_INSTRUMENTS) || (!that->Headers[nInstr])) {
	if (s) {
	    *s = '\0';
	}
	return 0;
    }
    penv = that->Headers[nInstr];
    memcpy(buf, penv->Name, 32);
    buf[31] = 0;
    if (s) {
	strcpy(s, buf);
    }

    return strlen(buf);
}

#ifndef MODPLUG_NO_PACKING
static unsigned PackSample(CSoundFile const * that, int *sample, int next)
//------------------------------------------------
{
    unsigned i = 0;
    int delta = next - *sample;

    if (delta >= 0) {
	for (i = 0; i < 7; i++)
	    if (delta <= (int)that->CompressionTable[i + 1])
		break;
    } else {
	for (i = 8; i < 15; i++)
	    if (delta >= (int)that->CompressionTable[i + 1])
		break;
    }
    *sample += (int)that->CompressionTable[i];
    return i;
}

int CSoundFile_CanPackSample(CSoundFile * that, char const *pSample,
    unsigned nLen, unsigned nPacking, uint8_t * result)
//-----------------------------------------------------------------------------------
{
    int pos, old, oldpos, besttable = 0;
    uint32_t dwErr, dwTotal, dwResult;
    int i, j;

    if (result) {
	*result = 0;
    }
    if ((!pSample) || (nLen < 1024))
	return 0;
    // Try packing with different tables
    dwResult = 0;
    for (j = 1; j < MAX_PACK_TABLES; j++) {
	memcpy(that->CompressionTable, UnpackTable[j], 16);
	dwErr = 0;
	dwTotal = 1;
	old = pos = oldpos = 0;
	for (i = 0; i < (int)nLen; i++) {
	    int s = (int)pSample[i];

	    PackSample(that,&pos, s);
	    dwErr += abs(pos - oldpos);
	    dwTotal += abs(s - old);
	    old = s;
	    oldpos = pos;
	}
	dwErr = _muldiv(dwErr, 100, dwTotal);
	if (dwErr >= dwResult) {
	    dwResult = dwErr;
	    besttable = j;
	}
    }
    memcpy(that->CompressionTable, UnpackTable[besttable], 16);
    if (result) {
	if (dwResult > 100)
	    *result = 100;
	else
	    *result = (uint8_t) dwResult;
    }
    return (dwResult >= nPacking) ? 1 : 0;
}

#endif // MODPLUG_NO_PACKING

#ifndef MODPLUG_NO_FILESAVE

unsigned CSoundFile_WriteSample(CSoundFile const * that, FILE * f,
    MODINSTRUMENT const *pins, unsigned nFlags, unsigned nMaxLen)
//-----------------------------------------------------------------------------------
{
    unsigned j;
    unsigned len;
    unsigned nLen;
    unsigned bufcount;
    int8_t buffer[4096];
    int8_t *pSample;

    nLen = pins->nLength;
    if (nMaxLen && nLen > nMaxLen) {
	nLen = nMaxLen;
    }
    pSample = (int8_t *) pins->pSample;
    if (!pSample ||(f == NULL) || !nLen) {
	return 0;
    }
    len = 0;
    switch (nFlags) {
#ifndef MODPLUG_NO_PACKING
	    // 3: 4-bit ADPCM data
	case RS_ADPCM4:
	{
	    int pos;

	    len = (nLen + 1) / 2;
	    fwrite(that->CompressionTable, 16, 1, f);
	    bufcount = 0;
	    pos = 0;
	    for (j = 0; j < len; j++) {
		uint8_t b;

		// Sample #1
		b = PackSample(that,&pos, (int)pSample[j * 2]);
		// Sample #2
		b |= PackSample(that, &pos, (int)pSample[j * 2 + 1]) << 4;
		buffer[bufcount++] = (int8_t) b;
		if (bufcount >= sizeof(buffer)) {
		    fwrite(buffer, 1, bufcount, f);
		    bufcount = 0;
		}
	    }
	    if (bufcount)
		fwrite(buffer, 1, bufcount, f);
	    len += 16;
	}
	    break;
#endif // MODPLUG_NO_PACKING

	    // 16-bit samples
	case RS_PCM16U:
	case RS_PCM16D:
	case RS_PCM16S:
	{
	    int16_t *p = (int16_t *) pSample;
	    int s_old = 0, s_ofs;

	    len = nLen * 2;
	    bufcount = 0;
	    s_ofs = (nFlags == RS_PCM16U) ? 0x8000 : 0;
	    for (j = 0; j < nLen; j++) {
		int s_new = *p;

		p++;
		if (pins->uFlags & CHN_STEREO) {
		    s_new = (s_new + (*p) + 1) >> 1;
		    p++;
		}
		if (nFlags == RS_PCM16D) {
		    *((short *)(&buffer[bufcount])) = (short)(s_new - s_old);
		    s_old = s_new;
		} else {
		    *((short *)(&buffer[bufcount])) = (short)(s_new + s_ofs);
		}
		bufcount += 2;
		if (bufcount >= sizeof(buffer) - 1) {
		    fwrite(buffer, 1, bufcount, f);
		    bufcount = 0;
		}
	    }
	    if (bufcount)
		fwrite(buffer, 1, bufcount, f);
	}
	    break;

	    // 8-bit Stereo samples (not interleaved)
	case RS_STPCM8S:
	case RS_STPCM8U:
	case RS_STPCM8D:
	{
	    int s_ofs = (nFlags == RS_STPCM8U) ? 0x80 : 0;
	    unsigned  iCh;

	    for (iCh = 0; iCh < 2; iCh++) {
		int8_t *p = pSample + iCh;
		int s_old = 0;

		bufcount = 0;
		for (j = 0; j < nLen; j++) {
		    int s_new = *p;

		    p += 2;
		    if (nFlags == RS_STPCM8D) {
			buffer[bufcount++] = (int8_t) (s_new - s_old);
			s_old = s_new;
		    } else {
			buffer[bufcount++] = (int8_t) (s_new + s_ofs);
		    }
		    if (bufcount >= sizeof(buffer)) {
			fwrite(buffer, 1, bufcount, f);
			bufcount = 0;
		    }
		}
		if (bufcount)
		    fwrite(buffer, 1, bufcount, f);
	    }
	}
	    len = nLen * 2;
	    break;

	    // 16-bit Stereo samples (not interleaved)
	case RS_STPCM16S:
	case RS_STPCM16U:
	case RS_STPCM16D:
	{
	    int s_ofs = (nFlags == RS_STPCM16U) ? 0x8000 : 0;
	    unsigned iCh;

	    for (iCh = 0; iCh < 2; iCh++) {
		signed short *p = ((signed short *)pSample) + iCh;
		int s_old = 0;

		bufcount = 0;
		for (j = 0; j < nLen; j++) {
		    int s_new = *p;

		    p += 2;
		    if (nFlags == RS_STPCM16D) {
			*((short *)(&buffer[bufcount])) =
			    (short)(s_new - s_old);
			s_old = s_new;
		    } else {
			*((short *)(&buffer[bufcount])) =
			    (short)(s_new + s_ofs);
		    }
		    bufcount += 2;
		    if (bufcount >= sizeof(buffer)) {
			fwrite(buffer, 1, bufcount, f);
			bufcount = 0;
		    }
		}
		if (bufcount) {
		    fwrite(buffer, 1, bufcount, f);
		}
	    }
	}
	    len = nLen * 4;
	    break;

	    //  Stereo signed interleaved
	case RS_STIPCM8S:
	case RS_STIPCM16S:
	    len = nLen * 2;
	    if (nFlags == RS_STIPCM16S) {
		len *= 2;
	    }
	    fwrite(pSample, 1, len, f);
	    break;

	    // Default: assume 8-bit PCM data
	default:
	    len = nLen;
	    bufcount = 0;
	    {
		int8_t *p = pSample;
		int sinc = (pins->uFlags & CHN_16BIT) ? 2 : 1;
		int s_old = 0, s_ofs = (nFlags == RS_PCM8U) ? 0x80 : 0;

		if (pins->uFlags & CHN_16BIT)
		    p++;
		for (j = 0; j < len; j++) {
		    int s_new = (int8_t) (*p);

		    p += sinc;
		    if (pins->uFlags & CHN_STEREO) {
			s_new = (s_new + ((int)*p) + 1) >> 1;
			p += sinc;
		    }
		    if (nFlags == RS_PCM8D) {
			buffer[bufcount++] = (int8_t) (s_new - s_old);
			s_old = s_new;
		    } else {
			buffer[bufcount++] = (int8_t) (s_new + s_ofs);
		    }
		    if (bufcount >= sizeof(buffer)) {
			fwrite(buffer, 1, bufcount, f);
			bufcount = 0;
		    }
		}
		if (bufcount) {
		    fwrite(buffer, 1, bufcount, f);
		}
	    }
    }
    return len;
}

#endif // MODPLUG_NO_FILESAVE

// Flags:
//  0 = signed 8-bit PCM data (default)
//  1 = unsigned 8-bit PCM data
//  2 = 8-bit ADPCM data with linear table
//  3 = 4-bit ADPCM data
//  4 = 16-bit ADPCM data with linear table
//  5 = signed 16-bit PCM data
//  6 = unsigned 16-bit PCM data

unsigned CSoundFile_ReadSample(CSoundFile * that, MODINSTRUMENT * pIns,
    unsigned nFlags, const char *lpMemFile, uint32_t dwMemLength)
//------------------------------------------------------------------------------------------------
{
    unsigned len;
    unsigned mem;
    unsigned j;

    if (!pIns || pIns->nLength < 4 || !lpMemFile) {
	return 0;
    }
    if (pIns->nLength > MAX_SAMPLE_LENGTH) {
	pIns->nLength = MAX_SAMPLE_LENGTH;
    }
    mem = pIns->nLength + 6;
    pIns->uFlags &= ~(CHN_16BIT | CHN_STEREO);
    if (nFlags & RSF_16BIT) {
	mem *= 2;
	pIns->uFlags |= CHN_16BIT;
    }
    if (nFlags & RSF_STEREO) {
	mem *= 2;
	pIns->uFlags |= CHN_STEREO;
    }
    if ((pIns->pSample = CSoundFile_AllocateSample(mem)) == NULL) {
	pIns->nLength = 0;
	return 0;
    }

    switch (nFlags) {
	    // 1: 8-bit unsigned PCM data
	case RS_PCM8U:
	{
	    int8_t *pSample;

	    len = pIns->nLength;
	    if (len > dwMemLength)
		len = pIns->nLength = dwMemLength;
	    pSample = pIns->pSample;

	    for (j = 0; j < len; j++)
		pSample[j] = (int8_t) (lpMemFile[j] - 0x80);
	}
	    break;

	    // 2: 8-bit ADPCM data with linear table
	case RS_PCM8D:
	{
	    int8_t *pSample;
	    const int8_t *p;
	    int delta;

	    len = pIns->nLength;
	    if (len > dwMemLength) {
		break;
	    }
	    pSample = pIns->pSample;
	    p = (const int8_t *)lpMemFile;
	    delta = 0;

	    for (j = 0; j < len; j++) {
		delta += p[j];
		*pSample++ = (int8_t) delta;
	    }
	}
	    break;

	    // 3: 4-bit ADPCM data
	case RS_ADPCM4:
	{
	    int8_t *pSample;
	    int8_t delta;

	    len = (pIns->nLength + 1) / 2;
	    if (len > dwMemLength - 16)
		break;
	    memcpy(that->CompressionTable, lpMemFile, 16);
	    lpMemFile += 16;
	    pSample = pIns->pSample;
	    delta = 0;

	    for (j = 0; j < len; j++) {
		uint8_t b0 = (uint8_t) lpMemFile[j];
		uint8_t b1 = (uint8_t) (lpMemFile[j] >> 4);

		delta =
		    (int8_t) CSoundFile_GetDeltaValue(that, (int)delta, b0);
		pSample[0] = delta;
		delta =
		    (int8_t) CSoundFile_GetDeltaValue(that, (int)delta, b1);
		pSample[1] = delta;
		pSample += 2;
	    }
	    len += 16;
	}
	    break;

	    // 4: 16-bit ADPCM data with linear table
	case RS_PCM16D:
	{
	    int16_t *pSample;
	    int16_t *p;
	    int delta16;

	    len = pIns->nLength * 2;
	    if (len > dwMemLength)
		break;
	    pSample = (int16_t *) pIns->pSample;
	    p = (int16_t *) lpMemFile;
	    delta16 = 0;

	    for (j = 0; j < len; j += 2) {
		delta16 += bswapLE16(*p++);
		*pSample++ = (int16_t) delta16;
	    }
	}
	    break;

	    // 5: 16-bit signed PCM data
	case RS_PCM16S:
	{
	    int16_t *pSample;

	    len = pIns->nLength * 2;
	    if (len <= dwMemLength)
		memcpy(pIns->pSample, lpMemFile, len);
	    pSample = (int16_t *) pIns->pSample;

	    for (j = 0; j < len; j += 2) {
		*pSample = bswapLE16(*pSample);
		pSample++;
	    }
	}
	    break;

	    // 16-bit signed mono PCM motorola byte order
	case RS_PCM16M:
	    len = pIns->nLength * 2;
	    if (len > dwMemLength)
		len = dwMemLength & ~1;
	    if (len > 1) {
		int8_t *pSample = (int8_t *) pIns->pSample;
		int8_t *pSrc = (int8_t *) lpMemFile;

		for (j = 0; j < len; j += 2) {
		    // pSample[j] = pSrc[j+1];
		    // pSample[j+1] = pSrc[j];
		    *((unsigned short *)(pSample + j)) =
			bswapBE16(*((unsigned short *)(pSrc + j)));
		}
	    }
	    break;

	    // 6: 16-bit unsigned PCM data
	case RS_PCM16U:
	{
	    int16_t *pSample;
	    int16_t *pSrc;

	    len = pIns->nLength * 2;
	    if (len > dwMemLength)
		break;
	    pSample = (int16_t *) pIns->pSample;
	    pSrc = (int16_t *) lpMemFile;

	    for (j = 0; j < len; j += 2)
		*pSample++ = bswapLE16(*(pSrc++)) - 0x8000;
	}
	    break;

	    // 16-bit signed stereo big endian
	case RS_STPCM16M:
	    len = pIns->nLength * 2;
	    if (len * 2 <= dwMemLength) {
		int8_t *pSample = (int8_t *) pIns->pSample;
		int8_t *pSrc = (int8_t *) lpMemFile;

		for (j = 0; j < len; j += 2) {
		    // pSample[j*2] = pSrc[j+1];
		    // pSample[j*2+1] = pSrc[j];
		    // pSample[j*2+2] = pSrc[j+1+len];
		    // pSample[j*2+3] = pSrc[j+len];
		    *((unsigned short *)(pSample + j * 2)) =
			bswapBE16(*((unsigned short *)(pSrc + j)));
		    *((unsigned short *)(pSample + j * 2 + 2)) =
			bswapBE16(*((unsigned short *)(pSrc + j + len)));
		}
		len *= 2;
	    }
	    break;

	    // 8-bit stereo samples
	case RS_STPCM8S:
	case RS_STPCM8U:
	case RS_STPCM8D:
	{
	    int iadd_l;
	    int iadd_r;
	    int8_t *psrc;
	    int8_t *pSample;

	    iadd_l = iadd_r = 0;
	    if (nFlags == RS_STPCM8U) {
		iadd_l = iadd_r = -128;
	    }
	    len = pIns->nLength;
	    psrc = (int8_t *) lpMemFile;
	    pSample = (int8_t *) pIns->pSample;

	    if (len * 2 > dwMemLength)
		break;
	    for (j = 0; j < len; j++) {
		pSample[j * 2] = (int8_t) (psrc[0] + iadd_l);
		pSample[j * 2 + 1] = (int8_t) (psrc[len] + iadd_r);
		psrc++;
		if (nFlags == RS_STPCM8D) {
		    iadd_l = pSample[j * 2];
		    iadd_r = pSample[j * 2 + 1];
		}
	    }
	    len *= 2;
	}
	    break;

	    // 16-bit stereo samples
	case RS_STPCM16S:
	case RS_STPCM16U:
	case RS_STPCM16D:
	{
	    int iadd_l = 0, iadd_r = 0;
	    int16_t *psrc;
	    int16_t *pSample;

	    if (nFlags == RS_STPCM16U) {
		iadd_l = iadd_r = -0x8000;
	    }
	    len = pIns->nLength;
	    psrc = (int16_t *) lpMemFile;
	    pSample = (int16_t *) pIns->pSample;

	    if (len * 4 > dwMemLength)
		break;
	    for (j = 0; j < len; j++) {
		pSample[j * 2] = (int16_t) (bswapLE16(psrc[0]) + iadd_l);
		pSample[j * 2 + 1] = (int16_t) (bswapLE16(psrc[len]) + iadd_r);
		psrc++;
		if (nFlags == RS_STPCM16D) {
		    iadd_l = pSample[j * 2];
		    iadd_r = pSample[j * 2 + 1];
		}
	    }
	    len *= 4;
	}
	    break;

	    // IT 2.14 compressed samples
	case RS_IT2148:
	case RS_IT21416:
	case RS_IT2158:
	case RS_IT21516:
	    len = dwMemLength;
	    if (len < 4)
		break;
	    if ((nFlags == RS_IT2148) || (nFlags == RS_IT2158))
		CSoundFile_ITUnpack8Bit(pIns->pSample, pIns->nLength,
		    (uint8_t *) lpMemFile, dwMemLength, (nFlags == RS_IT2158));
	    else
		CSoundFile_ITUnpack16Bit(pIns->pSample, pIns->nLength,
		    (uint8_t *) lpMemFile, dwMemLength,
		    (nFlags == RS_IT21516));
	    break;

#ifndef MODPLUG_BASIC_SUPPORT
#ifndef MODPLUG_FASTSOUNDLIB
	    // 8-bit interleaved stereo samples
	case RS_STIPCM8S:
	case RS_STIPCM8U:
	{
	    int iadd = 0;
	    uint8_t *psrc;
	    uint8_t *pSample;

	    if (nFlags == RS_STIPCM8U) {
		iadd = -0x80;
	    }
	    len = pIns->nLength;
	    if (len * 2 > dwMemLength)
		len = dwMemLength >> 1;
	    psrc = (uint8_t *) lpMemFile;
	    pSample = (uint8_t *) pIns->pSample;

	    for (j = 0; j < len; j++) {
		pSample[j * 2] = (int8_t) (psrc[0] + iadd);
		pSample[j * 2 + 1] = (int8_t) (psrc[1] + iadd);
		psrc += 2;
	    }
	    len *= 2;
	}
	    break;

	    // 16-bit interleaved stereo samples
	case RS_STIPCM16S:
	case RS_STIPCM16U:
	{
	    int16_t *psrc;
	    int16_t *pSample;
	    int iadd = 0;

	    if (nFlags == RS_STIPCM16U)
		iadd = -32768;
	    len = pIns->nLength;
	    if (len * 4 > dwMemLength)
		len = dwMemLength >> 2;
	    psrc = (int16_t *) lpMemFile;
	    pSample = (int16_t *) pIns->pSample;

	    for (j = 0; j < len; j++) {
		pSample[j * 2] = (int16_t) (bswapLE16(psrc[0]) + iadd);
		pSample[j * 2 + 1] = (int16_t) (bswapLE16(psrc[1]) + iadd);
		psrc += 2;
	    }
	    len *= 4;
	}
	    break;

	    // AMS compressed samples
	case RS_AMS8:
	case RS_AMS16:
	    len = 9;
	    if (dwMemLength > 9) {
		const char *psrc = lpMemFile;
		char packcharacter = lpMemFile[8], *pdest =
		    (char *)pIns->pSample;
		unsigned dmax;

		len += bswapLE32(*((uint32_t *) (lpMemFile + 4)));
		if (len > dwMemLength) {
		    len = dwMemLength;
		}
		dmax = pIns->nLength;
		if (pIns->uFlags & CHN_16BIT) {
		    dmax <<= 1;
		}
		CSoundFile_AMSUnpack(psrc + 9, len - 9, pdest, dmax,
		    packcharacter);
	    }
	    break;

	    // PTM 8bit delta to 16-bit sample
	case RS_PTM8DTO16:
	{
	    int8_t *pSample;
	    int8_t delta8;
	    uint16_t *pSampleW;

	    len = pIns->nLength * 2;
	    if (len > dwMemLength)
		break;
	    pSample = (int8_t *) pIns->pSample;
	    delta8 = 0;

	    for (j = 0; j < len; j++) {
		delta8 += lpMemFile[j];
		*pSample++ = delta8;
	    }
	    pSampleW = (uint16_t *) pIns->pSample;

	    for (j = 0; j < len; j += 2)	// swaparoni!
	    {
		*pSampleW = bswapLE16(*pSampleW);
		pSampleW++;
	    }
	}
	    break;

	    // Huffman MDL compressed samples
	case RS_MDL8:
	case RS_MDL16:
	    len = dwMemLength;
	    if (len >= 4) {
		uint8_t *pSample = (uint8_t *) pIns->pSample;
		uint8_t *ibuf = (uint8_t *) lpMemFile;
		uint32_t bitbuf = bswapLE32(*((uint32_t *) ibuf));
		unsigned bitnum = 32;
		uint8_t dlt = 0, lowbyte = 0;

		ibuf += 4;
		for (j = 0; j < pIns->nLength; j++) {
		    uint8_t hibyte;
		    uint8_t sign;

		    if (nFlags == RS_MDL16)
			lowbyte =
			    (uint8_t) MDLReadBits(&bitbuf, &bitnum, &ibuf, 8);
		    sign = (uint8_t) MDLReadBits(&bitbuf, &bitnum, &ibuf, 1);
		    if (MDLReadBits(&bitbuf, &bitnum, &ibuf, 1)) {
			hibyte =
			    (uint8_t) MDLReadBits(&bitbuf, &bitnum, &ibuf, 3);
		    } else {
			hibyte = 8;
			while (!MDLReadBits(&bitbuf, &bitnum, &ibuf, 1))
			    hibyte += 0x10;
			hibyte += MDLReadBits(&bitbuf, &bitnum, &ibuf, 4);
		    }
		    if (sign)
			hibyte = ~hibyte;
		    dlt += hibyte;
		    if (nFlags != RS_MDL16)
			pSample[j] = dlt;
		    else {
			pSample[j << 1] = lowbyte;
			pSample[(j << 1) + 1] = dlt;
		    }
		}
	    }
	    break;

	case RS_DMF8:
	case RS_DMF16:
	    len = dwMemLength;
	    if (len >= 4) {
		unsigned maxlen = pIns->nLength;
		uint8_t *ibuf;
		uint8_t *ibufmax;

		if (pIns->uFlags & CHN_16BIT) {
		    maxlen <<= 1;
		}
		ibuf = (uint8_t *) lpMemFile;
		ibufmax = (uint8_t *) (lpMemFile + dwMemLength);
		len =
		    CSoundFile_DMFUnpack((uint8_t *) pIns->pSample, ibuf,
		    ibufmax, maxlen);
	    }
	    break;

#ifdef MODPLUG_TRACKER
	    // PCM 24-bit signed -> load sample, and normalize it to 16-bit
	case RS_PCM24S:
	case RS_PCM32S:
	    len = pIns->nLength * 3;
	    if (nFlags == RS_PCM32S)
		len += pIns->nLength;
	    if (len > dwMemLength)
		break;
	    if (len > 4 * 8) {
		unsigned slsize = (nFlags == RS_PCM32S) ? 4 : 3;
		uint8_t *pSrc = (uint8_t *) lpMemFile;
		long max = 255;

		if (nFlags == RS_PCM32S)
		    pSrc++;
		for (j = 0; j < len; j += slsize) {
		    long l =
			((((pSrc[j + 2] << 8) + pSrc[j + 1]) << 8) +
			pSrc[j]) << 8;
		    l /= 256;
		    if (l > max)
			max = l;
		    if (-l > max)
			max = -l;
		}
		max = (max / 128) + 1;
		signed short *pDest = (signed short *)pIns->pSample;

		for (unsigned k = 0; k < len; k += slsize) {
		    long l =
			((((pSrc[k + 2] << 8) + pSrc[k + 1]) << 8) +
			pSrc[k]) << 8;
		    *pDest++ = (signed short)(l / max);
		}
	    }
	    break;

	    // Stereo PCM 24-bit signed -> load sample, and normalize it to 16-bit
	case RS_STIPCM24S:
	case RS_STIPCM32S:
	    len = pIns->nLength * 6;
	    if (nFlags == RS_STIPCM32S)
		len += pIns->nLength * 2;
	    if (len > dwMemLength)
		break;
	    if (len > 8 * 8) {
		unsigned slsize = (nFlags == RS_STIPCM32S) ? 4 : 3;
		uint8_t *pSrc = (uint8_t *) lpMemFile;
		long max = 255;

		if (nFlags == RS_STIPCM32S)
		    pSrc++;
		for (j = 0; j < len; j += slsize) {
		    long l =
			((((pSrc[j + 2] << 8) + pSrc[j + 1]) << 8) +
			pSrc[j]) << 8;
		    l /= 256;
		    if (l > max)
			max = l;
		    if (-l > max)
			max = -l;
		}
		max = (max / 128) + 1;
		signed short *pDest = (signed short *)pIns->pSample;

		for (unsigned k = 0; k < len; k += slsize) {
		    long lr =
			((((pSrc[k + 2] << 8) + pSrc[k + 1]) << 8) +
			pSrc[k]) << 8;
		    k += slsize;
		    long ll =
			((((pSrc[k + 2] << 8) + pSrc[k + 1]) << 8) +
			pSrc[k]) << 8;
		    pDest[0] = (signed short)ll;
		    pDest[1] = (signed short)lr;
		    pDest += 2;
		}
	    }
	    break;

	    // 16-bit signed big endian interleaved stereo
	case RS_STIPCM16M:
	{
	    len = pIns->nLength;
	    if (len * 4 > dwMemLength)
		len = dwMemLength >> 2;
	    const uint8_t *psrc = (const uint8_t *)lpMemFile;
	    int16_t *pSample = (int16_t *) pIns->pSample;

	    for (j = 0; j < len; j++) {
		pSample[j * 2] =
		    (signed short)(((unsigned)psrc[0] << 8) | (psrc[1]));
		pSample[j * 2 + 1] =
		    (signed short)(((unsigned)psrc[2] << 8) | (psrc[3]));
		psrc += 4;
	    }
	    len *= 4;
	}
	    break;

#endif // MODPLUG_TRACKER
#endif // !MODPLUG_FASTSOUNDLIB
#endif // !MODPLUG_BASIC_SUPPORT

	    // Default: 8-bit signed PCM data
	default:
	    len = pIns->nLength;
	    if (len > dwMemLength)
		len = pIns->nLength = dwMemLength;
	    memcpy(pIns->pSample, lpMemFile, len);
    }
    if (len > dwMemLength) {
	if (pIns->pSample) {
	    pIns->nLength = 0;
	    CSoundFile_FreeSample(pIns->pSample);
	    pIns->pSample = NULL;
	}
	return 0;
    }
    CSoundFile_AdjustSampleLoop(that, pIns);
    return len;
}

void CSoundFile_AdjustSampleLoop(CSoundFile * that, MODINSTRUMENT * pIns)
//----------------------------------------------------
{
    unsigned len;

    if (!pIns->pSample) {
	return;
    }
    if (pIns->nLoopEnd > pIns->nLength) {
	pIns->nLoopEnd = pIns->nLength;
    }
    if (pIns->nLoopStart + 2 >= pIns->nLoopEnd) {
	pIns->nLoopStart = pIns->nLoopEnd = 0;
	pIns->uFlags &= ~CHN_LOOP;
    }
    len = pIns->nLength;

    if (pIns->uFlags & CHN_16BIT) {
	int16_t *pSample = (int16_t *) pIns->pSample;

	// Adjust end of sample
	if (pIns->uFlags & CHN_STEREO) {
	    pSample[len * 2 + 6] = pSample[len * 2 + 4] =
		pSample[len * 2 + 2] = pSample[len * 2] = pSample[len * 2 - 2];
	    pSample[len * 2 + 7] = pSample[len * 2 + 5] =
		pSample[len * 2 + 3] = pSample[len * 2 + 1] =
		pSample[len * 2 - 1];
	} else {
	    pSample[len + 4] = pSample[len + 3] = pSample[len + 2] =
		pSample[len + 1] = pSample[len] = pSample[len - 1];
	}
	if ((pIns->uFlags & (CHN_LOOP | CHN_PINGPONGLOOP | CHN_STEREO)) ==
	    CHN_LOOP) {
	    // Fix bad loops
	    if ((pIns->nLoopEnd + 3 >= pIns->nLength)
		|| (that->m_nType & MOD_TYPE_S3M)) {
		pSample[pIns->nLoopEnd] = pSample[pIns->nLoopStart];
		pSample[pIns->nLoopEnd + 1] = pSample[pIns->nLoopStart + 1];
		pSample[pIns->nLoopEnd + 2] = pSample[pIns->nLoopStart + 2];
		pSample[pIns->nLoopEnd + 3] = pSample[pIns->nLoopStart + 3];
		pSample[pIns->nLoopEnd + 4] = pSample[pIns->nLoopStart + 4];
	    }
	}
    } else {
	int8_t *pSample = pIns->pSample;

#ifndef MODPLUG_FASTSOUNDLIB
	// Crappy samples (except chiptunes) ?
	if ((pIns->nLength > 0x100)
	    && (that->m_nType & (MOD_TYPE_MOD | MOD_TYPE_S3M))
	    && (!(pIns->uFlags & CHN_STEREO))) {
	    int delta;
	    int smpend = pSample[pIns->nLength - 1], smpfix = 0, kscan;

	    for (kscan = pIns->nLength - 1; kscan > 0; kscan--) {
		smpfix = pSample[kscan - 1];
		if (smpfix != smpend)
		    break;
	    }
	    delta = smpfix - smpend;

	    if (((!(pIns->uFlags & CHN_LOOP)) || (kscan > (int)pIns->nLoopEnd))
		&& ((delta < -8) || (delta > 8))) {
		while (kscan < (int)pIns->nLength) {
		    if (!(kscan & 7)) {
			if (smpfix > 0)
			    smpfix--;
			if (smpfix < 0)
			    smpfix++;
		    }
		    pSample[kscan] = (int8_t) smpfix;
		    kscan++;
		}
	    }
	}
#endif
	// Adjust end of sample
	if (pIns->uFlags & CHN_STEREO) {
	    pSample[len * 2 + 6] = pSample[len * 2 + 4] =
		pSample[len * 2 + 2] = pSample[len * 2] = pSample[len * 2 - 2];
	    pSample[len * 2 + 7] = pSample[len * 2 + 5] =
		pSample[len * 2 + 3] = pSample[len * 2 + 1] =
		pSample[len * 2 - 1];
	} else {
	    pSample[len + 4] = pSample[len + 3] = pSample[len + 2] =
		pSample[len + 1] = pSample[len] = pSample[len - 1];
	}
	if ((pIns->uFlags & (CHN_LOOP | CHN_PINGPONGLOOP | CHN_STEREO)) ==
	    CHN_LOOP) {
	    if ((pIns->nLoopEnd + 3 >= pIns->nLength)
		|| (that->m_nType & (MOD_TYPE_MOD | MOD_TYPE_S3M))) {
		pSample[pIns->nLoopEnd] = pSample[pIns->nLoopStart];
		pSample[pIns->nLoopEnd + 1] = pSample[pIns->nLoopStart + 1];
		pSample[pIns->nLoopEnd + 2] = pSample[pIns->nLoopStart + 2];
		pSample[pIns->nLoopEnd + 3] = pSample[pIns->nLoopStart + 3];
		pSample[pIns->nLoopEnd + 4] = pSample[pIns->nLoopStart + 4];
	    }
	}
    }
}

/////////////////////////////////////////////////////////////
// Transpose <-> Frequency conversions

// returns 8363*2^((transp*128+ftune)/(12*128))
uint32_t CSoundFile_TransposeToFrequency(int transp, int ftune)
//-----------------------------------------------------------
{
    //---GCCFIX:  Removed assembly.
    return (uint32_t) (8363 * pow(2, (transp * 128 + ftune) / (1536)));

//#ifdef _MSC_VER
#if 0
    const float _fbase = 8363;
    const float _factor = 1.0f / (12.0f * 128.0f);
    int result;
    uint32_t freq;

    transp = (transp << 7) + ftune;
    _asm {
	fild transp fld _factor fmulp st(1), st(0)
	fist result fisub result f2xm1 fild result fld _fbase fscale fstp st(1)
	fmul st(1), st(0)
	faddp st(1), st(0)
    fistp freq}
    unsigned derr = freq % 11025;

    if (derr <= 8)
	freq -= derr;
    if (derr >= 11015)
	freq += 11025 - derr;
    derr = freq % 1000;
    if (derr <= 5)
	freq -= derr;
    if (derr >= 995)
	freq += 1000 - derr;
    return freq;
#endif
}

// returns 12*128*log2(freq/8363)
int CSoundFile_FrequencyToTranspose(uint32_t freq)
//----------------------------------------------
{
#ifdef _MSC_VER
    const float _f1_8363 = 1.0f / 8363.0f;
    const float _factor = 128 * 12;
    long result;

    if (!freq)
	return 0;
    __asm {
	fld _factor
	fild freq
	fld _f1_8363
	fmulp st(1),st(0)
	fyl2x
	fistp result}
    return result;
#else

    //---GCCFIX:  Removed assembly.
    return 1536 * (log(freq / 8363) / log(2));
#endif
}

void CSoundFile_FrequencyToTranspose_Instrument(MODINSTRUMENT * psmp)
//--------------------------------------------------------
{
    int f2t = CSoundFile_FrequencyToTranspose(psmp->nC4Speed);
    int transp = f2t >> 7;
    int ftune = f2t & 0x7F;

    if (ftune > 80) {
	transp++;
	ftune -= 128;
    }
    if (transp > 127)
	transp = 127;
    if (transp < -127)
	transp = -127;
    psmp->RelativeTone = transp;
    psmp->nFineTune = ftune;
}

void CSoundFile_CheckCPUUsage(CSoundFile * that, unsigned nCPU)
//---------------------------------------
{
    if (nCPU > 100) {
	nCPU = 100;
    }
    CSoundFile_gnCPUUsage = nCPU;
    if (nCPU < 90) {
	that->m_dwSongFlags &= ~SONG_CPUVERYHIGH;
    } else if ((that->m_dwSongFlags & SONG_CPUVERYHIGH) && (nCPU >= 94)) {
	unsigned i = MAX_CHANNELS;

	while (i >= 8) {
	    i--;
	    if (that->Chn[i].nLength) {
		that->Chn[i].nLength = that->Chn[i].nPos = 0;
		nCPU -= 2;
		if (nCPU < 94)
		    break;
	    }
	}
    } else if (nCPU > 90) {
	that->m_dwSongFlags |= SONG_CPUVERYHIGH;
    }
}

int CSoundFile_SetPatternName(CSoundFile * that, unsigned nPat,
    const char *lpszName)
//---------------------------------------------------------
{
    char szName[MAX_PATTERNNAME] = "";	// changed from CHAR

    if (nPat >= MAX_PATTERNS)
	return 0;
    if (lpszName)
	strncpy(szName, lpszName, MAX_PATTERNNAME);
    szName[MAX_PATTERNNAME - 1] = 0;
    if (!that->m_lpszPatternNames)
	that->m_nPatternNames = 0;
    if (nPat >= that->m_nPatternNames) {
	unsigned len;
	char *p;

	if (!lpszName[0]) {
	    return 1;
	}

	len = (nPat + 1) * MAX_PATTERNNAME;
	p = malloc(len);		// FIXME: use calloc
	if (!p) {
	    return 0;
	}
	memset(p, 0, len);

	if (that->m_lpszPatternNames) {
	    memcpy(p, that->m_lpszPatternNames,
		that->m_nPatternNames * MAX_PATTERNNAME);
	    free(that->m_lpszPatternNames);

	    that->m_lpszPatternNames = NULL;
	}
	that->m_lpszPatternNames = p;
	that->m_nPatternNames = nPat + 1;
    }
    memcpy(that->m_lpszPatternNames + nPat * MAX_PATTERNNAME, szName,
	MAX_PATTERNNAME);
    return 1;
}

int CSoundFile_GetPatternName(CSoundFile const *that, unsigned nPat,
    char *lpszName, unsigned cbSize)
//---------------------------------------------------------------------------
{
    if ((!lpszName) || (!cbSize)) {
	return 0;
    }
    lpszName[0] = 0;
    if (cbSize > MAX_PATTERNNAME) {
	cbSize = MAX_PATTERNNAME;
    }
    if ((that->m_lpszPatternNames) && (nPat < that->m_nPatternNames)) {
	memcpy(lpszName, that->m_lpszPatternNames + nPat * MAX_PATTERNNAME,
	    cbSize);
	lpszName[cbSize - 1] = 0;
	return 1;
    }
    return 0;
}

#ifndef MODPLUG_FASTSOUNDLIB

unsigned CSoundFile_DetectUnusedSamples(CSoundFile * that, int *pbIns)
//-----------------------------------------------
{
    unsigned nExt;
    unsigned j;
    unsigned ipat;

    if (!pbIns || !that->m_nInstruments ) {
	return 0;
    }
    memset(pbIns, 0, MAX_SAMPLES * sizeof(int));
    for (ipat = 0; ipat < MAX_PATTERNS; ipat++) {
	MODCOMMAND *p = that->Patterns[ipat];

	if (p) {
	    unsigned jmax = that->PatternSize[ipat] * that->m_nChannels;

	    for (j = 0; j < jmax; j++, p++) {
		if ((p->note) && (p->note <= 120)) {
		    if ((p->instr) && (p->instr < MAX_INSTRUMENTS)) {
			INSTRUMENTHEADER *penv = that->Headers[p->instr];

			if (penv) {
			    unsigned n = penv->Keyboard[p->note - 1];

			    if (n < MAX_SAMPLES) {
				pbIns[n] = 1;
			    }
			}
		    } else {
			unsigned k;

			for (k = 1; k <= that->m_nInstruments; k++) {
			    INSTRUMENTHEADER *penv = that->Headers[k];

			    if (penv) {
				unsigned n = penv->Keyboard[p->note - 1];

				if (n < MAX_SAMPLES) {
				    pbIns[n] = 1;
				}
			    }
			}
		    }
		}
	    }
	}
    }
    nExt = 0;
    for (j = 1; j <= that->m_nSamples; j++) {
	if (!pbIns[j] && that->Ins[j].pSample) {
	    nExt++;
	}
    }
    return nExt;
}

int CSoundFile_RemoveSelectedSamples(CSoundFile * that, int *pbIns)
//-------------------------------------------------
{
    unsigned j;

    if (!pbIns) {
	return 0;
    }
    for (j = 1; j < MAX_SAMPLES; j++) {
	if ((!pbIns[j]) && (that->Ins[j].pSample)) {
	    CSoundFile_DestroySample(that,j);
	    if ((j == that->m_nSamples) && (j > 1)) {
		that->m_nSamples--;
	    }
	}
    }
    return 1;
}

int CSoundFile_DestroySample(CSoundFile * that, unsigned nSample)
//------------------------------------------
{
    MODINSTRUMENT *pins;
    int8_t *pSample;
    unsigned i;

    if ((!nSample) || (nSample >= MAX_SAMPLES)) {
	return 0;
    }

    pins = &that->Ins[nSample];
    pSample = pins->pSample;
    if (!pSample) {
	return 1;
    }

    pins->pSample = NULL;
    pins->nLength = 0;
    pins->uFlags &= ~(CHN_16BIT);

    for (i = 0; i < MAX_CHANNELS; i++) {
	if (that->Chn[i].pSample == pSample) {
	    that->Chn[i].nPos = that->Chn[i].nLength = 0;
	    that->Chn[i].pSample = that->Chn[i].pCurrentSample = NULL;
	}
    }

    CSoundFile_FreeSample(pSample);
    return 1;
}

#endif // MODPLUG_FASTSOUNDLIB
