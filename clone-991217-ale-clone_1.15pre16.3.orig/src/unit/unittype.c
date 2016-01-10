/*
**	A clone of a famous game.
*/
/**@name unittype.c	-	The unit types. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: unittype.c,v 1.40 1999/12/09 16:27:15 root Exp $
*/

//@{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if !defined(__CYGWIN__) && !defined(__MINGW32__)
#include <netinet/in.h>

#include <endian.h>
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
#include <byteswap.h>
#endif
#endif

#include "clone.h"
#include "video.h"
#include "tileset.h"
#include "map.h"
#include "sound_id.h"
#include "unitsound.h"
#include "button.h"
#include "unittype.h"
#include "player.h"

/**
**	Table unit-type enums -> string.
**
**	FIXME: This will be removed soon.
*/
local const char* UnitTypeNames[] = {
    "UnitFootman",
    "UnitGrunt",
    "UnitPeasant",
    "UnitPeon",
    "UnitBallista",
    "UnitCatapult",
    "UnitKnight",
    "UnitOgre",
    "UnitArcher",
    "UnitAxethrower",
    "UnitMage",
    "UnitDeathKnight",
    "UnitPaladin",
    "UnitOgreMage",
    "UnitDwarves",
    "UnitGoblinSappers",
    "UnitAttackPeasant",
    "UnitAttackPeon",
    "UnitRanger",
    "UnitBerserker",
    "UnitAlleria",
    "UnitTeronGorefiend",
    "UnitKurdanAndSky_ree",
    "UnitDentarg",
    "UnitKhadgar",
    "UnitGnomHellscream",
    "UnitTankerHuman",
    "UnitTankerOrc",
    "UnitTransportHuman",
    "UnitTransportOrc",
    "UnitElvenDestroyer",
    "UnitTrollDestroyer",
    "UnitBattleship",
    "UnitJuggernaught",
    "UnitNothing",
    "UnitDeathwing",
    "UnitNothing1",
    "UnitNothing2",
    "UnitGnomishSubmarine",
    "UnitGiantTurtle",
    "UnitGnomishFlyingMachine",
    "UnitGoblinZeppelin",
    "UnitGryphonRider",
    "UnitDragon",
    "UnitTuralyon",
    "UnitEyeOfKilrogg",
    "UnitDanath",
    "UnitKorgathBladefist",
    "UnitNothing3",
    "UnitCho_gall",
    "UnitLothar",
    "UnitGul_dan",
    "UnitUtherLightbringer",
    "UnitZuljin",
    "UnitNothing4",
    "UnitSkeleton",
    "UnitDaemon",
    "UnitCritter",
    "UnitFarm",
    "UnitPigFarm",
    "UnitBarracksHuman",
    "UnitBarracksOrc",
    "UnitChurch",
    "UnitAltarOfStorms",
    "UnitScoutTowerHuman",
    "UnitScoutTowerOrc",
    "UnitStables",
    "UnitOgreMound",
    "UnitGnomishInventor",
    "UnitGoblinAlchemist",
    "UnitGryphonAviary",
    "UnitDragonRoost",
    "UnitShipyardHuman",
    "UnitShipyardOrc",
    "UnitTownHall",
    "UnitGreatHall",
    "UnitElvenLumberMill",
    "UnitTrollLumberMill",
    "UnitFoundryHuman",
    "UnitFoundryOrc",
    "UnitMageTower",
    "UnitTempleOfTheDamned",
    "UnitBlacksmithHuman",
    "UnitBlacksmithOrc",
    "UnitRefineryHuman",
    "UnitRefineryOrc",
    "UnitOilPlatformHuman",
    "UnitOilPlatformOrc",
    "UnitKeep",
    "UnitStronghold",
    "UnitCastle",
    "UnitFortress",
    "UnitGoldMine",
    "UnitOilPatch",
    "UnitStartLocationHuman",
    "UnitStartLocationOrc",
    "UnitGuardTowerHuman",
    "UnitGuardTowerOrc",
    "UnitCannonTowerHuman",
    "UnitCannonTowerOrc",
    "UnitCircleofPower",
    "UnitDarkPortal",
    "UnitRunestone",
    "UnitWallHuman",
    "UnitWallOrc",
    "UnitDeadBody",
    "Unit1x1DestroyedPlace",
    "Unit2x2DestroyedPlace",
    "Unit3x3DestroyedPlace",
    "Unit4x4DestroyedPlace",
    "UnitPeasantWithGold",
    "UnitPeonWithGold",
    "UnitPeasantWithWood",
    "UnitPeonWithWood",
    "UnitTankerHumanFull",
    "UnitTankerOrcFull",
    NULL
};

/**
**	Generate C - table for UnitTypes.
*/
global void PrintUnitTypeTable(void)
{
    int i;
    UnitType* type;

    for( i=0; i<sizeof(UnitTypes)/sizeof(*UnitTypes); ++i ) {
	type=&UnitTypes[i];
	printf("\n{   UnitTypeType, \"%s\"",type->Ident);
	printf("\n    ,\"%s\"\n    ",type->Name);
	if( type->SameSprite ) {
	    printf(",\"%s\"",type->SameSprite);
	} else {
	    printf(",NULL");
	}
	printf(", {\n");
	if( type->File[0] ) {
	    printf("\t\"%s\"\n",type->File[0]);
	} else {
	    printf("\tDEFAULT\n");
	}
	if( type->File[1] ) {
	    printf("\t,\"%s\"\n",type->File[1]);
	} else {
	    printf("\t,DEFAULT\n");
	}
	if( type->File[2] ) {
	    printf("\t,\"%s\"\n",type->File[2]);
	} else {
	    printf("\t,DEFAULT\n");
	}
	if( type->File[3] ) {
	    printf("\t,\"%s\" }\n",type->File[3]);
	} else {
	    printf("\t,DEFAULT }\n");
	}

	printf("    ,%3d,%3d\t\t\t// graphic size\n"
		,type->Width,type->Height);

#if 1
	printf("   ,_%sAnimations\t// animations\n"
	    ,UnitTypeNames[i]+4);
#endif
	printf("\t,{ \"%s\" }\n",IdentOfIcon(type->Icon.Icon));

	printf("\t//Speed\tOvFrame\tSightR\tHitpnt\tMagic\tBTime\tGold\tWood\tOil\n");
	printf("\t,%6d,%7d,%6d,%7d,%6d, {%5d,%6d,%7d,%6d }\n"
	    ,type->_Speed
	    ,type->OverlapFrame
	    ,type->_SightRange
	    ,type->_HitPoints
	    ,type->Magic
	    ,type->_Costs[TimeCost]
	    ,type->_Costs[GoldCost]
	    ,type->_Costs[WoodCost]
	    ,type->_Costs[OilCost]);
	printf("\t//TileW\tTileH\tBoxW\tBoxH\tAttack\tReactC\tReactH\n");
	printf("\t,%6d,%5d,%6d,%7d,%9d,%7d,%7d\n"
	    ,type->TileWidth
	    ,type->TileHeight
	    ,type->BoxWidth
	    ,type->BoxHeight
	    ,type->_AttackRange
	    ,type->ReactRangeComputer
	    ,type->ReactRangeHuman);

	printf("\t//Armor\tPrior\tDamage\tPierc\tWUpgr\tAUpgr\n");
	printf("\t,%6d,%5d,%8d,%6d,%7d,%7d\n"
	    ,type->_Armor
	    ,type->Priority
	    ,type->_BasicDamage
	    ,type->_PiercingDamage
	    ,type->WeaponsUpgradable
	    ,type->ArmorUpgradable);

	printf("\t//Weap.\tType\tDecay\tAnnoy\tMouse\tPoints\n");
	printf("\t,%6d,%4d,%8d,%7d,%7d,%8d\n"
	    ,type->MissileWeapon
	    ,type->UnitType
	    ,type->DecayRate
	    ,type->AnnoyComputerFactor
	    ,type->MouseAction
	    ,type->Points);

	printf("\t//Targ\tLand\tAir\tSea\tExplode\tCritter\tBuild\tSubmarine\n");
	printf("\t,%5d,%5d,%6d,%7d,%11d,%7d,%5d,%11d\n"
	    ,type->CanTarget
	    ,type->LandUnit
	    ,type->AirUnit
	    ,type->SeaUnit
	    ,type->ExplodeWhenKilled
	    ,type->Critter
	    ,type->Building
	    ,type->Submarine);

	printf("\t//SeeSu\tCowerP\tTanker\tTrans\tGOil\tSOil\tVanish\tGAttack\n");
	printf("\t,%6d,%6d,%7d,%6d,%6d,%7d,%9d,%8d\n"
	    ,type->CanSeeSubmarine
	    ,type->CowerPeon
	    ,type->Tanker
	    ,type->Transporter
	    ,type->GivesOil
	    ,type->StoresGold
	    ,type->Vanishes
	    ,type->GroundAttack);

	printf("\t//Udead\tShore\tSpell\tSWood\tCanAtt\tTower\tOlPatch\tGoldmine\n");
	printf("\t,%6d,%5d,%7d,%7d,%8d,%6d,%9d,%8d\n"
	    ,type->IsUndead
	    ,type->ShoreBuilding
	    ,type->CanCastSpell
	    ,type->StoresWood
	    ,type->CanAttack
	    ,type->Tower
	    ,type->OilPatch
	    ,type->GoldMine);

	printf("\t//Hero\tSOil\tExplode\tCowerM\tOrganic\tSelect\n");
	printf("\t,%5d,%5d,%10d,%6d,%8d,%6d\n"
	    ,type->Hero
	    ,type->StoresOil
	    ,type->Explodes
	    ,type->CowerMage
	    ,type->Organic
	    ,type->SelectableByRectangle);

	if( !type->Buttons && 0 ) {
	    printf("   ,NULL\t\t// buttons\n");
	} else {
	    printf("   ,_%sButtons\t// buttons\n"
		,UnitTypeNames[i]+4);
	}

	printf("   ,{\t\t// sound\n");
	printf("\t { \"%s\" }\n",type->Sound.Selected.Name);
	printf("\t,{ \"%s\" }\n",type->Sound.Acknowledgement.Name);
	printf("\t,{ \"%s\" }\n",type->Sound.Ready.Name);
	printf("\t,{ \"%s\" }\n",type->Sound.Help.Name);
	printf("\t,{ \"%s\" }\n",type->Sound.Dead.Name);
#if 0
	printf("\t { \"%s-selected\" }\n",type->Ident);
	printf("\t,{ \"%s-acknowledge\" }\n",type->Ident);
	printf("\t,{ \"%s-ready\" }\n",type->Ident);
	printf("\t,{ \"%s-help\" }\n",type->Ident);
	printf("\t,{ \"%s-dead\" }\n",type->Ident);
	//FIXME: add here autogeneration of attack information (at least sound)
#endif
	printf("   },");
	printf("   {");
	printf("\t { \"%s\" }\n",type->Weapon.Attack.Name);
	printf("   }");
	printf(" },\n");
    }
}

/**
**	Update the player stats for changed unit types.
*/
global void UpdateStats(void)
{
    UnitType* type;
    UnitStats* stats;
    unsigned player;

    //
    //	Update players stats
    //
    for( type=UnitTypes;
	    type<&UnitTypes[sizeof(UnitTypes)/sizeof(*UnitTypes)]; ++type ) {
	for( player=0; player<PlayerMax; ++player ) {
	    stats=&type->Stats[player];
	    stats->AttackRange=type->_AttackRange;
	    stats->SightRange=type->_SightRange;
	    stats->Armor=type->_Armor;
	    stats->BasicDamage=type->_BasicDamage;
	    stats->PiercingDamage=type->_PiercingDamage;
	    stats->Speed=type->_Speed;
	    stats->HitPoints=type->_HitPoints;
	    stats->Costs[TimeCost]=type->_Costs[TimeCost];
	    stats->Costs[GoldCost]=type->_Costs[GoldCost];
	    stats->Costs[WoodCost]=type->_Costs[WoodCost];
	    stats->Costs[OilCost]=type->_Costs[OilCost];
	    stats->Level=1;
	}
    }
}

#define ReadByte()   (*((unsigned char*)udta)++)

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
#define ReadWord()   bswap_16(*((unsigned short*)udta)++)
#define ReadLong()   bswap_32(*((unsigned long*)udta)++)
#else
#define ReadWord()   (*((unsigned short*)udta)++)
#define ReadLong()   (*((unsigned long*)udta)++)
#endif

/**
**	Parse UDTA area from puds.
**
**	@param udta	Pointer to udta area.
**	@param length	length of udta area.
*/
global void ParsePudUDTA(const char* udta,int length)
{
    int i;
    int v;
    const char* start;
    UnitType* unittype;

    // FIXME: not the fastest, remove UnitTypeByWcNum from loops!
    if( length!=5694 && length!=5948 ) {
	DebugLevel0("\n"__FUNCTION__": ***\n"__FUNCTION__": %d\n",length);
	DebugLevel0(__FUNCTION__": ***\n\n");
    }
    start=udta;

    for( i=0; i<110; ++i ) {		// overlap frames
	unittype=UnitTypeByWcNum(i);
	v=ReadWord();
	unittype->OverlapFrame=v;
    }
    for( i=0; i<508; ++i ) {		// skip obselete data
	v=ReadWord();
    }
    for( i=0; i<110; ++i ) {		// sight range
	unittype=UnitTypeByWcNum(i);
	v=ReadLong();
	unittype->_SightRange=v;
    }
    for( i=0; i<110; ++i ) {		// hit points
	unittype=UnitTypeByWcNum(i);
	v=ReadWord();
	unittype->_HitPoints=v;
    }
    for( i=0; i<110; ++i ) {		// Flag if unit is magic
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->Magic=v;
    }
    for( i=0; i<110; ++i ) {		// Build time * 6 = one second FRAMES
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->_Costs[TimeCost]=v;
    }
    for( i=0; i<110; ++i ) {		// Gold cost / 10
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->_Costs[GoldCost]=v*10;
    }
    for( i=0; i<110; ++i ) {		// Lumber cost / 10
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->_Costs[WoodCost]=v*10;
    }
    for( i=0; i<110; ++i ) {		// Oil cost / 10
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->_Costs[OilCost]=v*10;
    }
    for( i=0; i<110; ++i ) {		// Unit size in tiles
	unittype=UnitTypeByWcNum(i);
	v=ReadWord();
	unittype->TileWidth=v;
	v=ReadWord();
	unittype->TileHeight=v;
    }
    for( i=0; i<110; ++i ) {		// Box size in pixel
	unittype=UnitTypeByWcNum(i);
	v=ReadWord();
	unittype->BoxWidth=v;
	v=ReadWord();
	unittype->BoxHeight=v;
    }

    for( i=0; i<110; ++i ) {		// Attack range
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->_AttackRange=v;
    }
    for( i=0; i<110; ++i ) {		// React range
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->ReactRangeComputer=v;
    }
    for( i=0; i<110; ++i ) {		// React range
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->ReactRangeHuman=v;
    }
    for( i=0; i<110; ++i ) {		// Armor
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->_Armor=v;
    }
    for( i=0; i<110; ++i ) {		// Selectable via rectangle
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->SelectableByRectangle=v!=0;
    }
    for( i=0; i<110; ++i ) {		// Priority
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->Priority=v;
    }
    for( i=0; i<110; ++i ) {		// Basic damage
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->_BasicDamage=v;
    }
    for( i=0; i<110; ++i ) {		// Piercing damage
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->_PiercingDamage=v;
    }
    for( i=0; i<110; ++i ) {		// Weapons upgradable
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->WeaponsUpgradable=v;
    }
    for( i=0; i<110; ++i ) {		// Armor upgradable
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->ArmorUpgradable=v;
    }
    for( i=0; i<110; ++i ) {		// Missile Weapon
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->MissileWeapon=v;
    }
    for( i=0; i<110; ++i ) {		// Unit type
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->UnitType=v;
    }
    for( i=0; i<110; ++i ) {		// Decay rate * 6 = secs
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->DecayRate=v;
    }
    for( i=0; i<110; ++i ) {		// Annoy computer factor
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->AnnoyComputerFactor=v;
    }
    for( i=0; i<58; ++i ) {		// 2nd mouse button action
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->MouseAction=v;
    }
    for( ; i<110; ++i ) {		// 2nd mouse button action
	unittype=UnitTypeByWcNum(i);
	unittype->MouseAction=0;
    }
    for( i=0; i<110; ++i ) {		// Point value for killing unit
	unittype=UnitTypeByWcNum(i);
	v=ReadWord();
	unittype->Points=v;
    }
    for( i=0; i<110; ++i ) {		// Can target (1 land, 2 sea, 4 air)
	unittype=UnitTypeByWcNum(i);
	v=ReadByte();
	unittype->CanTarget=v;
    }

    for( i=0; i<110; ++i ) {		// Flags
	unittype=UnitTypeByWcNum(i);
	v=ReadLong();
	// unittype->Flags=v;
#define BIT(b,v)	(((v>>b))&1)
	unittype->LandUnit=BIT(0,v);
	unittype->AirUnit=BIT(1,v);
	unittype->ExplodeWhenKilled=BIT(2,v);
	unittype->SeaUnit=BIT(3,v);
	unittype->Critter=BIT(4,v);
	unittype->Building=BIT(5,v);
	unittype->Submarine=BIT(6,v);
	unittype->CanSeeSubmarine=BIT(7,v);
	unittype->CowerPeon=BIT(8,v);
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
	unittype->Explodes=BIT(25,v);
	UnitTypes[i].CowerMage=BIT(26,v);
	UnitTypes[i].Organic=BIT(27,v);
	if( BIT(28,v) )	DebugLevel0("Unused bit 28 used in %d\n",i);
	if( BIT(29,v) )	DebugLevel0("Unused bit 29 used in %d\n",i);
	if( BIT(30,v) )	DebugLevel0("Unused bit 30 used in %d\n",i);
	if( BIT(31,v) )	DebugLevel0("Unused bit 31 used in %d\n",i);
#undef BIT
    }
    DebugLevel0("\tUDTA used %Zd bytes\n",udta-start);

    UpdateStats();
}

/**
**	Save state of the unit-type table to file.
**
**	@param file	Output file.
*/
global void SaveUnitTypes(FILE* file)
{
    fprintf(file,"\n;;; -----------------------------------------\n");
    fprintf(file,";;; MODULE: unittypes $Id: unittype.c,v 1.40 1999/12/09 16:27:15 root Exp $\n");

    // FIXME: More todo
/*
    Unit* unit;

    for( unit=Units; unit<&Units[NumUnits]; ++unit ) {
	if( !unit->Type ) {		// unused slot
	    continue;
	}
	SaveUnit(unit,file);
    }
*/
}

/**
**	Find unit-type by wc number.
**
**	@param num	The unit-type number used in f.e. puds.
**	@return		Unit-type pointer.
*/
global UnitType* UnitTypeByWcNum(int num)
{
    // FIXME: This works only with the current version
    return &UnitTypes[num];
}

/**
**	Find unit-type by identifier.
**
**	@param ident	The unit-type identifier.
**	@return		Unit-type pointer.
*/
global UnitType* UnitTypeByIdent(const char* ident)
{
    unsigned type;

    DebugLevel3(__FUNCTION__": Searching name %s\n",ident);

    // FIXME: This is the slow method, we should use hash/rb-trees.
    for( type=0; type<sizeof(UnitTypes)/sizeof(*UnitTypes); ++type ) {
	if( !strcmp(ident,UnitTypes[type].Ident) ) {
	    DebugLevel3(__FUNCTION__": This are the good names %s\n",ident);
	    return &UnitTypes[type];
	}
    }
    IfDebug(
    for( type=0; UnitTypeNames[type]; ++type ) {
	if( !strcmp(ident,UnitTypeNames[type]) ) {
	    DebugLevel0(__FUNCTION__": This are the wrong names %s\n",ident);
	    return &UnitTypes[type];
	}
    }
    );

    DebugLevel0(__FUNCTION__": Name %s not found\n",ident);

    return NULL;
}

/**
**	Init unit types.
*/
global void InitUnitTypes(void)
{
    unsigned type;

    for( type=0; type<sizeof(UnitTypes)/sizeof(*UnitTypes); ++type ) {
	//
	//	Initialize:
	//
	UnitTypes[type].Type=type;
    }
}

/**
**	Load the graphics for the units.
*/
global void LoadUnitSprites(void)
{
    UnitType* unittype;
    unsigned type;
    const char* file;

    for( type=0; type<sizeof(UnitTypes)/sizeof(*UnitTypes); ++type ) {
	//
	//	Unit-type uses the same sprite as an other.
	//
	if( UnitTypes[type].SameSprite ) {
	    continue;
	}

	file=UnitTypes[type].File[TheMap.Terrain];
	if( !file ) {			// default one
	    file=UnitTypes[type].File[0];
	}
	if( file ) {
	    char* buf;

	    buf=alloca(strlen(file)+9+1);
	    file=strcat(strcpy(buf,"graphic/"),file);
	    ShowLoadProgress("\tUnit `%s'\n",file);
	    UnitTypes[type].RleSprite=LoadRleSprite(file
		    ,UnitTypes[type].Width,UnitTypes[type].Height);
	}
    }

    for( type=0; type<sizeof(UnitTypes)/sizeof(*UnitTypes); ++type ) {
	//
	//	Unit-type uses the same sprite as an other.
	//
	if( UnitTypes[type].SameSprite ) {
	    unittype=UnitTypeByIdent(UnitTypes[type].SameSprite);
	    if( !unittype ) {
		fprintf(stderr,__FUNCTION__": unit-type %s not found\n"
			,UnitTypes[type].SameSprite);
		exit(-1);
	    }
	    UnitTypes[type].RleSprite=unittype->RleSprite;
	}

	//
	//	Lookup icons.
	//
	UnitTypes[type].Icon.Icon=IconByIdent(UnitTypes[type].Icon.Name);
    }

    // FIXME: Must build hash-tables

    // FIXME: must copy unit data from peon/peasant to with gold/wood
    // FIXME: must copy unit data from tanker to tanker full
}

/**
**	Draw unit on map.
*/
global void DrawUnitType(UnitType* type,unsigned frame,int x,int y)
{
    DebugLevel3("%s\n",type->Name);

    x-=(type->Width-type->TileWidth*TileSizeX)/2;
    y-=(type->Height-type->TileHeight*TileSizeY)/2;

    // FIXME: This is a hack for mirrored sprites
    if( frame&128 ) {
	DrawRleSpriteClippedX(type->RleSprite,frame&127,x,y);
    } else {
	DrawRleSpriteClipped(type->RleSprite,frame,x,y);
    }
}

//@}
