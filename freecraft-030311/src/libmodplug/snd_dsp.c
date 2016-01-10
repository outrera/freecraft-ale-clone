/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: snd_dsp.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

#ifdef MODPLUG_FASTSOUNDLIB
#define MODPLUG_NO_REVERB
#endif

// Delayed Surround Filters
#ifndef MODPLUG_FASTSOUNDLIB
#define nDolbyHiFltAttn	    6
#define nDolbyHiFltMask	    3
#define DOLBYATTNROUNDUP    31
#else
#define nDolbyHiFltAttn	    3
#define nDolbyHiFltMask	    3
#define DOLBYATTNROUNDUP    3
#endif

// Bass Expansion
#define XBASS_DELAY	    14		// 2.5 ms

// Buffer Sizes
#define XBASSBUFFERSIZE	    64		// 2 ms at 50KHz
#define FILTERBUFFERSIZE    64		// 1.25 ms
#define SURROUNDBUFFERSIZE  ((MAX_SAMPLE_RATE * 50) / 1000)
#define REVERBBUFFERSIZE    ((MAX_SAMPLE_RATE * 200) / 1000)
#define REVERBBUFFERSIZE2   ((REVERBBUFFERSIZE*13) / 17)
#define REVERBBUFFERSIZE3   ((REVERBBUFFERSIZE*7) / 13)
#define REVERBBUFFERSIZE4   ((REVERBBUFFERSIZE*7) / 19)

// DSP Effects: PUBLIC members
unsigned CSoundFile_XBassDepth = 6;
unsigned CSoundFile_XBassRange = XBASS_DELAY;
unsigned CSoundFile_ReverbDepth = 1;
unsigned CSoundFile_ReverbDelay = 100;
unsigned CSoundFile_ProLogicDepth = 12;
unsigned CSoundFile_ProLogicDelay = 20;

////////////////////////////////////////////////////////////////////
// DSP Effects internal state

// Bass Expansion: low-pass filter
static long nXBassSum;
static long nXBassBufferPos;
static long nXBassDlyPos;
static long nXBassMask;

// Noise Reduction: simple low-pass filter
static long nLeftNR;
static long nRightNR;

// Surround Encoding: 1 delay line + low-pass filter + high-pass filter
static long nSurroundSize;
static long nSurroundPos;
static long nDolbyDepth;
static long nDolbyLoDlyPos;
static long nDolbyLoFltPos;
static long nDolbyLoFltSum;
static long nDolbyHiFltPos;
static long nDolbyHiFltSum;

// Reverb: 4 delay lines + high-pass filter + low-pass filter
#ifndef MODPLUG_NO_REVERB
static long nReverbSize;
static long nReverbBufferPos;
static long nReverbSize2;
static long nReverbBufferPos2;
static long nReverbSize3;
static long nReverbBufferPos3;
static long nReverbSize4;
static long nReverbBufferPos4;
static long nReverbLoFltSum;
static long nReverbLoFltPos;
static long nReverbLoDlyPos;
static long nFilterAttn;
static long gRvbLowPass[8];
static long gRvbLPPos;
static long gRvbLPSum;
static long ReverbLoFilterBuffer[XBASSBUFFERSIZE];
static long ReverbLoFilterDelay[XBASSBUFFERSIZE];
static long ReverbBuffer[REVERBBUFFERSIZE];
static long ReverbBuffer2[REVERBBUFFERSIZE2];
static long ReverbBuffer3[REVERBBUFFERSIZE3];
static long ReverbBuffer4[REVERBBUFFERSIZE4];
#endif
static long XBassBuffer[XBASSBUFFERSIZE];
static long XBassDelay[XBASSBUFFERSIZE];
static long DolbyLoFilterBuffer[XBASSBUFFERSIZE];
static long DolbyLoFilterDelay[XBASSBUFFERSIZE];
static long DolbyHiFilterBuffer[FILTERBUFFERSIZE];
static long SurroundBuffer[SURROUNDBUFFERSIZE];


static unsigned GetMaskFromSize(unsigned len)
//-----------------------------------
{
    unsigned n = 2;

    while (n <= len) {
	n <<= 1;
    }
    return (n >> 1) - 1;
}

void CSoundFile_InitializeDSP(int bReset)
//-----------------------------------------
{
    int bResetBass;

    if (!CSoundFile_ReverbDelay) {
	CSoundFile_ReverbDelay = 100;
    }
    if (!CSoundFile_XBassRange) {
	CSoundFile_XBassRange = XBASS_DELAY;
    }
    if (!CSoundFile_ProLogicDelay) {
	CSoundFile_ProLogicDelay = 20;
    }
    if (CSoundFile_XBassDepth > 8) {
	CSoundFile_XBassDepth = 8;
    }
    if (CSoundFile_XBassDepth < 2) {
	CSoundFile_XBassDepth = 2;
    }
    if (bReset) {
	// Noise Reduction
	nLeftNR = nRightNR = 0;
    }
    // Pro-Logic Surround
    nSurroundPos = nSurroundSize = 0;
    nDolbyLoFltPos = nDolbyLoFltSum = nDolbyLoDlyPos = 0;
    nDolbyHiFltPos = nDolbyHiFltSum = 0;
    if (CSoundFile_gdwSoundSetup & SNDMIX_SURROUND) {
	memset(DolbyLoFilterBuffer, 0, sizeof(DolbyLoFilterBuffer));
	memset(DolbyHiFilterBuffer, 0, sizeof(DolbyHiFilterBuffer));
	memset(DolbyLoFilterDelay, 0, sizeof(DolbyLoFilterDelay));
	memset(SurroundBuffer, 0, sizeof(SurroundBuffer));
	nSurroundSize = (CSoundFile_gdwMixingFreq * CSoundFile_ProLogicDelay) / 1000;
	if (nSurroundSize > SURROUNDBUFFERSIZE) {
	    nSurroundSize = SURROUNDBUFFERSIZE;
	}
	if (CSoundFile_ProLogicDepth < 8) {
	    nDolbyDepth = (32 >> CSoundFile_ProLogicDepth) + 32;
	} else {
	    nDolbyDepth =
		(CSoundFile_ProLogicDepth <
		16) ? (8 + (CSoundFile_ProLogicDepth - 8) * 7) : 64;
	}
	nDolbyDepth >>= 2;
    }
    // Reverb Setup
#ifndef MODPLUG_NO_REVERB
    if (CSoundFile_gdwSoundSetup & SNDMIX_REVERB) {
	unsigned nrs = (CSoundFile_gdwMixingFreq * CSoundFile_ReverbDelay) / 1000;
	unsigned nfa = CSoundFile_ReverbDepth + 1;

	if (nrs > REVERBBUFFERSIZE)
	    nrs = REVERBBUFFERSIZE;
	if ((bReset) || (nrs != (unsigned)nReverbSize)
	    || (nfa != (unsigned)nFilterAttn)) {
	    nFilterAttn = nfa;
	    nReverbSize = nrs;
	    nReverbBufferPos = nReverbBufferPos2 = nReverbBufferPos3 =
		nReverbBufferPos4 = 0;
	    nReverbLoFltSum = nReverbLoFltPos = nReverbLoDlyPos = 0;
	    gRvbLPSum = gRvbLPPos = 0;
	    nReverbSize2 = (nReverbSize * 13) / 17;
	    if (nReverbSize2 > REVERBBUFFERSIZE2)
		nReverbSize2 = REVERBBUFFERSIZE2;
	    nReverbSize3 = (nReverbSize * 7) / 13;
	    if (nReverbSize3 > REVERBBUFFERSIZE3)
		nReverbSize3 = REVERBBUFFERSIZE3;
	    nReverbSize4 = (nReverbSize * 7) / 19;
	    if (nReverbSize4 > REVERBBUFFERSIZE4)
		nReverbSize4 = REVERBBUFFERSIZE4;
	    memset(ReverbLoFilterBuffer, 0, sizeof(ReverbLoFilterBuffer));
	    memset(ReverbLoFilterDelay, 0, sizeof(ReverbLoFilterDelay));
	    memset(ReverbBuffer, 0, sizeof(ReverbBuffer));
	    memset(ReverbBuffer2, 0, sizeof(ReverbBuffer2));
	    memset(ReverbBuffer3, 0, sizeof(ReverbBuffer3));
	    memset(ReverbBuffer4, 0, sizeof(ReverbBuffer4));
	    memset(gRvbLowPass, 0, sizeof(gRvbLowPass));
	}
    } else {
	nReverbSize = 0;
    }
#endif
    bResetBass = 0;

    // Bass Expansion Reset
    if (CSoundFile_gdwSoundSetup & SNDMIX_MEGABASS) {
	unsigned mask;
	unsigned nXBassSamples;

	nXBassSamples = (CSoundFile_gdwMixingFreq * CSoundFile_XBassRange) / 10000;
	if (nXBassSamples > XBASSBUFFERSIZE) {
	    nXBassSamples = XBASSBUFFERSIZE;
	}
	mask = GetMaskFromSize(nXBassSamples);

	if (bReset || mask != (unsigned)nXBassMask) {
	    nXBassMask = mask;
	    bResetBass = 1;
	}
    } else {
	nXBassMask = 0;
	bResetBass = 1;
    }
    if (bResetBass) {
	nXBassSum = nXBassBufferPos = nXBassDlyPos = 0;
	memset(XBassBuffer, 0, sizeof(XBassBuffer));
	memset(XBassDelay, 0, sizeof(XBassDelay));
    }
}

void CSoundFile_ProcessStereoDSP(int count)
//------------------------------------------
{
#ifndef MODPLUG_NO_REVERB
    // Reverb
    if (CSoundFile_gdwSoundSetup & SNDMIX_REVERB) {
	int *pr = CSoundFile_MixSoundBuffer;
	int *pin = CSoundFile_MixReverbBuffer;
	int rvbcount = count;

	do {
	    int n;
	    int tmp;
	    int echo =
		ReverbBuffer[nReverbBufferPos] +
		ReverbBuffer2[nReverbBufferPos2]
		+ ReverbBuffer3[nReverbBufferPos3] + ReverbBuffer4[nReverbBufferPos4];	// echo = reverb signal

	    // Delay line and remove Low Frequencies            // v = original signal
	    int echodly = ReverbLoFilterDelay[nReverbLoDlyPos];	// echodly = delayed signal

	    ReverbLoFilterDelay[nReverbLoDlyPos] = echo >> 1;
	    nReverbLoDlyPos++;
	    nReverbLoDlyPos &= 0x1F;
	    n = nReverbLoFltPos;

	    nReverbLoFltSum -= ReverbLoFilterBuffer[n];
	    tmp = echo / 128;

	    ReverbLoFilterBuffer[n] = tmp;
	    nReverbLoFltSum += tmp;
	    echodly -= nReverbLoFltSum;
	    nReverbLoFltPos = (n + 1) & 0x3F;
	    // Reverb
	    tmp = (pin[0] + pin[1]) >> nFilterAttn;

	    pr[0] += pin[0] + echodly;
	    pr[1] += pin[1] + echodly;
	    tmp += echodly >> 2;
	    ReverbBuffer3[nReverbBufferPos3] = tmp;
	    ReverbBuffer4[nReverbBufferPos4] = tmp;
	    tmp += echodly >> 4;
	    tmp >>= 1;
	    gRvbLPSum -= gRvbLowPass[gRvbLPPos];
	    gRvbLPSum += tmp;
	    gRvbLowPass[gRvbLPPos] = tmp;

	    gRvbLPPos++;
	    gRvbLPPos &= 7;

	    tmp = gRvbLPSum >> 2;
	    ReverbBuffer[nReverbBufferPos] = tmp;
	    ReverbBuffer2[nReverbBufferPos2] = tmp;
	    if (++nReverbBufferPos >= nReverbSize)
		nReverbBufferPos = 0;
	    if (++nReverbBufferPos2 >= nReverbSize2)
		nReverbBufferPos2 = 0;
	    if (++nReverbBufferPos3 >= nReverbSize3)
		nReverbBufferPos3 = 0;
	    if (++nReverbBufferPos4 >= nReverbSize4)
		nReverbBufferPos4 = 0;
	    pr += 2;
	    pin += 2;
	} while (--rvbcount);
    }
#endif
    // Dolby Pro-Logic Surround
    if (CSoundFile_gdwSoundSetup & SNDMIX_SURROUND) {
	int *pr = CSoundFile_MixSoundBuffer, n = nDolbyLoFltPos;
	int r;

	for (r = count; r; r--) {
	    int secho;
	    int tmp;
	    int v =
		(pr[0] + pr[1] + DOLBYATTNROUNDUP) >> (nDolbyHiFltAttn + 1);
#ifndef MODPLUG_FASTSOUNDLIB
	    v *= (int)nDolbyDepth;
#endif
	    // Low-Pass Filter
	    nDolbyHiFltSum -= DolbyHiFilterBuffer[nDolbyHiFltPos];
	    DolbyHiFilterBuffer[nDolbyHiFltPos] = v;
	    nDolbyHiFltSum += v;
	    v = nDolbyHiFltSum;
	    nDolbyHiFltPos++;
	    nDolbyHiFltPos &= nDolbyHiFltMask;
	    // Surround
	    secho = SurroundBuffer[nSurroundPos];

	    SurroundBuffer[nSurroundPos] = v;
	    // Delay line and remove low frequencies
	    v = DolbyLoFilterDelay[nDolbyLoDlyPos];	// v = delayed signal
	    DolbyLoFilterDelay[nDolbyLoDlyPos] = secho;	// secho = signal
	    nDolbyLoDlyPos++;
	    nDolbyLoDlyPos &= 0x1F;
	    nDolbyLoFltSum -= DolbyLoFilterBuffer[n];
	    tmp = secho / 64;

	    DolbyLoFilterBuffer[n] = tmp;
	    nDolbyLoFltSum += tmp;
	    v -= nDolbyLoFltSum;
	    n++;
	    n &= 0x3F;
	    // Add echo
	    pr[0] += v;
	    pr[1] -= v;
	    if (++nSurroundPos >= nSurroundSize)
		nSurroundPos = 0;
	    pr += 2;
	}
	nDolbyLoFltPos = n;
    }
    // Bass Expansion
    if (CSoundFile_gdwSoundSetup & SNDMIX_MEGABASS) {
	int *px = CSoundFile_MixSoundBuffer;
	int xba = CSoundFile_XBassDepth + 1, xbamask = (1 << xba) - 1;
	int n = nXBassBufferPos;
	int x;

	for (x = count; x; x--) {
	    int tmp0 = px[0] + px[1];
	    int tmp = (tmp0 + ((tmp0 >> 31) & xbamask)) >> xba;

	    nXBassSum -= XBassBuffer[n];
	    XBassBuffer[n] = tmp;
	    nXBassSum += tmp;
	    tmp = XBassDelay[nXBassDlyPos];

	    XBassDelay[nXBassDlyPos] = px[0];
	    px[0] = tmp + nXBassSum;
	    tmp = XBassDelay[nXBassDlyPos + 1];
	    XBassDelay[nXBassDlyPos + 1] = px[1];
	    px[1] = tmp + nXBassSum;
	    nXBassDlyPos = (nXBassDlyPos + 2) & nXBassMask;
	    px += 2;
	    n++;
	    n &= nXBassMask;
	}
	nXBassBufferPos = n;
    }
    // Noise Reduction
    if (CSoundFile_gdwSoundSetup & SNDMIX_NOISEREDUCTION) {
	int n1 = nLeftNR, n2 = nRightNR;
	int *pnr = CSoundFile_MixSoundBuffer;
	int nr;

	for (nr = count; nr; nr--) {
	    int vnr = pnr[0] >> 1;

	    pnr[0] = vnr + n1;
	    n1 = vnr;
	    vnr = pnr[1] >> 1;
	    pnr[1] = vnr + n2;
	    n2 = vnr;
	    pnr += 2;
	}
	nLeftNR = n1;
	nRightNR = n2;
    }
}

void CSoundFile_ProcessMonoDSP(int count)
//----------------------------------------
{
#ifndef MODPLUG_NO_REVERB
    // Reverb
    if (CSoundFile_gdwSoundSetup & SNDMIX_REVERB) {
	int *pr = CSoundFile_MixSoundBuffer, rvbcount = count, *pin = CSoundFile_MixReverbBuffer;

	do {
	    int n;
	    int tmp;
	    int echo =
		ReverbBuffer[nReverbBufferPos] +
		ReverbBuffer2[nReverbBufferPos2]
		+ ReverbBuffer3[nReverbBufferPos3] + ReverbBuffer4[nReverbBufferPos4];	// echo = reverb signal

	    // Delay line and remove Low Frequencies            // v = original signal
	    int echodly = ReverbLoFilterDelay[nReverbLoDlyPos];	// echodly = delayed signal

	    ReverbLoFilterDelay[nReverbLoDlyPos] = echo >> 1;
	    nReverbLoDlyPos++;
	    nReverbLoDlyPos &= 0x1F;
	    n = nReverbLoFltPos;

	    nReverbLoFltSum -= ReverbLoFilterBuffer[n];
	    tmp = echo / 128;

	    ReverbLoFilterBuffer[n] = tmp;
	    nReverbLoFltSum += tmp;
	    echodly -= nReverbLoFltSum;
	    nReverbLoFltPos = (n + 1) & 0x3F;
	    // Reverb
	    tmp = pin[0] >> (nFilterAttn - 1);

	    *pr++ += pin[0] + echodly;
	    pin++;
	    tmp += echodly >> 2;
	    ReverbBuffer3[nReverbBufferPos3] = tmp;
	    ReverbBuffer4[nReverbBufferPos4] = tmp;
	    tmp += echodly >> 4;
	    tmp >>= 1;
	    gRvbLPSum -= gRvbLowPass[gRvbLPPos];
	    gRvbLPSum += tmp;
	    gRvbLowPass[gRvbLPPos] = tmp;
	    gRvbLPPos++;
	    gRvbLPPos &= 7;

	    tmp = gRvbLPSum >> 2;
	    ReverbBuffer[nReverbBufferPos] = tmp;
	    ReverbBuffer2[nReverbBufferPos2] = tmp;
	    if (++nReverbBufferPos >= nReverbSize)
		nReverbBufferPos = 0;
	    if (++nReverbBufferPos2 >= nReverbSize2)
		nReverbBufferPos2 = 0;
	    if (++nReverbBufferPos3 >= nReverbSize3)
		nReverbBufferPos3 = 0;
	    if (++nReverbBufferPos4 >= nReverbSize4)
		nReverbBufferPos4 = 0;
	} while (--rvbcount);
    }
#endif
    // Bass Expansion
    if (CSoundFile_gdwSoundSetup & SNDMIX_MEGABASS) {
	int *px = CSoundFile_MixSoundBuffer;
	int xba = CSoundFile_XBassDepth, xbamask = (1 << xba) - 1;
	int n = nXBassBufferPos;
	int x;

	for (x = count; x; x--) {
	    int tmp0 = *px;
	    int tmp = (tmp0 + ((tmp0 >> 31) & xbamask)) >> xba;

	    nXBassSum -= XBassBuffer[n];
	    XBassBuffer[n] = tmp;
	    nXBassSum += tmp;
	    tmp = XBassDelay[nXBassDlyPos];

	    XBassDelay[nXBassDlyPos] = *px;
	    *px++ = tmp + nXBassSum;
	    nXBassDlyPos = (nXBassDlyPos + 2) & nXBassMask;
	    n++;
	    n &= nXBassMask;
	}
	nXBassBufferPos = n;
    }
    // Noise Reduction
    if (CSoundFile_gdwSoundSetup & SNDMIX_NOISEREDUCTION) {
	int n = nLeftNR;
	int *pnr = CSoundFile_MixSoundBuffer;
	int nr;

	for (nr = count; nr; pnr++, nr--) {
	    int vnr = *pnr >> 1;

	    *pnr = vnr + n;
	    n = vnr;
	}
	nLeftNR = n;
    }
}

/////////////////////////////////////////////////////////////////
// Clean DSP Effects interface

// [Reverb level 0(quiet)-100(loud)], [delay in ms, usually 40-200ms]
int CSoundFile_SetReverbParameters(CSoundFile *that __attribute__((unused)),
    unsigned nDepth, unsigned nDelay)
//------------------------------------------------------------
{
    unsigned gain;

    if (nDepth > 100) {
	nDepth = 100;
    }
    gain = nDepth / 20;

    if (gain > 4) {
	gain = 4;
    }
    CSoundFile_ReverbDepth = 4 - gain;
    if (nDelay < 40) {
	nDelay = 40;
    }
    if (nDelay > 250) {
	nDelay = 250;
    }
    CSoundFile_ReverbDelay = nDelay;

    return 1;
}

// [XBass level 0(quiet)-100(loud)], [cutoff in Hz 20-100]
int CSoundFile_SetXBassParameters(CSoundFile * that __attribute__((unused)),
    unsigned nDepth, unsigned nRange)
//-----------------------------------------------------------
{
    unsigned gain;
    unsigned range;

    if (nDepth > 100) {
	nDepth = 100;
    }
    gain = nDepth / 20;

    if (gain > 4) {
	gain = 4;
    }
    CSoundFile_XBassDepth = 8 - gain;	// filter attenuation 1/256 .. 1/16
    range = nRange / 5;

    if (range > 5) {
	range -= 5;
    } else {
	range = 0;
    }
    if (nRange > 16) {
	nRange = 16;
    }
    CSoundFile_XBassRange = 21 - range;	// filter average on 0.5-1.6ms
    return 1;
}

// [Surround level 0(quiet)-100(heavy)] [delay in ms, usually 5-50ms]
int CSoundFile_SetSurroundParameters(CSoundFile * that __attribute__((unused)),
    unsigned nDepth, unsigned nDelay)
//--------------------------------------------------------------
{
    unsigned gain = (nDepth * 16) / 100;

    if (gain > 16) {
	gain = 16;
    }
    if (gain < 1) {
	gain = 1;
    }
    CSoundFile_ProLogicDepth = gain;
    if (nDelay < 4) {
	nDelay = 4;
    }
    if (nDelay > 50) {
	nDelay = 50;
    }
    CSoundFile_ProLogicDelay = nDelay;
    return 1;
}

int CSoundFile_SetWaveConfigEx(CSoundFile * that __attribute__((unused)),
    int bSurround, int bNoOverSampling, int bReverb, int hqido,
    int bMegaBass, int bNR, int bEQ)
//----------------------------------------------------------------------------------------------------------------------------
{
    uint32_t d =
	CSoundFile_gdwSoundSetup & ~(SNDMIX_SURROUND | SNDMIX_NORESAMPLING
	    | SNDMIX_REVERB | SNDMIX_HQRESAMPLER | SNDMIX_MEGABASS
	    | SNDMIX_NOISEREDUCTION | SNDMIX_EQ);

    if (bSurround) {
	d |= SNDMIX_SURROUND;
    }
    if (bNoOverSampling) {
	d |= SNDMIX_NORESAMPLING;
    }
    if (bReverb) {
	d |= SNDMIX_REVERB;
    }
    if (hqido) {
	d |= SNDMIX_HQRESAMPLER;
    }
    if (bMegaBass) {
	d |= SNDMIX_MEGABASS;
    }
    if (bNR) {
	d |= SNDMIX_NOISEREDUCTION;
    }
    if (bEQ) {
	d |= SNDMIX_EQ;
    }
    CSoundFile_gdwSoundSetup = d;
    CSoundFile_InitPlayer(0);
    return 1;
}
