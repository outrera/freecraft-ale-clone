/*
**	A clone of a famous game.
*/
/**@name botpanel.c	-	The bottom panel. */
/*
**	(c) Copyright 1999 by Lutz Sammer, Vladi Belperchinov-Shabanski
**
**	$Id: botpanel.c,v 1.36 2000/01/03 02:30:32 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "clone.h"

// FIXME: Check if all are needed?
#include "video.h"
#include "icons.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "upgrade_structs.h"
#include "upgrade.h"
#include "interface.h"
#include "image.h"
#include "missile.h"
#include "sound.h"
#include "actions.h"
#include "cursor.h"
#include "tileset.h"
#include "map.h"
#include "unit.h"
#include "network.h"

/*----------------------------------------------------------------------------
--      Defines
----------------------------------------------------------------------------*/

	/// How many different buttons are allowed
#define MAX_BUTTONS	2048

/*----------------------------------------------------------------------------
--      Variables
----------------------------------------------------------------------------*/

	/// for unit buttons sub-menus etc.
global int CurrentButtonLevel;

	/// All buttons for units
local ButtonAction *UnitButtonTable[MAX_BUTTONS];
	/// buttons in UnitButtonTable
local int UnitButtonCount;

/*----------------------------------------------------------------------------
--      Functions
----------------------------------------------------------------------------*/

/**
**	Add buttons table.
*/
local void AddButtonTable(const ButtonAction* button)
{
    for( ;button->Pos; ++button ) {
	AddButton(button->Pos,button->Level,button->Icon.Name
	    ,button->Action,button->ValueStr,button->Allowed
	    ,button->AllowStr,button->Key,button->Hint,button->UMask);
    }
}

/**
**	Initialize the buttons.
*/
global void InitButtons(void)
{
    UnitButtonCount = 0;

    //
    //	Add all pre-defined buttons.
    //
    AddButtonTable(AllButtons);
    // FIXME: AddButtonTable(ExtensionButtons);
}
















/*----------------------------------------------------------------------------
--      Buttons structures
----------------------------------------------------------------------------*/

global ButtonAction* CurrentButtons;
global ButtonAction  _current_buttons[9]; //FIXME: this is just for test

int AddButton( int pos, int level, const char* IconIdent,
	enum _button_cmd_ action, const char* value,
	const void* func, const void* allow,
	int key, const char* hint, const char* umask )
{
  char buf[2048];
  ButtonAction* ba = (ButtonAction*)malloc(sizeof(ButtonAction));
  DebugCheck(!ba); //FIXME: perhaps should return error?
  ba->Pos = pos;
  ba->Level = level;
  ba->Icon.Name = (char*)IconIdent;
  ba->Icon.Icon = IconByIdent( IconIdent );
  ba->Action = action;
  if( value ) {
      ba->ValueStr = strdup( value );
      switch( action )
	{
	case B_Magic:	 ba->Value = UpgradeIdByIdent( value ); break;
	case B_Train:	 ba->Value = UnitTypeIdByIdent( value ); break;
	case B_Research: ba->Value = UpgradeIdByIdent( value ); break;
	case B_UpgradeTo: ba->Value = UnitTypeIdByIdent( value ); break;
	case B_Build:	 ba->Value = UnitTypeIdByIdent( value ); break;
	default:	ba->Value = atoi( value ); break;
	}
  } else {
      ba->ValueStr = NULL;
      ba->Value = 0;
  }

  ba->Allowed = func;
  if( allow ) {
      ba->AllowStr=strdup(allow);
  } else {
      ba->AllowStr=NULL;
  }

  ba->Key = key;
  ba->Hint = strdup( hint );
  //FIXME: here should be added costs to the hint
  //FIXME: johns: show should be nice done?
  if ( umask[0] == '*' )
    strcpy( buf, umask );
  else
    sprintf( buf, ",%s,", umask );
  ba->UMask = strdup( buf );
  UnitButtonTable[UnitButtonCount++] = ba;

  DebugCheck( ba->Icon.Icon==-1 );	// just checks, that's why at the end
  return 1;
};


global void DoneButtons(void)
{
    int z;

    for ( z = 0; z < UnitButtonCount; z++ ) {
	DebugCheck( !UnitButtonTable[z] );
	free( UnitButtonTable[z]->ValueStr );
	free( UnitButtonTable[z]->Hint );
	free( UnitButtonTable[z]->UMask );
	free( UnitButtonTable[z] );
    }
    UnitButtonCount = 0;
};

/**
**	Draw bottom panel.
*/
global void DrawBottomPanel(void)
{
    int i;
    int v;
    const UnitStats* stats;
    const ButtonAction* buttons;

    DrawImage(ImagePanelBottom,0,0,0,336);

    if( !(buttons=CurrentButtons) ) {	// no buttons
	return;
    }

    for( i=0; i<9; ++i ) {
	if( buttons[i].Pos!=-1 ) {
	    // cursor is on that button
	    if( ButtonUnderCursor==i+10 ) {
		v=IconActive;
		if( MouseButtons&LeftButton ) {
		    v=IconClicked;
		}
	    } else {
		v=0;
	    }
	    //
	    //	Any better ideas?
	    //	Show the current action state of the unit
	    //	with the buttons.
	    //
	    if( NumSelected==1 ) {
		switch( buttons[i].Action ) {
		    case B_Stop:
			if( Selected[0]->Command.Action==UnitActionStill ) {
			    v=IconSelected;
			}
			break;
		    case B_StandGround:
			if( Selected[0]->Command.Action
				==UnitActionStandGround ) {
			    v=IconSelected;
			}
			break;
		    case B_Move:
			if( Selected[0]->Command.Action==UnitActionMove ) {
			    v=IconSelected;
			}
			break;
		    case B_Attack:
			if( Selected[0]->Command.Action==UnitActionAttack ) {
			    v=IconSelected;
			}
			break;
		    case B_Demolish:
			if( Selected[0]->Command.Action==UnitActionDemolish ) {
			    v=IconSelected;
			}
			break;
/* FIXME: attackground and attack are both ActionAttack
		    case B_AttackGround:
			if( Selected[0]->Command.Action==UnitActionAttack ) {
			    v=IconSelected;
			}
			break;
*/

		    // FIXME: must handle more actions

		    default:
			break;
		}
	    }

	    DrawUnitIcon(buttons[i].Icon.Icon
		    ,v,Buttons[i+10].X,Buttons[i+10].Y);

	    //
	    //	Update status line for this button
	    //
	    if( ButtonUnderCursor==i+10 ) {
		SetStatusLine(buttons[i].Hint);
		// FIXME: Draw costs
		v=buttons[i].Value;
		switch( buttons[i].Action ) {
		    case B_Build:
		    case B_Train:
		    case B_UpgradeTo:
			stats=&UnitTypes[v].Stats[ThisPlayer->Player];
			SetCosts(0,stats->Costs[GoldCost]
				,stats->Costs[WoodCost]
				,stats->Costs[OilCost]);
			break;
		    //case B_Upgrade:
		    case B_Research:
			SetCosts(0
				/*Upgrades[v].Costs[ManaCost]*/
				,Upgrades[v].Costs[GoldCost]
				,Upgrades[v].Costs[WoodCost]
				,Upgrades[v].Costs[OilCost]);
			break;
		    case B_Magic:
			// FIXME: correct costs!!!
			SetCosts(11 ,0,0,0);
			break;

		    default:
			ClearCosts();
			break;
		}
	    }
	}
    }
}

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Update bottom panel for multiple units.
*/
local void UpdateBottomPanelMultipleUnits(void)
{
    const char* unit_ident;
    int z;
    int i;

    // first clear the table
    for ( z = 0; z < 9; z++ ) {
	_current_buttons[z].Pos = -1;
    }

    IfDebug( unit_ident=""; );		// keep the compiler happy

    // when we have more races this should become a function
    switch( ThisPlayer->Race ) {
	case PlayerRaceHuman: unit_ident=",human-group,"; break;
	case PlayerRaceOrc: unit_ident=",orc-group,"; break;
	case PlayerRaceNeutral: unit_ident=",neutral-group,"; break;
	default: DebugLevel0("what %d ",ThisPlayer->Race); abort();
    }

    for( z = 0; z < UnitButtonCount; z++ ) {
	if ( UnitButtonTable[z]->Level != CurrentButtonLevel ) {
	    continue;
	}

	// any unit or unit in list
	if ( UnitButtonTable[z]->UMask[0] != '*'
		&& strstr( UnitButtonTable[z]->UMask, unit_ident ) ) {
	    int allow;

	    allow=0;
	    DebugLevel3("%d: %p\n",z,UnitButtonTable[z]->Allowed);
	    if ( UnitButtonTable[z]->Allowed ) {
		// there is check function -- call it
		if (UnitButtonTable[z]->Allowed( NULL, UnitButtonTable[z] )) {
		    allow = 1;
		}
	    } else {
		// there is not allow function -- should check dependencies
		// any unit of the group must have this feature
		if ( UnitButtonTable[z]->Action == B_Attack ) {
		    for( i=NumSelected; --i; ) {
			if( Selected[i]->Type->CanAttack ) {
			    allow = 1;
			    break;
			}
		    }
		} else if ( UnitButtonTable[z]->Action == B_AttackGround ) {
		    for( i=NumSelected; --i; ) {
			if( Selected[i]->Type->GroundAttack ) {
			    allow = 1;
			    break;
			}
		    }
		} else if ( UnitButtonTable[z]->Action == B_Demolish ) {
		    for( i=NumSelected; --i; ) {
			if( Selected[i]->Type->Explodes ) {
			    allow = 1;
			    break;
			}
		    }
		} else {
		    allow = 1;
		}
	    }

	    if (allow) {		// is button allowed after all?
		_current_buttons[UnitButtonTable[z]->Pos-1]
			= (*UnitButtonTable[z]);
	    }
	}
    }

    CurrentButtons = _current_buttons;
    MustRedraw|=RedrawBottomPanel;
}

/**
**	Update bottom panel.
*/
global void UpdateBottomPanel(void)
{
    Unit* unit;
    char unit_ident[128];
    int z;

    CurrentButtons=NULL;

    if( !NumSelected ) {		// no unit selected
	return;
    }

    if( NumSelected>1 ) {		// multiple selected
        int at;

        for ( at=z = 1; z < NumSelected; z++ ) {
	    // if current type is equal to first one count it
            if ( Selected[z]->Type == Selected[0]->Type ) {
               at++;
	    }
	}

	if ( at != NumSelected ) {
	    // oops we have selected different units types
	    // -- set default buttons and exit
	    UpdateBottomPanelMultipleUnits();
	    return;
	}
	// we have same type units selected
	// -- continue with setting buttons as for the first unit
    }

    unit=Selected[0];
    DebugCheck( (unit==NoUnitP) );

    if( unit->Player!=ThisPlayer ) {	// foreign unit
	return;
    }

    // first clear the table
    for ( z = 0; z < 9; z++ ) {
	_current_buttons[z].Pos = -1;
    }

    //
    //	FIXME: johns: some hacks for cancel buttons
    //
    /* if( CursorBuilding ) {
	strcpy(unit_ident,",cancel-build,");
    // FIXME: johns: my ->Constructed didn't seem ok. if (unit->Constructed)
    } else */
    if( unit->Command.Action==UnitActionBuilded ) {
	// Trick 17 to get the cancel-build button
	strcpy(unit_ident,",cancel-build,");
    } else if( unit->Command.Action==UnitActionUpgradeTo ) {
	// Trick 17 to get the cancel-upgrade button
	strcpy(unit_ident,",cancel-upgrade,");
    } else if( unit->Command.Action==UnitActionResearch ) {
	// Trick 17 to get the cancel-upgrade button
	strcpy(unit_ident,",cancel-upgrade,");
    } else {
	sprintf(unit_ident, ",%s,", unit->Type->Ident);
    }

    for( z = 0; z < UnitButtonCount; z++ ) {
	//FIXME: we have to check and if these unit buttons are available
	//       i.e. if button action is B_Train for example check if required
	//       unit is not restricted etc...
	if ( UnitButtonTable[z]->Level != CurrentButtonLevel ) {
	    continue;
	}
	// any unit or unit in list
	if ( UnitButtonTable[z]->UMask[0] == '*'
		|| strstr( UnitButtonTable[z]->UMask, unit_ident ) ) {
	    int allow;

	    allow=0;
	    DebugLevel3("%d: %p\n",z,UnitButtonTable[z]->Allowed);
	    if ( UnitButtonTable[z]->Allowed ) {
		// there is check function -- call it
		if (UnitButtonTable[z]->Allowed( unit, UnitButtonTable[z] )) {
		    allow = 1;
		}
	    } else {
		// there is not allow function -- should check dependencies
		if ( UnitButtonTable[z]->Action == B_Attack ) {
		    if( /*NumSelected==1 &&*/ Selected[0]->Type->CanAttack ) {
			allow = 1;
		    }
		} else if ( UnitButtonTable[z]->Action == B_AttackGround ) {
		    if( /*NumSelected==1 &&*/ Selected[0]->Type->GroundAttack ) {
			allow = 1;
		    }
		} else if ( UnitButtonTable[z]->Action == B_Demolish ) {
		    if( /*NumSelected==1 &&*/ Selected[0]->Type->Explodes ) {
			allow = 1;
		    }
		} else if ( UnitButtonTable[z]->Action == B_Train
			// || UnitButtonTable[z]->Action == B_Upgrade
			|| UnitButtonTable[z]->Action == B_UpgradeTo
			|| UnitButtonTable[z]->Action == B_Research
			|| UnitButtonTable[z]->Action == B_Build
			) {
		    allow = CheckDepend( ThisPlayer,
				UnitButtonTable[z]->ValueStr );
		    if ( allow && !strncmp( UnitButtonTable[z]->ValueStr,
				"upgrade-", 8 ) ) {
			    allow=UpgradeIdentAllowed( ThisPlayer,
				UnitButtonTable[z]->ValueStr )=='A';
		    }
		} else if ( UnitButtonTable[z]->Action == B_Magic ) {
		    DebugLevel3("Magic: %d,%c\n",
			    CheckDepend( ThisPlayer,
				UnitButtonTable[z]->ValueStr ),
			    UpgradeIdentAllowed( ThisPlayer,
				UnitButtonTable[z]->ValueStr ));
		    allow = CheckDepend( ThisPlayer,
				UnitButtonTable[z]->ValueStr )
			    && UpgradeIdentAllowed( ThisPlayer,
				UnitButtonTable[z]->ValueStr )=='R';
		} else if ( UnitButtonTable[z]->Action == B_Unload ) {
		    DebugLevel3("Unload: %d\n",Selected[0]->Value);
		    allow = Selected[0]->Value;
		} else {
		    // so we have NULL check function
		    // and button is not one of Train/Build/Research
		    allow = 1;
		}
	    }

	    if (allow) {		// is button allowed after all?
		_current_buttons[UnitButtonTable[z]->Pos-1]
			= (*UnitButtonTable[z]);
	    }
	}
    }

    CurrentButtons = _current_buttons;
    MustRedraw|=RedrawBottomPanel;
}

/*
**	Handle bottom button clicked.
*/
global void DoBottomButtonClicked(int button)
{
    int i;
    UnitType* type;
    const UnitStats* stats;

    DebugLevel3("Button clicked %d\n",button);

    if( !CurrentButtons ) {		// no buttons
	return;
    }

    //
    //	Button not available.
    //
    if( CurrentButtons[button].Pos==-1 ) {
	return;
    }

    PlayGameSound(GameSounds.Click.Sound,MaxSampleVolume);

    //
    //	Handle action on button.
    //
    switch( CurrentButtons[button].Action ) {
	case B_Unload:
	    if( NumSelected==1
		    && CoastOnMap(Selected[0]->X,Selected[0]->Y) ) {
		SendCommandUnload(Selected[0]
			,Selected[0]->X,Selected[0]->Y,NoUnitP);
		break;
	    }
	case B_Move:
	case B_Patrol:
	case B_Harvest:
	case B_Attack:
	case B_Repair:
	case B_AttackGround:
	case B_Demolish:
	    CursorState=CursorStateSelect;
	    CloneCursor=&Cursors[CursorTypeYellowHair];
	    CursorAction=CurrentButtons[button].Action;
            CurrentButtonLevel=9;	// level 9 is cancel-only
            UpdateBottomPanel();
	    MustRedraw|=RedrawCursor;
	    SetStatusLine("Select Target");
	    break;
	case B_Return:
	    for( i=0; i<NumSelected; ++i ) {
	        SendCommandReturnGoods(Selected[i]);
	    }
	    break;
	case B_Stop:
	    for( i=0; i<NumSelected; ++i ) {
	        SendCommandStopUnit(Selected[i]);
	    }
	    break;
	case B_StandGround:
	    for( i=0; i<NumSelected; ++i ) {
	        SendCommandStandGround(Selected[i]);
	    }
	    break;
	case B_Button:
            CurrentButtonLevel=CurrentButtons[button].Value;
            UpdateBottomPanel();
	    break;

	case B_Cancel:
	    if ( NumSelected==1
		    && Selected[0]->Type->Building ) {
		if( Selected[0]->Command.Action == UnitActionUpgradeTo ) {
		    type=Selected[0]->Command.Data.UpgradeTo.What;
		    stats=&type->Stats[ThisPlayer->Player];
		    PlayerAddCosts(ThisPlayer,
				   stats->Costs[GoldCost]*0.75,
				   stats->Costs[WoodCost]*0.75,
				   stats->Costs[OilCost]*0.75);
		    SendCommandCancelUpgradeTo(Selected[0]);
		} else
		if( Selected[0]->Command.Action == UnitActionResearch ) {
		    i=Selected[0]->Command.Data.Research.What;
		    PlayerAddCosts(ThisPlayer
			    ,Upgrades[i].Costs[GoldCost]*0.75
			    ,Upgrades[i].Costs[WoodCost]*0.75
			    ,Upgrades[i].Costs[OilCost]*0.75);
		    SendCommandCancelResearch(Selected[0]);
		}
	    }
	    DebugLevel0("Cancel\n");

	    ClearStatusLine();
	    ClearCosts();
            CurrentButtonLevel = 0;
	    UpdateBottomPanel();
	    CloneCursor=&Cursors[CursorTypePoint];
	    CursorBuilding=NULL;
	    CursorState=CursorStatePoint;
	    MustRedraw|=RedrawCursor;
	    break;

	case B_CancelTrain:
	    // FIXME: This didn't work in the network
	    DebugCheck( !Selected[0]->Command.Data.Train.Count );
	    PlayerAddResources(ThisPlayer
		,Selected[0]->Command.Data.Train.What[
		    Selected[0]->Command.Data.Train.Count-1]);
	    ClearStatusLine();
	    ClearCosts();
	    SendCommandCancelTraining(Selected[0]);
	    break;
	case B_CancelBuild:
	    // FIXME: johns is this not sure, only building should have this?
	    if( NumSelected==1 && Selected[0]->Type->Building) {
	        stats=Selected[0]->Stats;
		// Player gets back 75% of the original cost for a building.
		PlayerAddCosts(ThisPlayer,
			       stats->Costs[GoldCost]*0.75,
			       stats->Costs[WoodCost]*0.75,
			       stats->Costs[OilCost]*0.75);
		SendCommandCancelBuilding(Selected[0],
		        Selected[0]->Command.Data.Builded.Peon);
	    }
	    break;

	case B_Build:
	    type=&UnitTypes[CurrentButtons[button].Value];
	    if( PlayerCheckResources(ThisPlayer,type) ) {
		SetStatusLine("Select Location");
		ClearCosts();
		CursorBuilding=type;
                CurrentButtonLevel=9;	// level 9 is cancel-only
		UpdateBottomPanel();
		MustRedraw|=RedrawCursor;
	    }
	    break;
	case B_Train:
	    type=&UnitTypes[CurrentButtons[button].Value];
	    if( PlayerCheckResources(ThisPlayer,type)
		    && PlayerCheckFood(ThisPlayer,type) ) {
		PlayerSubResources(ThisPlayer,type);
		SendCommandTrainUnit(Selected[0],type);
		ClearStatusLine();
		ClearCosts();
		UpdateBottomPanel();
		MustRedraw|=RedrawTopPanel;
	    }
	    break;
	case B_UpgradeTo:
	    type=&UnitTypes[CurrentButtons[button].Value];
	    if( PlayerCheckResources(ThisPlayer,type) ) {
		PlayerSubResources(ThisPlayer,type);
		SendCommandUpgradeTo(Selected[0],type);
		ClearStatusLine();
		ClearCosts();
		UpdateBottomPanel();
		MustRedraw|=RedrawTopPanel;
	    }
	    break;
	case B_Research:
	//case B_Upgrade:
	    i=CurrentButtons[button].Value;
	    if( PlayerCheckCosts(ThisPlayer
		    ,Upgrades[i].Costs[GoldCost]
		    ,Upgrades[i].Costs[WoodCost]
		    ,Upgrades[i].Costs[OilCost]) ) {
		PlayerSubCosts(ThisPlayer
			,Upgrades[i].Costs[GoldCost]
			,Upgrades[i].Costs[WoodCost]
			,Upgrades[i].Costs[OilCost]);
		SendCommandResearch(Selected[0],i);
		ClearStatusLine();
		ClearCosts();
//		FIXME: ? Johns CurrentButtons=CancelUpgradeButtons;
		MustRedraw|=RedrawTopPanel;
	    }
	    break;
	default:
	    DebugLevel1(__FUNCTION__": Unknown action %d\n"
		    ,CurrentButtons[button].Action);
	    break;
    }
}

/**
**	Lookup key for bottom panel buttons.
**
**	@param key	Internal key symbol for pressed key.
*/
global void DoBottomPanelKey(int key)
{
    int i;

    if( !CurrentButtons ) {		// no buttons
	return;
    }

    // cade: this is required for action queues SHIFT+M should be `m'
    if ( key >= 'A' && key <= 'Z' ) {
	key = tolower(key);
    }

    for( i=0; i<9; ++i ) {
	if( CurrentButtons[i].Pos!=-1 && key==CurrentButtons[i].Key ) {
	    DoBottomButtonClicked(i);
	    return;
	}
    }
}

//@}
