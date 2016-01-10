/*
 * This source code is public domain.
 *
 * Authors: Olivier Lapicque <olivierl@jps.net>
 *
 *	$Id: load_umx.c,v 1.1 2001/11/07 23:36:13 johns Exp $
*/

#include "stdafx.h"
#include "sndfile.h"

#define MODMAGIC_OFFSET (20+31*30+130)

int CSoundFile_ReadUMX(CSoundFile * that, const uint8_t * lpStream,
    uint32_t dwMemLength)
//---------------------------------------------------------------
{
    if ((!lpStream) || (dwMemLength < 0x800)) {
	return 0;
    }
    // Rip Mods from UMX
    if ((bswapLE32(*((uint32_t *) (lpStream + 0x20))) < dwMemLength)
	&& (bswapLE32(*((uint32_t *) (lpStream + 0x18))) <= dwMemLength - 0x10)
	&& (bswapLE32(*((uint32_t *) (lpStream + 0x18))) >=
	    dwMemLength - 0x200)) {
	unsigned uscan;

	for (uscan = 0x40; uscan < 0x500; uscan++) {
	    uint32_t dwScan = bswapLE32(*((uint32_t *) (lpStream + uscan)));

	    // IT
	    if (dwScan == 0x4D504D49) {
		uint32_t dwRipOfs = uscan;

		return CSoundFile_ReadIT(that, lpStream + dwRipOfs,
		    dwMemLength - dwRipOfs);
	    }
	    // S3M
	    if (dwScan == 0x4D524353) {
		uint32_t dwRipOfs = uscan - 44;

		return CSoundFile_ReadS3M(that, lpStream + dwRipOfs,
		    dwMemLength - dwRipOfs);
	    }
	    // XM
	    if (!strnicmp((const char *)(lpStream + uscan), "Extended Module",
		    15)) {
		uint32_t dwRipOfs = uscan;

		return CSoundFile_ReadXM(that, lpStream + dwRipOfs,
		    dwMemLength - dwRipOfs);
	    }
	    // MOD
	    if ((uscan > MODMAGIC_OFFSET) && (dwScan == 0x2e4b2e4d)) {
		uint32_t dwRipOfs = uscan - MODMAGIC_OFFSET;

		return CSoundFile_ReadMod(that, lpStream + dwRipOfs,
		    dwMemLength - dwRipOfs);
	    }
	}
    }
    return 0;
}
