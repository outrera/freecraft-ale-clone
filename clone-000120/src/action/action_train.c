/*
**	A clone of a famous game.
*/
/**@name action_train.c -	The building train action. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: action_train.c,v 1.15 2000/01/03 02:30:29 root Exp $
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
#include "missile.h"
#include "sound.h"
#include "ai.h"
#include "interface.h"

/**
**	Unit trains unit!
**
**	@param unit	Unit that trains.
*/
global void HandleActionTrain(Unit* unit)
{
    Unit* nunit;
    UnitType* type;
    Player* player;

#if 0
    // JOHNS: should be checked by the user-interface
    if( &Players[unit->Player]==ThisPlayer ) {
	// FIXME: If so used you get millions of messages.
	if( ThisPlayer->Food<=ThisPlayer->Units
		&& unit->Command.Data.Train.Ticks ) {
	    SetMessage( "You need more farms!" );
	} else {
	    AiNeedMoreFarms(unit);
	}
    }
#endif

    player=unit->Player;
    unit->Command.Data.Train.Ticks+=SpeedTrain;
    // FIXME: Should count down
    if( unit->Command.Data.Train.Ticks
	    >=unit->Command.Data.Train.What[0]
		->Stats[player->Player].Costs[TimeCost] ) {

	//
	//	Check if enough food available.
	//
	if( player->Food<=player->NumUnits ) {

	    // FIXME: GameMessage
	    if( player==ThisPlayer ) {
		// FIXME: PlayVoice :), see task.txt
		SetMessage("You need more farms!");
	    } else {
		// FIXME: Callback for AI!
		// AiNeedMoreFarms(unit);
	    }

	    unit->Command.Data.Train.Ticks-=SpeedTrain;
	    unit->Reset=1;
	    unit->Wait=FRAMES_PER_SECOND/6;
	    return;
	}

	nunit=MakeUnit(&UnitTypes[unit->Command.Data.Train.What[0]->Type]
		,player);
	nunit->X=unit->X;
	nunit->Y=unit->Y;
	type=unit->Type;
	DropOutOnSide(nunit,HeadingW,type->TileWidth,type->TileHeight);

	// FIXME: GameMessage
	if( player==ThisPlayer ) {
	    SetMessage("Training complete");
	    PlayUnitSound(nunit,VoiceReady);
	} else {
	    AiTrainingComplete(unit,nunit);
	}

	unit->Reset=1;
	unit->Wait=1;
        
	if ( --unit->Command.Data.Train.Count ) {
	    int z;
	    for( z = 0; z < MAX_UNIT_TRAIN-1; z++ ) {
		unit->Command.Data.Train.What[z] =
			unit->Command.Data.Train.What[z+1];
	    }
	    unit->Command.Data.Train.Ticks=0;
	} else {
	    unit->Command.Action=UnitActionStill;
	}

	nunit->Command=unit->PendCommand;

	if( IsSelected(unit) ) {
	    UpdateBottomPanel();
	    MustRedraw|=RedrawPanels;
	}

	return;
    }

    if( IsSelected(unit) ) {
	MustRedraw|=RedrawTopPanel;
    }

    unit->Reset=1;
    unit->Wait=FRAMES_PER_SECOND/6;
}

//@}
