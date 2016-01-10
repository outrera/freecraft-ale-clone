/*
**	A clone of a famous game.
*/
/**@name ub_table.c	-	The unit button table. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: ub_table.c,v 1.2 1999/07/08 21:09:55 root Exp $
*/

//@{

#if 0

#include <stdio.h>

#include "clone.h"
#include "video.h"
#include "tileset.h"
#include "map.h"
#include "sound_id.h"
#include "unitsound.h"
#include "icons.h"
#include "button.h"
#include "unittype.h"


#include "player.h"
#include "unit.h"
#include "interface.h"

#define R_Nothing	0
#define R_Upgrade	0

/*----------------------------------------------------------------------------
--	Button table
----------------------------------------------------------------------------*/

/**
**	Button type definition
*/
global char ButtonType[] = "button-type";

/**
**	Unit button definition
*/
global UnitButton UnitButtons[] = {
// * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING *
// Warning this is generated!!

{    ButtonType, "human-move"
    ,{ "MovePeasant" }
    ,R_Nothing,		0
    ,B_Move,		0
    ,'m',	"~MOVE"
},
{
    ButtonType, "human-shield1"
    ,{ "Shield1Human" }
    ,R_Upgrade,		1
    ,B_Stop,		0
    ,'s',	"~STOP"
},
{
    ButtonType, "human-shield2"
    ,{ "Shield2Human" }
    ,R_Upgrade,		2
    ,B_Stop,		0
    ,'s',	"~STOP"
},
{
    ButtonType, "human-shield3"
    ,{ "Shield3Human" }
    ,R_Upgrade,		3
    ,B_Stop,		0
    ,'s',	"~STOP"
},
{
    ButtonType, "human-sword1"
    ,{ "Sword1Human" }
    ,R_Upgrade,		1
    ,B_Stop,		0
    ,'a',	"~ATTACK"
},
{
    ButtonType, "human-sword2"
    ,{ "Sword2Human" }
    ,R_Upgrade,		2
    ,B_Stop,		0
    ,'a',	"~ATTACK"
},
{
    ButtonType, "human-sword3"
    ,{ "Sword3Human" }
    ,R_Upgrade,		3
    ,B_Stop,		0
    ,'a',	"~ATTACK"
},
{
    ButtonType, "human-patrol"
    ,{ "PatrolLandHuman" }
    ,R_Nothing,		0
    ,B_Patrol,		0
    ,'p',	"~PATROL"
},
{
    ButtonType, "human-stand-ground"
    ,{ "StandGroundHuman" }
    ,R_Nothing,		0
    ,B_StandGround,	0
    ,'t',	"S~TAND GROUND"
},

{
    NULL, NULL
},
// Warning this is generated!!
// * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING *
};

#endif

//@}
