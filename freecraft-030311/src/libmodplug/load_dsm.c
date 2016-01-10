/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: load_dsm.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

//////////////////////////////////////////////
// DSIK Internal Format (DSM) module loader //
//////////////////////////////////////////////

#include "stdafx.h"
#include "sndfile.h"

#pragma pack(1)

#define DSMID_RIFF  0x46464952		// "RIFF"
#define DSMID_DSMF  0x464d5344		// "DSMF"
#define DSMID_SONG  0x474e4f53		// "SONG"
#define DSMID_INST  0x54534e49		// "INST"
#define DSMID_PATT  0x54544150		// "PATT"

typedef struct DSMNOTE
{
    uint8_t note, ins, vol, cmd, inf;
} DSMNOTE;

typedef struct DSMINST
{
    uint32_t id_INST;
    uint32_t inst_len;
    int8_t filename[13];
    uint8_t flags;
    uint8_t flags2;
    uint8_t volume;
    uint32_t length;
    uint32_t loopstart;
    uint32_t loopend;
    uint32_t reserved1;
    uint16_t c2spd;
    uint16_t reserved2;
    int8_t samplename[28];
} DSMINST;

typedef struct DSMFILEHEADER
{
    uint32_t id_RIFF;			// "RIFF"
    uint32_t riff_len;
    uint32_t id_DSMF;			// "DSMF"
    uint32_t id_SONG;			// "SONG"
    uint32_t song_len;
} DSMFILEHEADER;

typedef struct DSMSONG
{
    int8_t songname[28];
    uint16_t reserved1;
    uint16_t flags;
    uint32_t reserved2;
    uint16_t numord;
    uint16_t numsmp;
    uint16_t numpat;
    uint16_t numtrk;
    uint8_t globalvol;
    uint8_t mastervol;
    uint8_t speed;
    uint8_t bpm;
    uint8_t panpos[16];
    uint8_t orders[128];
} DSMSONG;

typedef struct DSMPATT
{
    uint32_t id_PATT;
    uint32_t patt_len;
    uint8_t dummy1;
    uint8_t dummy2;
} DSMPATT;

#pragma pack()

int CSoundFile_ReadDSM(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//-----------------------------------------------------------
{
    DSMFILEHEADER *pfh = (DSMFILEHEADER *) lpStream;
    DSMSONG *psong;
    uint32_t dwMemPos;
    unsigned i;
    unsigned nPat, nSmp;

    if ((!lpStream) || (dwMemLength < 1024) || (pfh->id_RIFF != DSMID_RIFF)
	|| (pfh->riff_len + 8 > dwMemLength) || (pfh->riff_len < 1024)
	|| (pfh->id_DSMF != DSMID_DSMF) || (pfh->id_SONG != DSMID_SONG)
	|| (pfh->song_len > dwMemLength)) {
	return 0;
    }
    psong = (DSMSONG *) (lpStream + sizeof(DSMFILEHEADER));
    dwMemPos = sizeof(DSMFILEHEADER) + pfh->song_len;
    that->m_nType = MOD_TYPE_DSM;
    that->m_nChannels = psong->numtrk;
    if (that->m_nChannels < 4) {
	that->m_nChannels = 4;
    }
    if (that->m_nChannels > 16) {
	that->m_nChannels = 16;
    }
    that->m_nSamples = psong->numsmp;
    if (that->m_nSamples > MAX_SAMPLES) {
	that->m_nSamples = MAX_SAMPLES;
    }
    that->m_nDefaultSpeed = psong->speed;
    that->m_nDefaultTempo = psong->bpm;
    that->m_nDefaultGlobalVolume = psong->globalvol << 2;
    if ((!that->m_nDefaultGlobalVolume)
	|| (that->m_nDefaultGlobalVolume > 256)) {
	that->m_nDefaultGlobalVolume = 256;
    }
    that->m_nSongPreAmp = psong->mastervol & 0x7F;
    for (i = 0; i < MAX_ORDERS; i++) {
	that->Order[i] =
	    (uint8_t) ((i < psong->numord) ? psong->orders[i] : 0xFF);
    }
    for (i = 0; i < 16; i++) {
	that->ChnSettings[i].nPan = 0x80;
	if (psong->panpos[i] <= 0x80) {
	    that->ChnSettings[i].nPan = psong->panpos[i] << 1;
	}
    }
    memcpy(that->m_szNames[0], psong->songname, 28);
    nPat = 0;
    nSmp = 1;
    while (dwMemPos < dwMemLength - 8) {
	DSMPATT *ppatt = (DSMPATT *) (lpStream + dwMemPos);
	DSMINST *pins = (DSMINST *) (lpStream + dwMemPos);

	// Reading Patterns
	if (ppatt->id_PATT == DSMID_PATT) {
	    uint32_t dwPos;
	    MODCOMMAND *m;
	    unsigned row;

	    dwMemPos += 8;
	    if (dwMemPos + ppatt->patt_len >= dwMemLength) {
		break;
	    }
	    dwPos = dwMemPos;
	    dwMemPos += ppatt->patt_len;

	    m = CSoundFile_AllocatePattern(64, that->m_nChannels);
	    if (!m) {
		break;
	    }
	    that->PatternSize[nPat] = 64;
	    that->Patterns[nPat] = m;
	    row = 0;

	    while ((row < 64) && (dwPos + 2 <= dwMemPos)) {
		unsigned flag = lpStream[dwPos++];

		if (flag) {
		    unsigned ch = (flag & 0x0F) % that->m_nChannels;

		    if (flag & 0x80) {
			unsigned note = lpStream[dwPos++];

			if (note) {
			    if (note <= 12 * 9)
				note += 12;
			    m[ch].note = (uint8_t) note;
			}
		    }
		    if (flag & 0x40) {
			m[ch].instr = lpStream[dwPos++];
		    }
		    if (flag & 0x20) {
			m[ch].volcmd = VOLCMD_VOLUME;
			m[ch].vol = lpStream[dwPos++];
		    }
		    if (flag & 0x10) {
			unsigned command = lpStream[dwPos++];
			unsigned param = lpStream[dwPos++];

			switch (command) {
				// 4-bit Panning
			    case 0x08:
				switch (param & 0xF0) {
				    case 0x00:
					param <<= 4;
					break;
				    case 0x10:
					command = 0x0A;
					param = (param & 0x0F) << 4;
					break;
				    case 0x20:
					command = 0x0E;
					param = (param & 0x0F) | 0xA0;
					break;
				    case 0x30:
					command = 0x0E;
					param = (param & 0x0F) | 0x10;
					break;
				    case 0x40:
					command = 0x0E;
					param = (param & 0x0F) | 0x20;
					break;
				    default:
					command = 0;
				}
				break;
				// Portamentos
			    case 0x11:
			    case 0x12:
				command &= 0x0F;
				break;
				// 3D Sound (?)
			    case 0x13:
				command = 'X' - 55;
				param = 0x91;
				break;
			    default:
				// Volume + Offset (?)
				command =
				    ((command & 0xF0) == 0x20) ? 0x09 : 0;
			}
			m[ch].command = (uint8_t) command;
			m[ch].param = (uint8_t) param;
			if (command)
			    CSoundFile_ConvertModCommand(that, &m[ch]);
		    }
		} else {
		    m += that->m_nChannels;
		    row++;
		}
	    }
	    nPat++;
	    // Reading Samples
	} else if ((nSmp <= that->m_nSamples)
		&& (pins->id_INST == DSMID_INST)) {
	    uint32_t dwPos;
	    MODINSTRUMENT *psmp;
	    unsigned smptype;

	    if (dwMemPos + pins->inst_len >= dwMemLength - 8) {
		break;
	    }
	    dwPos = dwMemPos + sizeof(DSMINST);

	    dwMemPos += 8 + pins->inst_len;
	    memcpy(that->m_szNames[nSmp], pins->samplename, 28);
	    psmp = &that->Ins[nSmp];

	    memcpy(psmp->Name, pins->filename, 13);
	    psmp->nGlobalVol = 64;
	    psmp->nC4Speed = pins->c2spd;
	    psmp->uFlags = (uint16_t) ((pins->flags & 1) ? CHN_LOOP : 0);
	    psmp->nLength = pins->length;
	    psmp->nLoopStart = pins->loopstart;
	    psmp->nLoopEnd = pins->loopend;
	    psmp->nVolume = (uint16_t) (pins->volume << 2);
	    if (psmp->nVolume > 256) {
		psmp->nVolume = 256;
	    }
	    smptype = (pins->flags & 2) ? RS_PCM8S : RS_PCM8U;

	    CSoundFile_ReadSample(that,psmp, smptype,
		(const char *)(lpStream + dwPos), dwMemLength - dwPos);
	    nSmp++;
	} else {
	    break;
	}
    }
    return 1;
}
