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
/**@name ccl_missile.c	-	The missile-type ccl functions. */
//
//	(c) Copyright 2002-2003 by Lutz Sammer
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
//	$Id: ccl_missile.c,v 1.24 2003/03/05 07:24:38 jsalmon3 Exp $

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "freecraft.h"
#include "video.h"
#include "tileset.h"
#include "unittype.h"
#include "missile.h"
#include "ccl_sound.h"
#include "ccl.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

IfDebug(
extern int NoWarningMissileType;		/// quiet ident lookup.
);

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Parse missile-type.
**
**	@param list	List describing missile-type.
*/
local SCM CclDefineMissileType(SCM list)
{
    SCM value;
    char* str;
    MissileType* mtype;
    unsigned i;

    //	Slot identifier

    str=gh_scm2newstr(gh_car(list),NULL);
    list=gh_cdr(list);
    IfDebug( i=NoWarningMissileType; NoWarningMissileType=1; );
    mtype=MissileTypeByIdent(str);
    IfDebug( NoWarningMissileType=i; );
    if( mtype ) {
	DebugLevel0Fn("Redefining missile-type `%s'\n" _C_ str);
	free(str);
    } else {
	mtype=NewMissileTypeSlot(str);	// str consumed!
    }

    mtype->NumDirections=8;
    //
    //	Parse the arguments, already the new tagged format.
    //
    while( !gh_null_p(list) ) {
	value=gh_car(list);
	list=gh_cdr(list);
	if( gh_eq_p(value,gh_symbol2scm("file")) ) {
	    free(mtype->File);
	    mtype->File=gh_scm2newstr(gh_car(list),NULL);
	} else if( gh_eq_p(value,gh_symbol2scm("size")) ) {
	    value=gh_car(list);
	    mtype->Width=gh_scm2int(gh_car(value));
	    value=gh_cdr(value);
	    mtype->Height=gh_scm2int(gh_car(value));
	} else if( gh_eq_p(value,gh_symbol2scm("frames")) ) {
	    mtype->SpriteFrames=gh_scm2int(gh_car(list));
	} else if( gh_eq_p(value,gh_symbol2scm("num-directions")) ) {
	    mtype->NumDirections=gh_scm2int(gh_car(list));
	} else if( gh_eq_p(value,gh_symbol2scm("fired-sound")) ) {
	    free(mtype->FiredSound.Name);
	    mtype->FiredSound.Name=gh_scm2newstr(gh_car(list),NULL);
	} else if( gh_eq_p(value,gh_symbol2scm("impact-sound")) ) {
	    free(mtype->ImpactSound.Name);
	    mtype->ImpactSound.Name=gh_scm2newstr(gh_car(list),NULL);
	} else if( gh_eq_p(value,gh_symbol2scm("class")) ) {
	    value=gh_car(list);
	    for( i=0; MissileClassNames[i]; ++i ) {
		if( gh_eq_p(value,
			    gh_symbol2scm((char*)MissileClassNames[i])) ) {
		    mtype->Class=i;
		    break;
		}
	    }
	    if( !MissileClassNames[i] ) {
		// FIXME: this leaves a half initialized missile-type
		errl("Unsupported class",value);
	    }
	} else if( gh_eq_p(value,gh_symbol2scm("delay")) ) {
	    mtype->StartDelay=gh_scm2int(gh_car(list));
	} else if( gh_eq_p(value,gh_symbol2scm("sleep")) ) {
	    mtype->Sleep=gh_scm2int(gh_car(list));
	} else if( gh_eq_p(value,gh_symbol2scm("speed")) ) {
	    mtype->Speed=gh_scm2int(gh_car(list));
	} else if( gh_eq_p(value,gh_symbol2scm("range")) ) {
	    mtype->Range=gh_scm2int(gh_car(list));
	} else if( gh_eq_p(value,gh_symbol2scm("impact-missile")) ) {
	    free(mtype->ImpactName);
	    mtype->ImpactName=gh_scm2newstr(gh_car(list),NULL);
	} else if( gh_eq_p(value,gh_symbol2scm("can-hit-owner")) ) {
	    mtype->CanHitOwner=gh_scm2bool(gh_car(list));
	} else if( gh_eq_p(value,gh_symbol2scm("friendly-fire")) ) {
	    mtype->FriendlyFire=gh_scm2bool(gh_car(list));
	} else {
	    // FIXME: this leaves a half initialized missile-type
	    errl("Unsupported tag",value);
	}
	list=gh_cdr(list);
    }

    return SCM_UNSPECIFIED;
}

/**
**	Define missile type mapping from original number to internal symbol
**
**	@param list	List of all names.
*/
local SCM CclDefineMissileTypeWcNames(SCM list)
{
    int i;
    char** cp;

    if( (cp=MissileTypeWcNames) ) {		// Free all old names
	while( *cp ) {
	    free(*cp++);
	}
	free(MissileTypeWcNames);
    }

    //
    //	Get new table.
    //
    i=gh_length(list);
    MissileTypeWcNames=cp=malloc((i+1)*sizeof(char*));
    while( i-- ) {
	*cp++=gh_scm2newstr(gh_car(list),NULL);
	list=gh_cdr(list);
    }
    *cp=NULL;

    return SCM_UNSPECIFIED;
}

/**
**	Create a missile.
**
**	@param list	List of all names.
*/
local SCM CclMissile(SCM list)
{
    SCM value;
    char *str;
    MissileType *type;
    int x, y, dx, dy;
    Missile *missile;

    DebugLevel0Fn("FIXME: not finished\n");

    missile=NULL;
    type=NULL;
    x = dx = y = dy = -1;

    while (!gh_null_p (list)) {
	value = gh_car (list);
	list = gh_cdr (list);

	if (gh_eq_p (value, gh_symbol2scm ("type"))) {
	    value = gh_car (list);
	    list = gh_cdr (list);
	    str = gh_scm2newstr (value, NULL);
	    type = MissileTypeByIdent (str);
	    free (str);
	} else if (gh_eq_p (value, gh_symbol2scm ("pos"))) {
	    SCM sublist = gh_car (list);
	    list = gh_cdr (list);
	    x = gh_scm2int (gh_car (sublist));
	    y = gh_scm2int (gh_cadr (sublist));
	} else if (gh_eq_p (value, gh_symbol2scm ("goal"))) {
	    SCM sublist = gh_car (list);
	    list = gh_cdr (list);
	    dx = gh_scm2int (gh_car (sublist));
	    dy = gh_scm2int (gh_cadr (sublist));
	} else if (gh_eq_p (value, gh_symbol2scm ("local"))) {
	    DebugCheck (!type);
	    missile = MakeLocalMissile (type, x, y, dx, dy);
		// we need to reinitialize position parameters - that's because of
		// the way InitMissile() (called from MakeLocalMissile()) computes
		// them - it works for creating a missile during a game but breaks
		// loading the missile from a file.
		missile->X = x;
		missile->Y = y;
		missile->DX = dx;
		missile->DY = dy;
	    missile->Local = 1;
	} else if (gh_eq_p (value, gh_symbol2scm ("global"))) {
	    DebugCheck (!type);
	    missile = MakeMissile (type, x, y, dx, dy);
		missile->X = x;
		missile->Y = y;
		missile->DX = dx;
		missile->DY = dy;
	    missile->Local = 0;
	} else if (gh_eq_p (value, gh_symbol2scm ("frame"))) {
	    DebugCheck (!missile);
	    missile->SpriteFrame = gh_scm2int (gh_car (list));
	    list = gh_cdr (list);
	} else if (gh_eq_p (value, gh_symbol2scm ("state"))) {
	    DebugCheck (!missile);
	    missile->State = gh_scm2int (gh_car (list));
	    list = gh_cdr (list);
	} else if (gh_eq_p (value, gh_symbol2scm ("wait"))) {
	    DebugCheck (!missile);
	    missile->Wait = gh_scm2int (gh_car (list));
	    list = gh_cdr (list);
	} else if (gh_eq_p (value, gh_symbol2scm ("delay"))) {
	    DebugCheck (!missile);
	    missile->Delay = gh_scm2int (gh_car (list));
	    list = gh_cdr (list);
	} else if (gh_eq_p (value, gh_symbol2scm ("source"))) {
	    DebugCheck (!missile);
	    value = gh_car (list);
	    list = gh_cdr (list);
	    str = gh_scm2newstr (value, NULL);
	    missile->SourceUnit = UnitSlots[strtol (str+1, 0, 16)];
	    free(str);
	    ++missile->SourceUnit->Refs;
	} else if (gh_eq_p (value, gh_symbol2scm ("target"))) {
	    DebugCheck (!missile);
	    value = gh_car (list);
	    list = gh_cdr (list);
	    str = gh_scm2newstr (value, NULL);
	    missile->TargetUnit = UnitSlots[strtol (str+1, 0, 16)];
	    free(str);
	    ++missile->TargetUnit->Refs;
	} else if (gh_eq_p (value, gh_symbol2scm ("damage"))) {
	    DebugCheck (!missile);
	    missile->Damage = gh_scm2int (gh_car (list));
	    list = gh_cdr (list);
	} else if (gh_eq_p (value, gh_symbol2scm ("ttl"))) {
	    DebugCheck (!missile);
	    missile->TTL = gh_scm2int (gh_car (list));
	    list = gh_cdr (list);
	} else if (gh_eq_p (value, gh_symbol2scm ("controller"))) {
	    DebugCheck (!missile);
	    DebugLevel0Fn ("FIXME: no point in reading a pointer value from "
		    "a saved game!!\n");
	    //missile->Controller = gh_scm2int (gh_car (list));
	    missile->Controller = NULL;
	    list = gh_cdr (list);
	} else if (gh_eq_p (value, gh_symbol2scm ("data"))) {
	    SCM sublist = gh_car (list);
	    list = gh_cdr (list);
	    missile->D = gh_scm2int (gh_car (sublist));
	    sublist = gh_cdr (sublist);
	    missile->Dx = gh_scm2int (gh_car (sublist));
	    sublist = gh_cdr (sublist);
	    missile->Dy = gh_scm2int (gh_car (sublist));
	    sublist = gh_cdr (sublist);
	    missile->Xstep = gh_scm2int (gh_car (sublist));
	    sublist = gh_cdr (sublist);
	    missile->Ystep = gh_scm2int (gh_car (sublist));
	}
    }
    return SCM_UNSPECIFIED;
}

/**
**	Register CCL features for missile-type.
*/
global void MissileCclRegister(void)
{
    gh_new_procedureN("define-missiletype-wc-names",
	    CclDefineMissileTypeWcNames);
    gh_new_procedureN("define-missile-type",CclDefineMissileType);
    gh_new_procedureN("missile",CclMissile);
}

//@}
