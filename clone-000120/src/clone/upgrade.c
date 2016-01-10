/*
**	A clone of a famous game.
*/
/**@name upgrade.c	-	The upgrade/allow headerfile. */
/*
**	(c) Copyright 1999 by Vladi Belperchinov-Shabanski
**
**	$Id: upgrade.c,v 1.8 1999/12/17 16:03:52 root Exp $
*/

//@{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clone.h"
#include "upgrade_structs.h"
#include "upgrade.h"
#include "player.h"

/*----------------------------------------------------------------------------
--	Units/Upgrades Dependencies
----------------------------------------------------------------------------*/

local struct DependRule   DependRules[MAX_DEPS];
local int DependCount;

global void AddDepend( const char* target, const char* Required, int Count )
{
    if ( DependCount == MAX_DEPS ) {
	// DependRules table is full... should we run error?
	return;
    }

  if ( strncmp( target, "unit-", 5 ) == 0 )
    { // target string refers to unit-xxx
    DependRules[DependCount].Target = UnitTypeIdByIdent( target );
    DependRules[DependCount].TargetType = dtUnit;
    };
  if ( strncmp( target, "upgrade-", 8 ) == 0 )
    { // target string refers to upgrade-XXX
    DependRules[DependCount].Target = UpgradeIdByIdent( target );
    DependRules[DependCount].TargetType = dtUpgrade;
    };
  if ( strncmp( Required, "unit-", 5 ) == 0 )
    { // required string refers to unit-xxx
    DependRules[DependCount].Required = UnitTypeIdByIdent( Required );
    DependRules[DependCount].RequiredType = dtUnit;
    };
  if ( strncmp( Required, "upgrade-", 8 ) == 0 )
    { // required string refers to upgrade-XXX
    DependRules[DependCount].Required = UpgradeIdByIdent( Required );
    DependRules[DependCount].RequiredType = dtUpgrade;
    };
    if ( Count < 0 || Count > 255 ) {
	Count = 255;
    }
  DependRules[DependCount].RequiredCount = Count; // used only if RequiredType is dtUnit!
  DependCount++;
}

/**
**	Check if this upgrade or unit is availalbe.
**
**	@param player	For this player available.
**	@param target	Unit or Upgrade.
**		
**	@return 	True if available, false otherwise.	
*/
global int CheckDepend(Player* player, const char* target)
{
  int z;
  int id; // target id
  int dt; // target type [ dtUnit | dtUpgrade ]

  // first have to check if target is allowed itself

  if ( strncmp( target, "unit-", 5 ) == 0 )
    { // target string refers to unit-xxx
    id = UnitTypeIdByIdent( target );
    if ( UnitIdAllowed( player, id ) != 'A' ) return 0;
    dt = dtUnit;
    } else
  if ( strncmp( target, "upgrade-", 8 ) == 0 )
    { // target string refers to upgrade-XXX
    id = UpgradeIdByIdent( target );
    z=UpgradeIdAllowed( player, id );
    if ( z != 'R' && z != 'A' ) return 0;
    dt = dtUpgrade;
    } else
  return 0; //FIXME: should this be an error?! this target string is neither Unit nor Upgrade

  // now check dependencies
  for ( z = 0; z < DependCount; z++ )
    {
    if ( DependRules[z].TargetType == dt && DependRules[z].Target == id )
      { // target match found
      if (  DependRules[z].RequiredType == dtUnit )
	{
	DebugLevel3(__FUNCTION__": %s %d\n"
	    ,(UnitTypes+DependRules[z].Required)->Ident
	    ,HaveUnitTypeByType( player, UnitTypes+DependRules[z].Required ));
	if ( HaveUnitTypeByType( player, UnitTypes+DependRules[z].Required )
		< DependRules[z].RequiredCount )
	    return 0; // don't have required count of units -- forbidden
	}
      else
	{
	if ( UpgradeIdAllowed( player, DependRules[z].Required ) != 'R' )
	    return 0;  // not researched -- forbidden
	}
      }
    }
  return 1; // there is no target matching at all or all target matches are passed ok
}

/**
**	Initialize unit and upgrade dependencies.
**
**	FIXME:	Some problems:
**		ballista should be possible if *any* lumber mill is available
**		also an "or" function whould be usefull.
*/
global void InitDeps(void)
{
    // human naval units
    AddDepend( "unit-human-transport", "unit-elven-lumber-mill", 1 );
    AddDepend( "unit-battleship", "unit-human-foundry", 1 );
    AddDepend( "unit-gnomish-submarine", "unit-gnomish-inventor", 1 );

    // human land forces
    AddDepend( "unit-archer", "unit-elven-lumber-mill", 1 );
    AddDepend( "unit-ranger", "upgrade-ranger", 0 );
    AddDepend( "unit-ranger", "unit-elven-lumber-mill", 1 );

    AddDepend( "unit-knight", "unit-human-blacksmith", 1 );
    AddDepend( "unit-knight", "unit-stables", 1 );
    AddDepend( "unit-paladin", "upgrade-paladin", 0 );
    AddDepend( "unit-paladin", "unit-human-blacksmith", 1 );
    AddDepend( "unit-paladin", "unit-stables", 1 );

    AddDepend( "unit-ballista", "unit-elven-lumber-mill", 1 );
    AddDepend( "unit-ballista", "unit-human-blacksmith", 1 );

    // human flying ones
    AddDepend( "unit-gnomish-flying-machine", "unit-elven-lumber-mill", 1 );

    // human buildings
    AddDepend("unit-human-guard-tower", "unit-elven-lumber-mill", 1 );
    AddDepend("unit-human-cannon-tower", "unit-human-blacksmith", 1 );

    AddDepend("unit-human-foundry", "unit-human-shipyard", 1 );
    AddDepend("unit-human-refinery", "unit-human-shipyard", 1 );

    AddDepend("unit-mage-tower", "unit-castle", 1 );
    AddDepend("unit-church", "unit-castle", 1 );
    AddDepend("unit-gryphon-aviary", "unit-castle", 1 );

    AddDepend("unit-keep", "unit-human-barracks", 1);

    AddDepend("unit-castle", "unit-elven-lumber-mill", 1);
    AddDepend("unit-castle", "unit-human-blacksmith", 1);
    AddDepend("unit-castle", "unit-stables", 1);

    // human upgrades
    AddDepend( "upgrade-sword2", "upgrade-sword1", 0 );
    AddDepend( "upgrade-arrow2", "upgrade-arrow1", 0 );
    AddDepend( "upgrade-human-shield2", "upgrade-human-shield1", 0 );
    AddDepend( "upgrade-ballista2", "upgrade-ballista1", 0 );

    AddDepend( "upgrade-ranger", "unit-castle", 1 );
    AddDepend( "upgrade-ranger-scouting", "upgrade-ranger", 0 );
    AddDepend( "upgrade-longbow", "upgrade-ranger", 0 );
    AddDepend( "upgrade-ranger-marksmanship", "upgrade-ranger", 0 );

    AddDepend( "upgrade-holy-vision", "upgrade-paladin", 0 );
    AddDepend( "upgrade-heal", "upgrade-paladin", 0 );
    AddDepend( "upgrade-exorcism", "upgrade-paladin", 0 );

    AddDepend( "upgrade-human-ship-cannon2", "upgrade-human-ship-cannon1", 0 );
    AddDepend( "upgrade-human-ship-armor2", "upgrade-human-ship-armor1", 0 );

    // ------------------------------------------------------------------------

    // orcs naval units
    AddDepend( "unit-orc-transport", "unit-orc-foundry", 1 );
    AddDepend( "unit-ogre-juggernaught", "unit-orc-foundry", 1 );
    AddDepend( "unit-giant-turtle", "unit-goblin-alchemist", 1 );

    // orcs land forces
    AddDepend( "unit-ogre", "unit-orc-blacksmith", 1 );
    AddDepend( "unit-ogre", "unit-ogre-mound", 1 );
    AddDepend( "unit-ogre-mage", "upgrade-ogre-mage", 0 );
    AddDepend( "unit-ogre-mage", "unit-orc-blacksmith", 1 );
    AddDepend( "unit-ogre-mage", "unit-ogre-mound", 1 );

    AddDepend( "unit-axethrower", "unit-troll-lumber-mill", 1 );
    AddDepend( "unit-berserker", "upgrade-berserker", 0 );
    AddDepend( "unit-berserker", "unit-troll-lumber-mill", 1 );

    AddDepend( "unit-catapult", "unit-troll-lumber-mill", 1 );
    AddDepend( "unit-catapult", "unit-orc-blacksmith", 1 );

    // orc flying ones
    AddDepend( "unit-goblin-zeppelin", "unit-troll-lumber-mill", 1 );

    // orc buildings
    AddDepend( "unit-orc-shipyard", "unit-troll-lumber-mill", 1 );

    // FIXME: this needs or or one time researched
    //AddDepend( "unit-orc-mound", "unit-stronghold", 1 );
    //AddDepend( "unit-alchemist", "unit-stronghold", 1 );

    AddDepend( "unit-orc-guard-tower", "unit-troll-lumber-mill", 1 );
    AddDepend( "unit-orc-cannon-tower", "unit-orc-blacksmith", 1 );

    AddDepend( "unit-orc-foundry", "unit-orc-shipyard", 1 );
    AddDepend( "unit-orc-refinery", "unit-orc-shipyard", 1 );

    AddDepend( "unit-temple-of-the-damned", "unit-fortress", 1 );
    AddDepend( "unit-altar-of-storms", "unit-fortress", 1 );
    AddDepend( "unit-dragon-roost", "unit-fortress", 1 );

    AddDepend("unit-stronghold", "unit-orc-barracks", 1);

    AddDepend("unit-fortress", "unit-troll-lumber-mill", 1);
    AddDepend("unit-fortress", "unit-orc-blacksmith", 1);
    AddDepend("unit-fortress", "unit-ogre-mound", 1);

    // orc upgrades
    AddDepend( "upgrade-battle-axe2", "upgrade-battle-axe1", 0 );
    AddDepend( "upgrade-throwing-axe2", "upgrade-throwing-axe1", 0 );
    AddDepend( "upgrade-orc-shield2", "upgrade-orc-shield1", 0 );
    AddDepend( "upgrade-catapult2", "upgrade-catapult1", 0 );

    AddDepend( "upgrade-berserker", "unit-fortress", 1 );
    AddDepend( "upgrade-berserker-scouting", "upgrade-berserker", 0 );
    AddDepend( "upgrade-light-axes", "upgrade-berserker", 0 );
    AddDepend( "upgrade-berserker-regeneration", "upgrade-berserker", 0 );

    AddDepend( "upgrade-eye-of-kilrogg", "upgrade-ogre-mage", 0 );
    AddDepend( "upgrade-bloodlust", "upgrade-ogre-mage", 0 );
    AddDepend( "upgrade-runes", "upgrade-ogre-mage", 0 );

    AddDepend( "upgrade-orc-ship-cannon2", "upgrade-orc-ship-cannon1", 0 );
    AddDepend( "upgrade-orc-ship-armor2", "upgrade-orc-ship-armor1", 0 );

    /*
    AddDepend( "", "", 0 );
    */
}

/*----------------------------------------------------------------------------
--	Init/Done/Add functions
----------------------------------------------------------------------------*/

#define UPGRADE_MODIFIERS_MAX	1024
global Upgrade Upgrades[MAXUACOUNT];
local UpgradeModifier* UpgradeModifiers[UPGRADE_MODIFIERS_MAX];
local int UpgradesCount;
local int UpgradeModifiersCount;


/**
**	Init upgrade/allow structures
*/
global void UpgradesInit(void)
{
    int z;

    // first we have to allow everything
    for ( z = 0; z < PlayerMax; z++ ) {
	memset(Players[z].Allow.upgrades,'A',
		sizeof(Players[z].Allow.upgrades));
	memset(Players[z].Allow.units,'A',
		sizeof(Players[z].Allow.units));
	memset(Players[z].Allow.actions,'A',
		sizeof(Players[z].Allow.actions));
    }

     // now forbid some units...
     // ...well none yet -- most of this is handled by dependacies above
     // however it is not the same! perhaps could be used in the future
     //		(map restrictions)

 // NOTE: Unit statistics are relative not absolute! So if you enter 10 for
 // AttackRange for the Ballista ( for example ) you will *ADD* 10 to current
 // Ballista range but not set it to absolute 10!
 // You may enter negative numbers as well! (Downgrades)

 //FIXME: URGENT: the following upgrades stats/modifiers are not ALL correct/complete!
 //									AttackRange, SightRange, BasicDamage, PiercingDamage, Armor, Speed, HitPoints, ApplyToUnitsList
 //			Name				Time   Gold  Wood Oil   Icon      At Si BD PD Ar Sp HP
 AddSimpleUpgrade( "upgrade-sword1"			, 200,  800,   0,   0,      "icon-sword2", 0, 0, 0, 2, 0, 0, 0, "unit-footman,unit-knight,unit-paladin" );
 AddSimpleUpgrade( "upgrade-sword2"			, 250, 2400,   0,   0,      "icon-sword3", 0, 0, 0, 2, 0, 0, 0, "unit-footman,unit-knight,unit-paladin" );
 AddSimpleUpgrade( "upgrade-battle-axe1"		, 200,  500, 100,   0,      "icon-battle-axe2", 0, 0, 0, 2, 0, 0, 0, "unit-grunt,unit-ogre,unit-ogre-mage" );
 AddSimpleUpgrade( "upgrade-battle-axe2"		, 250, 1500, 300,   0,      "icon-battle-axe3", 0, 0, 0, 2, 0, 0, 0, "unit-grunt,unit-ogre,unit-ogre-mage" );
 AddSimpleUpgrade( "upgrade-arrow1"			, 200,  300, 300,   0,      "icon-arrow2", 0, 0, 0, 1, 0, 0, 0, "unit-archer,unit-ranger" );
 AddSimpleUpgrade( "upgrade-arrow2"			, 250,  900, 500,   0,      "icon-arrow3", 0, 0, 0, 1, 0, 0, 0, "unit-archer,unit-ranger" );
 AddSimpleUpgrade( "upgrade-throwing-axe1"		, 200,  300, 300,   0,      "icon-throwing-axe2", 0, 0, 0, 1, 0, 0, 0, "unit-axethrower,unit-berserker" );
 AddSimpleUpgrade( "upgrade-throwing-axe2"		, 250,  900, 500,   0,      "icon-throwing-axe3", 0, 0, 0, 1, 0, 0, 0, "unit-axethrower,unit-berserker" );
 AddSimpleUpgrade( "upgrade-human-shield1"		, 200,  300, 300,   0,      "icon-human-shield2", 0, 0, 0, 0, 2, 0, 0, "unit-footman,unit-knight,unit-paladin" );
 AddSimpleUpgrade( "upgrade-human-shield2"		, 250,  900, 500,   0,      "icon-human-shield3", 0, 0, 0, 0, 2, 0, 0, "unit-footman,unit-knight,unit-paladin" );
 AddSimpleUpgrade( "upgrade-orc-shield1"		, 200,  300, 300,   0,      "icon-orc-shield2", 0, 0, 0, 0, 2, 0, 0, "unit-grunt,unit-ogre,unit-ogre-mage" );
 AddSimpleUpgrade( "upgrade-orc-shield2"		, 250,  900, 500,   0,      "icon-orc-shield3", 0, 0, 0, 0, 2, 0, 0, "unit-grunt,unit-ogre,unit-ogre-mage" );
 AddSimpleUpgrade( "upgrade-human-ship-cannon1"		, 200,  700, 100,1000,      "icon-human-ship-cannon2", 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-human-ship-cannon2"		, 250, 2000, 250,3000,      "icon-human-ship-cannon3", 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-orc-ship-cannon1"		, 200,  700, 100,1000,      "icon-orc-ship-cannon2", 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-orc-ship-cannon2"		, 250, 2000, 250,3000,      "icon-orc-ship-cannon3", 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-human-ship-armor1"		, 200,  500, 500,   0,      "icon-human-ship-armor2", 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-human-ship-armor2"		, 250, 1500, 900,   0,      "icon-human-ship-armor3", 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-orc-ship-armor1"		, 200,  500, 500,   0,      "icon-orc-ship-armor2", 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-orc-ship-armor2"		, 250, 1500, 900,   0,      "icon-orc-ship-armor3", 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-catapult1"			, 250, 1500,   0,   0,      "icon-catapult1", 0, 0, 0,15, 0, 0, 0, "unit-catapult" );
 AddSimpleUpgrade( "upgrade-catapult2"			, 250, 4000,   0,   0,      "icon-catapult2", 1, 0, 0,15, 0, 0, 0, "unit-catapult" );
 AddSimpleUpgrade( "upgrade-ballista1"			, 250, 1500,   0,   0,      "icon-ballista1", 0, 0, 0,15, 0, 0, 0, "unit-ballista" );
 AddSimpleUpgrade( "upgrade-ballista2"			, 250, 4000,   0,   0,      "icon-ballista2", 1, 0, 0,15, 0, 0, 0, "unit-ballista" );
 AddSimpleUpgrade( "upgrade-ranger"			, 250, 1500,   0,   0,      NULL, 0, 0, 0, 0, 1, 0, 0, "unit-archer,unit-ranger" );
 AddSimpleUpgrade( "upgrade-longbow"			, 250, 2000,   0,   0,      NULL, 0, 1, 0, 0, 0, 0, 0, "unit-archer,unit-ranger" );
 AddSimpleUpgrade( "upgrade-ranger-scouting"		, 250, 1500,   0,   0,      NULL, 3, 0, 0, 0, 0, 0, 0, "unit-archer,unit-ranger" );
 AddSimpleUpgrade( "upgrade-ranger-marksmanship"	, 250, 2500,   0,   0,      NULL, 0, 0, 0, 3, 0, 0, 0, "unit-archer,unit-ranger" );
 AddSimpleUpgrade( "upgrade-berserker"			, 250, 1500,   0,   0,      NULL, 0, 0, 0, 0, 1, 0, 0, "unit-axethrower,unit-berserker" );
 AddSimpleUpgrade( "upgrade-light-axes"			, 250, 2000,   0,   0,      NULL, 0, 1, 0, 0, 0, 0, 0, "unit-axethrower,unit-berserker" );
 AddSimpleUpgrade( "upgrade-berserker-scouting"		, 250, 1500,   0,   0,      NULL, 3, 0, 0, 0, 0, 0, 0, "unit-axethrower,unit-berserker" );
 AddSimpleUpgrade( "upgrade-berserker-regeneration"	, 250, 3000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "unit-axethrower,unit-berserker" );
 AddSimpleUpgrade( "upgrade-ogre-mage"			, 250, 1000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-paladin"			, 250, 1000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-holy-vision"		,   0,    0,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-heal"			, 200, 1000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-exorcism"			, 200, 2000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-flame-shield"		, 100, 1000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-fireball"			,   0,    0,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-slow"			, 100,  500,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-invisibility"		, 200, 2500,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-polymorph"			, 200, 2000,   0,   0, 	    NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-blizzard"			, 200, 2000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-eye-of-kilrogg"		,   0,    0,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-bloodlust"			, 100, 1000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-raise-dead"			, 100, 1500,   0,   0,	    NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-death-coil"			, 100,    0,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-whirlwind"			, 150, 1500,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-haste"			, 100,  500,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-unholy-armor"		, 200, 2500,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-runes"			, 150, 1000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );
 AddSimpleUpgrade( "upgrade-death-and-decay"		, 200, 2000,   0,   0,      NULL, 0, 0, 0, 0, 0, 0, 0, "" );

    // Give some upgrades as default.
    for ( z = 0; z < PlayerMax; z++ ) {
	AllowUpgradeIdent(&Players[z],"upgrade-death-coil",'R');
	AllowUpgradeIdent(&Players[z],"upgrade-fireball",'R');
	AllowUpgradeIdent(&Players[z],"upgrade-holy-vision",'R');
	AllowUpgradeIdent(&Players[z],"upgrade-eye-of-kilrogg",'R');
    }
}

void UpgradesDone(void) // free upgrade/allow structures
{
  int z;

  memset( &Upgrades, 0, sizeof(Upgrades) );
  UpgradesCount = 0;

  for ( z = 0; z < UpgradeModifiersCount; z++ )
    free( UpgradeModifiers[z] );
  UpgradeModifiersCount = 0;
}


// returns upgrade id or -1 for error
int AddUpgrade( const char* aIdent,
  int aTimeCost,
  int aGoldCost,
  int aWoodCost,
  int aOilCost,
  const char* aIcon )
{
    char buf[256];

    if ( UpgradesCount == MAXUACOUNT ) {
	return -1;
    }
    strcpy( Upgrades[UpgradesCount].Ident, aIdent );
    Upgrades[UpgradesCount].Costs[TimeCost] = aTimeCost;
    Upgrades[UpgradesCount].Costs[GoldCost] = aGoldCost;
    Upgrades[UpgradesCount].Costs[WoodCost] = aWoodCost;
    Upgrades[UpgradesCount].Costs[OilCost] = aOilCost;
    if( aIcon ) {
	Upgrades[UpgradesCount].Icon = IconByIdent(aIcon);
    } else {
	sprintf(buf,"icon-%s",aIdent+8);
	Upgrades[UpgradesCount].Icon = IconByIdent(buf);
    }
    UpgradesCount++;
    return UpgradesCount-1;
}

// returns upgrade modifier id or -1 for error ( actually this id is useless, just error checking )
int AddUpgradeModifier( int aUid,

  int aattack_range,
  int asight_range,
  int abasic_damage,
  int apiercing_damage,
  int aarmor,
  int aspeed,
  int ahit_points,

  int atime_cost,
  int agold_cost,
  int awood_cost,
  int aoil_cost,

  // following are comma separated list of required string id's

  const char* aaf_units,    // "A:unit-mage,F:unit-grunt" -- allow mages, forbid grunts
  const char* aaf_actions,  // "A:PeonAttack"
  const char* aaf_upgrades, // "F:upgrade-Shield1,R:upgrade-ShieldTotal" -- :)
  const char* aapply_to	    // "unit-Peon,unit-Peasant"

  )
{
    char *s1;
    char *s2;
    UpgradeModifier *um;

    um=(UpgradeModifier*)malloc(sizeof(UpgradeModifier));
    if( !um ) {
	return -1;
    }

    um->uid = aUid;

    // get/save stats modifiers
    um->mods.attack_range	= aattack_range;
    um->mods.sight_range	= asight_range;
    um->mods.basic_damage	= abasic_damage;
    um->mods.piercing_damage	= apiercing_damage;
    um->mods.armor		= aarmor;
    um->mods.speed		= aspeed;
    um->mods.hit_points		= ahit_points;

    um->mods.Costs[TimeCost]	= atime_cost;
    um->mods.Costs[GoldCost]	= agold_cost;
    um->mods.Costs[WoodCost]	= awood_cost;
    um->mods.Costs[OilCost]	= aoil_cost;

    // FIXME: all the thing below is sensitive to the format of the string!
    // FIXME: it will be good if things are checked for errors better!
    // FIXME: perhaps the function `strtok()' should be replaced with local one?

    memset( um->af_units,    '?', sizeof(um->af_units)    );
    memset( um->af_actions,  '?', sizeof(um->af_actions)  );
    memset( um->af_upgrades, '?', sizeof(um->af_upgrades) );
    memset( um->apply_to,    '?', sizeof(um->apply_to)    );

    //
    // get allow/forbid's for units
    //
    s1 = strdup( aaf_units );
    DebugCheck(!s1);
    for( s2 = strtok( s1, "," ); s2; s2=strtok( NULL, "," ) ) {
	int id;
	DebugCheck(! (s2[0] == 'A' || s2[0] == 'F' ));
	DebugCheck(! (s2[1] == ':' ));
	id = UnitTypeIdByIdent( s2+2 );
	if ( id == -1 ) {
	    continue;		// should we cancel all and return error?!
	}
	um->af_units[id] = s2[0];
    }
    free(s1);

    //
    // get allow/forbid's for actions
    //
    s1 = strdup( aaf_actions );
    DebugCheck(!s1);
    for( s2 = strtok( s1, "," ); s2; s2=strtok( NULL, "," ) ) {
	int id;
	DebugCheck(!( s2[0] == 'A' || s2[0] == 'F' ));
	DebugCheck(!( s2[1] == ':' ));
	id = ActionIdByIdent( s2+2 );
	if ( id == -1 ) {
	    continue;		// should we cancel all and return error?!
	}
	um->af_actions[id] = s2[0];
    }
    free(s1);

    //
    // get allow/forbid's for upgrades
    //
    s1 = strdup( aaf_upgrades );
    DebugCheck(!s1);
    for( s2 = strtok( s1, "," ); s2; s2=strtok( NULL, "," ) ) {
	int id;
	DebugCheck(!( s2[0] == 'A' || s2[0] == 'F' || s2[0] == 'R' ));
	DebugCheck(!( s2[1] == ':' ));
	id = UpgradeIdByIdent( s2+2 );
	if ( id == -1 ) {
	    continue;		// should we cancel all and return error?!
	}
	um->af_upgrades[id] = s2[0];
    }
    free(s1);

    //
    // get units that are affected by this upgrade
    //
    s1 = strdup( aapply_to );
    DebugCheck(!s1);
    for( s2 = strtok( s1, "," ); s2; s2=strtok( NULL, "," ) ) {
	int id;

	DebugLevel3(__FUNCTION__": %s\n",s2);
	id = UnitTypeIdByIdent( s2 );
	if ( id == -1 ) {
	    break;		// cade: should we cancel all and return error?!
	}
	um->apply_to[id] = 'X'; // something other than '?'
    }
    free(s1);

    UpgradeModifiers[UpgradeModifiersCount] = um;
    UpgradeModifiersCount++;

    return UpgradeModifiersCount-1;
}

// this function is used for define `simple' upgrades
// with only one modifier
int AddSimpleUpgrade( const char* aIdent,
  // upgrade costs
  int aTimeCost,
  int aGoldCost,
  int aWoodCost,
  int aOilCost,
  const char* aIcon,
  // upgrade modifiers
  int aattack_range,
  int asight_range,
  int abasic_damage,
  int apiercing_damage,
  int aarmor,
  int aspeed,
  int ahit_points,
  const char* aapply_to		// "unit-Peon,unit-Peasant"
  )
{
    int uid;

    uid = AddUpgrade( aIdent,aTimeCost,aGoldCost,aWoodCost,aOilCost,aIcon);
    if (uid == -1)  {
	return uid;
    }
    AddUpgradeModifier( uid,aattack_range,asight_range,abasic_damage,
			apiercing_damage,aarmor,aspeed,ahit_points,
			0,0,0,0, // costs are same
			"","","", // no allow/forbid maps
			aapply_to
    );
    return uid;
}

/*----------------------------------------------------------------------------
--	General/Map functions
----------------------------------------------------------------------------*/

int ConvertUGRD( void* buf ) // called by load_pud?
{
  return 0;
}

int ConvertALOW( void* buf ) // called by load_pud?
{
  return 0;
}

int UpgradesLoad() // map load
{
  return 0;
}

int UpgradesSave() // map save
{
  return 0;
}

// AllowStruct and UpgradeTimers will be static in the player so will be
// load/saved with the player struct

global int UnitTypeIdByIdent(const char* sid)
{
    UnitType* type;

    if( (type=UnitTypeByIdent(sid)) ) {
	return type->Type;
    }
    DebugLevel0(__FUNCTION__": fix this %s\n",sid);
    return -1;
}

global int UpgradeIdByIdent( const char* sid )
{
  int z = 0;

  // FIXME: need hash or something like this for this.
  for ( z = 0; z < UpgradesCount; z++ )
    if ( strcmp( Upgrades[z].Ident, sid ) == 0 ) return z;
  DebugLevel0(__FUNCTION__": fix this %s\n",sid);
  return -1;
}

global int ActionIdByIdent( const char* sid )
{
  // FIXME: there's no actions table yet
  DebugLevel0(__FUNCTION__": fix this %s\n",sid);
  return -1;
}

/*----------------------------------------------------------------------------
--	Upgrades
----------------------------------------------------------------------------*/

// amount==-1 to cancel upgrade, could happen when building destroyed during upgrade
// using this we could have one upgrade research in two buildings, so we can have
// this upgrade faster.
void UpgradeIncTime( Player* player, int id, int amount )
{
  player->UTimers.upgrades[id] += amount;
  if ( player->UTimers.upgrades[id] >= Upgrades[id].Costs[TimeCost] )
    {
    player->UTimers.upgrades[id] = Upgrades[id].Costs[TimeCost];
    UpgradeAcquire( player, id );
    }
}

// this function will mark upgrade done and do all required modifications to
// unit types and will modify allow/forbid maps
void ApplyUpgradeModifier( Player* player, UpgradeModifier* um )
{
  int z;
  int pn = player-Players; // player number

  for ( z = 0; z < MAXUACOUNT; z++ )
    {
    // allow/forbid unit types for player
    if ( um->af_units[z] == 'A' ) player->Allow.units[z] = 'A';
    if ( um->af_units[z] == 'F' ) player->Allow.units[z] = 'F';

    // allow/forbid actions for player
    if ( um->af_actions[z] == 'A' ) player->Allow.actions[z] = 'A';
    if ( um->af_actions[z] == 'F' ) player->Allow.actions[z] = 'F';

    // allow/forbid upgrades for player
    if ( player->Allow.upgrades[z] != 'R' )
      { // only if upgrade is not acquired
      if ( um->af_upgrades[z] == 'A' ) player->Allow.upgrades[z] = 'A';
      if ( um->af_upgrades[z] == 'F' ) player->Allow.upgrades[z] = 'F';
      // we can even have upgrade acquired w/o costs
      if ( um->af_upgrades[z] == 'R' ) player->Allow.upgrades[z] = 'R';
      }

    DebugCheck(!( um->apply_to[z] == '?' || um->apply_to[z] == 'X' ));
    if ( um->apply_to[z] == 'X' )
      { // this modifier should be applied to unittype id == z

      DebugLevel3(__FUNCTION__": applied to %d\n",z);
      // upgrade stats
      UnitTypes[z].Stats[pn].AttackRange     += um->mods.attack_range;
      UnitTypes[z].Stats[pn].SightRange      += um->mods.sight_range;
      UnitTypes[z].Stats[pn].BasicDamage     += um->mods.basic_damage;
      UnitTypes[z].Stats[pn].PiercingDamage  += um->mods.piercing_damage;
      UnitTypes[z].Stats[pn].Armor	     += um->mods.armor;
      UnitTypes[z].Stats[pn].Speed	     += um->mods.speed;
      UnitTypes[z].Stats[pn].HitPoints	     += um->mods.hit_points;

      // upgrade costs :)
      UnitTypes[z].Stats[pn].Costs[TimeCost] += um->mods.Costs[TimeCost];
      UnitTypes[z].Stats[pn].Costs[GoldCost] += um->mods.Costs[GoldCost];
      UnitTypes[z].Stats[pn].Costs[WoodCost] += um->mods.Costs[WoodCost];
      UnitTypes[z].Stats[pn].Costs[OilCost]  += um->mods.Costs[OilCost];

      UnitTypes[z].Stats[pn].Level++;
      }
    }
}

 // called by UpgradeIncTime() when timer reached
global void UpgradeAcquire( Player* player, int id )
{
    int z;

    player->UTimers.upgrades[id] = Upgrades[id].Costs[TimeCost];
    AllowUpgradeId( player, id, 'R' );		// research done

    for ( z = 0; z < UpgradeModifiersCount; z++ ) {
	if ( UpgradeModifiers[z]->uid == id ) {
	    ApplyUpgradeModifier( player, UpgradeModifiers[z] );
	}
    }
}

// for now it will be empty?
// perhaps acquired upgrade can be lost if ( for example ) a building is lost
// ( lumber mill? stronghold? )
// this function will apply all modifiers in reverse way
void UpgradeLost( Player* player, int id )
{
  return; //FIXME: remove this if implemented below

  player->UTimers.upgrades[id] = 0;
  AllowUpgradeId( player, id, 'A' ); // research is lost i.e. available
  // FIXME: here we should reverse apply upgrade...
}

/*----------------------------------------------------------------------------
--	Allow(s)
----------------------------------------------------------------------------*/

// all the following functions are just map handlers, no specific notes
void AllowUnitId( Player* player, int id, char af ) // id -- unit type id, af -- `A'llow/`F'orbid
{
  DebugCheck(!( af == 'A' || af == 'F' ));
  player->Allow.units[id] = af;
}

void AllowActionId( Player* player,  int id, char af )
{
  DebugCheck(!( af == 'A' || af == 'F' ));
  player->Allow.actions[id] = af;
}

void AllowUpgradeId( Player* player,  int id, char af )
{
  DebugCheck(!( af == 'A' || af == 'F' || af == 'R' ));
  player->Allow.upgrades[id] = af;
}

char UnitIdAllowed( Player* player,  int id )
{
  if ( id < 0 || id >= MAXUACOUNT ) return 'F';
  return player->Allow.units[id];
}

char ActionIdAllowed( Player* player,  int id )
{
  if ( id < 0 || id >= MAXUACOUNT ) return 'F';
  return player->Allow.actions[id];
}

global char UpgradeIdAllowed( Player* player,  int id )
{
    // JOHNS: Don't be kind, the people should code correct!
    DebugCheck( id < 0 || id >= MAXUACOUNT );

    return player->Allow.upgrades[id];
}

// ***************by sid's
void UpgradeIncTime2( Player* player, char* sid, int amount ) // by ident string
  { UpgradeIncTime( player, UpgradeIdByIdent(sid), amount ); }
void UpgradeAcquire2( Player* player, char* sid ) // by ident string
  { UpgradeAcquire( player, UpgradeIdByIdent(sid) ); }
void UpgradeLost2( Player* player, char* sid ) // by ident string
  { UpgradeLost( player, UpgradeIdByIdent(sid) ); }
void AllowUnitIdent( Player* player,  char* sid, char af )
     { AllowUnitId( player,  UnitTypeIdByIdent(sid), af ); };
void AllowActionIdent( Player* player,  char* sid, char af )
     { AllowActionId( player,  ActionIdByIdent(sid), af ); };
void AllowUpgradeIdent( Player* player,  char* sid, char af )
     { AllowUpgradeId( player,  UpgradeIdByIdent(sid), af ); };
char UnitIdentAllowed( Player* player,  char* sid )
     { return UnitIdAllowed( player,  UnitTypeIdByIdent(sid) ); };
char ActionIdentAllowed( Player* player,  char* sid )
     { return ActionIdAllowed( player,  ActionIdByIdent(sid) ); };
char UpgradeIdentAllowed( Player* player,  char* sid )
     { return UpgradeIdAllowed( player,  UpgradeIdByIdent(sid) ); };

//@}
