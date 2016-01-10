/*
**	A clone of a famous game.
*/
/**@name action_move.c	-	The move action. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: action_move.c,v 1.14 2000/01/03 02:30:29 root Exp $
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
#include "tileset.h"
#include "map.h"
#include "actions.h"
#include "pathfinder.h"
#include "missile.h"
#include "sound.h"

//
//	Convert heading into direction.
//
			//  N NE  E SE  S SW  W NW
local int Heading2X[8] = {  0,+1,+1,+1, 0,-1,-1,-1 };
local int Heading2Y[8] = { -1,-1, 0,+1,+1,+1, 0,-1 };

/*----------------------------------------------------------------------------
--	Move Table
----------------------------------------------------------------------------*/

//	Critter
local Animation CritterMove[] = {
    {0, 2, 2,  0}, {0, 2, 3,  0}, {0, 2, 3,  0}, {0, 2, 3,  0}, {0, 2, 3,  0},
    {0, 2, 3,  0}, {0, 2, 3,  0}, {0, 2, 3,  0}, {0, 2, 3,  0}, {0, 2, 3,  0},
    {0, 2, 3,  0}, {0, 2, 3,  0}, {0, 2, 3,  0}, {0, 2, 3,  0}, {0, 2, 3,  0},
    {0, 2, 3,  0}, {3, 0, 1,  0}
};

//	Footman,Grunt,Grom Hellscream,Danath,Korgath Bladefist
local Animation GruntMove[] = {
    {0, 3, 2,  0}, {0, 3, 1,  5}, {0, 3, 2,  0}, {0, 2, 1,  5}, {0, 3, 1,  0},
    {0, 2, 1,-10}, {0, 3, 2,  0}, {0, 3, 1, 15}, {0, 3, 2,  0}, {0, 2, 1,  5},
    {0, 3, 1,  0}, {3, 2, 1,-20}
};

//	Peon, Peasant, Attacking Peon, Attacking Peasant.
local Animation PeonMove[] = {
    {0, 3, 2,  0}, {0, 3, 1,  5}, {0, 3, 2,  0}, {0, 2, 1,  5}, {0, 3, 1,  0},
    {0, 2, 1,-10}, {0, 3, 2,  0}, {0, 3, 1, 15}, {0, 3, 2,  0}, {0, 2, 1,  5},
    {0, 3, 1,  0}, {3, 2, 1,-20}
};

//	Ballista
local Animation BallistaMove[] = {
    {0, 0, 1,  0}, {0, 2, 2,  5}, {0, 2, 2, -5}, {0, 2, 2,  5}, {0, 2, 2, -5},
    {0, 2, 2,  5}, {0, 2, 2, -5}, {0, 2, 2,  5}, {0, 2, 2, -5}, {0, 2, 2,  5},
    {0, 2, 2, -5}, {0, 2, 2,  5}, {0, 2, 2, -5}, {0, 2, 2,  5}, {0, 2, 2, -5},
    {0, 2, 2,  5}, {3, 2, 1, -5}
};

//	Catapult
local Animation CatapultMove[] = {
    {0, 0, 1,  0}, {0, 2, 2,  5}, {0, 2, 2, -5}, {0, 2, 2,  5}, {0, 2, 2, -5},
    {0, 2, 2,  5}, {0, 2, 2, -5}, {0, 2, 2,  5}, {0, 2, 2, -5}, {0, 2, 2,  5},
    {0, 2, 2, -5}, {0, 2, 2,  5}, {0, 2, 2, -5}, {0, 2, 2,  5}, {0, 2, 2, -5},
    {0, 2, 2,  5}, {3, 2, 1, -5}
};

//	Knight, Paladin, Turalyon, Lothar, Uther Lightbringer
local Animation KnightMove[] = {
    {0, 3, 1,  0}, {0, 3, 1,  5}, {0, 4, 2,  0}, {0, 3, 1,  5}, {0, 3, 1,  0},
    {0, 3, 1,  5}, {0, 4, 2,  0}, {0, 3, 1,  5}, {0, 3, 1,  0}, {3, 3, 1,-20}
};

//	Ogre, Ogre-mage, Dentarg, Cho'gall
local Animation OgreMove[] = {
    {0, 3, 1,  0}, {0, 3, 1,  5}, {0, 3, 1,  0}, {0, 2, 1,  5}, {0, 3, 1,  0},
    {0, 2, 1,-10}, {0, 3, 1,  0}, {0, 3, 1, 15}, {0, 3, 1,  0}, {0, 2, 1,  5},
    {0, 3, 1,  0}, {3, 2, 1,-20}
};

//	Archer, Ranger, Alleria
local Animation ArcherMove[] = {
    {0, 3, 2,  0}, {0, 3, 1,  5}, {0, 3, 2,  0}, {0, 2, 1,  5}, {0, 3, 1,  0},
    {0, 2, 1,-10}, {0, 3, 2,  0}, {0, 3, 1, 15}, {0, 3, 2,  0}, {0, 2, 1,  5},
    {0, 3, 1,  0}, {3, 2, 1,-20}
};

//	Axethrower, Berserker, Zuljin
local Animation AxethrowerMove[] = {
    {0, 3, 2,  0}, {0, 3, 1,  5}, {0, 3, 2,  0}, {0, 2, 1,  5}, {0, 3, 1,  0},
    {0, 2, 1,-10}, {0, 3, 2,  0}, {0, 3, 1, 15}, {0, 3, 2,  0}, {0, 2, 1,  5},
    {0, 3, 1,  0}, {3, 2, 1,-20}
};

//	Mage, Khadar
local Animation MageMove[] = {
    {0, 3, 2,  0}, {0, 3, 1,  5}, {0, 3, 2,  0}, {0, 2, 1,  5}, {0, 3, 2,  0},
    {0, 2, 1,-10}, {0, 3, 2,  0}, {0, 3, 1, 15}, {0, 3, 2,  0}, {0, 2, 1,  5},
    {0, 3, 2,  0}, {3, 2, 1,-20}
};

//	Death Knight, Teron Gorefiend, Gul'dan
local Animation DeathKnightMove[] = {
    {0, 3, 2,  0}, {0, 3, 2,  5}, {0, 4, 2,  0}, {0, 3, 2,  5}, {0, 4, 2,  0},
    {0, 3, 2,  5}, {0, 4, 2,  0}, {0, 3, 2,  5}, {0, 3, 2,  0}, {3, 3, 1,-20}
};

//	Dwarves
local Animation DwarvesMove[] = {
    {0, 3, 2,  0}, {0, 3, 1, 10}, {0, 4, 2,  0}, {0, 3, 1, 15}, {0, 3, 2,  0},
    {0, 3, 1, 15}, {0, 4, 2,  0}, {0, 3, 1, 15}, {0, 3, 1,  0}, {3, 3, 1,-55}
};

//	Goblin Sappers
local Animation GoblinSappersMove[] = {
    {0, 3, 1,  0}, {0, 3, 1, 10}, {0, 2, 1,  0}, {0, 3, 2, 15}, {0, 3, 1,  0},
    {0, 2, 1, 15}, {0, 3, 1,  0}, {0, 3, 1, 15}, {0, 2, 1,  0}, {0, 3, 2, 10},
    {0, 3, 1,  0}, {3, 2, 1,-65}
};

//	Gryphon Rider, Kurdan and Sky'ree:
local Animation GryphonRiderMove[] = {
    {0, 0, 1,  0}, {0, 2, 2,  0}, {0, 3, 2,  0}, {0, 3, 2,  5}, {0, 2, 2,  0},
    {0, 3, 2,  0}, {0, 3, 2,  5}, {0, 2, 2,  0}, {0, 3, 2,  0}, {0, 3, 2,  5},
    {0, 2, 2,  0}, {0, 3, 2,  0}, {3, 3, 1,-15}
};

//	Dragon, Deathwing
local Animation DragonMove[] = {
    {0, 0, 1,  0}, {0, 2, 2,  0}, {0, 3, 2,  0}, {0, 3, 2,  5}, {0, 2, 2,  0},
    {0, 3, 2,  0}, {0, 3, 2,  5}, {0, 2, 2,  0}, {0, 3, 2,  0}, {0, 3, 2,  5},
    {0, 2, 2,  0}, {0, 3, 2,  0}, {3, 3, 1,-15}
};

//	Eye of kilrogg
local Animation EyeOfKilroggMove[] = {
    {0, 4, 1,  0}, {0, 4, 1,  0}, {0, 4, 1,  0}, {0, 4, 1,  0}, {0, 4, 1,  0},
    {0, 4, 1,  0}, {0, 4, 1,  0}, {3, 4, 1,  0}
};

//	Human tanker, orc tanker:
local Animation TankerMove[] = {
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {3, 1, 1,  0}
};

//	Human transporter, orc transporter:
local Animation TransportMove[] = {
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {3, 1, 1,  0}
};

//	Elven destroyer, Troll destroyer:
local Animation DestroyerMove[] = {
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {3, 1, 1,  0}
};

//	Battleship, Juggernaught
local Animation BattleshipMove[] = {
    {0, 1, 2,  0}, {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 2,  0},
    {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 2,  0},
    {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 2,  0}, {0, 1, 1,  0},
    {0, 1, 2,  0}, {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 2,  0},
    {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 2,  0},
    {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 2,  0}, {0, 1, 1,  0},
    {0, 1, 2,  0}, {3, 1, 1,  0}
};

//	Gnomish submarine, giant turtle
local Animation SubmarineMove[] = {
    {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 2,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 1,  0},
    {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 2,  0},
    {0, 1, 1,  0}, {0, 1, 1,  0}, {0, 1, 2,  0}, {0, 1, 1,  0}, {0, 1, 1,  0},
    {0, 1, 2,  0}, {3, 1, 1,  0}
};

//	Gnomish flying machine
local Animation GnomishFlyingMachineMove[] = {
    {0, 2, 1,  0}, {0, 1, 1,  5}, {0, 2, 1,  0}, {0, 2, 1, -5}, {0, 1, 1,  0},
    {0, 2, 1,  5}, {0, 1, 1,  0}, {0, 2, 1, -5}, {0, 2, 1,  0}, {0, 1, 1,  5},
    {0, 2, 1,  0}, {0, 1, 1, -5}, {0, 2, 1,  0}, {0, 2, 1,  5}, {0, 1, 1,  0},
    {0, 2, 1, -5}, {0, 1, 1,  0}, {0, 2, 1,  5}, {0, 2, 1,  0}, {3, 1, 1, -5}
};

//	Goblin zeppelin
local Animation GoblinZeppelinMove[] = {
    {0, 2, 1,  0}, {0, 1, 1,  0}, {0, 2, 1,  0}, {0, 2, 1,  0}, {0, 1, 1,  0},
    {0, 2, 1,  0}, {0, 1, 1,  0}, {0, 2, 1,  0}, {0, 2, 1,  0}, {0, 1, 1,  0},
    {0, 2, 1,  0}, {0, 1, 1,  0}, {0, 2, 1,  0}, {0, 2, 1,  0}, {0, 1, 1,  0},
    {0, 2, 1,  0}, {0, 1, 1,  0}, {0, 2, 1,  0}, {0, 2, 1,  0}, {3, 1, 1,  0}
};

//	Skeleton
local Animation SkeletonMove[] = {
    {0, 3, 2,  0}, {0, 3, 2, 10}, {0, 3, 1,  0}, {0, 2, 2, 15}, {0, 3, 3,  0},
    {0, 2, 1,-25}, {0, 3, 2,  0}, {0, 3, 2, 40}, {0, 3, 1,  0}, {0, 2, 2, 15},
    {0, 3, 2,  0}, {3, 2, 1,-55}
};

//	Daemon
local Animation DaemonMove[] = {
    {0, 3, 2,  0}, {0, 2, 1,  0}, {0, 2, 2,  5}, {0, 2, 2,  0}, {0, 2, 1,  0},
    {0, 2, 2,  5}, {0, 2, 1,  0}, {0, 3, 2,  0}, {0, 2, 2,  5}, {0, 2, 1,  0},
    {0, 2, 2,  0}, {0, 2, 1,  5}, {0, 2, 2,  0}, {0, 2, 2,  0}, {3, 2, 1,-20}
};

/*
**	Move animation. FIXME: move this to unit-type, CCL configurable
*/
global Animation* UnitMoveAnimation[UnitTypeInternalMax] = {
    GruntMove,			// UnitFootman
    GruntMove,			// UnitGrunt
    PeonMove,			// UnitPeasant
    PeonMove,			// UnitPeon
    BallistaMove,		// UnitBallista
    CatapultMove,		// UnitCatapult
    KnightMove,			// UnitKnight
    OgreMove,			// UnitOgre
    ArcherMove,			// UnitArcher
    AxethrowerMove,		// UnitAxethrower
    MageMove,			// UnitMage
    DeathKnightMove,		// UnitDeathKnight
    KnightMove,			// UnitPaladin
    OgreMove,			// UnitOgreMage
    DwarvesMove,		// UnitDwarves
    GoblinSappersMove,		// UnitGoblinSappers
    PeonMove,			// UnitAttackPeasant
    PeonMove,			// UnitAttackPeon
    ArcherMove,			// UnitRanger
    AxethrowerMove,		// UnitBerserker
    ArcherMove,			// UnitAlleria
    DeathKnightMove,		// UnitTeronGorefiend
    GryphonRiderMove,		// UnitKurdanAndSky_ree
    OgreMove,			// UnitDentarg
    MageMove,			// UnitKhadgar
    GruntMove,			// UnitGromHellscream
    TankerMove,			// UnitTankerHuman
    TankerMove,			// UnitTankerOrc
    TransportMove,		// UnitTransportHuman
    TransportMove,		// UnitTransportOrc
    DestroyerMove,		// UnitElvenDestroyer
    DestroyerMove,		// UnitTrollDestroyer
    BattleshipMove,		// UnitBattleship
    BattleshipMove,		// UnitJuggernaught
    0,				// UnitNothing
    DragonMove,			// UnitDeathwing
    0,				// UnitNothing1
    0,				// UnitNothing2
    SubmarineMove,		// UnitGnomishSubmarine
    SubmarineMove,		// UnitGiantTurtle
    GnomishFlyingMachineMove,	// UnitGnomishFlyingMachine
    GoblinZeppelinMove,		// UnitGoblinZeppelin
    GryphonRiderMove,		// UnitGryphonRider
    DragonMove,			// UnitDragon
    KnightMove,			// UnitTuralyon
    EyeOfKilroggMove,		// UnitEyeOfKilrogg
    GruntMove,			// UnitDanath
    GruntMove,			// UnitKorgathBladefist
    0,				// UnitNothing3
    OgreMove,			// UnitCho_gall
    KnightMove,			// UnitLothar
    DeathKnightMove,		// UnitGul_dan
    KnightMove,			// UnitUtherLightbringer
    AxethrowerMove,		// UnitZuljin
    0,				// UnitNothing4
    SkeletonMove,		// UnitSkeleton
    DaemonMove,			// UnitDaemon
    CritterMove,		// UnitCritter
    0,				// UnitFarm
    0,				// UnitPigFarm
    0,				// UnitBarracksHuman
    0,				// UnitBarracksOrc
    0,				// UnitChurch
    0,				// UnitAltarOfStorms
    0,				// UnitScoutTowerHuman
    0,				// UnitScoutTowerOrc
    0,				// UnitStables
    0,				// UnitOgreMound
    0,				// UnitGnomishInventor
    0,				// UnitGoblinAlchemist
    0,				// UnitGryphonAviary
    0,				// UnitDragonRoost
    0,				// UnitShipyardHuman
    0,				// UnitShipyardOrc
    0,				// UnitTownHall
    0,				// UnitGreatHall
    0,				// UnitElvenLumberMill
    0,				// UnitTrollLumberMill
    0,				// UnitFoundryHuman
    0,				// UnitFoundryOrc
    0,				// UnitMageTower
    0,				// UnitTempleOfTheDamned
    0,				// UnitBlacksmithHuman
    0,				// UnitBlacksmithOrc
    0,				// UnitRefineryHuman
    0,				// UnitRefineryOrc
    0,				// UnitOilPlatformHuman
    0,				// UnitOilPlatformOrc
    0,				// UnitKeep
    0,				// UnitStronghold
    0,				// UnitCastle
    0,				// UnitFortress
    0,				// UnitGoldMine
    0,				// UnitOilPatch
    0,				// UnitStartLocationHuman
    0,				// UnitStartLocationOrc
    0,				// UnitGuardTowerHuman
    0,				// UnitGuardTowerOrc
    0,				// UnitCannonTowerHuman
    0,				// UnitCannonTowerOrc
    0,				// UnitCircleofPower
    0,				// UnitDarkPortal
    0,				// UnitRunestone
    0,				// UnitWallHuman
    0,				// UnitWallOrc
    0,				// UnitDeadBody
    0,				// Unit1x1DestroyedPlace
    0,				// Unit2x2DestroyedPlace
    0,				// Unit3x3DestroyedPlace
    0,				// Unit4x4DestroyedPlace
    PeonMove,			// UnitPeasantWithGold
    PeonMove,			// UnitPeonWithGold
    PeonMove,			// UnitPeasantWithWood
    PeonMove,			// UnitPeonWithWood
    TankerMove,			// UnitTankerHumanFull
    TankerMove,			// UnitTankerOrcFull
};

/**
**	Generic unit mover.
**
**	@param unit	Unit that moves.
**	@param move	Animation script for unit.
**
**	@returns	True if still moving, False if stopping.
*/
local int DoActionMoveGeneric(Unit* unit,Animation* move)
{
    int xd;
    int yd;
    int state;
    Unit* goal;
    int i;

    if( !(state=unit->State) ) {

	//
	//	Target killed?
	//
	goal=unit->Command.Data.Move.Goal;
	DebugLevel3("UNIT %p GOAL %p\n",unit,goal);
	if( goal && (!goal->Type || UnitUnusable(goal)) ) {
	    unit->Command.Data.Move.Goal=NoUnitP;
	    DebugLevel3("goal killed\n");
	}

#if 1
	// FIXME: should I give same error codes to caller
	switch( NextPathElement(unit,&xd,&yd) ) {
	    case -2:			// Can't reach, stop
		unit->Reset=unit->Wait=1;
		unit->Moving=0;
		unit->Command.Action=UnitActionStill;
		return -1;
	    case -1:			// Reached goal, stop
		unit->Reset=unit->Wait=1;
		unit->Moving=0;
		unit->Command.Action=UnitActionStill;
		return 1;
	    case 0:			// No path, wait
		unit->Reset=unit->Wait=1;
		return 0;
	    default:			// On the way moving
		break;
	}
#else
	i=NewPath(unit,&xd,&yd);

	// FIXME: if unreachable extra return code!

	if( i ) {			// Test movement end ...
	    DebugLevel3("Reached %d\n",i);
	    unit->Reset=1;
	    unit->Wait=1;
	    unit->Moving=0;
	    unit->Command.Action=UnitActionStill;
	    return i;
	}
#endif

	//
	//	Transporter (un)docking?
	//
	if( unit->Type->Transporter
	    && ( (WaterOnMap(unit->X,unit->Y)
		    && CoastOnMap(unit->X+xd,unit->Y+yd))
		|| (CoastOnMap(unit->X,unit->Y)
			&& WaterOnMap(unit->X+xd,unit->Y+yd)) ) ) {
	    PlayUnitSound(unit,VoiceDocking);
	}

	//
	//	Update movement map.
	//
	i=UnitFieldFlags(unit);
	TheMap.Fields[unit->X+unit->Y*TheMap.Width].Flags&=~i;

	UnitCacheRemove(unit);
	unit->X+=xd;
	unit->Y+=yd;
	UnitCacheInsert(unit);

	TheMap.Fields[unit->X+unit->Y*TheMap.Width].Flags|=i;

	MustRedraw|=RedrawMinimap;
	//
	//	Update visible area.
	//
	if( unit->Player==ThisPlayer ) {
	    // FIXME: need only mark to new direction!!
	    MapMarkSight(unit->X,unit->Y,unit->Stats->SightRange);
	}

	unit->Moving=1;
	unit->IX=-xd*TileSizeX;
	unit->IY=-yd*TileSizeY;
	UnitNewHeadingFromXY(unit,xd,yd);
    } else {
	xd=Heading2X[unit->Heading];
	yd=Heading2Y[unit->Heading];
    }

    DebugLevel3("Move: %d,%d State %2d ",xd,yd,unit->State);
    DebugLevel3("Walk %d Frame %2d Wait %3d Heading %d %d,%d\n"
	    ,move[state].Pixel
	    ,move[state].Frame
	    ,move[state].Sleep
	    ,unit->Heading
	    ,unit->IX,unit->IY);

    unit->IX+=xd*move[state].Pixel;
    unit->IY+=yd*move[state].Pixel;
    unit->Frame+=move[state].Frame;
    unit->Wait=move[state].Sleep;

    if( (move[state].Pixel || move[state].Frame) && UnitVisible(unit) ) {
	MustRedraw|=RedrawMap;
    }

    if( move[state].Flags&AnimationReset ) {
	unit->Reset=1;
    }
    if( move[state].Flags&AnimationRestart ) {
	unit->State=0;
    } else {
	++unit->State;
    }

    return 0;
}

/**
**	Unit moves!
**
**	@param unit	Pointer to unit.
**
**	@returns	0=moving,1=stopped reached,-1=stopped unreached
*/
global int HandleActionMove(Unit* unit)
{
    int type;

    // FIXME: This should become the new version, like many other places
    // FIXME: only started.
    if( unit->Type->Animations ) {
	DebugLevel3("%s: %p\n",unit->Type->Ident,unit->Type->Animations );
	return DoActionMoveGeneric(unit,unit->Type->Animations->Move);
    }

    type=unit->Type->Type;
    if( type<UnitTypeInternalMax && UnitMoveAnimation[type] ) {
	return DoActionMoveGeneric(unit,UnitMoveAnimation[type]);
    }

    return -1;
}

//@}
