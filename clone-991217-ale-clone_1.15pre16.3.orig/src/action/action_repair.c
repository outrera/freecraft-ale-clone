/*
**	A clone of a famous game.
*/
/**@name action_repair.c	-	The repair action. */
/*
**	(c) Copyright 1999 by Vladi Shabanski
**
**	$Id: action_repair.c,v 1.3 1999/07/08 21:09:57 root Exp $
*/


/*

  This is a quick hack: repair action, done with attack action reversing
  :)
  
*/

//@{

#include <stdio.h>
#include <stdlib.h>

#include "clone.h"
#include "video.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "missile.h"
#include "actions.h"
#include "sound.h"
#include "tileset.h"
#include "map.h"

/*----------------------------------------------------------------------------
--      Repair
----------------------------------------------------------------------------*/

//	Peon, Peasant, Attacking Peon, Attacking Peasant.
local Animation PeonRepair[] = {
    { 0, 0, 3, 25},{ 0, 0, 3,  5},{ 0, 0, 3,  5},{12, 0, 5,  5},{ 0, 0, 3,  5},
    { 0, 0, 7,-20},{ 3, 0, 1,  0}
};

/**
**	Generic unit repair.
**
**	@param unit	Unit, for that the repair animation is played.
**	@param repair	Repair animation.
*/
local void DoActionRepairGeneric(Unit* unit,Animation* repair)
{
    Unit* goal;
    int flags;
    Player* player;

    flags=UnitShowAnimation(unit,repair);

    if( (flags&AnimationSound) ) {	
	PlayUnitSound(unit,VoiceAttacking); //FIXME: should be something else...
    }

    goal=unit->Command.Data.Move.Goal;
    if (!goal) {
	// FIXME: Should abort the repair
	return;
    }

    // FIXME: Should substract the correct values for repair

    //	Check if enough resources are available

    player=unit->Player;
    if( !player->Gold || !player->Wood ) {
	return;
    }

    //	Repair the unit

    goal->HP++;
    if ( goal->HP > goal->Stats->HitPoints ) {
	goal->HP = goal->Stats->HitPoints;
    }
    // FIXME: must update panel if unit is selected!

    //	Subtract the resources

    player->Gold--;
    player->Wood--;

    if( player==ThisPlayer ) {
	MustRedraw|=RedrawResources;
    }
}

/**
**	Animate unit repair
**
**	@param unit	Unit, for that the repair animation is played.
*/
local int AnimateActionRepair(Unit* unit)
{
    int type;

    type=unit->Type->Type;
    if( type<UnitTypeInternalMax ) {
	DoActionRepairGeneric(unit,PeonRepair);
    }

    return 0;
}

/**
**	Unit repairs
**
**	@param unit	Unit, for that the attack is handled.
*/
global int HandleActionRepair(Unit* unit)
{
    Unit* goal;
    int err;

    DebugLevel3("Repair %d\n",unit-Units);

    switch( unit->SubAction ) {
	//
	//	Move near to target.
	//
	case 0:
	    // FIXME: RESET FIRST!!
	    err=HandleActionMove(unit); 
	    if( unit->Reset ) {
		//
		//	No goal: if meeting enemy attack it.
		//
		goal=unit->Command.Data.Move.Goal;

		//
		//	Target is dead, choose new one.
		//

		//
		//	Have reached target?
		//
		if( goal && MapDistanceToUnit(unit->X,unit->Y,goal)
			<=unit->Stats->AttackRange ) {
		    unit->State=0;
		    if( !unit->Type->Tower ) {
			UnitNewHeadingFromXY(unit
			    ,goal->X-unit->X,goal->Y-unit->Y);
		    }
		    unit->SubAction=1;
		} else if( err ) {
		    return 1;
		}
		unit->Command.Action=UnitActionRepair;
	    }
	    break;

	//
	//	Repair the target.
	//
	case 1:
	    AnimateActionRepair(unit);
	    if( unit->Reset ) {
		goal=unit->Command.Data.Move.Goal;

		//
		//	Target is fine, choose new one.
		//
                if ( !goal || goal->HP >= goal->Stats->HitPoints ) {
                    unit->Command.Action=UnitActionStill;
		    unit->SubAction=0;
		    unit->State=0;
                    return 1;
		}
#if 0
		//
		//	Still near to target, if not goto target.
		//
		if( MapDistanceToUnit(unit->X,unit->Y,goal)
			>unit->Type->AttackRange ) {
		    unit->Command.Data.Move.Fast=1;
		    unit->Command.Data.Move.DX=goal->X;
		    unit->Command.Data.Move.DY=goal->Y;
		    unit->Frame=0;
		    unit->State=0;
		    unit->SubAction=0;
		    break;
		}
#endif
	    }
	    break;
    }

    return 0;
}

//@}
