/*
**	A clone of a famous game.
*/
/**@name pud.h		-	The pud headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: pud.h,v 1.3 1999/06/19 22:21:52 root Exp $
*/

#ifndef __PUD_H__
#define __PUD_H__

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include "map.h"

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

    /// Load a pud file.
extern void LoadPud(const char* pud,WorldMap* map);

//@}

#endif // !__PUD_H__
