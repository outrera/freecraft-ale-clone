//   ___________		     _________		      _____  __
//   \_	  _____/______   ____   ____ \_   ___ \____________ _/ ____\/  |_
//    |    __) \_  __ \_/ __ \_/ __ \/    \  \/\_  __ \__  \\   __\\   __|
//    |     \   |  | \/\  ___/\  ___/\     \____|  | \// __ \|  |   |  |
//    \___  /   |__|    \___  >\___  >\______  /|__|  (____  /__|   |__|
//	  \/		    \/	   \/	     \/		   \/
//  ______________________                           ______________________
//			  T H E   W A R   B E G I N S
//	   FreeCraft - A free fantasy real time strategy game engine
//
/**@name unittype.c	-	The unit types. */
//
//	(c) Copyright 1998-2002 by Lutz Sammer
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
//	$Id: unittype.c,v 1.69 2002/12/22 15:19:42 ariclone Exp $

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "freecraft.h"
#include "video.h"
#include "tileset.h"
#include "map.h"
#include "sound_id.h"
#include "unitsound.h"
#include "construct.h"
#include "unittype.h"
#include "player.h"
#include "missile.h"
#include "ccl.h"

#include "etlib/hash.h"

#include "myendian.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	Unit-type type definition
*/
global const char UnitTypeType[] = "unit-type";

IfDebug(
global int NoWarningUnitType;		/// quiet ident lookup
);

global UnitType* UnitTypes;		/// unit-types definition
global int NumUnitTypes;		/// number of unit-types made

/*
**	Next unit type are used hardcoded in the source.
**
**	FIXME: find a way to make it configurable!
*/
global UnitType*UnitTypeOilPatch;	/// Oil patch for oil patch removement
global UnitType*UnitTypeGoldMine;	/// Gold mine unit type pointer
global UnitType*UnitTypeOrcTanker;	/// Orc tanker unit type pointer
global UnitType*UnitTypeHumanTanker;	/// Human tanker unit type pointer
global UnitType*UnitTypeOrcTankerFull;	/// Orc tanker full unit type pointer
global UnitType*UnitTypeHumanTankerFull;/// Human tanker full unit type pointer
global UnitType*UnitTypeHumanWorker;	/// Human worker
global UnitType*UnitTypeOrcWorker;	/// Orc worker
global UnitType*UnitTypeHumanWorkerWithGold;	/// Human worker with gold
global UnitType*UnitTypeOrcWorkerWithGold;	/// Orc worker with gold
global UnitType*UnitTypeHumanWorkerWithWood;	/// Human worker with wood
global UnitType*UnitTypeOrcWorkerWithWood;	/// Orc worker with wood
global UnitType*UnitTypeHumanWall;	/// Human wall
global UnitType*UnitTypeOrcWall;	/// Orc wall
global UnitType*UnitTypeCritter;	/// Critter unit type pointer
global UnitType*UnitTypeBerserker;	/// Berserker for berserker regeneration

/**
**	Mapping of W*rCr*ft number to our internal unit-type symbol.
**	The numbers are used in puds.
*/
global char** UnitTypeWcNames;

#ifdef DOXYGEN                          // no real code, only for document

/**
**	Lookup table for unit-type names
*/
local UnitType* UnitTypeHash[61];

#else

/**
**	Lookup table for unit-type names
*/
local hashtable(UnitType*,61) UnitTypeHash;

#endif

/**
**	Default resources for a new player.
*/
global int DefaultResources[MaxCosts];

/**
**	Default resources for a new player with low resources.
*/
global int DefaultResourcesLow[MaxCosts];

/**
**	Default resources for a new player with mid resources.
*/
global int DefaultResourcesMedium[MaxCosts];

/**
**	Default resources for a new player with high resources.
*/
global int DefaultResourcesHigh[MaxCosts];

/**
**	Default incomes for a new player.
*/
global int DefaultIncomes[MaxCosts];

/**
**	Default action for the resources.
*/
global char* DefaultActions[MaxCosts];

/**
**	Default names for the resources.
*/
global char* DefaultResourceNames[MaxCosts];

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Update the player stats for changed unit types.
**      @param reset indicates wether default value should be set to each stat ( level, upgrades )
*/
global void UpdateStats(int reset)
{
    UnitType* type;
    UnitStats* stats;
    unsigned player;
    unsigned i;

    //
    //  Update players stats
    //
    for (type = UnitTypes; type->OType; ++type) {
        if (reset){
	    // LUDO : FIXME : reset loading of player stats !
	for (player = 0; player < PlayerMax; ++player) {
	    stats = &type->Stats[player];
	    stats->AttackRange = type->_AttackRange;
	    stats->SightRange = type->_SightRange;
	    stats->Armor = type->_Armor;
	    stats->BasicDamage = type->_BasicDamage;
	    stats->PiercingDamage = type->_PiercingDamage;
	    stats->Speed = type->_Speed;
	    stats->HitPoints = type->_HitPoints;
	    for (i = 0; i < MaxCosts; ++i) {
		stats->Costs[i] = type->_Costs[i];
	    }
	    if (type->Building) {
		stats->Level = 0;	// Disables level display
	    } else {
		stats->Level = 1;
	    }
	}
	}

	//
	//      As side effect we calculate the movement flags/mask here.
	//
	switch (type->UnitType) {
	    case UnitTypeLand:		// on land
		type->MovementMask =
		    MapFieldLandUnit
		    | MapFieldSeaUnit
		    | MapFieldBuilding	// already occuppied
		    | MapFieldCoastAllowed
		    | MapFieldWaterAllowed	// can't move on this
		    | MapFieldUnpassable;
		break;
	    case UnitTypeFly:		// in air
		type->MovementMask =
		    MapFieldAirUnit;	// already occuppied
		break;
	    case UnitTypeNaval:		// on water
		if (type->Transporter) {
		    type->MovementMask =
			MapFieldLandUnit
			| MapFieldSeaUnit
			| MapFieldBuilding	// already occuppied
			| MapFieldLandAllowed;	// can't move on this
		    // Johns: MapFieldUnpassable only for land units?
		} else {
		    type->MovementMask =
			MapFieldLandUnit
			| MapFieldSeaUnit
			| MapFieldBuilding	// already occuppied
			| MapFieldCoastAllowed
			| MapFieldLandAllowed	// can't move on this
			| MapFieldUnpassable;
		}
		break;
	    default:
		DebugLevel1Fn("Were moves this unit?\n");
		type->MovementMask = 0;
		break;
	}
	if( type->Building || type->ShoreBuilding ) {
	    // Shore building is something special.
	    if( type->ShoreBuilding ) {
		type->MovementMask =
		    MapFieldLandUnit
		    | MapFieldSeaUnit
		    | MapFieldBuilding	// already occuppied
		    | MapFieldLandAllowed;	// can't build on this
	    }
	    type->MovementMask |= MapFieldNoBuilding;
	    //
	    //	A little chaos, buildings without HP can be entered.
	    //	The oil-patch is a very special case.
	    //
	    if( type->_HitPoints ) {
		type->FieldFlags = MapFieldBuilding;
	    } else if( type->OilPatch ) {
		type->FieldFlags = 0;
	    } else {
		type->FieldFlags = MapFieldNoBuilding;
	    }
	} else switch (type->UnitType) {
	    case UnitTypeLand:		// on land
		type->FieldFlags = MapFieldLandUnit;
		break;
	    case UnitTypeFly:		// in air
		type->FieldFlags = MapFieldAirUnit;
		break;
	    case UnitTypeNaval:		// on water
		type->FieldFlags = MapFieldSeaUnit;
		break;
	    default:
		DebugLevel1Fn("Were moves this unit?\n");
		type->FieldFlags = 0;
		break;
	}
    }
}

    /// Macro to fetch an 8bit value, to have some looking 8/16/32 bit funcs.
#define Fetch8(p)   (*((unsigned char*)(p))++)

/**
**	Parse UDTA area from puds.
**
**	@param udta	Pointer to udta area.
**	@param length	length of udta area.
*/
global void ParsePudUDTA(const char* udta,int length __attribute__((unused)))
{
    int i;
    int v;
    const char* start;
    UnitType* unittype;

    // FIXME: not the fastest, remove UnitTypeByWcNum from loops!
    IfDebug(
	if( length!=5694 && length!=5948 ) {
	    DebugLevel0("\n***\n");
	    DebugLevel0Fn("%d\n" _C_ length);
	    DebugLevel0("***\n\n");
	}
    )
    start=udta;

    for( i=0; i<110; ++i ) {		// overlap frames
	unittype=UnitTypeByWcNum(i);
	v=FetchLE16(udta);
	unittype->Construction=ConstructionByWcNum(v);
    }
    for( i=0; i<508; ++i ) {		// skip obsolete data
	v=FetchLE16(udta);
    }
    for( i=0; i<110; ++i ) {		// sight range
	unittype=UnitTypeByWcNum(i);
	v=FetchLE32(udta);
	unittype->_SightRange=v;
    }
    for( i=0; i<110; ++i ) {		// hit points
	unittype=UnitTypeByWcNum(i);
	v=FetchLE16(udta);
	unittype->_HitPoints=v;
    }
    for( i=0; i<110; ++i ) {		// Flag if unit is magic
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->Magic=v;
    }
    for( i=0; i<110; ++i ) {		// Build time * 6 = one second FRAMES
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->_Costs[TimeCost]=v;
    }
    for( i=0; i<110; ++i ) {		// Gold cost / 10
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->_Costs[GoldCost]=v*10;
    }
    for( i=0; i<110; ++i ) {		// Lumber cost / 10
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->_Costs[WoodCost]=v*10;
    }
    for( i=0; i<110; ++i ) {		// Oil cost / 10
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->_Costs[OilCost]=v*10;
    }
    for( i=0; i<110; ++i ) {		// Unit size in tiles
	unittype=UnitTypeByWcNum(i);
	v=FetchLE16(udta);
	unittype->TileWidth=v;
	v=FetchLE16(udta);
	unittype->TileHeight=v;
    }
    for( i=0; i<110; ++i ) {		// Box size in pixel
	unittype=UnitTypeByWcNum(i);
	v=FetchLE16(udta);
	unittype->BoxWidth=v;
	v=FetchLE16(udta);
	unittype->BoxHeight=v;
    }

    for( i=0; i<110; ++i ) {		// Attack range
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->_AttackRange=v;
    }
    for( i=0; i<110; ++i ) {		// React range
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->ReactRangeComputer=v;
    }
    for( i=0; i<110; ++i ) {		// React range
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->ReactRangePerson=v;
    }
    for( i=0; i<110; ++i ) {		// Armor
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->_Armor=v;
    }
    for( i=0; i<110; ++i ) {		// Selectable via rectangle
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->SelectableByRectangle=v!=0;
    }
    for( i=0; i<110; ++i ) {		// Priority
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->Priority=v;
    }
    for( i=0; i<110; ++i ) {		// Basic damage
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->_BasicDamage=v;
    }
    for( i=0; i<110; ++i ) {		// Piercing damage
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->_PiercingDamage=v;
    }
    for( i=0; i<110; ++i ) {		// Weapons upgradable
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->WeaponsUpgradable=v;
    }
    for( i=0; i<110; ++i ) {		// Armor upgradable
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->ArmorUpgradable=v;
    }
    for( i=0; i<110; ++i ) {		// Missile Weapon
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->Missile.Name=strdup(MissileTypeWcNames[v]);
	DebugCheck( unittype->Missile.Missile );
    }
    for( i=0; i<110; ++i ) {		// Unit type
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->UnitType=v;
    }
    for( i=0; i<110; ++i ) {		// Decay rate * 6 = secs
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->DecayRate=v;
    }
    for( i=0; i<110; ++i ) {		// Annoy computer factor
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->AnnoyComputerFactor=v;
    }
    for( i=0; i<58; ++i ) {		// 2nd mouse button action
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->MouseAction=v;
    }
    for( ; i<110; ++i ) {		// 2nd mouse button action
	unittype=UnitTypeByWcNum(i);
	unittype->MouseAction=0;
    }
    for( i=0; i<110; ++i ) {		// Point value for killing unit
	unittype=UnitTypeByWcNum(i);
	v=FetchLE16(udta);
	unittype->Points=v;
    }
    for( i=0; i<110; ++i ) {		// Can target (1 land, 2 sea, 4 air)
	unittype=UnitTypeByWcNum(i);
	v=Fetch8(udta);
	unittype->CanTarget=v;
    }

    for( i=0; i<110; ++i ) {		// Flags
	unittype=UnitTypeByWcNum(i);
	v=FetchLE32(udta);
    /// Nice looking bit macro
#define BIT(b,v)	(((v>>b))&1)
	unittype->LandUnit=BIT(0,v);
	unittype->AirUnit=BIT(1,v);
	unittype->ExplodeWhenKilled=BIT(2,v);
	unittype->SeaUnit=BIT(3,v);
	unittype->Critter=BIT(4,v);
	unittype->Building=BIT(5,v);
	unittype->Submarine=BIT(6,v);
	unittype->CanSeeSubmarine=BIT(7,v);
	unittype->CowerWorker=BIT(8,v);
	unittype->Tanker=BIT(9,v);
	unittype->Transporter=BIT(10,v);
	unittype->GivesOil=BIT(11,v);
	unittype->StoresGold=BIT(12,v);
	unittype->Vanishes=BIT(13,v);
	unittype->GroundAttack=BIT(14,v);
	unittype->IsUndead=BIT(15,v);
	unittype->ShoreBuilding=BIT(16,v);
	unittype->CanCastSpell=BIT(17,v);
	unittype->StoresWood=BIT(18,v);
	unittype->CanAttack=BIT(19,v);
	unittype->Tower=BIT(20,v);
	unittype->OilPatch=BIT(21,v);
	unittype->GoldMine=BIT(22,v);
	unittype->Hero=BIT(23,v);
	unittype->StoresOil=BIT(24,v);
	unittype->Volatile=BIT(25,v);
	unittype->CowerMage=BIT(26,v);
	unittype->Organic=BIT(27,v);

#ifdef DEBUG
	if( BIT(28,v) )	DebugLevel0("Unused bit 28 used in %d\n" _C_ i);
	if( BIT(29,v) )	DebugLevel0("Unused bit 29 used in %d\n" _C_ i);
	if( BIT(30,v) )	DebugLevel0("Unused bit 30 used in %d\n" _C_ i);
	if( BIT(31,v) )	DebugLevel0("Unused bit 31 used in %d\n" _C_ i);
#endif
#undef BIT
	//
	//	Unit type checks.
	//
	if( unittype->CanCastSpell && !unittype->_MaxMana ) {
	    DebugLevel0Fn("%s: Need max mana value\n" _C_ unittype->Ident);
	    unittype->_MaxMana=255;
	}
    }

    // FIXME: peon applies also to peon-with-gold and peon-with-wood
    // FIXME: oil-tanker applies also to oil-tanker-full

    DebugLevel0("\tUDTA used %d bytes\n" _C_ udta-start);

    UpdateStats(1);
}

/**
**	Get the animations structure by ident.
**
**	@param ident	Identifier for the animation.
**	@return		Pointer to the animation structure.
**
**	@todo	Remove the use of scheme symbols to store, use own hash.
*/
global Animations* AnimationsByIdent(const char* ident)
{
    SCM sym;

    sym=gh_symbol2scm((char*)ident);
    if( symbol_boundp(sym,NIL)==SCM_BOOL_T ) {
	return (Animations*)gh_scm2int(symbol_value(sym,NIL));
    }
    DebugLevel0Fn("Warning animation `%s' not found\n" _C_ ident);
    return NULL;
}

/**
**	Save state of an animition set to file.
**
**	@param name	Animation name.
**	@param anim	Save this animation.
**	@param file	Output file.
*/
local void SaveAnimation(const char* name,const Animation* anim,FILE* file)
{
    int i;
    int p;
    int frame;
    const Animation* temp;

    if( anim ) {
	//
	//	Calculate the wait sum and pixels count.
	//
	i=p=0;
	for( temp=anim; ; ++temp ) {
	    i+=temp->Sleep&0xFF;
	    p+=temp->Pixel;
	    if( temp->Flags&AnimationEnd ) {
		break;
	    }
	    if( temp->Flags&AnimationRestart ) {
		i=p=0;
	    }
	}

	fprintf(file,"\n  '%s '(\t; #%d",name,i);
	if( p ) {
	    fprintf(file," P%d",p);
	}

	temp=anim;
	frame=0;
	for( ;; ) {
	    fprintf(file,"\n    ");
	    for( i=0; i<4; ++i ) {
		if( i ) {
		    fputc(' ',file);
		}
		frame+=temp->Frame;
		fprintf(file,"#(%2d %d %3d %3d)",
			temp->Flags&0x7F,temp->Pixel,temp->Sleep&0xFF,frame);
		if( temp->Flags&AnimationRestart ) {
		    frame=0;
		}
		if( temp->Flags&AnimationEnd ) {
		    fprintf(file,")");
		    return;
		}
		++temp;
	    }
	}
    }
}

/**
**	Save state of the animitions set to file.
**
**	We save only the first occurance of an animation.
**
**	@param type	Save animations of this unit-type.
**	@param file	Output file.
*/
local void SaveAnimations(const UnitType* type,FILE* file)
{
    const UnitType* temp;
    const Animations* anims;
    int i;

    if( !(anims=type->Animations) ) {
	return;
    }

    //
    //	Look if this is the first use of it.
    //
    for( temp=UnitTypes; temp->OType && temp!=type ; ++temp ) {
	if( temp->Animations==anims ) {
	    return;			// allready handled.
	}
    }

    fprintf(file,"\n;;------\n;;\t");
    //
    //	Print all units that use this animation.
    //
    i=0;
    for( temp=UnitTypes; temp->OType; ++temp ) {
	if( temp->Animations==anims ) {
	    if( i ) {
		fprintf(file,", ");
	    }
	    fprintf(file,"%s",temp->Name);
	    i=1;
	}
    }
    fprintf(file,"\n(define-animations 'animations-%s",type->Ident+5);

    SaveAnimation("still",anims->Still,file);
    SaveAnimation("move",anims->Move,file);
    SaveAnimation("attack",anims->Attack,file);
    SaveAnimation("die",anims->Die,file);

    fprintf(file,")\n");
}

/**
**	Save state of an unit-type to file.
**
**	@param file	Output file.
**	@param type	Unit-type to save.
**	@param all	Flag save all values.
**
**	@todo	Arrange the variables more logical
*/
local void SaveUnitType(FILE* file,const UnitType* type,int all)
{
    int i;
    int flag;
    const UnitType* temp;

    fprintf(file,"(define-unit-type '%s",type->Ident);
    fprintf(file," 'name \"%s\"\n  ",type->Name);
    // Graphic files
    if( type->SameSprite ) {
	fprintf(file,"'use '%s",type->SameSprite);
    } else {
	fputs("'files '(",file);
	for( flag=i=0; i<TilesetMax; ++i ) {
	    if( type->File[i] ) {
		if( flag ) {
		    fputs("\n    ",file);
		}
		fprintf(file,"%s \"%s\"",Tilesets[i]->Ident,type->File[i]);
		flag=1;
	    }
	}
	fputs(")",file);
    }
    fprintf(file,"\n  'size '(%d %d)\n",type->Width,type->Height);
    if( type->ShadowFile ) {
	fprintf(file,"  'shadow '(file \"%s\" width %d height %d)\n",
		type->ShadowFile, type->ShadowWidth, type->ShadowHeight);
    }

    //
    //	Animations are shared, find first use of the unit-type animation.
    //
    for( temp=UnitTypes; temp->OType && temp!=type ; ++temp ) {
	if( temp->Animations==type->Animations ) {
	    break;
	}
    }
    fprintf(file,"  'animations 'animations-%s",temp->Ident+5);
    fprintf(file," 'icon '%s\n",IdentOfIcon(type->Icon.Icon));
    for( i=flag=0; i<MaxCosts; ++i ) {
	if( all || type->_Costs[i] ) {
	    if( !flag ) {
		fputs("  'costs '(",file);
		flag=1;
	    } else {
		fputs(" ",file);
	    }
	    fprintf(file,"%s %d",DefaultResourceNames[i],type->_Costs[i]);
	}
    }
    if( flag ) {
	fputs(")\n",file);
    }

    if( type->Construction ) {
	fprintf(file,"  'construction '%s\n",type->Construction->Ident);
    }
    fprintf(file,"  'speed %d\n",type->_Speed);
    fprintf(file,"  'hit-points %d\n",type->_HitPoints);
    if( all || type->_MaxMana ) {
	fprintf(file,"  'max-mana %d\n",type->_MaxMana);
    }
    if( all || type->Magic ) {
	fprintf(file,"  'magic %d\n",type->Magic);
    }
    fprintf(file,"  'tile-size '(%d %d)",type->TileWidth,type->TileHeight);
    fprintf(file," 'box-size '(%d %d)\n",type->BoxWidth,type->BoxHeight);
    fprintf(file,"  'sight-range %d",type->_SightRange);
    if( all || type->ReactRangeComputer ) {
	fprintf(file," 'computer-reaction-range %d",type->ReactRangeComputer);
    }
    if( all || type->ReactRangePerson ) {
	fprintf(file," 'person-reaction-range %d",type->ReactRangePerson);
    }
    fputs("\n",file);

    if( all || type->_Armor ) {
	fprintf(file,"  'armor %d",type->_Armor);
    } else {
	fputs(" ",file);
    }
    fprintf(file," 'basic-damage %d",type->_BasicDamage);
    fprintf(file," 'piercing-damage %d",type->_PiercingDamage);
    fprintf(file," 'missile '%s\n",type->Missile.Name);
    if( all || type->MinAttackRange ) {
	fprintf(file,"  'min-attack-range %d",type->MinAttackRange);
	fprintf(file," 'max-attack-range %d\n",type->_AttackRange);
    } else if( type->_AttackRange ) {
	fprintf(file,"  'max-attack-range %d\n",type->_AttackRange);
    }
    if( all || type->WeaponsUpgradable ) {
	fprintf(file,"  'weapons-upgradable %d",type->WeaponsUpgradable);
	if( all || type->ArmorUpgradable ) {
	    fprintf(file," 'armor-upgradable %d\n",type->ArmorUpgradable);
	} else {
	    fputs("\n",file);
	}
    } else if( type->ArmorUpgradable ) {
	fprintf(file,"  'armor-upgradable %d\n",type->ArmorUpgradable);
    }
    fprintf(file,"  'priority %d",type->Priority);
    if( all || type->AnnoyComputerFactor ) {
	fprintf(file," 'annoy-computer-factor %d",type->AnnoyComputerFactor);
    }
    fputs("\n",file);
    if( all || type->DecayRate ) {
	fprintf(file,"  'decay-rate %d\n",type->DecayRate);
    }
    if( all || type->Points ) {
	fprintf(file,"  'points %d\n",type->Points);
    }
    if( all || type->Demand ) {
	fprintf(file,"  'demand %d\n",type->Demand);
    }
    if( all || type->Supply ) {
	fprintf(file,"  'supply %d\n",type->Supply);
    }

    if( type->CorpseName ) {
	fprintf(file,"  'corpse '(%s %d)\n",
		type->CorpseName,type->CorpseScript);
    }
    if( type->ExplodeWhenKilled ) {
	fprintf(file,"  'explode-when-killed\n");
    }

    fprintf(file,"  ");
    switch( type->UnitType ) {
	case UnitTypeLand:
	    fputs("'type-land",file);
	    break;
	case UnitTypeFly:
	    fputs("'type-fly",file);
	    break;
	case UnitTypeNaval:
	    fputs("'type-naval",file);
	    break;
	default:
	    fputs("'type-unknown",file);
	    break;
    }
    fprintf(file,"\n");

    fprintf(file,"  ");
    switch( type->MouseAction ) {
	case MouseActionNone:
	    if( all ) {
		fprintf(file,"'right-none");
	    }
	    break;
	case MouseActionAttack:
	    fprintf(file,"'right-attack");
	    break;
	case MouseActionMove:
	    fprintf(file,"'right-move");
	    break;
	case MouseActionHarvest:
	    fprintf(file,"'right-harvest");
	    break;
	case MouseActionHaulOil:
	    fprintf(file,"'right-haul-oil");
	    break;
	case MouseActionDemolish:
	    fprintf(file,"'right-demolish");
	    break;
	case MouseActionSail:
	    fprintf(file,"'right-sail");
	    break;
	default:
	    fprintf(file,"'right-unknown");
	    break;
    }
    fprintf(file,"\n");

    if( type->GroundAttack ) {
	fprintf(file,"  'can-ground-attack\n");
    }
    if( type->CanAttack ) {
	fprintf(file,"  'can-attack\n");
    }
    if( type->CanTarget ) {
	fprintf(file,"  ");
	if( type->CanTarget&CanTargetLand ) {
	    fprintf(file,"'can-target-land ");
	}
	if( type->CanTarget&CanTargetSea ) {
	    fprintf(file,"'can-target-sea ");
	}
	if( type->CanTarget&CanTargetAir ) {
	    fprintf(file,"'can-target-air ");
	}
	if( type->CanTarget&~7 ) {
	    fprintf(file,"'can-target-other ");
	}
	fprintf(file,"\n");
    }

    if( type->Building ) {
	fprintf(file,"  'building\n");
    }
    if( type->ShoreBuilding ) {
	fprintf(file,"  'shore-building\n");
    }
    if( type->LandUnit ) {
	fprintf(file,"  'land-unit\n");
    }
    if( type->AirUnit ) {
	fprintf(file,"  'air-unit\n");
    }
    if( type->SeaUnit ) {
	fprintf(file,"  'sea-unit\n");
    }

    if( type->Critter ) {
	fprintf(file,"  'critter\n");
    }
    if( type->Submarine ) {
	fprintf(file,"  'submarine\n");
    }
    if( type->CanSeeSubmarine ) {
	fprintf(file,"  'can-see-submarine\n");
    }
    if( type->Transporter ) {
	fprintf(file,"  'transporter\n");
    }

    if( type->CowerWorker ) {
	fprintf(file,"  'cower-worker\n");
    }
    if( type->Tanker ) {
	fprintf(file,"  'tanker\n");
    }
    if( type->GivesOil ) {
	fprintf(file,"  'gives-oil\n");
    }
    if( type->StoresGold ) {
	fprintf(file,"  'stores-gold\n");
    }
    if( type->StoresWood ) {
	fprintf(file,"  'stores-wood\n");
    }
    if( type->OilPatch ) {
	fprintf(file,"  'oil-patch\n");
    }
    if( type->GoldMine ) {
	fprintf(file,"  'gives-gold\n");
    }
    if( type->StoresOil ) {
	fprintf(file,"  'stores-oil\n");
    }

    if( type->Vanishes ) {
	fprintf(file,"  'vanishes\n");
    }
    if( type->Tower ) {
	fprintf(file,"  'tower\n");
    }
    if( type->Hero ) {
	fprintf(file,"  'hero\n");
    }
    if( type->Volatile ) {
	fprintf(file,"  'volatile\n");
    }
    if( type->CowerMage ) {
	fprintf(file,"  'cower-mage\n");
    }
    if( type->IsUndead ) {
	fprintf(file,"  'isundead\n");
    }
    if( type->CanCastSpell ) {
	fprintf(file,"  'can-cast-spell\n");
    }
    if( type->Organic ) {
	fprintf(file,"  'organic\n");
    }
    if( type->SelectableByRectangle ) {
	fprintf(file,"  'selectable-by-rectangle\n");
    }
    if( type->Teleporter ) {
	fprintf(file,"  'teleporter\n");
    }

    fprintf(file,"  'sounds '(");
    if( type->Sound.Selected.Name ) {
	fprintf(file,"\n    selected \"%s\"",type->Sound.Selected.Name);
    }
    if( type->Sound.Acknowledgement.Name ) {
	fprintf(file,"\n    acknowledge \"%s\"",
		type->Sound.Acknowledgement.Name);
    }
    if( type->Sound.Ready.Name ) {
	fprintf(file,"\n    ready \"%s\"",type->Sound.Ready.Name);
    }
    if( type->Sound.Help.Name ) {
	fprintf(file,"\n    help \"%s\"",type->Sound.Help.Name);
    }
    if( type->Sound.Dead.Name ) {
	fprintf(file,"\n    dead \"%s\"",type->Sound.Dead.Name);
    }
    // FIXME: Attack should be removed!
    if( type->Weapon.Attack.Name ) {
	fprintf(file,"\n    attack \"%s\"",type->Weapon.Attack.Name);
    }
    fprintf(file,")");
    fprintf(file,")\n\n");
}

/**
**	Save state of an unit-stats to file.
**
**	@param stats	Unit-stats to save.
**	@param ident	Unit-type ident.
**	@param plynr	Player number.
**	@param file	Output file.
*/
local void SaveUnitStats(const UnitStats* stats,const char* ident,int plynr,
	FILE* file)
{
    int j;

    fprintf(file,"(define-unit-stats '%s %d\n  ",ident,plynr);
    fprintf(file,"'level %d ",stats->Level);
    fprintf(file,"'speed %d ",stats->Speed);
    fprintf(file,"'attack-range %d ",stats->AttackRange);
    fprintf(file,"'sight-range %d\n  ",stats->SightRange);
    fprintf(file,"'armor %d ",stats->Armor);
    fprintf(file,"'basic-damage %d ",stats->BasicDamage);
    fprintf(file,"'piercing-damage %d ",stats->PiercingDamage);
    fprintf(file,"'hit-points %d\n  ",stats->HitPoints);
    fprintf(file,"'costs '(");
    for( j=0; j<MaxCosts; ++j ) {
	if( j ) {
//	    if( j==MaxCosts/2 ) {
//		fputs("\n    ",file);
//	    } else {
		fputc(' ',file);
//	    }
	}
	fprintf(file,"%s %d",DefaultResourceNames[j],stats->Costs[j]);
    }

    fprintf(file,") )\n");
}

/**
**	Save state of the unit-type table to file.
**
**	@param file	Output file.
*/
global void SaveUnitTypes(FILE* file)
{
    const UnitType* type;
    char* const* sp;
    int i;

    fprintf(file,"\n;;; -----------------------------------------\n");
    fprintf(file,";;; MODULE: unittypes $Id: unittype.c,v 1.69 2002/12/22 15:19:42 ariclone Exp $\n\n");

    //	Original number to internal unit-type name.

    i=fprintf(file,"(define-unittype-wc-names");
    for( sp=UnitTypeWcNames; *sp; ++sp ) {
	if( i+strlen(*sp)>79 ) {
	    i=fprintf(file,"\n ");
	}
	i+=fprintf(file," '%s",*sp);
    }
    fprintf(file,")\n");

    //	Save all animations.

    for( type=UnitTypes; type->OType; ++type ) {
	SaveAnimations(type,file);
    }

    //	Save all types

    for( type=UnitTypes; type->OType; ++type ) {
	fputc('\n',file);
	SaveUnitType(file,type,0);
    }

    //	Save all stats

    for( type=UnitTypes; type->OType; ++type ) {
	fputc('\n',file);
	for( i=0; i<PlayerMax; ++i ) {
	    SaveUnitStats(&type->Stats[i],type->Ident,i,file);
	}
    }
}

/**
**	Find unit-type by identifier.
**
**	@param ident	The unit-type identifier.
**	@return		Unit-type pointer.
*/
global UnitType* UnitTypeByIdent(const char* ident)
{
    UnitType* const* type;

    type=(UnitType* const*)hash_find(UnitTypeHash,ident);
    if( type ) {
	return *type;
    }

    IfDebug(
	if( !NoWarningUnitType ) {
	    DebugLevel0Fn("Name `%s' not found\n" _C_ ident);
	}
    );

    return NULL;
}

/**
**	Find unit-type by wc number.
**
**	@param num	The unit-type number used in f.e. puds.
**	@return		Unit-type pointer.
*/
global UnitType* UnitTypeByWcNum(unsigned num)
{
    return UnitTypeByIdent(UnitTypeWcNames[num]);
}

/**
**	Allocate an empty unit-type slot.
**
**	@param ident	Identifier to identify the slot (malloced by caller!).
**
**	@return		New allocated (zeroed) unit-type pointer.
*/
global UnitType* NewUnitTypeSlot(char* ident)
{
    UnitType* type;
    int i;

    // +2 for slot and an empty slot at end.
    type=calloc(NumUnitTypes+2,sizeof(UnitType));
    if( !type ) {
	fprintf(stderr,"Out of memory\n");
	ExitFatal(-1);
    }
    memcpy(type,UnitTypes,sizeof(UnitType)*NumUnitTypes);
    if( UnitTypes ) {
	free(UnitTypes);
    }
    UnitTypes=type;
    type=UnitTypes+NumUnitTypes;
    type->OType=UnitTypeType;
    type->Type=NumUnitTypes++;
    type->Ident=ident;
    //
    //	Rehash.
    //
    for( i=0; i<NumUnitTypes; ++i ) {
	*(UnitType**)hash_add(UnitTypeHash,UnitTypes[i].Ident)=&UnitTypes[i];
    }

    return type;
}

/**
**	Draw unit-type on map.
**
**	@param type	Unit-type pointer.
**	@param frame	Animation frame of unit-type.
**	@param x	Screen X pixel postion to draw unit-type.
**	@param y	Screen Y pixel postion to draw unit-type.
**
**	@todo	Do screen position caculation in high level.
**		Better way to handle in x mirrored sprites.
*/
global void DrawUnitType(const UnitType* type,int frame,int x,int y)
{
    // FIXME: move this calculation to high level.
    x-=(type->Width-type->TileWidth*TileSizeX)/2;
    y-=(type->Height-type->TileHeight*TileSizeY)/2;

    // FIXME: This is a hack for mirrored sprites
    if( frame<0 ) {
	VideoDrawClipX(type->Sprite,-frame,x,y);
    } else {
	VideoDrawClip(type->Sprite,frame,x,y);
    }
}

/**
**	Init unit types.
*/
global void InitUnitTypes(int reset_player_stats)
{
    int type;

    for( type=0; UnitTypes[type].OType; ++type ) {
	//
	//	Initialize:
	//
	DebugCheck( UnitTypes[type].Type!=type );
	//
	//	Add idents to hash.
	//
	*(UnitType**)hash_add(UnitTypeHash,UnitTypes[type].Ident)
		=&UnitTypes[type];
	//
	//	Hardcoded incomes, FIXME: should be moved to some configs.
	//
	if( !strcmp(UnitTypes[type].Ident,"unit-elven-lumber-mill") ) {
	    UnitTypes[type].ImproveIncomes[WoodCost]=
		    DefaultIncomes[WoodCost]+25;
	}
	if( !strcmp(UnitTypes[type].Ident,"unit-troll-lumber-mill") ) {
	    UnitTypes[type].ImproveIncomes[WoodCost]=
		    DefaultIncomes[WoodCost]+25;
	}
	if( !strcmp(UnitTypes[type].Ident,"unit-human-refinery") ) {
	    UnitTypes[type].ImproveIncomes[OilCost]=
		    DefaultIncomes[OilCost]+25;
	}
	if( !strcmp(UnitTypes[type].Ident,"unit-orc-refinery") ) {
	    UnitTypes[type].ImproveIncomes[OilCost]=
		    DefaultIncomes[OilCost]+25;
	}
	if( !strcmp(UnitTypes[type].Ident,"unit-keep") ) {
	    UnitTypes[type].ImproveIncomes[GoldCost]=
		    DefaultIncomes[GoldCost]+10;
	}
	if( !strcmp(UnitTypes[type].Ident,"unit-stronghold") ) {
	    UnitTypes[type].ImproveIncomes[GoldCost]=
		    DefaultIncomes[GoldCost]+10;
	}
	if( !strcmp(UnitTypes[type].Ident,"unit-castle") ) {
	    UnitTypes[type].ImproveIncomes[GoldCost]=
		    DefaultIncomes[GoldCost]+20;
	}
	if( !strcmp(UnitTypes[type].Ident,"unit-fortress") ) {
	    UnitTypes[type].ImproveIncomes[GoldCost]=
		    DefaultIncomes[GoldCost]+20;
	}
    }

    // LUDO : called after game is loaded -> don't reset stats !
    UpdateStats(reset_player_stats);	// Calculate the stats

    //
    //	Setup hardcoded unit types. FIXME: should be moved to some configs.
    //
    UnitTypeOilPatch=UnitTypeByIdent("unit-oil-patch");
    UnitTypeGoldMine=UnitTypeByIdent("unit-gold-mine");
    UnitTypeHumanTanker=UnitTypeByIdent("unit-human-oil-tanker");
    UnitTypeOrcTanker=UnitTypeByIdent("unit-orc-oil-tanker");
    UnitTypeHumanTankerFull=UnitTypeByIdent("unit-human-oil-tanker-full");
    UnitTypeOrcTankerFull=UnitTypeByIdent("unit-orc-oil-tanker-full");
    UnitTypeHumanWorker=UnitTypeByIdent("unit-peasant");
    UnitTypeOrcWorker=UnitTypeByIdent("unit-peon");
    UnitTypeHumanWorkerWithGold=UnitTypeByIdent("unit-peasant-with-gold");
    UnitTypeOrcWorkerWithGold=UnitTypeByIdent("unit-peon-with-gold");
    UnitTypeHumanWorkerWithWood=UnitTypeByIdent("unit-peasant-with-wood");
    UnitTypeOrcWorkerWithWood=UnitTypeByIdent("unit-peon-with-wood");
    UnitTypeHumanWall=UnitTypeByIdent("unit-human-wall");
    UnitTypeOrcWall=UnitTypeByIdent("unit-orc-wall");
    UnitTypeCritter=UnitTypeByIdent("unit-critter");
    UnitTypeBerserker=UnitTypeByIdent("unit-berserker");
}

/**
**	Load the graphics for the unit-types.
*/
global void LoadUnitTypes(void)
{
    UnitType* type;
    const char* file;

    for( type=UnitTypes; type->OType; ++type ) {
	if( (file=type->ShadowFile) ) {
	    char *buf;
	    buf=alloca(strlen(file)+9+1);
	    file=strcat(strcpy(buf,"graphics/"),file);
	    ShowLoadProgress("\tUnit `%s'\n",file);
	    type->ShadowSprite=LoadSprite(file,type->ShadowWidth,type->ShadowHeight);
	}

	//
	//	Unit-type uses the same sprite as an other.
	//
	if( type->SameSprite ) {
	    continue;
	}

	//
	//	FIXME: must handle terrain different!
	//

	file=type->File[TheMap.Terrain];
	if( !file ) {			// default one
	    file=type->File[0];
	}
	if( file ) {
	    char* buf;

	    buf=alloca(strlen(file)+9+1);
	    file=strcat(strcpy(buf,"graphics/"),file);
	    ShowLoadProgress("\tUnit `%s'\n",file);
	    type->Sprite=LoadSprite(file,type->Width,type->Height);
	}
    }

    for( type=UnitTypes; type->OType; ++type ) {
	//
	//	Unit-type uses the same sprite as an other.
	//
	if( type->SameSprite ) {
	    const UnitType* unittype;

	    unittype=UnitTypeByIdent(type->SameSprite);
	    if( !unittype ) {
		PrintFunction();
		fprintf(stdout,"Unit-type %s not found\n" ,type->SameSprite);
		ExitFatal(-1);
	    }
	    type->Sprite=unittype->Sprite;
	}

	//
	//	Lookup icons.
	//
	type->Icon.Icon=IconByIdent(type->Icon.Name);
	//
	//	Lookup missiles.
	//
	type->Missile.Missile=MissileTypeByIdent(type->Missile.Name);
	//
	//	Lookup corpse.
	//
	if( type->CorpseName ) {
	    type->CorpseType=UnitTypeByIdent(type->CorpseName);
	}

	// FIXME: should i copy the animations of same graphics?
    }

    // FIXME: must copy unit data from peon/peasant to with gold/wood
    // FIXME: must copy unit data from tanker to tanker full
}

/**
**	Cleanup the unit-type module.
*/
global void CleanUnitTypes(void)
{
    UnitType* type;
    void** ptr;
#ifdef USE_OPENGL
    int i;
#endif

    DebugLevel0Fn("FIXME: icon, sounds not freed.\n");

    //
    //	Mapping the original unit-type numbers in puds to our internal strings
    //
    if( (ptr=(void**)UnitTypeWcNames) ) {	// Free all old names
	while( *ptr ) {
	    free(*ptr++);
	}
	free(UnitTypeWcNames);

	UnitTypeWcNames=NULL;
    }

    //	FIXME: scheme contains references on this structure.
    //	Clean all animations.

    for( type=UnitTypes; type->OType; ++type ) {
	Animations* anims;
	UnitType* temp;

	if( !(anims=type->Animations) ) {	// Must be handled?
	    continue;
	}
	for( temp=type; temp->OType; ++temp ) {	// remove all uses
	    if( anims==temp->Animations ) {
		temp->Animations=NULL;
	    }
	}
	type->Animations=NULL;
	if( anims->Still ) {
	    free(anims->Still);
	}
	if( anims->Move ) {
	    free(anims->Move);
	}
	if( anims->Attack ) {
	    free(anims->Attack);
	}
	if( anims->Die ) {
	    free(anims->Die);
	}
	free(anims);
    }

    //	Clean all unit-types

    if( UnitTypes ) {
	for( type=UnitTypes; type->OType; ++type ) {
	    hash_del(UnitTypeHash,type->Ident);

	    DebugCheck( !type->Ident );
	    free(type->Ident);
	    DebugCheck( !type->Name );
	    free(type->Name);

	    if( type->SameSprite ) {
		free(type->SameSprite);
	    }
	    if( type->File[0] ) {
		free(type->File[0]);
	    }
	    if( type->File[1] ) {
		free(type->File[1]);
	    }
	    if( type->File[2] ) {
		free(type->File[2]);
	    }
	    if( type->File[3] ) {
		free(type->File[3]);
	    }
	    if( type->Icon.Name ) {
		free(type->Icon.Name);
	    }
	    if( type->Missile.Name ) {
		free(type->Missile.Name);
	    }
	    if( type->CorpseName ) {
		free(type->CorpseName);
	    }

	    //
	    //	FIXME: Sounds can't be freed, they still stuck in sound hash.
	    //
	    if( type->Sound.Selected.Name ) {
		free(type->Sound.Selected.Name);
	    }
	    if( type->Sound.Acknowledgement.Name ) {
		free(type->Sound.Acknowledgement.Name);
	    }
	    if( type->Sound.Ready.Name ) {
		free(type->Sound.Ready.Name);
	    }
	    if( type->Sound.Help.Name ) {
		free(type->Sound.Help.Name);
	    }
	    if( type->Sound.Dead.Name ) {
		free(type->Sound.Dead.Name);
	    }
	    if( type->Weapon.Attack.Name ) {
		free(type->Weapon.Attack.Name);
	    }

	    if( !type->SameSprite ) {	// our own graphics
		VideoSaveFree(type->Sprite);
	    }
#ifdef USE_OPENGL
	    for( i=0; i<PlayerMax; ++i ) {
		VideoSaveFree(type->PlayerColorSprite[i]);
	    }
#endif
	}
	free(UnitTypes);
	UnitTypes=NULL;
	NumUnitTypes=0;
    }

    //
    //	Clean hardcoded unit types.
    //
    UnitTypeGoldMine=NULL;
    UnitTypeHumanTanker=NULL;
    UnitTypeOrcTanker=NULL;
    UnitTypeHumanTankerFull=NULL;
    UnitTypeOrcTankerFull=NULL;
    UnitTypeHumanWorker=NULL;
    UnitTypeOrcWorker=NULL;
    UnitTypeHumanWorkerWithGold=NULL;
    UnitTypeOrcWorkerWithGold=NULL;
    UnitTypeHumanWorkerWithWood=NULL;
    UnitTypeOrcWorkerWithWood=NULL;
    UnitTypeHumanWall=NULL;
    UnitTypeOrcWall=NULL;
    UnitTypeCritter=NULL;
    UnitTypeBerserker=NULL;
}

//@}
