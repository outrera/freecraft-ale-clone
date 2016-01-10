/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>,
 *	    Adam Goode	     <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: load_mod.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

//#pragma warning(disable:4244)

//////////////////////////////////////////////////////////
// ProTracker / NoiseTracker MOD/NST file support

void CSoundFile_ConvertModCommand(CSoundFile const *const that,
    MODCOMMAND * m)
{
    unsigned command = m->command;
    unsigned param = m->param;

    switch (command) {
	case 0x00:
	    if (param) {
		command = CMD_ARPEGGIO;
	    }
	    break;
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
	    command = CMD_TONEPORTAVOL;
	    if (param & 0xF0) {
		param &= 0xF0;
	    }
	    break;
	case 0x06:
	    command = CMD_VIBRATOVOL;
	    if (param & 0xF0) {
		param &= 0xF0;
	    }
	    break;
	case 0x07:
	    command = CMD_TREMOLO;
	    break;
	case 0x08:
	    command = CMD_PANNING8;
	    break;
	case 0x09:
	    command = CMD_OFFSET;
	    break;
	case 0x0A:
	    command = CMD_VOLUMESLIDE;
	    if (param & 0xF0) {
		param &= 0xF0;
	    }
	    break;
	case 0x0B:
	    command = CMD_POSITIONJUMP;
	    break;
	case 0x0C:
	    command = CMD_VOLUME;
	    break;
	case 0x0D:
	    command = CMD_PATTERNBREAK;
	    param = ((param >> 4) * 10) + (param & 0x0F);
	    break;
	case 0x0E:
	    command = CMD_MODCMDEX;
	    break;
	case 0x0F:
	    command =
		(param <=
		(unsigned)((that->
			m_nType & (MOD_TYPE_XM | MOD_TYPE_MT2)) ? 0x1F : 0x20))
		? CMD_SPEED : CMD_TEMPO;
	    if ((param == 0xFF) && (that->m_nSamples == 15)) {
		command = 0;
	    }
	    break;
	    // Extension for XM extended effects
	case 'G' - 55:
	    command = CMD_GLOBALVOLUME;
	    break;
	case 'H' - 55:
	    command = CMD_GLOBALVOLSLIDE;
	    if (param & 0xF0) {
		param &= 0xF0;
	    }
	    break;
	case 'K' - 55:
	    command = CMD_KEYOFF;
	    break;
	case 'L' - 55:
	    command = CMD_SETENVPOSITION;
	    break;
	case 'M' - 55:
	    command = CMD_CHANNELVOLUME;
	    break;
	case 'N' - 55:
	    command = CMD_CHANNELVOLSLIDE;
	    break;
	case 'P' - 55:
	    command = CMD_PANNINGSLIDE;
	    if (param & 0xF0)
		param &= 0xF0;
	    break;
	case 'R' - 55:
	    command = CMD_RETRIG;
	    break;
	case 'T' - 55:
	    command = CMD_TREMOR;
	    break;
	case 'X' - 55:
	    command = CMD_XFINEPORTAUPDOWN;
	    break;
	case 'Y' - 55:
	    command = CMD_PANBRELLO;
	    break;
	case 'Z' - 55:
	    command = CMD_MIDI;
	    break;
	default:
	    command = 0;
    }
    m->command = command;
    m->param = param;
}

#ifndef MODPLUG_NO_FILESAVE

unsigned CSoundFile_ModSaveCommand(CSoundFile const * that, const MODCOMMAND * m,
    int bXM)
{
    unsigned command = m->command & 0x3F;
    unsigned param = m->param;

    switch (command) {
	case 0:
	    command = param = 0;
	    break;
	case CMD_ARPEGGIO:
	    command = 0;
	    break;
	case CMD_PORTAMENTOUP:
	    if (that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT | MOD_TYPE_STM)) {
		if ((param & 0xF0) == 0xE0) {
		    command = 0x0E;
		    param = ((param & 0x0F) >> 2) | 0x10;
		    break;
		} else if ((param & 0xF0) == 0xF0) {
		    command = 0x0E;
		    param &= 0x0F;
		    param |= 0x10;
		    break;
		}
	    }
	    command = 0x01;
	    break;
	case CMD_PORTAMENTODOWN:
	    if (that->m_nType & (MOD_TYPE_S3M | MOD_TYPE_IT | MOD_TYPE_STM)) {
		if ((param & 0xF0) == 0xE0) {
		    command = 0x0E;
		    param = ((param & 0x0F) >> 2) | 0x20;
		    break;
		} else if ((param & 0xF0) == 0xF0) {
		    command = 0x0E;
		    param &= 0x0F;
		    param |= 0x20;
		    break;
		}
	    }
	    command = 0x02;
	    break;
	case CMD_TONEPORTAMENTO:
	    command = 0x03;
	    break;
	case CMD_VIBRATO:
	    command = 0x04;
	    break;
	case CMD_TONEPORTAVOL:
	    command = 0x05;
	    break;
	case CMD_VIBRATOVOL:
	    command = 0x06;
	    break;
	case CMD_TREMOLO:
	    command = 0x07;
	    break;
	case CMD_PANNING8:
	    command = 0x08;
	    if (bXM) {
		if ((that->m_nType != MOD_TYPE_IT)
		    && (that->m_nType != MOD_TYPE_XM)
		    && (param <= 0x80)) {
		    param <<= 1;
		    if (param > 255) {
			param = 255;
		    }
		}
	    } else {
		if ((that->m_nType == MOD_TYPE_IT)
		    || (that->m_nType == MOD_TYPE_XM)) {
		    param >>= 1;
		}
	    }
	    break;
	case CMD_OFFSET:
	    command = 0x09;
	    break;
	case CMD_VOLUMESLIDE:
	    command = 0x0A;
	    break;
	case CMD_POSITIONJUMP:
	    command = 0x0B;
	    break;
	case CMD_VOLUME:
	    command = 0x0C;
	    break;
	case CMD_PATTERNBREAK:
	    command = 0x0D;
	    param = ((param / 10) << 4) | (param % 10);
	    break;
	case CMD_MODCMDEX:
	    command = 0x0E;
	    break;
	case CMD_SPEED:
	    command = 0x0F;
	    if (param > 0x20) {
		param = 0x20;
	    }
	    break;
	case CMD_TEMPO:
	    if (param > 0x20) {
		command = 0x0F;
		break;
	    }
	case CMD_GLOBALVOLUME:
	    command = 'G' - 55;
	    break;
	case CMD_GLOBALVOLSLIDE:
	    command = 'H' - 55;
	    break;
	case CMD_KEYOFF:
	    command = 'K' - 55;
	    break;
	case CMD_SETENVPOSITION:
	    command = 'L' - 55;
	    break;
	case CMD_CHANNELVOLUME:
	    command = 'M' - 55;
	    break;
	case CMD_CHANNELVOLSLIDE:
	    command = 'N' - 55;
	    break;
	case CMD_PANNINGSLIDE:
	    command = 'P' - 55;
	    break;
	case CMD_RETRIG:
	    command = 'R' - 55;
	    break;
	case CMD_TREMOR:
	    command = 'T' - 55;
	    break;
	case CMD_XFINEPORTAUPDOWN:
	    command = 'X' - 55;
	    break;
	case CMD_PANBRELLO:
	    command = 'Y' - 55;
	    break;
	case CMD_MIDI:
	    command = 'Z' - 55;
	    break;
	case CMD_S3MCMDEX:
	    switch (param & 0xF0) {
		case 0x10:
		    command = 0x0E;
		    param = (param & 0x0F) | 0x30;
		    break;
		case 0x20:
		    command = 0x0E;
		    param = (param & 0x0F) | 0x50;
		    break;
		case 0x30:
		    command = 0x0E;
		    param = (param & 0x0F) | 0x40;
		    break;
		case 0x40:
		    command = 0x0E;
		    param = (param & 0x0F) | 0x70;
		    break;
		case 0x90:
		    command = 'X' - 55;
		    break;
		case 0xB0:
		    command = 0x0E;
		    param = (param & 0x0F) | 0x60;
		    break;
		case 0xA0:
		case 0x50:
		case 0x70:
		case 0x60:
		    command = param = 0;
		    break;
		default:
		    command = 0x0E;
		    break;
	    }
	    break;
	default:
	    command = param = 0;
    }
    return ((command << 8) | (param)) & 0xFFFF;
}

#endif // MODPLUG_NO_FILESAVE

#pragma pack(1)

typedef struct _MODSAMPLE
{
    int8_t name[22];
    uint16_t length;
    uint8_t finetune;
    uint8_t volume;
    uint16_t loopstart;
    uint16_t looplen;
} MODSAMPLE;

typedef struct _MODMAGIC
{
    uint8_t nOrders;
    uint8_t nRestartPos;
    uint8_t Orders[128];
    char Magic[4];			// changed from CHAR
} MODMAGIC;

#pragma pack()

static inline int IsMagic(const char *s1, const char *s2)
{
    return ((*(uint32_t *) s1) == (*(uint32_t *) s2)) ? 1 : 0;
}

int CSoundFile_ReadMod(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//---------------------------------------------------------------
{
    char s[1024];			// changed from CHAR
    uint32_t dwMemPos;
    uint32_t dwErrCheck;
    uint32_t dwTotalSampleLen;
    MODMAGIC *pMagic;
    unsigned nErr;
    unsigned i;
    unsigned nbp;
    unsigned nbpbuggy;
    unsigned nbpbuggy2;
    unsigned norders;

    if ((!lpStream) || (dwMemLength < 0x600)) {
	return 0;
    }
    dwMemPos = 20;
    that->m_nSamples = 31;
    that->m_nChannels = 4;
    pMagic = (MODMAGIC *) (lpStream + dwMemPos + sizeof(MODSAMPLE) * 31);
    // Check Mod Magic
    memcpy(s, pMagic->Magic, 4);
    if ((IsMagic(s, "M.K.")) || (IsMagic(s, "M!K!"))
	|| (IsMagic(s, "M&K!")) || (IsMagic(s, "N.T."))) {
	that->m_nChannels = 4;
    } else if ((IsMagic(s, "CD81")) || (IsMagic(s, "OKTA"))) {
	that->m_nChannels = 8;
    } else if ((s[0] == 'F') && (s[1] == 'L') && (s[2] == 'T') && (s[3] >= '4')
	&& (s[3] <= '9'))
	that->m_nChannels = s[3] - '0';
    else if ((s[0] >= '4') && (s[0] <= '9') && (s[1] == 'C') && (s[2] == 'H')
	&& (s[3] == 'N'))
	that->m_nChannels = s[0] - '0';
    else if ((s[0] == '1') && (s[1] >= '0') && (s[1] <= '9') && (s[2] == 'C')
	&& (s[3] == 'H'))
	that->m_nChannels = s[1] - '0' + 10;
    else if ((s[0] == '2') && (s[1] >= '0') && (s[1] <= '9') && (s[2] == 'C')
	&& (s[3] == 'H'))
	that->m_nChannels = s[1] - '0' + 20;
    else if ((s[0] == '3') && (s[1] >= '0') && (s[1] <= '2') && (s[2] == 'C')
	&& (s[3] == 'H'))
	that->m_nChannels = s[1] - '0' + 30;
    else if ((s[0] == 'T') && (s[1] == 'D') && (s[2] == 'Z') && (s[3] >= '4')
	&& (s[3] <= '9'))
	that->m_nChannels = s[3] - '0';
    else if (IsMagic(s, "16CN"))
	that->m_nChannels = 16;
    else if (IsMagic(s, "32CN"))
	that->m_nChannels = 32;
    else {
	that->m_nSamples = 15;
    }

    // Load Samples
    nErr = dwTotalSampleLen = 0;
    for (i = 1; i <= that->m_nSamples; i++) {
	MODSAMPLE *pms = (MODSAMPLE *) (lpStream + dwMemPos);
	MODINSTRUMENT *psmp = &that->Ins[i];
	unsigned loopstart, looplen;

	memcpy(that->m_szNames[i], pms->name, 22);
	that->m_szNames[i][22] = 0;
	psmp->uFlags = 0;
	psmp->nLength = bswapBE16(pms->length) * 2;
	dwTotalSampleLen += psmp->nLength;
	psmp->nFineTune = MOD2XMFineTune(pms->finetune & 0x0F);
	psmp->nVolume = 4 * pms->volume;
	if (psmp->nVolume > 256) {
	    psmp->nVolume = 256;
	    nErr++;
	}
	psmp->nGlobalVol = 64;
	psmp->nPan = 128;
	loopstart = bswapBE16(pms->loopstart) * 2;
	looplen = bswapBE16(pms->looplen) * 2;
	// Fix loops
	if ((looplen > 2) && (loopstart + looplen > psmp->nLength)
	    && (loopstart / 2 + looplen <= psmp->nLength)) {
	    loopstart /= 2;
	}
	psmp->nLoopStart = loopstart;
	psmp->nLoopEnd = loopstart + looplen;
	if (psmp->nLength < 4)
	    psmp->nLength = 0;
	if (psmp->nLength) {
	    unsigned derr = 0;

	    if (psmp->nLoopStart >= psmp->nLength) {
		psmp->nLoopStart = psmp->nLength - 1;
		derr |= 1;
	    }
	    if (psmp->nLoopEnd > psmp->nLength) {
		psmp->nLoopEnd = psmp->nLength;
		derr |= 1;
	    }
	    if (psmp->nLoopStart > psmp->nLoopEnd)
		derr |= 1;
	    if ((psmp->nLoopStart > psmp->nLoopEnd) || (psmp->nLoopEnd <= 8)
		|| (psmp->nLoopEnd - psmp->nLoopStart <= 4)) {
		psmp->nLoopStart = 0;
		psmp->nLoopEnd = 0;
	    }
	    if (psmp->nLoopEnd > psmp->nLoopStart) {
		psmp->uFlags |= CHN_LOOP;
	    }
	}
	dwMemPos += sizeof(MODSAMPLE);
    }
    if ((that->m_nSamples == 15) && (dwTotalSampleLen > dwMemLength * 4)) {
	return 0;
    }
    pMagic = (MODMAGIC *) (lpStream + dwMemPos);
    dwMemPos += sizeof(MODMAGIC);
    if (that->m_nSamples == 15) {
	dwMemPos -= 4;
    }
    memset(that->Order, 0, sizeof(that->Order));
    memcpy(that->Order, pMagic->Orders, 128);

    norders = pMagic->nOrders;
    if ((!norders) || (norders > 0x80)) {
	norders = 0x80;
	while ((norders > 1) && (!that->Order[norders - 1])) {
	    norders--;
	}
    }
    nbpbuggy = 0;
    nbpbuggy2 = 0;
    nbp = 0;
    for (i = 0; i < 128; i++) {
	unsigned t = that->Order[i];

	if ((t < 0x80) && (nbp <= t)) {
	    nbp = t + 1;
	    if (i < norders) {
		nbpbuggy = nbp;
	    }
	}
	if (t >= nbpbuggy2) {
	    nbpbuggy2 = t + 1;
	}
    }
    for (i = norders; i < MAX_ORDERS; i++) {
	that->Order[i] = 0xFF;
    }
    norders--;
    that->m_nRestartPos = pMagic->nRestartPos;
    if (that->m_nRestartPos >= 0x78)
	that->m_nRestartPos = 0;
    if (that->m_nRestartPos + 1 >= (unsigned)norders)
	that->m_nRestartPos = 0;
    if (!nbp) {
	return 0;
    }
    i = dwTotalSampleLen + dwMemPos;

    if ((IsMagic(pMagic->Magic, "M.K."))
	&& (i + nbp * 8 * 256 == dwMemLength))
	that->m_nChannels = 8;
    if ((nbp != nbpbuggy)
	&& (i + nbp * that->m_nChannels * 256 != dwMemLength)) {
	if (i + nbpbuggy * that->m_nChannels * 256 == dwMemLength)
	    nbp = nbpbuggy;
	else
	    nErr += 8;
    } else if ((nbpbuggy2 > nbp)
	&& (i + nbpbuggy2 * that->m_nChannels * 256 == dwMemLength)) {
	nbp = nbpbuggy2;
    }
    if ((i < 0x600) || (i > dwMemLength))
	nErr += 8;
    if ((that->m_nSamples == 15) && (nErr >= 16))
	return 0;
    // Default settings
    that->m_nType = MOD_TYPE_MOD;
    that->m_nDefaultSpeed = 6;
    that->m_nDefaultTempo = 125;
    that->m_nMinPeriod = 14 << 2;
    that->m_nMaxPeriod = 3424 << 2;
    memcpy(that->m_szNames, lpStream, 20);
    // Setting channels pan
    for (i = 0; i < that->m_nChannels; i++) {
	that->ChnSettings[i].nVolume = 64;
	if (CSoundFile_gdwSoundSetup & SNDMIX_MAXDEFAULTPAN) {
	    that->ChnSettings[i].nPan = (((i & 3) == 1)
		|| ((i & 3) == 2)) ? 256 : 0;
	} else {
	    that->ChnSettings[i].nPan = (((i & 3) == 1)
		|| ((i & 3) == 2)) ? 0xC0 : 0x40;
	}
    }
    // Reading channels
    for (i = 0; i < nbp; i++) {
	if (i < MAX_PATTERNS) {
	    MODCOMMAND *m;
	    const uint8_t *p;
	    unsigned j;

	    if ((that->Patterns[i] =
		    CSoundFile_AllocatePattern(64, that->m_nChannels)) == NULL)
		break;
	    that->PatternSize[i] = 64;
	    if (dwMemPos + that->m_nChannels * 256 >= dwMemLength)
		break;
	    m = that->Patterns[i];
	    p = lpStream + dwMemPos;

	    for (j = that->m_nChannels * 64; j; m++, p += 4, j--) {
		uint8_t A0 = p[0], A1 = p[1], A2 = p[2], A3 = p[3];
		unsigned n = ((((unsigned)A0 & 0x0F) << 8) | (A1));

		if ((n) && (n != 0xFFF))
		    m->note = CSoundFile_GetNoteFromPeriod(that, n << 2);
		m->instr = ((unsigned)A2 >> 4) | (A0 & 0x10);
		m->command = A2 & 0x0F;
		m->param = A3;
		if ((m->command) || (m->param)) {
		    CSoundFile_ConvertModCommand(that, m);
		}
	    }
	}
	dwMemPos += that->m_nChannels * 256;
    }
    // Reading instruments
    dwErrCheck = 0;

    for (i = 1; i <= that->m_nSamples; i++) {
	if (that->Ins[i].nLength) {
	    uint32_t dwSize;
	    const char *p;
	    unsigned flags;

	    if (dwMemPos + 5 >= dwMemLength) {
		break;
	    }

	    flags = 0;
	    p = (const char *)(lpStream + dwMemPos);
	    if (!strnicmp(p, "ADPCM", 5)) {
		flags = 3;
		p += 5;
		dwMemPos += 5;
	    }
	    dwSize = CSoundFile_ReadSample(that,&that->Ins[i], flags, p,
		dwMemLength - dwMemPos);

	    if (dwSize) {
		dwMemPos += dwSize;
		dwErrCheck++;
	    }
	}
    }
#ifdef MODPLUG_TRACKER
    return 1;
#else
    return (dwErrCheck) ? 1 : 0;
#endif
}

#ifndef MODPLUG_NO_FILESAVE
#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

int CSoundFile_SaveMod(CSoundFile * that, const char *lpszFileName,
    unsigned nPacking)
//----------------------------------------------------------
{
    uint8_t insmap[32];
    unsigned inslen[32];
    uint8_t bTab[32];
    uint8_t ord[128];
    FILE *f;
    unsigned i;
    unsigned j;
    unsigned nbp;
    unsigned norders;

    if ((!that->m_nChannels) || (!lpszFileName)) {
	return 0;
    }
    if ((f = fopen(lpszFileName, "wb")) == NULL) {
	return 0;
    }

    memset(ord, 0, sizeof(ord));
    memset(inslen, 0, sizeof(inslen));
    if (that->m_nInstruments) {
	memset(insmap, 0, sizeof(insmap));
	for (i = 1; i < 32; i++)
	    if (that->Headers[i]) {
		for (j = 0; j < 128; j++)
		    if (that->Headers[i]->Keyboard[j]) {
			insmap[i] = that->Headers[i]->Keyboard[j];
			break;
		    }
	    }
    } else {
	for (i = 0; i < 32; i++) {
	    insmap[i] = (uint8_t) i;
	}
    }

    // Writing song name
    fwrite(that->m_szNames, 20, 1, f);
    // Writing instrument definition
    for (i = 1; i <= 31; i++) {
	MODINSTRUMENT const *pins = &that->Ins[insmap[i]];

	memcpy(bTab, that->m_szNames[i], 22);
	inslen[i] = pins->nLength;
	if (inslen[i] > 0x1fff0)
	    inslen[i] = 0x1fff0;
	bTab[22] = inslen[i] >> 9;
	bTab[23] = inslen[i] >> 1;
	if (pins->RelativeTone < 0)
	    bTab[24] = 0x08;
	else if (pins->RelativeTone > 0)
	    bTab[24] = 0x07;
	else
	    bTab[24] = (uint8_t) XM2MODFineTune(pins->nFineTune);
	bTab[25] = pins->nVolume >> 2;
	bTab[26] = pins->nLoopStart >> 9;
	bTab[27] = pins->nLoopStart >> 1;
	bTab[28] = (pins->nLoopEnd - pins->nLoopStart) >> 9;
	bTab[29] = (pins->nLoopEnd - pins->nLoopStart) >> 1;
	fwrite(bTab, 30, 1, f);
    }

    // Writing number of patterns
    nbp = 0;
    norders = 128;

    for (i = 0; i < 128; i++) {
	if (that->Order[i] == 0xFF) {
	    norders = i;
	    break;
	}
	if ((that->Order[i] < 0x80) && (nbp <= that->Order[i])) {
	    nbp = that->Order[i] + 1;
	}
    }
    bTab[0] = norders;
    bTab[1] = that->m_nRestartPos;
    fwrite(bTab, 2, 1, f);
    // Writing pattern list
    if (norders) {
	memcpy(ord, that->Order, norders);
    }
    fwrite(ord, 128, 1, f);
    // Writing signature
    if (that->m_nChannels == 4) {
	strcpy((char *)&bTab, "M.K.");
    } else {
	sprintf(&bTab, "%luCHN", (unsigned long)that->m_nChannels);
    }
    fwrite(bTab, 4, 1, f);
    // Writing patterns
    for (j = 0; j < nbp; j++) {
	if (that->Patterns[j]) {
	    uint8_t s[64 * 4];
	    MODCOMMAND *m = that->Patterns[j];

	    for (i = 0; i < 64; i++) {
		if (i < that->PatternSize[j]) {
		    uint8_t *p = s;
		    unsigned c;

		    for (c = 0; c < that->m_nChannels;
			c++, p += 4, m++) {
			unsigned param;
			unsigned command;
			unsigned period;
			unsigned instr;

			param = CSoundFile_ModSaveCommand(that, m, 0);
			command = param >> 8;
			param &= 0xFF;
			if (command > 0x0F) {
			    command = param = 0;
			}
			if (m->vol >= 0x10 && m->vol <= 0x50 && !command
				&& !param) {
			    command = 0x0C;
			    param = m->vol - 0x10;
			}

			period = m->note;
			if (period) {
			    if (period < 37)
				period = 37;
			    period -= 37;
			    if (period >= 6 * 12)
				period = 6 * 12 - 1;
			    period = CSoundFile_ProTrackerPeriodTable[period];
			}
			instr = (m->instr > 31) ? 0 : m->instr;

			p[0] = ((period >> 8) & 0x0F) | (instr & 0x10);
			p[1] = period & 0xFF;
			p[2] = ((instr & 0x0F) << 4) | (command & 0x0F);
			p[3] = param;
		    }
		    fwrite(s, that->m_nChannels, 4, f);
		} else {
		    memset(s, 0, that->m_nChannels * 4);
		    fwrite(s, that->m_nChannels, 4, f);
		}
	    }
	}
    }

    // Writing instruments
    for (i = 1; i <= 31; i++) {
	if (inslen[i]) {
	    MODINSTRUMENT const *pins = &that->Ins[insmap[i]];
	    unsigned flags = RS_PCM8S;

#ifndef MODPLUG_NO_PACKING
	    if (!(pins->uFlags & (CHN_16BIT | CHN_STEREO))) {
		if ((nPacking)
		    && (CSoundFile_CanPackSample(that,(char *)pins->pSample,
			    inslen[i], nPacking, NULL))) {
		    fwrite("ADPCM", 1, 5, f);
		    flags = RS_ADPCM4;
		}
	    }
#endif
	    CSoundFile_WriteSample(that, f, pins, flags, inslen[i]);
	}
    }
    fclose(f);
    return 1;
}

#ifdef _MSC_VER
#pragma warning(default:4100)
#endif

#endif // MODPLUG_NO_FILESAVE
