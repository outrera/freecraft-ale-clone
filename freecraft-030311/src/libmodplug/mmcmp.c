/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 * NOTE this code will break on big endian machines
 *
 *	$Id: mmcmp.c,v 1.7 2003/02/16 03:55:37 mr-russ Exp $
*/

#include "stdafx.h"

#ifdef MODPLUG_MMCMP_SUPPORT	// {

#ifndef __MINGW32__
#ifdef BSD
#include <inttypes.h>
#else
#include <stdint.h>
#endif // BSD
#endif // __MINGW32__

//#define MMCMP_LOG

#ifdef MMCMP_LOG
extern void Log(const char *s, ...);
#endif

//////////////////////////////////////////////////////////////////////////////
//
//      PowerPack PP20 Unpacker
//

typedef struct _PPBITBUFFER
{
    unsigned bitcount;
    uint32_t bitbuffer;
    const uint8_t *pStart;
    const uint8_t *pSrc;
} PPBITBUFFER;

static uint32_t PPBITBUFFER_GetBits(PPBITBUFFER * this, unsigned n)
{
    uint32_t result;
    unsigned i;

    result = 0;
    for (i = 0; i < n; i++) {
	if (!this->bitcount) {
	    this->bitcount = 8;
	    if (this->pSrc != this->pStart) {
		this->pSrc--;
	    }
	    this->bitbuffer = *this->pSrc;
	}
	result = (result << 1) | (this->bitbuffer & 1);
	this->bitbuffer >>= 1;
	this->bitcount--;
    }
    return result;
}

static void PP20_DoUnpack(const uint8_t *pSrc, unsigned nSrcLen, uint8_t *pDst,
    unsigned nDstLen)
{
    PPBITBUFFER BitBuffer;
    uint32_t nBytesLeft;
    unsigned i;

    BitBuffer.pStart = pSrc;
    BitBuffer.pSrc = pSrc + nSrcLen - 4;
    BitBuffer.bitbuffer = 0;
    BitBuffer.bitcount = 0;
    PPBITBUFFER_GetBits(&BitBuffer, pSrc[nSrcLen - 1]);
    nBytesLeft = nDstLen;
    while (nBytesLeft > 0) {
	unsigned n;
	unsigned nbits;
	unsigned nofs;

	if (!PPBITBUFFER_GetBits(&BitBuffer, 1)) {
	    n = 1;

	    while (n < nBytesLeft) {
		unsigned code = PPBITBUFFER_GetBits(&BitBuffer, 2);

		n += code;
		if (code != 3) {
		    break;
		}
	    }
	    for (i = 0; i < n; i++) {
		pDst[--nBytesLeft] =
		    (uint8_t)PPBITBUFFER_GetBits(&BitBuffer, 8);
	    }
	    if (!nBytesLeft) {
		break;
	    }
	}
	n = PPBITBUFFER_GetBits(&BitBuffer, 2) + 1;
	nbits = pSrc[n - 1];

	if (n == 4) {
	    nofs =
		PPBITBUFFER_GetBits(&BitBuffer,
		(PPBITBUFFER_GetBits(&BitBuffer, 1)) ? nbits : 7);
	    while (n < nBytesLeft) {
		unsigned code = PPBITBUFFER_GetBits(&BitBuffer, 3);

		n += code;
		if (code != 7) {
		    break;
		}
	    }
	} else {
	    nofs = PPBITBUFFER_GetBits(&BitBuffer, nbits);
	}
	for (i = 0; i <= n; i++) {
	    pDst[nBytesLeft - 1] =
		(nBytesLeft + nofs < nDstLen) ? pDst[nBytesLeft + nofs] : 0;
	    if (!--nBytesLeft) {
		break;
	    }
	}
    }
}

static int PP20_Unpack(const uint8_t **ppMemFile, uint32_t * pdwMemLength)
{
    uint32_t dwMemLength;
    const uint8_t *lpMemFile;
    uint32_t dwDstLen;
    uint8_t *pBuffer;

    dwMemLength = *pdwMemLength;
    lpMemFile = *ppMemFile;
    if ((!lpMemFile) || (dwMemLength < 256)
	|| (*(uint32_t *) lpMemFile != 0x30325050)) {
	return 0;
    }

    dwDstLen =
	(lpMemFile[dwMemLength - 4] << 16) | (lpMemFile[dwMemLength -
	    3] << 8) | (lpMemFile[dwMemLength - 2]);
    //Log("PP20 detected: Packed length=%d, Unpacked length=%d\n", dwMemLength, dwDstLen);
    if ((dwDstLen < 512) || (dwDstLen > 0x400000)
	|| (dwDstLen > 16 * dwMemLength))
	return 0;
    if ((pBuffer =
	    (uint8_t *)GlobalAllocPtr(GHND, (dwDstLen + 31) & ~15)) == NULL)
	return 0;
    PP20_DoUnpack(lpMemFile + 4, dwMemLength - 4, pBuffer, dwDstLen);
    *ppMemFile = pBuffer;
    *pdwMemLength = dwDstLen;
    return 1;
}

//////////////////////////////////////////////////////////////////////////////
//
//      MMCMP Unpacker
//

typedef struct MMCMPFILEHEADER
{
    uint32_t id_ziRC;			// "ziRC"
    uint32_t id_ONia;			// "ONia"
    uint16_t hdrsize;
} MMCMPFILEHEADER;

typedef struct MMCMPHEADER
{
    uint16_t version;
    uint16_t nblocks;
    uint32_t filesize;
    uint32_t blktable;
    uint8_t glb_comp;
    uint8_t fmt_comp;
} MMCMPHEADER;

typedef struct MMCMPBLOCK
{
    uint32_t unpk_size;
    uint32_t pk_size;
    uint32_t xor_chk;
    uint16_t sub_blk;
    uint16_t flags;
    uint16_t tt_entries;
    uint16_t num_bits;
} MMCMPBLOCK;

typedef struct MMCMPSUBBLOCK
{
    uint32_t unpk_pos;
    uint32_t unpk_size;
} MMCMPSUBBLOCK;

#define MMCMP_COMP	0x0001
#define MMCMP_DELTA	0x0002
#define MMCMP_16BIT	0x0004
#define MMCMP_STEREO	0x0100
#define MMCMP_ABS16	0x0200
#define MMCMP_ENDIAN	0x0400

typedef struct MMCMPBITBUFFER
{
    unsigned bitcount;
    uint32_t bitbuffer;
    const uint8_t *pSrc;
    const uint8_t *pEnd;
} MMCMPBITBUFFER;

static uint32_t MMCMPBITBUFFER_GetBits(MMCMPBITBUFFER * this, unsigned nBits)
//---------------------------------------
{
    uint32_t d;

    if (!nBits) {
	return 0;
    }
    while (this->bitcount < 24) {
	this->bitbuffer |=
	    ((this->pSrc < this->pEnd) ? *this->pSrc++ : 0) << this->bitcount;
	this->bitcount += 8;
    }
    d = this->bitbuffer & ((1 << nBits) - 1);
    this->bitbuffer >>= nBits;
    this->bitcount -= nBits;

    return d;
}

static const unsigned MMCMP8BitCommands[8] = {
    0x01, 0x03, 0x07, 0x0F, 0x1E, 0x3C, 0x78, 0xF8
};

static const unsigned MMCMP8BitFetch[8] = {
    3, 3, 3, 3, 2, 1, 0, 0
};

static const unsigned MMCMP16BitCommands[16] = {
    0x01, 0x03, 0x07, 0x0F, 0x1E, 0x3C, 0x78, 0xF0,
    0x1F0, 0x3F0, 0x7F0, 0xFF0, 0x1FF0, 0x3FF0, 0x7FF0, 0xFFF0
};

static const unsigned MMCMP16BitFetch[16] = {
    4, 4, 4, 4, 3, 2, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

int MMCMP_Unpack(const uint8_t **ppMemFile, uint32_t * pdwMemLength)
//---------------------------------------------------------
{
    uint32_t dwMemLength = *pdwMemLength;
    const uint8_t *lpMemFile = *ppMemFile;
    uint8_t *pBuffer;
    MMCMPFILEHEADER *pmfh = (MMCMPFILEHEADER *) (lpMemFile);
    MMCMPHEADER *pmmh = (MMCMPHEADER *) (lpMemFile + 10);
    uint32_t *pblk_table;
    uint32_t dwFileSize;
    unsigned i;

    if (PP20_Unpack(ppMemFile, pdwMemLength)) {
	return 1;
    }

    if ((dwMemLength < 256) || (!pmfh) || (pmfh->id_ziRC != 0x4352697A)
	|| (pmfh->id_ONia != 0x61694e4f) || (pmfh->hdrsize < 14)
	|| (!pmmh->nblocks) || (pmmh->filesize < 16)
	|| (pmmh->filesize > 0x8000000)
	|| (pmmh->blktable >= dwMemLength)
	|| (pmmh->blktable + 4 * pmmh->nblocks > dwMemLength)) {
	return 0;
    }

    dwFileSize = pmmh->filesize;
    if ((pBuffer =
	    (uint8_t *)GlobalAllocPtr(GHND,
		(dwFileSize + 31) & ~15)) == NULL) {
	return 0;
    }

    pblk_table = (uint32_t *) (lpMemFile + pmmh->blktable);
    for (i = 0; i < pmmh->nblocks; i++) {
	uint32_t dwMemPos = pblk_table[i];
	MMCMPBLOCK *pblk = (MMCMPBLOCK *) (lpMemFile + dwMemPos);
	MMCMPSUBBLOCK *psubblk = (MMCMPSUBBLOCK *) (lpMemFile + dwMemPos + 20);

	if ((dwMemPos + 20 >= dwMemLength)
	    || (dwMemPos + 20 + pblk->sub_blk * 8 >= dwMemLength)) {
	    break;
	}
	dwMemPos += 20 + pblk->sub_blk * 8;
#ifdef MMCMP_LOG
	Log("block %d: flags=%04X sub_blocks=%d", i, (unsigned)pblk->flags,
	    (unsigned)pblk->sub_blk);
	Log(" pksize=%d unpksize=%d", pblk->pk_size, pblk->unpk_size);
	Log(" tt_entries=%d num_bits=%d\n", pblk->tt_entries, pblk->num_bits);
#endif
	// Data is not packed
	if (!(pblk->flags & MMCMP_COMP)) {
	    for (i = 0; i < pblk->sub_blk; i++) {
		if ((psubblk->unpk_pos > dwFileSize)
		    || (psubblk->unpk_pos + psubblk->unpk_size > dwFileSize))
		    break;
#ifdef MMCMP_LOG
		Log("  Unpacked sub-block %d: offset %d, size=%d\n", i,
		    psubblk->unpk_pos, psubblk->unpk_size);
#endif
		memcpy(pBuffer + psubblk->unpk_pos, lpMemFile + dwMemPos,
		    psubblk->unpk_size);
		dwMemPos += psubblk->unpk_size;
		psubblk++;
	    }
	    // Data is 16-bit packed
	} else if (pblk->flags & MMCMP_16BIT) {
	    MMCMPBITBUFFER bb;
	    uint16_t *pDest = (uint16_t *) (pBuffer + psubblk->unpk_pos);
	    uint32_t dwSize = psubblk->unpk_size >> 1;
	    uint32_t dwPos = 0;
	    unsigned numbits = pblk->num_bits;
	    unsigned subblk = 0, oldval = 0;

#ifdef MMCMP_LOG
	    Log("  16-bit block: pos=%d size=%d ", psubblk->unpk_pos,
		psubblk->unpk_size);
	    if (pblk->flags & MMCMP_DELTA)
		Log("DELTA ");
	    if (pblk->flags & MMCMP_ABS16)
		Log("ABS16 ");
	    Log("\n");
#endif
	    bb.bitcount = 0;
	    bb.bitbuffer = 0;
	    bb.pSrc = lpMemFile + dwMemPos + pblk->tt_entries;
	    bb.pEnd = lpMemFile + dwMemPos + pblk->pk_size;
	    while (subblk < pblk->sub_blk) {
		unsigned newval = 0x10000;
		uint32_t d = MMCMPBITBUFFER_GetBits(&bb, numbits + 1);

		if (d >= MMCMP16BitCommands[numbits]) {
		    unsigned nFetch = MMCMP16BitFetch[numbits];
		    unsigned newbits = MMCMPBITBUFFER_GetBits(&bb,
			nFetch) + ((d -
			    MMCMP16BitCommands[numbits]) << nFetch);
		    if (newbits != numbits) {
			numbits = newbits & 0x0F;
		    } else {
			if ((d = MMCMPBITBUFFER_GetBits(&bb, 4)) == 0x0F) {
			    if (MMCMPBITBUFFER_GetBits(&bb, 1))
				break;
			    newval = 0xFFFF;
			} else {
			    newval = 0xFFF0 + d;
			}
		    }
		} else {
		    newval = d;
		}
		if (newval < 0x10000) {
		    newval =
			(newval & 1) ? (unsigned)-(int)((newval + 1) >> 1) : (newval >> 1);
		    if (pblk->flags & MMCMP_DELTA) {
			newval += oldval;
			oldval = newval;
		    } else if (!(pblk->flags & MMCMP_ABS16)) {
			newval ^= 0x8000;
		    }
		    pDest[dwPos++] = (uint16_t) newval;
		}
		if (dwPos >= dwSize) {
		    subblk++;
		    dwPos = 0;
		    dwSize = psubblk[subblk].unpk_size >> 1;
		    pDest = (uint16_t *) (pBuffer + psubblk[subblk].unpk_pos);
		}
	    }
	    // Data is 8-bit packed
	} else {
	    MMCMPBITBUFFER bb;
	    uint8_t *pDest = pBuffer + psubblk->unpk_pos;
	    uint32_t dwSize = psubblk->unpk_size;
	    uint32_t dwPos = 0;
	    unsigned numbits = pblk->num_bits;
	    unsigned subblk = 0, oldval = 0;
	    const uint8_t *ptable = lpMemFile + dwMemPos;

	    bb.bitcount = 0;
	    bb.bitbuffer = 0;
	    bb.pSrc = lpMemFile + dwMemPos + pblk->tt_entries;
	    bb.pEnd = lpMemFile + dwMemPos + pblk->pk_size;
	    while (subblk < pblk->sub_blk) {
		unsigned newval = 0x100;
		uint32_t d = MMCMPBITBUFFER_GetBits(&bb, numbits + 1);

		if (d >= MMCMP8BitCommands[numbits]) {
		    unsigned nFetch = MMCMP8BitFetch[numbits];
		    unsigned newbits = MMCMPBITBUFFER_GetBits(&bb,
			nFetch) + ((d - MMCMP8BitCommands[numbits]) << nFetch);

		    if (newbits != numbits) {
			numbits = newbits & 0x07;
		    } else {
			if ((d = MMCMPBITBUFFER_GetBits(&bb, 3)) == 7) {
			    if (MMCMPBITBUFFER_GetBits(&bb, 1))
				break;
			    newval = 0xFF;
			} else {
			    newval = 0xF8 + d;
			}
		    }
		} else {
		    newval = d;
		}
		if (newval < 0x100) {
		    int n = ptable[newval];

		    if (pblk->flags & MMCMP_DELTA) {
			n += oldval;
			oldval = n;
		    }
		    pDest[dwPos++] = (uint8_t)n;
		}
		if (dwPos >= dwSize) {
		    subblk++;
		    dwPos = 0;
		    dwSize = psubblk[subblk].unpk_size;
		    pDest = pBuffer + psubblk[subblk].unpk_pos;
		}
	    }
	}
    }

    *ppMemFile = pBuffer;
    *pdwMemLength = dwFileSize;
    return 1;
}

#endif	// } MODPLUG_MMCMP_SUPPORT
