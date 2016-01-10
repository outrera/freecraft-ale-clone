/*
**	A clone of a famous game.
*/
/**name action_upgradeto.c -	The unit upgrading to new action. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: action_upgradeto.c,v 1.12 1999/11/25 00:59:57 root Exp $
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
#include "ai.h"
#include "interface.h"

/**
**	Unit upgrades unit!
**
**	@param unit	Pointer to unit.
*/
global void HandleActionUpgradeTo(Unit* unit)
{
    Player* player;
    UnitType* type;
    const UnitStats* stats;

    DebugLevel3(__FUNCTION__": %Zd\n",unit-UnitsPool);

    player=unit->Player;
    type=unit->Command.Data.UpgradeTo.What;
    stats=&type->Stats[player->Player];

    unit->Command.Data.UpgradeTo.Ticks+=SpeedUpgrade;
    // FIXME: Should count down here
    if( unit->Command.Data.UpgradeTo.Ticks>=stats->Costs[TimeCost] ) {

	// FIXME: HP, I add the difference to the new unit?
	unit->HP+=stats->HitPoints-unit->Type->Stats[player->Player].HitPoints;
	// don't have such unit now
	player->UnitTypesCount[unit->Type->Type]--;
	unit->Type=type;
	unit->Stats=(UnitStats*)stats;
	// and we have new one...
	player->UnitTypesCount[unit->Type->Type]++;
	UpdateForNewUnit(unit,1);

	// FIXME: SendNotify("upgrade-complete");
	if( player==ThisPlayer ) {
	    SetMessage("Upgrade complete");
	} else {
	    // FIXME: AiUpgradeToComplete(unit,type);
	}
	unit->Reset=1;
	unit->Wait=1;
	unit->Command.Action=UnitActionStill;

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
