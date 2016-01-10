/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: load_xm.c,v 1.2 2002/08/30 20:32:24 jsalmon3 Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

////////////////////////////////////////////////////////
// FastTracker II XM file support

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif

#pragma pack(1)
typedef struct tagXMFILEHEADER
{
    uint32_t size;
    uint16_t norder;
    uint16_t restartpos;
    uint16_t channels;
    uint16_t patterns;
    uint16_t instruments;
    uint16_t flags;
    uint16_t speed;
    uint16_t tempo;
    uint8_t order[256];
} XMFILEHEADER;

typedef struct tagXMINSTRUMENTHEADER
{
    uint32_t size;
    int8_t Name[22];
    uint8_t type;
    uint8_t samples;
    uint8_t samplesh;
} XMINSTRUMENTHEADER;

typedef struct tagXMSAMPLEHEADER
{
    uint32_t shsize;
    uint8_t snum[96];
    uint16_t venv[24];
    uint16_t penv[24];
    uint8_t vnum, pnum;
    uint8_t vsustain, vloops, vloope, psustain, ploops, ploope;
    uint8_t vtype, ptype;
    uint8_t vibtype, vibsweep, vibdepth, vibrate;
    uint16_t volfade;
    uint16_t res;
    uint8_t reserved1[20];
} XMSAMPLEHEADER;

typedef struct tagXMSAMPLESTRUCT
{
    uint32_t samplen;
    uint32_t loopstart;
    uint32_t looplen;
    uint8_t vol;
    int8_t finetune;
    uint8_t type;
    uint8_t pan;
    int8_t relnote;
    uint8_t res;
    char Name[22];
} XMSAMPLESTRUCT;

#pragma pack()

int CSoundFile_ReadXM(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//--------------------------------------------------------------
{
    XMSAMPLEHEADER xmsh;
    XMSAMPLESTRUCT xmss;
    uint32_t dwMemPos, dwHdrSize;
    uint16_t norders = 0, restartpos = 0, channels = 0, patterns =
	0, instruments = 0;
    uint16_t xmflags = 0, deftempo = 125, defspeed = 6;
    int InstUsed[256];
    uint8_t channels_used[MAX_CHANNELS];
    uint8_t pattern_map[256];
    int samples_used[MAX_SAMPLES];
    unsigned unused_samples;
    unsigned i;
    unsigned j;
    unsigned ienv;

    that->m_nChannels = 0;
    if ((!lpStream) || (dwMemLength < 0x200))
	return 0;
    if (strnicmp((const char *)lpStream, "Extended Module", 15))
	return 0;

    memcpy(that->m_szNames[0], lpStream + 17, 20);
    dwHdrSize = bswapLE32(*((uint32_t *) (lpStream + 60)));
    norders = bswapLE16(*((uint16_t *) (lpStream + 64)));
    if ((!norders) || (norders > MAX_ORDERS))
	return 0;
    restartpos = bswapLE16(*((uint16_t *) (lpStream + 66)));
    channels = bswapLE16(*((uint16_t *) (lpStream + 68)));
    if ((!channels) || (channels > 64))
	return 0;
    that->m_nType = MOD_TYPE_XM;
    that->m_nMinPeriod = 27;
    that->m_nMaxPeriod = 54784;
    that->m_nChannels = channels;
    if (restartpos < norders)
	that->m_nRestartPos = restartpos;
    patterns = bswapLE16(*((uint16_t *) (lpStream + 70)));
    if (patterns > 256)
	patterns = 256;
    instruments = bswapLE16(*((uint16_t *) (lpStream + 72)));
    if (instruments >= MAX_INSTRUMENTS)
	instruments = MAX_INSTRUMENTS - 1;
    that->m_nInstruments = instruments;
    that->m_nSamples = 0;
    memcpy(&xmflags, lpStream + 74, 2);
    xmflags = bswapLE16(xmflags);
    if (xmflags & 1)
	that->m_dwSongFlags |= SONG_LINEARSLIDES;
    if (xmflags & 0x1000)
	that->m_dwSongFlags |= SONG_EXFILTERRANGE;
    defspeed = bswapLE16(*((uint16_t *) (lpStream + 76)));
    deftempo = bswapLE16(*((uint16_t *) (lpStream + 78)));
    if ((deftempo >= 32) && (deftempo < 256))
	that->m_nDefaultTempo = deftempo;
    if ((defspeed > 0) && (defspeed < 40))
	that->m_nDefaultSpeed = defspeed;
    memcpy(that->Order, lpStream + 80, norders);
    memset(InstUsed, 0, sizeof(InstUsed));
    if (patterns > MAX_PATTERNS) {

	for (i = 0; i < norders; i++) {
	    if (that->Order[i] < patterns)
		InstUsed[that->Order[i]] = 1;
	}
	j = 0;
	for (i = 0; i < 256; i++) {
	    if (InstUsed[i])
		pattern_map[i] = j++;
	}
	for (i = 0; i < 256; i++) {
	    if (!InstUsed[i]) {
		pattern_map[i] = (j < MAX_PATTERNS) ? j : 0xFE;
		j++;
	    }
	}
	for (i = 0; i < norders; i++) {
	    that->Order[i] = pattern_map[that->Order[i]];
	}
    } else {
	for (i = 0; i < 256; i++) {
	    pattern_map[i] = i;
	}
    }
    memset(InstUsed, 0, sizeof(InstUsed));
    dwMemPos = dwHdrSize + 60;
    if (dwMemPos + 8 >= dwMemLength)
	return 1;
    // Reading patterns
    memset(channels_used, 0, sizeof(channels_used));
    for (i = 0; i < patterns; i++) {
	unsigned ipatmap = pattern_map[i];
	uint32_t dwSize = 0;
	uint16_t rows = 64, packsize = 0;
	unsigned row;
	MODCOMMAND *p;
	const uint8_t *src;

	dwSize = bswapLE32(*((uint32_t *) (lpStream + dwMemPos)));
	while ((dwMemPos + dwSize >= dwMemLength) || (dwSize & 0xFFFFFF00)) {
	    if (dwMemPos + 4 >= dwMemLength)
		break;
	    dwMemPos++;
	    dwSize = bswapLE32(*((uint32_t *) (lpStream + dwMemPos)));
	}
	rows = bswapLE16(*((uint16_t *) (lpStream + dwMemPos + 5)));
	if ((!rows) || (rows > 256))
	    rows = 64;
	packsize = bswapLE16(*((uint16_t *) (lpStream + dwMemPos + 7)));
	if (dwMemPos + dwSize + 4 > dwMemLength)
	    return 1;
	dwMemPos += dwSize;
	if (dwMemPos + packsize + 4 > dwMemLength) {
	    return 1;
	}

	if (ipatmap < MAX_PATTERNS) {
	    that->PatternSize[ipatmap] = rows;
	    if ((that->Patterns[ipatmap] =
		    CSoundFile_AllocatePattern(rows,
			that->m_nChannels)) == NULL)
		return 1;
	    if (!packsize)
		continue;
	    p = that->Patterns[ipatmap];
	} else {
	    p = NULL;
	}
	src = lpStream + dwMemPos;
	j = 0;
	for (row = 0; row < rows; row++) {
	    unsigned chn;

	    for (chn = 0; chn < that->m_nChannels; chn++) {
		if ((p) && (j < packsize)) {
		    uint8_t b = src[j++];
		    unsigned vol = 0;

		    if (b & 0x80) {
			if (b & 1)
			    p->note = src[j++];
			if (b & 2)
			    p->instr = src[j++];
			if (b & 4)
			    vol = src[j++];
			if (b & 8)
			    p->command = src[j++];
			if (b & 16)
			    p->param = src[j++];
		    } else {
			p->note = b;
			p->instr = src[j++];
			vol = src[j++];
			p->command = src[j++];
			p->param = src[j++];
		    }
		    if (p->note == 97)
			p->note = 0xFF;
		    else if ((p->note) && (p->note < 97))
			p->note += 12;
		    if (p->note)
			channels_used[chn] = 1;
		    if (p->command | p->param)
			CSoundFile_ConvertModCommand(that, p);
		    if (p->instr == 0xff)
			p->instr = 0;
		    if (p->instr)
			InstUsed[p->instr] = 1;
		    if ((vol >= 0x10) && (vol <= 0x50)) {
			p->volcmd = VOLCMD_VOLUME;
			p->vol = vol - 0x10;
		    } else if (vol >= 0x60) {
			unsigned v = vol & 0xF0;

			vol &= 0x0F;
			p->vol = vol;
			switch (v) {
				// 60-6F: Volume Slide Down
			    case 0x60:
				p->volcmd = VOLCMD_VOLSLIDEDOWN;
				break;
				// 70-7F: Volume Slide Up:
			    case 0x70:
				p->volcmd = VOLCMD_VOLSLIDEUP;
				break;
				// 80-8F: Fine Volume Slide Down
			    case 0x80:
				p->volcmd = VOLCMD_FINEVOLDOWN;
				break;
				// 90-9F: Fine Volume Slide Up
			    case 0x90:
				p->volcmd = VOLCMD_FINEVOLUP;
				break;
				// A0-AF: Set Vibrato Speed
			    case 0xA0:
				p->volcmd = VOLCMD_VIBRATOSPEED;
				break;
				// B0-BF: Vibrato
			    case 0xB0:
				p->volcmd = VOLCMD_VIBRATO;
				break;
				// C0-CF: Set Panning
			    case 0xC0:
				p->volcmd = VOLCMD_PANNING;
				p->vol = (vol << 2) + 2;
				break;
				// D0-DF: Panning Slide Left
			    case 0xD0:
				p->volcmd = VOLCMD_PANSLIDELEFT;
				break;
				// E0-EF: Panning Slide Right
			    case 0xE0:
				p->volcmd = VOLCMD_PANSLIDERIGHT;
				break;
				// F0-FF: Tone Portamento
			    case 0xF0:
				p->volcmd = VOLCMD_TONEPORTAMENTO;
				break;
			}
		    }
		    p++;
		} else if (j < packsize) {
		    uint8_t b = src[j++];

		    if (b & 0x80) {
			if (b & 1)
			    j++;
			if (b & 2)
			    j++;
			if (b & 4)
			    j++;
			if (b & 8)
			    j++;
			if (b & 16)
			    j++;
		    } else
			j += 4;
		} else
		    break;
	    }
	}
	dwMemPos += packsize;
    }
    // Wrong offset check
    while (dwMemPos + 4 < dwMemLength) {
	uint32_t d = bswapLE32(*((uint32_t *) (lpStream + dwMemPos)));

	if (d < 0x300)
	    break;
	dwMemPos++;
    }
    memset(samples_used, 0, sizeof(samples_used));
    unused_samples = 0;
    // Reading instruments
    for (i = 1; i <= instruments; i++) {
	XMINSTRUMENTHEADER *pih;
	INSTRUMENTHEADER *penv;
	uint8_t flags[32];
	uint32_t samplesize[32];
	unsigned samplemap[32];
	uint16_t nsamples;
	unsigned newsamples;
	unsigned nmap;

	if (dwMemPos + sizeof(XMINSTRUMENTHEADER) >= dwMemLength)
	    return 1;
	pih = (XMINSTRUMENTHEADER *) (lpStream + dwMemPos);
	if (dwMemPos + bswapLE32(pih->size) > dwMemLength)
	    return 1;
	if ((that->Headers[i] = malloc(sizeof(INSTRUMENTHEADER))) == NULL)
	    continue;
	memset(that->Headers[i], 0, sizeof(INSTRUMENTHEADER));
	memcpy(that->Headers[i]->Name, pih->Name, 22);
	if ((nsamples = pih->samples) > 0) {
	    if (dwMemPos + sizeof(XMSAMPLEHEADER) > dwMemLength)
		return 1;
	    memcpy(&xmsh, lpStream + dwMemPos + sizeof(XMINSTRUMENTHEADER),
		sizeof(XMSAMPLEHEADER));
	    xmsh.shsize = bswapLE32(xmsh.shsize);
	    for (j = 0; j < 24; ++j) {
		xmsh.venv[j] = bswapLE16(xmsh.venv[j]);
		xmsh.penv[j] = bswapLE16(xmsh.penv[j]);
	    }
	    xmsh.volfade = bswapLE16(xmsh.volfade);
	    xmsh.res = bswapLE16(xmsh.res);
	    dwMemPos += bswapLE32(pih->size);
	} else {
	    if (bswapLE32(pih->size))
		dwMemPos += bswapLE32(pih->size);
	    else
		dwMemPos += sizeof(XMINSTRUMENTHEADER);
	    continue;
	}
	memset(samplemap, 0, sizeof(samplemap));
	if (nsamples > 32) {
	    return 1;
	}

	newsamples = that->m_nSamples;
	for (nmap = 0; nmap < nsamples; nmap++) {
	    unsigned n = that->m_nSamples + nmap + 1;

	    if (n >= MAX_SAMPLES) {
		n = that->m_nSamples;
		while (n > 0) {
		    if (!that->Ins[n].pSample) {
			unsigned xmapchk;
			unsigned clrs;

			for (xmapchk = 0; xmapchk < nmap; xmapchk++) {
			    if (samplemap[xmapchk] == n)
				goto alreadymapped;
			}
			for (clrs = 1; clrs < i; clrs++)
			    if (that->Headers[clrs]) {
				INSTRUMENTHEADER *pks = that->Headers[clrs];
				unsigned ks;

				for (ks = 0; ks < 128; ks++) {
				    if (pks->Keyboard[ks] == n)
					pks->Keyboard[ks] = 0;
				}
			    }
			break;
		    }
		  alreadymapped:
		    n--;
		}
#ifndef MODPLUG_FASTSOUNDLIB
		// Damn! more than 200 samples: look for duplicates
		if (!n) {
		    if (!unused_samples) {
			unused_samples =
			    CSoundFile_DetectUnusedSamples(that,samples_used);
			if (!unused_samples) {
			    unused_samples = 0xFFFF;
			}
		    }
		    if ((unused_samples) && (unused_samples != 0xFFFF)) {
			unsigned iext;

			for (iext = that->m_nSamples; iext >= 1;
			    iext--)
			    if (!samples_used[iext]) {
				unsigned mapchk;
				unsigned clrs;

				unused_samples--;
				samples_used[iext] = 1;
				CSoundFile_DestroySample(that,iext);
				n = iext;
				for (mapchk = 0; mapchk < nmap;
				    mapchk++) {
				    if (samplemap[mapchk] == n)
					samplemap[mapchk] = 0;
				}
				for (clrs = 1; clrs < i; clrs++) {
				    if (that->Headers[clrs]) {
					unsigned ks;

					INSTRUMENTHEADER *pks =
					    that->Headers[clrs];
					for (ks = 0; ks < 128; ks++) {
					    if (pks->Keyboard[ks] == n)
						pks->Keyboard[ks] = 0;
					}
				    }
				}
				memset(&that->Ins[n], 0, sizeof(that->Ins[0]));
				break;
			    }
		    }
		}
#endif // MODPLUG_FASTSOUNDLIB
	    }
	    if (newsamples < n)
		newsamples = n;
	    samplemap[nmap] = n;
	}
	that->m_nSamples = newsamples;
	// Reading Volume Envelope
	penv = that->Headers[i];

	penv->nMidiProgram = pih->type;
	penv->nFadeOut = xmsh.volfade;
	penv->nPan = 128;
	penv->nPPC = 5 * 12;
	if (xmsh.vtype & 1)
	    penv->dwFlags |= ENV_VOLUME;
	if (xmsh.vtype & 2)
	    penv->dwFlags |= ENV_VOLSUSTAIN;
	if (xmsh.vtype & 4)
	    penv->dwFlags |= ENV_VOLLOOP;
	if (xmsh.ptype & 1)
	    penv->dwFlags |= ENV_PANNING;
	if (xmsh.ptype & 2)
	    penv->dwFlags |= ENV_PANSUSTAIN;
	if (xmsh.ptype & 4)
	    penv->dwFlags |= ENV_PANLOOP;
	if (xmsh.vnum > 12)
	    xmsh.vnum = 12;
	if (xmsh.pnum > 12)
	    xmsh.pnum = 12;
	penv->nVolEnv = xmsh.vnum;
	if (!xmsh.vnum)
	    penv->dwFlags &= ~ENV_VOLUME;
	if (!xmsh.pnum)
	    penv->dwFlags &= ~ENV_PANNING;
	penv->nPanEnv = xmsh.pnum;
	penv->nVolSustainBegin = penv->nVolSustainEnd = xmsh.vsustain;
	if (xmsh.vsustain >= 12)
	    penv->dwFlags &= ~ENV_VOLSUSTAIN;
	penv->nVolLoopStart = xmsh.vloops;
	penv->nVolLoopEnd = xmsh.vloope;
	if (penv->nVolLoopEnd >= 12)
	    penv->nVolLoopEnd = 0;
	if (penv->nVolLoopStart >= penv->nVolLoopEnd)
	    penv->dwFlags &= ~ENV_VOLLOOP;
	penv->nPanSustainBegin = penv->nPanSustainEnd = xmsh.psustain;
	if (xmsh.psustain >= 12)
	    penv->dwFlags &= ~ENV_PANSUSTAIN;
	penv->nPanLoopStart = xmsh.ploops;
	penv->nPanLoopEnd = xmsh.ploope;
	if (penv->nPanLoopEnd >= 12)
	    penv->nPanLoopEnd = 0;
	if (penv->nPanLoopStart >= penv->nPanLoopEnd)
	    penv->dwFlags &= ~ENV_PANLOOP;
	penv->nGlobalVol = 64;
	for (ienv = 0; ienv < 12; ienv++) {
	    penv->VolPoints[ienv] = (uint16_t) xmsh.venv[ienv * 2];
	    penv->VolEnv[ienv] = (uint8_t) xmsh.venv[ienv * 2 + 1];
	    penv->PanPoints[ienv] = (uint16_t) xmsh.penv[ienv * 2];
	    penv->PanEnv[ienv] = (uint8_t) xmsh.penv[ienv * 2 + 1];
	    if (ienv) {
		if (penv->VolPoints[ienv] < penv->VolPoints[ienv - 1]) {
		    penv->VolPoints[ienv] &= 0xFF;
		    penv->VolPoints[ienv] +=
			penv->VolPoints[ienv - 1] & 0xFF00;
		    if (penv->VolPoints[ienv] < penv->VolPoints[ienv - 1])
			penv->VolPoints[ienv] += 0x100;
		}
		if (penv->PanPoints[ienv] < penv->PanPoints[ienv - 1]) {
		    penv->PanPoints[ienv] &= 0xFF;
		    penv->PanPoints[ienv] +=
			penv->PanPoints[ienv - 1] & 0xFF00;
		    if (penv->PanPoints[ienv] < penv->PanPoints[ienv - 1])
			penv->PanPoints[ienv] += 0x100;
		}
	    }
	}
	for (j = 0; j < 96; j++) {
	    penv->NoteMap[j + 12] = j + 1 + 12;
	    if (xmsh.snum[j] < nsamples)
		penv->Keyboard[j + 12] = samplemap[xmsh.snum[j]];
	}
	// Reading samples
	for (j = 0; j < nsamples; j++) {
	    unsigned imapsmp;
	    MODINSTRUMENT *pins;

	    if ((dwMemPos + sizeof(xmss) > dwMemLength)
		    || (dwMemPos + xmsh.shsize > dwMemLength)) {
		return 1;
	    }
	    memcpy(&xmss, lpStream + dwMemPos, sizeof(xmss));
	    xmss.samplen = bswapLE32(xmss.samplen);
	    xmss.loopstart = bswapLE32(xmss.loopstart);
	    xmss.looplen = bswapLE32(xmss.looplen);
	    dwMemPos += xmsh.shsize;
	    flags[j] = (xmss.type & 0x10) ? RS_PCM16D : RS_PCM8D;
	    if (xmss.type & 0x20)
		flags[j] = (xmss.type & 0x10) ? RS_STPCM16D : RS_STPCM8D;
	    samplesize[j] = xmss.samplen;
	    if (!samplemap[j])
		continue;
	    if (xmss.type & 0x10) {
		xmss.looplen >>= 1;
		xmss.loopstart >>= 1;
		xmss.samplen >>= 1;
	    }
	    if (xmss.type & 0x20) {
		xmss.looplen >>= 1;
		xmss.loopstart >>= 1;
		xmss.samplen >>= 1;
	    }
	    if (xmss.samplen > MAX_SAMPLE_LENGTH)
		xmss.samplen = MAX_SAMPLE_LENGTH;
	    if (xmss.loopstart >= xmss.samplen)
		xmss.type &= ~3;
	    xmss.looplen += xmss.loopstart;
	    if (xmss.looplen > xmss.samplen)
		xmss.looplen = xmss.samplen;
	    if (!xmss.looplen)
		xmss.type &= ~3;
	    imapsmp = samplemap[j];

	    memcpy(that->m_szNames[imapsmp], xmss.Name, 22);
	    that->m_szNames[imapsmp][22] = 0;
	    pins = &that->Ins[imapsmp];

	    pins->nLength =
		(xmss.samplen >
		MAX_SAMPLE_LENGTH) ? MAX_SAMPLE_LENGTH : xmss.samplen;
	    pins->nLoopStart = xmss.loopstart;
	    pins->nLoopEnd = xmss.looplen;
	    if (pins->nLoopEnd > pins->nLength)
		pins->nLoopEnd = pins->nLength;
	    if (pins->nLoopStart >= pins->nLoopEnd) {
		pins->nLoopStart = pins->nLoopEnd = 0;
	    }
	    if (xmss.type & 3)
		pins->uFlags |= CHN_LOOP;
	    if (xmss.type & 2)
		pins->uFlags |= CHN_PINGPONGLOOP;
	    pins->nVolume = xmss.vol << 2;
	    if (pins->nVolume > 256)
		pins->nVolume = 256;
	    pins->nGlobalVol = 64;
	    if ((xmss.res == 0xAD) && (!(xmss.type & 0x30))) {
		flags[j] = RS_ADPCM4;
		samplesize[j] = (samplesize[j] + 1) / 2 + 16;
	    }
	    pins->nFineTune = xmss.finetune;
	    pins->RelativeTone = (int)xmss.relnote;
	    pins->nPan = xmss.pan;
	    pins->uFlags |= CHN_PANNING;
	    pins->nVibType = xmsh.vibtype;
	    pins->nVibSweep = xmsh.vibsweep;
	    pins->nVibDepth = xmsh.vibdepth;
	    pins->nVibRate = xmsh.vibrate;
	    memcpy(pins->Name, xmss.Name, 22);
	    pins->Name[21] = 0;
	}
#if 0
	if ((xmsh.reserved2 > nsamples) && (xmsh.reserved2 <= 16)) {
	    dwMemPos += (((unsigned)xmsh.reserved2) - nsamples) * xmsh.shsize;
	}
#endif
	for (j = 0; j < nsamples; j++) {
	    if ((samplemap[j]) && (samplesize[j])
		&& (dwMemPos < dwMemLength)) {
		CSoundFile_ReadSample(that,&that->Ins[samplemap[j]], flags[j],
		    (char *)(lpStream + dwMemPos), dwMemLength - dwMemPos);
	    }
	    dwMemPos += samplesize[j];
	    if (dwMemPos >= dwMemLength)
		break;
	}
    }
    // Read song comments: "TEXT"
    if ((dwMemPos + 8 < dwMemLength)
	&& (bswapLE32(*((uint32_t *) (lpStream + dwMemPos))) == 0x74786574)) {
	unsigned len = *((uint32_t *) (lpStream + dwMemPos + 4));

	dwMemPos += 8;
	if ((dwMemPos + len <= dwMemLength) && (len < 16384)) {
	    that->m_lpszSongComments = malloc(len + 1);

	    if (that->m_lpszSongComments) {
		memcpy(that->m_lpszSongComments, lpStream + dwMemPos, len);
		that->m_lpszSongComments[len] = 0;
	    }
	    dwMemPos += len;
	}
    }
    // Read midi config: "MIDI"
    if ((dwMemPos + 8 < dwMemLength)
	&& (bswapLE32(*((uint32_t *) (lpStream + dwMemPos))) == 0x4944494D)) {
	unsigned len = *((uint32_t *) (lpStream + dwMemPos + 4));

	dwMemPos += 8;
	if (len == sizeof(MODMIDICFG)) {
	    memcpy(&that->m_MidiCfg, lpStream + dwMemPos, len);
	    that->m_dwSongFlags |= SONG_EMBEDMIDICFG;
	}
    }
    // Read pattern names: "PNAM"
    if ((dwMemPos + 8 < dwMemLength)
	&& (bswapLE32(*((uint32_t *) (lpStream + dwMemPos))) == 0x4d414e50)) {
	unsigned len = *((uint32_t *) (lpStream + dwMemPos + 4));

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
    // Read channel names: "CNAM"
    if ((dwMemPos + 8 < dwMemLength)
	&& (bswapLE32(*((uint32_t *) (lpStream + dwMemPos))) == 0x4d414e43)) {
	unsigned len = *((uint32_t *) (lpStream + dwMemPos + 4));

	dwMemPos += 8;
	if ((dwMemPos + len <= dwMemLength)
	    && (len <= MAX_BASECHANNELS * MAX_CHANNELNAME)) {
	    unsigned n = len / MAX_CHANNELNAME;

	    for (j = 0; j < n; j++) {
		memcpy(that->ChnSettings[j].szName,
		    (lpStream + dwMemPos + j * MAX_CHANNELNAME),
		    MAX_CHANNELNAME);
		that->ChnSettings[j].szName[MAX_CHANNELNAME - 1] = 0;
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
    return 1;
}

#ifndef MODPLUG_NO_FILESAVE

int CSoundFile_SaveXM(CSoundFile * that, const char *lpszFileName,
    unsigned nPacking)
//---------------------------------------------------------
{
    uint8_t s[64 * 64 * 5];
    XMFILEHEADER header;
    XMINSTRUMENTHEADER xmih;
    XMSAMPLEHEADER xmsh;
    XMSAMPLESTRUCT xmss;
    uint8_t smptable[32];
    uint8_t xmph[9];
    FILE *f;
    int i;
    unsigned j;

    if ((!that->m_nChannels) || (!lpszFileName))
	return 0;
    if ((f = fopen(lpszFileName, "wb")) == NULL)
	return 0;
    fwrite("Extended Module: ", 17, 1, f);
    fwrite(that->m_szNames[0], 20, 1, f);
    s[0] = 0x1A;
    strcpy((char *)&s[1],
	(nPacking) ? "MOD Plugin packed   " : "FastTracker v2.00   ");
    s[21] = 0x04;
    s[22] = 0x01;
    fwrite(s, 23, 1, f);
    // Writing song header
    memset(&header, 0, sizeof(header));
    header.size = sizeof(XMFILEHEADER);
    header.norder = 0;
    header.restartpos = that->m_nRestartPos;
    header.channels = that->m_nChannels;
    header.patterns = 0;
    for (i = 0; i < MAX_ORDERS; i++) {
	if (that->Order[i] == 0xFF)
	    break;
	header.norder++;
	if ((that->Order[i] >= header.patterns)
	    && (that->Order[i] < MAX_PATTERNS))
	    header.patterns = that->Order[i] + 1;
    }
    header.instruments = that->m_nInstruments;
    if (!header.instruments)
	header.instruments = that->m_nSamples;
    header.flags = (that->m_dwSongFlags & SONG_LINEARSLIDES) ? 0x01 : 0x00;
    if (that->m_dwSongFlags & SONG_EXFILTERRANGE)
	header.flags |= 0x1000;
    header.tempo = that->m_nDefaultTempo;
    header.speed = that->m_nDefaultSpeed;
    memcpy(header.order, that->Order, header.norder);
    fwrite(&header, 1, sizeof(header), f);
    // Writing patterns
    for (i = 0; i < header.patterns; i++)
	if (that->Patterns[i]) {
	    MODCOMMAND *p = that->Patterns[i];
	    unsigned len = 0;

	    memset(&xmph, 0, sizeof(xmph));
	    xmph[0] = 9;
	    xmph[5] = (uint8_t) (that->PatternSize[i] & 0xFF);
	    xmph[6] = (uint8_t) (that->PatternSize[i] >> 8);
	    for (j = that->m_nChannels * that->PatternSize[i]; j;
		j--, p++) {
		unsigned note = p->note;
		unsigned param = CSoundFile_ModSaveCommand(that, p, 1);
		unsigned command = param >> 8;
		unsigned vol;

		param &= 0xFF;
		if (note >= 0xFE)
		    note = 97;
		else if ((note <= 12) || (note > 96 + 12))
		    note = 0;
		else
		    note -= 12;
		vol = 0;

		if (p->volcmd) {
		    unsigned volcmd = p->volcmd;

		    switch (volcmd) {
			case VOLCMD_VOLUME:
			    vol = 0x10 + p->vol;
			    break;
			case VOLCMD_VOLSLIDEDOWN:
			    vol = 0x60 + (p->vol & 0x0F);
			    break;
			case VOLCMD_VOLSLIDEUP:
			    vol = 0x70 + (p->vol & 0x0F);
			    break;
			case VOLCMD_FINEVOLDOWN:
			    vol = 0x80 + (p->vol & 0x0F);
			    break;
			case VOLCMD_FINEVOLUP:
			    vol = 0x90 + (p->vol & 0x0F);
			    break;
			case VOLCMD_VIBRATOSPEED:
			    vol = 0xA0 + (p->vol & 0x0F);
			    break;
			case VOLCMD_VIBRATO:
			    vol = 0xB0 + (p->vol & 0x0F);
			    break;
			case VOLCMD_PANNING:
			    vol = 0xC0 + (p->vol >> 2);
			    if (vol > 0xCF)
				vol = 0xCF;
			    break;
			case VOLCMD_PANSLIDELEFT:
			    vol = 0xD0 + (p->vol & 0x0F);
			    break;
			case VOLCMD_PANSLIDERIGHT:
			    vol = 0xE0 + (p->vol & 0x0F);
			    break;
			case VOLCMD_TONEPORTAMENTO:
			    vol = 0xF0 + (p->vol & 0x0F);
			    break;
		    }
		}
		if ((note) && (p->instr) && (vol > 0x0F) && (command)
		    && (param)) {
		    s[len++] = note;
		    s[len++] = p->instr;
		    s[len++] = vol;
		    s[len++] = command;
		    s[len++] = param;
		} else {
		    uint8_t b = 0x80;

		    if (note)
			b |= 0x01;
		    if (p->instr)
			b |= 0x02;
		    if (vol >= 0x10)
			b |= 0x04;
		    if (command)
			b |= 0x08;
		    if (param)
			b |= 0x10;
		    s[len++] = b;
		    if (b & 1)
			s[len++] = note;
		    if (b & 2)
			s[len++] = p->instr;
		    if (b & 4)
			s[len++] = vol;
		    if (b & 8)
			s[len++] = command;
		    if (b & 16)
			s[len++] = param;
		}
		if (len > sizeof(s) - 5)
		    break;
	    }
	    xmph[7] = (uint8_t) (len & 0xFF);
	    xmph[8] = (uint8_t) (len >> 8);
	    fwrite(xmph, 1, 9, f);
	    fwrite(s, 1, len, f);
	} else {
	    memset(&xmph, 0, sizeof(xmph));
	    xmph[0] = 9;
	    xmph[5] = (uint8_t) (that->PatternSize[i] & 0xFF);
	    xmph[6] = (uint8_t) (that->PatternSize[i] >> 8);
	    fwrite(xmph, 1, 9, f);
	}
    // Writing instruments
    for (i = 1; i <= header.instruments; i++) {
	MODINSTRUMENT *pins;
	uint8_t flags[32];

	memset(&xmih, 0, sizeof(xmih));
	memset(&xmsh, 0, sizeof(xmsh));
	xmih.size = sizeof(xmih) + sizeof(xmsh);
	memcpy(xmih.Name, that->m_szNames[i], 22);
	xmih.type = 0;
	xmih.samples = 0;
	if (that->m_nInstruments) {
	    INSTRUMENTHEADER *penv = that->Headers[i];

	    if (penv) {
		unsigned ienv;

		memcpy(xmih.Name, penv->Name, 22);
		xmih.type = penv->nMidiProgram;
		xmsh.volfade = penv->nFadeOut;
		xmsh.vnum = (uint8_t) penv->nVolEnv;
		xmsh.pnum = (uint8_t) penv->nPanEnv;
		if (xmsh.vnum > 12) {
		    xmsh.vnum = 12;
		}
		if (xmsh.pnum > 12) {
		    xmsh.pnum = 12;
		}
		for (ienv = 0; ienv < 12; ienv++) {
		    xmsh.venv[ienv * 2] = penv->VolPoints[ienv];
		    xmsh.venv[ienv * 2 + 1] = penv->VolEnv[ienv];
		    xmsh.penv[ienv * 2] = penv->PanPoints[ienv];
		    xmsh.penv[ienv * 2 + 1] = penv->PanEnv[ienv];
		}
		if (penv->dwFlags & ENV_VOLUME)
		    xmsh.vtype |= 1;
		if (penv->dwFlags & ENV_VOLSUSTAIN)
		    xmsh.vtype |= 2;
		if (penv->dwFlags & ENV_VOLLOOP)
		    xmsh.vtype |= 4;
		if (penv->dwFlags & ENV_PANNING)
		    xmsh.ptype |= 1;
		if (penv->dwFlags & ENV_PANSUSTAIN)
		    xmsh.ptype |= 2;
		if (penv->dwFlags & ENV_PANLOOP)
		    xmsh.ptype |= 4;
		xmsh.vsustain = (uint8_t) penv->nVolSustainBegin;
		xmsh.vloops = (uint8_t) penv->nVolLoopStart;
		xmsh.vloope = (uint8_t) penv->nVolLoopEnd;
		xmsh.psustain = (uint8_t) penv->nPanSustainBegin;
		xmsh.ploops = (uint8_t) penv->nPanLoopStart;
		xmsh.ploope = (uint8_t) penv->nPanLoopEnd;
		for (j = 0; j < 96; j++)
		    if (penv->Keyboard[j + 12]) {
			unsigned k;

			for (k = 0; k < xmih.samples; k++)
			    if (smptable[k] == penv->Keyboard[j + 12])
				break;
			if (k == xmih.samples) {
			    smptable[xmih.samples++] = penv->Keyboard[j + 12];
			}
			if (xmih.samples >= 32)
			    break;
			xmsh.snum[j] = k;
		    }
//              xmsh.reserved2 = xmih.samples;
	    }
	} else {
	    xmih.samples = 1;
//          xmsh.reserved2 = 1;
	    smptable[0] = i;
	}
	xmsh.shsize = (xmih.samples) ? 40 : 0;
	fwrite(&xmih, 1, sizeof(xmih), f);
	if (smptable[0]) {
	    MODINSTRUMENT *pvib = &that->Ins[smptable[0]];

	    xmsh.vibtype = pvib->nVibType;
	    xmsh.vibsweep = pvib->nVibSweep;
	    xmsh.vibdepth = pvib->nVibDepth;
	    xmsh.vibrate = pvib->nVibRate;
	}
	fwrite(&xmsh, 1, xmih.size - sizeof(xmih), f);
	if (!xmih.samples)
	    continue;
	for (j = 0; j < xmih.samples; j++) {
	    memset(&xmss, 0, sizeof(xmss));
	    if (smptable[j])
		memcpy(xmss.Name, that->m_szNames[smptable[j]], 22);
	    pins = &that->Ins[smptable[j]];
	    xmss.samplen = pins->nLength;
	    xmss.loopstart = pins->nLoopStart;
	    xmss.looplen = pins->nLoopEnd - pins->nLoopStart;
	    xmss.vol = pins->nVolume / 4;
	    xmss.finetune = (char)pins->nFineTune;
	    xmss.type = 0;
	    if (pins->uFlags & CHN_LOOP)
		xmss.type = (pins->uFlags & CHN_PINGPONGLOOP) ? 2 : 1;
	    flags[j] = RS_PCM8D;
#ifndef MODPLUG_NO_PACKING
	    if (nPacking) {
		if ((!(pins->uFlags & (CHN_16BIT | CHN_STEREO)))
		    && (CSoundFile_CanPackSample(that,(char *)pins->pSample,
			    pins->nLength, nPacking, NULL))) {
		    flags[j] = RS_ADPCM4;
		    xmss.res = 0xAD;
		}
	    } else
#endif
	    {
		if (pins->uFlags & CHN_16BIT) {
		    flags[j] = RS_PCM16D;
		    xmss.type |= 0x10;
		    xmss.looplen *= 2;
		    xmss.loopstart *= 2;
		    xmss.samplen *= 2;
		}
		if (pins->uFlags & CHN_STEREO) {
		    flags[j] =
			(pins->uFlags & CHN_16BIT) ? RS_STPCM16D : RS_STPCM8D;
		    xmss.type |= 0x20;
		    xmss.looplen *= 2;
		    xmss.loopstart *= 2;
		    xmss.samplen *= 2;
		}
	    }
	    xmss.pan = 255;
	    if (pins->nPan < 256)
		xmss.pan = (uint8_t) pins->nPan;
	    xmss.relnote = (int8_t) pins->RelativeTone;
	    fwrite(&xmss, 1, xmsh.shsize, f);
	}
	for (j = 0; j < xmih.samples; j++) {
	    pins = &that->Ins[smptable[j]];
	    if (pins->pSample) {
#ifndef MODPLUG_NO_PACKING
		if ((flags[j] == RS_ADPCM4) && (xmih.samples > 1)) {
		    CSoundFile_CanPackSample(that,(char *)pins->pSample,
			pins->nLength, nPacking, NULL);
		}
#endif // MODPLUG_NO_PACKING
		CSoundFile_WriteSample(that, f, pins, flags[j], 0);
	    }
	}
    }
    // Writing song comments
    if ((that->m_lpszSongComments) && (that->m_lpszSongComments[0])) {
	uint32_t d = 0x74786574;

	fwrite(&d, 1, 4, f);
	d = strlen(that->m_lpszSongComments);
	fwrite(&d, 1, 4, f);
	fwrite(that->m_lpszSongComments, 1, d, f);
    }
    // Writing midi cfg
    if (that->m_dwSongFlags & SONG_EMBEDMIDICFG) {
	uint32_t d = 0x4944494D;

	fwrite(&d, 1, 4, f);
	d = sizeof(MODMIDICFG);
	fwrite(&d, 1, 4, f);
	fwrite(&that->m_MidiCfg, 1, sizeof(MODMIDICFG), f);
    }
    // Writing Pattern Names
    if ((that->m_nPatternNames) && (that->m_lpszPatternNames)) {
	uint32_t dwLen = that->m_nPatternNames * MAX_PATTERNNAME;

	while ((dwLen >= MAX_PATTERNNAME)
	    && (!that->m_lpszPatternNames[dwLen - MAX_PATTERNNAME]))
	    dwLen -= MAX_PATTERNNAME;
	if (dwLen >= MAX_PATTERNNAME) {
	    uint32_t d = 0x4d414e50;

	    fwrite(&d, 1, 4, f);
	    fwrite(&dwLen, 1, 4, f);
	    fwrite(that->m_lpszPatternNames, 1, dwLen, f);
	}
    }
    // Writing Channel Names
    {
	unsigned nChnNames = 0;

	for (i = 0; i < that->m_nChannels; i++) {
	    if (that->ChnSettings[i].szName[0])
		nChnNames = i + 1;
	}
	// Do it!
	if (nChnNames) {
	    uint32_t dwLen = nChnNames * MAX_CHANNELNAME;
	    uint32_t d = 0x4d414e43;

	    fwrite(&d, 1, 4, f);
	    fwrite(&dwLen, 1, 4, f);
	    for (i = 0; i < nChnNames; i++) {
		fwrite(that->ChnSettings[i].szName, 1, MAX_CHANNELNAME, f);
	    }
	}
    }
    // Save mix plugins information
    CSoundFile_SaveMixPlugins(that, f,1);
    fclose(f);
    return 1;
}

#endif // MODPLUG_NO_FILESAVE
