/*
**	A clone of a famous game.
*/
/**@name button.c	-	The button code. */
/*
**	(c) Copyright 1999 by Lutz Sammer
**
**	$Id: button.c,v 1.2 1999/07/08 21:09:55 root Exp $
*/

//@{

#if 0
// This was the start of my (johns) version, the new and active version
// is from cade and could be found in ui/button_table.c and ui/botpanel.c

#include <stdio.h>
#include <string.h>

#include "clone.h"
#include "video.h"
#include "tileset.h"
#include "map.h"
#include "sound_id.h"
#include "unitsound.h"
#include "button.h"

#include "etlib/hash.h"

local hashtable(int,61) ButtonHash;	/// lookup table for button names

/**
**	Find unit button by identifier.
**
**	@param ident	The button identifier.
**	@return		Unit button pointer.
*/
global UnitButton* ButtonByIdent(const char* ident)
{
    return *(UnitButton**)hash_find(ButtonHash,(char*)ident);
}

/**
**	Init button.
*/
global void InitUnitButtons(void)
{
    int i;

//    for( i=0; i<sizeof(UnitButtons)/sizeof(*UnitButtons); ++i ) {
    for( i=0; UnitButtons[i].Ident; ++i ) {
//	DebugLevel0("%s\n",UnitButtons[i].Ident); 
	*(UnitButton**)hash_add(ButtonHash,UnitButtons[i].Ident)=
		&UnitButtons[i];
    }
}

#endif

//@}
