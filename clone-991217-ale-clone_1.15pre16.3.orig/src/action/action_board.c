/*
**	A clone of a famous game.
*/
/**@name action_board.c		-	The board action. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: action_board.c,v 1.8 1999/11/21 22:00:11 root Exp $
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
#include "interface.h"

/**
**	Move to transporter.
**
**	@param unit	Pointer to unit.
**	@return		-1 if unreachable, True if reached, False otherwise.
*/
local int MoveToTransporter(Unit* unit)
{
    int i;

    if( !(i=HandleActionMove(unit)) ) {	// reached end-point
	return 0;
    }
    DebugLevel3(__FUNCTION__" result: %d\n",i);
    if( i==-1 ) {
	DebugLevel3("SHIP NOT REACHED\n");
	return -1;
    }

    unit->Command.Action=UnitActionBoard;
    return 1;
}

/**
**	Wait for transporter.
**
**	@param unit	Pointer to unit.
**	@return		True if ship arrived/present, False otherwise.
*/
local int WaitForTransporter(Unit* unit)
{
#if 0
    Unit* table[MAX_UNITS];
    int i;
    int n;
    int x;
    int y;

    unit->Wait=1;
    unit->Reset=1;

    x=unit->X;
    y=unit->Y;
    n=SelectUnits(x-1,y-1,x+1,y+1,table);	// limit checks needed?
    DebugLevel2("Found %d surrounding\n",n);
    for( i=0; i<n; ++i ) {
	if( table[i]->Type->Transporter
		&& &Players[table[i]->Player]==ThisPlayer ) {
	    // FIXME: space in transporter? correct transporter?
	    DebugLevel0("Enter transporter\n");
	    unit->Command.Data.Move.Goal=table[i];
	    return 1;
	}
    }
#endif
    Unit* trans;

    unit->Wait=1;
    unit->Reset=1;

    trans=unit->Command.Data.Move.Goal;
    // FIXME: destination destroyed??
    if( !trans || !trans->Type->Transporter ) {
	DebugLevel2("TRANSPORTER NOT REACHED %d,%d\n",unit->X,unit->Y);
	return 0;
    }

    if( MapDistanceToUnit(unit->X,unit->Y,trans)==1 ) {
	DebugLevel3("Enter transporter\n");
	return 1;
    }

    return 0;
}

/**
**	Enter the transporter.
**
**	@param unit	Pointer to unit.
*/
local void EnterTransporter(Unit* unit)
{
    Unit* transporter;
    int i;

    unit->Wait=1;
    unit->Command.Action=UnitActionStill;
    unit->SubAction=0;

    transporter=unit->Command.Data.Move.Goal;
    //
    //	Find free slot in transporter.
    //
    for( i=0; i<sizeof(unit->OnBoard)/sizeof(*unit->OnBoard); ++i ) {
	if( transporter->OnBoard[i]==NoUnitP ) {
	    transporter->OnBoard[i]=unit;
	    transporter->Value++;
	    RemoveUnit(unit);
	    if( IsSelected(transporter) ) {
		UpdateBottomPanel();
		MustRedraw|=RedrawPanels;
	    }
	    return;
	}
    }
    DebugLevel0("No free slot in transporter\n");
}

/**
**	The unit boards a transporter.
**
**	@param unit	Pointer to unit.
*/
global void HandleActionBoard(Unit* unit)
{
    int i;

    DebugLevel3(__FUNCTION__": %p(%Zd) SubAction %d\n"
		,unit,unit-UnitsPool,unit->SubAction);

    switch( unit->SubAction ) {
	//
	//	Wait for transporter
	//
	case 101:
	    // FIXME: show still animations
	    DebugLevel3("Waiting\n");
	    if( WaitForTransporter(unit) ) {
		unit->SubAction=102;
	    }
	    break;
	//
	//	Enter transporter
	//
	case 102:
	    EnterTransporter(unit);
	    break;
	//
	//	Move to transporter
	//
        default:
	    if( unit->SubAction<=100 ) {
		if( (i=MoveToTransporter(unit)) ) {
		    if( i==-1 ) {
			if( ++unit->SubAction==100 ) {
			    unit->Command.Action=UnitActionStill;
			    unit->SubAction=0;
			}
		    } else {
			unit->SubAction=101;
		    }
		}
	    }
	    break;
    }
}

//@}
