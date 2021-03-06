/*
**	A clone of a famous game.
*/
/**@name action_minegold.c -	The mine gold action. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: action_minegold.c,v 1.16 1999/11/25 00:59:57 root Exp $
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
#include "actions.h"
#include "tileset.h"
#include "map.h"
#include "interface.h"

/**
**	Move to goldmine.
**
**	@param unit	Pointer to worker unit.
**
**	@return		TRUE if reached, otherwise FALSE.
*/
local int MoveToGoldMine(Unit* unit)
{
    Unit* destu;

    if( !HandleActionMove(unit) ) {	// reached end-point
	return 0;
    }
    // FIXME: HandleActionMove must return this: reached nearly?

    unit->Command.Action=UnitActionMineGold;

    destu=unit->Command.Data.Move.Goal;
    if( !destu || MapDistanceToUnit(unit->X,unit->Y,destu)!=1 ) {
	DebugLevel3("GOLD-MINE NOT REACHED %d,%d\n",dx,dy);
	return -1;
    }

    //
    // Activate gold-mine
    //
    // FIXME: hmmm... we're in trouble here.
    // we should check if there's still some gold left in the mine instead.
    if( !destu->Type->GoldMine ) {  // Goldmine destroyed.
	DebugLevel1("WAIT after goldmine destroyed %d\n",unit->Wait);
	unit->Command.Action=UnitActionStill;
	unit->SubAction=0;
	return 0;
    }
    destu->Command.Data.GoldMine.Active++;
    destu->Frame=1;			// FIXME: configurable

    RemoveUnit(unit);
    unit->X=destu->X;
    unit->Y=destu->Y;

    if( MINE_FOR_GOLD<UNIT_MAX_WAIT ) {
	unit->Wait=MINE_FOR_GOLD;
    } else {
	unit->Wait=UNIT_MAX_WAIT;
    }
    unit->Value=MINE_FOR_GOLD-unit->Wait;

    return 1;
}

/**
**	Mine gold in goldmine.
**
**	@param unit	Pointer to worker unit.
**
**	@return		TRUE if ready, otherwise FALSE.
*/
local int MineInGoldmine(Unit* unit)
{
    Unit* mine;
    Unit* destu;

    DebugLevel3("Waiting\n");
    if( !unit->Value ) {
	//
	// Have gold
	//
	mine=GoldMineOnMap(unit->X,unit->Y);
	IfDebug(
	    DebugLevel3("Found %d,%d=%Zd\n",unit->X,unit->Y,mine-UnitsPool);
	    if( !mine ) {
		DebugLevel0("No unit? (%d,%d)\n",unit->X,unit->Y);
		abort();
	    } );

	//
	//	End of gold: destroy gold-mine.
	//
	if( mine->Value<=GOLD_PER_MINE ) {
	    // FIXME: better way to fix bug
	    unit->Removed=0;
	    DropOutAll(mine);
	    DestroyUnit(mine);
	    if( mine->Value<GOLD_PER_MINE ) {
		// FIXME: should return 0 here?
		DebugLevel0(__FUNCTION__": Too less gold\n");
	    }
	}

	//
	//	Update gold mine.
	//
	mine->Value-=GOLD_PER_MINE;	// remove gold from store
	if( !--mine->Command.Data.GoldMine.Active ) {
	    mine->Frame=0;
	}
	if( IsSelected(mine) ) {
	    MustRedraw|=RedrawTopPanel;
	}

	if( !(destu=FindGoldDeposit(unit->Player,unit->X,unit->Y)) ) {
	    DropOutOnSide(unit,HeadingW
		    ,mine->Type->TileWidth
		    ,mine->Type->TileHeight);
	    unit->Command.Action=UnitActionStill;
	    unit->SubAction=0;
	    DebugLevel3("Mine without goldmine\n");
	} else {
	    DropOutNearest(unit
		    ,destu->X
		    ,destu->Y
		    ,mine->Type->TileWidth
		    ,mine->Type->TileHeight);
	    unit->Command.Data.Move.Fast=1;
	    unit->Command.Data.Move.Goal=destu;
	    unit->Command.Data.Move.Range=1;
#if 1
	    NearestOfUnit(destu,unit->X,unit->Y
		,&unit->Command.Data.Move.DX
		,&unit->Command.Data.Move.DY);
#else
	    unit->Command.Data.Move.DX=destu->X;
	    unit->Command.Data.Move.DY=destu->Y;
#endif
	    unit->Command.Action=UnitActionMineGold;
	}

	if( unit->Type->Type==UnitPeon ) {
	    unit->Type=&UnitTypes[UnitPeonWithGold];
	} else if( unit->Type->Type==UnitPeasant ) {
	    unit->Type=&UnitTypes[UnitPeasantWithGold];
	} else {
	    DebugLevel0("Wrong unit for mining gold %d\n"
		,unit->Type->Type);
	}
	if( UnitVisible(unit) ) {
	    MustRedraw|=RedrawMap;
	}
	if( IsSelected(unit) ) {
	    UpdateBottomPanel();
	    MustRedraw|=RedrawBottomPanel;
	}
	unit->Wait=1;
	return unit->SubAction;
    }

    //
    //	Continue waiting
    //
    if( unit->Value<UNIT_MAX_WAIT ) {
	unit->Wait=unit->Value;
    } else {
	unit->Wait=UNIT_MAX_WAIT;
    }
    unit->Value-=unit->Wait;
    return 0;
}

/**
**	Unit mines gold!
**
**	FIXME: must move to center of gold-mine.
**	XXX
**	XOX
**	XXX	X are move animated!
*/
global void HandleActionMineGold(Unit* unit)
{
    int x;
    int y;
    int i;
    Unit* destu;

// FIXME: GoldMine and GreatHall hard coded

    switch( unit->SubAction ) {
	//
	//	Move to gold-mine
	//
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:					// 4 tries to reach gold-mine
	    if( (i=MoveToGoldMine(unit)) ) {
		if( i==-1 ) {
		    if( ++unit->SubAction==5 ) {
			unit->Command.Action=UnitActionStill;
			unit->SubAction=0;
		    }
		} else {
		    unit->SubAction=64;
		}
	    }
	    break;

	//
	//	Wait for mine gold
	//
	case 64:
	    if( MineInGoldmine(unit) ) {
		++unit->SubAction;
	    }
	    break;

	//
	//	Return to gold deposit
	//
	case 65:
	case 66:
	case 67:
	case 68:				// 4 tries to reach depot
	    if( !HandleActionMove(unit) ) {	// reached end-point
		return;
	    }

	    unit->Command.Action=UnitActionMineGold;
	    x=unit->Command.Data.Move.DX;
	    y=unit->Command.Data.Move.DY;
	    destu=GoldDepositOnMap(x,y);
	    if( !destu || MapDistanceToUnit(unit->X,unit->Y,destu)!=1 ) {
		if( ++unit->SubAction==69 ) {
		    DebugLevel2("GOLD-DEPOSIT NOT REACHED %Zd=%d,%d ? %d\n"
			,destu-UnitsPool,x,y
			,MapDistanceToUnit(unit->X,unit->Y,destu));
		    unit->Command.Action=UnitActionStill;
		    unit->SubAction=0;
		}
		return;
	    }

	    RemoveUnit(unit);
	    unit->X=destu->X;
	    unit->Y=destu->Y;

	    //
	    //	Update gold.
	    //
	    unit->Player->Gold+=unit->Player->GoldPerMine;
	    if( unit->Player==ThisPlayer ) {
	        MustRedraw|=RedrawResources;
	    }
	    
	    if( unit->Type->Type==UnitPeonWithGold ) {
		unit->Type=&UnitTypes[UnitPeon];
	    } else if( unit->Type->Type==UnitPeasantWithGold ) {
		unit->Type=&UnitTypes[UnitPeasant];
	    } else {
		DebugLevel0("Wrong unit for returning gold %d\n"
		    ,unit->Type->Type);
	    }

	    if( WAIT_FOR_GOLD<UNIT_MAX_WAIT ) {
		unit->Wait=WAIT_FOR_GOLD;
	    } else {
		unit->Wait=UNIT_MAX_WAIT;
	    }
	    unit->Value=WAIT_FOR_GOLD-unit->Wait;
	    unit->SubAction=128;
	    break;

	//
	//	Wait for gold stored.
	//
	case 128:
	    DebugLevel3("Waiting\n");
	    if( !unit->Value ) {
		// FIXME: return to last position!
		if( !(destu=FindGoldMine(unit->X,unit->Y)) ) {
		    DropOutOnSide(unit,HeadingW
			    ,UnitTypes[UnitGreatHall].TileWidth
			    ,UnitTypes[UnitGreatHall].TileHeight);
		    unit->Command.Action=UnitActionStill;
		    unit->SubAction=0;
		} else {
		    DropOutNearest(unit
			    ,destu->X
			    ,destu->Y
			    ,UnitTypes[UnitGreatHall].TileWidth
			    ,UnitTypes[UnitGreatHall].TileHeight);
		    unit->Command.Data.Move.Fast=1;
		    unit->Command.Data.Move.Goal=destu;
		    unit->Command.Data.Move.Range=1;
#if 1
		    NearestOfUnit(destu,unit->X,unit->Y
			,&unit->Command.Data.Move.DX
			,&unit->Command.Data.Move.DY);
#else
		    unit->Command.Data.Move.DX=destu->X;
		    unit->Command.Data.Move.DY=destu->Y;
#endif
		    unit->Command.Action=UnitActionMineGold;
		    // unit->Value=destu-UnitsPool;  // unused...
		}

		if( UnitVisible(unit) ) {
		    MustRedraw|=RedrawMap;
		}
		unit->Wait=1;
		unit->SubAction=0;
	    } else {
		if( unit->Value<UNIT_MAX_WAIT ) {
		    unit->Wait=unit->Value;
		} else {
		    unit->Wait=UNIT_MAX_WAIT;
		}
		unit->Value-=unit->Wait;
	    }
	    break;
    }
}

//@}
