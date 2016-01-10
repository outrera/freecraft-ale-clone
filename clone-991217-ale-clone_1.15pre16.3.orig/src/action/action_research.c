/*
**	A clone of a famous game.
*/
/**@name action_research.c -	The research action. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: action_research.c,v 1.8 1999/11/21 22:00:11 root Exp $
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
#include "upgrade_structs.h"
#include "upgrade.h"
#include "interface.h"

/*
**	Unit researches!
*/
global void HandleActionResearch(Unit* unit)
{
    int upgrade;

    DebugLevel3("Research %d\n",unit);
    upgrade=unit->Command.Data.Research.What;
    unit->Command.Data.Research.Ticks+=SpeedResearch;
    if( unit->Command.Data.Research.Ticks
	    >=Upgrades[upgrade].Costs[TimeCost] ) {

	// FIXME: should als speak and tell ai
	SetMessage("Upgrade complete");

	// NewUpgrade(upgrade);
        UpgradeAcquire(unit->Player,upgrade);

	unit->Reset=1;
	unit->Wait=1;
	unit->Command.Action=UnitActionStill;

	// Upgrade can change all
	UpdateBottomPanel();
	MustRedraw|=RedrawPanels;

	return;
    }

    if( IsSelected(unit) ) {
	MustRedraw|=RedrawTopPanel;
    }

    unit->Reset=1;
    unit->Wait=FRAMES_PER_SECOND/6;
    // FIXME: should animations here?
}

//@}
