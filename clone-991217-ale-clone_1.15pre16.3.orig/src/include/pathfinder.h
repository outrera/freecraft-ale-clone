/*
**	A clone of a famous game.
*/
/**@name pathfinder.h	-	The path finder headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: pathfinder.h,v 1.7 1999/12/03 09:18:05 root Exp $
*/

#ifndef	__PATH_FINDER_H__
#define	__PATH_FINDER_H__

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include "unit.h"
#include "map.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

extern unsigned char Matrix[(MaxMapWidth+1)*(MaxMapHeight+1)];  /// Path matrix


/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

    ///
extern unsigned char* CreateMatrix(void);
    ///
extern int NewPath(Unit* unit,int* xdp,int* ydp);
    ///
extern int PlaceReachable(Unit* unit,int x,int y);
    ///
extern int UnitReachable(Unit* unit,Unit* dest);

    /// Returns the next element of the path
extern int NextPathElement(Unit*,int* xdp,int* ydp);

//@}

#endif	// !__PATH_FINDER_H__
