/*
**	A clone of a famous game.
*/
/**@name upgrade.h	-	The upgrades headerfile. */
/*
**	(c) Copyright 1999 by Vladi Belperchinov-Shabanski
**
**	$Id: upgrade.h,v 1.8 1999/12/17 16:03:52 root Exp $
*/

#ifndef __UPGRADE_H__
#define __UPGRADE_H__

//@{

/*

  Small help notes -- How to use upgrades -- Demo
  ----------------------------------------------------------------------------
  This is just a demo -- perhaps is not usefull IRL

  // start

  UpgradesInit(); // should be called in the beginning

  uid=AddUpgrade( "UpgradeBerserker", 100, 200, 300, 0, IconUpgradeBerserkerId)
  AddUpgradeModifier( uid,
	  1, 1,    // more sight
	  +10, +5, // more damage
	  -5,      // less armor
	  0, 0,    // speed and HP are the same
	  0, 0, 0, 0, // costs are the same
		 // allow berserker and forbid axethrower
	  "A:UnitBerserker,F:UnitAxeThrower",
		  // allows BerserkerRange1 upgrade
	  "A:UpgradeBerserkerRange1",
		  // there are no allow/frobid actions
	  "", 
		  // apply to Berserker units
	  "UnitBerserker"
  );
		
  UpgradesDone(); // this should be called at the end of the game

  // end

  this is general idea, you can have multiple upgrade modifiers for
  applying to different units and whatever you like

*/

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include "player.h"

/*----------------------------------------------------------------------------
--	Declartion
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

// CHAOS PUR

/*----------------------------------------------------------------------------
--	Units/Upgrades Dependencies
----------------------------------------------------------------------------*/

// FIXME: this dependencies code should be improved to support and OR rules

#define MAX_DEPS    2048

#define dtUnit    1
#define dtUpgrade 2
struct DependRule
{
  unsigned char TargetType; // dtUnit or dtUpgrade
  int Target; // unit/upgrade id

  unsigned char RequiredType; // dtUnit or dtUpgrade
  int Required; // unit/upgrade id
  unsigned char RequiredCount; // required units count (255 max)
};

extern struct DependRule   DependRules[MAX_DEPS];
extern int DependCount;

extern void AddDepend( const char* Target, const char* Required, int Count );
extern int CheckDepend( Player* player, const char* CharTarget ); // returns 1 for ok and 0 for disallow

extern void InitDeps(void);

/*----------------------------------------------------------------------------
--	Init/Done/Add functions
----------------------------------------------------------------------------*/

extern void UpgradesInit(void);	/// init upgrade/allow structures
extern void UpgradesDone(void);	/// free upgrade/allow structures

// returns upgrade id or -1 for error
int AddUpgrade( const char* aIdent,
  int aTimeCost,
  int aGoldCost,
  int aWoodCost,
  int aOilCost,
  const char* aIcon );

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
  int alumber_cost,
  int aoil_cost,

  // following are comma separated list of required string id's

  const char* aaf_units,    // "A:UnitMage,F:UnitGrunt" -- allow mages, forbid grunts
  const char* aaf_actions,  // "A:PeonAttack"
  const char* aaf_upgrades, // "F:UpgradeShield1"
  const char* aapply_to	    // "UnitPeon,UnitPeasant"

  );

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
  const char* aapply_to	    // "UnitPeon,UnitPeasant"
  );

/*----------------------------------------------------------------------------
--	General/Map functions
----------------------------------------------------------------------------*/

extern int ConvertUGRD( void* buf ); // called by load_pud?
extern int ConvertALOW( void* buf ); // called by load_pud?

extern int UpgradesLoad(); // map load
extern int UpgradesSave(); // map save

// AllowStruct and UpgradeTimers will be static in the player so will be
// load/saved with the player struct

extern int UnitTypeIdByIdent( const char* sid );
extern int UpgradeIdByIdent( const char* sid );
extern int ActionIdByIdent( const char* sid );

/*----------------------------------------------------------------------------
--	Upgrades
----------------------------------------------------------------------------*/

// amount==-1 to cancel upgrade, could happen when building destroyed during upgrade
// using this we could have one upgrade research in two buildings, so we can have
// this upgrade faster.
void UpgradeIncTime( Player* player, int id, int amount );
void UpgradeIncTime2( Player* player, char* sid, int amount ); // by ident string

// this function will mark upgrade done and do all required modifications to
// unit types and will modify allow/forbid maps
void UpgradeAcquire( Player* player, int id ); // called by UpgradeIncTime() when timer reached
void UpgradeAcquire2( Player* player, char* sid ); // by ident string

// for now it will be empty?
// perhaps acquired upgrade can be lost if ( for example ) a building is lost
// ( lumber mill? stronghold? )
// this function will apply all modifiers in reverse way
void UpgradeLost( Player* player, int id );
void UpgradeLost2( Player* player, char* sid ); // by ident string

/*----------------------------------------------------------------------------
--	Allow(s)
----------------------------------------------------------------------------*/

// all the following functions are just map handlers, no specific notes
void AllowUnitId( Player* player, int id, char af ); // id -- unit type id, af -- `A'llow/`F'orbid
void AllowUnitIdent( Player* player,  char* sid, char af );

void AllowActionId( Player* player,  int id, char af );
void AllowActionIdent( Player* player,  char* sid, char af );

void AllowUpgradeId( Player* player,  int id, char af );
void AllowUpgradeIdent( Player* player,  char* sid, char af );

char UnitIdAllowed( Player* player,  int id );
char UnitIdentAllowed( Player* player,  char* sid );

char ActionIdAllowed( Player* player,  int id );
char ActionIdentAllowed( Player* player,  char* sid );

char UpgradeIdAllowed( Player* player,  int id );
char UpgradeIdentAllowed( Player* player,  char* sid );

/*----------------------------------------------------------------------------
--	eof
----------------------------------------------------------------------------*/
//@}

#endif	// !__UPGRADE_H__
