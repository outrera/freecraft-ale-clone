/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: sndmix.c,v 1.3 2002/04/13 19:27:18 jsalmon3 Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

#ifdef MODPLUG_TRACKER
#define ENABLE_STEREOVU
#endif

// Volume ramp length, in 1/10 ms
#define VOLUMERAMPLEN	146		// 1.46ms = 64 samples at 44.1kHz

// VU-Meter
#define VUMETER_DECAY	    4

typedef uint32_t (MPPASMCALL * LPCONVERTPROC) (void *, const int *, uint32_t,
    long *, long *);

// SNDMIX: These are global flags for playback control
unsigned CSoundFile_StereoSeparation = 128;
long CSoundFile_StreamVolume = 0x8000;
unsigned CSoundFile_MaxMixChannels = 32;

// Mixing Configuration (SetWaveConfig)
uint32_t CSoundFile_gdwSysInfo;
uint32_t CSoundFile_gnChannels = 1;
uint32_t CSoundFile_gdwSoundSetup;
uint32_t CSoundFile_gdwMixingFreq = 44100;
uint32_t CSoundFile_gnBitsPerSample = 16;

// Mixing data initialized in
#ifndef MODPLUG_NO_AGC
unsigned CSoundFile_gnAGC = AGC_UNITY;
#endif
unsigned CSoundFile_gnVolumeRampSamples = 64;
unsigned CSoundFile_gnVUMeter;
unsigned CSoundFile_gnCPUUsage;
void (*CSoundFile_gpSndMixHook) (int *buffer, unsigned long samples,
    unsigned long channels) = NULL;
int (*CSoundFile_gpMixPluginCreateProc) (SNDMIXPLUGIN *) = NULL;
long CSoundFile_gnDryROfsVol;
long CSoundFile_gnDryLOfsVol;
long CSoundFile_gnRvbROfsVol;
long CSoundFile_gnRvbLOfsVol;
static int gbInitPlugins;

#ifndef MODPLUG_NO_REVERB
unsigned CSoundFile_gnReverbSend;
#endif

// Log tables for pre-amp
// We don't want the tracker to get too loud
static const unsigned PreAmpTable[16] = {
    0x60, 0x60, 0x60, 0x70,		// 0-7
    0x80, 0x88, 0x90, 0x98,		// 8-15
    0xA0, 0xA4, 0xA8, 0xB0,		// 16-23
    0xB4, 0xB8, 0xBC, 0xC0,		// 24-31
};

static const unsigned PreAmpAGCTable[16] = {
    0x60, 0x60, 0x60, 0x60,
    0x68, 0x70, 0x78, 0x80,
    0x84, 0x88, 0x8C, 0x90,
    0x94, 0x98, 0x9C, 0xA0,
};

// Return (a*b)/c - no divide error
int _muldiv(long a, long b, long c)
{
#ifdef _MSC_VER
    int sign, result;

    _asm {
	mov eax, a
	mov ebx, b
	or eax, eax
	mov edx, eax
	jge aneg
	neg eax
aneg:	xor edx, ebx
	or ebx, ebx
	mov ecx, c
	jge bneg
	neg ebx
bneg:	xor edx, ecx
	or ecx, ecx
	mov sign, edx
	jge cneg
	neg ecx
cneg:	mul ebx
	cmp edx, ecx
	jae diverr
	div ecx
	jmp ok
diverr:	mov eax, 0x7fffffff
ok:	mov edx, sign
	or edx, edx
	jge rneg
	neg eax
rneg:	mov result, eax}
    return result;
#else
    return ((unsigned long long)a * (unsigned long long)b) / c;
#endif
}

// Return (a*b+c/2)/c - no divide error
int _muldivr(long a, long b, long c)
{
#ifdef _MSC_VER
    int sign, result;

    _asm {
	mov eax, a
	mov ebx, b
	or eax, eax
	mov edx, eax
	jge aneg
	neg eax
aneg:	xor edx, ebx
	or ebx, ebx
	mov ecx, c
	jge bneg
	neg ebx
bneg:	xor edx, ecx
	or ecx, ecx
	mov sign, edx
	jge cneg
	neg ecx
cneg:	mul ebx
	mov ebx, ecx
	shr ebx, 1
	add eax, ebx
	adc edx, 0
	cmp edx, ecx
	jae diverr
	div ecx
	jmp ok
diverr:	mov eax, 0x7fffffff
ok:	mov edx, sign
	or edx, edx
	jge rneg
	neg eax
rneg:	mov result, eax}
    return result;
#else
    return ((unsigned long long)a * (unsigned long long)b + (c >> 1)) / c;
#endif
}

int CSoundFile_InitPlayer(int bReset)
//--------------------------------------
{
    if (CSoundFile_MaxMixChannels > MAX_CHANNELS) {
	CSoundFile_MaxMixChannels = MAX_CHANNELS;
    }
    if (CSoundFile_gdwMixingFreq < 4000) {
	CSoundFile_gdwMixingFreq = 4000;
    }
    if (CSoundFile_gdwMixingFreq > MAX_SAMPLE_RATE) {
	CSoundFile_gdwMixingFreq = MAX_SAMPLE_RATE;
    }
    CSoundFile_gnVolumeRampSamples =
	(CSoundFile_gdwMixingFreq * VOLUMERAMPLEN) / 100000;
    if (CSoundFile_gnVolumeRampSamples < 8) {
	CSoundFile_gnVolumeRampSamples = 8;
    }
    CSoundFile_gnDryROfsVol = CSoundFile_gnDryLOfsVol = 0;
    CSoundFile_gnRvbROfsVol = CSoundFile_gnRvbLOfsVol = 0;
    if (bReset) {
	CSoundFile_gnVUMeter = 0;
	CSoundFile_gnCPUUsage = 0;
    }
    gbInitPlugins = (bReset) ? 3 : 1;

    CSoundFile_InitializeDSP(bReset);
    return 1;
}

int CSoundFile_FadeSong(CSoundFile * that, unsigned msec)
//----------------------------------
{
    long nsamples = _muldiv(msec, CSoundFile_gdwMixingFreq, 1000);
    long nRampLength;
    unsigned noff;

    if (nsamples <= 0) {
	return 0;
    }
    if (nsamples > 0x100000) {
	nsamples = 0x100000;
    }
    that->m_nBufferCount = nsamples;
    nRampLength = that->m_nBufferCount;

    // Ramp everything down
    for (noff = 0; noff < that->m_nMixChannels; noff++) {
	MODCHANNEL *pramp = &that->Chn[that->ChnMix[noff]];

	if (!pramp) {
	    continue;
	}
	pramp->nNewLeftVol = pramp->nNewRightVol = 0;
	pramp->nRightRamp =
	    (-pramp->nRightVol << VOLUMERAMPPRECISION) / nRampLength;
	pramp->nLeftRamp =
	    (-pramp->nLeftVol << VOLUMERAMPPRECISION) / nRampLength;
	pramp->nRampRightVol = pramp->nRightVol << VOLUMERAMPPRECISION;
	pramp->nRampLeftVol = pramp->nLeftVol << VOLUMERAMPPRECISION;
	pramp->nRampLength = nRampLength;
	pramp->dwFlags |= CHN_VOLUMERAMP;
    }
    that->m_dwSongFlags |= SONG_FADINGSONG;
    return 1;
}

int CSoundFile_GlobalFadeSong(CSoundFile * that, unsigned msec)
//----------------------------------------
{
    if (that->m_dwSongFlags & SONG_GLOBALFADE) {
	return 0;
    }
    that->m_nGlobalFadeMaxSamples = _muldiv(msec, CSoundFile_gdwMixingFreq, 1000);
    that->m_nGlobalFadeSamples = that->m_nGlobalFadeMaxSamples;
    that->m_dwSongFlags |= SONG_GLOBALFADE;
    return 1;
}

unsigned CSoundFile_Read(CSoundFile * that, void *lpDestBuffer,
    unsigned cbBuffer)
//-------------------------------------------------------
{
    uint8_t *lpBuffer = (uint8_t *) lpDestBuffer;
    LPCONVERTPROC pCvt;
    unsigned lRead;
    unsigned lMax;
    unsigned lSampleSize;
    unsigned lCount;
    unsigned lSampleCount;
    unsigned nStat = 0;
    long nVUMeterMin = 0x7FFFFFFF;
    long nVUMeterMax = -0x7FFFFFFF;
    unsigned max_plugins;
    unsigned lTotalSampleCount;

#ifndef MODPLUG_NO_IMIXPLUGIN
    max_plugins = MAX_MIXPLUGINS;
    while ((max_plugins > 0)
	&& (!that->m_MixPlugins[max_plugins - 1].pMixPlugin)) {
	max_plugins--;
    }
#else
    max_plugins = 0;
#endif

    that->m_nMixStat = 0;
    lSampleSize = CSoundFile_gnChannels;
    if (CSoundFile_gnBitsPerSample == 16) {
	lSampleSize *= 2;
	pCvt = X86_Convert32To16;
#ifndef MODPLUG_FASTSOUNDLIB
    } else if (CSoundFile_gnBitsPerSample == 24) {
	lSampleSize *= 3;
	pCvt = X86_Convert32To24;
    } else if (CSoundFile_gnBitsPerSample == 32) {
	lSampleSize *= 4;
	pCvt = X86_Convert32To32;
#endif
    } else {
	pCvt = X86_Convert32To8;
    }

    lMax = cbBuffer / lSampleSize;
    if ((!lMax) || (!lpBuffer) || (!that->m_nChannels)) {
	return 0;
    }
    lRead = lMax;
    if (that->m_dwSongFlags & SONG_ENDREACHED) {
	goto MixDone;
    }
    while (lRead > 0) {
	// Update Channel Data
	if (!that->m_nBufferCount) {
#ifndef MODPLUG_FASTSOUNDLIB
	    if (that->m_dwSongFlags & SONG_FADINGSONG) {
		that->m_dwSongFlags |= SONG_ENDREACHED;
		that->m_nBufferCount = lRead;
	    } else
#endif
	    if (!CSoundFile_ReadNote(that)) {
#ifndef MODPLUG_FASTSOUNDLIB
		if (!CSoundFile_FadeSong(that, FADESONGDELAY))
#endif
		{
		    that->m_dwSongFlags |= SONG_ENDREACHED;
		    if (lRead == lMax)
			goto MixDone;
		    that->m_nBufferCount = lRead;
		}
	    }
	}
	lCount = that->m_nBufferCount;
	if (lCount > MIXBUFFERSIZE)
	    lCount = MIXBUFFERSIZE;
	if (lCount > lRead)
	    lCount = lRead;
	if (!lCount)
	    break;
	lSampleCount = lCount;
#ifndef MODPLUG_NO_REVERB
	CSoundFile_gnReverbSend = 0;
#endif
	// Resetting sound buffer
	X86_StereoFill(CSoundFile_MixSoundBuffer, lSampleCount, &CSoundFile_gnDryROfsVol,
	    &CSoundFile_gnDryLOfsVol);
	if (CSoundFile_gnChannels >= 2) {
	    lSampleCount *= 2;
	    that->m_nMixStat += CSoundFile_CreateStereoMix(that, lCount);
	    CSoundFile_ProcessStereoDSP(lCount);
	} else {
	    that->m_nMixStat += CSoundFile_CreateStereoMix(that, lCount);
	    if (max_plugins)
		ProcessPlugins(lCount);
	    CSoundFile_ProcessStereoDSP(lCount);
	    X86_MonoFromStereo(CSoundFile_MixSoundBuffer, lCount);
	}
	nStat++;
#ifndef MODPLUG_NO_AGC
	// Automatic Gain Control
	if (CSoundFile_gdwSoundSetup & SNDMIX_AGC) {
	    CSoundFile_ProcessAGC(lSampleCount);
	}
#endif
	lTotalSampleCount = lSampleCount;

#ifndef MODPLUG_FASTSOUNDLIB
	// Multichannel
	if (CSoundFile_gnChannels > 2) {
	    X86_InterleaveFrontRear(CSoundFile_MixSoundBuffer,
		CSoundFile_MixRearBuffer, lSampleCount);
	    lTotalSampleCount *= 2;
	}
	// Hook Function
	if (CSoundFile_gpSndMixHook) {
	    CSoundFile_gpSndMixHook(CSoundFile_MixSoundBuffer,
		lTotalSampleCount, CSoundFile_gnChannels);
	}
#endif
	// Perform clipping + VU-Meter
	lpBuffer +=
	    pCvt(lpBuffer, CSoundFile_MixSoundBuffer, lTotalSampleCount, &nVUMeterMin,
	    &nVUMeterMax);
	// Buffer ready
	lRead -= lCount;
	that->m_nBufferCount -= lCount;
    }
MixDone:
    if (lRead) {
	memset(lpBuffer, (CSoundFile_gnBitsPerSample == 8) ? 0x80 : 0,
	    lRead * lSampleSize);
    }
    // VU-Meter
    nVUMeterMin >>= (24 - MIXING_ATTENUATION);
    nVUMeterMax >>= (24 - MIXING_ATTENUATION);
    if (nVUMeterMax < nVUMeterMin) {
	nVUMeterMax = nVUMeterMin;
    }
    if ((CSoundFile_gnVUMeter = (unsigned)(nVUMeterMax - nVUMeterMin)) > 0xFF) {
	CSoundFile_gnVUMeter = 0xFF;
    }
    if (nStat) {
	that->m_nMixStat += nStat - 1;
	that->m_nMixStat /= nStat;
    }
    return lMax - lRead;
}

/////////////////////////////////////////////////////////////////////////////
// Handles navigation/effects

int CSoundFile_ProcessRow(CSoundFile * that)
//---------------------------
{
    if (++that->m_nTickCount >=
	    that->m_nMusicSpeed * (that->m_nPatternDelay + 1) +
	    that->m_nFrameDelay) {
	MODCHANNEL *pChn;
	MODCOMMAND *m;
	unsigned nChn;

	that->m_nPatternDelay = 0;
	that->m_nFrameDelay = 0;
	that->m_nTickCount = 0;
	that->m_nRow = that->m_nNextRow;
	// Reset Pattern Loop Effect
	if (that->m_nCurrentPattern != that->m_nNextPattern)
	    that->m_nCurrentPattern = that->m_nNextPattern;
	// Check if pattern is valid
	if (!(that->m_dwSongFlags & SONG_PATTERNLOOP)) {
	    that->m_nPattern =
		(that->m_nCurrentPattern <
		MAX_ORDERS) ? that->Order[that->m_nCurrentPattern] : 0xFF;
	    if ((that->m_nPattern < MAX_PATTERNS)
		&& (!that->Patterns[that->m_nPattern]))
		that->m_nPattern = 0xFE;
	    while (that->m_nPattern >= MAX_PATTERNS) {
		// End of song ?
		if ((that->m_nPattern == 0xFF)
		    || (that->m_nCurrentPattern >= MAX_ORDERS)) {
		    //if (!that->m_nRepeatCount)
		    return 0;		//never repeat entire song
		    if (!that->m_nRestartPos) {
			unsigned i;

			that->m_nMusicSpeed = that->m_nDefaultSpeed;
			that->m_nMusicTempo = that->m_nDefaultTempo;
			that->m_nGlobalVolume = that->m_nDefaultGlobalVolume;
			for (i = 0; i < MAX_CHANNELS; i++) {
			    that->Chn[i].dwFlags |= CHN_NOTEFADE | CHN_KEYOFF;
			    that->Chn[i].nFadeOutVol = 0;
			    if (i < that->m_nChannels) {
				that->Chn[i].nGlobalVol =
				    that->ChnSettings[i].nVolume;
				that->Chn[i].nVolume =
				    that->ChnSettings[i].nVolume;
				that->Chn[i].nPan = that->ChnSettings[i].nPan;
				that->Chn[i].nPanSwing =
				    that->Chn[i].nVolSwing = 0;
				that->Chn[i].nOldVolParam = 0;
				that->Chn[i].nOldOffset = 0;
				that->Chn[i].nOldHiOffset = 0;
				that->Chn[i].nPortamentoDest = 0;
				if (!that->Chn[i].nLength) {
				    that->Chn[i].dwFlags =
					that->ChnSettings[i].dwFlags;
				    that->Chn[i].nLoopStart = 0;
				    that->Chn[i].nLoopEnd = 0;
				    that->Chn[i].pHeader = NULL;
				    that->Chn[i].pSample = NULL;
				    that->Chn[i].pInstrument = NULL;
				}
			    }
			}
		    }
//                  if (that->m_nRepeatCount > 0) that->m_nRepeatCount--;
		    that->m_nCurrentPattern = that->m_nRestartPos;
		    that->m_nRow = 0;
		    if ((that->Order[that->m_nCurrentPattern] >= MAX_PATTERNS)
			|| (!that->Patterns[that->Order[that->
				    m_nCurrentPattern]])) {
			return 0;
		    }
		} else {
		    that->m_nCurrentPattern++;
		}
		that->m_nPattern =
		    (that->m_nCurrentPattern <
		    MAX_ORDERS) ? that->Order[that->m_nCurrentPattern] : 0xFF;
		if ((that->m_nPattern < MAX_PATTERNS)
		    && (!that->Patterns[that->m_nPattern]))
		    that->m_nPattern = 0xFE;
	    }
	    that->m_nNextPattern = that->m_nCurrentPattern;
	}
	// Weird stuff?
	if ((that->m_nPattern >= MAX_PATTERNS)
	    || (!that->Patterns[that->m_nPattern]))
	    return 0;
	// Should never happen
	if (that->m_nRow >= that->PatternSize[that->m_nPattern])
	    that->m_nRow = 0;
	that->m_nNextRow = that->m_nRow + 1;
	if (that->m_nNextRow >= that->PatternSize[that->m_nPattern]) {
	    if (!(that->m_dwSongFlags & SONG_PATTERNLOOP))
		that->m_nNextPattern = that->m_nCurrentPattern + 1;
	    that->m_nNextRow = 0;
	}
	// Reset channel values
	pChn = that->Chn;
	m = that->Patterns[that->m_nPattern] +
	    that->m_nRow * that->m_nChannels;
	for (nChn = 0; nChn < that->m_nChannels; pChn++, nChn++, m++) {
	    pChn->nRowNote = m->note;
	    pChn->nRowInstr = m->instr;
	    pChn->nRowVolCmd = m->volcmd;
	    pChn->nRowVolume = m->vol;
	    pChn->nRowCommand = m->command;
	    pChn->nRowParam = m->param;

	    pChn->nLeftVol = pChn->nNewLeftVol;
	    pChn->nRightVol = pChn->nNewRightVol;
	    pChn->dwFlags &=
		~(CHN_PORTAMENTO | CHN_VIBRATO | CHN_TREMOLO | CHN_PANBRELLO);
	    pChn->nCommand = 0;
	}
    }
    // Should we process tick0 effects?
    if (!that->m_nMusicSpeed) {
	that->m_nMusicSpeed = 1;
    }
    that->m_dwSongFlags |= SONG_FIRSTTICK;
    if (that->m_nTickCount) {
	that->m_dwSongFlags &= ~SONG_FIRSTTICK;
	if ((!(that->m_nType & MOD_TYPE_XM))
	    && (that->m_nTickCount <
		that->m_nMusicSpeed * (1 + that->m_nPatternDelay))) {
	    if (!(that->m_nTickCount % that->m_nMusicSpeed))
		that->m_dwSongFlags |= SONG_FIRSTTICK;
	}

    }

    // Update Effects
    return CSoundFile_ProcessEffects(that);
}

///////////////////////////////////////////////////////////////////////////////
// Handles envelopes & mixer setup

int CSoundFile_ReadNote(CSoundFile * that)
//-------------------------
{
    uint32_t nMasterVol;
    MODCHANNEL *pChn;
    int nchn32;
    unsigned attenuation;
    uint32_t mastervol;
    uint32_t realmastervol;
    unsigned nChn;

    if (!CSoundFile_ProcessRow(that)) {
	return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    that->m_nTotalCount++;
    if (!that->m_nMusicTempo) {
	return 0;
    }
    that->m_nBufferCount =
	(CSoundFile_gdwMixingFreq * 5 * that->m_nTempoFactor) /
	(that->m_nMusicTempo << 8);
    // Master Volume + Pre-Amplification / Attenuation setup

    nchn32 = (that->m_nChannels < 32) ? that->m_nChannels : 31;

    if ((that->m_nType & MOD_TYPE_IT) && (that->m_nInstruments)
	&& (nchn32 < 6))
	nchn32 = 6; {
    }
    realmastervol = that->m_nMasterVolume;

    if (realmastervol > 0x80) {
	realmastervol =
	    0x80 + ((realmastervol - 0x80) * (nchn32 + 4)) / 16;
    }
    attenuation = (CSoundFile_gdwSoundSetup & SNDMIX_AGC)
	    ? PreAmpAGCTable[nchn32 >> 1] : PreAmpTable[nchn32 >> 1];
    mastervol = (realmastervol * (that->m_nSongPreAmp + 0x10)) >> 6;
    if (mastervol > 0x200) {
	mastervol = 0x200;
    }
    if ((that->m_dwSongFlags & SONG_GLOBALFADE)
	&& (that->m_nGlobalFadeMaxSamples)) {
	mastervol = _muldiv(mastervol, that->m_nGlobalFadeSamples,
	    that->m_nGlobalFadeMaxSamples);
    }
    nMasterVol = (mastervol << 7) / attenuation;
    if (nMasterVol > 0x180) {
	nMasterVol = 0x180;
    }

    //////////////////////////////////////////////////////////////////////////
    // Update channels data
    that->m_nMixChannels = 0;
    pChn = that->Chn;

    for (nChn = 0; nChn < MAX_CHANNELS; nChn++, pChn++) {
	if ((pChn->dwFlags & CHN_NOTEFADE)
	    && (!(pChn->nFadeOutVol | pChn->nRightVol | pChn->nLeftVol))) {
	    pChn->nLength = 0;
	    pChn->nROfs = pChn->nLOfs = 0;
	}
	// Check for unused channel
	if ((pChn->dwFlags & CHN_MUTE) || ((nChn >= that->m_nChannels)
		&& (!pChn->nLength))) {
	    pChn->nVUMeter = 0;
#ifdef ENABLE_STEREOVU
	    pChn->nLeftVU = pChn->nRightVU = 0;
#endif
	    continue;
	}
	// Reset channel data
	pChn->nInc = 0;
	pChn->nRealVolume = 0;
	pChn->nRealPan = pChn->nPan + pChn->nPanSwing;
	if (pChn->nRealPan < 0)
	    pChn->nRealPan = 0;
	if (pChn->nRealPan > 256)
	    pChn->nRealPan = 256;
	pChn->nRampLength = 0;

	// Calc Frequency
	if ((pChn->nPeriod) && (pChn->nLength)) {
	    int period;
	    int nPeriodFrac;
	    int vol = pChn->nVolume + pChn->nVolSwing;

	    if (vol < 0)
		vol = 0;
	    if (vol > 256)
		vol = 256;
	    // Tremolo
	    if (pChn->dwFlags & CHN_TREMOLO) {
		unsigned trempos = pChn->nTremoloPos & 0x3F;

		if (vol > 0) {
		    int tremattn = (that->m_nType & MOD_TYPE_XM) ? 5 : 6;

		    switch (pChn->nTremoloType & 0x03) {
			case 1:
			    vol +=
				(CSoundFile_ModRampDownTable[trempos] *
				(int)pChn->nTremoloDepth) >> tremattn;
			    break;
			case 2:
			    vol +=
				(CSoundFile_ModSquareTable[trempos] *
				(int)pChn->nTremoloDepth) >> tremattn;
			    break;
			case 3:
			    vol +=
				(CSoundFile_ModRandomTable[trempos] *
				(int)pChn->nTremoloDepth) >> tremattn;
			    break;
			default:
			    vol +=
				(CSoundFile_ModSinusTable[trempos] *
				(int)pChn->nTremoloDepth) >> tremattn;
		    }
		}
		if ((that->m_nTickCount)
		    || ((that->
			    m_nType & (MOD_TYPE_STM | MOD_TYPE_S3M |
				MOD_TYPE_IT))
			&& (!(that->m_dwSongFlags & SONG_ITOLDEFFECTS)))) {
		    pChn->nTremoloPos = (trempos + pChn->nTremoloSpeed) & 0x3F;
		}
	    }
	    // Tremor
	    if (pChn->nCommand == CMD_TREMOR) {
		unsigned tremcount;
		unsigned ontime = pChn->nTremorParam >> 4;
		unsigned n =
		    (pChn->nTremorParam >> 4) + (pChn->nTremorParam & 0x0F);

		if ((!(that->m_nType & MOD_TYPE_IT))
		    || (that->m_dwSongFlags & SONG_ITOLDEFFECTS)) {
		    n += 2;
		    ontime++;
		}
		tremcount = (unsigned)pChn->nTremorCount;

		if (tremcount >= n)
		    tremcount = 0;
		if ((that->m_nTickCount)
		    || (that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT))) {
		    if (tremcount >= ontime)
			vol = 0;
		    pChn->nTremorCount = (uint8_t) (tremcount + 1);
		}
		pChn->dwFlags |= CHN_FASTVOLRAMP;
	    }
	    // Clip volume
	    if (vol < 0)
		vol = 0;
	    if (vol > 0x100)
		vol = 0x100;
	    vol <<= 6;
	    // Process Envelopes
	    if (pChn->pHeader) {
		INSTRUMENTHEADER *penv = pChn->pHeader;
		unsigned i;

		// Volume Envelope
		if ((pChn->dwFlags & CHN_VOLENV) && (penv->nVolEnv)) {
		    int envpos = pChn->nVolEnvPosition;
		    unsigned pt = penv->nVolEnv - 1;
		    int x2;
		    int x1, envvol;

		    for (i = 0; i < (unsigned)(penv->nVolEnv - 1);
			i++) {
			if (envpos <= penv->VolPoints[i]) {
			    pt = i;
			    break;
			}
		    }
		    x2 = penv->VolPoints[pt];

		    if (envpos >= x2) {
			envvol = penv->VolEnv[pt] << 2;
			x1 = x2;
		    } else if (pt) {
			envvol = penv->VolEnv[pt - 1] << 2;
			x1 = penv->VolPoints[pt - 1];
		    } else {
			envvol = 0;
			x1 = 0;
		    }
		    if (envpos > x2)
			envpos = x2;
		    if ((x2 > x1) && (envpos > x1)) {
			envvol +=
			    ((envpos - x1) * (((int)penv->VolEnv[pt] << 2) -
				envvol)) / (x2 - x1);
		    }
		    if (envvol < 0)
			envvol = 0;
		    if (envvol > 256)
			envvol = 256;
		    vol = (vol * envvol) >> 8;
		}
		// Panning Envelope
		if ((pChn->dwFlags & CHN_PANENV) && (penv->nPanEnv)) {
		    int envpos = pChn->nPanEnvPosition;
		    unsigned pt = penv->nPanEnv - 1;
		    int x2, y2;
		    int x1, envpan;
		    int pan;

		    for (i = 0; i < (unsigned)(penv->nPanEnv - 1);
			i++) {
			if (envpos <= penv->PanPoints[i]) {
			    pt = i;
			    break;
			}
		    }
		    x2 = penv->PanPoints[pt];
		    y2 = penv->PanEnv[pt];

		    if (envpos >= x2) {
			envpan = y2;
			x1 = x2;
		    } else if (pt) {
			envpan = penv->PanEnv[pt - 1];
			x1 = penv->PanPoints[pt - 1];
		    } else {
			envpan = 128;
			x1 = 0;
		    }
		    if ((x2 > x1) && (envpos > x1)) {
			envpan += ((envpos - x1) * (y2 - envpan)) / (x2 - x1);
		    }
		    if (envpan < 0) {
			envpan = 0;
		    }
		    if (envpan > 64) {
			envpan = 64;
		    }
		    pan = pChn->nPan;

		    if (pan >= 128) {
			pan += ((envpan - 32) * (256 - pan)) / 32;
		    } else {
			pan += ((envpan - 32) * (pan)) / 32;
		    }
		    if (pan < 0)
			pan = 0;
		    if (pan > 256)
			pan = 256;
		    pChn->nRealPan = pan;
		}
		// FadeOut volume
		if (pChn->dwFlags & CHN_NOTEFADE) {
		    unsigned fadeout = penv->nFadeOut;

		    if (fadeout) {
			pChn->nFadeOutVol -= fadeout << 1;
			if (pChn->nFadeOutVol <= 0)
			    pChn->nFadeOutVol = 0;
			vol = (vol * pChn->nFadeOutVol) >> 16;
		    } else if (!pChn->nFadeOutVol) {
			vol = 0;
		    }
		}
		// Pitch/Pan separation
		if ((penv->nPPS) && (pChn->nRealPan) && (pChn->nNote)) {
		    int pandelta =
			(int)pChn->nRealPan + (int)((int)(pChn->nNote -
			    penv->nPPC - 1) * (int)penv->nPPS) / (int)8;
		    if (pandelta < 0)
			pandelta = 0;
		    if (pandelta > 256)
			pandelta = 256;
		    pChn->nRealPan = pandelta;
		}
	    } else {
		// No Envelope: key off => note cut
		if (pChn->dwFlags & CHN_NOTEFADE)	// 1.41-: CHN_KEYOFF|CHN_NOTEFADE
		{
		    pChn->nFadeOutVol = 0;
		    vol = 0;
		}
	    }
	    // vol is 14-bits
	    if (vol) {
		// IMPORTANT: pChn->nRealVolume is 14 bits !!!
		// -> _muldiv( 14+8, 6+6, 18); => RealVolume: 14-bit result (22+12-20)
		pChn->nRealVolume =
		    _muldiv(vol * that->m_nGlobalVolume,
		    pChn->nGlobalVol * pChn->nInsVol, 1 << 20);
	    }

	    if (pChn->nPeriod < that->m_nMinPeriod) {
		pChn->nPeriod = that->m_nMinPeriod;
	    }
	    period = pChn->nPeriod;

	    if ((pChn->dwFlags & (CHN_GLISSANDO | CHN_PORTAMENTO)) ==
		(CHN_GLISSANDO | CHN_PORTAMENTO)) {
		period =
		    CSoundFile_GetPeriodFromNote(that,
		    CSoundFile_GetNoteFromPeriod(that, period),
		    pChn->nFineTune, pChn->nC4Speed);
	    }
	    // Arpeggio ?
	    if (pChn->nCommand == CMD_ARPEGGIO) {
		switch (that->m_nTickCount % 3) {
		    case 1:
			period =
			    CSoundFile_GetPeriodFromNote(that,
			    pChn->nNote + (pChn->nArpeggio >> 4),
			    pChn->nFineTune, pChn->nC4Speed);
			break;
		    case 2:
			period =
			    CSoundFile_GetPeriodFromNote(that,
			    pChn->nNote + (pChn->nArpeggio & 0x0F),
			    pChn->nFineTune, pChn->nC4Speed);
			break;
		}
	    }

	    if (that->m_dwSongFlags & SONG_AMIGALIMITS) {
		if (period < 113 * 4)
		    period = 113 * 4;
		if (period > 856 * 4)
		    period = 856 * 4;
	    }
	    // Pitch/Filter Envelope
	    if ((pChn->pHeader) && (pChn->dwFlags & CHN_PITCHENV)
		&& (pChn->pHeader->nPitchEnv)) {
		INSTRUMENTHEADER *penv = pChn->pHeader;
		int envpos = pChn->nPitchEnvPosition;
		unsigned pt = penv->nPitchEnv - 1;
		int x2;
		int x1, envpitch;
		unsigned i;

		for (i = 0; i < (unsigned)(penv->nPitchEnv - 1); i++) {
		    if (envpos <= penv->PitchPoints[i]) {
			pt = i;
			break;
		    }
		}
		x2 = penv->PitchPoints[pt];

		if (envpos >= x2) {
		    envpitch = (((int)penv->PitchEnv[pt]) - 32) * 8;
		    x1 = x2;
		} else if (pt) {
		    envpitch = (((int)penv->PitchEnv[pt - 1]) - 32) * 8;
		    x1 = penv->PitchPoints[pt - 1];
		} else {
		    envpitch = 0;
		    x1 = 0;
		}
		if (envpos > x2)
		    envpos = x2;
		if ((x2 > x1) && (envpos > x1)) {
		    int envpitchdest = (((int)penv->PitchEnv[pt]) - 32) * 8;

		    envpitch +=
			((envpos - x1) * (envpitchdest - envpitch)) / (x2 -
			x1);
		}
		if (envpitch < -256)
		    envpitch = -256;
		if (envpitch > 256)
		    envpitch = 256;
		// Filter Envelope: controls cutoff frequency
		if (penv->dwFlags & ENV_FILTER) {
#ifndef MODPLUG_NO_FILTER
		    CSoundFile_SetupChannelFilter(that, pChn,
			(pChn->dwFlags & CHN_FILTER) ? 0 : 1, envpitch);
#endif // MODPLUG_NO_FILTER
		    // Pitch Envelope
		} else {
		    int l = envpitch;

		    if (l < 0) {
			l = -l;
			if (l > 255)
			    l = 255;
			period =
			    _muldiv(period, CSoundFile_LinearSlideUpTable[l],
			    0x10000);
		    } else {
			if (l > 255)
			    l = 255;
			period =
			    _muldiv(period, CSoundFile_LinearSlideDownTable[l],
			    0x10000);
		    }
		}
	    }
	    // Vibrato
	    if (pChn->dwFlags & CHN_VIBRATO) {
		unsigned vibpos = pChn->nVibratoPos;
		long vdelta;
		unsigned vdepth;

		switch (pChn->nVibratoType & 0x03) {
		    case 1:
			vdelta = CSoundFile_ModRampDownTable[vibpos];
			break;
		    case 2:
			vdelta = CSoundFile_ModSquareTable[vibpos];
			break;
		    case 3:
			vdelta = CSoundFile_ModRandomTable[vibpos];
			break;
		    default:
			vdelta = CSoundFile_ModSinusTable[vibpos];
		}
		vdepth = ((that->m_nType != MOD_TYPE_IT)
		    || (that->m_dwSongFlags & SONG_ITOLDEFFECTS)) ? 6 : 7;
		vdelta = (vdelta * (int)pChn->nVibratoDepth) >> vdepth;
		if ((that->m_dwSongFlags & SONG_LINEARSLIDES)
		    && (that->m_nType & MOD_TYPE_IT)) {
		    long l = vdelta;

		    if (l < 0) {
			l = -l;
			vdelta =
			    _muldiv(period,
			    CSoundFile_LinearSlideDownTable[l >> 2],
			    0x10000) - period;
			if (l & 0x03)
			    vdelta +=
				_muldiv(period,
				CSoundFile_FineLinearSlideDownTable[l & 0x03],
				0x10000) - period;

		    } else {
			vdelta =
			    _muldiv(period,
			    CSoundFile_LinearSlideUpTable[l >> 2],
			    0x10000) - period;
			if (l & 0x03)
			    vdelta +=
				_muldiv(period,
				CSoundFile_FineLinearSlideUpTable[l & 0x03],
				0x10000) - period;

		    }
		}
		period += vdelta;
		if ((that->m_nTickCount) || ((that->m_nType & MOD_TYPE_IT)
			&& (!(that->m_dwSongFlags & SONG_ITOLDEFFECTS)))) {
		    pChn->nVibratoPos = (vibpos + pChn->nVibratoSpeed) & 0x3F;
		}
	    }
	    // Panbrello
	    if (pChn->dwFlags & CHN_PANBRELLO) {
		unsigned panpos = ((pChn->nPanbrelloPos + 0x10) >> 2) & 0x3F;
		long pdelta;

		switch (pChn->nPanbrelloType & 0x03) {
		    case 1:
			pdelta = CSoundFile_ModRampDownTable[panpos];
			break;
		    case 2:
			pdelta = CSoundFile_ModSquareTable[panpos];
			break;
		    case 3:
			pdelta = CSoundFile_ModRandomTable[panpos];
			break;
		    default:
			pdelta = CSoundFile_ModSinusTable[panpos];
		}
		pChn->nPanbrelloPos += pChn->nPanbrelloSpeed;
		pdelta = ((pdelta * (int)pChn->nPanbrelloDepth) + 2) >> 3;
		pdelta += pChn->nRealPan;
		if (pdelta < 0)
		    pdelta = 0;
		if (pdelta > 256)
		    pdelta = 256;
		pChn->nRealPan = pdelta;
	    }

	    nPeriodFrac = 0;
	    // Instrument Auto-Vibrato
	    if ((pChn->pInstrument) && (pChn->pInstrument->nVibDepth)) {
		MODINSTRUMENT *pins = pChn->pInstrument;
		int val;
		int n;

		if (pins->nVibSweep == 0) {
		    pChn->nAutoVibDepth = pins->nVibDepth << 8;
		} else {
		    if (that->m_nType & MOD_TYPE_IT) {
			pChn->nAutoVibDepth += pins->nVibSweep << 3;
		    } else if (!(pChn->dwFlags & CHN_KEYOFF)) {
			pChn->nAutoVibDepth +=
			    (pins->nVibDepth << 8) / pins->nVibSweep;
		    }
		    if ((pChn->nAutoVibDepth >> 8) > pins->nVibDepth)
			pChn->nAutoVibDepth = pins->nVibDepth << 8;
		}
		pChn->nAutoVibPos += pins->nVibRate;

		switch (pins->nVibType) {
		    case 4:		// Random
			val =
			    CSoundFile_ModRandomTable[pChn->
			    nAutoVibPos & 0x3F];
			pChn->nAutoVibPos++;
			break;
		    case 3:		// Ramp Down
			val =
			    ((0x40 - (pChn->nAutoVibPos >> 1)) & 0x7F) - 0x40;
			break;
		    case 2:		// Ramp Up
			val =
			    ((0x40 + (pChn->nAutoVibPos >> 1)) & 0x7f) - 0x40;
			break;
		    case 1:		// Square
			val = (pChn->nAutoVibPos & 128) ? +64 : -64;
			break;
		    default:		// Sine
			val =
			    CSoundFile_Ft2VibratoTable[pChn->
			    nAutoVibPos & 255];
		}
		n = ((val * pChn->nAutoVibDepth) >> 8);

		if (that->m_nType & MOD_TYPE_IT) {
		    int df1, df2;
		    unsigned n1;

		    if (n < 0) {
			n = -n;
			n1 = n >> 8;

			df1 = CSoundFile_LinearSlideUpTable[n1];
			df2 = CSoundFile_LinearSlideUpTable[n1 + 1];
		    } else {
			n1 = n >> 8;

			df1 = CSoundFile_LinearSlideDownTable[n1];
			df2 = CSoundFile_LinearSlideDownTable[n1 + 1];
		    }
		    n >>= 2;
		    period =
			_muldiv(period, df1 + ((df2 - df1) * (n & 0x3F) >> 6),
			256);
		    nPeriodFrac = period & 0xFF;
		    period >>= 8;
		} else {
		    period += (n >> 6);
		}
	    }
	    // Final Period
	    if (period <= that->m_nMinPeriod) {
		if (that->m_nType & MOD_TYPE_S3M)
		    pChn->nLength = 0;
		period = that->m_nMinPeriod;
	    }
	    if (period > that->m_nMaxPeriod) {
		if ((that->m_nType & MOD_TYPE_IT) || (period >= 0x100000)) {
		    pChn->nFadeOutVol = 0;
		    pChn->dwFlags |= CHN_NOTEFADE;
		    pChn->nRealVolume = 0;
		}
		period = that->m_nMaxPeriod;
		nPeriodFrac = 0;
	    }
	    {
		unsigned ninc;
		unsigned freq =
		    CSoundFile_GetFreqFromPeriod(that, period, pChn->nC4Speed,
		    nPeriodFrac);

		if ((that->m_nType & MOD_TYPE_IT) && (freq < 256)) {
		    pChn->nFadeOutVol = 0;
		    pChn->dwFlags |= CHN_NOTEFADE;
		    pChn->nRealVolume = 0;
		}
		ninc = _muldiv(freq, 0x10000, CSoundFile_gdwMixingFreq);

		if ((ninc >= 0xFFB0) && (ninc <= 0x10090)) {
		    ninc = 0x10000;
		}
		if (that->m_nFreqFactor != 128) {
		    ninc = (ninc * that->m_nFreqFactor) >> 7;
		}
		if (ninc > 0xFF0000) {
		    ninc = 0xFF0000;
		}
		pChn->nInc = (ninc + 1) & ~3;
	    }
	}

	// Increment envelope position
	if (pChn->pHeader) {
	    INSTRUMENTHEADER *penv = pChn->pHeader;

	    // Volume Envelope
	    if (pChn->dwFlags & CHN_VOLENV) {
		// Increase position
		pChn->nVolEnvPosition++;
		// Volume Loop ?
		if (penv->dwFlags & ENV_VOLLOOP) {
		    unsigned volloopend = penv->VolPoints[penv->nVolLoopEnd];

		    if (that->m_nType != MOD_TYPE_XM)
			volloopend++;
		    if (pChn->nVolEnvPosition == volloopend) {
			pChn->nVolEnvPosition =
			    penv->VolPoints[penv->nVolLoopStart];
			if ((penv->nVolLoopEnd == penv->nVolLoopStart)
			    && (!penv->VolEnv[penv->nVolLoopStart])
			    && ((!(that->m_nType & MOD_TYPE_XM))
				|| (penv->nVolLoopEnd + 1 == penv->nVolEnv))) {
			    pChn->dwFlags |= CHN_NOTEFADE;
			    pChn->nFadeOutVol = 0;
			}
		    }
		}
		// Volume Sustain ?
		if ((penv->dwFlags & ENV_VOLSUSTAIN)
		    && (!(pChn->dwFlags & CHN_KEYOFF))) {
		    if (pChn->nVolEnvPosition ==
			(unsigned)penv->VolPoints[penv->nVolSustainEnd] + 1)
			pChn->nVolEnvPosition =
			    penv->VolPoints[penv->nVolSustainBegin];
		} else
		    // End of Envelope ?
		if (pChn->nVolEnvPosition > penv->VolPoints[penv->nVolEnv - 1]) {
		    if ((that->m_nType & MOD_TYPE_IT)
			|| (pChn->dwFlags & CHN_KEYOFF))
			pChn->dwFlags |= CHN_NOTEFADE;
		    pChn->nVolEnvPosition = penv->VolPoints[penv->nVolEnv - 1];
		    if ((!penv->VolEnv[penv->nVolEnv - 1])
			&& ((nChn >= that->m_nChannels)
			    || (that->m_nType & MOD_TYPE_IT))) {
			pChn->dwFlags |= CHN_NOTEFADE;
			pChn->nFadeOutVol = 0;

			pChn->nRealVolume = 0;
		    }
		}
	    }
	    // Panning Envelope
	    if (pChn->dwFlags & CHN_PANENV) {
		pChn->nPanEnvPosition++;
		if (penv->dwFlags & ENV_PANLOOP) {
		    unsigned panloopend = penv->PanPoints[penv->nPanLoopEnd];

		    if (that->m_nType != MOD_TYPE_XM)
			panloopend++;
		    if (pChn->nPanEnvPosition == panloopend)
			pChn->nPanEnvPosition =
			    penv->PanPoints[penv->nPanLoopStart];
		}
		// Panning Sustain ?
		if ((penv->dwFlags & ENV_PANSUSTAIN)
		    && (pChn->nPanEnvPosition ==
			(unsigned)penv->PanPoints[penv->nPanSustainEnd] + 1)
		    && (!(pChn->dwFlags & CHN_KEYOFF))) {
		    // Panning sustained
		    pChn->nPanEnvPosition =
			penv->PanPoints[penv->nPanSustainBegin];
		} else {
		    if (pChn->nPanEnvPosition >
			penv->PanPoints[penv->nPanEnv - 1])
			pChn->nPanEnvPosition =
			    penv->PanPoints[penv->nPanEnv - 1];
		}
	    }
	    // Pitch Envelope
	    if (pChn->dwFlags & CHN_PITCHENV) {
		// Increase position
		pChn->nPitchEnvPosition++;
		// Pitch Loop ?
		if (penv->dwFlags & ENV_PITCHLOOP) {
		    if (pChn->nPitchEnvPosition >=
			penv->PitchPoints[penv->nPitchLoopEnd])
			pChn->nPitchEnvPosition =
			    penv->PitchPoints[penv->nPitchLoopStart];
		}
		// Pitch Sustain ?
		if ((penv->dwFlags & ENV_PITCHSUSTAIN)
		    && (!(pChn->dwFlags & CHN_KEYOFF))) {
		    if (pChn->nPitchEnvPosition ==
			(unsigned)penv->PitchPoints[penv->nPitchSustainEnd] +
			1)
			pChn->nPitchEnvPosition =
			    penv->PitchPoints[penv->nPitchSustainBegin];
		} else {
		    if (pChn->nPitchEnvPosition >
			penv->PitchPoints[penv->nPitchEnv - 1])
			pChn->nPitchEnvPosition =
			    penv->PitchPoints[penv->nPitchEnv - 1];
		}
	    }
	}

#ifdef MODPLUG_PLAYER
	// Limit CPU -> > 80% -> don't ramp
	if ((CSoundFile_gnCPUUsage >= 80) && (!pChn->nRealVolume)) {
	    pChn->nLeftVol = pChn->nRightVol = 0;
	}
#endif // MODPLUG_PLAYER
	// Volume ramping
	pChn->dwFlags &= ~CHN_VOLUMERAMP;
	if ((pChn->nRealVolume) || (pChn->nLeftVol) || (pChn->nRightVol))
	    pChn->dwFlags |= CHN_VOLUMERAMP;
#ifdef MODPLUG_PLAYER
	// Decrease VU-Meter
	if (pChn->nVUMeter > VUMETER_DECAY)
	    pChn->nVUMeter -= VUMETER_DECAY;
	else
	    pChn->nVUMeter = 0;
#endif // MODPLUG_PLAYER
#ifdef ENABLE_STEREOVU
	if (pChn->nLeftVU > VUMETER_DECAY)
	    pChn->nLeftVU -= VUMETER_DECAY;
	else
	    pChn->nLeftVU = 0;
	if (pChn->nRightVU > VUMETER_DECAY)
	    pChn->nRightVU -= VUMETER_DECAY;
	else
	    pChn->nRightVU = 0;
#endif
	// Check for too big nInc
	if (((pChn->nInc >> 16) + 1) >=
	    (long)(pChn->nLoopEnd - pChn->nLoopStart))
	    pChn->dwFlags &= ~CHN_LOOP;
	pChn->nNewRightVol = pChn->nNewLeftVol = 0;
	pChn->pCurrentSample = ((pChn->pSample) && (pChn->nLength)
	    && (pChn->nInc)) ? pChn->pSample : NULL;
	if (pChn->pCurrentSample) {
	    // Update VU-Meter (nRealVolume is 14-bit)
#ifdef MODPLUG_PLAYER
	    unsigned vutmp = pChn->nRealVolume >> (14 - 8);

	    if (vutmp > 0xFF)
		vutmp = 0xFF;
	    if (pChn->nVUMeter >= 0x100)
		pChn->nVUMeter = vutmp;
	    vutmp >>= 1;
	    if (pChn->nVUMeter < vutmp)
		pChn->nVUMeter = vutmp;
#endif // MODPLUG_PLAYER
#ifdef ENABLE_STEREOVU
	    unsigned vul = (pChn->nRealVolume * pChn->nRealPan) >> 14;

	    if (vul > 127)
		vul = 127;
	    if (pChn->nLeftVU > 127)
		pChn->nLeftVU = (uint8_t) vul;
	    vul >>= 1;
	    if (pChn->nLeftVU < vul)
		pChn->nLeftVU = (uint8_t) vul;
	    unsigned vur = (pChn->nRealVolume * (256 - pChn->nRealPan)) >> 14;

	    if (vur > 127)
		vur = 127;
	    if (pChn->nRightVU > 127)
		pChn->nRightVU = (uint8_t) vur;
	    vur >>= 1;
	    if (pChn->nRightVU < vur)
		pChn->nRightVU = (uint8_t) vur;
#endif
#ifdef MODPLUG_TRACKER
	    unsigned kChnMasterVol =
		(pChn->dwFlags & CHN_EXTRALOUD) ? 0x100 : nMasterVol;
#else
#define	    kChnMasterVol   nMasterVol
#endif // MODPLUG_TRACKER
	    // Adjusting volumes
	    if (CSoundFile_gnChannels >= 2) {
		long realvol;
		int pan = ((int)pChn->nRealPan) - 128;

		pan *= (int)CSoundFile_StereoSeparation;
		pan /= 128;
		pan += 128;

		if (pan < 0)
		    pan = 0;
		if (pan > 256)
		    pan = 256;
#ifndef FASTSOUNDLIB
		if (CSoundFile_gdwSoundSetup & SNDMIX_REVERSESTEREO) {
		    pan = 256 - pan;
		}
#endif
		realvol = (pChn->nRealVolume * kChnMasterVol) >> (8 - 1);

		if (CSoundFile_gdwSoundSetup & SNDMIX_SOFTPANNING) {
		    if (pan < 128) {
			pChn->nNewLeftVol = (realvol * pan) >> 8;
			pChn->nNewRightVol = (realvol * 128) >> 8;
		    } else {
			pChn->nNewLeftVol = (realvol * 128) >> 8;
			pChn->nNewRightVol = (realvol * (256 - pan)) >> 8;
		    }
		} else {
		    pChn->nNewLeftVol = (realvol * pan) >> 8;
		    pChn->nNewRightVol = (realvol * (256 - pan)) >> 8;
		}
	    } else {
		pChn->nNewRightVol = (pChn->nRealVolume * kChnMasterVol) >> 8;
		pChn->nNewLeftVol = pChn->nNewRightVol;
	    }
	    // Clipping volumes
	    if (pChn->nNewRightVol > 0xFFFF)
		pChn->nNewRightVol = 0xFFFF;
	    if (pChn->nNewLeftVol > 0xFFFF)
		pChn->nNewLeftVol = 0xFFFF;
	    // Check IDO
	    if (CSoundFile_gdwSoundSetup & SNDMIX_NORESAMPLING) {
		pChn->dwFlags |= CHN_NOIDO;
	    } else {
		pChn->dwFlags &= ~(CHN_NOIDO | CHN_HQSRC);
		if (pChn->nInc == 0x10000) {
		    pChn->dwFlags |= CHN_NOIDO;
		} else {
		    if (((CSoundFile_gdwSoundSetup & SNDMIX_HQRESAMPLER) == 0)
			&& ((CSoundFile_gdwSoundSetup & SNDMIX_ULTRAHQSRCMODE) ==
			    0)) {
			if (pChn->nInc >= 0xFF00)
			    pChn->dwFlags |= CHN_NOIDO;
		    }
		}
	    }
	    pChn->nNewRightVol >>= MIXING_ATTENUATION;
	    pChn->nNewLeftVol >>= MIXING_ATTENUATION;
	    pChn->nRightRamp = pChn->nLeftRamp = 0;
	    // Dolby Pro-Logic Surround
	    if ((pChn->dwFlags & CHN_SURROUND) && (CSoundFile_gnChannels <= 2))
		pChn->nNewLeftVol = -pChn->nNewLeftVol;
	    // Checking Ping-Pong Loops
	    if (pChn->dwFlags & CHN_PINGPONGFLAG)
		pChn->nInc = -pChn->nInc;
	    // Setting up volume ramp
	    if ((pChn->dwFlags & CHN_VOLUMERAMP)
		&& ((pChn->nRightVol != pChn->nNewRightVol)
		    || (pChn->nLeftVol != pChn->nNewLeftVol))) {
		long nRampLength = CSoundFile_gnVolumeRampSamples;
		long nRightDelta =
		    ((pChn->nNewRightVol -
			pChn->nRightVol) << VOLUMERAMPPRECISION);
		long nLeftDelta =
		    ((pChn->nNewLeftVol -
			pChn->nLeftVol) << VOLUMERAMPPRECISION);
#ifndef MODPLUG_FASTSOUNDLIB
		if ((CSoundFile_gdwSoundSetup & SNDMIX_DIRECTTODISK)
		    || ((CSoundFile_gdwSysInfo
			    & (SYSMIX_ENABLEMMX | SYSMIX_FASTCPU))
			&& (CSoundFile_gdwSoundSetup & SNDMIX_HQRESAMPLER)
			&& (CSoundFile_gnCPUUsage <= 20))) {
		    if ((pChn->nRightVol | pChn->nLeftVol)
			&& (pChn->nNewRightVol | pChn->nNewLeftVol)
			&& (!(pChn->dwFlags & CHN_FASTVOLRAMP))) {
			nRampLength = that->m_nBufferCount;
			if (nRampLength > (1 << (VOLUMERAMPPRECISION - 1)))
			    nRampLength = (1 << (VOLUMERAMPPRECISION - 1));
			if (nRampLength < (long)CSoundFile_gnVolumeRampSamples)
			    nRampLength = CSoundFile_gnVolumeRampSamples;
		    }
		}
#endif
		pChn->nRightRamp = nRightDelta / nRampLength;
		pChn->nLeftRamp = nLeftDelta / nRampLength;
		pChn->nRightVol =
		    pChn->nNewRightVol -
		    ((pChn->nRightRamp * nRampLength) >> VOLUMERAMPPRECISION);
		pChn->nLeftVol =
		    pChn->nNewLeftVol -
		    ((pChn->nLeftRamp * nRampLength) >> VOLUMERAMPPRECISION);
		if (pChn->nRightRamp | pChn->nLeftRamp) {
		    pChn->nRampLength = nRampLength;
		} else {
		    pChn->dwFlags &= ~CHN_VOLUMERAMP;
		    pChn->nRightVol = pChn->nNewRightVol;
		    pChn->nLeftVol = pChn->nNewLeftVol;
		}
	    } else {
		pChn->dwFlags &= ~CHN_VOLUMERAMP;
		pChn->nRightVol = pChn->nNewRightVol;
		pChn->nLeftVol = pChn->nNewLeftVol;
	    }
	    pChn->nRampRightVol = pChn->nRightVol << VOLUMERAMPPRECISION;
	    pChn->nRampLeftVol = pChn->nLeftVol << VOLUMERAMPPRECISION;
	    // Adding the channel in the channel list
	    that->ChnMix[that->m_nMixChannels++] = nChn;
	    if (that->m_nMixChannels >= MAX_CHANNELS)
		break;
	} else {
#ifdef ENABLE_STEREOVU
	    // Note change but no sample
	    if (pChn->nLeftVU > 128)
		pChn->nLeftVU = 0;
	    if (pChn->nRightVU > 128)
		pChn->nRightVU = 0;
#endif
	    if (pChn->nVUMeter > 0xFF)
		pChn->nVUMeter = 0;
	    pChn->nLeftVol = pChn->nRightVol = 0;
	    pChn->nLength = 0;
	}
    }
    // Checking Max Mix Channels reached: ordering by volume
    if ((that->m_nMixChannels >= CSoundFile_MaxMixChannels)
	&& (!(CSoundFile_gdwSoundSetup & SNDMIX_DIRECTTODISK))) {
	unsigned i;

	for (i = 0; i < that->m_nMixChannels; i++) {
	    unsigned j = i;

	    while ((j + 1 < that->m_nMixChannels)
		&& (that->Chn[that->ChnMix[j]].nRealVolume <
		    that->Chn[that->ChnMix[j + 1]].nRealVolume)) {
		unsigned n = that->ChnMix[j];

		that->ChnMix[j] = that->ChnMix[j + 1];
		that->ChnMix[j + 1] = n;
		j++;
	    }
	}
    }
    if (that->m_dwSongFlags & SONG_GLOBALFADE) {
	if (!that->m_nGlobalFadeSamples) {
	    that->m_dwSongFlags |= SONG_ENDREACHED;
	    return 0;
	}
	if (that->m_nGlobalFadeSamples > that->m_nBufferCount)
	    that->m_nGlobalFadeSamples -= that->m_nBufferCount;
	else
	    that->m_nGlobalFadeSamples = 0;
    }
    return 1;
}
