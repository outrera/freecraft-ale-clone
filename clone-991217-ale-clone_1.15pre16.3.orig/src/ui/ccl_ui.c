/*
**	A clone of a famous game.
*/
/**@name ccl_ui.c	-	The ui ccl functions. */
/*
**	(c) Copyright 1999 by Lutz Sammer
**
**	$Id: ccl_ui.c,v 1.2 1999/12/03 09:18:17 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>

#include "clone.h"

#ifdef USE_CCL	// {

#include "ccl.h"
#include "ui.h"

#include <guile/gh.h>			// I use guile for a quick hack

extern SCM gh_new_procedureN(char* proc_name, SCM (*fn) ());

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Enable the reverse map move.
*/
local SCM CclReverseMouseMove(void)
{
    TheUI.ReverseMouseMove=1;

    return SCM_UNSPECIFIED;
}

/**
**	Disable the reverse map move.
*/
local SCM CclNoReverseMouseMove(void)
{
    TheUI.ReverseMouseMove=0;

    return SCM_UNSPECIFIED;
}

/**
**	Defines the SVGALIB mouse speed adjust (must be > 0)
*/
local SCM CclMouseAdjust(SCM adjust)
{
    SCM old;

    old=gh_int2scm(TheUI.MouseAdjust);
    TheUI.MouseAdjust=gh_scm2int(adjust);

    return old;
}

local SCM CclMouseScale(SCM scale)
{
    SCM old;

    old=gh_int2scm(TheUI.MouseScale);
    TheUI.MouseScale=gh_scm2int(scale);

    return old;
}

/**
**	Register CCL features for ui.
*/
global void UserInterfaceCclRegister(void)
{
    gh_new_procedure0_0("reverse-map-move",CclReverseMouseMove);
    gh_new_procedure0_0("no-reverse-map-move",CclNoReverseMouseMove);
    gh_new_procedure1_0("mouse-adjust",CclMouseAdjust);
    gh_new_procedure1_0("mouse-scale",CclMouseScale);
}

#endif	// } USE_CCL

//@}
