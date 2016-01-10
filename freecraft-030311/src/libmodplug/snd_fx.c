/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: snd_fx.c,v 1.3 2002/04/17 23:16:01 johns Exp $
*/

#include "stdafx.h"
#include "sndfile.h"
#ifndef __MINGW32__
#ifdef BSD
#include <inttypes.h>
#else
#include <stdint.h>
#endif // BSD

#endif // __MINGW32__
#include <stdlib.h>

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif

////////////////////////////////////////////////////////////
// Length

uint32_t CSoundFile_GetLength(CSoundFile * that, int bAdjust, int bTotal)
//----------------------------------------------------
{
    unsigned dwElapsedTime = 0;
    unsigned nRow = 0;
    unsigned nCurrentPattern = 0;
    unsigned nNextPattern = 0;
    unsigned nPattern = that->Order[0];
    unsigned nMusicSpeed = that->m_nDefaultSpeed;
    unsigned nMusicTempo = that->m_nDefaultTempo;
    unsigned nNextRow = 0;
    unsigned nMaxRow = 0;
    unsigned nMaxPattern = 0;
    long nGlbVol = that->m_nDefaultGlobalVolume;
    long nOldGlbVolSlide = 0;
    uint8_t samples[MAX_CHANNELS];
    uint8_t instr[MAX_CHANNELS];
    uint8_t notes[MAX_CHANNELS];
    uint8_t vols[MAX_CHANNELS];
    uint8_t oldparam[MAX_CHANNELS];
    uint8_t chnvols[MAX_CHANNELS];
    uint32_t patloop[MAX_CHANNELS];
    unsigned i;

    memset(instr, 0, sizeof(instr));
    memset(notes, 0, sizeof(notes));
    memset(vols, 0xFF, sizeof(vols));
    memset(patloop, 0, sizeof(patloop));
    memset(oldparam, 0, sizeof(oldparam));
    memset(chnvols, 64, sizeof(chnvols));
    memset(samples, 0, sizeof(samples));

    for (i = 0; i < that->m_nChannels; i++) {
	chnvols[i] = that->ChnSettings[i].nVolume;
    }
    nMaxRow = that->m_nNextRow;
    nMaxPattern = that->m_nNextPattern;
    nCurrentPattern = nNextPattern = 0;
    nPattern = that->Order[0];
    nRow = nNextRow = 0;

    for (;;) {
	MODCHANNEL *pChn;
	MODCOMMAND *p;
	unsigned nSpeedCount = 0;

	nRow = nNextRow;
	nCurrentPattern = nNextPattern;
	// Check if pattern is valid
	nPattern = that->Order[nCurrentPattern];
	while (nPattern >= MAX_PATTERNS) {
	    // End of song ?
	    if ((nPattern == 0xFF) || (nCurrentPattern >= MAX_ORDERS)) {
		goto EndMod;
	    } else {
		nCurrentPattern++;
		nPattern =
		    (nCurrentPattern <
		    MAX_ORDERS) ? that->Order[nCurrentPattern] : 0xFF;
	    }
	    nNextPattern = nCurrentPattern;
	}
	// Weird stuff?
	if ((nPattern >= MAX_PATTERNS) || (!that->Patterns[nPattern])) {
	    break;
	}
	// Should never happen
	if (nRow >= that->PatternSize[nPattern]) {
	    nRow = 0;
	}
	// Update next position
	nNextRow = nRow + 1;
	if (nNextRow >= that->PatternSize[nPattern]) {
	    nNextPattern = nCurrentPattern + 1;
	    nNextRow = 0;
	}
	if (!nRow) {
	    for (i = 0; i < that->m_nChannels; i++) {
		patloop[i] = dwElapsedTime;
	    }
	}
	if (!bTotal) {
	    if ((nCurrentPattern > nMaxPattern)
		|| ((nCurrentPattern == nMaxPattern) && (nRow >= nMaxRow))) {
		if (bAdjust) {
		    that->m_nMusicSpeed = nMusicSpeed;
		    that->m_nMusicTempo = nMusicTempo;
		}
		break;
	    }
	}
	pChn = that->Chn;
	p = that->Patterns[nPattern] + nRow * that->m_nChannels;

	for (i = 0; i < that->m_nChannels; p++, pChn++, i++)
	    if (*((uint32_t *) p)) {
		unsigned command = p->command;
		unsigned param = p->param;
		unsigned note = p->note;

		if (p->instr) {
		    instr[i] = p->instr;
		    notes[i] = 0;
		    vols[i] = 0xFF;
		}
		if ((note) && (note <= 120))
		    notes[i] = note;
		if (p->volcmd == VOLCMD_VOLUME) {
		    vols[i] = p->vol;
		}
		if (command)
		    switch (command) {
			    // Position Jump
			case CMD_POSITIONJUMP:
			    if (param <= nCurrentPattern)
				goto EndMod;
			    nNextPattern = param;
			    nNextRow = 0;
			    if (bAdjust) {
				pChn->nPatternLoopCount = 0;
				pChn->nPatternLoop = 0;
			    }
			    break;
			    // Pattern Break
			case CMD_PATTERNBREAK:
			    nNextRow = param;
			    nNextPattern = nCurrentPattern + 1;
			    if (bAdjust) {
				pChn->nPatternLoopCount = 0;
				pChn->nPatternLoop = 0;
			    }
			    break;
			    // Set Speed
			case CMD_SPEED:
			    if (!param)
				break;
			    if ((param <= 0x20)
				|| (that->m_nType != MOD_TYPE_MOD)) {
				if (param < 128)
				    nMusicSpeed = param;
			    }
			    break;
			    // Set Tempo
			case CMD_TEMPO:
			    if ((bAdjust)
				&& (that->
				    m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT))) {
				if (param)
				    pChn->nOldTempo = param;
				else
				    param = pChn->nOldTempo;
			    }
			    if (param >= 0x20)
				nMusicTempo = param;
			    else
				// Tempo Slide
			    if ((param & 0xF0) == 0x10) {
				nMusicTempo += param & 0x0F;
				if (nMusicTempo > 255)
				    nMusicTempo = 255;
			    } else {
				nMusicTempo -= param & 0x0F;
				if (nMusicTempo < 32)
				    nMusicTempo = 32;
			    }
			    break;
			    // Pattern Delay
			case CMD_S3MCMDEX:
			    if ((param & 0xF0) == 0x60) {
				nSpeedCount = param & 0x0F;
				break;
			    } else if ((param & 0xF0) == 0xB0) {
				param &= 0x0F;
				param |= 0x60;
			    }
			case CMD_MODCMDEX:
			    if ((param & 0xF0) == 0xE0)
				nSpeedCount = (param & 0x0F) * nMusicSpeed;
			    else if ((param & 0xF0) == 0x60) {
				if (param & 0x0F)
				    dwElapsedTime +=
					(dwElapsedTime -
					patloop[i]) * (param & 0x0F);
				else
				    patloop[i] = dwElapsedTime;
			    }
			    break;
		    }
		if (!bAdjust)
		    continue;
		switch (command) {
			// Portamento Up/Down
		    case CMD_PORTAMENTOUP:
		    case CMD_PORTAMENTODOWN:
			if (param)
			    pChn->nOldPortaUpDown = param;
			break;
			// Tone-Portamento
		    case CMD_TONEPORTAMENTO:
			if (param)
			    pChn->nPortamentoSlide = param << 2;
			break;
			// Offset
		    case CMD_OFFSET:
			if (param)
			    pChn->nOldOffset = param;
			break;
			// Volume Slide
		    case CMD_VOLUMESLIDE:
		    case CMD_TONEPORTAVOL:
		    case CMD_VIBRATOVOL:
			if (param)
			    pChn->nOldVolumeSlide = param;
			break;
			// Set Volume
		    case CMD_VOLUME:
			vols[i] = param;
			break;
			// Global Volume
		    case CMD_GLOBALVOLUME:
			if (that->m_nType != MOD_TYPE_IT)
			    param <<= 1;
			if (param > 128)
			    param = 128;
			nGlbVol = param << 1;
			break;
			// Global Volume Slide
		    case CMD_GLOBALVOLSLIDE:
			if (param)
			    nOldGlbVolSlide = param;
			else
			    param = nOldGlbVolSlide;
			if (((param & 0x0F) == 0x0F) && (param & 0xF0)) {
			    param >>= 4;
			    if (that->m_nType != MOD_TYPE_IT)
				param <<= 1;
			    nGlbVol += param << 1;
			} else if (((param & 0xF0) == 0xF0) && (param & 0x0F)) {
			    param = (param & 0x0F) << 1;
			    if (that->m_nType != MOD_TYPE_IT)
				param <<= 1;
			    nGlbVol -= param;
			} else if (param & 0xF0) {
			    param >>= 4;
			    param <<= 1;
			    if (that->m_nType != MOD_TYPE_IT)
				param <<= 1;
			    nGlbVol += param * nMusicSpeed;
			} else {
			    param = (param & 0x0F) << 1;
			    if (that->m_nType != MOD_TYPE_IT)
				param <<= 1;
			    nGlbVol -= param * nMusicSpeed;
			}
			if (nGlbVol < 0)
			    nGlbVol = 0;
			if (nGlbVol > 256)
			    nGlbVol = 256;
			break;
		    case CMD_CHANNELVOLUME:
			if (param <= 64)
			    chnvols[i] = param;
			break;
		    case CMD_CHANNELVOLSLIDE:
			if (param)
			    oldparam[i] = param;
			else
			    param = oldparam[i];
			pChn->nOldChnVolSlide = param;
			if (((param & 0x0F) == 0x0F) && (param & 0xF0)) {
			    param = (param >> 4) + chnvols[i];
			} else if (((param & 0xF0) == 0xF0) && (param & 0x0F)) {
			    if (chnvols[i] > (int)(param & 0x0F))
				param = chnvols[i] - (param & 0x0F);
			    else
				param = 0;
			} else if (param & 0x0F) {
			    param = (param & 0x0F) * nMusicSpeed;
			    param =
				(chnvols[i] >
				param) ? chnvols[i] - param : 0;
			} else
			    param =
				((param & 0xF0) >> 4) * nMusicSpeed +
				chnvols[i];
			if (param > 64)
			    param = 64;
			chnvols[i] = param;
			break;
		}
	    }
	nSpeedCount += nMusicSpeed;
	dwElapsedTime += (2500 * nSpeedCount) / nMusicTempo;
    }
  EndMod:
    if ((bAdjust) && (!bTotal)) {
	unsigned n;

	that->m_nGlobalVolume = nGlbVol;
	that->m_nOldGlbVolSlide = nOldGlbVolSlide;
	for (n = 0; n < that->m_nChannels; n++) {
	    that->Chn[n].nGlobalVol = chnvols[n];
	    if (notes[n])
		that->Chn[n].nNewNote = notes[n];
	    if (instr[n])
		that->Chn[n].nNewIns = instr[n];
	    if (vols[n] != 0xFF) {
		if (vols[n] > 64)
		    vols[n] = 64;
		that->Chn[n].nVolume = vols[n] << 2;
	    }
	}
    }
    return (dwElapsedTime + 500) / 1000;
}

///////////////////////////////////////////////////////////////////////////////
// Effects

void CSoundFile_InstrumentChange(CSoundFile * that, MODCHANNEL * pChn,
    unsigned instr, int bPorta, int bUpdVol, int bResetEnv)
//-----------------------------------------------------------------------------
{
    int bInstrumentChanged;
    INSTRUMENTHEADER *penv;
    MODINSTRUMENT *psmp;
    unsigned note;

    if (instr >= MAX_INSTRUMENTS) {
	return;
    }

    penv = that->Headers[instr];
    note = pChn->nNewNote;

    if (penv && note && note <= 128) {
	unsigned n;

	if (penv->NoteMap[note - 1] >= 0xFE) {
	    return;
	}
	n = penv->Keyboard[note - 1];

	psmp = ((n) && (n < MAX_SAMPLES)) ? &that->Ins[n] : NULL;
    } else if (that->m_nInstruments) {
	if (note >= 0xFE) {
	    return;
	}
	psmp = NULL;
    } else {
	psmp = &that->Ins[instr];
    }
    // Update Volume
    if (bUpdVol) {
	pChn->nVolume = (psmp) ? psmp->nVolume : 0;
    }
    // bInstrumentChanged is used for IT carry-on env option
    if (penv != pChn->pHeader) {
	bInstrumentChanged = 1;
	pChn->pHeader = penv;
    } else {
	bInstrumentChanged = 0;
	// Special XM hack
	if ((bPorta) && (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2))
	    && (penv)
	    && (pChn->pInstrument) && (psmp != pChn->pInstrument)) {
	    // FT2 doesn't change the sample in this case,
	    // but still uses the sample info from the old one (bug?)
	    return;
	}
    }
    // Instrument adjust
    pChn->nNewIns = 0;
    if (psmp) {
	if (penv) {
	    pChn->nInsVol = (psmp->nGlobalVol * penv->nGlobalVol) >> 6;
	    if (penv->dwFlags & ENV_SETPANNING)
		pChn->nPan = penv->nPan;
	    pChn->nNNA = penv->nNNA;
	} else {
	    pChn->nInsVol = psmp->nGlobalVol;
	}
	if (psmp->uFlags & CHN_PANNING)
	    pChn->nPan = psmp->nPan;
    }
    // Reset envelopes
    if (bResetEnv) {
	if ((!bPorta) || (!(that->m_nType & MOD_TYPE_IT))
	    || (that->m_dwSongFlags & SONG_ITCOMPATMODE)
	    || (!pChn->nLength) || ((pChn->dwFlags & CHN_NOTEFADE)
		&& (!pChn->nFadeOutVol))) {
	    pChn->dwFlags |= CHN_FASTVOLRAMP;
	    if ((that->m_nType & MOD_TYPE_IT) && (!bInstrumentChanged)
		&& (penv)
		&& (!(pChn->dwFlags & (CHN_KEYOFF | CHN_NOTEFADE)))) {
		if (!(penv->dwFlags & ENV_VOLCARRY))
		    pChn->nVolEnvPosition = 0;
		if (!(penv->dwFlags & ENV_PANCARRY))
		    pChn->nPanEnvPosition = 0;
		if (!(penv->dwFlags & ENV_PITCHCARRY))
		    pChn->nPitchEnvPosition = 0;
	    } else {
		pChn->nVolEnvPosition = 0;
		pChn->nPanEnvPosition = 0;
		pChn->nPitchEnvPosition = 0;
	    }
	    pChn->nAutoVibDepth = 0;
	    pChn->nAutoVibPos = 0;
	} else if ((penv) && (!(penv->dwFlags & ENV_VOLUME))) {
	    pChn->nVolEnvPosition = 0;
	    pChn->nAutoVibDepth = 0;
	    pChn->nAutoVibPos = 0;
	}
    }
    // Invalid sample ?
    if (!psmp) {
	pChn->pInstrument = NULL;
	pChn->nInsVol = 0;
	return;
    }
    // Tone-Portamento doesn't reset the pingpong direction flag
    if ((bPorta) && (psmp == pChn->pInstrument)) {
	if (that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT))
	    return;
	pChn->dwFlags &= ~(CHN_KEYOFF | CHN_NOTEFADE);
	pChn->dwFlags =
	    (pChn->dwFlags & (0xFFFFFF00 | CHN_PINGPONGFLAG)) | (psmp->uFlags);
    } else {
	pChn->dwFlags &=
	    ~(CHN_KEYOFF | CHN_NOTEFADE | CHN_VOLENV | CHN_PANENV |
	    CHN_PITCHENV);
	pChn->dwFlags = (pChn->dwFlags & 0xFFFFFF00) | (psmp->uFlags);
	if (penv) {
	    if (penv->dwFlags & ENV_VOLUME)
		pChn->dwFlags |= CHN_VOLENV;
	    if (penv->dwFlags & ENV_PANNING)
		pChn->dwFlags |= CHN_PANENV;
	    if (penv->dwFlags & ENV_PITCH)
		pChn->dwFlags |= CHN_PITCHENV;
	    if ((penv->dwFlags & ENV_PITCH) && (penv->dwFlags & ENV_FILTER)) {
		if (!pChn->nCutOff)
		    pChn->nCutOff = 0x7F;
	    }
	    if (penv->nIFC & 0x80)
		pChn->nCutOff = penv->nIFC & 0x7F;
	    if (penv->nIFR & 0x80)
		pChn->nResonance = penv->nIFR & 0x7F;
	}
	pChn->nVolSwing = pChn->nPanSwing = 0;
    }
    pChn->pInstrument = psmp;
    pChn->nLength = psmp->nLength;
    pChn->nLoopStart = psmp->nLoopStart;
    pChn->nLoopEnd = psmp->nLoopEnd;
    pChn->nC4Speed = psmp->nC4Speed;
    pChn->pSample = psmp->pSample;
    pChn->nTranspose = psmp->RelativeTone;
    pChn->nFineTune = psmp->nFineTune;
    if (pChn->dwFlags & CHN_SUSTAINLOOP) {
	pChn->nLoopStart = psmp->nSustainStart;
	pChn->nLoopEnd = psmp->nSustainEnd;
	pChn->dwFlags |= CHN_LOOP;
	if (pChn->dwFlags & CHN_PINGPONGSUSTAIN)
	    pChn->dwFlags |= CHN_PINGPONGLOOP;
    }
    if ((pChn->dwFlags & CHN_LOOP) && (pChn->nLoopEnd < pChn->nLength))
	pChn->nLength = pChn->nLoopEnd;
}

void CSoundFile_NoteChange(CSoundFile * that, unsigned nChn, int note,
    int bPorta, int bResetEnv)
//---------------------------------------------------------------------------
{
    MODCHANNEL *pChn;
    MODINSTRUMENT *pins;
    INSTRUMENTHEADER *penv;
    unsigned period;

    if (note < 1) {
	return;
    }
    pChn = &that->Chn[nChn];
    pins = pChn->pInstrument;
    penv = pChn->pHeader;

    if (penv && note <= 0x80) {
	unsigned n = penv->Keyboard[note - 1];

	if (n && n < MAX_SAMPLES) {
	    pins = &that->Ins[n];
	}
	note = penv->NoteMap[note - 1];
    }
    // Key Off
    if (note >= 0x80) {			// 0xFE or invalid note => key off
	// Key Off
	CSoundFile_KeyOff(that, nChn);
	// Note Cut
	if (note == 0xFE) {
	    pChn->dwFlags |= (CHN_NOTEFADE | CHN_FASTVOLRAMP);
	    if ((!(that->m_nType & MOD_TYPE_IT)) || (that->m_nInstruments))
		pChn->nVolume = 0;
	    pChn->nFadeOutVol = 0;
	}
	return;
    }
    if (!pins) {
	return;
    }
    if ((!bPorta)
	&& (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MED | MOD_TYPE_MT2))) {
	pChn->nTranspose = pins->RelativeTone;
	pChn->nFineTune = pins->nFineTune;
    }
    if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2 | MOD_TYPE_MED))
	note += pChn->nTranspose;
    if (note < 1)
	note = 1;
    if (note > 132)
	note = 132;
    pChn->nNote = note;
    if ((!bPorta) || (that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT)))
	pChn->nNewIns = 0;
    period =
	CSoundFile_GetPeriodFromNote(that, note, pChn->nFineTune, pChn->nC4Speed);
    if (period) {
	if ((!bPorta) || (!pChn->nPeriod))
	    pChn->nPeriod = period;
	pChn->nPortamentoDest = period;
	if ((!bPorta) || ((!pChn->nLength)
		&& (!(that->m_nType & MOD_TYPE_S3M)))) {
	    pChn->pInstrument = pins;
	    pChn->pSample = pins->pSample;
	    pChn->nLength = pins->nLength;
	    pChn->nLoopEnd = pins->nLength;
	    pChn->nLoopStart = 0;
	    pChn->dwFlags = (pChn->dwFlags & 0xFFFFFF00) | (pins->uFlags);
	    if (pChn->dwFlags & CHN_SUSTAINLOOP) {
		pChn->nLoopStart = pins->nSustainStart;
		pChn->nLoopEnd = pins->nSustainEnd;
		pChn->dwFlags &= ~CHN_PINGPONGLOOP;
		pChn->dwFlags |= CHN_LOOP;
		if (pChn->dwFlags & CHN_PINGPONGSUSTAIN)
		    pChn->dwFlags |= CHN_PINGPONGLOOP;
		if (pChn->nLength > pChn->nLoopEnd)
		    pChn->nLength = pChn->nLoopEnd;
	    } else if (pChn->dwFlags & CHN_LOOP) {
		pChn->nLoopStart = pins->nLoopStart;
		pChn->nLoopEnd = pins->nLoopEnd;
		if (pChn->nLength > pChn->nLoopEnd)
		    pChn->nLength = pChn->nLoopEnd;
	    }
	    pChn->nPos = 0;
	    pChn->nPosLo = 0;
	    if (pChn->nVibratoType < 4)
		pChn->nVibratoPos = ((that->m_nType & MOD_TYPE_IT)
		    && (!(that->
			    m_dwSongFlags & SONG_ITOLDEFFECTS))) ? 0x10 : 0;
	    if (pChn->nTremoloType < 4)
		pChn->nTremoloPos = 0;
	}
	if (pChn->nPos >= pChn->nLength)
	    pChn->nPos = pChn->nLoopStart;
    } else
	bPorta = 0;
    if ((!bPorta) || (!(that->m_nType & MOD_TYPE_IT))
	|| ((pChn->dwFlags & CHN_NOTEFADE) && (!pChn->nFadeOutVol))
	|| ((that->m_dwSongFlags & SONG_ITCOMPATMODE) && (pChn->nRowInstr))) {
	if ((that->m_nType & MOD_TYPE_IT) && (pChn->dwFlags & CHN_NOTEFADE)
	    && (!pChn->nFadeOutVol)) {
	    pChn->nVolEnvPosition = 0;
	    pChn->nPanEnvPosition = 0;
	    pChn->nPitchEnvPosition = 0;
	    pChn->nAutoVibDepth = 0;
	    pChn->nAutoVibPos = 0;
	    pChn->dwFlags &= ~CHN_NOTEFADE;
	    pChn->nFadeOutVol = 65536;
	}
	if ((!bPorta) || (!(that->m_dwSongFlags & SONG_ITCOMPATMODE))
	    || (pChn->nRowInstr)) {
	    if ((!(that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))
		|| (pChn->nRowInstr)) {
		pChn->dwFlags &= ~CHN_NOTEFADE;
		pChn->nFadeOutVol = 65536;
	    }
	}
    }
    pChn->dwFlags &= ~(CHN_EXTRALOUD | CHN_KEYOFF);
    // Enable Ramping
    if (!bPorta) {
	int bFlt;

	pChn->nVUMeter = 0x100;
	pChn->nLeftVU = pChn->nRightVU = 0xFF;
	pChn->dwFlags &= ~CHN_FILTER;
	pChn->dwFlags |= CHN_FASTVOLRAMP;
	pChn->nRetrigCount = 0;
	pChn->nTremorCount = 0;
	if (bResetEnv) {
	    pChn->nVolSwing = pChn->nPanSwing = 0;
	    if (penv) {
		if (!(penv->dwFlags & ENV_VOLCARRY))
		    pChn->nVolEnvPosition = 0;
		if (!(penv->dwFlags & ENV_PANCARRY))
		    pChn->nPanEnvPosition = 0;
		if (!(penv->dwFlags & ENV_PITCHCARRY))
		    pChn->nPitchEnvPosition = 0;
		if (that->m_nType & MOD_TYPE_IT) {
		    // Volume Swing
		    if (penv->nVolSwing) {
			int d =
			    ((long)penv->nVolSwing * (long)((rand() & 0xFF) -
				0x7F)) / 128;
			pChn->nVolSwing =
			    (signed short)((d * pChn->nVolume + 1) / 128);
		    }
		    // Pan Swing
		    if (penv->nPanSwing) {
			int d =
			    ((long)penv->nPanSwing * (long)((rand() & 0xFF) -
				0x7F)) / 128;
			pChn->nPanSwing = (signed short)d;
		    }
		}
	    }
	    pChn->nAutoVibDepth = 0;
	    pChn->nAutoVibPos = 0;
	}
	pChn->nLeftVol = pChn->nRightVol = 0;
	bFlt = (that->m_dwSongFlags & SONG_MPTFILTERMODE) ? 0 : 1;

	// Setup Initial Filter for this note
	if (penv) {
	    if (penv->nIFR & 0x80) {
		pChn->nResonance = penv->nIFR & 0x7F;
		bFlt = 1;
	    }
	    if (penv->nIFC & 0x80) {
		pChn->nCutOff = penv->nIFC & 0x7F;
		bFlt = 1;
	    }
	} else {
	    pChn->nVolSwing = pChn->nPanSwing = 0;
	}
#ifndef MODPLUG_NO_FILTER
	if ((pChn->nCutOff < 0x7F) && (bFlt))
	    CSoundFile_SetupChannelFilter(that, pChn, 1, 255);
#endif // MODPLUG_NO_FILTER
    }
}

unsigned CSoundFile_GetNNAChannel(CSoundFile const *that, unsigned nChn)
//---------------------------------------------
{
    unsigned i;
    const MODCHANNEL *pi;
    unsigned result;
    uint32_t vol;
    uint32_t envpos;

    // Check for empty channel
    pi = &that->Chn[that->m_nChannels];
    // FIXME: johns: bad code i and pi can be combined.
    for (i = that->m_nChannels; i < MAX_CHANNELS; i++, pi++) {
	if (!pi->nLength) {
	    return i;
	}
    }
    if (!that->Chn[nChn].nFadeOutVol) {
	return 0;
    }

    // All channels are used: check for lowest volume
    result = 0;
    vol = 64 * 65536;			// 25%
    envpos = 0xFFFFFF;

    pi = &that->Chn[that->m_nChannels];
    for (i = that->m_nChannels; i < MAX_CHANNELS; i++, pi++) {
	uint32_t v;

	if (!pi->nFadeOutVol) {
	    return i;
	}
	v = pi->nVolume;

	if (pi->dwFlags & CHN_NOTEFADE)
	    v = v * pi->nFadeOutVol;
	else
	    v <<= 16;
	if (pi->dwFlags & CHN_LOOP) {
	    v >>= 1;
	}
	if ((v < vol) || ((v == vol) && (pi->nVolEnvPosition > envpos))) {
	    envpos = pi->nVolEnvPosition;
	    vol = v;
	    result = i;
	}
    }
    return result;
}

void CSoundFile_CheckNNA(CSoundFile * that, unsigned nChn, unsigned instr,
    int note, int bForceCut)
//------------------------------------------------------------------------
{
    INSTRUMENTHEADER *penv;
    INSTRUMENTHEADER *pHeader;
    MODCHANNEL *pChn;
    MODCHANNEL *p;
    int8_t *pSample;
    unsigned i;

    if (note > 0x80) {
	note = 0;
    }
    if (note < 1) {
	return;
    }

    // Always NNA cut - using
    pChn = &that->Chn[nChn];
    if ((!(that->m_nType & (MOD_TYPE_IT | MOD_TYPE_MT2)))
	|| (!that->m_nInstruments) || (bForceCut)) {
	unsigned n;

	if ((that->m_dwSongFlags & SONG_CPUVERYHIGH)
	    || (!pChn->nLength) || (pChn->dwFlags & CHN_MUTE)
	    || ((!pChn->nLeftVol) && (!pChn->nRightVol))) {
	    return;
	}
	n = CSoundFile_GetNNAChannel(that, nChn);

	if (!n) {
	    return;
	}
	p = &that->Chn[n];

	// Copy Channel
	*p = *pChn;
	p->dwFlags &=
	    ~(CHN_VIBRATO | CHN_TREMOLO | CHN_PANBRELLO | CHN_MUTE |
	    CHN_PORTAMENTO);
	p->nMasterChn = nChn + 1;
	p->nCommand = 0;
	// Cut the note
	p->nFadeOutVol = 0;
	p->dwFlags |= (CHN_NOTEFADE | CHN_FASTVOLRAMP);
	// Stop this channel
	pChn->nLength = pChn->nPos = pChn->nPosLo = 0;
	pChn->nROfs = pChn->nLOfs = 0;
	pChn->nLeftVol = pChn->nRightVol = 0;
	return;
    }

    if (instr >= MAX_INSTRUMENTS) {
	instr = 0;
    }
    pSample = pChn->pSample;
    pHeader = pChn->pHeader;
    if ((instr) && (note)) {
	pHeader = that->Headers[instr];
	if (pHeader) {
	    unsigned n = 0;

	    if (note <= 0x80) {
		n = pHeader->Keyboard[note - 1];
		note = pHeader->NoteMap[note - 1];
		if ((n) && (n < MAX_SAMPLES))
		    pSample = that->Ins[n].pSample;
	    }
	} else
	    pSample = NULL;
    }

    penv = pChn->pHeader;
    if (!penv) {
	return;
    }
    p = pChn;

    for (i = nChn; i < MAX_CHANNELS; p++, i++)
	if ((i >= that->m_nChannels) || (p == pChn)) {
	    if (((p->nMasterChn == nChn + 1) || (p == pChn)) && (p->pHeader)) {
		int bOk = 0;

		// Duplicate Check Type
		switch (p->pHeader->nDCT) {
			// Note
		    case DCT_NOTE:
			if ((note) && (p->nNote == note)
			    && (pHeader == p->pHeader))
			    bOk = 1;
			break;
			// Sample
		    case DCT_SAMPLE:
			if ((pSample) && (pSample == p->pSample))
			    bOk = 1;
			break;
			// Instrument
		    case DCT_INSTRUMENT:
			if (pHeader == p->pHeader)
			    bOk = 1;
			break;
		}
		// Duplicate Note Action
		if (bOk) {
		    switch (p->pHeader->nDNA) {
			    // Cut
			case DNA_NOTECUT:
			    CSoundFile_KeyOff(that, i);
			    p->nVolume = 0;
			    break;
			    // Note Off
			case DNA_NOTEOFF:
			    CSoundFile_KeyOff(that, i);
			    break;
			    // Note Fade
			case DNA_NOTEFADE:
			    p->dwFlags |= CHN_NOTEFADE;
			    break;
		    }
		    if (!p->nVolume) {
			p->nFadeOutVol = 0;
			p->dwFlags |= (CHN_NOTEFADE | CHN_FASTVOLRAMP);
		    }
		}
	    }
	}
    if (pChn->dwFlags & CHN_MUTE)
	return;
    // New Note Action
    if ((pChn->nVolume) && (pChn->nLength)) {
	unsigned n = CSoundFile_GetNNAChannel(that, nChn);

	if (n) {
	    p = &that->Chn[n];

	    // Copy Channel
	    *p = *pChn;
	    p->dwFlags &=
		~(CHN_VIBRATO | CHN_TREMOLO | CHN_PANBRELLO | CHN_MUTE |
		CHN_PORTAMENTO);
	    p->nMasterChn = nChn + 1;
	    p->nCommand = 0;
	    // Key Off the note
	    switch (pChn->nNNA) {
		case NNA_NOTEOFF:
		    CSoundFile_KeyOff(that, n);
		    break;
		case NNA_NOTECUT:
		    p->nFadeOutVol = 0;
		case NNA_NOTEFADE:
		    p->dwFlags |= CHN_NOTEFADE;
		    break;
	    }
	    if (!p->nVolume) {
		p->nFadeOutVol = 0;
		p->dwFlags |= (CHN_NOTEFADE | CHN_FASTVOLRAMP);
	    }
	    // Stop this channel
	    pChn->nLength = pChn->nPos = pChn->nPosLo = 0;
	    pChn->nROfs = pChn->nLOfs = 0;
	}
    }
}

int CSoundFile_ProcessEffects(CSoundFile * that)
//-------------------------------
{
    int nBreakRow = -1;
    int nPosJump = -1;
    int nPatLoopRow = -1;
    MODCHANNEL *pChn = that->Chn;
    unsigned nChn;

    for (nChn = 0; nChn < that->m_nChannels; nChn++, pChn++) {
	unsigned instr = pChn->nRowInstr;
	unsigned volcmd = pChn->nRowVolCmd;
	unsigned vol = pChn->nRowVolume;
	unsigned cmd = pChn->nRowCommand;
	unsigned param = pChn->nRowParam;
	int bPorta = ((cmd != CMD_TONEPORTAMENTO) && (cmd != CMD_TONEPORTAVOL)
	    && (volcmd != VOLCMD_TONEPORTAMENTO)) ? 0 : 1;
	unsigned nStartTick = 0;

	pChn->dwFlags &= ~CHN_FASTVOLRAMP;
	// Process special effects (note delay, pattern delay, pattern loop)
	if ((cmd == CMD_MODCMDEX) || (cmd == CMD_S3MCMDEX)) {
	    if ((!param) && (that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT)))
		param = pChn->nOldCmdEx;
	    else
		pChn->nOldCmdEx = param;
	    // Note Delay ?
	    if ((param & 0xF0) == 0xD0) {
		nStartTick = param & 0x0F;
	    } else if (!that->m_nTickCount) {
		// Pattern Loop ?
		if ((((param & 0xF0) == 0x60) && (cmd == CMD_MODCMDEX))
		    || (((param & 0xF0) == 0xB0) && (cmd == CMD_S3MCMDEX))) {
		    int nloop = CSoundFile_PatternLoop(that, pChn, param &0x0F);

		    if (nloop >= 0)
			nPatLoopRow = nloop;
		} else
		    // Pattern Delay
		if ((param & 0xF0) == 0xE0) {
		    that->m_nPatternDelay = param & 0x0F;
		}
	    }
	}
	// Handles note/instrument/volume changes
	if (that->m_nTickCount == nStartTick)	// can be delayed by a note delay effect
	{
	    unsigned note = pChn->nRowNote;

	    if (instr)
		pChn->nNewIns = instr;
	    // XM: Key-Off + Sample == Note Cut
	    if (that->m_nType & (MOD_TYPE_MOD | MOD_TYPE_XM | MOD_TYPE_MT2)) {
		if ((note == 0xFF) && ((!pChn->pHeader)
			|| (!(pChn->pHeader->dwFlags & ENV_VOLUME)))) {
		    pChn->dwFlags |= CHN_FASTVOLRAMP;
		    pChn->nVolume = 0;
		    note = instr = 0;
		}
	    }
	    if ((!note) && (instr)) {
		if (that->m_nInstruments) {
		    if (pChn->pInstrument)
			pChn->nVolume = pChn->pInstrument->nVolume;
		    if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) {
			pChn->dwFlags |= CHN_FASTVOLRAMP;
			pChn->nVolEnvPosition = 0;
			pChn->nPanEnvPosition = 0;
			pChn->nPitchEnvPosition = 0;
			pChn->nAutoVibDepth = 0;
			pChn->nAutoVibPos = 0;
			pChn->dwFlags &= ~CHN_NOTEFADE;
			pChn->nFadeOutVol = 65536;
		    }
		} else {
		    if (instr < MAX_SAMPLES)
			pChn->nVolume = that->Ins[instr].nVolume;
		}
		if (!(that->m_nType & MOD_TYPE_IT))
		    instr = 0;
	    }
	    // Invalid Instrument ?
	    if (instr >= MAX_INSTRUMENTS)
		instr = 0;
	    // Note Cut/Off => ignore instrument
	    if (note >= 0xFE)
		instr = 0;
	    if ((note) && (note <= 128))
		pChn->nNewNote = note;
	    // New Note Action ?
	    if ((note) && (note <= 128) && (!bPorta)) {
		CSoundFile_CheckNNA(that, nChn, instr, note, 0);
	    }
	    // Instrument Change ?
	    if (instr) {
		MODINSTRUMENT *psmp = pChn->pInstrument;

		CSoundFile_InstrumentChange(that, pChn, instr, bPorta, 1, 1);
		pChn->nNewIns = 0;
		// Special IT case: portamento+note causes sample change -> ignore portamento
		if ((that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT))
		    && (psmp != pChn->pInstrument) && (note) && (note < 0x80)) {
		    bPorta = 0;
		}
	    }
	    // New Note ?
	    if (note) {
		if ((!instr) && (pChn->nNewIns) && (note < 0x80)) {
		    CSoundFile_InstrumentChange(that, pChn, pChn->nNewIns,
			bPorta, 0, (that->
			    m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) ? 0 : 1);
		    pChn->nNewIns = 0;
		}
		CSoundFile_NoteChange(that, nChn, note, bPorta,
		    (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) ? 0 : 1);
		if ((bPorta) && (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2))
		    && (instr)) {
		    pChn->dwFlags |= CHN_FASTVOLRAMP;
		    pChn->nVolEnvPosition = 0;
		    pChn->nPanEnvPosition = 0;
		    pChn->nPitchEnvPosition = 0;
		    pChn->nAutoVibDepth = 0;
		    pChn->nAutoVibPos = 0;
		}
	    }
	    // Tick-0 only volume commands
	    if (volcmd == VOLCMD_VOLUME) {
		if (vol > 64)
		    vol = 64;
		pChn->nVolume = vol << 2;
		pChn->dwFlags |= CHN_FASTVOLRAMP;
	    } else if (volcmd == VOLCMD_PANNING) {
		if (vol > 64)
		    vol = 64;
		pChn->nPan = vol << 2;
		pChn->dwFlags |= CHN_FASTVOLRAMP;
	    }
	}
	// Volume Column Effect (except volume & panning)
	if ((volcmd > VOLCMD_PANNING) && (that->m_nTickCount >= nStartTick)) {
	    if (volcmd == VOLCMD_TONEPORTAMENTO) {
		if (that->m_nType & MOD_TYPE_IT)
		    CSoundFile_TonePortamento(that, pChn,
			CSoundFile_ImpulseTrackerPortaVolCmd[vol & 0x0F]);
		else
		    CSoundFile_TonePortamento(that, pChn, vol * 16);
	    } else {
		if (vol)
		    pChn->nOldVolParam = vol;
		else
		    vol = pChn->nOldVolParam;
		switch (volcmd) {
		    case VOLCMD_VOLSLIDEUP:
			CSoundFile_VolumeSlide(that, pChn, vol << 4);
			break;

		    case VOLCMD_VOLSLIDEDOWN:
			CSoundFile_VolumeSlide(that, pChn, vol);
			break;

		    case VOLCMD_FINEVOLUP:
			if (that->m_nType & MOD_TYPE_IT) {
			    if (that->m_nTickCount == nStartTick)
				CSoundFile_VolumeSlide(that, pChn,
				    (vol << 4) | 0x0F);
			} else
			    CSoundFile_FineVolumeUp(that, pChn, vol);
			break;

		    case VOLCMD_FINEVOLDOWN:
			if (that->m_nType & MOD_TYPE_IT) {
			    if (that->m_nTickCount == nStartTick)
				CSoundFile_VolumeSlide(that, pChn, 0xF0 | vol);
			} else
			    CSoundFile_FineVolumeDown(that, pChn, vol);
			break;

		    case VOLCMD_VIBRATOSPEED:
			CSoundFile_Vibrato(pChn, vol << 4);
			break;

		    case VOLCMD_VIBRATO:
			CSoundFile_Vibrato(pChn, vol);
			break;

		    case VOLCMD_PANSLIDELEFT:
			CSoundFile_PanningSlide(that, pChn, vol);
			break;

		    case VOLCMD_PANSLIDERIGHT:
			CSoundFile_PanningSlide(that, pChn, vol << 4);
			break;

		    case VOLCMD_PORTAUP:
			CSoundFile_PortamentoUp(that, pChn, vol << 2);
			break;

		    case VOLCMD_PORTADOWN:
			CSoundFile_PortamentoDown(that, pChn, vol << 2);
			break;
		}
	    }
	}
	// Effects
	if (cmd)
	    switch (cmd) {
		    // Set Volume
		case CMD_VOLUME:
		    if (!that->m_nTickCount) {
			pChn->nVolume = (param < 64) ? param * 4 : 256;
			pChn->dwFlags |= CHN_FASTVOLRAMP;
		    }
		    break;

		    // Portamento Up
		case CMD_PORTAMENTOUP:
		    if ((!param) && (that->m_nType & MOD_TYPE_MOD))
			break;
		    CSoundFile_PortamentoUp(that, pChn, param);
		    break;

		    // Portamento Down
		case CMD_PORTAMENTODOWN:
		    if ((!param) && (that->m_nType & MOD_TYPE_MOD))
			break;
		    CSoundFile_PortamentoDown(that, pChn, param);
		    break;

		    // Volume Slide
		case CMD_VOLUMESLIDE:
		    if ((param) || (that->m_nType != MOD_TYPE_MOD))
			CSoundFile_VolumeSlide(that, pChn, param);
		    break;

		    // Tone-Portamento
		case CMD_TONEPORTAMENTO:
		    CSoundFile_TonePortamento(that, pChn, param);
		    break;

		    // Tone-Portamento + Volume Slide
		case CMD_TONEPORTAVOL:
		    if ((param) || (that->m_nType != MOD_TYPE_MOD))
			CSoundFile_VolumeSlide(that, pChn, param);
		    CSoundFile_TonePortamento(that, pChn, 0);
		    break;

		    // Vibrato
		case CMD_VIBRATO:
		    CSoundFile_Vibrato(pChn, param);
		    break;

		    // Vibrato + Volume Slide
		case CMD_VIBRATOVOL:
		    if ((param) || (that->m_nType != MOD_TYPE_MOD))
			CSoundFile_VolumeSlide(that, pChn, param);
		    CSoundFile_Vibrato(pChn, 0);
		    break;

		    // Set Speed
		case CMD_SPEED:
		    if (!that->m_nTickCount)
			CSoundFile_SetSpeed(that, param);
		    break;

		    // Set Tempo
		case CMD_TEMPO:
		    if (!that->m_nTickCount) {
			if (that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT)) {
			    if (param)
				pChn->nOldTempo = param;
			    else
				param = pChn->nOldTempo;
			}
			CSoundFile_SetTempo(that, param);
		    }
		    break;

		    // Set Offset
		case CMD_OFFSET:
		    if (that->m_nTickCount)
			break;
		    if (param)
			pChn->nOldOffset = param;
		    else
			param = pChn->nOldOffset;
		    param <<= 8;
		    param |= (unsigned)(pChn->nOldHiOffset) << 16;
		    if ((pChn->nRowNote) && (pChn->nRowNote < 0x80)) {
			if (bPorta)
			    pChn->nPos = param;
			else
			    pChn->nPos += param;
			if (pChn->nPos >= pChn->nLength) {
			    if (!(that->
				    m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2))) {
				pChn->nPos = pChn->nLoopStart;
				if ((that->m_dwSongFlags & SONG_ITOLDEFFECTS)
				    && (pChn->nLength > 4)) {
				    pChn->nPos = pChn->nLength - 2;
				}
			    }
			}
		    } else if ((param < pChn->nLength)
			&& (that->m_nType & (MOD_TYPE_MTM | MOD_TYPE_DMF))) {
			pChn->nPos = param;
		    }
		    break;

		    // Arpeggio
		case CMD_ARPEGGIO:
		    if ((that->m_nTickCount) || (!pChn->nPeriod)
			|| (!pChn->nNote))
			break;
		    if ((!param)
			&& (!(that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT))))
			break;
		    pChn->nCommand = CMD_ARPEGGIO;
		    if (param)
			pChn->nArpeggio = param;
		    break;

		    // Retrig
		case CMD_RETRIG:
		    if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) {
			if (!(param & 0xF0))
			    param |= pChn->nRetrigParam & 0xF0;
			if (!(param & 0x0F))
			    param |= pChn->nRetrigParam & 0x0F;
			param |= 0x100;	// increment retrig count on first row
		    }
		    if (param)
			pChn->nRetrigParam = (uint8_t) (param & 0xFF);
		    else
			param = pChn->nRetrigParam;
		    CSoundFile_RetrigNote(that, nChn, param);
		    break;

		    // Tremor
		case CMD_TREMOR:
		    if (that->m_nTickCount)
			break;
		    pChn->nCommand = CMD_TREMOR;
		    if (param)
			pChn->nTremorParam = param;
		    break;

		    // Set Global Volume
		case CMD_GLOBALVOLUME:
		    if (that->m_nTickCount)
			break;
		    if (that->m_nType != MOD_TYPE_IT)
			param <<= 1;
		    if (param > 128)
			param = 128;
		    that->m_nGlobalVolume = param << 1;
		    break;

		    // Global Volume Slide
		case CMD_GLOBALVOLSLIDE:
		    CSoundFile_GlobalVolSlide(that, param);
		    break;

		    // Set 8-bit Panning
		case CMD_PANNING8:
		    if (that->m_nTickCount)
			break;
		    if (!(that->m_dwSongFlags & SONG_SURROUNDPAN))
			pChn->dwFlags &= ~CHN_SURROUND;
		    if (that->
			m_nType & (MOD_TYPE_IT | MOD_TYPE_XM | MOD_TYPE_MT2)) {
			pChn->nPan = param;
		    } else if (param <= 0x80) {
			pChn->nPan = param << 1;
		    } else if (param == 0xA4) {
			pChn->dwFlags |= CHN_SURROUND;
			pChn->nPan = 0x80;
		    }
		    pChn->dwFlags |= CHN_FASTVOLRAMP;
		    break;

		    // Panning Slide
		case CMD_PANNINGSLIDE:
		    CSoundFile_PanningSlide(that, pChn, param);
		    break;

		    // Tremolo
		case CMD_TREMOLO:
		    CSoundFile_Tremolo(pChn, param);
		    break;

		    // Fine Vibrato
		case CMD_FINEVIBRATO:
		    CSoundFile_FineVibrato(pChn, param);
		    break;

		    // MOD/XM Exx Extended Commands
		case CMD_MODCMDEX:
		    CSoundFile_ExtendedMODCommands(that, nChn, param);
		    break;

		    // S3M/IT Sxx Extended Commands
		case CMD_S3MCMDEX:
		    CSoundFile_ExtendedS3MCommands(that, nChn, param);
		    break;

		    // Key Off
		case CMD_KEYOFF:
		    if (!that->m_nTickCount)
			CSoundFile_KeyOff(that, nChn);
		    break;

		    // Extra-fine porta up/down
		case CMD_XFINEPORTAUPDOWN:
		    switch (param & 0xF0) {
			case 0x10:
			    CSoundFile_ExtraFinePortamentoUp(that, pChn,
				param & 0x0F);
			    break;
			case 0x20:
			    CSoundFile_ExtraFinePortamentoDown(that, pChn,
				param & 0x0F);
			    break;
			    // Modplug XM Extensions
			case 0x50:
			case 0x60:
			case 0x70:
			case 0x90:
			case 0xA0:
			    CSoundFile_ExtendedS3MCommands(that, nChn, param);
			    break;
		    }
		    break;

		    // Set Channel Global Volume
		case CMD_CHANNELVOLUME:
		    if (that->m_nTickCount)
			break;
		    if (param <= 64) {
			pChn->nGlobalVol = param;
			pChn->dwFlags |= CHN_FASTVOLRAMP;
		    }
		    break;

		    // Channel volume slide
		case CMD_CHANNELVOLSLIDE:
		    CSoundFile_ChannelVolSlide(that, pChn, param);
		    break;

		    // Panbrello (IT)
		case CMD_PANBRELLO:
		    CSoundFile_Panbrello(pChn, param);
		    break;

		    // Set Envelope Position
		case CMD_SETENVPOSITION:
		    if (!that->m_nTickCount) {
			pChn->nVolEnvPosition = param;
			pChn->nPanEnvPosition = param;
			pChn->nPitchEnvPosition = param;
			if (pChn->pHeader) {
			    INSTRUMENTHEADER *penv = pChn->pHeader;

			    if ((pChn->dwFlags & CHN_PANENV) && (penv->nPanEnv)
				&& (param >
				    penv->PanPoints[penv->nPanEnv - 1])) {
				pChn->dwFlags &= ~CHN_PANENV;
			    }
			}
		    }
		    break;

		    // Position Jump
		case CMD_POSITIONJUMP:
		    nPosJump = param;
		    break;

		    // Pattern Break
		case CMD_PATTERNBREAK:
		    nBreakRow = param;
		    break;

		    // Midi Controller
		case CMD_MIDI:
		    if (that->m_nTickCount)
			break;
		    if (param < 0x80) {
			CSoundFile_ProcessMidiMacro(that, nChn,
			    &that->m_MidiCfg.szMidiSFXExt[pChn->
				nActiveMacro << 5], param);
		    } else {
			CSoundFile_ProcessMidiMacro(that, nChn,
			    &that->m_MidiCfg.szMidiZXXExt[(param & 0x7F) << 5],
			    0);
		    }
		    break;
	    }
    }

    // Navigation Effects
    if (!that->m_nTickCount) {
	// Pattern Loop
	if (nPatLoopRow >= 0) {
	    that->m_nNextPattern = that->m_nCurrentPattern;
	    that->m_nNextRow = nPatLoopRow;
	    if (that->m_nPatternDelay)
		that->m_nNextRow++;
	} else
	    // Pattern Break / Position Jump only if no loop running
	if ((nBreakRow >= 0) || (nPosJump >= 0)) {
	    int bNoLoop = 0;

	    if (nPosJump < 0)
		nPosJump = that->m_nCurrentPattern + 1;
	    if (nBreakRow < 0)
		nBreakRow = 0;
	    // Modplug Tracker & ModPlugin allow backward jumps
#ifndef MODPLUG_FASTSOUNDLIB
	    if ((nPosJump < (int)that->m_nCurrentPattern)
		|| ((nPosJump == (int)that->m_nCurrentPattern)
		    && (nBreakRow <= (int)that->m_nRow))) {
		if (!CSoundFile_IsValidBackwardJump(that,
			that->m_nCurrentPattern, that->m_nRow, nPosJump,
			nBreakRow)) {
		    if (that->m_nRepeatCount) {
			if (that->m_nRepeatCount > 0)
			    that->m_nRepeatCount--;
		    } else {
#ifdef MODPLUG_TRACKER
			if (CSoundFile_gdwSoundSetup & SNDMIX_NOBACKWARDJUMPS)
#endif
			    // Backward jump disabled
			    bNoLoop = 1;
			//reset repeat count incase there are multiple loops.
			//(i.e. Unreal tracks)
			that->m_nRepeatCount = that->m_nInitialRepeatCount;
		    }
		}
	    }
#endif // MODPLUG_FASTSOUNDLIB
	    if (((!bNoLoop) && (nPosJump < MAX_ORDERS))
		&& ((nPosJump != (int)that->m_nCurrentPattern)
		    || (nBreakRow != (int)that->m_nRow))) {
		if (nPosJump != (int)that->m_nCurrentPattern) {
		    unsigned i;

		    for (i = 0; i < that->m_nChannels; i++)
			that->Chn[i].nPatternLoopCount = 0;
		}
		that->m_nNextPattern = nPosJump;
		that->m_nNextRow = (unsigned)nBreakRow;
	    }
	}
    }
    return 1;
}

////////////////////////////////////////////////////////////
// Channels effects

void CSoundFile_PortamentoUp(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//---------------------------------------------------------
{
    if (param)
	pChn->nOldPortaUpDown = param;
    else
	param = pChn->nOldPortaUpDown;
    if ((that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT | MOD_TYPE_STM))
	&& ((param & 0xF0) >= 0xE0)) {
	if (param & 0x0F) {
	    if ((param & 0xF0) == 0xF0) {
		CSoundFile_FinePortamentoUp(that, pChn, param & 0x0F);
	    } else if ((param & 0xF0) == 0xE0) {
		CSoundFile_ExtraFinePortamentoUp(that, pChn, param & 0x0F);
	    }
	}
	return;
    }
    // Regular Slide
    if (!(that->m_dwSongFlags & SONG_FIRSTTICK)) {
	CSoundFile_DoFreqSlide(that, pChn, -(int)(param * 4));
    }
}

void CSoundFile_PortamentoDown(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//-----------------------------------------------------------
{
    if (param)
	pChn->nOldPortaUpDown = param;
    else
	param = pChn->nOldPortaUpDown;
    if ((that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT | MOD_TYPE_STM))
	&& ((param & 0xF0) >= 0xE0)) {
	if (param & 0x0F) {
	    if ((param & 0xF0) == 0xF0) {
		CSoundFile_FinePortamentoDown(that, pChn, param & 0x0F);
	    } else if ((param & 0xF0) == 0xE0) {
		CSoundFile_ExtraFinePortamentoDown(that, pChn, param & 0x0F);
	    }
	}
	return;
    }
    if (!(that->m_dwSongFlags & SONG_FIRSTTICK))
	CSoundFile_DoFreqSlide(that, pChn, (int)(param << 2));
}

void CSoundFile_FinePortamentoUp(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//-------------------------------------------------------------
{
    if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) {
	if (param)
	    pChn->nOldFinePortaUpDown = param;
	else
	    param = pChn->nOldFinePortaUpDown;
    }
    if (that->m_dwSongFlags & SONG_FIRSTTICK) {
	if ((pChn->nPeriod) && (param)) {
	    if ((that->m_dwSongFlags & SONG_LINEARSLIDES)
		&& (!(that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))) {
		pChn->nPeriod =
		    _muldivr(pChn->nPeriod,
		    CSoundFile_LinearSlideDownTable[param & 0x0F], 65536);
	    } else {
		pChn->nPeriod -= (int)(param * 4);
	    }
	    if (pChn->nPeriod < 1)
		pChn->nPeriod = 1;
	}
    }
}

void CSoundFile_FinePortamentoDown(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//---------------------------------------------------------------
{
    if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) {
	if (param)
	    pChn->nOldFinePortaUpDown = param;
	else
	    param = pChn->nOldFinePortaUpDown;
    }
    if (that->m_dwSongFlags & SONG_FIRSTTICK) {
	if ((pChn->nPeriod) && (param)) {
	    if ((that->m_dwSongFlags & SONG_LINEARSLIDES)
		&& (!(that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))) {
		pChn->nPeriod =
		    _muldivr(pChn->nPeriod,
		    CSoundFile_LinearSlideUpTable[param & 0x0F], 65536);
	    } else {
		pChn->nPeriod += (int)(param * 4);
	    }
	    if (pChn->nPeriod > 0xFFFF)
		pChn->nPeriod = 0xFFFF;
	}
    }
}

void CSoundFile_ExtraFinePortamentoUp(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//------------------------------------------------------------------
{
    if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) {
	if (param)
	    pChn->nOldFinePortaUpDown = param;
	else
	    param = pChn->nOldFinePortaUpDown;
    }
    if (that->m_dwSongFlags & SONG_FIRSTTICK) {
	if ((pChn->nPeriod) && (param)) {
	    if ((that->m_dwSongFlags & SONG_LINEARSLIDES)
		&& (!(that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))) {
		pChn->nPeriod =
		    _muldivr(pChn->nPeriod,
		    CSoundFile_FineLinearSlideDownTable[param & 0x0F], 65536);
	    } else {
		pChn->nPeriod -= (int)(param);
	    }
	    if (pChn->nPeriod < 1)
		pChn->nPeriod = 1;
	}
    }
}

void CSoundFile_ExtraFinePortamentoDown(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//--------------------------------------------------------------------
{
    if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) {
	if (param)
	    pChn->nOldFinePortaUpDown = param;
	else
	    param = pChn->nOldFinePortaUpDown;
    }
    if (that->m_dwSongFlags & SONG_FIRSTTICK) {
	if ((pChn->nPeriod) && (param)) {
	    if ((that->m_dwSongFlags & SONG_LINEARSLIDES)
		&& (!(that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))) {
		pChn->nPeriod =
		    _muldivr(pChn->nPeriod,
		    CSoundFile_FineLinearSlideUpTable[param & 0x0F], 65536);
	    } else {
		pChn->nPeriod += (int)(param);
	    }
	    if (pChn->nPeriod > 0xFFFF)
		pChn->nPeriod = 0xFFFF;
	}
    }
}

// Portamento Slide
void CSoundFile_TonePortamento(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//-----------------------------------------------------------
{
    if (param)
	pChn->nPortamentoSlide = param * 4;
    pChn->dwFlags |= CHN_PORTAMENTO;
    if ((pChn->nPeriod) && (pChn->nPortamentoDest)
	&& (!(that->m_dwSongFlags & SONG_FIRSTTICK))) {
	if (pChn->nPeriod < pChn->nPortamentoDest) {
	    long delta = (int)pChn->nPortamentoSlide;

	    if ((that->m_dwSongFlags & SONG_LINEARSLIDES)
		&& (!(that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))) {
		unsigned n = pChn->nPortamentoSlide >> 2;

		if (n > 255)
		    n = 255;
		delta =
		    _muldivr(pChn->nPeriod, CSoundFile_LinearSlideUpTable[n],
		    65536) - pChn->nPeriod;
		if (delta < 1)
		    delta = 1;
	    }
	    pChn->nPeriod += delta;
	    if (pChn->nPeriod > pChn->nPortamentoDest)
		pChn->nPeriod = pChn->nPortamentoDest;
	} else if (pChn->nPeriod > pChn->nPortamentoDest) {
	    long delta = -(int)pChn->nPortamentoSlide;

	    if ((that->m_dwSongFlags & SONG_LINEARSLIDES)
		&& (!(that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))) {
		unsigned n = pChn->nPortamentoSlide >> 2;

		if (n > 255)
		    n = 255;
		delta =
		    _muldivr(pChn->nPeriod, CSoundFile_LinearSlideDownTable[n],
		    65536) - pChn->nPeriod;
		if (delta > -1)
		    delta = -1;
	    }
	    pChn->nPeriod += delta;
	    if (pChn->nPeriod < pChn->nPortamentoDest)
		pChn->nPeriod = pChn->nPortamentoDest;
	}
    }
}

void CSoundFile_Vibrato(MODCHANNEL * p, unsigned param)
//-------------------------------------------------
{
    if (param & 0x0F)
	p->nVibratoDepth = (param & 0x0F) * 4;
    if (param & 0xF0)
	p->nVibratoSpeed = (param >> 4) & 0x0F;
    p->dwFlags |= CHN_VIBRATO;
}

void CSoundFile_FineVibrato(MODCHANNEL * p, unsigned param)
//-----------------------------------------------------
{
    if (param & 0x0F)
	p->nVibratoDepth = param & 0x0F;
    if (param & 0xF0)
	p->nVibratoSpeed = (param >> 4) & 0x0F;
    p->dwFlags |= CHN_VIBRATO;
}

void CSoundFile_Panbrello(MODCHANNEL * p, unsigned param)
//---------------------------------------------------
{
    if (param & 0x0F)
	p->nPanbrelloDepth = param & 0x0F;
    if (param & 0xF0)
	p->nPanbrelloSpeed = (param >> 4) & 0x0F;
    p->dwFlags |= CHN_PANBRELLO;
}

void CSoundFile_VolumeSlide(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//--------------------------------------------------------
{
    long newvolume;

    if (param)
	pChn->nOldVolumeSlide = param;
    else
	param = pChn->nOldVolumeSlide;
    newvolume = pChn->nVolume;

    if (that->
	m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT | MOD_TYPE_STM | MOD_TYPE_AMF)) {
	if ((param & 0x0F) == 0x0F) {
	    if (param & 0xF0) {
		CSoundFile_FineVolumeUp(that, pChn, (param >> 4));
		return;
	    } else {
		if ((that->m_dwSongFlags & SONG_FIRSTTICK)
		    && (!(that->m_dwSongFlags & SONG_FASTVOLSLIDES))) {
		    newvolume -= 0x0F * 4;
		}
	    }
	} else if ((param & 0xF0) == 0xF0) {
	    if (param & 0x0F) {
		CSoundFile_FineVolumeDown(that, pChn, (param & 0x0F));
		return;
	    } else {
		if ((that->m_dwSongFlags & SONG_FIRSTTICK)
		    && (!(that->m_dwSongFlags & SONG_FASTVOLSLIDES))) {
		    newvolume += 0x0F * 4;
		}
	    }
	}
    }
    if ((!(that->m_dwSongFlags & SONG_FIRSTTICK))
	|| (that->m_dwSongFlags & SONG_FASTVOLSLIDES)) {
	if (param & 0x0F)
	    newvolume -= (int)((param & 0x0F) * 4);
	else
	    newvolume += (int)((param & 0xF0) >> 2);
	if (that->m_nType & MOD_TYPE_MOD)
	    pChn->dwFlags |= CHN_FASTVOLRAMP;
    }
    if (newvolume < 0)
	newvolume = 0;
    if (newvolume > 256)
	newvolume = 256;
    pChn->nVolume = newvolume;
}

void CSoundFile_PanningSlide(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//---------------------------------------------------------
{
    long nPanSlide = 0;

    if (param)
	pChn->nOldPanSlide = param;
    else
	param = pChn->nOldPanSlide;
    if (that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT | MOD_TYPE_STM)) {
	if (((param & 0x0F) == 0x0F) && (param & 0xF0)) {
	    if (that->m_dwSongFlags & SONG_FIRSTTICK) {
		param = (param & 0xF0) >> 2;
		nPanSlide = -(int)param;
	    }
	} else if (((param & 0xF0) == 0xF0) && (param & 0x0F)) {
	    if (that->m_dwSongFlags & SONG_FIRSTTICK) {
		nPanSlide = (param & 0x0F) << 2;
	    }
	} else {
	    if (!(that->m_dwSongFlags & SONG_FIRSTTICK)) {
		if (param & 0x0F)
		    nPanSlide = (int)((param & 0x0F) << 2);
		else
		    nPanSlide = -(int)((param & 0xF0) >> 2);
	    }
	}
    } else {
	if (!(that->m_dwSongFlags & SONG_FIRSTTICK)) {
	    if (param & 0x0F)
		nPanSlide = -(int)((param & 0x0F) << 2);
	    else
		nPanSlide = (int)((param & 0xF0) >> 2);
	}
    }
    if (nPanSlide) {
	nPanSlide += pChn->nPan;
	if (nPanSlide < 0)
	    nPanSlide = 0;
	if (nPanSlide > 256)
	    nPanSlide = 256;
	pChn->nPan = nPanSlide;
    }
}

void CSoundFile_FineVolumeUp(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//---------------------------------------------------------
{
    if (param)
	pChn->nOldFineVolUpDown = param;
    else
	param = pChn->nOldFineVolUpDown;
    if (that->m_dwSongFlags & SONG_FIRSTTICK) {
	pChn->nVolume += param * 4;
	if (pChn->nVolume > 256)
	    pChn->nVolume = 256;
	if (that->m_nType & MOD_TYPE_MOD)
	    pChn->dwFlags |= CHN_FASTVOLRAMP;
    }
}

void CSoundFile_FineVolumeDown(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//-----------------------------------------------------------
{
    if (param)
	pChn->nOldFineVolUpDown = param;
    else
	param = pChn->nOldFineVolUpDown;
    if (that->m_dwSongFlags & SONG_FIRSTTICK) {
	pChn->nVolume -= param * 4;
	if (pChn->nVolume < 0)
	    pChn->nVolume = 0;
	if (that->m_nType & MOD_TYPE_MOD)
	    pChn->dwFlags |= CHN_FASTVOLRAMP;
    }
}

void CSoundFile_Tremolo(MODCHANNEL * p, unsigned param)
//-------------------------------------------------
{
    if (param & 0x0F) {
	p->nTremoloDepth = (param & 0x0F) << 2;
    }
    if (param & 0xF0) {
	p->nTremoloSpeed = (param >> 4) & 0x0F;
    }
    p->dwFlags |= CHN_TREMOLO;
}

void CSoundFile_ChannelVolSlide(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//------------------------------------------------------------
{
    long nChnSlide = 0;

    if (param)
	pChn->nOldChnVolSlide = param;
    else
	param = pChn->nOldChnVolSlide;
    if (((param & 0x0F) == 0x0F) && (param & 0xF0)) {
	if (that->m_dwSongFlags & SONG_FIRSTTICK)
	    nChnSlide = param >> 4;
    } else if (((param & 0xF0) == 0xF0) && (param & 0x0F)) {
	if (that->m_dwSongFlags & SONG_FIRSTTICK)
	    nChnSlide = -(int)(param & 0x0F);
    } else {
	if (!(that->m_dwSongFlags & SONG_FIRSTTICK)) {
	    if (param & 0x0F)
		nChnSlide = -(int)(param & 0x0F);
	    else
		nChnSlide = (int)((param & 0xF0) >> 4);
	}
    }
    if (nChnSlide) {
	nChnSlide += pChn->nGlobalVol;
	if (nChnSlide < 0)
	    nChnSlide = 0;
	if (nChnSlide > 64)
	    nChnSlide = 64;
	pChn->nGlobalVol = nChnSlide;
    }
}

void CSoundFile_ExtendedMODCommands(CSoundFile * that, unsigned nChn,
    unsigned param)
//---------------------------------------------------------
{
    MODCHANNEL *pChn = &that->Chn[nChn];
    unsigned command = param & 0xF0;

    param &= 0x0F;
    switch (command) {
	    // E0x: Set Filter
	    // E1x: Fine Portamento Up
	case 0x10:
	    if ((param) || (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))
		CSoundFile_FinePortamentoUp(that, pChn, param);
	    break;
	    // E2x: Fine Portamento Down
	case 0x20:
	    if ((param) || (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))
		CSoundFile_FinePortamentoDown(that, pChn, param);
	    break;
	    // E3x: Set Glissando Control
	case 0x30:
	    pChn->dwFlags &= ~CHN_GLISSANDO;
	    if (param)
		pChn->dwFlags |= CHN_GLISSANDO;
	    break;
	    // E4x: Set Vibrato WaveForm
	case 0x40:
	    pChn->nVibratoType = param & 0x07;
	    break;
	    // E5x: Set FineTune
	case 0x50:
	    if (that->m_nTickCount)
		break;
	    pChn->nC4Speed = CSoundFile_S3MFineTuneTable[param];
	    if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2))
		pChn->nFineTune = param * 2;
	    else
		pChn->nFineTune = MOD2XMFineTune(param);
	    if (pChn->nPeriod)
		pChn->nPeriod =
		    CSoundFile_GetPeriodFromNote(that, pChn->nNote, pChn->nFineTune,
		    pChn->nC4Speed);
	    break;
	    // E6x: Pattern Loop
	    // E7x: Set Tremolo WaveForm
	case 0x70:
	    pChn->nTremoloType = param & 0x07;
	    break;
	    // E8x: Set 4-bit Panning
	case 0x80:
	    if (!that->m_nTickCount) {
		pChn->nPan = (param << 4) + 8;
		pChn->dwFlags |= CHN_FASTVOLRAMP;
	    }
	    break;
	    // E9x: Retrig
	case 0x90:
	    CSoundFile_RetrigNote(that, nChn, param);
	    break;
	    // EAx: Fine Volume Up
	case 0xA0:
	    if ((param) || (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))
		CSoundFile_FineVolumeUp(that, pChn, param);
	    break;
	    // EBx: Fine Volume Down
	case 0xB0:
	    if ((param) || (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))
		CSoundFile_FineVolumeDown(that, pChn, param);
	    break;
	    // ECx: Note Cut
	case 0xC0:
	    CSoundFile_NoteCut(that, nChn, param);
	    break;
	    // EDx: Note Delay
	    // EEx: Pattern Delay
	    // EFx: MOD: Invert Loop, XM: Set Active Midi Macro
	case 0xF0:
	    pChn->nActiveMacro = param;
	    break;
    }
}

void CSoundFile_ExtendedS3MCommands(CSoundFile * that, unsigned nChn,
    unsigned param)
//---------------------------------------------------------
{
    MODCHANNEL *pChn = &that->Chn[nChn];
    unsigned command = param & 0xF0;

    param &= 0x0F;
    switch (command) {
	    // S0x: Set Filter
	    // S1x: Set Glissando Control
	case 0x10:
	    pChn->dwFlags &= ~CHN_GLISSANDO;
	    if (param)
		pChn->dwFlags |= CHN_GLISSANDO;
	    break;
	    // S2x: Set FineTune
	case 0x20:
	    if (that->m_nTickCount)
		break;
	    pChn->nC4Speed = CSoundFile_S3MFineTuneTable[param & 0x0F];
	    pChn->nFineTune = MOD2XMFineTune(param);
	    if (pChn->nPeriod)
		pChn->nPeriod =
		    CSoundFile_GetPeriodFromNote(that, pChn->nNote, pChn->nFineTune,
		    pChn->nC4Speed);
	    break;
	    // S3x: Set Vibrato WaveForm
	case 0x30:
	    pChn->nVibratoType = param & 0x07;
	    break;
	    // S4x: Set Tremolo WaveForm
	case 0x40:
	    pChn->nTremoloType = param & 0x07;
	    break;
	    // S5x: Set Panbrello WaveForm
	case 0x50:
	    pChn->nPanbrelloType = param & 0x07;
	    break;
	    // S6x: Pattern Delay for x frames
	case 0x60:
	    that->m_nFrameDelay = param;
	    break;
	    // S7x: Envelope Control
	case 0x70:
	    if (that->m_nTickCount)
		break;
	    switch (param) {
		case 0:
		case 1:
		case 2:
		{
		    MODCHANNEL *bkp = &that->Chn[that->m_nChannels];
		    unsigned i;

		    for (i = that->m_nChannels; i < MAX_CHANNELS;
			i++, bkp++) {
			if (bkp->nMasterChn == nChn + 1) {
			    if (param == 1)
				CSoundFile_KeyOff(that, i);
			    else if (param == 2)
				bkp->dwFlags |= CHN_NOTEFADE;
			    else {
				bkp->dwFlags |= CHN_NOTEFADE;
				bkp->nFadeOutVol = 0;
			    }
			}
		    }
		}
		    break;
		case 3:
		    pChn->nNNA = NNA_NOTECUT;
		    break;
		case 4:
		    pChn->nNNA = NNA_CONTINUE;
		    break;
		case 5:
		    pChn->nNNA = NNA_NOTEOFF;
		    break;
		case 6:
		    pChn->nNNA = NNA_NOTEFADE;
		    break;
		case 7:
		    pChn->dwFlags &= ~CHN_VOLENV;
		    break;
		case 8:
		    pChn->dwFlags |= CHN_VOLENV;
		    break;
		case 9:
		    pChn->dwFlags &= ~CHN_PANENV;
		    break;
		case 10:
		    pChn->dwFlags |= CHN_PANENV;
		    break;
		case 11:
		    pChn->dwFlags &= ~CHN_PITCHENV;
		    break;
		case 12:
		    pChn->dwFlags |= CHN_PITCHENV;
		    break;
	    }
	    break;
	    // S8x: Set 4-bit Panning
	case 0x80:
	    if (!that->m_nTickCount) {
		pChn->nPan = (param << 4) + 8;
		pChn->dwFlags |= CHN_FASTVOLRAMP;
	    }
	    break;
	    // S9x: Set Surround
	case 0x90:
	    CSoundFile_ExtendedChannelEffect(that, pChn, param & 0x0F);
	    break;
	    // SAx: Set 64k Offset
	case 0xA0:
	    if (!that->m_nTickCount) {
		pChn->nOldHiOffset = param;
		if ((pChn->nRowNote) && (pChn->nRowNote < 0x80)) {
		    uint32_t pos = param << 16;

		    if (pos < pChn->nLength)
			pChn->nPos = pos;
		}
	    }
	    break;
	    // SBx: Pattern Loop
	    // SCx: Note Cut
	case 0xC0:
	    CSoundFile_NoteCut(that, nChn, param);
	    break;
	    // SDx: Note Delay
	    // case 0xD0:   break;
	    // SEx: Pattern Delay for x rows
	    // SFx: S3M: Funk Repeat, IT: Set Active Midi Macro
	case 0xF0:
	    pChn->nActiveMacro = param;
	    break;
    }
}

void CSoundFile_ExtendedChannelEffect(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//------------------------------------------------------------------
{
    // S9x and X9x commands (S3M/XM/IT only)
    if (that->m_nTickCount) {
	return;
    }
    switch (param & 0x0F) {
	    // S90: Surround Off
	case 0x00:
	    pChn->dwFlags &= ~CHN_SURROUND;
	    break;
	    // S91: Surround On
	case 0x01:
	    pChn->dwFlags |= CHN_SURROUND;
	    pChn->nPan = 128;
	    break;
	    ////////////////////////////////////////////////////////////
	    // Modplug Extensions
	    // S98: Reverb Off
	case 0x08:
	    pChn->dwFlags &= ~CHN_REVERB;
	    pChn->dwFlags |= CHN_NOREVERB;
	    break;
	    // S99: Reverb On
	case 0x09:
	    pChn->dwFlags &= ~CHN_NOREVERB;
	    pChn->dwFlags |= CHN_REVERB;
	    break;
	    // S9A: 2-Channels surround mode
	case 0x0A:
	    that->m_dwSongFlags &= ~SONG_SURROUNDPAN;
	    break;
	    // S9B: 4-Channels surround mode
	case 0x0B:
	    that->m_dwSongFlags |= SONG_SURROUNDPAN;
	    break;
	    // S9C: IT Filter Mode
	case 0x0C:
	    that->m_dwSongFlags &= ~SONG_MPTFILTERMODE;
	    break;
	    // S9D: MPT Filter Mode
	case 0x0D:
	    that->m_dwSongFlags |= SONG_MPTFILTERMODE;
	    break;
	    // S9E: Go forward
	case 0x0E:
	    pChn->dwFlags &= ~(CHN_PINGPONGFLAG);
	    break;
	    // S9F: Go backward (set position at the end for non-looping samples)
	case 0x0F:
	    if ((!(pChn->dwFlags & CHN_LOOP)) && (!pChn->nPos)
		&& (pChn->nLength)) {
		pChn->nPos = pChn->nLength - 1;
		pChn->nPosLo = 0xFFFF;
	    }
	    pChn->dwFlags |= CHN_PINGPONGFLAG;
	    break;
    }
}

void CSoundFile_ProcessMidiMacro(CSoundFile * that, unsigned nChn,
    const char *pszMidiMacro, unsigned param)
//---------------------------------------------------------------------------
{
    MODCHANNEL *pChn = &that->Chn[nChn];
    uint32_t dwMacro = (*((uint32_t *) pszMidiMacro)) & 0x7F5F7F5F;

    // Not Internal Device ?
    if (dwMacro != 0x30463046) {
	unsigned pos = 0, nNib = 0, nBytes = 0;
	uint32_t dwMidiCode = 0, dwByteCode = 0;

	while (pos + 6 <= 32) {
	    int8_t cData = pszMidiMacro[pos++];

	    if (!cData)
		break;
	    if ((cData >= '0') && (cData <= '9')) {
		dwByteCode = (dwByteCode << 4) | (cData - '0');
		nNib++;
	    } else if ((cData >= 'A') && (cData <= 'F')) {
		dwByteCode = (dwByteCode << 4) | (cData - 'A' + 10);
		nNib++;
	    } else if ((cData >= 'a') && (cData <= 'f')) {
		dwByteCode = (dwByteCode << 4) | (cData - 'a' + 10);
		nNib++;
	    } else if ((cData == 'z') || (cData == 'Z')) {
		dwByteCode = param & 0x7f;
		nNib = 2;
	    } else if ((cData == 'x') || (cData == 'X')) {
		dwByteCode = param & 0x70;
		nNib = 2;
	    } else if ((cData == 'y') || (cData == 'Y')) {
		dwByteCode = (param & 0x0f) << 3;
		nNib = 2;
	    } else if (nNib >= 2) {
		nNib = 0;
		dwMidiCode |= dwByteCode << (nBytes * 8);
		dwByteCode = 0;
		nBytes++;
		if (nBytes >= 3) {
		    unsigned nMasterCh =
			(nChn <
			that->m_nChannels) ? nChn + 1 : pChn->nMasterChn;
		    if ((nMasterCh) && (nMasterCh <= that->m_nChannels)) {
			unsigned nPlug =
			    that->ChnSettings[nMasterCh - 1].nMixPlugin;
			if ((nPlug) && (nPlug <= MAX_MIXPLUGINS)) {
#ifndef MODPLUG_NO_IMIXPLUGIN

			    IMixPlugin *pPlugin =
				that->m_MixPlugins[nPlug - 1].pMixPlugin;
			    if ((pPlugin)
				&& (that->m_MixPlugins[nPlug - 1].pMixState)) {
				pPlugin->MidiSend(dwMidiCode);
			    }
#endif
			}
		    }
		    nBytes = 0;
		    dwMidiCode = 0;
		}
	    }

	}
	return;
    }
    // Internal device
    pszMidiMacro += 4;
    // Filter ?
    if (pszMidiMacro[0] == '0') {
	int8_t cData1 = pszMidiMacro[2];
	uint32_t dwParam = 0;

	if ((cData1 == 'z') || (cData1 == 'Z')) {
	    dwParam = param;
	} else {
	    int8_t cData2 = pszMidiMacro[3];

	    if ((cData1 >= '0') && (cData1 <= '9'))
		dwParam += (cData1 - '0') << 4;
	    else if ((cData1 >= 'A') && (cData1 <= 'F'))
		dwParam += (cData1 - 'A' + 0x0A) << 4;
	    if ((cData2 >= '0') && (cData2 <= '9'))
		dwParam += (cData2 - '0');
	    else if ((cData2 >= 'A') && (cData2 <= 'F'))
		dwParam += (cData2 - 'A' + 0x0A);
	}
	switch (pszMidiMacro[1]) {
		// F0.F0.00.xx: Set CutOff
	    case '0':
	    {
		int oldcutoff = pChn->nCutOff;

		if (dwParam < 0x80)
		    pChn->nCutOff = dwParam;
#ifndef MODPLUG_NO_FILTER
		oldcutoff -= pChn->nCutOff;

		if (oldcutoff < 0)
		    oldcutoff = -oldcutoff;
		if ((pChn->nVolume > 0) || (oldcutoff < 0x10)
		    || (!(pChn->dwFlags & CHN_FILTER))
		    || (!(pChn->nLeftVol | pChn->nRightVol)))
		    CSoundFile_SetupChannelFilter(that, pChn,
			(pChn->dwFlags & CHN_FILTER) ? 0 : 1, 255);
#endif // MODPLUG_NO_FILTER
	    }
		break;

		// F0.F0.01.xx: Set Resonance
	    case '1':
		if (dwParam < 0x80)
		    pChn->nResonance = dwParam;
#ifndef MODPLUG_NO_FILTER
		CSoundFile_SetupChannelFilter(that, pChn,
		    (pChn->dwFlags & CHN_FILTER) ? 0 : 1, 255);
#endif // MODPLUG_NO_FILTER

		break;
	}

    }
}

void CSoundFile_RetrigNote(CSoundFile * that, unsigned nChn, unsigned param)
//------------------------------------------------
{
    // Retrig: bit 8 is set if it's the new XM retrig
    MODCHANNEL *pChn = &that->Chn[nChn];
    unsigned nRetrigSpeed = param & 0x0F;
    unsigned nRetrigCount = pChn->nRetrigCount;
    int bDoRetrig = 0;

    if (that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT)) {
	if (!nRetrigSpeed)
	    nRetrigSpeed = 1;
	if ((nRetrigCount) && (!(nRetrigCount % nRetrigSpeed)))
	    bDoRetrig = 1;
	nRetrigCount++;
    } else {
	unsigned realspeed = nRetrigSpeed;

	if ((param & 0x100) && (pChn->nRowVolCmd == VOLCMD_VOLUME)
	    && (pChn->nRowParam & 0xF0))
	    realspeed++;
	if ((that->m_nTickCount) || (param & 0x100)) {
	    if (!realspeed)
		realspeed = 1;
	    if ((!(param & 0x100)) && (that->m_nMusicSpeed)
		&& (!(that->m_nTickCount % realspeed)))
		bDoRetrig = 1;
	    nRetrigCount++;
	} else if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2))
	    nRetrigCount = 0;
	if (nRetrigCount >= realspeed) {
	    if ((that->m_nTickCount) || ((param & 0x100) && (!pChn->nRowNote)))
		bDoRetrig = 1;
	}
    }
    if (bDoRetrig) {
	unsigned nNote;
	long nOldPeriod;
	int bResetEnv;
	unsigned dv = (param >> 4) & 0x0F;

	if (dv) {
	    int vol = pChn->nVolume;

	    if (CSoundFile_RetrigTable1[dv])
		vol = (vol * CSoundFile_RetrigTable1[dv]) >> 4;
	    else
		vol += ((int)CSoundFile_RetrigTable2[dv]) << 2;
	    if (vol < 0)
		vol = 0;
	    if (vol > 256)
		vol = 256;
	    pChn->nVolume = vol;
	    pChn->dwFlags |= CHN_FASTVOLRAMP;
	}
	nNote = pChn->nNewNote;
	nOldPeriod = pChn->nPeriod;

	if ((nNote) && (nNote <= 120) && (pChn->nLength)) {
	    CSoundFile_CheckNNA(that, nChn, 0, nNote, 1);
	}
	bResetEnv = 0;

	if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) {
	    if ((pChn->nRowInstr) && (param < 0x100)) {
		CSoundFile_InstrumentChange(that, pChn, pChn->nRowInstr, 0, 0, 1);
		bResetEnv = 1;
	    }
	    if (param < 0x100)
		bResetEnv = 1;
	}
	CSoundFile_NoteChange(that, nChn, nNote, 0, bResetEnv);
	if ((that->m_nType & MOD_TYPE_IT) && (!pChn->nRowNote) && (nOldPeriod))
	    pChn->nPeriod = nOldPeriod;
	if (!(that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT)))
	    nRetrigCount = 0;
    }
    pChn->nRetrigCount = (uint8_t) nRetrigCount;
}

void CSoundFile_DoFreqSlide(CSoundFile * that, MODCHANNEL * pChn,
    long nFreqSlide)
//-------------------------------------------------------------
{
    // IT Linear slides
    if (!pChn->nPeriod)
	return;
    if ((that->m_dwSongFlags & SONG_LINEARSLIDES)
	&& (!(that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))) {
	if (nFreqSlide < 0) {
	    unsigned n = (-nFreqSlide) >> 2;

	    if (n > 255)
		n = 255;
	    pChn->nPeriod =
		_muldivr(pChn->nPeriod, CSoundFile_LinearSlideDownTable[n],
		65536);
	} else {
	    unsigned n = (nFreqSlide) >> 2;

	    if (n > 255)
		n = 255;
	    pChn->nPeriod =
		_muldivr(pChn->nPeriod, CSoundFile_LinearSlideUpTable[n],
		65536);
	}
    } else {
	pChn->nPeriod += nFreqSlide;
    }
    if (pChn->nPeriod < 1) {
	pChn->nPeriod = 1;
	if (that->m_nType & MOD_TYPE_IT) {
	    pChn->dwFlags |= CHN_NOTEFADE;
	    pChn->nFadeOutVol = 0;
	}
    }
}

void CSoundFile_NoteCut(CSoundFile * that, unsigned nChn, unsigned nTick)
//---------------------------------------------
{
    if (that->m_nTickCount == nTick) {
	MODCHANNEL *pChn = &that->Chn[nChn];

	// if (that->m_nInstruments) CSoundFile_KeyOff(that, pChn); ?
	pChn->nVolume = 0;
	pChn->dwFlags |= CHN_FASTVOLRAMP;
    }
}

void CSoundFile_KeyOff(CSoundFile * that, unsigned nChn)
//--------------------------------
{
    MODCHANNEL *pChn = &that->Chn[nChn];
    int bKeyOn = (pChn->dwFlags & CHN_KEYOFF) ? 0 : 1;

    pChn->dwFlags |= CHN_KEYOFF;
    //if ((!pChn->pHeader) || (!(pChn->dwFlags & CHN_VOLENV)))
    if ((pChn->pHeader) && (!(pChn->dwFlags & CHN_VOLENV))) {
	pChn->dwFlags |= CHN_NOTEFADE;
    }
    if (!pChn->nLength)
	return;
    if ((pChn->dwFlags & CHN_SUSTAINLOOP) && (pChn->pInstrument) && (bKeyOn)) {
	MODINSTRUMENT *psmp = pChn->pInstrument;

	if (psmp->uFlags & CHN_LOOP) {
	    if (psmp->uFlags & CHN_PINGPONGLOOP)
		pChn->dwFlags |= CHN_PINGPONGLOOP;
	    else
		pChn->dwFlags &= ~(CHN_PINGPONGLOOP | CHN_PINGPONGFLAG);
	    pChn->dwFlags |= CHN_LOOP;
	    pChn->nLength = psmp->nLength;
	    pChn->nLoopStart = psmp->nLoopStart;
	    pChn->nLoopEnd = psmp->nLoopEnd;
	    if (pChn->nLength > pChn->nLoopEnd)
		pChn->nLength = pChn->nLoopEnd;
	} else {
	    pChn->dwFlags &= ~(CHN_LOOP | CHN_PINGPONGLOOP | CHN_PINGPONGFLAG);
	    pChn->nLength = psmp->nLength;
	}
    }
    if (pChn->pHeader) {
	INSTRUMENTHEADER *penv = pChn->pHeader;

	if (((penv->dwFlags & ENV_VOLLOOP)
		|| (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)))
	    && (penv->nFadeOut))
	    pChn->dwFlags |= CHN_NOTEFADE;
    }
}

//////////////////////////////////////////////////////////
// CSoundFile: Global Effects

void CSoundFile_SetSpeed(CSoundFile * that, unsigned param)
//-----------------------------------
{
    unsigned max = (that->m_nType == MOD_TYPE_IT) ? 256 : 128;

    // Modplug Tracker and Mod-Plugin don't do this check
#ifndef MODPLUG_TRACKER
#ifndef MODPLUG_FASTSOUNDLIB
    // Big Hack!!!
    if ((!param) || (param >= 0x80)
	|| ((that->m_nType & (MOD_TYPE_MOD | MOD_TYPE_XM | MOD_TYPE_MT2))
	    && (param >= 0x1E))) {
	if (CSoundFile_IsSongFinished(that, that->m_nCurrentPattern,
		that->m_nRow + 1)) {
	    CSoundFile_GlobalFadeSong(that, 1000);
	}
    }
#endif // MODPLUG_FASTSOUNDLIB
#endif // MODPLUG_TRACKER
    if ((that->m_nType & MOD_TYPE_S3M) && (param > 0x80))
	param -= 0x80;
    if ((param) && (param <= max))
	that->m_nMusicSpeed = param;
}

void CSoundFile_SetTempo(CSoundFile * that, unsigned param)
//-----------------------------------
{
    if (param < 0x20) {
	// Tempo Slide
	if ((param & 0xF0) == 0x10) {
	    that->m_nMusicTempo += (param & 0x0F) * 2;
	    if (that->m_nMusicTempo > 255)
		that->m_nMusicTempo = 255;
	} else {
	    that->m_nMusicTempo -= (param & 0x0F) * 2;
	    if ((long)that->m_nMusicTempo < 32)
		that->m_nMusicTempo = 32;
	}
    } else {
	that->m_nMusicTempo = param;
    }
}

int CSoundFile_PatternLoop(CSoundFile * that, MODCHANNEL * pChn,
    unsigned param)
//-------------------------------------------------------
{
    if (param) {
	if (pChn->nPatternLoopCount) {
	    pChn->nPatternLoopCount--;
	    if (!pChn->nPatternLoopCount) {
		return -1;
	    }
	} else {
	    MODCHANNEL *p = that->Chn;
	    unsigned i;

	    for (i = 0; i < that->m_nChannels; i++, p++) {
		if (p != pChn) {	// Loop already done
		    if (p->nPatternLoopCount) {
			return -1;
		    }
		}
	    }
	    pChn->nPatternLoopCount = param;
	}
	return pChn->nPatternLoop;
    } else {
	pChn->nPatternLoop = that->m_nRow;
    }
    return -1;
}

void CSoundFile_GlobalVolSlide(CSoundFile * that, unsigned param)
//-----------------------------------------
{
    long nGlbSlide = 0;

    if (param) {
	that->m_nOldGlbVolSlide = param;
    } else {
	param = that->m_nOldGlbVolSlide;
    }
    if (((param & 0x0F) == 0x0F) && (param & 0xF0)) {
	if (that->m_dwSongFlags & SONG_FIRSTTICK) {
	    nGlbSlide = (param >> 4) * 2;
	}
    } else if (((param & 0xF0) == 0xF0) && (param & 0x0F)) {
	if (that->m_dwSongFlags & SONG_FIRSTTICK)
	    nGlbSlide = -(int)((param & 0x0F) * 2);
    } else {
	if (!(that->m_dwSongFlags & SONG_FIRSTTICK)) {
	    if (param & 0xF0)
		nGlbSlide = (int)((param & 0xF0) >> 4) * 2;
	    else
		nGlbSlide = -(int)((param & 0x0F) * 2);
	}
    }
    if (nGlbSlide) {
	if (that->m_nType != MOD_TYPE_IT)
	    nGlbSlide *= 2;
	nGlbSlide += that->m_nGlobalVolume;
	if (nGlbSlide < 0)
	    nGlbSlide = 0;
	if (nGlbSlide > 256)
	    nGlbSlide = 256;
	that->m_nGlobalVolume = nGlbSlide;
    }
}

uint32_t CSoundFile_IsSongFinished(CSoundFile * that, unsigned nStartOrder,
    unsigned nStartRow)
//----------------------------------------------------------------------
{
    unsigned nOrd;

    for (nOrd = nStartOrder; nOrd < MAX_ORDERS; nOrd++) {
	unsigned nPat = that->Order[nOrd];

	if (nPat != 0xFE) {
	    MODCOMMAND *p;

	    if (nPat >= MAX_PATTERNS)
		break;
	    p = that->Patterns[nPat];
	    if (p) {
		unsigned len = that->PatternSize[nPat] * that->m_nChannels;
		unsigned pos = (nOrd == nStartOrder) ? nStartRow : 0;

		pos *= that->m_nChannels;
		while (pos < len) {
		    unsigned cmd;

		    if ((p[pos].note) || (p[pos].volcmd))
			return 0;
		    cmd = p[pos].command;
		    if (cmd == CMD_MODCMDEX) {
			unsigned cmdex = p[pos].param & 0xF0;

			if ((!cmdex) || (cmdex == 0x60) || (cmdex == 0xE0)
			    || (cmdex == 0xF0))
			    cmd = 0;
		    }
		    if ((cmd) && (cmd != CMD_SPEED) && (cmd != CMD_TEMPO))
			return 0;
		    pos++;
		}
	    }
	}
    }
    return (nOrd < MAX_ORDERS) ? nOrd : MAX_ORDERS - 1;
}

int CSoundFile_IsValidBackwardJump(CSoundFile * that, unsigned nStartOrder,
    unsigned nStartRow, unsigned nJumpOrder, unsigned nJumpRow)
//----------------------------------------------------------------------------------------------------------
{
    uint8_t row_hist[256];
    unsigned nRows;
    unsigned row;

    while ((nJumpOrder < MAX_PATTERNS) && (that->Order[nJumpOrder] == 0xFE)) {
	nJumpOrder++;
    }
    if ((nStartOrder >= MAX_PATTERNS) || (nJumpOrder >= MAX_PATTERNS)) {
	return 0;
    }
    // Treat only case with jumps in the same pattern
    if (nJumpOrder > nStartOrder) {
	return 1;
    }
    if ((nJumpOrder < nStartOrder)
	|| (nJumpRow >= that->PatternSize[nStartOrder])
	|| (!that->Patterns[nStartOrder]) || (nStartRow >= 256)
	|| (nJumpRow >= 256)) {
	return 0;
    }
    // See if the pattern is being played backward
    memset(row_hist, 0, sizeof(row_hist));
    nRows = that->PatternSize[nStartOrder];
    row = nJumpRow;

    if (nRows > 256)
	nRows = 256;
    row_hist[nStartRow] = 1;
    while ((row < 256) && (!row_hist[row])) {
	unsigned i;
	MODCOMMAND *p;
	int breakrow;
	int posjump;

	if (row >= nRows)
	    return 1;
	row_hist[row] = 1;
	p = that->Patterns[nStartOrder] + row * that->m_nChannels;

	row++;
	breakrow = -1;
	posjump = 0;

	for (i = 0; i < that->m_nChannels; i++, p++) {
	    if (p->command == CMD_POSITIONJUMP) {
		if (p->param < nStartOrder)
		    return 0;
		if (p->param > nStartOrder)
		    return 1;
		posjump = 1;
	    } else if (p->command == CMD_PATTERNBREAK) {
		breakrow = p->param;
	    }
	}
	if (breakrow >= 0) {
	    if (!posjump) {
		return 1;
	    }
	    row = breakrow;
	}
	if (row >= nRows) {
	    return 1;
	}
    }
    return 0;
}

//////////////////////////////////////////////////////
// Note/Period/Frequency functions

unsigned CSoundFile_GetNoteFromPeriod(CSoundFile const * that, unsigned period)
//---------------------------------------------------
{
    unsigned i;

    if (!period) {
	return 0;
    }
    if (that-> m_nType & (MOD_TYPE_MED
	    | MOD_TYPE_MOD | MOD_TYPE_MTM | MOD_TYPE_669 | MOD_TYPE_OKT
	    | MOD_TYPE_AMF0)) {

	period >>= 2;
	for (i = 0; i < 6 * 12; i++) {
	    if (period >= CSoundFile_ProTrackerPeriodTable[i]) {
		if ((period != CSoundFile_ProTrackerPeriodTable[i]) && (i)) {
		    unsigned p1 = CSoundFile_ProTrackerPeriodTable[i - 1];
		    unsigned p2 = CSoundFile_ProTrackerPeriodTable[i];

		    if (p1 - period < (period - p2))
			return i + 36;
		}
		return i + 1 + 36;
	    }
	}
	return 6 * 12 + 36;
    } else {
	for (i = 1; i < 120; i++) {
	    long n = CSoundFile_GetPeriodFromNote(that, i, 0, 0);

	    if ((n > 0) && (n <= (long)period))
		return i;
	}
	return 120;
    }
}

unsigned CSoundFile_GetPeriodFromNote(CSoundFile const * that, unsigned note,
    int nFineTune, unsigned nC4Speed)
//-------------------------------------------------------------------------------
{
    if ((!note) || (note > 0xF0))
	return 0;
    if (that->
	m_nType & (MOD_TYPE_IT | MOD_TYPE_S3M | MOD_TYPE_STM | MOD_TYPE_MDL |
	    MOD_TYPE_ULT | MOD_TYPE_WAV | MOD_TYPE_FAR | MOD_TYPE_DMF |
	    MOD_TYPE_PTM | MOD_TYPE_AMS | MOD_TYPE_DBM | MOD_TYPE_AMF |
	    MOD_TYPE_PSM)) {
	note--;
	if (that->m_dwSongFlags & SONG_LINEARSLIDES) {
	    return (CSoundFile_FreqS3MTable[note % 12] << 5) >> (note / 12);
	} else {
	    if (!nC4Speed)
		nC4Speed = 8363;
	    return _muldiv(8363, (CSoundFile_FreqS3MTable[note % 12] << 5),
		nC4Speed << (note / 12));
	}
    } else if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) {
	if (note < 13)
	    note = 13;
	note -= 13;
	if (that->m_dwSongFlags & SONG_LINEARSLIDES) {
	    long l = ((120 - note) << 6) - (nFineTune / 2);

	    if (l < 1)
		l = 1;
	    return (unsigned)l;
	} else {
	    int finetune = nFineTune;
	    unsigned rnote = (note % 12) << 3;
	    unsigned roct = note / 12;
	    int rfine = finetune / 16;
	    int i = rnote + rfine + 8;
	    unsigned per1;
	    unsigned per2;

	    if (i < 0)
		i = 0;
	    if (i >= 104)
		i = 103;
	    per1 = CSoundFile_XMPeriodTable[i];

	    if (finetune < 0) {
		rfine--;
		finetune = -finetune;
	    } else
		rfine++;
	    i = rnote + rfine + 8;
	    if (i < 0)
		i = 0;
	    if (i >= 104)
		i = 103;
	    per2 = CSoundFile_XMPeriodTable[i];

	    rfine = finetune & 0x0F;
	    per1 *= 16 - rfine;
	    per2 *= rfine;
	    return ((per1 + per2) << 1) >> roct;
	}
    } else {
	note--;
	nFineTune = XM2MODFineTune(nFineTune);
	if ((nFineTune) || (note < 36) || (note >= 36 + 6 * 12))
	    return (CSoundFile_ProTrackerTunedPeriods[nFineTune * 12 +
		    note % 12] << 5) >> (note / 12);
	else
	    return (CSoundFile_ProTrackerPeriodTable[note - 36] << 2);
    }
}

/*
**	Convert period to frequenz
*/
unsigned CSoundFile_GetFreqFromPeriod(CSoundFile const * that, unsigned period,
    unsigned nC4Speed, int nPeriodFrac)
{
    if (!period) {
	return 0;
    }
    if (that->
	m_nType & (MOD_TYPE_MED | MOD_TYPE_MOD | MOD_TYPE_MTM | MOD_TYPE_669 |
	    MOD_TYPE_OKT | MOD_TYPE_AMF0)) {
	return (3546895L * 4) / period;
    }
    if (that->m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) {
	if (that->m_dwSongFlags & SONG_LINEARSLIDES) {
	    return CSoundFile_XMLinearTable[period % 768] >> (period / 768);
	}
	return 8363 * 1712L / period;
    }
    if (that->m_dwSongFlags & SONG_LINEARSLIDES) {
	if (!nC4Speed) {
	    nC4Speed = 8363;
	}
	return _muldiv(nC4Speed, 1712L << 8, (period << 8) + nPeriodFrac);
    }
    return _muldiv(8363, 1712L << 8, (period << 8) + nPeriodFrac);
}
