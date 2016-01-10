/*
**	A clone of a famous game.
*/
/**@name player.h	-	The player headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: player.h,v 1.23 1999/11/25 00:59:27 root Exp $
*/

#ifndef __PLAYER_H__
#define __PLAYER_H__

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

typedef struct _player_ Player;

#include "upgrade_structs.h"
#include "unittype.h"
#include "unit.h"

/*----------------------------------------------------------------------------
--	Player type
----------------------------------------------------------------------------*/

/**
**	Player structure
*/
struct _player_ {
    unsigned	Player;			/// player as number
    char*	Name;			/// name of non computer
    unsigned	Type;			/// type of player (human,computer,...)
    unsigned	Race;			/// race of player (orc,human,...)
    unsigned	Team;			/// team of player
    unsigned	Ai;			/// AI for computer

    unsigned	X;			/// map tile start X position
    unsigned	Y;			/// map tile start Y position

// FIXME: change to Resource[]
    unsigned	Gold;			/// resources of gold in store
    unsigned	Wood;			/// resources of wood in store
    unsigned	Oil;			/// resources of oil in store

// FIXME: change to Income[]
    unsigned	WoodPerChop;		/// how much gives a piece wood
    unsigned	GoldPerMine;		/// how much gives a piece gold
    unsigned	OilPerHaul;		/// how much gives an oil transport

//    unsigned	UnitFlags[
//	(UnitTypeInternalMax+BitsOf(unsigned)-1)
//	    /BitsOf(unsigned)];		/// flags what units are available
    unsigned    UnitTypesCount[MaxUnitTypes];       /// each type unit count

    unsigned	AiEnabled;		/// handle ai on this computer

    Unit**	Units;			/// units of this player
    unsigned	TotalNumUnits;		/// total # units for units' list.

    unsigned	Food;			/// food available/produced
    unsigned	NumUnits;		/// # units (need food)
    unsigned	NumBuildings;		/// # buildings (don't need food)

    unsigned	Score;			/// points for killing ...

    unsigned	Color;			/// color of units on minimap

    unsigned	UnitColor1;		/// Unit color 1 on map and icons
    unsigned	UnitColor2;		/// Unit color 2 on map and icons
    unsigned	UnitColor3;		/// Unit color 3 on map and icons
    unsigned	UnitColor4;		/// Unit color 4 on map and icons

//  Upgrades/Allows:

    Allow		Allow;		/// Allowed for player
    UpgradeTimers	UTimers;	/// Timer for the upgrades
};

/*
**	Races for the player
*/
#define PlayerRaceHuman		0	/// belongs to human
#define PlayerRaceOrc		1	/// belongs to orc
#define PlayerRaceNeutral	2	/// belongs to none

#define PlayerMaxRaces		2	/// maximal races supported

/*
**	Types for the player
*/
#define PlayerNeutral		2	/// neutral
#define PlayerNobody		3	/// unused slot
#define PlayerComputer		4	/// computer player
#define PlayerHuman		5	/// human player
#define PlayerRescuePassive	6	/// rescued passive
#define PlayerRescueActive	7	/// rescued  active

/*
**	Ai types for the player
*/
#define PlayerAiLand		0	/// attack at land
#define PlayerAiPassive		1
#define PlayerAiSea		0x19	/// attack at sea
#define PlayerAiAir		0x1A	/// attack at air

#define PlayerAiUniversal	0xFF	/// attack best

#define PlayerNumNeutral	15	/// this is the neutral player slot

#define PlayerMax		16	/// maximal players supported

#define GOLD_PER_MINE		100	/// default value for gold
#define WOOD_PER_CHOP		100	/// default value for wood
#define OIL_PER_HAUL		100	/// default value for oil

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

extern int NumPlayers;			/// Player in play
extern int NetPlayers;			/// Network players
extern Player Players[PlayerMax];	/// All players
extern Player* ThisPlayer;		/// Player on this computer

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

    /// FIXME: write short docu
extern void CreatePlayer(char* name,int type);
    /// FIXME: write short docu
extern void PlayerSetSide(Player* player,int side);
    /// FIXME: write short docu
extern void PlayerSetAiNum(Player* player,int ai);
    /// FIXME: write short docu
extern void PlayerSetGold(Player* player,int gold);
    /// FIXME: write short docu
extern void PlayerSetWood(Player* player,int wood);
    /// FIXME: write short docu
extern void PlayerSetOil(Player* player,int oil);

    /// Check if enough food is available for unit type
extern int PlayerCheckFood(const Player* player,const UnitType* type);
    /// Check if enough gold is available for unit type
extern int PlayerCheckGoldCost(const Player* player,const UnitType* type);
    /// Check if enough wood is available for unit type
extern int PlayerCheckWoodCost(const Player* player,const UnitType* type);
    /// Check if enough oil is available for unit type
extern int PlayerCheckOilCost(const Player* player,const UnitType* type);
    /// Check if enough resources are available
extern int PlayerCheckCosts(const Player* player,int gold,int wood,int oil);
    /// Check if enough resources are available for unit type
extern int PlayerCheckResources(const Player* player,const UnitType* type);

    /// FIXME: write short docu
extern void PlayerAddCosts(Player* player,int gold,int wood,int oil);
    /// FIXME: write short docu
extern void PlayerSubCosts(Player* player,int gold,int wood,int oil);
    /// FIXME: write short docu
extern void PlayerSubResources(Player* player,const UnitType* type);
    /// FIXME: write short docu
extern void PlayerAddResources(Player* player,const UnitType* type);

    /// Has the player units of that type
extern int HaveUnitTypeByType(const Player* player,const UnitType* type);
    /// Has the player units of that type
extern int HaveUnitTypeByIdent(const Player* player,const char* ident);

    /// FIXME: write short docu
extern void PlayersInitAi(void);
    /// FIXME: write short docu
extern void PlayersEachFrame(void);
    /// FIXME: write short docu
extern void PlayersEachSecond(void);

    /// Change current color set to new player.
extern void PlayerPixels(const Player* player);

    /// Output debug informations for players
extern void DebugPlayers(void);

//@}

#endif // !__PLAYER_H__
