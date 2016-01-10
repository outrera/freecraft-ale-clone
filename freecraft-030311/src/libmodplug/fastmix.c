/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *	    Markus Fick <webmaster@mark-f.de> spline + fir-resampler
 *
 *	$Id: fastmix.c,v 1.3 2002/04/13 19:27:18 jsalmon3 Exp $
*/

#include "stdafx.h"
#include "sndfile.h"
#include <math.h>

#ifdef _MSC_VER
#pragma bss_seg(".modplug")
#endif

// Front Mix Buffer (Also room for interleaved rear mix)
int CSoundFile_MixSoundBuffer[MIXBUFFERSIZE * 4];

// Reverb Mix Buffer
#ifndef MODPLUG_NO_REVERB
int CSoundFile_MixReverbBuffer[MIXBUFFERSIZE * 2];
#endif

#ifndef MODPLUG_FASTSOUNDLIB
int CSoundFile_MixRearBuffer[MIXBUFFERSIZE * 2];
#endif

#ifdef _MSC_VER
#pragma bss_seg()
#endif

#ifndef _MSC_VER
#undef __declspec
#define __declspec(x)
#endif

/*
  ------------------------------------------------------------------------------------------------
   cubic spline interpolation doc,
    (derived from "digital image warping", g. wolberg)

    interpolation polynomial: f(x) = A3*(x-floor(x))**3 + A2*(x-floor(x))**2 + A1*(x-floor(x)) + A0

    with Y = equispaced data points (dist=1), YD = first derivates of data points and IP = floor(x)
    the A[0..3] can be found by solving
      A0  = Y[IP]
      A1  = YD[IP]
      A2  = 3*(Y[IP+1]-Y[IP])-2.0*YD[IP]-YD[IP+1]
      A3  = -2.0 * (Y[IP+1]-Y[IP]) + YD[IP] - YD[IP+1]

    with the first derivates as
      YD[IP]	= 0.5 * (Y[IP+1] - Y[IP-1]);
      YD[IP+1]	= 0.5 * (Y[IP+2] - Y[IP])

    the coefs becomes
      A0  = Y[IP]
      A1  = YD[IP]
	  =  0.5 * (Y[IP+1] - Y[IP-1]);
      A2  =  3.0 * (Y[IP+1]-Y[IP])-2.0*YD[IP]-YD[IP+1]
	  =  3.0 * (Y[IP+1] - Y[IP]) - 0.5 * 2.0 * (Y[IP+1] - Y[IP-1]) - 0.5 * (Y[IP+2] - Y[IP])
	  =  3.0 * Y[IP+1] - 3.0 * Y[IP] - Y[IP+1] + Y[IP-1] - 0.5 * Y[IP+2] + 0.5 * Y[IP]
	  = -0.5 * Y[IP+2] + 2.0 * Y[IP+1] - 2.5 * Y[IP] + Y[IP-1]
	  = Y[IP-1] + 2 * Y[IP+1] - 0.5 * (5.0 * Y[IP] + Y[IP+2])
      A3  = -2.0 * (Y[IP+1]-Y[IP]) + YD[IP] + YD[IP+1]
	  = -2.0 * Y[IP+1] + 2.0 * Y[IP] + 0.5 * (Y[IP+1] - Y[IP-1]) + 0.5 * (Y[IP+2] - Y[IP])
	  = -2.0 * Y[IP+1] + 2.0 * Y[IP] + 0.5 * Y[IP+1] - 0.5 * Y[IP-1] + 0.5 * Y[IP+2] - 0.5 * Y[IP]
	  =  0.5 * Y[IP+2] - 1.5 * Y[IP+1] + 1.5 * Y[IP] - 0.5 * Y[IP-1]
	  =  0.5 * (3.0 * (Y[IP] - Y[IP+1]) - Y[IP-1] + YP[IP+2])

    then interpolated data value is (horner rule)
      out = (((A3*x)+A2)*x+A1)*x+A0

    this gives parts of data points Y[IP-1] to Y[IP+2] of
      part	 x**3	 x**2	 x**1	 x**0
      Y[IP-1]	 -0.5	  1	 -0.5	 0
      Y[IP]	  1.5	 -2.5	  0	 1
      Y[IP+1]	 -1.5	  2	  0.5	 0
      Y[IP+2]	  0.5	 -0.5	  0	 0
  --------------------------------------------------------------------------------------------------
*/

// number of bits used to scale spline coefs
#define SPLINE_QUANTBITS    14
#define SPLINE_QUANTSCALE   (1L<<SPLINE_QUANTBITS)
#define SPLINE_8SHIFT	    (SPLINE_QUANTBITS-8)
#define SPLINE_16SHIFT	    (SPLINE_QUANTBITS)
// forces coefsset to unity gain
#define SPLINE_CLAMPFORUNITY
// log2(number) of precalculated splines (range is [4..14])
#define SPLINE_FRACBITS 10
#define SPLINE_LUTLEN (1L<<SPLINE_FRACBITS)

static int16_t CzCUBICSPLINE_lut[4 * (1L << SPLINE_FRACBITS)];

void CSoundFile_CzCUBICSPLINE(void)
{
    int _LIi;
    int _LLen;
    float _LFlen;
    float _LScale;
    static int init_done;

    if( init_done ) {
	return;
    }
    ++init_done;

    _LLen = (1L << SPLINE_FRACBITS);
    _LFlen = 1.0f / (float)_LLen;
    _LScale = (float)SPLINE_QUANTSCALE;
    for (_LIi = 0; _LIi < _LLen; _LIi++) {
	float _LCm1, _LC0, _LC1, _LC2;
	float _LX = ((float)_LIi) * _LFlen;
	int _LSum, _LIdx = _LIi << 2;

	_LCm1 =
	    (float)floor(0.5 + _LScale * (-0.5 * _LX * _LX * _LX +
		1.0 * _LX * _LX - 0.5 * _LX));
	_LC0 =
	    (float)floor(0.5 + _LScale * (1.5 * _LX * _LX * _LX -
		2.5 * _LX * _LX + 1.0));
	_LC1 =
	    (float)floor(0.5 + _LScale * (-1.5 * _LX * _LX * _LX +
		2.0 * _LX * _LX + 0.5 * _LX));
	_LC2 =
	    (float)floor(0.5 + _LScale * (0.5 * _LX * _LX * _LX -
		0.5 * _LX * _LX));
	CzCUBICSPLINE_lut[_LIdx + 0] =
	    (signed short)((_LCm1 < -_LScale) ? -_LScale : ((_LCm1 >
		    _LScale) ? _LScale : _LCm1));
	CzCUBICSPLINE_lut[_LIdx + 1] =
	    (signed short)((_LC0 < -_LScale) ? -_LScale : ((_LC0 >
		    _LScale) ? _LScale : _LC0));
	CzCUBICSPLINE_lut[_LIdx + 2] =
	    (signed short)((_LC1 < -_LScale) ? -_LScale : ((_LC1 >
		    _LScale) ? _LScale : _LC1));
	CzCUBICSPLINE_lut[_LIdx + 3] =
	    (signed short)((_LC2 < -_LScale) ? -_LScale : ((_LC2 >
		    _LScale) ? _LScale : _LC2));
#ifdef SPLINE_CLAMPFORUNITY
	_LSum =
	    CzCUBICSPLINE_lut[_LIdx + 0] + CzCUBICSPLINE_lut[_LIdx + 1] + CzCUBICSPLINE_lut[_LIdx + 2] + CzCUBICSPLINE_lut[_LIdx + 3];
	if (_LSum != SPLINE_QUANTSCALE) {
	    int _LMax = _LIdx;

	    if (CzCUBICSPLINE_lut[_LIdx + 1] > CzCUBICSPLINE_lut[_LMax])
		_LMax = _LIdx + 1;
	    if (CzCUBICSPLINE_lut[_LIdx + 2] > CzCUBICSPLINE_lut[_LMax])
		_LMax = _LIdx + 2;
	    if (CzCUBICSPLINE_lut[_LIdx + 3] > CzCUBICSPLINE_lut[_LMax])
		_LMax = _LIdx + 3;
	    CzCUBICSPLINE_lut[_LMax] += (SPLINE_QUANTSCALE - _LSum);
	}
#endif
    }
}

/*
  ------------------------------------------------------------------------------------------------
   fir interpolation doc,
    (derived from "an engineer's guide to fir digital filters", n.j. loy)

    calculate coefficients for ideal lowpass filter (with cutoff = fc in 0..1 (mapped to 0..nyquist))
      c[-N..N] = (i==0) ? fc : sin(fc*pi*i)/(pi*i)

    then apply selected window to coefficients
      c[-N..N] *= w(0..N)
    with n in 2*N and w(n) being a window function (see loy)

    then calculate gain and scale filter coefs to have unity gain.
  ------------------------------------------------------------------------------------------------
*/
// quantizer scale of window coefs
#define WFIR_QUANTBITS	    15
#define WFIR_QUANTSCALE	    (1L<<WFIR_QUANTBITS)
#define WFIR_8SHIFT	    (WFIR_QUANTBITS-8)
#define WFIR_16BITSHIFT	    (WFIR_QUANTBITS)
// log2(number)-1 of precalculated taps range is [4..12]
#define WFIR_FRACBITS	    10
#define WFIR_LUTLEN	    ((1L<<(WFIR_FRACBITS+1))+1)
// number of samples in window
#define WFIR_LOG2WIDTH	    3
#define WFIR_WIDTH	    (1L<<WFIR_LOG2WIDTH)
#define WFIR_SMPSPERWING    ((WFIR_WIDTH-1)>>1)
// cutoff (1.0 == pi/2)
#define WFIR_CUTOFF	    0.90f
// wfir type
#define WFIR_HANN	    0
#define WFIR_HAMMING	    1
#define WFIR_BLACKMANEXACT  2
#define WFIR_BLACKMAN3T61   3
#define WFIR_BLACKMAN3T67   4
#define WFIR_BLACKMAN4T92   5
#define WFIR_BLACKMAN4T74   6
#define WFIR_KAISER4T	    7
#define WFIR_TYPE	    WFIR_BLACKMANEXACT
// wfir help
#ifndef M_zPI
#define M_zPI		3.1415926535897932384626433832795
#endif
#define M_zEPS		1e-8
#define M_zBESSELEPS	1e-21

static float coef(int _PCnr, float _POfs, float _PCut, int _PWidth, int _PType)
{
    double _LWidthM1 = _PWidth - 1;
    double _LWidthM1Half = 0.5 * _LWidthM1;
    double _LPosU = ((double)_PCnr - _POfs);
    double _LPos = _LPosU - _LWidthM1Half;
    double _LPIdl = 2.0 * M_zPI / _LWidthM1;
    double _LWc, _LSi;
    if (fabs(_LPos) < M_zEPS)
    {
	_LWc = 1.0;
	_LSi = _PCut;
    } else
    {
	switch (_PType) {
	    case WFIR_HANN:
		_LWc = 0.50 - 0.50 * cos(_LPIdl * _LPosU);
		break;
	    case WFIR_HAMMING:
		_LWc = 0.54 - 0.46 * cos(_LPIdl * _LPosU);
		break;
	    case WFIR_BLACKMANEXACT:
		_LWc =
		    0.42 - 0.50 * cos(_LPIdl * _LPosU) +
		    0.08 * cos(2.0 * _LPIdl * _LPosU);
		break;
	    case WFIR_BLACKMAN3T61:
		_LWc =
		    0.44959 - 0.49364 * cos(_LPIdl * _LPosU) +
		    0.05677 * cos(2.0 * _LPIdl * _LPosU);
		break;
	    case WFIR_BLACKMAN3T67:
		_LWc =
		    0.42323 - 0.49755 * cos(_LPIdl * _LPosU) +
		    0.07922 * cos(2.0 * _LPIdl * _LPosU);
		break;
	    case WFIR_BLACKMAN4T92:
		_LWc =
		    0.35875 - 0.48829 * cos(_LPIdl * _LPosU) +
		    0.14128 * cos(2.0 * _LPIdl * _LPosU) -
		    0.01168 * cos(3.0 * _LPIdl * _LPosU);
		break;
	    case WFIR_BLACKMAN4T74:
		_LWc =
		    0.40217 - 0.49703 * cos(_LPIdl * _LPosU) +
		    0.09392 * cos(2.0 * _LPIdl * _LPosU) -
		    0.00183 * cos(3.0 * _LPIdl * _LPosU);
		break;
	    case WFIR_KAISER4T:
		_LWc =
		    0.40243 - 0.49804 * cos(_LPIdl * _LPosU) +
		    0.09831 * cos(2.0 * _LPIdl * _LPosU) -
		    0.00122 * cos(3.0 * _LPIdl * _LPosU);
		break;
	    default:
		_LWc = 1.0;
		break;
	}
	_LPos *= M_zPI;
	_LSi = sin(_PCut * _LPos) / _LPos;
    }
    return (float)(_LWc * _LSi);
}

static int16_t CzWINDOWEDFIR_lut[WFIR_LUTLEN * WFIR_WIDTH];

void CSoundFile_CzWINDOWEDFIR(void)
{
    int _LPcl;
    float _LPcllen;
    float _LNorm;
    float _LCut;
    float _LScale;
    static int init_done;

    if( init_done ) {
	return;
    }
    ++init_done;

    _LPcllen = (float)(1L << WFIR_FRACBITS);	// number of precalculated lines for 0..1 (-1..0)
    _LNorm = 1.0f / (float)(2.0f * _LPcllen);
    _LCut = WFIR_CUTOFF;
    _LScale = (float)WFIR_QUANTSCALE;

    for (_LPcl = 0; _LPcl < WFIR_LUTLEN; _LPcl++) {
	float _LGain, _LCoefs[WFIR_WIDTH];
	float _LOfs = ((float)_LPcl - _LPcllen) * _LNorm;
	int _LCc, _LIdx = _LPcl << WFIR_LOG2WIDTH;

	for (_LCc = 0, _LGain = 0.0f; _LCc < WFIR_WIDTH; _LCc++) {
	    _LGain += (_LCoefs[_LCc] =
		coef(_LCc, _LOfs, _LCut, WFIR_WIDTH, WFIR_TYPE));
	}
	_LGain = 1.0f / _LGain;
	for (_LCc = 0; _LCc < WFIR_WIDTH; _LCc++) {
	    float _LCoef =
		(float)floor(0.5 + _LScale * _LCoefs[_LCc] * _LGain);
	    CzWINDOWEDFIR_lut[_LIdx + _LCc] =
		(signed short)((_LCoef < -_LScale) ? -_LScale : ((_LCoef >
			_LScale) ? _LScale : _LCoef));
	}
    }
}

// ---------------------------------------------------------------------------
// MIXING MACROS
// ---------------------------------------------------------------------------

/////////////////////////////////////////////////////
// Mixing Macros

/**
**	Sample loop begin 8bit
*/
#define SNDMIX_BEGINSAMPLELOOP8	\
    register MODCHANNEL * const pChn = pChannel; \
    const int8_t *p = (const int8_t *)(pChn->pCurrentSample+pChn->nPos); \
    int *pvol = pbuffer; \
    nPos = pChn->nPosLo; \
    if (pChn->dwFlags & CHN_STEREO) { \
	p += pChn->nPos; \
    } \
    do {

/**
**	Sample loop begin 16bit
*/
#define SNDMIX_BEGINSAMPLELOOP16	\
    register MODCHANNEL * const pChn = pChannel; \
    const int16_t *p = (const int16_t *)(pChn->pCurrentSample+(pChn->nPos*2));\
    int *pvol = pbuffer; \
    nPos = pChn->nPosLo; \
    if (pChn->dwFlags & CHN_STEREO) { \
	p += pChn->nPos; \
    } \
    do {

/**
**	Sample loop end general
*/
#define SNDMIX_ENDSAMPLELOOP	\
	nPos += pChn->nInc; \
    } while (pvol < pbufmax); \
    pChn->nPos += nPos >> 16; \
    pChn->nPosLo = nPos & 0xFFFF;

#define SNDMIX_ENDSAMPLELOOP8	SNDMIX_ENDSAMPLELOOP
#define SNDMIX_ENDSAMPLELOOP16	SNDMIX_ENDSAMPLELOOP

//////////////////////////////////////////////////////////////////////////////
// Mono

// No interpolation
#define SNDMIX_GETMONOVOL8NOIDO\
    int vol = p[nPos >> 16] << 8;

#define SNDMIX_GETMONOVOL16NOIDO\
    int vol = p[nPos >> 16];

// Linear Interpolation
#define SNDMIX_GETMONOVOL8LINEAR\
    int poshi = nPos >> 16;\
    int poslo = (nPos >> 8) & 0xFF;\
    int srcvol = p[poshi];\
    int destvol = p[poshi+1];\
    int vol = (srcvol<<8) + ((int)(poslo * (destvol - srcvol)));

#define SNDMIX_GETMONOVOL16LINEAR\
    int poshi = nPos >> 16;\
    int poslo = (nPos >> 8) & 0xFF;\
    int srcvol = p[poshi];\
    int destvol = p[poshi+1];\
    int vol = srcvol + ((int)(poslo * (destvol - srcvol)) >> 8);

// spline interpolation (2 guard bits should be enough???)
#define SPLINE_FRACSHIFT ((16-SPLINE_FRACBITS)-2)
#define SPLINE_FRACMASK	 (((1L<<(16-SPLINE_FRACSHIFT))-1)&~3)

#define SNDMIX_GETMONOVOL8SPLINE \
    int poshi	= nPos >> 16; \
    int poslo	= (nPos >> SPLINE_FRACSHIFT) & SPLINE_FRACMASK; \
    int vol	= (CzCUBICSPLINE_lut[poslo  ]*(int)p[poshi-1] + \
		   CzCUBICSPLINE_lut[poslo+1]*(int)p[poshi  ] + \
		   CzCUBICSPLINE_lut[poslo+3]*(int)p[poshi+2] + \
		   CzCUBICSPLINE_lut[poslo+2]*(int)p[poshi+1]) >> SPLINE_8SHIFT;

#define SNDMIX_GETMONOVOL16SPLINE \
    int poshi	= nPos >> 16; \
    int poslo	= (nPos >> SPLINE_FRACSHIFT) & SPLINE_FRACMASK; \
    int vol	= (CzCUBICSPLINE_lut[poslo  ]*(int)p[poshi-1] + \
		   CzCUBICSPLINE_lut[poslo+1]*(int)p[poshi  ] + \
		   CzCUBICSPLINE_lut[poslo+3]*(int)p[poshi+2] + \
		   CzCUBICSPLINE_lut[poslo+2]*(int)p[poshi+1]) >> SPLINE_16SHIFT;

// fir interpolation
#define WFIR_FRACSHIFT	(16-(WFIR_FRACBITS+1+WFIR_LOG2WIDTH))
#define WFIR_FRACMASK	((((1L<<(17-WFIR_FRACSHIFT))-1)&~((1L<<WFIR_LOG2WIDTH)-1)))
#define WFIR_FRACHALVE	(1L<<(16-(WFIR_FRACBITS+2)))

#define SNDMIX_GETMONOVOL8FIRFILTER \
    int poshi  = nPos >> 16;\
    int poslo  = (nPos & 0xFFFF);\
    int firidx = ((poslo+WFIR_FRACHALVE)>>WFIR_FRACSHIFT) & WFIR_FRACMASK; \
    int vol    = (CzWINDOWEDFIR_lut[firidx+0]*(int)p[poshi+1-4]);  \
	vol   += (CzWINDOWEDFIR_lut[firidx+1]*(int)p[poshi+2-4]);  \
	vol   += (CzWINDOWEDFIR_lut[firidx+2]*(int)p[poshi+3-4]);  \
	vol   += (CzWINDOWEDFIR_lut[firidx+3]*(int)p[poshi+4-4]);  \
	vol   += (CzWINDOWEDFIR_lut[firidx+4]*(int)p[poshi+5-4]);  \
	vol   += (CzWINDOWEDFIR_lut[firidx+5]*(int)p[poshi+6-4]);  \
	vol   += (CzWINDOWEDFIR_lut[firidx+6]*(int)p[poshi+7-4]);  \
	vol   += (CzWINDOWEDFIR_lut[firidx+7]*(int)p[poshi+8-4]);  \
	vol  >>= WFIR_8SHIFT;

#define SNDMIX_GETMONOVOL16FIRFILTER \
    int poshi  = nPos >> 16;\
    int poslo  = (nPos & 0xFFFF);\
    int firidx = ((poslo+WFIR_FRACHALVE)>>WFIR_FRACSHIFT) & WFIR_FRACMASK; \
    int vol; \
    int vol1; \
    int vol2; \
    vol1   = (CzWINDOWEDFIR_lut[firidx+0]*(int)p[poshi+1-4]);  \
    vol1  += (CzWINDOWEDFIR_lut[firidx+1]*(int)p[poshi+2-4]);  \
    vol1  += (CzWINDOWEDFIR_lut[firidx+2]*(int)p[poshi+3-4]);  \
    vol1  += (CzWINDOWEDFIR_lut[firidx+3]*(int)p[poshi+4-4]);  \
    vol2   = (CzWINDOWEDFIR_lut[firidx+4]*(int)p[poshi+5-4]);  \
    vol2  += (CzWINDOWEDFIR_lut[firidx+5]*(int)p[poshi+6-4]);  \
    vol2  += (CzWINDOWEDFIR_lut[firidx+6]*(int)p[poshi+7-4]);  \
    vol2  += (CzWINDOWEDFIR_lut[firidx+7]*(int)p[poshi+8-4]);  \
    vol    = ((vol1>>1)+(vol2>>1)) >> (WFIR_16BITSHIFT-1);

/////////////////////////////////////////////////////////////////////////////
// Stereo

// No interpolation
#define SNDMIX_GETSTEREOVOL8NOIDO\
    int vol_l = p[(nPos>>16)*2] << 8;\
    int vol_r = p[(nPos>>16)*2+1] << 8;

#define SNDMIX_GETSTEREOVOL16NOIDO\
    int vol_l = p[(nPos>>16)*2];\
    int vol_r = p[(nPos>>16)*2+1];

// Linear Interpolation
#define SNDMIX_GETSTEREOVOL8LINEAR\
    int poshi = nPos >> 16;\
    int poslo = (nPos >> 8) & 0xFF;\
    int srcvol_l = p[poshi*2];\
    int vol_l = (srcvol_l<<8) + ((int)(poslo * (p[poshi*2+2] - srcvol_l)));\
    int srcvol_r = p[poshi*2+1];\
    int vol_r = (srcvol_r<<8) + ((int)(poslo * (p[poshi*2+3] - srcvol_r)));

#define SNDMIX_GETSTEREOVOL16LINEAR\
    int poshi = nPos >> 16;\
    int poslo = (nPos >> 8) & 0xFF;\
    int srcvol_l = p[poshi*2];\
    int vol_l = srcvol_l + ((int)(poslo * (p[poshi*2+2] - srcvol_l)) >> 8);\
    int srcvol_r = p[poshi*2+1];\
    int vol_r = srcvol_r + ((int)(poslo * (p[poshi*2+3] - srcvol_r)) >> 8);\

// Spline Interpolation
#define SNDMIX_GETSTEREOVOL8SPLINE \
    int poshi	= nPos >> 16; \
    int poslo	= (nPos >> SPLINE_FRACSHIFT) & SPLINE_FRACMASK; \
    int vol_l	= (CzCUBICSPLINE_lut[poslo  ]*(int)p[(poshi-1)*2  ] + \
		   CzCUBICSPLINE_lut[poslo+1]*(int)p[(poshi  )*2  ] + \
		   CzCUBICSPLINE_lut[poslo+2]*(int)p[(poshi+1)*2  ] + \
		   CzCUBICSPLINE_lut[poslo+3]*(int)p[(poshi+2)*2  ]) >> SPLINE_8SHIFT; \
    int vol_r	= (CzCUBICSPLINE_lut[poslo  ]*(int)p[(poshi-1)*2+1] + \
		   CzCUBICSPLINE_lut[poslo+1]*(int)p[(poshi  )*2+1] + \
		   CzCUBICSPLINE_lut[poslo+2]*(int)p[(poshi+1)*2+1] + \
		   CzCUBICSPLINE_lut[poslo+3]*(int)p[(poshi+2)*2+1]) >> SPLINE_8SHIFT;

#define SNDMIX_GETSTEREOVOL16SPLINE \
    int poshi	= nPos >> 16; \
    int poslo	= (nPos >> SPLINE_FRACSHIFT) & SPLINE_FRACMASK; \
    int vol_l	= (CzCUBICSPLINE_lut[poslo  ]*(int)p[(poshi-1)*2  ] + \
		   CzCUBICSPLINE_lut[poslo+1]*(int)p[(poshi  )*2  ] + \
		   CzCUBICSPLINE_lut[poslo+2]*(int)p[(poshi+1)*2  ] + \
		   CzCUBICSPLINE_lut[poslo+3]*(int)p[(poshi+2)*2  ]) >> SPLINE_16SHIFT; \
    int vol_r	= (CzCUBICSPLINE_lut[poslo  ]*(int)p[(poshi-1)*2+1] + \
		   CzCUBICSPLINE_lut[poslo+1]*(int)p[(poshi  )*2+1] + \
		   CzCUBICSPLINE_lut[poslo+2]*(int)p[(poshi+1)*2+1] + \
		   CzCUBICSPLINE_lut[poslo+3]*(int)p[(poshi+2)*2+1]) >> SPLINE_16SHIFT;

// fir interpolation
#define SNDMIX_GETSTEREOVOL8FIRFILTER \
    int poshi	= nPos >> 16;\
    int poslo	= (nPos & 0xFFFF);\
    int firidx	= ((poslo+WFIR_FRACHALVE)>>WFIR_FRACSHIFT) & WFIR_FRACMASK; \
    int vol_l; \
    int vol_r; \
    vol_l   = (CzWINDOWEDFIR_lut[firidx+0]*(int)p[(poshi+1-4)*2  ]);   \
    vol_l  += (CzWINDOWEDFIR_lut[firidx+1]*(int)p[(poshi+2-4)*2  ]);   \
    vol_l  += (CzWINDOWEDFIR_lut[firidx+2]*(int)p[(poshi+3-4)*2  ]);   \
    vol_l  += (CzWINDOWEDFIR_lut[firidx+3]*(int)p[(poshi+4-4)*2  ]);   \
    vol_l  += (CzWINDOWEDFIR_lut[firidx+4]*(int)p[(poshi+5-4)*2  ]);   \
    vol_l  += (CzWINDOWEDFIR_lut[firidx+5]*(int)p[(poshi+6-4)*2  ]);   \
    vol_l  += (CzWINDOWEDFIR_lut[firidx+6]*(int)p[(poshi+7-4)*2  ]);   \
    vol_l  += (CzWINDOWEDFIR_lut[firidx+7]*(int)p[(poshi+8-4)*2  ]);   \
    vol_l >>= WFIR_8SHIFT; \
    vol_r   = (CzWINDOWEDFIR_lut[firidx+0]*(int)p[(poshi+1-4)*2+1]);   \
    vol_r  += (CzWINDOWEDFIR_lut[firidx+1]*(int)p[(poshi+2-4)*2+1]);   \
    vol_r  += (CzWINDOWEDFIR_lut[firidx+2]*(int)p[(poshi+3-4)*2+1]);   \
    vol_r  += (CzWINDOWEDFIR_lut[firidx+3]*(int)p[(poshi+4-4)*2+1]);   \
    vol_r  += (CzWINDOWEDFIR_lut[firidx+4]*(int)p[(poshi+5-4)*2+1]);   \
    vol_r  += (CzWINDOWEDFIR_lut[firidx+5]*(int)p[(poshi+6-4)*2+1]);   \
    vol_r  += (CzWINDOWEDFIR_lut[firidx+6]*(int)p[(poshi+7-4)*2+1]);   \
    vol_r  += (CzWINDOWEDFIR_lut[firidx+7]*(int)p[(poshi+8-4)*2+1]);   \
    vol_r >>= WFIR_8SHIFT;

#define SNDMIX_GETSTEREOVOL16FIRFILTER \
    int poshi	= nPos >> 16;\
    int poslo	= (nPos & 0xFFFF);\
    int firidx	= ((poslo+WFIR_FRACHALVE)>>WFIR_FRACSHIFT) & WFIR_FRACMASK; \
    int vol1_l; \
    int vol2_l; \
    int vol_l; \
    int vol1_r; \
    int vol2_r; \
    int vol_r; \
    vol1_l  = (CzWINDOWEDFIR_lut[firidx+0]*(int)p[(poshi+1-4)*2  ]);   \
    vol1_l += (CzWINDOWEDFIR_lut[firidx+1]*(int)p[(poshi+2-4)*2  ]);   \
    vol1_l += (CzWINDOWEDFIR_lut[firidx+2]*(int)p[(poshi+3-4)*2  ]);   \
    vol1_l += (CzWINDOWEDFIR_lut[firidx+3]*(int)p[(poshi+4-4)*2  ]);   \
    vol2_l  = (CzWINDOWEDFIR_lut[firidx+4]*(int)p[(poshi+5-4)*2  ]);   \
    vol2_l += (CzWINDOWEDFIR_lut[firidx+5]*(int)p[(poshi+6-4)*2  ]);   \
    vol2_l += (CzWINDOWEDFIR_lut[firidx+6]*(int)p[(poshi+7-4)*2  ]);   \
    vol2_l += (CzWINDOWEDFIR_lut[firidx+7]*(int)p[(poshi+8-4)*2  ]);   \
    vol_l   = ((vol1_l>>1)+(vol2_l>>1)) >> (WFIR_16BITSHIFT-1); \
    vol1_r  = (CzWINDOWEDFIR_lut[firidx+0]*(int)p[(poshi+1-4)*2+1]);   \
    vol1_r += (CzWINDOWEDFIR_lut[firidx+1]*(int)p[(poshi+2-4)*2+1]);   \
    vol1_r += (CzWINDOWEDFIR_lut[firidx+2]*(int)p[(poshi+3-4)*2+1]);   \
    vol1_r += (CzWINDOWEDFIR_lut[firidx+3]*(int)p[(poshi+4-4)*2+1]);   \
    vol2_r  = (CzWINDOWEDFIR_lut[firidx+4]*(int)p[(poshi+5-4)*2+1]);   \
    vol2_r += (CzWINDOWEDFIR_lut[firidx+5]*(int)p[(poshi+6-4)*2+1]);   \
    vol2_r += (CzWINDOWEDFIR_lut[firidx+6]*(int)p[(poshi+7-4)*2+1]);   \
    vol2_r += (CzWINDOWEDFIR_lut[firidx+7]*(int)p[(poshi+8-4)*2+1]);   \
    vol_r   = ((vol1_r>>1)+(vol2_r>>1)) >> (WFIR_16BITSHIFT-1);

/////////////////////////////////////////////////////////////////////////////

#define SNDMIX_STOREMONOVOL\
    pvol[0] += vol * pChn->nRightVol;\
    pvol[1] += vol * pChn->nLeftVol;\
    pvol += 2;

#define SNDMIX_STORESTEREOVOL\
    pvol[0] += vol_l * pChn->nRightVol;\
    pvol[1] += vol_r * pChn->nLeftVol;\
    pvol += 2;

#define SNDMIX_STOREFASTMONOVOL\
    { int v = vol * pChn->nRightVol;\
    pvol[0] += v;\
    pvol[1] += v;\
    } \
    pvol += 2;

#define SNDMIX_RAMPMONOVOL\
    nRampLeftVol += pChn->nLeftRamp;\
    nRampRightVol += pChn->nRightRamp;\
    pvol[0] += vol * (nRampRightVol >> VOLUMERAMPPRECISION);\
    pvol[1] += vol * (nRampLeftVol >> VOLUMERAMPPRECISION);\
    pvol += 2;

#define SNDMIX_RAMPFASTMONOVOL\
    nRampRightVol += pChn->nRightRamp;\
    { int fastvol = vol * (nRampRightVol >> VOLUMERAMPPRECISION);\
    pvol[0] += fastvol;\
    pvol[1] += fastvol;\
    } \
    pvol += 2;

#define SNDMIX_RAMPSTEREOVOL\
    nRampLeftVol += pChn->nLeftRamp;\
    nRampRightVol += pChn->nRightRamp;\
    pvol[0] += vol_l * (nRampRightVol >> VOLUMERAMPPRECISION);\
    pvol[1] += vol_r * (nRampLeftVol >> VOLUMERAMPPRECISION);\
    pvol += 2;

///////////////////////////////////////////////////
// Resonant Filters

// Mono
#define MIX_BEGIN_FILTER\
    int fy1 = pChannel->nFilter_Y1;\
    int fy2 = pChannel->nFilter_Y2;\

#define MIX_END_FILTER\
    pChannel->nFilter_Y1 = fy1;\
    pChannel->nFilter_Y2 = fy2;

#define SNDMIX_PROCESSFILTER\
    vol = (vol * pChn->nFilter_A0 + fy1 * pChn->nFilter_B0 + fy2 * pChn->nFilter_B1 + 4096) >> 13;\
    fy2 = fy1;\
    fy1 = vol;\

// Stereo
#define MIX_BEGIN_STEREO_FILTER\
    int fy1 = pChannel->nFilter_Y1;\
    int fy2 = pChannel->nFilter_Y2;\
    int fy3 = pChannel->nFilter_Y3;\
    int fy4 = pChannel->nFilter_Y4;\

#define MIX_END_STEREO_FILTER\
    pChannel->nFilter_Y1 = fy1;\
    pChannel->nFilter_Y2 = fy2;\
    pChannel->nFilter_Y3 = fy3;\
    pChannel->nFilter_Y4 = fy4;\

#define SNDMIX_PROCESSSTEREOFILTER\
    vol_l = (vol_l * pChn->nFilter_A0 + fy1 * pChn->nFilter_B0 + fy2 * pChn->nFilter_B1 + 4096) >> 13;\
    vol_r = (vol_r * pChn->nFilter_A0 + fy3 * pChn->nFilter_B0 + fy4 * pChn->nFilter_B1 + 4096) >> 13;\
    fy2 = fy1; fy1 = vol_l;\
    fy4 = fy3; fy3 = vol_r;\

//////////////////////////////////////////////////////////
// Interfaces

typedef void (MPPASMCALL* LPMIXINTERFACE) (MODCHANNEL *, int *, int *);

#define BEGIN_MIX_INTERFACE(func)\
    void MPPASMCALL func(MODCHANNEL *pChannel, int *pbuffer, int *pbufmax)\
    {\
	long nPos;

#define END_MIX_INTERFACE()\
	SNDMIX_ENDSAMPLELOOP\
    }

// Volume Ramps
#define BEGIN_RAMPMIX_INTERFACE(func)\
    BEGIN_MIX_INTERFACE(func)\
	long nRampRightVol = pChannel->nRampRightVol;\
	long nRampLeftVol = pChannel->nRampLeftVol;

#define END_RAMPMIX_INTERFACE()\
	SNDMIX_ENDSAMPLELOOP\
	pChannel->nRampRightVol = nRampRightVol;\
	pChannel->nRightVol = nRampRightVol >> VOLUMERAMPPRECISION;\
	pChannel->nRampLeftVol = nRampLeftVol;\
	pChannel->nLeftVol = nRampLeftVol >> VOLUMERAMPPRECISION;\
    }

#define BEGIN_FASTRAMPMIX_INTERFACE(func)\
    BEGIN_MIX_INTERFACE(func)\
	long nRampRightVol = pChannel->nRampRightVol;

#define END_FASTRAMPMIX_INTERFACE()\
	SNDMIX_ENDSAMPLELOOP\
	pChannel->nRampRightVol = nRampRightVol;\
	pChannel->nRampLeftVol = nRampRightVol;\
	pChannel->nRightVol = nRampRightVol >> VOLUMERAMPPRECISION;\
	pChannel->nLeftVol = pChannel->nRightVol;\
    }

// Mono Resonant Filters
#define BEGIN_MIX_FLT_INTERFACE(func)\
    BEGIN_MIX_INTERFACE(func)\
    MIX_BEGIN_FILTER

#define END_MIX_FLT_INTERFACE()\
    SNDMIX_ENDSAMPLELOOP\
    MIX_END_FILTER\
    }

#define BEGIN_RAMPMIX_FLT_INTERFACE(func)\
    BEGIN_MIX_INTERFACE(func)\
	long nRampRightVol = pChannel->nRampRightVol;\
	long nRampLeftVol = pChannel->nRampLeftVol;\
	MIX_BEGIN_FILTER

#define END_RAMPMIX_FLT_INTERFACE()\
	SNDMIX_ENDSAMPLELOOP\
	MIX_END_FILTER\
	pChannel->nRampRightVol = nRampRightVol;\
	pChannel->nRightVol = nRampRightVol >> VOLUMERAMPPRECISION;\
	pChannel->nRampLeftVol = nRampLeftVol;\
	pChannel->nLeftVol = nRampLeftVol >> VOLUMERAMPPRECISION;\
    }

// Stereo Resonant Filters
#define BEGIN_MIX_STFLT_INTERFACE(func)\
    BEGIN_MIX_INTERFACE(func)\
    MIX_BEGIN_STEREO_FILTER

#define END_MIX_STFLT_INTERFACE()\
    SNDMIX_ENDSAMPLELOOP\
    MIX_END_STEREO_FILTER\
    }

#define BEGIN_RAMPMIX_STFLT_INTERFACE(func)\
    BEGIN_MIX_INTERFACE(func)\
	long nRampRightVol = pChannel->nRampRightVol;\
	long nRampLeftVol = pChannel->nRampLeftVol;\
	MIX_BEGIN_STEREO_FILTER

#define END_RAMPMIX_STFLT_INTERFACE()\
	SNDMIX_ENDSAMPLELOOP\
	MIX_END_STEREO_FILTER\
	pChannel->nRampRightVol = nRampRightVol;\
	pChannel->nRightVol = nRampRightVol >> VOLUMERAMPPRECISION;\
	pChannel->nRampLeftVol = nRampLeftVol;\
	pChannel->nLeftVol = nRampLeftVol >> VOLUMERAMPPRECISION;\
    }

/////////////////////////////////////////////////////
//

static void MPPASMCALL X86_InitMixBuffer(int *pBuffer, unsigned nSamples);
static void MPPASMCALL X86_EndChannelOfs(MODCHANNEL * pChannel, int *pBuffer,
    unsigned nSamples);

/////////////////////////////////////////////////////
// Mono samples functions

BEGIN_MIX_INTERFACE(Mono8BitMix)
SNDMIX_BEGINSAMPLELOOP8 SNDMIX_GETMONOVOL8NOIDO SNDMIX_STOREMONOVOL
END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Mono16BitMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16NOIDO SNDMIX_STOREMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Mono8BitLinearMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8LINEAR SNDMIX_STOREMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Mono16BitLinearMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16LINEAR SNDMIX_STOREMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Mono8BitSplineMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8SPLINE SNDMIX_STOREMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Mono16BitSplineMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16SPLINE SNDMIX_STOREMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Mono8BitFirFilterMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8FIRFILTER SNDMIX_STOREMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Mono16BitFirFilterMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16FIRFILTER SNDMIX_STOREMONOVOL END_MIX_INTERFACE()

// Volume Ramps
BEGIN_RAMPMIX_INTERFACE(Mono8BitRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8NOIDO SNDMIX_RAMPMONOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Mono16BitRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16NOIDO SNDMIX_RAMPMONOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Mono8BitLinearRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8LINEAR SNDMIX_RAMPMONOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Mono16BitLinearRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16LINEAR SNDMIX_RAMPMONOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Mono8BitSplineRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8SPLINE SNDMIX_RAMPMONOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Mono16BitSplineRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16SPLINE SNDMIX_RAMPMONOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Mono8BitFirFilterRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8FIRFILTER SNDMIX_RAMPMONOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Mono16BitFirFilterRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16FIRFILTER SNDMIX_RAMPMONOVOL END_RAMPMIX_INTERFACE()

//////////////////////////////////////////////////////
// Fast mono mix for leftvol=rightvol (1 less imul)
BEGIN_MIX_INTERFACE(FastMono8BitMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8NOIDO SNDMIX_STOREFASTMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(FastMono16BitMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16NOIDO SNDMIX_STOREFASTMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(FastMono8BitLinearMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8LINEAR SNDMIX_STOREFASTMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(FastMono16BitLinearMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16LINEAR SNDMIX_STOREFASTMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(FastMono8BitSplineMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8SPLINE SNDMIX_STOREFASTMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(FastMono16BitSplineMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16SPLINE SNDMIX_STOREFASTMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(FastMono8BitFirFilterMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8FIRFILTER SNDMIX_STOREFASTMONOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(FastMono16BitFirFilterMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16FIRFILTER SNDMIX_STOREFASTMONOVOL END_MIX_INTERFACE()

// Fast Ramps
BEGIN_FASTRAMPMIX_INTERFACE(FastMono8BitRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8NOIDO SNDMIX_RAMPFASTMONOVOL END_FASTRAMPMIX_INTERFACE()
 BEGIN_FASTRAMPMIX_INTERFACE(FastMono16BitRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16NOIDO SNDMIX_RAMPFASTMONOVOL END_FASTRAMPMIX_INTERFACE()
 BEGIN_FASTRAMPMIX_INTERFACE(FastMono8BitLinearRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8LINEAR SNDMIX_RAMPFASTMONOVOL END_FASTRAMPMIX_INTERFACE()
 BEGIN_FASTRAMPMIX_INTERFACE(FastMono16BitLinearRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETMONOVOL16LINEAR SNDMIX_RAMPFASTMONOVOL
END_FASTRAMPMIX_INTERFACE()
 BEGIN_FASTRAMPMIX_INTERFACE(FastMono8BitSplineRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8SPLINE SNDMIX_RAMPFASTMONOVOL END_FASTRAMPMIX_INTERFACE()
 BEGIN_FASTRAMPMIX_INTERFACE(FastMono16BitSplineRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETMONOVOL16SPLINE SNDMIX_RAMPFASTMONOVOL
END_FASTRAMPMIX_INTERFACE()
 BEGIN_FASTRAMPMIX_INTERFACE(FastMono8BitFirFilterRampMix)
    SNDMIX_BEGINSAMPLELOOP8 SNDMIX_GETMONOVOL8FIRFILTER SNDMIX_RAMPFASTMONOVOL
END_FASTRAMPMIX_INTERFACE()
 BEGIN_FASTRAMPMIX_INTERFACE(FastMono16BitFirFilterRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETMONOVOL16FIRFILTER
    SNDMIX_RAMPFASTMONOVOL END_FASTRAMPMIX_INTERFACE()

//////////////////////////////////////////////////////
// Stereo samples
BEGIN_MIX_INTERFACE(Stereo8BitMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8NOIDO SNDMIX_STORESTEREOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Stereo16BitMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16NOIDO SNDMIX_STORESTEREOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Stereo8BitLinearMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8LINEAR SNDMIX_STORESTEREOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Stereo16BitLinearMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16LINEAR SNDMIX_STORESTEREOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Stereo8BitSplineMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8SPLINE SNDMIX_STORESTEREOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Stereo16BitSplineMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16SPLINE SNDMIX_STORESTEREOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Stereo8BitFirFilterMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8FIRFILTER SNDMIX_STORESTEREOVOL END_MIX_INTERFACE()
 BEGIN_MIX_INTERFACE(Stereo16BitFirFilterMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16FIRFILTER SNDMIX_STORESTEREOVOL END_MIX_INTERFACE()

// Volume Ramps
BEGIN_RAMPMIX_INTERFACE(Stereo8BitRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8NOIDO SNDMIX_RAMPSTEREOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Stereo16BitRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16NOIDO SNDMIX_RAMPSTEREOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Stereo8BitLinearRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8LINEAR SNDMIX_RAMPSTEREOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Stereo16BitLinearRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16LINEAR SNDMIX_RAMPSTEREOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Stereo8BitSplineRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8SPLINE SNDMIX_RAMPSTEREOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Stereo16BitSplineRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16SPLINE SNDMIX_RAMPSTEREOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Stereo8BitFirFilterRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8FIRFILTER SNDMIX_RAMPSTEREOVOL END_RAMPMIX_INTERFACE()
 BEGIN_RAMPMIX_INTERFACE(Stereo16BitFirFilterRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16FIRFILTER SNDMIX_RAMPSTEREOVOL END_RAMPMIX_INTERFACE()

//////////////////////////////////////////////////////
// Resonant Filter Mix
#ifndef MODPLUG_NO_FILTER
// Mono Filter Mix
BEGIN_MIX_FLT_INTERFACE(FilterMono8BitMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8NOIDO SNDMIX_PROCESSFILTER SNDMIX_STOREMONOVOL
END_MIX_FLT_INTERFACE()
 BEGIN_MIX_FLT_INTERFACE(FilterMono16BitMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16NOIDO SNDMIX_PROCESSFILTER SNDMIX_STOREMONOVOL
END_MIX_FLT_INTERFACE()
 BEGIN_MIX_FLT_INTERFACE(FilterMono8BitLinearMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8LINEAR SNDMIX_PROCESSFILTER SNDMIX_STOREMONOVOL
END_MIX_FLT_INTERFACE()
 BEGIN_MIX_FLT_INTERFACE(FilterMono16BitLinearMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16LINEAR SNDMIX_PROCESSFILTER SNDMIX_STOREMONOVOL
END_MIX_FLT_INTERFACE()
 BEGIN_MIX_FLT_INTERFACE(FilterMono8BitSplineMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8SPLINE SNDMIX_PROCESSFILTER SNDMIX_STOREMONOVOL
END_MIX_FLT_INTERFACE()
 BEGIN_MIX_FLT_INTERFACE(FilterMono16BitSplineMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16SPLINE SNDMIX_PROCESSFILTER SNDMIX_STOREMONOVOL
END_MIX_FLT_INTERFACE()
 BEGIN_MIX_FLT_INTERFACE(FilterMono8BitFirFilterMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8FIRFILTER SNDMIX_PROCESSFILTER SNDMIX_STOREMONOVOL
END_MIX_FLT_INTERFACE()
 BEGIN_MIX_FLT_INTERFACE(FilterMono16BitFirFilterMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16FIRFILTER SNDMIX_PROCESSFILTER SNDMIX_STOREMONOVOL
END_MIX_FLT_INTERFACE()
// Filter + Ramp
BEGIN_RAMPMIX_FLT_INTERFACE(FilterMono8BitRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETMONOVOL8NOIDO SNDMIX_PROCESSFILTER SNDMIX_RAMPMONOVOL
END_RAMPMIX_FLT_INTERFACE()
 BEGIN_RAMPMIX_FLT_INTERFACE(FilterMono16BitRampMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETMONOVOL16NOIDO SNDMIX_PROCESSFILTER SNDMIX_RAMPMONOVOL
END_RAMPMIX_FLT_INTERFACE()
 BEGIN_RAMPMIX_FLT_INTERFACE(FilterMono8BitLinearRampMix)
    SNDMIX_BEGINSAMPLELOOP8 SNDMIX_GETMONOVOL8LINEAR SNDMIX_PROCESSFILTER
    SNDMIX_RAMPMONOVOL END_RAMPMIX_FLT_INTERFACE()
 BEGIN_RAMPMIX_FLT_INTERFACE(FilterMono16BitLinearRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETMONOVOL16LINEAR SNDMIX_PROCESSFILTER
    SNDMIX_RAMPMONOVOL END_RAMPMIX_FLT_INTERFACE()
 BEGIN_RAMPMIX_FLT_INTERFACE(FilterMono8BitSplineRampMix)
    SNDMIX_BEGINSAMPLELOOP8 SNDMIX_GETMONOVOL8SPLINE SNDMIX_PROCESSFILTER
    SNDMIX_RAMPMONOVOL END_RAMPMIX_FLT_INTERFACE()
 BEGIN_RAMPMIX_FLT_INTERFACE(FilterMono16BitSplineRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETMONOVOL16SPLINE SNDMIX_PROCESSFILTER
    SNDMIX_RAMPMONOVOL END_RAMPMIX_FLT_INTERFACE()
 BEGIN_RAMPMIX_FLT_INTERFACE(FilterMono8BitFirFilterRampMix)
    SNDMIX_BEGINSAMPLELOOP8 SNDMIX_GETMONOVOL8FIRFILTER SNDMIX_PROCESSFILTER
    SNDMIX_RAMPMONOVOL END_RAMPMIX_FLT_INTERFACE()
 BEGIN_RAMPMIX_FLT_INTERFACE(FilterMono16BitFirFilterRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETMONOVOL16FIRFILTER SNDMIX_PROCESSFILTER
    SNDMIX_RAMPMONOVOL END_RAMPMIX_FLT_INTERFACE()

// Stereo Filter Mix
BEGIN_MIX_STFLT_INTERFACE(FilterStereo8BitMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8NOIDO SNDMIX_PROCESSSTEREOFILTER SNDMIX_STORESTEREOVOL
END_MIX_STFLT_INTERFACE()
 BEGIN_MIX_STFLT_INTERFACE(FilterStereo16BitMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16NOIDO SNDMIX_PROCESSSTEREOFILTER SNDMIX_STORESTEREOVOL
END_MIX_STFLT_INTERFACE()
 BEGIN_MIX_STFLT_INTERFACE(FilterStereo8BitLinearMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8LINEAR SNDMIX_PROCESSSTEREOFILTER SNDMIX_STORESTEREOVOL
END_MIX_STFLT_INTERFACE()
 BEGIN_MIX_STFLT_INTERFACE(FilterStereo16BitLinearMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16LINEAR SNDMIX_PROCESSSTEREOFILTER
    SNDMIX_STORESTEREOVOL END_MIX_STFLT_INTERFACE()
 BEGIN_MIX_STFLT_INTERFACE(FilterStereo8BitSplineMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8SPLINE SNDMIX_PROCESSSTEREOFILTER SNDMIX_STORESTEREOVOL
END_MIX_STFLT_INTERFACE()
 BEGIN_MIX_STFLT_INTERFACE(FilterStereo16BitSplineMix) SNDMIX_BEGINSAMPLELOOP16
    SNDMIX_GETSTEREOVOL16SPLINE SNDMIX_PROCESSSTEREOFILTER
    SNDMIX_STORESTEREOVOL END_MIX_STFLT_INTERFACE()
 BEGIN_MIX_STFLT_INTERFACE(FilterStereo8BitFirFilterMix)
    SNDMIX_BEGINSAMPLELOOP8 SNDMIX_GETSTEREOVOL8FIRFILTER
    SNDMIX_PROCESSSTEREOFILTER SNDMIX_STORESTEREOVOL END_MIX_STFLT_INTERFACE()
 BEGIN_MIX_STFLT_INTERFACE(FilterStereo16BitFirFilterMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETSTEREOVOL16FIRFILTER
    SNDMIX_PROCESSSTEREOFILTER SNDMIX_STORESTEREOVOL END_MIX_STFLT_INTERFACE()
// Stereo Filter + Ramp
BEGIN_RAMPMIX_STFLT_INTERFACE(FilterStereo8BitRampMix) SNDMIX_BEGINSAMPLELOOP8
    SNDMIX_GETSTEREOVOL8NOIDO SNDMIX_PROCESSSTEREOFILTER SNDMIX_RAMPSTEREOVOL
END_RAMPMIX_STFLT_INTERFACE()
 BEGIN_RAMPMIX_STFLT_INTERFACE(FilterStereo16BitRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETSTEREOVOL16NOIDO
    SNDMIX_PROCESSSTEREOFILTER SNDMIX_RAMPSTEREOVOL
END_RAMPMIX_STFLT_INTERFACE()
 BEGIN_RAMPMIX_STFLT_INTERFACE(FilterStereo8BitLinearRampMix)
    SNDMIX_BEGINSAMPLELOOP8 SNDMIX_GETSTEREOVOL8LINEAR
    SNDMIX_PROCESSSTEREOFILTER SNDMIX_RAMPSTEREOVOL
END_RAMPMIX_STFLT_INTERFACE()
 BEGIN_RAMPMIX_STFLT_INTERFACE(FilterStereo16BitLinearRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETSTEREOVOL16LINEAR
    SNDMIX_PROCESSSTEREOFILTER SNDMIX_RAMPSTEREOVOL
END_RAMPMIX_STFLT_INTERFACE()
 BEGIN_RAMPMIX_STFLT_INTERFACE(FilterStereo8BitSplineRampMix)
    SNDMIX_BEGINSAMPLELOOP8 SNDMIX_GETSTEREOVOL8SPLINE
    SNDMIX_PROCESSSTEREOFILTER SNDMIX_RAMPSTEREOVOL
END_RAMPMIX_STFLT_INTERFACE()
 BEGIN_RAMPMIX_STFLT_INTERFACE(FilterStereo16BitSplineRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETSTEREOVOL16SPLINE
    SNDMIX_PROCESSSTEREOFILTER SNDMIX_RAMPSTEREOVOL
END_RAMPMIX_STFLT_INTERFACE()
 BEGIN_RAMPMIX_STFLT_INTERFACE(FilterStereo8BitFirFilterRampMix)
    SNDMIX_BEGINSAMPLELOOP8 SNDMIX_GETSTEREOVOL8FIRFILTER
    SNDMIX_PROCESSSTEREOFILTER SNDMIX_RAMPSTEREOVOL
END_RAMPMIX_STFLT_INTERFACE()
 BEGIN_RAMPMIX_STFLT_INTERFACE(FilterStereo16BitFirFilterRampMix)
    SNDMIX_BEGINSAMPLELOOP16 SNDMIX_GETSTEREOVOL16FIRFILTER
    SNDMIX_PROCESSSTEREOFILTER SNDMIX_RAMPSTEREOVOL
END_RAMPMIX_STFLT_INTERFACE()

#else
// Mono
#define FilterMono8BitMix		    Mono8BitMix
#define FilterMono16BitMix		    Mono16BitMix
#define FilterMono8BitLinearMix		    Mono8BitLinearMix
#define FilterMono16BitLinearMix	    Mono16BitLinearMix
#define FilterMono8BitSplineMix		    Mono8BitSplineMix
#define FilterMono16BitSplineMix	    Mono16BitSplineMix
#define FilterMono8BitFirFilterMix	    Mono8BitFirFilterMix
#define FilterMono16BitFirFilterMix	    Mono16BitFirFilterMix
#define FilterMono8BitRampMix		    Mono8BitRampMix
#define FilterMono16BitRampMix		    Mono16BitRampMix
#define FilterMono8BitLinearRampMix	    Mono8BitLinearRampMix
#define FilterMono16BitLinearRampMix	    Mono16BitLinearRampMix
#define FilterMono8BitSplineRampMix	    Mono8BitSplineRampMix
#define FilterMono16BitSplineRampMix	    Mono16BitSplineRampMix
#define FilterMono8BitFirFilterRampMix	    Mono8BitFirFilterRampMix
#define FilterMono16BitFirFilterRampMix	    Mono16BitFirFilterRampMix
// Stereo
#define FilterStereo8BitMix		    Stereo8BitMix
#define FilterStereo16BitMix		    Stereo16BitMix
#define FilterStereo8BitLinearMix	    Stereo8BitLinearMix
#define FilterStereo16BitLinearMix	    Stereo16BitLinearMix
#define FilterStereo8BitSplineMix	    Stereo8BitSplineMix
#define FilterStereo16BitSplineMix	    Stereo16BitSplineMix
#define FilterStereo8BitFirFilterMix	    Stereo8BitFirFilterMix
#define FilterStereo16BitFirFilterMix	    Stereo16BitFirFilterMix
#define FilterStereo8BitRampMix		    Stereo8BitRampMix
#define FilterStereo16BitRampMix	    Stereo16BitRampMix
#define FilterStereo8BitLinearRampMix	    Stereo8BitLinearRampMix
#define FilterStereo16BitLinearRampMix	    Stereo16BitLinearRampMix
#define FilterStereo8BitSplineRampMix	    Stereo8BitSplineRampMix
#define FilterStereo16BitSplineRampMix	    Stereo16BitSplineRampMix
#define FilterStereo8BitFirFilterRampMix    Stereo8BitFirFilterRampMix
#define FilterStereo16BitFirFilterRampMix   Stereo16BitFirFilterRampMix
#endif
////////////////////////////////////////////////////////////////////////////
//
// Mix function tables
//
//
// Index is as follow:
//  [b1-b0] format (8-bit-mono, 16-bit-mono, 8-bit-stereo, 16-bit-stereo)
//  [b2]    ramp
//  [b3]    filter
//  [b5-b4] src type
//
#define MIXNDX_16BIT	    0x01
#define MIXNDX_STEREO	    0x02
#define MIXNDX_RAMP	    0x04
#define MIXNDX_FILTER	    0x08
#define MIXNDX_LINEARSRC    0x10
#define MIXNDX_SPLINESRC    0x20
#define MIXNDX_FIRSRC	    0x30
    const LPMIXINTERFACE gpMixFunctionTable[2 * 2 * 16] = {
	// No SRC
	Mono8BitMix, Mono16BitMix, Stereo8BitMix, Stereo16BitMix,
	Mono8BitRampMix, Mono16BitRampMix, Stereo8BitRampMix,
	    Stereo16BitRampMix,
	// No SRC, Filter
	FilterMono8BitMix, FilterMono16BitMix, FilterStereo8BitMix,
	    FilterStereo16BitMix,
	FilterMono8BitRampMix, FilterMono16BitRampMix, FilterStereo8BitRampMix,
	    FilterStereo16BitRampMix,
	// Linear SRC
	Mono8BitLinearMix, Mono16BitLinearMix, Stereo8BitLinearMix,
	    Stereo16BitLinearMix,
	Mono8BitLinearRampMix, Mono16BitLinearRampMix, Stereo8BitLinearRampMix,
	    Stereo16BitLinearRampMix,
	// Linear SRC, Filter
	FilterMono8BitLinearMix, FilterMono16BitLinearMix,
	    FilterStereo8BitLinearMix, FilterStereo16BitLinearMix,
	FilterMono8BitLinearRampMix, FilterMono16BitLinearRampMix,
	    FilterStereo8BitLinearRampMix, FilterStereo16BitLinearRampMix,

	// FirFilter SRC
	Mono8BitSplineMix, Mono16BitSplineMix, Stereo8BitSplineMix,
	    Stereo16BitSplineMix,
	Mono8BitSplineRampMix, Mono16BitSplineRampMix, Stereo8BitSplineRampMix,
	    Stereo16BitSplineRampMix,
	// Spline SRC, Filter
	FilterMono8BitSplineMix, FilterMono16BitSplineMix,
	    FilterStereo8BitSplineMix, FilterStereo16BitSplineMix,
	FilterMono8BitSplineRampMix, FilterMono16BitSplineRampMix,
	    FilterStereo8BitSplineRampMix, FilterStereo16BitSplineRampMix,

	// FirFilter  SRC
	Mono8BitFirFilterMix, Mono16BitFirFilterMix, Stereo8BitFirFilterMix,
	    Stereo16BitFirFilterMix,
	Mono8BitFirFilterRampMix, Mono16BitFirFilterRampMix,
	    Stereo8BitFirFilterRampMix, Stereo16BitFirFilterRampMix,
	// FirFilter  SRC, Filter
	FilterMono8BitFirFilterMix, FilterMono16BitFirFilterMix,
	    FilterStereo8BitFirFilterMix, FilterStereo16BitFirFilterMix,
	FilterMono8BitFirFilterRampMix, FilterMono16BitFirFilterRampMix,
	    FilterStereo8BitFirFilterRampMix, FilterStereo16BitFirFilterRampMix
    };

    const LPMIXINTERFACE gpFastMixFunctionTable[2 * 2 * 16] = {
	// No SRC
	FastMono8BitMix, FastMono16BitMix, Stereo8BitMix, Stereo16BitMix,
	FastMono8BitRampMix, FastMono16BitRampMix, Stereo8BitRampMix,
	    Stereo16BitRampMix,
	// No SRC, Filter
	FilterMono8BitMix, FilterMono16BitMix, FilterStereo8BitMix,
	    FilterStereo16BitMix,
	FilterMono8BitRampMix, FilterMono16BitRampMix, FilterStereo8BitRampMix,
	    FilterStereo16BitRampMix,
	// Linear SRC
	FastMono8BitLinearMix, FastMono16BitLinearMix, Stereo8BitLinearMix,
	    Stereo16BitLinearMix,
	FastMono8BitLinearRampMix, FastMono16BitLinearRampMix,
	    Stereo8BitLinearRampMix, Stereo16BitLinearRampMix,
	// Linear SRC, Filter
	FilterMono8BitLinearMix, FilterMono16BitLinearMix,
	    FilterStereo8BitLinearMix, FilterStereo16BitLinearMix,
	FilterMono8BitLinearRampMix, FilterMono16BitLinearRampMix,
	    FilterStereo8BitLinearRampMix, FilterStereo16BitLinearRampMix,

	// Spline SRC
	Mono8BitSplineMix, Mono16BitSplineMix, Stereo8BitSplineMix,
	    Stereo16BitSplineMix,
	Mono8BitSplineRampMix, Mono16BitSplineRampMix, Stereo8BitSplineRampMix,
	    Stereo16BitSplineRampMix,
	// Spline SRC, Filter
	FilterMono8BitSplineMix, FilterMono16BitSplineMix,
	    FilterStereo8BitSplineMix, FilterStereo16BitSplineMix,
	FilterMono8BitSplineRampMix, FilterMono16BitSplineRampMix,
	    FilterStereo8BitSplineRampMix, FilterStereo16BitSplineRampMix,

	// FirFilter SRC
	Mono8BitFirFilterMix, Mono16BitFirFilterMix, Stereo8BitFirFilterMix,
	    Stereo16BitFirFilterMix,
	Mono8BitFirFilterRampMix, Mono16BitFirFilterRampMix,
	    Stereo8BitFirFilterRampMix, Stereo16BitFirFilterRampMix,
	// FirFilter SRC, Filter
	FilterMono8BitFirFilterMix, FilterMono16BitFirFilterMix,
	    FilterStereo8BitFirFilterMix, FilterStereo16BitFirFilterMix,
	FilterMono8BitFirFilterRampMix, FilterMono16BitFirFilterRampMix,
	    FilterStereo8BitFirFilterRampMix,
	    FilterStereo16BitFirFilterRampMix,
    };

/////////////////////////////////////////////////////////////////////////

static long MPPFASTCALL GetSampleCount(MODCHANNEL * pChn, long nSamples)
//---------------------------------------------------------------------
{
    long nLoopStart;
    long nInc;
    long nPos;
    long nPosLo;
    long nSmpCount;

    nInc = pChn->nInc;
    if (nSamples <= 0 || !nInc || !pChn->nLength) {
	return 0;
    }

    nLoopStart = (pChn->dwFlags & CHN_LOOP) ? pChn->nLoopStart : 0;
    // Under zero ?
    if ((long)pChn->nPos < nLoopStart) {
	if (nInc < 0) {
	    // Invert loop for bidi loops
	    long nDelta =
		((nLoopStart - pChn->nPos) << 16) - (pChn->nPosLo & 0xffff);
	    pChn->nPos = nLoopStart | (nDelta >> 16);
	    pChn->nPosLo = nDelta & 0xffff;
	    if (((long)pChn->nPos < nLoopStart)
		|| (pChn->nPos >= (nLoopStart + pChn->nLength) / 2)) {
		pChn->nPos = nLoopStart;
		pChn->nPosLo = 0;
	    }
	    nInc = -nInc;
	    pChn->nInc = nInc;
	    pChn->dwFlags &= ~(CHN_PINGPONGFLAG);	// go forward
	    if ((!(pChn->dwFlags & CHN_LOOP)) || (pChn->nPos >= pChn->nLength)) {
		pChn->nPos = pChn->nLength;
		pChn->nPosLo = 0;
		return 0;
	    }
	} else {
	    // We probably didn't hit the loop end yet (first loop), so we do nothing
	    if ((long)pChn->nPos < 0)
		pChn->nPos = 0;
	}
	// Past the end
    } else if (pChn->nPos >= pChn->nLength) {
	if (!(pChn->dwFlags & CHN_LOOP)) {
	    return 0;			// not looping -> stop this channel
	}
	if (pChn->dwFlags & CHN_PINGPONGLOOP) {
	    long nDeltaHi;
	    long nDeltaLo;

	    // Invert loop
	    if (nInc > 0) {
		nInc = -nInc;
		pChn->nInc = nInc;
	    }
	    pChn->dwFlags |= CHN_PINGPONGFLAG;
	    // adjust loop position
	    nDeltaHi = (pChn->nPos - pChn->nLength);
	    nDeltaLo = 0x10000 - (pChn->nPosLo & 0xffff);

	    pChn->nPos = pChn->nLength - nDeltaHi - (nDeltaLo >> 16);
	    pChn->nPosLo = nDeltaLo & 0xffff;
	    if (pChn->nPos <= pChn->nLoopStart || pChn->nPos >= pChn->nLength) {
		pChn->nPos = pChn->nLength - 1;
	    }
	} else {
	    if (nInc < 0) {		// This is a bug
		nInc = -nInc;
		pChn->nInc = nInc;
	    }
	    // Restart at loop start
	    pChn->nPos += nLoopStart - pChn->nLength;
	    if ((long)pChn->nPos < nLoopStart) {
		pChn->nPos = pChn->nLoopStart;
	    }
	}
    }
    nPos = pChn->nPos;

    // too big increment, and/or too small loop length
    if (nPos < nLoopStart) {
	if ((nPos < 0) || (nInc < 0)) {
	    return 0;
	}
    }
    if (nPos < 0 || nPos >= (long)pChn->nLength) {
	return 0;
    }
    nPosLo = (uint16_t) pChn->nPosLo;
    nSmpCount = nSamples;

    if (nInc < 0) {
	long nDeltaHi;
	long nDeltaLo;
	long nPosDest;
	long nInv = -nInc;
	long maxsamples = 16384 / ((nInv >> 16) + 1);

	if (maxsamples < 2) {
	    maxsamples = 2;
	}
	if (nSamples > maxsamples) {
	    nSamples = maxsamples;
	}
	nDeltaHi = (nInv >> 16) * (nSamples - 1);
	nDeltaLo = (nInv & 0xffff) * (nSamples - 1);
	nPosDest = nPos - nDeltaHi + ((nPosLo - nDeltaLo) >> 16);

	if (nPosDest < nLoopStart) {
	    nSmpCount =
		(uint32_t) (((((int64_t)nPos - nLoopStart) << 16) + nPosLo -
		    1) / nInv) + 1;
	}
    } else {
	long nDeltaHi;
	long nDeltaLo;
	long nPosDest;
	long maxsamples = 16384 / ((nInc >> 16) + 1);

	if (maxsamples < 2) {
	    maxsamples = 2;
	}
	if (nSamples > maxsamples) {
	    nSamples = maxsamples;
	}
	nDeltaHi = (nInc >> 16) * (nSamples - 1);
	nDeltaLo = (nInc & 0xffff) * (nSamples - 1);
	nPosDest = nPos + nDeltaHi + ((nPosLo + nDeltaLo) >> 16);

	if (nPosDest >= (long)pChn->nLength) {
	    nSmpCount =
		(uint32_t) (((((int64_t)pChn->nLength - nPos) << 16) -
		    nPosLo - 1) / nInc) + 1;
	}
    }

    if (nSmpCount <= 1) {
	return 1;
    }
    if (nSmpCount > nSamples) {
	return nSamples;
    }
    return nSmpCount;
}

//----------------------------------------------------------------------------
unsigned CSoundFile_CreateStereoMix(CSoundFile * that, int count)
{
    long *pOfsL;
    long *pOfsR;
    uint32_t nchused;
    uint32_t nchmixed;
    unsigned nChn;

    if (!count) {
	return 0;
    }
#ifndef MODPLUG_FASTSOUNDLIB
    if (CSoundFile_gnChannels > 2) {
	X86_InitMixBuffer(CSoundFile_MixRearBuffer, count * 2);
    }
#endif
    nchused = nchmixed = 0;
    for (nChn = 0; nChn < that->m_nMixChannels; nChn++) {
	const LPMIXINTERFACE *pMixFuncTable;
	MODCHANNEL *const pChannel = &that->Chn[that->ChnMix[nChn]];
	unsigned nFlags, nMasterCh;
	long nSmpCount;
	int nsamples;
	unsigned nrampsamples;
	int *pbuffer;
	unsigned naddmix;

	if (!pChannel->pCurrentSample)
	    continue;
	nMasterCh =
	    (that->ChnMix[nChn] <
	    that->m_nChannels) ? that->ChnMix[nChn] + 1 : pChannel->nMasterChn;
	pOfsR = &CSoundFile_gnDryROfsVol;
	pOfsL = &CSoundFile_gnDryLOfsVol;
	nFlags = 0;
	if (pChannel->dwFlags & CHN_16BIT)
	    nFlags |= MIXNDX_16BIT;
	if (pChannel->dwFlags & CHN_STEREO)
	    nFlags |= MIXNDX_STEREO;
#ifndef MODPLUG_NO_FILTER
	if (pChannel->dwFlags & CHN_FILTER)
	    nFlags |= MIXNDX_FILTER;
#endif
	if (!(pChannel->dwFlags & CHN_NOIDO)) {
	    // use hq-fir mixer?
	    if ((CSoundFile_gdwSoundSetup & (SNDMIX_HQRESAMPLER |
			SNDMIX_ULTRAHQSRCMODE))
		== (SNDMIX_HQRESAMPLER | SNDMIX_ULTRAHQSRCMODE)) {
		nFlags += MIXNDX_FIRSRC;
	    } else if ((CSoundFile_gdwSoundSetup & (SNDMIX_HQRESAMPLER)) ==
		SNDMIX_HQRESAMPLER) {
		nFlags += MIXNDX_SPLINESRC;
	    } else {
		nFlags += MIXNDX_LINEARSRC;	// use
	    }
	}
	if ((nFlags < 0x40) && (pChannel->nLeftVol == pChannel->nRightVol)
	    && ((!pChannel->nRampLength)
		|| (pChannel->nLeftRamp == pChannel->nRightRamp))) {
	    pMixFuncTable = gpFastMixFunctionTable;
	} else {
	    pMixFuncTable = gpMixFunctionTable;
	}
	nsamples = count;
#ifndef MODPLUG_NO_REVERB
	pbuffer = (CSoundFile_gdwSoundSetup & SNDMIX_REVERB)
	    ? CSoundFile_MixReverbBuffer : CSoundFile_MixSoundBuffer;
	if (pChannel->dwFlags & CHN_NOREVERB) {
	    pbuffer = CSoundFile_MixSoundBuffer;
	}
	if (pChannel->dwFlags & CHN_REVERB) {
	    pbuffer = CSoundFile_MixReverbBuffer;
	}
	if (pbuffer == CSoundFile_MixReverbBuffer) {
	    if (!CSoundFile_gnReverbSend) {
		memset(CSoundFile_MixReverbBuffer, 0, count * 8);
	    }
	    CSoundFile_gnReverbSend += count;
	}
#else
	pbuffer = CSoundFile_MixSoundBuffer;
#endif
	nchused++;
	////////////////////////////////////////////////////
SampleLooping:

	nrampsamples = nsamples;

	if (pChannel->nRampLength > 0) {
	    if ((long)nrampsamples > pChannel->nRampLength)
		nrampsamples = pChannel->nRampLength;
	}
	if ((nSmpCount = GetSampleCount(pChannel, nrampsamples)) <= 0) {
	    // Stopping the channel
	    pChannel->pCurrentSample = NULL;
	    pChannel->nLength = 0;
	    pChannel->nPos = 0;
	    pChannel->nPosLo = 0;
	    pChannel->nRampLength = 0;
	    X86_EndChannelOfs(pChannel, pbuffer, nsamples);
	    *pOfsR += pChannel->nROfs;
	    *pOfsL += pChannel->nLOfs;
	    pChannel->nROfs = pChannel->nLOfs = 0;
	    pChannel->dwFlags &= ~CHN_PINGPONGFLAG;
	    continue;
	}
	// Should we mix this channel ?

	if (((nchmixed >= CSoundFile_MaxMixChannels)
		&& (!(CSoundFile_gdwSoundSetup & SNDMIX_DIRECTTODISK)))
	    || ((!pChannel->nRampLength)
		&& (!(pChannel->nLeftVol | pChannel->nRightVol)))) {
	    long delta =
		(pChannel->nInc * (long)nSmpCount) + (long)pChannel->nPosLo;
	    pChannel->nPosLo = delta & 0xFFFF;
	    pChannel->nPos += (delta >> 16);
	    pChannel->nROfs = pChannel->nLOfs = 0;
	    pbuffer += nSmpCount * 2;
	    naddmix = 0;
	} else
	    // Do mixing
	{
	    // Choose function for mixing
	    LPMIXINTERFACE pMixFunc;
	    int *pbufmax;

	    pMixFunc =
		(pChannel->
		nRampLength) ? pMixFuncTable[nFlags | MIXNDX_RAMP] :
		pMixFuncTable[nFlags];

	    pbufmax = pbuffer + (nSmpCount * 2);

	    pChannel->nROfs = -*(pbufmax - 2);
	    pChannel->nLOfs = -*(pbufmax - 1);
	    pMixFunc(pChannel, pbuffer, pbufmax);
	    pChannel->nROfs += *(pbufmax - 2);
	    pChannel->nLOfs += *(pbufmax - 1);
	    pbuffer = pbufmax;
	    naddmix = 1;

	}
	nsamples -= nSmpCount;
	if (pChannel->nRampLength) {
	    pChannel->nRampLength -= nSmpCount;
	    if (pChannel->nRampLength <= 0) {
		pChannel->nRampLength = 0;
		pChannel->nRightVol = pChannel->nNewRightVol;
		pChannel->nLeftVol = pChannel->nNewLeftVol;
		pChannel->nRightRamp = pChannel->nLeftRamp = 0;
		if ((pChannel->dwFlags & CHN_NOTEFADE)
		    && (!(pChannel->nFadeOutVol))) {
		    pChannel->nLength = 0;
		    pChannel->pCurrentSample = NULL;
		}
	    }
	}
	if (nsamples > 0)
	    goto SampleLooping;
	nchmixed += naddmix;
    }
    return nchused;
}

#ifdef _MSC_VER
#pragma warning (disable:4100)
#endif

// Clip and convert to 8 bit
#ifdef _MSC_VER

__declspec(naked)
uint32_t MPPASMCALL X86_Convert32To8(void *lp16, const int *pBuffer,
    uint32_t lSampleCount, long *lpMin, long *lpMax)
//----------------------------------------------------------------------------------------------------------------------------
{
    __asm {
	push ebx
	push esi
	push edi
	mov ebx, 16[esp]		// ebx = 8-bit buffer
	mov esi, 20[esp]		// esi = pBuffer
	mov edi, 24[esp]		// edi = lSampleCount
	mov eax, 28[esp]
	mov ecx, dword ptr[eax]		// ecx = clipmin
	mov eax, 32[esp]
	mov edx, dword ptr[eax]		// edx = clipmax
cliploop:mov eax, dword ptr[esi]
	inc ebx
	cdq
	and edx, (1 << (24 - MIXING_ATTENUATION)) - 1
	add eax, edx
	cmp eax, MIXING_CLIPMIN
	jl cliplow
	cmp eax, MIXING_CLIPMAX
	jg cliphigh
	cmp eax, ecx
	jl updatemin
	cmp eax, edx
	jg updatemax
cliprecover:add esi, 4
	sar eax, 24 - MIXING_ATTENUATION
	xor eax, 0x80
	dec edi
	mov byte ptr[ebx - 1], al
	jnz cliploop
	mov eax, 28[esp]
	mov dword ptr[eax], ecx
	mov eax, 32[esp]
	mov dword ptr[eax], edx
	mov eax, 24[esp]
	pop edi
	pop esi
	pop ebx
	ret
updatemin:mov ecx, eax
	jmp cliprecover
updatemax:mov edx, eax
	jmp cliprecover
cliplow:mov ecx, MIXING_CLIPMIN
	mov edx, MIXING_CLIPMAX
	mov eax, MIXING_CLIPMIN
	jmp cliprecover
cliphigh:mov ecx, MIXING_CLIPMIN
	mov edx, MIXING_CLIPMAX
	mov eax, MIXING_CLIPMAX
	jmp cliprecover}
}
#else //_MSC_VER

//---GCCFIX: Asm replaced with C function
// The C version was written by Rani Assaf <rani@magic.metawire.com>, I believe
__declspec(naked)
uint32_t MPPASMCALL X86_Convert32To8(void *lp8, const int *pBuffer,
    uint32_t lSampleCount, long *lpMin, long *lpMax)
{
    uint32_t i;
    long vumin;
    long vumax;
    uint8_t *p;

    vumin = *lpMin;
    vumax = *lpMax;
    p = (uint8_t *) lp8;
    for (i = 0; i < lSampleCount; i++) {
	int n = pBuffer[i];

	if (n < MIXING_CLIPMIN) {
	    n = MIXING_CLIPMIN;
	} else if (n > MIXING_CLIPMAX) {
	    n = MIXING_CLIPMAX;
	}
	if (n < vumin) {
	    vumin = n;
	} else if (n > vumax) {
	    vumax = n;
	}
	p[i] = (n >> (24 - MIXING_ATTENUATION)) ^ 0x80;	// 8-bit unsigned
    }
    *lpMin = vumin;
    *lpMax = vumax;

    return lSampleCount;
}

#endif //_MSC_VER, else

#ifdef _MSC_VER
// Clip and convert to 16 bit
__declspec(naked)
uint32_t MPPASMCALL X86_Convert32To16(void *lp16, const int *pBuffer,
    uint32_t lSampleCount, long *lpMin, long *lpMax)
//-----------------------------------------------------------------------------------------------------------------------------
{
    _asm {
	push ebx
	push esi
	push edi
	mov ebx, 16[esp]	// ebx = 16-bit buffer
	mov eax, 28[esp]
	mov esi, 20[esp]		// esi = pBuffer
	mov ecx, dword ptr[eax]		// ecx = clipmin
	mov edi, 24[esp]		// edi = lSampleCount
	mov eax, 32[esp]
	push ebp
	mov ebp, dword ptr[eax]	// edx = clipmax
cliploop:mov eax, dword ptr[esi]
	add ebx, 2
	cdq
	and edx, (1 << (16 - MIXING_ATTENUATION)) - 1
	add esi, 4
	add eax, edx
	cmp eax, MIXING_CLIPMIN
	jl cliplow
	cmp eax, MIXING_CLIPMAX
	jg cliphigh
	cmp eax, ecx
	jl updatemin
	cmp eax, ebp
	jg updatemax
cliprecover:sar eax, 16 - MIXING_ATTENUATION
	dec edi
	mov word ptr[ebx - 2], ax
	jnz cliploop
	mov edx, ebp
	pop ebp
	mov eax, 28[esp]
	mov dword ptr[eax], ecx
	mov eax, 32[esp]
	mov dword ptr[eax], edx
	mov eax, 24[esp]
	pop edi
	shl eax, 1
	pop esi
	pop ebx
	ret
updatemin:mov ecx, eax
	jmp cliprecover
updatemax:mov ebp, eax
	jmp cliprecover
cliplow:mov ecx, MIXING_CLIPMIN
	mov ebp, MIXING_CLIPMAX
	mov eax, MIXING_CLIPMIN
	jmp cliprecover
cliphigh:mov ecx, MIXING_CLIPMIN
	mov ebp, MIXING_CLIPMAX
	mov eax, MIXING_CLIPMAX
	jmp cliprecover}
}

#else //_MSC_VER

//---GCCFIX: Asm replaced with C function
// The C version was written by Rani Assaf <rani@magic.metawire.com>, I believe
__declspec(naked)
uint32_t MPPASMCALL X86_Convert32To16(void *lp16, const int *pBuffer,
    uint32_t lSampleCount, long *lpMin, long *lpMax)
{
    uint32_t i;
    long vumin;
    long vumax;
    int16_t *p;

    vumin = *lpMin;
    vumax = *lpMax;
    p = (int16_t *)lp16;
    for (i = 0; i < lSampleCount; i++) {
	int n = pBuffer[i];

	if (n < MIXING_CLIPMIN) {
	    n = MIXING_CLIPMIN;
	} else if (n > MIXING_CLIPMAX) {
	    n = MIXING_CLIPMAX;
	}
	if (n < vumin) {
	    vumin = n;
	} else if (n > vumax) {
	    vumax = n;
	}
	p[i] = n >> (16 - MIXING_ATTENUATION);	// 16-bit signed
    }
    *lpMin = vumin;
    *lpMax = vumax;

    return lSampleCount * 2;
}

#endif //_MSC_VER, else

#ifdef _MSC_VER
// Clip and convert to 24 bit
__declspec(naked)
    uint32_t MPPASMCALL X86_Convert32To24(void *lp16, const int *pBuffer,
    uint32_t lSampleCount, long *lpMin, long *lpMax)
//-----------------------------------------------------------------------------------------------------------------------------
{
    _asm {
	push ebx
	push esi
	push edi
	mov ebx, 16[esp]	// ebx = 8-bit buffer
	mov esi, 20[esp]		// esi = pBuffer
	mov edi, 24[esp]		// edi = lSampleCount
	mov eax, 28[esp]
	mov ecx, dword ptr[eax]		// ecx = clipmin
	mov eax, 32[esp]
	push ebp
	mov edx, dword ptr[eax]	// edx = clipmax
cliploop:mov eax, dword ptr[esi]
	mov ebp, eax
	sar ebp, 31
	and ebp, (1 << (8 - MIXING_ATTENUATION)) - 1
	add eax, ebp
	cmp eax, MIXING_CLIPMIN
	jl cliplow
	cmp eax, MIXING_CLIPMAX
	jg cliphigh
	cmp eax, ecx
	jl updatemin
	cmp eax, edx
	jg updatemax
cliprecover:add ebx, 3
	sar eax, 8 - MIXING_ATTENUATION
	add esi, 4
	mov word ptr[ebx - 3], ax
	shr eax, 16
	dec edi
	mov byte ptr[ebx - 1], al
	jnz cliploop
	pop ebp
	mov eax, 28[esp]
	mov dword ptr[eax], ecx
	mov eax, 32[esp]
	mov dword ptr[eax], edx
	mov edx, 24[esp]
	mov eax, edx
	pop edi
	shl eax, 1
	pop esi
	add eax, edx
	pop ebx
	ret
updatemin:mov ecx, eax
	jmp cliprecover
updatemax:mov edx, eax
	jmp cliprecover
cliplow:mov ecx, MIXING_CLIPMIN
	mov edx, MIXING_CLIPMAX
	mov eax, MIXING_CLIPMIN
	jmp cliprecover
cliphigh:mov ecx, MIXING_CLIPMIN
	mov edx, MIXING_CLIPMAX
	mov eax, MIXING_CLIPMAX
	jmp cliprecover}
}

#else //_MSC_VER

//---GCCFIX: Asm replaced with C function
__declspec(naked)
uint32_t MPPASMCALL X86_Convert32To24(void *lp24, const int *pBuffer,
    uint32_t lSampleCount, long *lpMin, long *lpMax)
{
    uint32_t i;
    long vumin;
    long vumax;
    int8_t *p;

    vumin = *lpMin;
    vumax = *lpMax;
    p = (int8_t *)lp24;
    for (i = 0; i < lSampleCount; i++) {
	int n = pBuffer[i];

	if (n < MIXING_CLIPMIN) {
	    n = MIXING_CLIPMIN;
	} else if (n > MIXING_CLIPMAX) {
	    n = MIXING_CLIPMAX;
	}
	if (n < vumin) {
	    vumin = n;
	} else if (n > vumax) {
	    vumax = n;
	}
	n >>= (8 - MIXING_ATTENUATION);	// 24-bit signed
	p[i*3+0] = n >> 0;
	p[i*3+1] = n >> 8;
	p[i*3+2] = n >> 16;		// FIXME: johns: not sure if correct
    }
    *lpMin = vumin;
    *lpMax = vumax;

    return lSampleCount * 3;
}

#endif

#ifdef _MSC_VER
// Clip and convert to 32 bit
__declspec(naked)
uint32_t MPPASMCALL X86_Convert32To32(void *lp32, const int *pBuffer,
    uint32_t lSampleCount, long *lpMin, long *lpMax)
//-----------------------------------------------------------------------------------------------------------------------------
{
    _asm {
	push ebx
	push esi
	push edi
	mov ebx, 16[esp]	// ebx = 32-bit buffer
	mov esi, 20[esp]		// esi = pBuffer
	mov edi, 24[esp]		// edi = lSampleCount
	mov eax, 28[esp]
	mov ecx, dword ptr[eax]		// ecx = clipmin
	mov eax, 32[esp]
	mov edx, dword ptr[eax]		// edx = clipmax
cliploop:mov eax, dword ptr[esi]
	add ebx, 4
	add esi, 4
	cmp eax, MIXING_CLIPMIN
	jl cliplow
	cmp eax, MIXING_CLIPMAX
	jg cliphigh
	cmp eax, ecx
	jl updatemin
	cmp eax, edx
	jg updatemax
cliprecover:shl eax, MIXING_ATTENUATION
	dec edi
	mov dword ptr[ebx - 4], eax
	jnz cliploop
	mov eax, 28[esp]
	mov dword ptr[eax], ecx
	mov eax, 32[esp]
	mov dword ptr[eax], edx
	mov edx, 24[esp]
	pop edi
	mov eax, edx
	pop esi
	shl eax, 2
	pop ebx
	ret
updatemin:mov ecx, eax
	jmp cliprecover
updatemax:mov edx, eax
	jmp cliprecover
cliplow:mov ecx, MIXING_CLIPMIN
	mov edx, MIXING_CLIPMAX
	mov eax, MIXING_CLIPMIN
	jmp cliprecover
cliphigh:mov ecx, MIXING_CLIPMIN
	 mov edx, MIXING_CLIPMAX
	 mov eax, MIXING_CLIPMAX
	 jmp cliprecover}
}

#else

//---GCCFIX: Asm replaced with C function
__declspec(naked)
uint32_t MPPASMCALL X86_Convert32To32(void *lp32, const int *pBuffer,
    uint32_t lSampleCount, long *lpMin, long *lpMax)
{
    uint32_t i;
    long vumin;
    long vumax;
    int32_t *p;

    vumin = *lpMin;
    vumax = *lpMax;
    p = (int32_t *)lp32;
    for (i = 0; i < lSampleCount; i++) {
	int n = pBuffer[i];

	if (n < MIXING_CLIPMIN) {
	    n = MIXING_CLIPMIN;
	} else if (n > MIXING_CLIPMAX) {
	    n = MIXING_CLIPMAX;
	}
	if (n < vumin) {
	    vumin = n;
	} else if (n > vumax) {
	    vumax = n;
	}
	p[i] = n << MIXING_ATTENUATION;
    }
    *lpMin = vumin;
    *lpMax = vumax;

    return lSampleCount * 4;
}

#endif

#ifdef _MSC_VER

static void MPPASMCALL X86_InitMixBuffer(int *pBuffer, unsigned nSamples)
//------------------------------------------------------------
{
    _asm {
	mov ecx, nSamples
	mov esi, pBuffer
	xor eax, eax
	mov edx, ecx
	shr ecx, 2
	and edx, 3
	jz unroll4x
loop1x:	add esi, 4
	dec edx
	mov dword ptr[esi - 4], eax
	jnz loop1x
unroll4x:or ecx, ecx
	jnz loop4x
	jmp done
loop4x:	add esi, 16
	dec ecx
	mov dword ptr[esi - 16], eax
	mov dword ptr[esi - 12], eax
	mov dword ptr[esi - 8], eax
	mov dword ptr[esi - 4], eax
	jnz loop4x
done:;
    }
}

#else

//---GCCFIX: Asm replaced with C function
// Will fill in later.
static void MPPASMCALL X86_InitMixBuffer(int *pBuffer, unsigned nSamples)
{
    memset(pBuffer, 0, nSamples * sizeof(int));
}

#endif

#ifdef _MSC_VER
__declspec(naked)
void MPPASMCALL X86_InterleaveFrontRear(int *pFrontBuf, int *pRearBuf,
    uint32_t nSamples)
//------------------------------------------------------------------------------------------------------
{
    _asm {
	push ebx
	push ebp
	push esi
	push edi
	mov ecx, 28[esp]	// ecx = samplecount
	mov esi, 20[esp]		// esi = front buffer
	mov edi, 24[esp]		// edi = rear buffer
	lea esi,[esi + ecx * 4]		// esi = &front[N]
	lea edi,[edi + ecx * 4]		// edi = &rear[N]
	lea ebx,[esi + ecx * 4]		// ebx = &front[N*2]
interleaveloop:mov eax, dword ptr[esi - 8]
	mov edx, dword ptr[esi - 4]
	sub ebx, 16
	mov ebp, dword ptr[edi - 8]
	mov dword ptr[ebx], eax
	mov dword ptr[ebx + 4], edx
	mov eax, dword ptr[edi - 4]
	sub esi, 8
	sub edi, 8
	dec ecx
	mov dword ptr[ebx + 8], ebp
	mov dword ptr[ebx + 12], eax
	jnz interleaveloop
	pop edi
	pop esi
	pop ebp
	pop ebx
	ret}
}
#else

//---GCCFIX: Asm replaced with C function
// Multichannel not supported.
__declspec(naked)
void MPPASMCALL X86_InterleaveFrontRear(int * pFrontBuf __attribute__((unused)),
    int * pRearBuf __attribute__((unused)),
    uint32_t nSamples __attribute__((unused)))
{
    // FIXME: not supported!!!
}

#endif

#ifdef _MSC_VER

void MPPASMCALL X86_MonoFromStereo(int *pMixBuf, unsigned nSamples)
//-------------------------------------------------------------
{
    _asm {
	mov ecx, nSamples
	mov esi, pMixBuf
	mov edi, esi
stloop:	mov eax, dword ptr[esi]
	mov edx, dword ptr[esi + 4]
	add edi, 4
	add esi, 8
	add eax, edx
	sar eax, 1
	dec ecx
	mov dword ptr[edi - 4], eax
	jnz stloop
    }
}

#else

//---GCCFIX: Asm replaced with C function
void MPPASMCALL X86_MonoFromStereo(int *pMixBuf, unsigned nSamples)
{
    unsigned j;
    unsigned i;

    for (j = i = 0; i < nSamples; i++, j += 2) {
	pMixBuf[i] = (pMixBuf[j] + pMixBuf[j + 1]) >> 1;
    }
}

#endif

#define OFSDECAYSHIFT	8
#define OFSDECAYMASK	0xFF

#ifdef _MSC_VER

void MPPASMCALL X86_StereoFill(int *pBuffer, unsigned nSamples,
    long *lpROfs, long *lpLOfs)
//---------------------------------------------------------------------------------------
{
    _asm {
	mov edi, pBuffer
	mov ecx, nSamples
	mov eax, lpROfs
	mov edx, lpLOfs
	mov eax,[eax]
	mov edx,[edx]
	or ecx, ecx
	jz fill_loop
	mov ebx, eax
	or ebx, edx
	jz fill_loop
ofsloop:mov ebx, eax
	mov esi, edx
	neg ebx
	neg esi
	sar ebx, 31
	sar esi, 31
	and ebx, OFSDECAYMASK
	and esi, OFSDECAYMASK
	add ebx, eax
	add esi, edx
	sar ebx, OFSDECAYSHIFT
	sar esi, OFSDECAYSHIFT
	sub eax, ebx
	sub edx, esi
	mov ebx, eax
	or ebx, edx
	jz fill_loop
	add edi, 8
	dec ecx
	mov[edi - 8], eax
	mov[edi - 4], edx
	jnz ofsloop
fill_loop:mov ebx, ecx
	and ebx, 3
	jz fill4x
fill1x:	mov [edi], eax
	mov[edi + 4], edx
	add edi, 8
	dec ebx
	jnz fill1x
fill4x:	shr ecx, 2
	or ecx, ecx
	jz done
fill4xloop:mov[edi], eax
	mov[edi + 4], edx
	mov[edi + 8], eax
	mov[edi + 12], edx
	add edi, 8 * 4
	dec ecx
	mov[edi - 16], eax
	mov[edi - 12], edx
	mov[edi - 8], eax
	mov[edi - 4], edx
	jnz fill4xloop
done:	mov esi, lpROfs
	mov edi, lpLOfs
	mov[esi], eax
	mov[edi], edx}
}

#else
//---GCCFIX: Asm replaced with C function

__declspec(naked)
void MPPASMCALL X86_StereoFill(int *pBuffer, unsigned nSamples,
    long *lpROfs, long *lpLOfs)
//-----------------------------------------------------------------------------
{
    unsigned i;
    int rofs;
    int lofs;

    rofs = *lpROfs;
    lofs = *lpLOfs;
    if ( !rofs && !lofs ) {
	X86_InitMixBuffer(pBuffer, nSamples * 2);
	return;
    }

    for (i = 0; i < nSamples; i++) {
	int x_r = (rofs + (((-rofs) >> 31) & OFSDECAYMASK)) >> OFSDECAYSHIFT;
	int x_l = (lofs + (((-lofs) >> 31) & OFSDECAYMASK)) >> OFSDECAYSHIFT;

	rofs -= x_r;
	lofs -= x_l;
	pBuffer[i * 2] = x_r;
	pBuffer[i * 2 + 1] = x_l;
    }

    *lpROfs = rofs;
    *lpLOfs = lofs;
}

#endif

#ifdef _MSC_VER
static void MPPASMCALL X86_EndChannelOfs(MODCHANNEL * pChannel, int *pBuffer,
    unsigned nSamples)
//----------------------------------------------------------------------------------
{
    _asm {
	mov esi, pChannel
	mov edi, pBuffer
	mov ecx, nSamples
	mov eax, dword ptr[esi + MODCHANNEL.nROfs]
	mov edx, dword ptr[esi + MODCHANNEL.nLOfs]
	or ecx, ecx
	jz brkloop
ofsloop:mov ebx, eax
	mov esi, edx
	neg ebx
	neg esi
	sar ebx, 31
	sar esi, 31
	and ebx, OFSDECAYMASK
	and esi, OFSDECAYMASK
	add ebx, eax
	add esi, edx
	sar ebx, OFSDECAYSHIFT
	sar esi, OFSDECAYSHIFT
	sub eax, ebx
	sub edx, esi
	mov ebx, eax
	add dword ptr[edi], eax
	add dword ptr[edi + 4], edx
	or ebx, edx
	jz brkloop
	add edi, 8
	dec ecx
	jnz ofsloop
brkloop:mov esi, pChannel
	mov dword ptr[esi + MODCHANNEL.nROfs], eax
	mov dword ptr[esi + MODCHANNEL.nLOfs], edx}
}
#else

//---GCCFIX: Asm replaced with C function
static void MPPASMCALL X86_EndChannelOfs(MODCHANNEL * pChannel, int *pBuffer,
    unsigned nSamples)
{
    int rofs;
    int lofs;
    unsigned i;

    rofs = pChannel->nROfs;
    lofs = pChannel->nLOfs;
    if (!rofs && !lofs) {
	return;
    }
    for (i = 0; i < nSamples; i++) {
	int x_r = (rofs + (((-rofs) >> 31) & OFSDECAYMASK)) >> OFSDECAYSHIFT;
	int x_l = (lofs + (((-lofs) >> 31) & OFSDECAYMASK)) >> OFSDECAYSHIFT;

	rofs -= x_r;
	lofs -= x_l;
	pBuffer[i * 2] += x_r;
	pBuffer[i * 2 + 1] += x_l;
    }
    pChannel->nROfs = rofs;
    pChannel->nLOfs = lofs;
}

#endif

//////////////////////////////////////////////////////////////////////////////
// Automatic Gain Control

#ifndef MODPLUG_NO_AGC			// {

// Limiter
#define MIXING_LIMITMAX	    (0x08100000)
#define MIXING_LIMITMIN	    (-MIXING_LIMITMAX)

__declspec(naked)
unsigned MPPASMCALL X86_AGC(int *pBuffer, unsigned nSamples, unsigned nAGC)
//-------------------------------------------------------------------------------
{
    __asm {
	push ebx
	push ebp
	push esi
	push edi
	mov esi, 20[esp]	// esi = pBuffer+i
	mov ecx, 24[esp]		// ecx = i
	mov edi, 28[esp]		// edi = AGC (0..256)
agcloop:mov eax, dword ptr[esi]
	imul edi
	shrd eax, edx, AGC_PRECISION
	add esi, 4
	cmp eax, MIXING_LIMITMIN
	jl agcupdate
	cmp eax, MIXING_LIMITMAX
	jg agcupdate
agcrecover:dec ecx
	mov dword ptr[esi - 4], eax
	jnz agcloop
	mov eax, edi
	pop edi
	pop esi
	pop ebp
	pop ebx
	ret
agcupdate:dec edi
	jmp agcrecover}
}

#pragma warning (default:4100)

void CSoundFile_ProcessAGC(int count)
//------------------------------------
{
    static uint32_t gAGCRecoverCount = 0;
    unsigned agc = X86_AGC(CSoundFile_MixSoundBuffer, count, CSoundFile_gnAGC);

    // Some kind custom law, so that the AGC stays quite stable, but slowly
    // goes back up if the sound level stays below a level inversely proportional
    // to the AGC level. (J'me comprends)
    if ((agc >= CSoundFile_gnAGC) && (CSoundFile_gnAGC < AGC_UNITY)
	&& (CSoundFile_gnVUMeter <
	    (0xFF - (CSoundFile_gnAGC >> (AGC_PRECISION - 7))))) {
	unsigned agctimeout;
	gAGCRecoverCount += count;
	agctimeout = CSoundFile_gdwMixingFreq + CSoundFile_gnAGC;

	if (CSoundFile_gnChannels >= 2)
	    agctimeout <<= 1;
	if (gAGCRecoverCount >= agctimeout) {
	    gAGCRecoverCount = 0;
	    CSoundFile_gnAGC++;
	}
    } else {
	CSoundFile_gnAGC = agc;
	gAGCRecoverCount = 0;
    }
}

void CSoundFile_ResetAGC(void)
//-------------------------
{
    CSoundFile_gnAGC = AGC_UNITY;
}

#endif // } !MODPLUG_NO_AGC