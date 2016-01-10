/*
**	A clone of a famous game.
*/
/**@name ui.c	-	The user interface globals. */
/*
**	(c) Copyright 1999 by Lutz Sammer and Andreas Arens
**
**	$Id: ui.c,v 1.2 1999/12/03 09:18:17 root Exp $
*/

//@{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clone.h"
#include "interface.h"
#include "ui.h"

#ifdef USE_CCL
#include <guile/gh.h>
#endif

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	The user interface configuration
*/
global UI TheUI = {
    0,					// reverse mouse
    -1,					// warp x
    -1,					// warp y
    MOUSEADJUST,			// mouse speed
    MOUSESCALE				// mouse scale
};

//@}
