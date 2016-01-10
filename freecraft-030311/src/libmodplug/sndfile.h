/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: sndfile.h,v 1.6 2002/05/27 21:43:35 johns Exp $
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef __SNDFILE_H
#define __SNDFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#define MOD_AMIGAC2	    0x1AB
#define MAX_SAMPLE_LENGTH   16000000
#define MAX_SAMPLE_RATE	    50000
#define MAX_ORDERS	    256
#define MAX_PATTERNS	    240
#define MAX_SAMPLES	    240
#define MAX_INSTRUMENTS	    MAX_SAMPLES
#ifdef MODPLUG_FASTSOUNDLIB
#define MAX_CHANNELS	    80
#else
#define MAX_CHANNELS	    128
#endif
#define MAX_BASECHANNELS    64
#define MAX_ENVPOINTS	    32
#define MIN_PERIOD	    0x0020
#define MAX_PERIOD	    0xFFFF
#define MAX_PATTERNNAME	    32
#define MAX_CHANNELNAME	    20
#define MAX_INFONAME	    80
#define MAX_EQ_BANDS	    6
#define MAX_MIXPLUGINS	    8


#define MOD_TYPE_NONE	    0x00
#define MOD_TYPE_MOD	    0x01
#define MOD_TYPE_S3M	    0x02
#define MOD_TYPE_XM	    0x04
#define MOD_TYPE_MED	    0x08
#define MOD_TYPE_MTM	    0x10
#define MOD_TYPE_IT	    0x20
#define MOD_TYPE_669	    0x40
#define MOD_TYPE_ULT	    0x80
#define MOD_TYPE_STM	    0x100
#define MOD_TYPE_FAR	    0x200
#define MOD_TYPE_WAV	    0x400
#define MOD_TYPE_AMF	    0x800
#define MOD_TYPE_AMS	    0x1000
#define MOD_TYPE_DSM	    0x2000
#define MOD_TYPE_MDL	    0x4000
#define MOD_TYPE_OKT	    0x8000
#define MOD_TYPE_MID	    0x10000
#define MOD_TYPE_DMF	    0x20000
#define MOD_TYPE_PTM	    0x40000
#define MOD_TYPE_DBM	    0x80000
#define MOD_TYPE_MT2	    0x100000
#define MOD_TYPE_AMF0	    0x200000
#define MOD_TYPE_PSM	    0x400000
#define MOD_TYPE_J2B	    0x800000
#define MOD_TYPE_UMX	    0x80000000 // Fake type
#define MAX_MODTYPE	    23



// Channel flags:
// Bits 0-7:	Sample Flags
#define CHN_16BIT	    0x01
#define CHN_LOOP	    0x02
#define CHN_PINGPONGLOOP    0x04
#define CHN_SUSTAINLOOP	    0x08
#define CHN_PINGPONGSUSTAIN 0x10
#define CHN_PANNING	    0x20
#define CHN_STEREO	    0x40
#define CHN_PINGPONGFLAG    0x80
// Bits 8-31:	Channel Flags
#define CHN_MUTE	    0x100
#define CHN_KEYOFF	    0x200
#define CHN_NOTEFADE	    0x400
#define CHN_SURROUND	    0x800
#define CHN_NOIDO	    0x1000
#define CHN_HQSRC	    0x2000
#define CHN_FILTER	    0x4000
#define CHN_VOLUMERAMP	    0x8000
#define CHN_VIBRATO	    0x10000
#define CHN_TREMOLO	    0x20000
#define CHN_PANBRELLO	    0x40000
#define CHN_PORTAMENTO	    0x80000
#define CHN_GLISSANDO	    0x100000
#define CHN_VOLENV	    0x200000
#define CHN_PANENV	    0x400000
#define CHN_PITCHENV	    0x800000
#define CHN_FASTVOLRAMP	    0x1000000
#define CHN_EXTRALOUD	    0x2000000
#define CHN_REVERB	    0x4000000
#define CHN_NOREVERB	    0x8000000


#define ENV_VOLUME	    0x0001
#define ENV_VOLSUSTAIN	    0x0002
#define ENV_VOLLOOP	    0x0004
#define ENV_PANNING	    0x0008
#define ENV_PANSUSTAIN	    0x0010
#define ENV_PANLOOP	    0x0020
#define ENV_PITCH	    0x0040
#define ENV_PITCHSUSTAIN    0x0080
#define ENV_PITCHLOOP	    0x0100
#define ENV_SETPANNING	    0x0200
#define ENV_FILTER	    0x0400
#define ENV_VOLCARRY	    0x0800
#define ENV_PANCARRY	    0x1000
#define ENV_PITCHCARRY	    0x2000

#define CMD_NONE		0
#define CMD_ARPEGGIO		1
#define CMD_PORTAMENTOUP	2
#define CMD_PORTAMENTODOWN	3
#define CMD_TONEPORTAMENTO	4
#define CMD_VIBRATO		5
#define CMD_TONEPORTAVOL	6
#define CMD_VIBRATOVOL		7
#define CMD_TREMOLO		8
#define CMD_PANNING8		9
#define CMD_OFFSET		10
#define CMD_VOLUMESLIDE		11
#define CMD_POSITIONJUMP	12
#define CMD_VOLUME		13
#define CMD_PATTERNBREAK	14
#define CMD_RETRIG		15
#define CMD_SPEED		16
#define CMD_TEMPO		17
#define CMD_TREMOR		18
#define CMD_MODCMDEX		19
#define CMD_S3MCMDEX		20
#define CMD_CHANNELVOLUME	21
#define CMD_CHANNELVOLSLIDE	22
#define CMD_GLOBALVOLUME	23
#define CMD_GLOBALVOLSLIDE	24
#define CMD_KEYOFF		25
#define CMD_FINEVIBRATO		26
#define CMD_PANBRELLO		27
#define CMD_XFINEPORTAUPDOWN	28
#define CMD_PANNINGSLIDE	29
#define CMD_SETENVPOSITION	30
#define CMD_MIDI		31


// Volume Column commands
#define VOLCMD_VOLUME		1
#define VOLCMD_PANNING		2
#define VOLCMD_VOLSLIDEUP	3
#define VOLCMD_VOLSLIDEDOWN	4
#define VOLCMD_FINEVOLUP	5
#define VOLCMD_FINEVOLDOWN	6
#define VOLCMD_VIBRATOSPEED	7
#define VOLCMD_VIBRATO		8
#define VOLCMD_PANSLIDELEFT	9
#define VOLCMD_PANSLIDERIGHT	10
#define VOLCMD_TONEPORTAMENTO	11
#define VOLCMD_PORTAUP		12
#define VOLCMD_PORTADOWN	13

#define RSF_16BIT	0x04
#define RSF_STEREO	0x08

#define RS_PCM8S	0   // 8-bit signed
#define RS_PCM8U	1   // 8-bit unsigned
#define RS_PCM8D	2   // 8-bit delta values
#define RS_ADPCM4	3   // 4-bit ADPCM-packed
#define RS_PCM16D	4   // 16-bit delta values
#define RS_PCM16S	5   // 16-bit signed
#define RS_PCM16U	6   // 16-bit unsigned
#define RS_PCM16M	7   // 16-bit motorola order
#define RS_STPCM8S	(RS_PCM8S|RSF_STEREO)	// stereo 8-bit signed
#define RS_STPCM8U	(RS_PCM8U|RSF_STEREO)	// stereo 8-bit unsigned
#define RS_STPCM8D	(RS_PCM8D|RSF_STEREO)	// stereo 8-bit delta values
#define RS_STPCM16S	(RS_PCM16S|RSF_STEREO)	// stereo 16-bit signed
#define RS_STPCM16U	(RS_PCM16U|RSF_STEREO)	// stereo 16-bit unsigned
#define RS_STPCM16D	(RS_PCM16D|RSF_STEREO)	// stereo 16-bit delta values
#define RS_STPCM16M	(RS_PCM16M|RSF_STEREO)	// stereo 16-bit signed big endian
// IT 2.14 compressed samples
#define RS_IT2148	0x10
#define RS_IT21416	0x14
#define RS_IT2158	0x12
#define RS_IT21516	0x16
// AMS Packed Samples
#define RS_AMS8		0x11
#define RS_AMS16	0x15
// DMF Huffman compression
#define RS_DMF8		0x13
#define RS_DMF16	0x17
// MDL Huffman compression
#define RS_MDL8		0x20
#define RS_MDL16	0x24
#define RS_PTM8DTO16	0x25
// Stereo Interleaved Samples
#define RS_STIPCM8S	(RS_PCM8S|0x40|RSF_STEREO)  // stereo 8-bit signed
#define RS_STIPCM8U	(RS_PCM8U|0x40|RSF_STEREO)  // stereo 8-bit unsigned
#define RS_STIPCM16S	(RS_PCM16S|0x40|RSF_STEREO) // stereo 16-bit signed
#define RS_STIPCM16U	(RS_PCM16U|0x40|RSF_STEREO) // stereo 16-bit unsigned
#define RS_STIPCM16M	(RS_PCM16M|0x40|RSF_STEREO) // stereo 16-bit signed big endian
// 24-bit signed
#define RS_PCM24S	(RS_PCM16S|0x80)	    // mono 24-bit signed
#define RS_STIPCM24S	(RS_PCM16S|0x80|RSF_STEREO) // stereo 24-bit signed
#define RS_PCM32S	(RS_PCM16S|0xC0)	    // mono 24-bit signed
#define RS_STIPCM32S	(RS_PCM16S|0xC0|RSF_STEREO) // stereo 24-bit signed

// NNA types
#define NNA_NOTECUT	0
#define NNA_CONTINUE	1
#define NNA_NOTEOFF	2
#define NNA_NOTEFADE	3

// DCT types
#define DCT_NONE	0
#define DCT_NOTE	1
#define DCT_SAMPLE	2
#define DCT_INSTRUMENT	3

// DNA types
#define DNA_NOTECUT	0
#define DNA_NOTEOFF	1
#define DNA_NOTEFADE	2

// Mixer Hardware-Dependent features
#define SYSMIX_ENABLEMMX    0x01
#define SYSMIX_WINDOWSNT    0x02
#define SYSMIX_SLOWCPU	    0x04
#define SYSMIX_FASTCPU	    0x08

// Module flags
#define SONG_EMBEDMIDICFG   0x0001
#define SONG_FASTVOLSLIDES  0x0002
#define SONG_ITOLDEFFECTS   0x0004
#define SONG_ITCOMPATMODE   0x0008
#define SONG_LINEARSLIDES   0x0010
#define SONG_PATTERNLOOP    0x0020
#define SONG_STEP	    0x0040
#define SONG_PAUSED	    0x0080
#define SONG_FADINGSONG	    0x0100
#define SONG_ENDREACHED	    0x0200
#define SONG_GLOBALFADE	    0x0400
#define SONG_CPUVERYHIGH    0x0800
#define SONG_FIRSTTICK	    0x1000
#define SONG_MPTFILTERMODE  0x2000
#define SONG_SURROUNDPAN    0x4000
#define SONG_EXFILTERRANGE  0x8000
#define SONG_AMIGALIMITS    0x10000

// Global Options (Renderer)
#define SNDMIX_REVERSESTEREO	0x0001
#define SNDMIX_NOISEREDUCTION	0x0002
#define SNDMIX_AGC		0x0004
#define SNDMIX_NORESAMPLING	0x0008
#define SNDMIX_HQRESAMPLER	0x0010
#define SNDMIX_MEGABASS		0x0020
#define SNDMIX_SURROUND		0x0040
#define SNDMIX_REVERB		0x0080
#define SNDMIX_EQ		0x0100
#define SNDMIX_SOFTPANNING	0x0200
#define SNDMIX_ULTRAHQSRCMODE	0x0400
// Misc Flags (can safely be turned on or off)
#define SNDMIX_DIRECTTODISK	0x10000
#define SNDMIX_ENABLEMMX	0x20000
#define SNDMIX_NOBACKWARDJUMPS	0x40000
#define SNDMIX_MAXDEFAULTPAN	0x80000 // Used by the MOD loader


// Reverb Types (GM2 Presets)
enum {
    REVERBTYPE_SMALLROOM,
    REVERBTYPE_MEDIUMROOM,
    REVERBTYPE_LARGEROOM,
    REVERBTYPE_SMALLHALL,
    REVERBTYPE_MEDIUMHALL,
    REVERBTYPE_LARGEHALL,
    NUM_REVERBTYPES
};


enum {
    SRCMODE_NEAREST,
    SRCMODE_LINEAR,
    SRCMODE_SPLINE,
    SRCMODE_POLYPHASE,
    NUM_SRC_MODES
};


// Sample Struct
typedef struct _MODINSTRUMENT
{
    unsigned nLength;
    unsigned nLoopStart;
    unsigned nLoopEnd;
    unsigned nSustainStart;
    unsigned nSustainEnd;
    int8_t *pSample;
    unsigned nC4Speed;
    uint16_t nPan;
    uint16_t nVolume;
    uint16_t nGlobalVol;
    uint16_t uFlags;
    int8_t RelativeTone;
    int8_t nFineTune;
    uint8_t nVibType;
    uint8_t nVibSweep;
    uint8_t nVibDepth;
    uint8_t nVibRate;
    int8_t Name[22];
} MODINSTRUMENT;


// Instrument Struct
typedef struct _INSTRUMENTHEADER
{
    unsigned nFadeOut;
    uint32_t dwFlags;
    uint16_t nGlobalVol;
    uint16_t nPan;
    uint16_t VolPoints[MAX_ENVPOINTS];
    uint16_t PanPoints[MAX_ENVPOINTS];
    uint16_t PitchPoints[MAX_ENVPOINTS];
    uint8_t VolEnv[MAX_ENVPOINTS];
    uint8_t PanEnv[MAX_ENVPOINTS];
    uint8_t PitchEnv[MAX_ENVPOINTS];
    uint8_t Keyboard[128];
    uint8_t NoteMap[128];

    uint8_t nVolEnv;
    uint8_t nPanEnv;
    uint8_t nPitchEnv;
    uint8_t nVolLoopStart;
    uint8_t nVolLoopEnd;
    uint8_t nVolSustainBegin;
    uint8_t nVolSustainEnd;
    uint8_t nPanLoopStart;
    uint8_t nPanLoopEnd;
    uint8_t nPanSustainBegin;
    uint8_t nPanSustainEnd;
    uint8_t nPitchLoopStart;
    uint8_t nPitchLoopEnd;
    uint8_t nPitchSustainBegin;
    uint8_t nPitchSustainEnd;
    uint8_t nNNA;
    uint8_t nDCT;
    uint8_t nDNA;
    uint8_t nPanSwing;
    uint8_t nVolSwing;
    uint8_t nIFC;
    uint8_t nIFR;
    uint16_t wMidiBank;
    uint8_t nMidiProgram;
    uint8_t nMidiChannel;
    uint8_t nMidiDrumKey;
    int8_t nPPS;
    uint8_t nPPC;
    int8_t Name[32];
    int8_t Filename[12];
} INSTRUMENTHEADER;


// Channel Struct
typedef struct _MODCHANNEL
{
    // First 32-bytes: Most used mixing information: don't change it
    int8_t * pCurrentSample;
    uint32_t nPos;
    uint32_t nPosLo;	// actually 16-bit
    long nInc;		// 16.16
    long nRightVol;
    long nLeftVol;
    long nRightRamp;
    long nLeftRamp;
    // 2nd cache line
    uint32_t nLength;
    uint32_t dwFlags;
    uint32_t nLoopStart;
    uint32_t nLoopEnd;
    long nRampRightVol;
    long nRampLeftVol;
    long nFilter_Y1, nFilter_Y2, nFilter_Y3, nFilter_Y4;
    long nFilter_A0, nFilter_B0, nFilter_B1;
    long nROfs, nLOfs;
    long nRampLength;
    // Information not used in the mixer
    int8_t * pSample;
    long nNewRightVol, nNewLeftVol;
    long nRealVolume, nRealPan;
    long nVolume, nPan, nFadeOutVol;
    long nPeriod, nC4Speed, nPortamentoDest;
    INSTRUMENTHEADER *pHeader;
    MODINSTRUMENT *pInstrument;
    uint32_t nVolEnvPosition, nPanEnvPosition, nPitchEnvPosition;
    uint32_t nMasterChn, nVUMeter;
    long nGlobalVol, nInsVol;
    long nFineTune, nTranspose;
    long nPortamentoSlide, nAutoVibDepth;
    unsigned nAutoVibPos, nVibratoPos, nTremoloPos, nPanbrelloPos;
    // 16-bit members
    signed short nVolSwing, nPanSwing;
    // 8-bit members
    uint8_t nNote, nNNA;
    uint8_t nNewNote, nNewIns, nCommand, nArpeggio;
    uint8_t nOldVolumeSlide, nOldFineVolUpDown;
    uint8_t nOldPortaUpDown, nOldFinePortaUpDown;
    uint8_t nOldPanSlide, nOldChnVolSlide;
    uint8_t nVibratoType, nVibratoSpeed, nVibratoDepth;
    uint8_t nTremoloType, nTremoloSpeed, nTremoloDepth;
    uint8_t nPanbrelloType, nPanbrelloSpeed, nPanbrelloDepth;
    uint8_t nOldCmdEx, nOldVolParam, nOldTempo;
    uint8_t nOldOffset, nOldHiOffset;
    uint8_t nCutOff, nResonance;
    uint8_t nRetrigCount, nRetrigParam;
    uint8_t nTremorCount, nTremorParam;
    uint8_t nPatternLoop, nPatternLoopCount;
    uint8_t nRowNote, nRowInstr;
    uint8_t nRowVolCmd, nRowVolume;
    uint8_t nRowCommand, nRowParam;
    uint8_t nLeftVU, nRightVU;
    uint8_t nActiveMacro, nPadding;
} MODCHANNEL;


typedef struct _MODCHANNELSETTINGS
{
    unsigned nPan;
    unsigned nVolume;
    uint32_t dwFlags;
    unsigned nMixPlugin;
    char szName[MAX_CHANNELNAME];	     // changed from CHAR
} MODCHANNELSETTINGS;


typedef struct _MODCOMMAND
{
    uint8_t note;
    uint8_t instr;
    uint8_t volcmd;
    uint8_t command;
    uint8_t vol;
    uint8_t param;
} MODCOMMAND;

////////////////////////////////////////////////////////////////////
// Mix Plugins
#define MIXPLUG_MIXREADY	    0x01    // Set when cleared

#ifndef MODPLUG_NO_IMIXPLUGIN

class IMixPlugin
{
public:
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual void SaveAllParameters() = 0;
    virtual void RestoreAllParameters() = 0;
    virtual void Process(float *pOutL, float *pOutR, unsigned long nSamples) = 0;
    virtual void Init(unsigned long nFreq, int bReset) = 0;
    virtual void MidiSend(uint32_t dwMidiCode) = 0;
    virtual void MidiCommand(unsigned nMidiCh, unsigned nMidiProg, unsigned note, unsigned vol) = 0;
};

#endif


#define MIXPLUG_INPUTF_MASTEREFFECT	0x01	// Apply to master mix
#define MIXPLUG_INPUTF_BYPASS		0x02	// Bypass effect
#define MIXPLUG_INPUTF_WETMIX		0x04	// Wet Mix (dry added)

typedef struct _SNDMIXPLUGINSTATE
{
    uint32_t dwFlags;		    // MIXPLUG_XXXX
    long nVolDecayL, nVolDecayR;    // Buffer click removal
    int *pMixBuffer;		    // Stereo effect send buffer
    float *pOutBufferL;		    // Temp storage for int -> float conversion
    float *pOutBufferR;
} SNDMIXPLUGINSTATE;

typedef struct _SNDMIXPLUGININFO
{
    uint32_t dwPluginId1;
    uint32_t dwPluginId2;
    uint32_t dwInputRouting;   // MIXPLUG_INPUTF_XXXX
    uint32_t dwOutputRouting;  // 0=mix 0x80+=fx
    uint32_t dwReserved[4];    // Reserved for routing info
    int8_t szName[32];
    int8_t szLibraryName[64]; // original DLL name
} SNDMIXPLUGININFO; // Size should be 128

typedef struct _SNDMIXPLUGIN
{
#ifndef MODPLUG_NO_IMIXPLUGIN
    IMixPlugin *pMixPlugin;
#endif
    SNDMIXPLUGINSTATE* pMixState;
    uint32_t nPluginDataSize;
    void* pPluginData;
    SNDMIXPLUGININFO Info;
} SNDMIXPLUGIN;

////////////////////////////////////////////////////////////////////

enum {
    MIDIOUT_START=0,
    MIDIOUT_STOP,
    MIDIOUT_TICK,
    MIDIOUT_NOTEON,
    MIDIOUT_NOTEOFF,
    MIDIOUT_VOLUME,
    MIDIOUT_PAN,
    MIDIOUT_BANKSEL,
    MIDIOUT_PROGRAM,
};


typedef struct MODMIDICFG
{
    char szMidiGlb[9*32];		// changed from CHAR
    char szMidiSFXExt[16*32];		// changed from CHAR
    char szMidiZXXExt[128*32];		// changed from CHAR
} MODMIDICFG;


//=============================================================================
// Old CSoundFile static members

extern unsigned CSoundFile_XBassDepth;
extern unsigned CSoundFile_XBassRange;
extern unsigned CSoundFile_ReverbDepth;
extern unsigned CSoundFile_ReverbDelay;
extern unsigned CSoundFile_gnReverbType;
extern unsigned CSoundFile_ProLogicDepth;
extern unsigned CSoundFile_ProLogicDelay;
extern unsigned CSoundFile_StereoSeparation;
extern unsigned CSoundFile_MaxMixChannels;
extern long CSoundFile_StreamVolume;
extern uint32_t CSoundFile_gdwSysInfo;
extern uint32_t CSoundFile_gdwSoundSetup;
extern uint32_t CSoundFile_gdwMixingFreq;
extern uint32_t CSoundFile_gnBitsPerSample;
extern uint32_t CSoundFile_gnChannels;
#ifndef MODPLUG_NO_AGC
extern unsigned CSoundFile_gnAGC;
#endif
extern unsigned CSoundFile_gnVolumeRampSamples;
extern unsigned CSoundFile_gnVUMeter;
extern unsigned CSoundFile_gnCPUUsage;
extern void (*CSoundFile_gpSndMixHook)(int *buffer, unsigned long samples, unsigned long channels);
extern int (*CSoundFile_gpMixPluginCreateProc)(SNDMIXPLUGIN*);

//=============================================================================
typedef struct _c_sound_file_
{
    // for Editing
    MODCHANNEL Chn[MAX_CHANNELS];		    // Channels
    unsigned ChnMix[MAX_CHANNELS];		    // Channels to be mixed
    MODINSTRUMENT Ins[MAX_SAMPLES];		    // Instruments
    INSTRUMENTHEADER *Headers[MAX_INSTRUMENTS];	    // Instrument Headers
    MODCHANNELSETTINGS ChnSettings[MAX_BASECHANNELS]; // Channels settings
    MODCOMMAND *Patterns[MAX_PATTERNS];		    // Patterns
    uint16_t PatternSize[MAX_PATTERNS];		    // Patterns Lengths
    uint8_t Order[MAX_ORDERS];			    // Pattern Orders
    MODMIDICFG m_MidiCfg;			    // Midi macro config table
    SNDMIXPLUGIN m_MixPlugins[MAX_MIXPLUGINS];	    // Mix plugins
    unsigned m_nDefaultSpeed;
    unsigned m_nDefaultTempo;
    unsigned m_nDefaultGlobalVolume;
    uint32_t m_dwSongFlags;			    // Song flags SONG_XXXX
    unsigned m_nChannels;
    unsigned m_nMixChannels;
    unsigned m_nMixStat;
    unsigned m_nBufferCount;
    unsigned m_nType;
    unsigned m_nSamples;
    unsigned m_nInstruments;
    unsigned m_nTickCount;
    unsigned m_nTotalCount;
    unsigned m_nPatternDelay;
    unsigned m_nFrameDelay;
    unsigned m_nMusicSpeed;
    unsigned m_nMusicTempo;
    unsigned m_nNextRow;
    unsigned m_nRow;
    unsigned m_nPattern,m_nCurrentPattern,m_nNextPattern,m_nRestartPos;
    unsigned m_nMasterVolume;
    unsigned m_nGlobalVolume;
    unsigned m_nSongPreAmp;
    unsigned m_nFreqFactor;
    unsigned m_nTempoFactor;
    unsigned m_nOldGlbVolSlide;
    long m_nMinPeriod, m_nMaxPeriod, m_nRepeatCount, m_nInitialRepeatCount;
    uint32_t m_nGlobalFadeSamples, m_nGlobalFadeMaxSamples;
    unsigned m_nMaxOrderPosition;
    unsigned m_nPatternNames;
    char* m_lpszSongComments;
    char* m_lpszPatternNames;
    char m_szNames[MAX_INSTRUMENTS][32];    // changed from CHAR
    int8_t CompressionTable[16];
} CSoundFile;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

extern long CSoundFile_gnDryROfsVol;
extern long CSoundFile_gnDryLOfsVol;
extern long CSoundFile_gnRvbROfsVol;
extern long CSoundFile_gnRvbLOfsVol;
#ifndef MODPLUG_NO_REVERB
extern unsigned CSoundFile_gnReverbSend;
#endif

//----------------------------------------------------------------------------

extern int CSoundFile_Create(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_Destroy(CSoundFile * that);
static inline unsigned CSoundFile_GetType(CSoundFile const *that)
{
    return that->m_nType;
}
extern unsigned CSoundFile_GetNumChannels(CSoundFile const * that);
static inline unsigned CSoundFile_GetLogicalChannels(CSoundFile const * that)
{
    return that->m_nChannels;
}
extern int CSoundFile_SetMasterVolume(CSoundFile * that, unsigned vol,
    int bAdjustAGC);
static inline unsigned CSoundFile_GetMasterVolume(CSoundFile const * that)
{
    return that->m_nMasterVolume;
}
extern unsigned CSoundFile_GetNumPatterns(CSoundFile const *that);
extern unsigned CSoundFile_GetNumInstruments(CSoundFile const *that);
static inline unsigned CSoundFile_GetNumSamples(CSoundFile const *that)
{
    return that->m_nSamples;
}
extern unsigned CSoundFile_GetCurrentPos(CSoundFile const * that);
static inline unsigned CSoundFile_GetCurrentPattern(CSoundFile const *that)
{
    return that->m_nPattern;
}
static inline unsigned CSoundFile_GetCurrentOrder(CSoundFile const *that)
{
    return that->m_nCurrentPattern;
}
extern unsigned CSoundFile_GetSongComments(CSoundFile const * that, char *s,
    unsigned cbsize, unsigned linesize);
extern unsigned CSoundFile_GetRawSongComments(CSoundFile const * that, char *s,
    unsigned cbsize, unsigned linesize);
extern unsigned CSoundFile_GetMaxPosition(CSoundFile const * that);
extern void CSoundFile_SetCurrentPos(CSoundFile * that, unsigned nPos);
extern void CSoundFile_SetCurrentOrder(CSoundFile * that, unsigned nOrder);
static inline const char *CSoundFile_GetTitle(CSoundFile * that)
{
    return that->m_szNames[0];
}
extern unsigned CSoundFile_GetSampleName(CSoundFile * that, unsigned nSample,
    char *s);
extern unsigned CSoundFile_GetInstrumentName(CSoundFile * that,
    unsigned nInstr, char *s);
static inline unsigned CSoundFile_GetMusicSpeed(CSoundFile * that)
{
    return that->m_nMusicSpeed;
}
static inline unsigned CSoundFile_GetMusicTempo(CSoundFile * that)
{
    return that->m_nMusicTempo;
}
extern uint32_t CSoundFile_GetLength(CSoundFile * that, int bAdjust,
    int bTotal);
static inline uint32_t CSoundFile_GetSongTime(CSoundFile * that)
{
    return CSoundFile_GetLength(that, 0, 1);
}
static inline void CSoundFile_SetRepeatCount(CSoundFile * that, int n)
{
    that->m_nRepeatCount = n;
    that->m_nInitialRepeatCount = n;
}
static inline int CSoundFile_GetRepeatCount(CSoundFile * that)
{
    return that->m_nRepeatCount;
}
static inline int CSoundFile_IsPaused(CSoundFile * that)
{
    return (that->m_dwSongFlags & SONG_PAUSED) ? 1 : 0;
}
extern void CSoundFile_LoopPattern(CSoundFile * that, int nPat, int nRow);
extern void CSoundFile_CheckCPUUsage(CSoundFile * that, unsigned nCPU);
extern int CSoundFile_SetPatternName(CSoundFile * that, unsigned nPat,
    const char *lpszName);
extern int CSoundFile_GetPatternName(CSoundFile const * that, unsigned nPat,
    char *lpszName, unsigned cbSize);
// Module Loaders
extern int CSoundFile_ReadXM(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadS3M(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadMod(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadMed(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadMTM(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadSTM(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadIT(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_Read669(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadUlt(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadWav(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadDSM(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadFAR(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadAMS(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadMDL(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadOKT(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadDMF(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadPTM(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadDBM(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadAMF(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadMT2(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadPSM(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadJ2B(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);
extern int CSoundFile_ReadUMX(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength);

// Save Functions
#ifndef MODPLUG_NO_FILESAVE
extern unsigned CSoundFile_WriteSample(CSoundFile const *that, FILE * f,
    MODINSTRUMENT const *pins, unsigned nFlags, unsigned nMaxLen);
extern int CSoundFile_SaveXM(CSoundFile * that, const char *lpszFileName,
    unsigned nPacking);
extern int CSoundFile_SaveS3M(CSoundFile * that, const char *lpszFileName,
    unsigned nPacking);
extern int CSoundFile_SaveMod(CSoundFile * that, const char *lpszFileName,
    unsigned nPacking);
extern int CSoundFile_SaveIT(CSoundFile * that, const char *lpszFileName,
    unsigned nPacking);
#endif // MODPLUG_NO_FILESAVE
// MOD Convert function
extern unsigned CSoundFile_GetBestSaveFormat(CSoundFile * that);
extern unsigned CSoundFile_GetSaveFormats(CSoundFile * that);
extern void CSoundFile_ConvertModCommand(CSoundFile const *const that,
    MODCOMMAND * m);
extern void CSoundFile_S3MConvert(MODCOMMAND * m, int bIT);
extern void CSoundFile_S3MSaveConvert(CSoundFile const * that, unsigned *pcmd, unsigned *pprm, int bIT);
extern unsigned CSoundFile_ModSaveCommand(CSoundFile const * that,
    const MODCOMMAND * m, int bXM);

// Real-time sound functions
extern void CSoundFile_ResetChannels(CSoundFile * that);

extern unsigned CSoundFile_Read(CSoundFile* that,void *lpBuffer, unsigned cbBuffer);
extern unsigned CSoundFile_CreateStereoMix(CSoundFile* that,int count);
extern int CSoundFile_FadeSong(CSoundFile * that, unsigned msec);
extern int CSoundFile_GlobalFadeSong(CSoundFile* that, unsigned msec);
static inline unsigned CSoundFile_GetTotalTickCount(CSoundFile const * that)
{
    return that->m_nTotalCount;
}
static inline void CSoundFile_ResetTotalTickCount(CSoundFile * that)
{
    that->m_nTotalCount = 0;
}

// Mixer Config
extern int CSoundFile_InitPlayer(int bReset);
extern int CSoundFile_SetWaveConfig(CSoundFile* that,unsigned nRate,
    unsigned nBits, unsigned nChannels, int bMMX);
extern int CSoundFile_SetResamplingMode(CSoundFile * that,unsigned nMode);
static inline int CSoundFile_IsStereo(CSoundFile const * that __attribute__((unused)))
{
    return (CSoundFile_gnChannels > 1) ? 1 : 0;
}
static inline uint32_t CSoundFile_GetSampleRate(CSoundFile const * that __attribute__((unused)))
{
    return CSoundFile_gdwMixingFreq;
}
static inline uint32_t CSoundFile_GetBitsPerSample(CSoundFile const * that __attribute__((unused)))
{
    return CSoundFile_gnBitsPerSample;
}
extern uint32_t CSoundFile_InitSysInfo(void);
static inline uint32_t CSoundFile_GetSysInfo(CSoundFile const * that __attribute__((unused)))
{
    return CSoundFile_gdwSysInfo;
}

// AGC
static inline int CSoundFile_GetAGC(CSoundFile const * that __attribute__((unused)))
{
    return (CSoundFile_gdwSoundSetup & SNDMIX_AGC) ? 1 : 0;
}
extern void CSoundFile_SetAGC(int b);
extern void CSoundFile_ResetAGC(void);
extern void CSoundFile_ProcessAGC(int count);

//GCCFIX -- added these functions back in!
extern int CSoundFile_SetWaveConfigEx(CSoundFile * that, int bSurround,
    int bNoOverSampling, int bReverb, int hqido, int bMegaBass, int bNR,
    int bEQ);
// DSP Effects
extern void CSoundFile_InitializeDSP(int bReset);
extern void CSoundFile_ProcessStereoDSP(int count);
extern void CSoundFile_ProcessMonoDSP(int count);

// [Reverb level 0(quiet)-100(loud)], [delay in ms, usually 40-200ms]
extern int CSoundFile_SetReverbParameters(CSoundFile * that, unsigned nDepth,
    unsigned nDelay);

// [XBass level 0(quiet)-100(loud)], [cutoff in Hz 10-100]
extern int CSoundFile_SetXBassParameters(CSoundFile * that, unsigned nDepth,
    unsigned nRange);

// [Surround level 0(quiet)-100(heavy)] [delay in ms, usually 5-40ms]
extern int CSoundFile_SetSurroundParameters(CSoundFile * that, unsigned nDepth,
    unsigned nDelay);
extern int CSoundFile_ReadNote(CSoundFile * that);
extern int CSoundFile_ProcessRow(CSoundFile * that);
extern int CSoundFile_ProcessEffects(CSoundFile * that);
extern unsigned CSoundFile_GetNNAChannel(CSoundFile const * that,
    unsigned nChn);
extern void CSoundFile_CheckNNA(CSoundFile * that, unsigned nChn,
    unsigned instr, int note, int bForceCut);
extern void CSoundFile_NoteChange(CSoundFile * that, unsigned nChn, int note,
    int bPorta, int bResetEnv);
extern void CSoundFile_InstrumentChange(CSoundFile * that, MODCHANNEL * pChn,
    unsigned instr, int bPorta, int bUpdVol, int bResetEnv);
// Channel Effects
extern void CSoundFile_PortamentoUp(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param);
extern void CSoundFile_PortamentoDown(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param);
extern void CSoundFile_FinePortamentoUp(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param);
extern void CSoundFile_FinePortamentoDown(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param);
extern void CSoundFile_ExtraFinePortamentoUp(CSoundFile * that,
    MODCHANNEL * pChn, unsigned param);
extern void CSoundFile_ExtraFinePortamentoDown(CSoundFile * that,
    MODCHANNEL * pChn, unsigned param);
extern void CSoundFile_TonePortamento(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param);
extern void CSoundFile_Vibrato(MODCHANNEL * pChn, unsigned param);
extern void CSoundFile_FineVibrato(MODCHANNEL * pChn, unsigned param);
extern void CSoundFile_VolumeSlide(CSoundFile* that, MODCHANNEL * pChn, unsigned param);
extern void CSoundFile_PanningSlide(CSoundFile* that, MODCHANNEL * pChn, unsigned param);
extern void CSoundFile_ChannelVolSlide(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param);
extern void CSoundFile_FineVolumeUp(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param);
extern void CSoundFile_FineVolumeDown(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param);
extern void CSoundFile_Tremolo(MODCHANNEL * pChn,
    unsigned param);
extern void CSoundFile_Panbrello(MODCHANNEL * pChn, unsigned param);
extern void CSoundFile_RetrigNote(CSoundFile * that, unsigned nChn,
    unsigned param);
extern void CSoundFile_NoteCut(CSoundFile * that, unsigned nChn,
    unsigned nTick);
extern void CSoundFile_KeyOff(CSoundFile * that, unsigned nChn);
extern int CSoundFile_PatternLoop(CSoundFile * that, MODCHANNEL *,
    unsigned param);
extern void CSoundFile_ExtendedMODCommands(CSoundFile * that, unsigned nChn,
    unsigned param);
extern void CSoundFile_ExtendedS3MCommands(CSoundFile * that, unsigned nChn,
    unsigned param);
extern void CSoundFile_ExtendedChannelEffect(CSoundFile * that, MODCHANNEL *,
    unsigned param);
extern void CSoundFile_ProcessMidiMacro(CSoundFile * that, unsigned nChn,
    const char *pszMidiMacro, unsigned param);
extern void CSoundFile_SetupChannelFilter(CSoundFile const *that,
    MODCHANNEL * pChn, int bReset, int flt_modifier);
// Low-Level effect processing
extern void CSoundFile_DoFreqSlide(CSoundFile * that, MODCHANNEL * pChn, long nFreqSlide);

// Global Effects
extern void CSoundFile_SetTempo(CSoundFile * that, unsigned param);
extern void CSoundFile_SetSpeed(CSoundFile * that, unsigned param);
extern void CSoundFile_GlobalVolSlide(CSoundFile * that, unsigned param);
extern uint32_t CSoundFile_IsSongFinished(CSoundFile * that, unsigned nOrder,
    unsigned nRow);
extern int CSoundFile_IsValidBackwardJump(CSoundFile * that,
    unsigned nStartOrder, unsigned nStartRow, unsigned nJumpOrder,
    unsigned nJumpRow);
// Read/Write sample functions
static inline int8_t CSoundFile_GetDeltaValue(CSoundFile const *that,
    int8_t prev, unsigned n)
{
    return (int8_t) (prev + that->CompressionTable[n & 0x0F]);
}
extern int CSoundFile_CanPackSample(CSoundFile * that, char const *pSample,
    unsigned nLen, unsigned nPacking, uint8_t * result);
extern unsigned CSoundFile_ReadSample(CSoundFile * that, MODINSTRUMENT * pIns,
    unsigned nFlags, const char *pMemFile, uint32_t dwMemLength);
extern int CSoundFile_DestroySample(CSoundFile * that, unsigned nSample);
extern int CSoundFile_DestroyInstrument(CSoundFile * that, unsigned nInstr);
extern int CSoundFile_IsSampleUsed(CSoundFile * that, unsigned nSample);
extern int CSoundFile_IsInstrumentUsed(CSoundFile * that, unsigned nInstr);
extern int CSoundFile_RemoveInstrumentSamples(CSoundFile * that,
    unsigned nInstr);
extern unsigned CSoundFile_DetectUnusedSamples(CSoundFile * that, int *);
extern int CSoundFile_RemoveSelectedSamples(CSoundFile * that, int *);
extern void CSoundFile_AdjustSampleLoop(CSoundFile * that,
    MODINSTRUMENT * pIns);

// I/O from another sound file
extern int CSoundFile_ReadInstrumentFromSong(unsigned nInstr, CSoundFile *,
    unsigned nSrcInstrument);
extern int CSoundFile_ReadSampleFromSong(unsigned nSample, CSoundFile *,
    unsigned nSrcSample);
// Period/Note functions
extern unsigned CSoundFile_GetNoteFromPeriod(CSoundFile const *that,
    unsigned period);
extern unsigned CSoundFile_GetPeriodFromNote(CSoundFile const *that,
    unsigned note, int nFineTune, unsigned nC4Speed);
extern unsigned CSoundFile_GetFreqFromPeriod(CSoundFile const *that,
    unsigned period, unsigned nC4Speed, int nPeriodFrac);
// Misc functions
static inline MODINSTRUMENT *CSoundFile_GetSample(CSoundFile * that,
    unsigned n)
{
    return that->Ins + n;
}
extern void CSoundFile_ResetMidiCfg(CSoundFile * that);
extern unsigned CSoundFile_MapMidiInstrument(uint32_t dwProgram,
    unsigned nChannel, unsigned nNote);
extern unsigned CSoundFile_SaveMixPlugins(CSoundFile const *that, FILE * f,
    int bUpdate);
extern unsigned CSoundFile_LoadMixPlugins(CSoundFile * that, const void *pData,
    unsigned nLen);

// Static helper functions
extern uint32_t CSoundFile_TransposeToFrequency(int transp, int ftune);
extern int CSoundFile_FrequencyToTranspose(uint32_t freq);
extern void CSoundFile_FrequencyToTranspose_Instrument(MODINSTRUMENT * psmp);

// System-Dependant functions
extern MODCOMMAND *CSoundFile_AllocatePattern(unsigned rows, unsigned nchns);
extern void CSoundFile_FreePattern(MODCOMMAND * pat);
extern int8_t *CSoundFile_AllocateSample(unsigned nbytes);
extern void CSoundFile_FreeSample(void *p);
extern unsigned CSoundFile_Normalize24BitBuffer(uint8_t * pbuffer,
    unsigned cbsizebytes, uint32_t lmax24, uint32_t dwByteInc);

#ifdef MODPLUG_MMCMP_SUPPORT
extern int MMCMP_Unpack(const uint8_t ** ppMemFile,
    uint32_t * pdwMemLength);
#endif

// External decompressors
extern void CSoundFile_AMSUnpack(const char *psrc, unsigned inputlen,
    char *pdest, unsigned dmax, char packcharacter);
extern uint16_t MDLReadBits(uint32_t * bitbuf, unsigned * bitnum,
    uint8_t ** ibuf, int8_t n);
extern int CSoundFile_DMFUnpack(uint8_t * psample, uint8_t * ibuf,
    uint8_t * ibufmax, unsigned maxlen);
extern uint32_t CSoundFile_ITReadBits(uint32_t * bitbuf, unsigned *bitnum,
    uint8_t ** ibuf, int8_t n);
extern void CSoundFile_ITUnpack8Bit(int8_t * pSample, uint32_t dwLen,
    uint8_t * lpMemFile, uint32_t dwMemLength, int b215);
extern void CSoundFile_ITUnpack16Bit(int8_t * pSample, uint32_t dwLen,
    uint8_t * lpMemFile, uint32_t dwMemLength, int b215);


//////////////////////////////////////////////////////////
// WAVE format information

#pragma pack(1)

// Standard IFF chunks IDs
#define IFFID_FORM	0x4d524f46
#define IFFID_RIFF	0x46464952
#define IFFID_WAVE	0x45564157
#define IFFID_LIST	0x5453494C
#define IFFID_INFO	0x4F464E49

// IFF Info fields
#define IFFID_ICOP	0x504F4349
#define IFFID_IART	0x54524149
#define IFFID_IPRD	0x44525049
#define IFFID_INAM	0x4D414E49
#define IFFID_ICMT	0x544D4349
#define IFFID_IENG	0x474E4549
#define IFFID_ISFT	0x54465349
#define IFFID_ISBJ	0x4A425349
#define IFFID_IGNR	0x524E4749
#define IFFID_ICRD	0x44524349

// Wave IFF chunks IDs
#define IFFID_wave	0x65766177
#define IFFID_fmt	0x20746D66
#define IFFID_wsmp	0x706D7377
#define IFFID_pcm	0x206d6370
#define IFFID_data	0x61746164
#define IFFID_smpl	0x6C706D73
#define IFFID_xtra	0x61727478

typedef struct WAVEFILEHEADER
{
    uint32_t id_RIFF;	// "RIFF"
    uint32_t filesize;	// file length-8
    uint32_t id_WAVE;
} WAVEFILEHEADER;


typedef struct WAVEFORMATHEADER
{
    uint32_t id_fmt;	// "fmt "
    uint32_t hdrlen;	// 16
    uint16_t format;	// 1
    uint16_t channels;	// 1:mono, 2:stereo
    uint32_t freqHz;	// sampling freq
    uint32_t bytessec;	// bytes/sec=freqHz*samplesize
    uint16_t samplesize;	// sizeof(sample)
    uint16_t bitspersample; // bits per sample (8/16)
} WAVEFORMATHEADER;


typedef struct WAVEDATAHEADER
{
    uint32_t id_data;	// "data"
    uint32_t length;	// length of data
} WAVEDATAHEADER;


typedef struct WAVESMPLHEADER
{
    // SMPL
    uint32_t smpl_id;	// "smpl"   -> 0x6C706D73
    uint32_t smpl_len;	// length of smpl: 3Ch	(54h with sustain loop)
    uint32_t dwManufacturer;
    uint32_t dwProduct;
    uint32_t dwSamplePeriod;   // 1000000000/freqHz
    uint32_t dwBaseNote;	// 3Ch = C-4 -> 60 + RelativeTone
    uint32_t dwPitchFraction;
    uint32_t dwSMPTEFormat;
    uint32_t dwSMPTEOffset;
    uint32_t dwSampleLoops;    // number of loops
    uint32_t cbSamplerData;
} WAVESMPLHEADER;


typedef struct SAMPLELOOPSTRUCT
{
    uint32_t dwIdentifier;
    uint32_t dwLoopType;	    // 0=normal, 1=bidi
    uint32_t dwLoopStart;
    uint32_t dwLoopEnd;	    // Byte offset ?
    uint32_t dwFraction;
    uint32_t dwPlayCount;	    // Loop Count, 0=infinite
} SAMPLELOOPSTRUCT;


typedef struct WAVESAMPLERINFO
{
    WAVESMPLHEADER wsiHdr;
    SAMPLELOOPSTRUCT wsiLoops[2];
} WAVESAMPLERINFO;


typedef struct WAVELISTHEADER
{
    uint32_t list_id;  // "LIST" -> 0x5453494C
    uint32_t list_len;
    uint32_t info;	    // "INFO"
} WAVELISTHEADER;


typedef struct WAVEEXTRAHEADER
{
    uint32_t xtra_id;  // "xtra"	-> 0x61727478
    uint32_t xtra_len;
    uint32_t dwFlags;
    uint16_t  wPan;
    uint16_t  wVolume;
    uint16_t  wGlobalVol;
    uint16_t  wReserved;
    uint8_t nVibType;
    uint8_t nVibSweep;
    uint8_t nVibDepth;
    uint8_t nVibRate;
} WAVEEXTRAHEADER;

#pragma pack()

///////////////////////////////////////////////////////////
// Low-level Mixing functions

#define MIXBUFFERSIZE	    512
#define MIXING_ATTENUATION  4
#define MIXING_CLIPMIN	    (-0x08000000)
#define MIXING_CLIPMAX	    (0x07FFFFFF)
#define VOLUMERAMPPRECISION 12
#define FADESONGDELAY	    100
#define EQ_BUFFERSIZE	    (MIXBUFFERSIZE)
#define AGC_PRECISION	    9
#define AGC_UNITY	    (1 << AGC_PRECISION)

// Calling conventions
#ifdef _MSC_VER
#define MPPASMCALL  __cdecl
#define MPPFASTCALL __fastcall
#else
#define MPPASMCALL
#define MPPFASTCALL
#endif

#define MOD2XMFineTune(k)   ((int)( (int8_t)((k)<<4) ))
#define XM2MODFineTune(k)   ((int)( (k>>4)&0x0f ))

extern int _muldiv(long a, long b, long c);
extern int _muldivr(long a, long b, long c);

// Access the main temporary mix buffer directly: avoids an extra pointer
extern int CSoundFile_MixSoundBuffer[MIXBUFFERSIZE * 4];
extern int CSoundFile_MixRearBuffer[MIXBUFFERSIZE * 2];
// Reverb Mix Buffer
#ifndef MODPLUG_NO_REVERB
extern int CSoundFile_MixReverbBuffer[MIXBUFFERSIZE*2];
#endif

// X86 Functions
extern uint32_t MPPASMCALL X86_Convert32To8(void *, const int *,
    uint32_t, long *, long *);
extern uint32_t MPPASMCALL X86_Convert32To16(void *, const int *,
    uint32_t, long *, long *);
extern uint32_t MPPASMCALL X86_Convert32To24(void *, const int *,
    uint32_t, long *, long *);
extern uint32_t MPPASMCALL X86_Convert32To32(void *, const int *,
    uint32_t, long *, long *);
extern unsigned MPPASMCALL X86_AGC(int *pBuffer, unsigned nSamples,
    unsigned nAGC);
extern void MPPASMCALL X86_Dither(int *pBuffer, unsigned nSamples,
    unsigned nBits);
extern void MPPASMCALL X86_InterleaveFrontRear(int *pFrontBuf, int *pRearBuf,
    uint32_t nSamples);
extern void MPPASMCALL X86_StereoFill(int *pBuffer, unsigned nSamples,
    long *lpROfs, long *lpLOfs);
extern void MPPASMCALL X86_MonoFromStereo(int *pMixBuf, unsigned nSamples);

extern void CSoundFile_CzCUBICSPLINE(void);
extern void CSoundFile_CzWINDOWEDFIR(void);

//----------------------------------------------------------------------------
// Byte swapping functions from the GNU C Library and libsdl

/* Swap bytes in 16 bit value.	*/
#ifdef __GNUC__
# define bswap_16(x) \
    (__extension__						\
    ({ uint16_t __bsx = (x);			    	\
    ((((__bsx) >> 8) & 0xff) | (((__bsx) & 0xff) << 8)); }))
#else
static __inline uint16_t bswap_16 (uint16_t __bsx)
{
    return ((((__bsx) >> 8) & 0xff) | (((__bsx) & 0xff) << 8));
}
#endif

/* Swap bytes in 32 bit value.	*/
#ifdef __GNUC__
# define bswap_32(x) \
    (__extension__						\
    ({ unsigned int __bsx = (x);				\
    ((((__bsx) & 0xff000000) >> 24) | (((__bsx) & 0x00ff0000) >>  8) |	\
    (((__bsx) & 0x0000ff00) <<	 8) | (((__bsx) & 0x000000ff) << 24)); }))
#else
static __inline unsigned int bswap_32 (unsigned int __bsx)
{
    return ((((__bsx) & 0xff000000) >> 24) | (((__bsx) & 0x00ff0000) >>  8) |
	(((__bsx) & 0x0000ff00) <<  8) | (((__bsx) & 0x000000ff) << 24));
}
#endif


// From libsdl
#ifdef WORDS_BIGENDIAN
#define bswapLE16(X) bswap_16(X)
#define bswapLE32(X) bswap_32(X)
#define bswapBE16(X) (X)
#define bswapBE32(X) (X)
#else
#define bswapLE16(X) (X)
#define bswapLE32(X) (X)
#define bswapBE16(X) bswap_16(X)
#define bswapBE32(X) bswap_32(X)
#endif

////////////////////////////////////////////////////////////////////
// Tables

// Tables defined in tables.c
extern uint8_t CSoundFile_ImpulseTrackerPortaVolCmd[16];
extern uint16_t CSoundFile_ProTrackerPeriodTable[6*12];
extern uint16_t CSoundFile_ProTrackerTunedPeriods[15*12];
extern uint16_t CSoundFile_FreqS3MTable[];
extern uint16_t CSoundFile_S3MFineTuneTable[16];
extern int16_t CSoundFile_ModSinusTable[64];
extern int16_t CSoundFile_ModRampDownTable[64];
extern int16_t CSoundFile_ModSquareTable[64];
extern int16_t CSoundFile_ModRandomTable[64];
extern int8_t CSoundFile_RetrigTable1[16];
extern int8_t CSoundFile_RetrigTable2[16];
extern uint16_t CSoundFile_XMPeriodTable[96+8];
extern uint32_t CSoundFile_XMLinearTable[768];
extern int8_t CSoundFile_Ft2VibratoTable[256];    // -64 .. +64

extern uint32_t CSoundFile_FineLinearSlideUpTable[16];
extern uint32_t CSoundFile_FineLinearSlideDownTable[16];
extern uint32_t CSoundFile_LinearSlideUpTable[256];
extern uint32_t CSoundFile_LinearSlideDownTable[256];
//extern int CSoundFile_SpectrumSinusTable[256*2];

#ifdef __cplusplus
}
#endif

#endif // __SNDFILE_H
