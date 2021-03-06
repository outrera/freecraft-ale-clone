/*
**	A clone of a famous game.
*/
/**@name unit_find.c	-	The find/select for units. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: unit_find.c,v 1.17 1999/11/21 21:59:58 root Exp $
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
#include "interface.h"
#include "tileset.h"
#include "map.h"

/**
**	Select units in range.
**
**	@param x1	Left column of selection rectangle
**	@param y1	Top row of selection rectangle
**	@param x2	Right column of selection rectangle
**	@param y2	Bottom row of selection rectangle
**	@param table	All units in the selection rectangle
**
**	@return		Returns the number of units found
*/
global int SelectUnits(int x1,int y1,int x2,int y2,Unit** table)
{
    return UnitCacheSelect(x1,y1,x2,y2,table);
}

/**
**	Find all units of type.
**
**	@param type	type of unit requested
**	@param table	table in which we have to store the units
**
**	@return		Returns the number of units found.
*/
global int FindUnitsByType(int type,Unit** table)
{
    Unit* unit;
    int num,i;

    for( num=0, i=0; i<NumUnits; i++ ) {
	unit=Units[i];
	if( unit->Type->Type==type && !UnitUnusable(unit) ) {
	    table[num++]=unit;
	}
    }
    return num;
}

/**
**	Find all units of type.
**
**	@param player	we're looking for the units of this player
**	@param type	type of unit requested
**	@param table	table in which we have to store the units
**
**	@return		Returns the number of units found.
*/
global int FindPlayerUnitsByType(const Player* player,int type,Unit** table)
{
    Unit* unit;
    Unit** units;
    int num,nunits,i;

    nunits=player->TotalNumUnits;
    units=player->Units;
    // FIXME: Can't abort if all units are found: UnitTypeCount
    for( num=0,i=0; i<nunits; i++ ) {
	unit=units[i];
	if( unit->Type->Type==type && !UnitUnusable(unit) ) {
	    table[num++]=unit;
	}
    }
    return num;
}

/**
**	Unit on map tile, no special prefered.
**
**	@param tx	X position on map, tile-based.
**	@param ty	Y position on map, tile-based.
**
**	@return		Returns first found unit on tile.
*/
global Unit* UnitOnMapTile(unsigned tx,unsigned ty)
{
    Unit* table[MAX_UNITS];
    int n;
    int i;

    n=SelectUnits(tx,ty,tx+1,ty+1,table);
    for( i=0; i<n; ++i ) {
        // Note: this is less restrictive than UnitActionDie...
        // Is it normal?
	if( table[i]->Type->Vanishes ) {
	    continue;
	}
	return table[i];
    }

    return NoUnitP;
}

/**
**	Choose target on map tile.
**
**	@param source	Unit which want to attack.
**	@param tx	X position on map, tile-based.
**	@param ty	Y position on map, tile-based.
**
**	@return		Returns ideal target on map tile.
*/
global Unit* TargetOnMapTile(Unit* source,unsigned tx,unsigned ty)
{
    Unit* unit;
    UnitType* type;
    int i;

    // FIXME: This can be later rewritten.

    for( i=0; i<NumUnits; i++ ) {
	unit=Units[i];
	// unusable unit ?
	// if( UnitUnusable(unit) ) {
	if( unit->Removed || unit->Command.Action==UnitActionDie ) {
	    continue;
	}
	type=unit->Type;
	if( tx<unit->X || tx>=unit->X+type->TileWidth
		|| ty<unit->Y || ty>=unit->Y+type->TileHeight ) {
	    continue;
	}
	if( !CanTarget(source->Type,unit->Type) ) {
	    continue;
	}
	return unit;
    }
    return NoUnitP;
}

/*----------------------------------------------------------------------------
--	Finding special units
----------------------------------------------------------------------------*/

/**
**	Gold mine on map tile
**
**	@param tx	X position on map, tile-based.
**	@param ty	Y position on map, tile-based.
**
**	@return		Returns the gold mine if found, or NoUnitP.
*/
global Unit* GoldMineOnMap(int tx,int ty)
{
    Unit* table[MAX_UNITS];
    int i;
    int n;

    n=SelectUnits(tx,ty,tx+1,ty+1,table);
    for( i=0; i<n; ++i ) {
	if( table[i]->Type->GoldMine ) {
	    return table[i];
	}
    }
    return NoUnitP;
}

/**
**	Oil patch on map tile
**
**	@param tx	X position on map, tile-based.
**	@param ty	Y position on map, tile-based.
**
**	@return		Returns the oil patch if found, or NoUnitP.
*/
global Unit* OilPatchOnMap(int tx,int ty)
{
    Unit* table[MAX_UNITS];
    int i;
    int n;

    n=SelectUnits(tx,ty,tx+1,ty+1,table);
    for( i=0; i<n; ++i ) {
	if( table[i]->Type->OilPatch ) {
	    return table[i];
	}
    }
    return NoUnitP;
}

/**
**	Oil platform on map tile
**
**	@param tx	X position on map, tile-based.
**	@param ty	Y position on map, tile-based.
**
**	@return		Returns the oil platform if found, or NoUnitP.
*/
global Unit* PlatformOnMap(int tx,int ty)
{
    Unit* table[MAX_UNITS];
    int i;
    int n;

    n=SelectUnits(tx,ty,tx+1,ty+1,table);
    for( i=0; i<n; ++i ) {
	if( table[i]->Type->GivesOil ) {
	    return table[i];
	}
    }
    return NoUnitP;
}

/**
**	Oil deposit on map tile
**
**	@param tx	X position on map, tile-based.
**	@param ty	Y position on map, tile-based.
**
**	@return		Returns the oil deposit if found, or NoUnitP.
*/
global Unit* OilDepositOnMap(int tx,int ty)
{
    Unit* table[MAX_UNITS];
    int i;
    int n;

    n=SelectUnits(tx,ty,tx+1,ty+1,table);
    for( i=0; i<n; ++i ) {
	if( table[i]->Type->StoresOil ) {
	    return table[i];
	}
    }
    return NoUnitP;
}

/*----------------------------------------------------------------------------
--	Finding units for attack
----------------------------------------------------------------------------*/

/**
**	Attack units in distance.
**
**		If the unit can attack must be handled by caller.
**
**	@param unit	Find in distance for this unit.
**	@param range	Distance range to look.
**
**	@return		Unit to be attacked.
*/
global Unit* AttackUnitsInDistance(Unit* unit,int range)
{
    Unit* dest;
    UnitType* type;
    UnitType* dtype;
    Unit* table[MAX_UNITS];
    int n;
    int i;
    const Player* player;
    Unit* best_unit;
    int best_priority;
    int best_hp;

    n=SelectUnits(unit->X-range,unit->Y-range
	    ,unit->X+range+1,unit->Y+range+1,table);

    best_unit=NoUnitP;
    best_priority=0;
    best_hp=99999;

    player=unit->Player;
    type=unit->Type;
    for( i=0; i<n; ++i ) {
	dest=table[i];
	// unusable unit
	/*
	if( UnitUnusable(dest) ) {
	    continue;
	}
	*/
	if( dest->Removed || dest->Command.Action==UnitActionDie ) {
	    continue;
	}

	dtype=dest->Type;
	if( !IsEnemy(player,dest) ) {		// a friend
	    continue;
	}
	if( !dest->HP ) {
	    DebugLevel0(__FUNCTION__": HP==0\n");
	    continue;
	}

	if( MapDistanceToUnit(unit->X,unit->Y,dest)>range ) {
	    DebugLevel0("Internal error: %d - %d `%s'\n"
		    ,MapDistanceToUnit(unit->X,unit->Y,dest)
		    ,range
		    ,dest->Type->Name);
	    continue;
	}

	// Check if I can attack this unit.
	DebugLevel3("Can attack unit %p <- %p\n",dest,unit);
	if( CanTarget(type,dtype) ) {
	    DebugLevel3("Can target unit %p <- %p\n",dest,unit);
	    if( best_priority<dtype->Priority ) {
		best_priority=dtype->Priority;
		best_hp=dest->HP;
		best_unit=dest;
		DebugLevel3("Higher priority\n");
	    } else if( best_priority==dtype->Priority ) {
		if( best_hp>dest->HP ) {
		    best_hp=dest->HP;
		    best_unit=dest;
		    DebugLevel3("Less hit-points\n");
		}
	    }
	}
    }

    if( best_unit ) {
	return best_unit;
    }

    return NoUnitP;
}

/**
**	Attack units in attack range.
**
**	@param unit	Find unit in attack range for this unit.
**
**	@return		Pointer to unit which should be attacked.
*/
global Unit* AttackUnitsInRange(Unit* unit)
{
    //
    //	Only units which can attack.
    //
    IfDebug(
	
	if( !unit->Type->CanAttack && !unit->Type->Tower ) {
	    DebugLevel0("Should be handled by caller?\n");
	    abort();
	    return NoUnitP;
	}
    );

    return AttackUnitsInDistance(unit,unit->Stats->AttackRange);
}

/**
**	Attack units in reaction range.
**
**	@param unit	Find unit in reaction range for this unit.
**
**	@return		Pointer to unit which should be attacked.
*/
global Unit* AttackUnitsInReactRange(Unit* unit)
{
    int range;
    UnitType* type;

    //
    //	Only units which can attack.
    //
    type=unit->Type;
    IfDebug(
	if( !type->CanAttack && !type->Tower ) {
	    DebugLevel0("Should be handled by caller?\n");
	    abort();
	    return NoUnitP;
	}
    );

    if( unit->Player->Type==PlayerHuman ) {
	range=type->ReactRangeHuman;
    } else {
	range=type->ReactRangeComputer;
    }

    return AttackUnitsInDistance(unit,range);
}

//@}
