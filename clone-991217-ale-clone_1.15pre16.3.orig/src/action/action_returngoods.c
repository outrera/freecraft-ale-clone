/*
**	A clone of a famous game.
*/
/**@name action_returngoods.c -	The return goods action. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: action_returngoods.c,v 1.8 1999/06/20 02:09:22 root Exp $
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

/**
**	Return goods to gold/wood deposit.
**
**	@param unit	pointer to unit.
*/
global void HandleActionReturnGoods(Unit* unit)
{
    int type;
    Unit* destu;

    //
    //	Select target to return goods.
    //
    type=unit->Type->Type;
    if( type==UnitPeonWithGold || type==UnitPeasantWithGold ) {
	if( !(destu=FindGoldDeposit(unit->Player,unit->X,unit->Y)) ) {
	    unit->Command.Action=UnitActionStill;
	    return;
	}
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
	unit->SubAction=65;
	DebugLevel3("Wait: %d\n",unit->Wait);
	unit->Wait=1;
	return;
    }

    if( type==UnitPeonWithWood || type==UnitPeasantWithWood ) {
	if( !(destu=FindWoodDeposit(unit->Player,unit->X,unit->Y)) ) {
	    unit->Command.Action=UnitActionStill;
	    return;
	}
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
	DebugLevel3("Return to %Zd=%d,%d\n"
	    ,destu-UnitsPool
	    ,unit->Command.Data.Move.DX
	    ,unit->Command.Data.Move.DY);
	unit->Command.Action=UnitActionHarvest;
	unit->SubAction=2;
	DebugLevel3("Wait: %d\n",unit->Wait);
	unit->Wait=1;
	return;
    }

    if( type==UnitTankerHumanFull || type==UnitTankerOrcFull ) {
	if( !(destu=FindOilDeposit(unit->Player,unit->X,unit->Y)) ) {
	    unit->Command.Action=UnitActionStill;
	    return;
	}
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
	DebugLevel3("Return to %Zd=%d,%d\n"
	    ,destu-UnitsPool
	    ,unit->Command.Data.Move.DX
	    ,unit->Command.Data.Move.DY);
	unit->Command.Action=UnitActionHaulOil;
	unit->SubAction=2;
	DebugLevel3("Wait: %d\n",unit->Wait);
	unit->Wait=1;
	return;
    }
}

//@}
