//   ___________		     _________		      _____  __
//   \_	  _____/______   ____   ____ \_   ___ \____________ _/ ____\/  |_
//    |    __) \_  __ \_/ __ \_/ __ \/    \  \/\_  __ \__  \\   __\\   __|
//    |     \   |  | \/\  ___/\  ___/\     \____|  | \// __ \|  |   |  |
//    \___  /   |__|    \___  >\___  >\______  /|__|  (____  /__|   |__|
//	  \/		    \/	   \/	     \/		   \/
//  ______________________                           ______________________
//			  T H E   W A R   B E G I N S
//	   FreeCraft - A free fantasy real time strategy game engine
//
/**@name mpq.h		-	The mpq headerfile. */
//
//	(c) Copyright 2002 by Jimmy Salmon, Lutz Sammer
//
//	FreeCraft is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published
//	by the Free Software Foundation; only version 2 of the License.
//
//	FreeCraft is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	$Id: mpq.h,v 1.4 2002/12/17 06:40:44 nehalmistry Exp $

#ifndef __MPQ_H__
#define __MPQ_H__

//@{

/*----------------------------------------------------------------------------
--	Defines
----------------------------------------------------------------------------*/

#define UInt32	unsigned long

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

    /// Block table
extern UInt32 *MpqBlockTable;
    /// Filename table
extern char *MpqFilenameTable;
    /// Identify table
extern char *MpqIdentifyTable;
    /// File count
extern int MpqFileCount;


/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

    /// Read mpq info
extern int MpqReadInfo(FILE *fpMpq);
    /// Extract mpq entry
extern int MpqExtractTo(unsigned char *mpqbuf, UInt32 entry, FILE *fpMpq);
    /// Clean mpq
extern void CleanMpq(void);


//@}

#endif // !__MPQ_H__
