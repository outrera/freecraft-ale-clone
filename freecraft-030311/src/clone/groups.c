//   \_	  _____/______   ____   ____ \_   ___ \____________ _/ ____\/  |_
//    |    __) \_  __ \_/ __ \_/ __ \/    \  \/\_  __ \__  \\   __\\   __|
//    |     \   |  | \/\  ___/\  ___/\     \____|  | \// __ \|  |   |  |
//    \___  /   |__|    \___  >\___  >\______  /|__|  (____  /__|   |__|
//	  \/		    \/	   \/	     \/		   \/
//  ______________________                           ______________________
//			  T H E   W A R   B E G I N S
//	   FreeCraft - A free fantasy real time strategy game engine
//
/**@name groups.c	-	The units' groups handling. */
//
//	(c) Copyright 1999-2002 by Patrice Fortier and Lutz Sammer
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
//	$Id: groups.c,v 1.16 2002/12/17 06:40:41 nehalmistry Exp $

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freecraft.h"
#include "unit.h"
#include "ccl.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	Defines a group of units.
*/
typedef struct _unit_group_ {
    Unit*	Units[NUM_UNITS_PER_GROUP];	/// Units in the group
    int		NumUnits;			/// How many units in the group
} UnitGroup;				/// group of units

global UnitGroup Groups[NUM_GROUPS];	/// Number of groups predefined

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Initialize group part.
**
**	@todo Not needed with the new unit code!
*/
global void InitGroups(void)
{
    int i;

    for( i=0; i<NUM_GROUPS; i++ ) {
	int n;

	if( (n=Groups[i].NumUnits) ) {		// Cleanup after load
	    while( n-- ) {
		DebugLevel0Fn("FIXME: old code!\n");
		Groups[i].Units[n]=UnitSlots[(int)Groups[i].Units[n]];
	    }
	}
    }
}

/**
**	Save groups.
**
**	@param file	Output file.
*/
global void SaveGroups(FILE* file)
{
    int i;
    int g;
    char* ref;

    fprintf(file,"\n;;; -----------------------------------------\n");
    fprintf(file,";;; MODULE: groups $Id: groups.c,v 1.16 2002/12/17 06:40:41 nehalmistry Exp $\n\n");

    for( g=0; g<NUM_GROUPS; g++ ) {
	fprintf(file,"(group %d %d '(",g,Groups[g].NumUnits);
	for( i=0; i<Groups[g].NumUnits; ++i ) {
	    ref=UnitReference(Groups[g].Units[i]);
	    fprintf(file,"%s ",ref);
	    free(ref);
	}
	fprintf(file,"))\n");
    }
}

/**
**	Clean up group part.
*/
global void CleanGroups(void)
{
    int i;

    for( i=0; i<NUM_GROUPS; i++ ) {
        memset(&Groups[i],0,sizeof(Groups[i]));
    }
}

/**
 **	Return the number of units of group #num
 **
 **	@param num	Group number.
 **	@return		Returns the number of units in the group.
 */
global int GetNumberUnitsOfGroup(int num)
{
    return Groups[num].NumUnits;
}

/**
 **	Return the units of group #num
 **
 **	@param num	Group number.
 **	@return		Returns an array of all units in the group.
 */
global Unit** GetUnitsOfGroup(int num)
{
    return Groups[num].Units;
}

/**
 **	Clear contents of group #num
 **
 **	@param num	Group number.
 */
global void ClearGroup(int num)
{
    UnitGroup *group;
    int i;

    group=&Groups[num];
    for( i=0; i<group->NumUnits; i++ ) {
	group->Units[i]->GroupId &= ~(1<<num);
	DebugCheck( group->Units[i]->Destroyed );
    }
    group->NumUnits=0;
}

/**
 **	Add units to group #num contents from unit array "units"
 **
 **	@param units	Array of units to place into group.
 **	@param nunits	Number of units in array.
 **	@param num	Group number for storage.
 */
global void AddToGroup(Unit **units,int nunits,int num)
{
    UnitGroup *group;
    int i;

    DebugCheck(num>NUM_GROUPS);

    group=&Groups[num];
    for( i=0; group->NumUnits<NUM_UNITS_PER_GROUP && i<nunits; i++ ) {
        group->Units[group->NumUnits++]=units[i];
	units[i]->GroupId |= (1<<num);
    }
}

/**
 **	Set group #num contents to unit array "units"
 **
 **	@param units	Array of units to place into group.
 **	@param nunits	Number of units in array.
 **	@param num	Group number for storage.
 */
global void SetGroup(Unit **units,int nunits,int num)
{
    DebugCheck(num>NUM_GROUPS || nunits>NUM_UNITS_PER_GROUP);

    ClearGroup(num);
    AddToGroup(units,nunits,num);
}

/**
 **	Remove unit from its groups
 **
 **	@param unit	Unit to remove from group.
 */
global void RemoveUnitFromGroups(Unit *unit)
{
    UnitGroup *group;
    int num;
    int i;

    DebugCheck( unit->GroupId==0 );	// unit doesn't belong to a group

    for( num=0; unit->GroupId; num++,unit->GroupId>>=1 ) {
	if( (unit->GroupId & 1) != 1 ) {
	    continue;
	}

	group=&Groups[num];
	for( i=0; group->Units[i]!=unit; i++ ) {
	    ;
	}

	DebugCheck( i>=group->NumUnits );	// oops not found

	// This is a clean way that will allow us to add a unit
	// to a group easily, or make an easy array walk...
	if( i<--group->NumUnits ) {
	    group->Units[i]=group->Units[group->NumUnits];
	}
    }
}

// ----------------------------------------------------------------------------

/**
**	Define the group.
**
**	@param group	Group number
**	@param num	Number of units in group
**	@param units	Units in group
*/
local SCM CclGroup(SCM group,SCM num,SCM units)
{
    int i;
    UnitGroup* grp;

    grp=&Groups[gh_scm2int(group)];
    grp->NumUnits=gh_scm2int(num);
    i=0;
    while( !gh_null_p(units) ) {
	char* str;

	str=gh_scm2newstr(gh_car(units),NULL);
	grp->Units[i++]=(Unit*)strtol(str+1,NULL,16);
	free(str);
	units=gh_cdr(units);
    }

    return SCM_UNSPECIFIED;
}

/**
**	Register CCL features for groups.
*/
global void GroupCclRegister(void)
{
    gh_new_procedure3_0("group",CclGroup);
}

//@}
