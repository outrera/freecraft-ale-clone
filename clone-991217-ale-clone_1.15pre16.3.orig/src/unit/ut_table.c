/*
**	A clone of a famous game.
*/
/**@name ut_table.c	-	The unit types table. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: ut_table.c,v 1.11 1999/12/03 09:18:13 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>

#include "clone.h"
#include "video.h"
#include "tileset.h"
#include "map.h"
#include "sound_id.h"
#include "unitsound.h"
#include "icons.h"
#include "button.h"
#include "unittype.h"

#define DEFAULT	NULL

/**
**	Unit type type definition
*/
global char UnitTypeType[] = "unit-type";

/*----------------------------------------------------------------------------
--	Buttons
----------------------------------------------------------------------------*/

#if 0
// This was the start of my (johns) version, the new and active version
// is from cade and could be found in ui/button_table.c and ui/botpanel.c
// FIXME: what is better? configure the buttons with the unit or separate it

// Footman, Knight
local ButtonConfig _FootmanButtons[] = {
     {"human-move"}
    ,{"human-shield1"}	,{"human-shield2"}	,{"human-shield3"}
    ,{"human-sword1"}	,{"human-sword2"}	,{"human-sword3"}
    ,{"human-patrol"}
    ,{"human-stand-ground"}
    ,{ NULL }
};
#endif

#define _FootmanButtons			NULL
#define _GruntButtons			NULL
#define _PeasantButtons			NULL
#define _PeonButtons			NULL
#define _BallistaButtons		NULL
#define _CatapultButtons		NULL
#define _KnightButtons			NULL
#define _OgreButtons			NULL
#define _ArcherButtons			NULL
#define _AxethrowerButtons		NULL
#define _MageButtons			NULL
#define _DeathKnightButtons		NULL
#define _PaladinButtons			NULL
#define _OgreMageButtons		NULL
#define _DwarvesButtons			NULL
#define _GoblinSappersButtons		NULL
#define _AttackPeasantButtons		NULL
#define _AttackPeonButtons		NULL
#define _RangerButtons			NULL
#define _BerserkerButtons		NULL
#define _AlleriaButtons			NULL
#define _TeronGorefiendButtons		NULL
#define _KurdanAndSky_reeButtons	NULL
#define _DentargButtons			NULL
#define _KhadgarButtons			NULL
#define _GnomHellscreamButtons		NULL
#define _TankerHumanButtons		NULL
#define _TankerOrcButtons		NULL
#define _TransportHumanButtons		NULL
#define _TransportOrcButtons		NULL
#define _ElvenDestroyerButtons		NULL
#define _TrollDestroyerButtons		NULL
#define _BattleshipButtons		NULL
#define _JuggernaughtButtons		NULL
#define _NothingButtons			NULL
#define _DeathwingButtons		NULL
#define _Nothing1Buttons		NULL
#define _Nothing2Buttons		NULL
#define _GnomishSubmarineButtons	NULL
#define _GiantTurtleButtons		NULL
#define _GnomishFlyingMachineButtons	NULL
#define _GoblinZeppelinButtons		NULL
#define _GryphonRiderButtons		NULL
#define _DragonButtons			NULL
#define _TuralyonButtons		NULL
#define _EyeOfKilroggButtons		NULL
#define _DanathButtons			NULL
#define _KorgathBladefistButtons	NULL
#define _Nothing3Buttons		NULL
#define _Cho_gallButtons		NULL
#define _LotharButtons			NULL
#define _Gul_danButtons			NULL
#define _UtherLightbringerButtons	NULL
#define _ZuljinButtons			NULL
#define _Nothing4Buttons		NULL
#define _SkeletonButtons		NULL
#define _DaemonButtons			NULL
#define _CritterButtons			NULL
#define _FarmButtons			NULL
#define _PigFarmButtons			NULL
#define _BarracksHumanButtons		NULL
#define _BarracksOrcButtons		NULL
#define _ChurchButtons			NULL
#define _AltarOfStormsButtons		NULL
#define _ScoutTowerHumanButtons		NULL
#define _ScoutTowerOrcButtons		NULL
#define _StablesButtons			NULL
#define _OgreMoundButtons		NULL
#define _GnomishInventorButtons		NULL
#define _GoblinAlchemistButtons		NULL
#define _GryphonAviaryButtons		NULL
#define _DragonRoostButtons		NULL
#define _ShipyardHumanButtons		NULL
#define _ShipyardOrcButtons		NULL
#define _TownHallButtons		NULL
#define _GreatHallButtons		NULL
#define _ElvenLumberMillButtons		NULL
#define _TrollLumberMillButtons		NULL
#define _FoundryHumanButtons		NULL
#define _FoundryOrcButtons		NULL
#define _MageTowerButtons		NULL
#define _TempleOfTheDamnedButtons	NULL
#define _BlacksmithHumanButtons		NULL
#define _BlacksmithOrcButtons		NULL
#define _RefineryHumanButtons		NULL
#define _RefineryOrcButtons		NULL
#define _OilPlatformHumanButtons	NULL
#define _OilPlatformOrcButtons		NULL
#define _KeepButtons			NULL
#define _StrongholdButtons		NULL
#define _CastleButtons			NULL
#define _FortressButtons		NULL
#define _GoldMineButtons		NULL
#define _OilPatchButtons		NULL
#define _StartLocationHumanButtons	NULL
#define _StartLocationOrcButtons	NULL
#define _GuardTowerHumanButtons		NULL
#define _GuardTowerOrcButtons		NULL
#define _CannonTowerHumanButtons	NULL
#define _CannonTowerOrcButtons		NULL
#define _CircleofPowerButtons		NULL
#define _DarkPortalButtons		NULL
#define _RunestoneButtons		NULL
#define _WallHumanButtons		NULL
#define _WallOrcButtons			NULL
#define _DeadBodyButtons		NULL
#define _1x1DestroyedPlaceButtons	NULL
#define _2x2DestroyedPlaceButtons	NULL
#define _3x3DestroyedPlaceButtons	NULL
#define _4x4DestroyedPlaceButtons	NULL
#define _PeasantWithGoldButtons		NULL
#define _PeonWithGoldButtons		NULL
#define _PeasantWithWoodButtons		NULL
#define _PeonWithWoodButtons		NULL
#define _TankerHumanFullButtons		NULL
#define _TankerOrcFullButtons		NULL

/*----------------------------------------------------------------------------
--	Animations
----------------------------------------------------------------------------*/

///	Default:
local Animation _DefaultStill[] = {
// FIXME: Reset frame 0, wait 1, than endless wait 5
    {0, 0, 4, 0}, {3, 0, 1, 0}
};

///	Footman,Grunt,Grom Hellscream,Danath,Korgath Bladefist
local Animation _GruntMove[] = {
    {0, 3, 2,  0}, {0, 3, 1,  5}, {0, 3, 2,  0}, {0, 2, 1,  5}, {0, 3, 1,  0},
    {0, 2, 1,-10}, {0, 3, 2,  0}, {0, 3, 1, 15}, {0, 3, 2,  0}, {0, 2, 1,  5},
    {0, 3, 1,  0}, {3, 2, 1,-20}
};

///	Footman,Grunt,Grom Hellscream,Danath,Korgath Bladefist
local Animation _GruntAttack[] = {
    { 0, 0, 3, 25},{ 0, 0, 3,  5},{ 0, 0, 3,  5},{12, 0, 5,  5},{ 0, 0,10,-40},
    { 3, 0, 1,  0}
};

///	Footman,Grunt,Grom Hellscream,Danath,Korgath Bladefist
local Animation _GruntDie[] = {
    { 0, 0, 3, 45},{ 0, 0, 3,  5},{ 0, 0,100,  5},{ 3, 0, 1,  0}
};

///	Footman,Grunt,Grom Hellscream,Danath,Korgath Bladefist
local Animations _GruntAnimations[] = {
{   _DefaultStill,
    _GruntMove,
    _GruntAttack,
    _GruntDie,
    NULL	}
};

#define _FootmanAnimations		_GruntAnimations
#define _GruntAnimations		_GruntAnimations
#define _PeasantAnimations		NULL
#define _PeonAnimations			NULL
#define _BallistaAnimations		NULL
#define _CatapultAnimations		NULL
#define _KnightAnimations		NULL
#define _OgreAnimations			NULL
#define _ArcherAnimations		NULL
#define _AxethrowerAnimations		NULL
#define _MageAnimations			NULL
#define _DeathKnightAnimations		NULL
#define _PaladinAnimations		NULL
#define _OgreMageAnimations		NULL
#define _DwarvesAnimations		NULL
#define _GoblinSappersAnimations	NULL
#define _AttackPeasantAnimations	NULL
#define _AttackPeonAnimations		NULL
#define _RangerAnimations		NULL
#define _BerserkerAnimations		NULL
#define _AlleriaAnimations		NULL
#define _TeronGorefiendAnimations	NULL
#define _KurdanAndSky_reeAnimations	NULL
#define _DentargAnimations		NULL
#define _KhadgarAnimations		NULL
#define _GnomHellscreamAnimations	NULL
#define _TankerHumanAnimations		NULL
#define _TankerOrcAnimations		NULL
#define _TransportHumanAnimations	NULL
#define _TransportOrcAnimations		NULL
#define _ElvenDestroyerAnimations	NULL
#define _TrollDestroyerAnimations	NULL
#define _BattleshipAnimations		NULL
#define _JuggernaughtAnimations		NULL
#define _NothingAnimations		NULL
#define _DeathwingAnimations		NULL
#define _Nothing1Animations		NULL
#define _Nothing2Animations		NULL
#define _GnomishSubmarineAnimations	NULL
#define _GiantTurtleAnimations		NULL
#define _GnomishFlyingMachineAnimations	NULL
#define _GoblinZeppelinAnimations	NULL
#define _GryphonRiderAnimations		NULL
#define _DragonAnimations		NULL
#define _TuralyonAnimations		NULL
#define _EyeOfKilroggAnimations		NULL
#define _DanathAnimations		NULL
#define _KorgathBladefistAnimations	NULL
#define _Nothing3Animations		NULL
#define _Cho_gallAnimations		NULL
#define _LotharAnimations		NULL
#define _Gul_danAnimations		NULL
#define _UtherLightbringerAnimations	NULL
#define _ZuljinAnimations		NULL
#define _Nothing4Animations		NULL
#define _SkeletonAnimations		NULL
#define _DaemonAnimations		NULL
#define _CritterAnimations		NULL
#define _FarmAnimations			NULL
#define _PigFarmAnimations		NULL
#define _BarracksHumanAnimations	NULL
#define _BarracksOrcAnimations		NULL
#define _ChurchAnimations		NULL
#define _AltarOfStormsAnimations	NULL
#define _ScoutTowerHumanAnimations	NULL
#define _ScoutTowerOrcAnimations	NULL
#define _StablesAnimations		NULL
#define _OgreMoundAnimations		NULL
#define _GnomishInventorAnimations	NULL
#define _GoblinAlchemistAnimations	NULL
#define _GryphonAviaryAnimations	NULL
#define _DragonRoostAnimations		NULL
#define _ShipyardHumanAnimations	NULL
#define _ShipyardOrcAnimations		NULL
#define _TownHallAnimations		NULL
#define _GreatHallAnimations		NULL
#define _ElvenLumberMillAnimations	NULL
#define _TrollLumberMillAnimations	NULL
#define _FoundryHumanAnimations		NULL
#define _FoundryOrcAnimations		NULL
#define _MageTowerAnimations		NULL
#define _TempleOfTheDamnedAnimations	NULL
#define _BlacksmithHumanAnimations	NULL
#define _BlacksmithOrcAnimations	NULL
#define _RefineryHumanAnimations	NULL
#define _RefineryOrcAnimations		NULL
#define _OilPlatformHumanAnimations	NULL
#define _OilPlatformOrcAnimations	NULL
#define _KeepAnimations			NULL
#define _StrongholdAnimations		NULL
#define _CastleAnimations		NULL
#define _FortressAnimations		NULL
#define _GoldMineAnimations		NULL
#define _OilPatchAnimations		NULL
#define _StartLocationHumanAnimations	NULL
#define _StartLocationOrcAnimations	NULL
#define _GuardTowerHumanAnimations	NULL
#define _GuardTowerOrcAnimations	NULL
#define _CannonTowerHumanAnimations	NULL
#define _CannonTowerOrcAnimations	NULL
#define _CircleofPowerAnimations	NULL
#define _DarkPortalAnimations		NULL
#define _RunestoneAnimations		NULL
#define _WallHumanAnimations		NULL
#define _WallOrcAnimations		NULL
#define _DeadBodyAnimations		NULL
#define _1x1DestroyedPlaceAnimations	NULL
#define _2x2DestroyedPlaceAnimations	NULL
#define _3x3DestroyedPlaceAnimations	NULL
#define _4x4DestroyedPlaceAnimations	NULL
#define _PeasantWithGoldAnimations	NULL
#define _PeonWithGoldAnimations		NULL
#define _PeasantWithWoodAnimations	NULL
#define _PeonWithWoodAnimations		NULL
#define _TankerHumanFullAnimations	NULL
#define _TankerOrcFullAnimations	NULL

/*----------------------------------------------------------------------------
--	Unit types table
----------------------------------------------------------------------------*/

/**
**	Unit types definition
*/
global UnitType UnitTypes[] = {
// * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING *
// Warning this is generated!!
{   UnitTypeType, "unit-footman"
    ,"Footman"
    ,NULL, {
	"footman.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_FootmanAnimations	// animations
	,{ "icon-footman" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     60,     0, {   60,   600,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     2,   60,       6,     3,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,      50
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_FootmanButtons	// buttons
   ,{		// sound
	 { "footman-selected" }
	,{ "footman-acknowledge" }
	,{ "footman-ready" }
	,{ "footman-help" }
	,{ "footman-dead" }
   },   {	 { "footman-attack" }
   } },

{   UnitTypeType, "unit-grunt"
    ,"Grunt"
    ,NULL, {
	"grunt.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_GruntAnimations	// animations
	,{ "icon-grunt" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     60,     0, {   60,   600,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     2,   60,       6,     3,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,      50
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_GruntButtons	// buttons
   ,{		// sound
	 { "grunt-selected" }
	,{ "grunt-acknowledge" }
	,{ "grunt-ready" }
	,{ "grunt-help" }
	,{ "grunt-dead" }
   },   {	 { "grunt-attack" }
   } },

{   UnitTypeType, "unit-peasant"
    ,"Peasant"
    ,NULL, {
	"peasant.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_PeasantAnimations	// animations
	,{ "icon-peasant" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     30,     0, {   45,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   50,       3,     2,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      3,      30
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     1,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_PeasantButtons	// buttons
   ,{		// sound
	 { "peasant-selected" }
	,{ "peasant-acknowledge" }
	,{ "peasant-ready" }
	,{ "peasant-help" }
	,{ "peasant-dead" }
   },   {	 { "peasant-attack" }
   } },

{   UnitTypeType, "unit-peon"
    ,"Peon"
    ,NULL, {
	"peon.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_PeonAnimations	// animations
	,{ "icon-peon" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     30,     0, {   45,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   50,       3,     2,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      3,      30
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     1,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_PeonButtons	// buttons
   ,{		// sound
	 { "peon-selected" }
	,{ "peon-acknowledge" }
	,{ "peon-ready" }
	,{ "peon-help" }
	,{ "peon-dead" }
   },   {	 { "peon-attack" }
   } },

{   UnitTypeType, "unit-ballista"
    ,"Ballista"
    ,NULL, {
	"ballista.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 64, 64			// graphic size
   ,_BallistaAnimations	// animations
	,{ "icon-ballista" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     5,      0,     9,    110,     0, {  250,   900,    300,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        8,     11,      9
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   70,      80,     0,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    14,   0,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    3,    0,     0,      0,          1,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       1
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_BallistaButtons	// buttons
   ,{		// sound
	 { "ballista-selected" }
	,{ "ballista-acknowledge" }
	,{ "ballista-ready" }
	,{ "ballista-help" }
	,{ "ballista-dead" }
   },   {	 { "ballista-attack" }
   } },

{   UnitTypeType, "unit-catapult"
    ,"Catapult"
    ,NULL, {
	"catapult.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 64, 64			// graphic size
   ,_CatapultAnimations	// animations
	,{ "icon-catapult" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     5,      0,     9,    110,     0, {  250,   900,    300,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        8,     11,      9
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   70,      80,     0,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    13,   0,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    3,    0,     0,      0,          1,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       1
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_CatapultButtons	// buttons
   ,{		// sound
	 { "catapult-selected" }
	,{ "catapult-acknowledge" }
	,{ "catapult-ready" }
	,{ "catapult-help" }
	,{ "catapult-dead" }
   },   {	 { "catapult-attack" }
   } },

{   UnitTypeType, "unit-knight"
    ,"Knight"
    ,NULL, {
	"knight.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_KnightAnimations	// animations
	,{ "icon-knight" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    13,      0,     4,     90,     0, {   90,   800,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    42,     42,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     4,   63,       8,     4,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_KnightButtons	// buttons
   ,{		// sound
	 { "knight-selected" }
	,{ "knight-acknowledge" }
	,{ "knight-ready" }
	,{ "knight-help" }
	,{ "knight-dead" }
   },   {	 { "knight-attack" }
   } },

{   UnitTypeType, "unit-ogre"
    ,"Ogre"
    ,NULL, {
	"ogre.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_OgreAnimations	// animations
	,{ "icon-ogre" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    13,      0,     4,     90,     0, {   90,   800,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    42,     42,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     4,   63,       8,     4,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_OgreButtons	// buttons
   ,{		// sound
	 { "ogre-selected" }
	,{ "ogre-acknowledge" }
	,{ "ogre-ready" }
	,{ "ogre-help" }
	,{ "ogre-dead" }
   },   {	 { "ogre-attack" }
   } },

{   UnitTypeType, "unit-archer"
    ,"Archer"
    ,NULL, {
	"archer.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_ArcherAnimations	// animations
	,{ "icon-archer" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     5,     40,     0, {   70,   500,     50,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    33,     33,        4,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   55,       3,     6,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    15,   0,       0,      0,      1,      60
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_ArcherButtons	// buttons
   ,{		// sound
	 { "archer-selected" }
	,{ "archer-acknowledge" }
	,{ "archer-ready" }
	,{ "archer-help" }
	,{ "archer-dead" }
   },   {	 { "archer-attack" }
   } },

{   UnitTypeType, "unit-axethrower"
    ,"Troll Axethrower"
    ,NULL, {
	"axethrower.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_AxethrowerAnimations	// animations
	,{ "icon-axethrower" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     5,     40,     0, {   70,   500,     50,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    36,     36,        4,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   55,       3,     6,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    16,   0,       0,      0,      1,      60
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_AxethrowerButtons	// buttons
   ,{		// sound
	 { "axethrower-selected" }
	,{ "axethrower-acknowledge" }
	,{ "axethrower-ready" }
	,{ "axethrower-help" }
	,{ "axethrower-dead" }
   },   {	 { "axethrower-attack" }
   } },

{   UnitTypeType, "unit-mage"
    ,"Mage"
    ,NULL, {
	"mage.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_MageAnimations	// animations
	,{ "icon-mage" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     8,      0,     9,     60,     1, {  120,  1200,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    33,     33,        2,     11,      9
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   70,       0,     9,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     1,       1,     1
   ,_MageButtons	// buttons
   ,{		// sound
	 { "mage-selected" }
	,{ "mage-acknowledge" }
	,{ "mage-ready" }
	,{ "mage-help" }
	,{ "mage-dead" }
   },   {	 { "mage-attack" }
   } },

{   UnitTypeType, "unit-death-knight"
    ,"Death Knight"
    ,NULL, {
	"death knight.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_DeathKnightAnimations	// animations
	,{ "icon-death-knight" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     8,      0,     9,     60,     1, {  120,  1200,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    39,     39,        3,     11,      9
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   70,       0,     9,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    10,   0,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     1,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     1,       1,     1
   ,_DeathKnightButtons	// buttons
   ,{		// sound
	 { "death-knight-selected" }
	,{ "death-knight-acknowledge" }
	,{ "death-knight-ready" }
	,{ "death-knight-help" }
	,{ "death-knight-dead" }
   },   {	 { "death-knight-attack" }
   } },

{   UnitTypeType, "unit-paladin"
    ,"Paladin"
    ,"unit-knight", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_PaladinAnimations	// animations
	,{ "icon-paladin" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    13,      0,     5,     90,     0, {   90,   800,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    42,     42,        1,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     4,   65,       8,     4,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,     110
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_PaladinButtons	// buttons
   ,{		// sound
	 { "paladin-selected" }
	,{ "paladin-acknowledge" }
	,{ "paladin-ready" }
	,{ "paladin-help" }
	,{ "paladin-dead" }
   },   {	 { "paladin-attack" }
   } },

{   UnitTypeType, "unit-ogre-mage"
    ,"Ogre Mage"
    ,"unit-ogre", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_OgreMageAnimations	// animations
	,{ "icon-ogre-mage" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    13,      0,     5,     90,     0, {   90,   800,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    42,     42,        1,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     4,   65,       8,     4,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,     110
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_OgreMageButtons	// buttons
   ,{		// sound
	 { "ogre-mage-selected" }
	,{ "ogre-mage-acknowledge" }
	,{ "ogre-mage-ready" }
	,{ "ogre-mage-help" }
	,{ "ogre-mage-dead" }
   },   {	 { "ogre-mage-attack" }
   } },

{   UnitTypeType, "unit-dwarves"
    ,"Dwarves"
    ,NULL, {
	"dwarves.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 56, 56			// graphic size
   ,_DwarvesAnimations	// animations
	,{ "icon-dwarves" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    11,      0,     4,     40,     0, {  200,   700,    250,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    32,     32,        1,      4,      2
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   55,       4,     2,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      5,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         1,     0,       1,     1
   ,_DwarvesButtons	// buttons
   ,{		// sound
	 { "dwarves-selected" }
	,{ "dwarves-acknowledge" }
	,{ "dwarves-ready" }
	,{ "dwarves-help" }
	,{ "dwarves-dead" }
   },   {	 { "dwarves-attack" }
   } },

{   UnitTypeType, "unit-goblin-sappers"
    ,"Goblin Sappers"
    ,NULL, {
	"goblin sapper.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 56, 56			// graphic size
   ,_GoblinSappersAnimations	// animations
	,{ "icon-goblin-sappers" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    11,      0,     4,     40,     0, {  200,   700,    250,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    37,     37,        1,      4,      2
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   55,       4,     2,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      5,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         1,     0,       1,     1
   ,_GoblinSappersButtons	// buttons
   ,{		// sound
	 { "goblin-sappers-selected" }
	,{ "goblin-sappers-acknowledge" }
	,{ "goblin-sappers-ready" }
	,{ "goblin-sappers-help" }
	,{ "goblin-sappers-dead" }
   },   {	 { "goblin-sappers-attack" }
   } },

{   UnitTypeType, "unit-attack-peasant"
    ,"Peasant"
    ,"unit-peasant", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_AttackPeasantAnimations	// animations
	,{ "icon-peasant" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     30,     0, {   45,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   50,       3,     2,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,      30
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_AttackPeasantButtons	// buttons
   ,{		// sound
	 { "peasant-selected" }
	,{ "peasant-acknowledge" }
	,{ "peasant-ready" }
	,{ "peasant-help" }
	,{ "peasant-dead" }
   },   {	 { "peasant-attack" }
   } },

{   UnitTypeType, "unit-attack-peon"
    ,"Peon"
    ,"unit-peon", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_AttackPeonAnimations	// animations
	,{ "icon-peon" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     30,     0, {   45,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   50,       3,     2,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,      30
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_AttackPeonButtons	// buttons
   ,{		// sound
	 { "peon-selected" }
	,{ "peon-acknowledge" }
	,{ "peon-ready" }
	,{ "peon-help" }
	,{ "peon-dead" }
   },   {	 { "peon-attack" }
   } },

{   UnitTypeType, "unit-ranger"
    ,"Ranger"
    ,"unit-archer", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_RangerAnimations	// animations
	,{ "icon-ranger" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     6,     50,     0, {   70,   500,     50,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    33,     33,        4,      9,      6
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   57,       3,     6,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    15,   0,       0,      0,      1,      70
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_RangerButtons	// buttons
   ,{		// sound
	 { "ranger-selected" }
	,{ "ranger-acknowledge" }
	,{ "ranger-ready" }
	,{ "ranger-help" }
	,{ "ranger-dead" }
   },   {	 { "ranger-attack" }
   } },

{   UnitTypeType, "unit-berserker"
    ,"Berserker"
    ,"unit-axethrower", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_BerserkerAnimations	// animations
	,{ "icon-berserker" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     6,     50,     0, {   70,   500,     50,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    36,     36,        4,      9,      6
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   57,       3,     6,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    16,   0,       0,      0,      1,      70
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_BerserkerButtons	// buttons
   ,{		// sound
	 { "berserker-selected" }
	,{ "berserker-acknowledge" }
	,{ "berserker-ready" }
	,{ "berserker-help" }
	,{ "berserker-dead" }
   },   {	 { "berserker-attack" }
   } },

{   UnitTypeType, "unit-alleria"
    ,"Alleria"
    ,"unit-archer", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_AlleriaAnimations	// animations
	,{ "icon-alleria" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     9,    120,     0, {   70,   500,     50,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    33,     33,        7,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     5,   55,      10,    18,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    15,   0,       0,      0,      1,      60
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_AlleriaButtons	// buttons
   ,{		// sound
	 { "alleria-selected" }
	,{ "alleria-acknowledge" }
	,{ "alleria-ready" }
	,{ "alleria-help" }
	,{ "alleria-dead" }
   },   {	 { "alleria-attack" }
   } },

{   UnitTypeType, "unit-teron-gorefiend"
    ,"Teron Gorefiend"
    ,"unit-death-knight", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_TeronGorefiendAnimations	// animations
	,{ "icon-teron-gorefiend" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     8,      0,     9,    180,     1, {  120,  1200,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    39,     39,        4,     11,      9
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     2,   70,       0,    16,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    10,   0,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     1,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     1,       1,     1
   ,_TeronGorefiendButtons	// buttons
   ,{		// sound
	 { "teron-gorefiend-selected" }
	,{ "teron-gorefiend-acknowledge" }
	,{ "teron-gorefiend-ready" }
	,{ "teron-gorefiend-help" }
	,{ "teron-gorefiend-dead" }
   },   {	 { "teron-gorefiend-attack" }
   } },

{   UnitTypeType, "unit-kurdan-and-sky'ree"
    ,"Kurdan and Sky'ree"
    ,NULL, {
	"gryphon rider.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 80, 80			// graphic size
   ,_KurdanAndSky_reeAnimations	// animations
	,{ "icon-kurdan-and-sky'ree" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    14,      0,     9,    250,     0, {  250,  2500,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        5,      8,      6
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     6,   65,       0,    25,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     1,   1,       0,      0,      1,     150
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     1,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_KurdanAndSky_reeButtons	// buttons
   ,{		// sound
	 { "kurdan-and-sky'ree-selected" }
	,{ "kurdan-and-sky'ree-acknowledge" }
	,{ "kurdan-and-sky'ree-ready" }
	,{ "kurdan-and-sky'ree-help" }
	,{ "kurdan-and-sky'ree-dead" }
   },   {	 { "kurdan-and-sky'ree-attack" }
   } },

{   UnitTypeType, "unit-dentarg"
    ,"Dentarg"
    ,"unit-ogre", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_DentargAnimations	// animations
	,{ "icon-dentarg" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    13,      0,     6,    300,     1, {   90,   800,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    42,     42,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     8,   63,      18,     6,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_DentargButtons	// buttons
   ,{		// sound
	 { "dentarg-selected" }
	,{ "dentarg-acknowledge" }
	,{ "dentarg-ready" }
	,{ "dentarg-help" }
	,{ "dentarg-dead" }
   },   {	 { "dentarg-attack" }
   } },

{   UnitTypeType, "unit-khadgar"
    ,"Khadgar"
    ,"unit-mage", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_KhadgarAnimations	// animations
	,{ "icon-khadgar" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     8,      0,     9,    120,     1, {  120,  1200,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    33,     33,        6,     11,      9
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     3,   70,       0,    16,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     1,       1,     1
   ,_KhadgarButtons	// buttons
   ,{		// sound
	 { "khadgar-selected" }
	,{ "khadgar-acknowledge" }
	,{ "khadgar-ready" }
	,{ "khadgar-help" }
	,{ "khadgar-dead" }
   },   {	 { "khadgar-attack" }
   } },

{   UnitTypeType, "unit-grom-hellscream"
    ,"Grom Hellscream"
    ,"unit-grunt", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_GnomHellscreamAnimations	// animations
	,{ "icon-grom-hellscream" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     5,    240,     0, {   60,   600,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     8,   60,      16,     6,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,      50
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_GnomHellscreamButtons	// buttons
   ,{		// sound
	 { "grom-hellscream-selected" }
	,{ "grom-hellscream-acknowledge" }
	,{ "grom-hellscream-ready" }
	,{ "grom-hellscream-help" }
	,{ "grom-hellscream-dead" }
   },   {	 { "grom-hellscream-attack" }
   } },

{   UnitTypeType, "unit-human-oil-tanker"
    ,"Oil tanker"
    ,NULL, {
	"human tanker (empty).png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_TankerHumanAnimations	// animations
	,{ "icon-human-oil-tanker" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     90,     0, {   50,   400,    200,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    10,   50,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   2,       0,     10,      4,      40
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      1,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_TankerHumanButtons	// buttons
   ,{		// sound
	 { "human-oil-tanker-selected" }
	,{ "human-oil-tanker-acknowledge" }
	,{ "human-oil-tanker-ready" }
	,{ "human-oil-tanker-help" }
	,{ "human-oil-tanker-dead" }
   },   {	 { "human-oil-tanker-attack" }
   } },

{   UnitTypeType, "unit-orc-oil-tanker"
    ,"Oil tanker"
    ,NULL, {
	"orc tanker (empty).png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_TankerOrcAnimations	// animations
	,{ "icon-orc-oil-tanker" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     90,     0, {   50,   400,    200,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    10,   50,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   2,       0,     10,      4,      40
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      1,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_TankerOrcButtons	// buttons
   ,{		// sound
	 { "orc-oil-tanker-selected" }
	,{ "orc-oil-tanker-acknowledge" }
	,{ "orc-oil-tanker-ready" }
	,{ "orc-oil-tanker-help" }
	,{ "orc-oil-tanker-dead" }
   },   {	 { "orc-oil-tanker-attack" }
   } },

{   UnitTypeType, "unit-human-transport"
    ,"Transport"
    ,NULL, {
	"human transport.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_TransportHumanAnimations	// animations
	,{ "icon-human-transport" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,    150,     0, {   70,   600,    200,   500 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   70,       0,     0,      0,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   2,       0,     15,      6,      50
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     1,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_TransportHumanButtons	// buttons
   ,{		// sound
	 { "human-transport-selected" }
	,{ "human-transport-acknowledge" }
	,{ "human-transport-ready" }
	,{ "human-transport-help" }
	,{ "human-transport-dead" }
   },   {	 { "human-transport-attack" }
   } },

{   UnitTypeType, "unit-orc-transport"
    ,"Transport"
    ,NULL, {
	"orc transport.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_TransportOrcAnimations	// animations
	,{ "icon-orc-transport" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,    150,     0, {   70,   600,    200,   500 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   70,       0,     0,      0,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   2,       0,     15,      6,      50
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     1,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_TransportOrcButtons	// buttons
   ,{		// sound
	 { "orc-transport-selected" }
	,{ "orc-transport-acknowledge" }
	,{ "orc-transport-ready" }
	,{ "orc-transport-help" }
	,{ "orc-transport-dead" }
   },   {	 { "orc-transport-attack" }
   } },

{   UnitTypeType, "unit-elven-destroyer"
    ,"Elven Destroyer"
    ,NULL, {
	"elven destroyer.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 80, 88			// graphic size
   ,_ElvenDestroyerAnimations	// animations
	,{ "icon-elven-destroyer" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     8,    100,     0, {   90,   700,    350,   700 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        4,     10,      8
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    10,   65,      35,     0,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    24,   2,       0,     20,      1,     150
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_ElvenDestroyerButtons	// buttons
   ,{		// sound
	 { "elven-destroyer-selected" }
	,{ "elven-destroyer-acknowledge" }
	,{ "elven-destroyer-ready" }
	,{ "elven-destroyer-help" }
	,{ "elven-destroyer-dead" }
   },   {	 { "elven-destroyer-attack" }
   } },

{   UnitTypeType, "unit-troll-destroyer"
    ,"Troll Destroyer"
    ,NULL, {
	"troll destroyer.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 88, 88			// graphic size
   ,_TrollDestroyerAnimations	// animations
	,{ "icon-troll-destroyer" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     8,    100,     0, {   90,   700,    350,   700 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        4,     10,      8
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    10,   65,      35,     0,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    24,   2,       0,     20,      1,     150
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_TrollDestroyerButtons	// buttons
   ,{		// sound
	 { "troll-destroyer-selected" }
	,{ "troll-destroyer-acknowledge" }
	,{ "troll-destroyer-ready" }
	,{ "troll-destroyer-help" }
	,{ "troll-destroyer-dead" }
   },   {	 { "troll-destroyer-attack" }
   } },

{   UnitTypeType, "unit-battleship"
    ,"Battleship"
    ,NULL, {
	"battleship.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 88, 88			// graphic size
   ,_BattleshipAnimations	// animations
	,{ "icon-battleship" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     6,      0,     8,    150,     0, {  140,  1000,    500,  1000 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    70,     70,        6,     10,      8
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    15,   63,     130,     0,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     7,   2,       0,     25,      1,     300
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    3,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       1
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_BattleshipButtons	// buttons
   ,{		// sound
	 { "battleship-selected" }
	,{ "battleship-acknowledge" }
	,{ "battleship-ready" }
	,{ "battleship-help" }
	,{ "battleship-dead" }
   },   {	 { "battleship-attack" }
   } },

{   UnitTypeType, "unit-ogre-juggernaught"
    ,"Ogre Juggernaught"
    ,NULL, {
	"juggernaught.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 88, 88			// graphic size
   ,_JuggernaughtAnimations	// animations
	,{ "icon-ogre-juggernaught" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     6,      0,     8,    150,     0, {  140,  1000,    500,  1000 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    70,     70,        6,     10,      8
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    15,   63,     130,     0,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     7,   2,       0,     25,      1,     300
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    3,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       1
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_JuggernaughtButtons	// buttons
   ,{		// sound
	 { "ogre-juggernaught-selected" }
	,{ "ogre-juggernaught-acknowledge" }
	,{ "ogre-juggernaught-ready" }
	,{ "ogre-juggernaught-help" }
	,{ "ogre-juggernaught-dead" }
   },   {	 { "ogre-juggernaught-attack" }
   } },

{   UnitTypeType, "unit-nothing-22"
    ,"Nothing 22"
    ,NULL, {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    ,  0,  0			// graphic size
   ,_NothingAnimations	// animations
	,{ "icon-cancel" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    99,      0,     0,      0,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     0,    0,     0,      0,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_NothingButtons	// buttons
   ,{		// sound
	 { "nothing-22-selected" }
	,{ "nothing-22-acknowledge" }
	,{ "nothing-22-ready" }
	,{ "nothing-22-help" }
	,{ "nothing-22-dead" }
   },   {	 { "nothing-22-attack" }
   } },

{   UnitTypeType, "unit-deathwing"
    ,"Deathwing"
    ,NULL, {
	"dragon.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 88, 80			// graphic size
   ,_DeathwingAnimations	// animations
	,{ "icon-deathwing" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    14,      0,     9,    800,     0, {  250,  2500,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    71,     71,        5,      8,      6
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    10,   65,      10,    25,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     2,   1,       0,      0,      1,     150
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     1,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_DeathwingButtons	// buttons
   ,{		// sound
	 { "deathwing-selected" }
	,{ "deathwing-acknowledge" }
	,{ "deathwing-ready" }
	,{ "deathwing-help" }
	,{ "deathwing-dead" }
   },   {	 { "deathwing-attack" }
   } },

{   UnitTypeType, "unit-nothing-24"
    ,"Nothing 24"
    ,NULL, {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 32, 32			// graphic size
   ,_Nothing1Animations	// animations
	,{ "icon-cancel" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    99,      0,     4,     60,     0, {   60,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,     20,     10
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     2,   40,       9,     1,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   2,       0,      0,      1,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_Nothing1Buttons	// buttons
   ,{		// sound
	 { "nothing-24-selected" }
	,{ "nothing-24-acknowledge" }
	,{ "nothing-24-ready" }
	,{ "nothing-24-help" }
	,{ "nothing-24-dead" }
   },   {	 { "nothing-24-attack" }
   } },

{   UnitTypeType, "unit-nothing-25"
    ,"Nothing 25"
    ,NULL, {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 32, 32			// graphic size
   ,_Nothing2Animations	// animations
	,{ "icon-cancel" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    99,      0,     4,     60,     0, {   60,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,     20,     10
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     2,   40,       9,     1,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   2,       0,      0,      1,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_Nothing2Buttons	// buttons
   ,{		// sound
	 { "nothing-25-selected" }
	,{ "nothing-25-acknowledge" }
	,{ "nothing-25-ready" }
	,{ "nothing-25-help" }
	,{ "nothing-25-dead" }
   },   {	 { "nothing-25-attack" }
   } },

{   UnitTypeType, "unit-gnomish-submarine"
    ,"Gnomish Submarine"
    ,NULL, {
	"gnomish submarine (summer,winter).png"
	,DEFAULT
	,"gnomish submarine (wasteland).png"
	,"gnomish submarine (swamp).png" }
    , 72, 72			// graphic size
   ,_GnomishSubmarineAnimations	// animations
	,{ "icon-gnomish-submarine" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     7,      0,     5,     60,     0, {  100,   800,    150,   900 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        4,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   60,      50,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    17,   2,       0,     20,      1,     120
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    2,    0,     0,      1,          0,      0,    0,          1
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_GnomishSubmarineButtons	// buttons
   ,{		// sound
	 { "gnomish-submarine-selected" }
	,{ "gnomish-submarine-acknowledge" }
	,{ "gnomish-submarine-ready" }
	,{ "gnomish-submarine-help" }
	,{ "gnomish-submarine-dead" }
   },   {	 { "gnomish-submarine-attack" }
   } },

{   UnitTypeType, "unit-giant-turtle"
    ,"Giant Turtle"
    ,NULL, {
	"giant turtle (summer,winter).png"
	,DEFAULT
	,"giant turtle (wasteland).png"
	,"giant turtle (swamp).png" }
    , 72, 72			// graphic size
   ,_GiantTurtleAnimations	// animations
	,{ "icon-giant-turtle" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     7,      0,     5,     60,     0, {  100,   800,    150,   900 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        4,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   60,      50,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    18,   2,       0,     20,      1,     120
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    2,    0,     0,      1,          0,      0,    0,          1
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_GiantTurtleButtons	// buttons
   ,{		// sound
	 { "giant-turtle-selected" }
	,{ "giant-turtle-acknowledge" }
	,{ "giant-turtle-ready" }
	,{ "giant-turtle-help" }
	,{ "giant-turtle-dead" }
   },   {	 { "giant-turtle-attack" }
   } },

{   UnitTypeType, "unit-gnomish-flying-machine"
    ,"Gnomish Flying Machine"
    ,NULL, {
	"gnomish flying machine.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 80, 80			// graphic size
   ,_GnomishFlyingMachineAnimations	// animations
	,{ "icon-gnomish-flying-machine" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    17,      0,     9,    150,     0, {   65,   500,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,     19,     15
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     2,   40,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   1,       0,      0,      2,      40
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     1,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_GnomishFlyingMachineButtons	// buttons
   ,{		// sound
	 { "gnomish-flying-machine-selected" }
	,{ "gnomish-flying-machine-acknowledge" }
	,{ "gnomish-flying-machine-ready" }
	,{ "gnomish-flying-machine-help" }
	,{ "gnomish-flying-machine-dead" }
   },   {	 { "gnomish-flying-machine-attack" }
   } },

{   UnitTypeType, "unit-goblin-zeppelin"
    ,"Goblin Zeppelin"
    ,NULL, {
	"goblin zeppelin.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_GoblinZeppelinAnimations	// animations
	,{ "icon-goblin-zeppelin" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    17,      0,     9,    150,     0, {   65,   500,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,     19,     15
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     2,   40,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   1,       0,      0,      2,      40
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     1,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_GoblinZeppelinButtons	// buttons
   ,{		// sound
	 { "goblin-zeppelin-selected" }
	,{ "goblin-zeppelin-acknowledge" }
	,{ "goblin-zeppelin-ready" }
	,{ "goblin-zeppelin-help" }
	,{ "goblin-zeppelin-dead" }
   },   {	 { "goblin-zeppelin-attack" }
   } },

{   UnitTypeType, "unit-gryphon-rider"
    ,"Gryphon Rider"
    ,"unit-kurdan-and-sky'ree", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 80, 80			// graphic size
   ,_GryphonRiderAnimations	// animations
	,{ "icon-gryphon-rider" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    14,      0,     6,    100,     0, {  250,  2500,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        4,      8,      6
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     5,   65,       0,    16,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     1,   1,       0,      0,      1,     150
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     1,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_GryphonRiderButtons	// buttons
   ,{		// sound
	 { "gryphon-rider-selected" }
	,{ "gryphon-rider-acknowledge" }
	,{ "gryphon-rider-ready" }
	,{ "gryphon-rider-help" }
	,{ "gryphon-rider-dead" }
   },   {	 { "gryphon-rider-attack" }
   } },

{   UnitTypeType, "unit-dragon"
    ,"Dragon"
    ,"unit-deathwing", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 88, 80			// graphic size
   ,_DragonAnimations	// animations
	,{ "icon-dragon" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    14,      0,     6,    100,     0, {  250,  2500,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    71,     71,        4,      8,      6
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     5,   65,       0,    16,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     2,   1,       0,      0,      1,     150
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     1,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_DragonButtons	// buttons
   ,{		// sound
	 { "dragon-selected" }
	,{ "dragon-acknowledge" }
	,{ "dragon-ready" }
	,{ "dragon-help" }
	,{ "dragon-dead" }
   },   {	 { "dragon-attack" }
   } },

{   UnitTypeType, "unit-turalyon"
    ,"Turalyon"
    ,"unit-knight", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_TuralyonAnimations	// animations
	,{ "icon-turalyon" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    13,      0,     6,    180,     0, {   90,   800,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    42,     42,        1,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    10,   65,      14,     5,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,     110
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_TuralyonButtons	// buttons
   ,{		// sound
	 { "turalyon-selected" }
	,{ "turalyon-acknowledge" }
	,{ "turalyon-ready" }
	,{ "turalyon-help" }
	,{ "turalyon-dead" }
   },   {	 { "turalyon-attack" }
   } },

{   UnitTypeType, "unit-eye-of-kilrogg"
    ,"Eye of Kilrogg"
    ,NULL, {
	"eye of kilrogg.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 32, 32			// graphic size
   ,_EyeOfKilroggAnimations	// animations
	,{ "icon-eye-of-kilrogg" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    42,      0,     3,    100,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,     20,     10
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   1,       3,      0,      2,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     1,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_EyeOfKilroggButtons	// buttons
   ,{		// sound
	 { "eye-of-kilrogg-selected" }
	,{ "eye-of-kilrogg-acknowledge" }
	,{ "eye-of-kilrogg-ready" }
	,{ "eye-of-kilrogg-help" }
	,{ "eye-of-kilrogg-dead" }
   },   {	 { "eye-of-kilrogg-attack" }
   } },

{   UnitTypeType, "unit-danath"
    ,"Danath"
    ,"unit-footman", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_DanathAnimations	// animations
	,{ "icon-danath" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     6,    220,     0, {   60,   600,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     8,   60,      15,     8,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,      50
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_DanathButtons	// buttons
   ,{		// sound
	 { "danath-selected" }
	,{ "danath-acknowledge" }
	,{ "danath-ready" }
	,{ "danath-help" }
	,{ "danath-dead" }
   },   {	 { "danath-attack" }
   } },

{   UnitTypeType, "unit-korgath-bladefist"
    ,"Korgath Bladefist"
    ,"unit-grunt", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_KorgathBladefistAnimations	// animations
	,{ "icon-korgath-bladefist" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     5,    240,     0, {   60,   600,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     8,   60,      16,     6,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,      50
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_KorgathBladefistButtons	// buttons
   ,{		// sound
	 { "korgath-bladefist-selected" }
	,{ "korgath-bladefist-acknowledge" }
	,{ "korgath-bladefist-ready" }
	,{ "korgath-bladefist-help" }
	,{ "korgath-bladefist-dead" }
   },   {	 { "korgath-bladefist-attack" }
   } },

{   UnitTypeType, "unit-nothing-30"
    ,"Nothing 30"
    ,NULL, {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    ,  0,  0			// graphic size
   ,_Nothing3Animations	// animations
	,{ "icon-cancel" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    99,      0,     0,      0,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     0,    0,     0,      0,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_Nothing3Buttons	// buttons
   ,{		// sound
	 { "nothing-30-selected" }
	,{ "nothing-30-acknowledge" }
	,{ "nothing-30-ready" }
	,{ "nothing-30-help" }
	,{ "nothing-30-dead" }
   },   {	 { "nothing-30-attack" }
   } },

{   UnitTypeType, "unit-cho'gall"
    ,"Cho'gall"
    ,"unit-ogre", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_Cho_gallAnimations	// animations
	,{ "icon-cho'gall" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    13,      0,     5,    100,     0, {  100,  1100,     50,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    42,     42,        1,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   65,      10,     5,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,     120
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    1,    0,         0,     0,       1,     1
   ,_Cho_gallButtons	// buttons
   ,{		// sound
	 { "cho'gall-selected" }
	,{ "cho'gall-acknowledge" }
	,{ "cho'gall-ready" }
	,{ "cho'gall-help" }
	,{ "cho'gall-dead" }
   },   {	 { "cho'gall-attack" }
   } },

{   UnitTypeType, "unit-lothar"
    ,"Lothar"
    ,"unit-knight", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_LotharAnimations	// animations
	,{ "icon-lothar" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    13,      0,     5,     90,     0, {  100,   900,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    42,     42,        1,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     4,   65,       8,     4,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,     120
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    1,    0,         0,     0,       1,     1
   ,_LotharButtons	// buttons
   ,{		// sound
	 { "lothar-selected" }
	,{ "lothar-acknowledge" }
	,{ "lothar-ready" }
	,{ "lothar-help" }
	,{ "lothar-dead" }
   },   {	 { "lothar-attack" }
   } },

{   UnitTypeType, "unit-gul'dan"
    ,"Gul'dan"
    ,"unit-death-knight", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_Gul_danAnimations	// animations
	,{ "icon-gul'dan" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     8,      0,     8,     40,     1, {  120,  1200,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    33,     33,        3,     10,      8
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   70,       0,     3,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    10,   0,       0,      0,      1,     120
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     1,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    1,    0,         0,     0,       1,     1
   ,_Gul_danButtons	// buttons
   ,{		// sound
	 { "gul'dan-selected" }
	,{ "gul'dan-acknowledge" }
	,{ "gul'dan-ready" }
	,{ "gul'dan-help" }
	,{ "gul'dan-dead" }
   },   {	 { "gul'dan-attack" }
   } },

{   UnitTypeType, "unit-uther-lightbringer"
    ,"Uther Lightbringer"
    ,"unit-knight", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_UtherLightbringerAnimations	// animations
	,{ "icon-uther-lightbringer" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    13,      0,     5,     90,     0, {  100,   900,    100,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    42,     42,        1,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     4,   65,       8,     4,      1,      1
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      1,     120
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      1,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    1,    0,         0,     0,       1,     1
   ,_UtherLightbringerButtons	// buttons
   ,{		// sound
	 { "uther-lightbringer-selected" }
	,{ "uther-lightbringer-acknowledge" }
	,{ "uther-lightbringer-ready" }
	,{ "uther-lightbringer-help" }
	,{ "uther-lightbringer-dead" }
   },   {	 { "uther-lightbringer-attack" }
   } },

{   UnitTypeType, "unit-zuljin"
    ,"Zuljin"
    ,"unit-axethrower", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_ZuljinAnimations	// animations
	,{ "icon-zuljin" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     6,     40,     0, {   70,   500,     50,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    36,     36,        5,      8,      6
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   55,       3,     6,      1,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    16,   0,       0,      0,      1,     120
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    1,    0,         0,     0,       1,     1
   ,_ZuljinButtons	// buttons
   ,{		// sound
	 { "zuljin-selected" }
	,{ "zuljin-acknowledge" }
	,{ "zuljin-ready" }
	,{ "zuljin-help" }
	,{ "zuljin-dead" }
   },   {	 { "zuljin-attack" }
   } },

{   UnitTypeType, "unit-nothing-36"
    ,"Nothing 36"
    ,NULL, {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    ,  0,  0			// graphic size
   ,_Nothing4Animations	// animations
	,{ "icon-cancel" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    99,      0,     0,      0,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     0,    0,     0,      0,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_Nothing4Buttons	// buttons
   ,{		// sound
	 { "nothing-36-selected" }
	,{ "nothing-36-acknowledge" }
	,{ "nothing-36-ready" }
	,{ "nothing-36-help" }
	,{ "nothing-36-dead" }
   },   {	 { "nothing-36-attack" }
   } },

{   UnitTypeType, "unit-skeleton"
    ,"Skeleton"
    ,NULL, {
	"skeleton.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 56, 56			// graphic size
   ,_SkeletonAnimations	// animations
	,{ "icon-skeleton" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    99,      0,     3,     40,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      4,      2
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   55,       6,     3,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,     100,      0,      1,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      1,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     1,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_SkeletonButtons	// buttons
   ,{		// sound
	 { "skeleton-selected" }
	,{ "skeleton-acknowledge" }
	,{ "skeleton-ready" }
	,{ "skeleton-help" }
	,{ "skeleton-dead" }
   },   {	 { "skeleton-attack" }
   } },

{   UnitTypeType, "unit-daemon"
    ,"Daemon"
    ,NULL, {
	"daemon.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_DaemonAnimations	// animations
	,{ "icon-daemon" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    99,      0,     5,     60,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        2,      7,      5
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     2,   63,       9,     1,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    27,   1,       0,      0,      1,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     1,      0,          0,      1,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     0
   ,_DaemonButtons	// buttons
   ,{		// sound
	 { "daemon-selected" }
	,{ "daemon-acknowledge" }
	,{ "daemon-ready" }
	,{ "daemon-help" }
	,{ "daemon-dead" }
   },   {	 { "daemon-attack" }
   } },

{   UnitTypeType, "unit-critter"
    ,"Critter"
    ,NULL, {
	"critter (summer).png"
	,"critter (winter).png"
	,"critter (wasteland).png"
	,"critter (swamp).png" }
    , 32, 32			// graphic size
   ,_CritterAnimations	// animations
	,{ "icon-critter" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     3,      0,     2,      5,     0, {   30,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,     20,     10
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   37,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      2,       1
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    1,     0,      0,          0,      1,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     0
   ,_CritterButtons	// buttons
   ,{		// sound
	 { "critter-selected" }
	,{ "critter-acknowledge" }
	,{ "critter-ready" }
	,{ "critter-help" }
	,{ "critter-dead" }
   },   {	 { "critter-attack" }
   } },

{   UnitTypeType, "unit-farm"
    ,"Farm"
    ,NULL, {
	"farm (summer).png"
	,"farm (winter).png"
	,"farm (wasteland).png"
	,"farm (swamp).png" }
    , 64, 64			// graphic size
   ,_FarmAnimations	// animations
	,{ "icon-farm" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    400,     0, {  100,   500,    250,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   20,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     45,      0,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_FarmButtons	// buttons
   ,{		// sound
	 { "farm-selected" }
	,{ "farm-acknowledge" }
	,{ "farm-ready" }
	,{ "farm-help" }
	,{ "farm-dead" }
   },   {	 { "farm-attack" }
   } },

{   UnitTypeType, "unit-pig-farm"
    ,"Pig Farm"
    ,NULL, {
	"pig farm (summer).png"
	,"pig farm (winter).png"
	,"pig farm (wasteland).png"
	,"pig farm (swamp).png" }
    , 64, 64			// graphic size
   ,_PigFarmAnimations	// animations
	,{ "icon-pig-farm" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    400,     0, {  100,   500,    250,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   20,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     45,      0,     100
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_PigFarmButtons	// buttons
   ,{		// sound
	 { "pig-farm-selected" }
	,{ "pig-farm-acknowledge" }
	,{ "pig-farm-ready" }
	,{ "pig-farm-help" }
	,{ "pig-farm-dead" }
   },   {	 { "pig-farm-attack" }
   } },

{   UnitTypeType, "unit-human-barracks"
    ,"Barracks"
    ,NULL, {
	"human barracks (summer).png"
	,"human barracks (winter).png"
	,DEFAULT
	,"human barracks (swamp).png" }
    , 96, 96			// graphic size
   ,_BarracksHumanAnimations	// animations
	,{ "icon-human-barracks" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    800,     0, {  200,   700,    450,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   30,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     35,      0,     160
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_BarracksHumanButtons	// buttons
   ,{		// sound
	 { "human-barracks-selected" }
	,{ "human-barracks-acknowledge" }
	,{ "human-barracks-ready" }
	,{ "human-barracks-help" }
	,{ "human-barracks-dead" }
   },   {	 { "human-barracks-attack" }
   } },

{   UnitTypeType, "unit-orc-barracks"
    ,"Barracks"
    ,NULL, {
	"orc barracks (summer).png"
	,"orc barracks (winter).png"
	,DEFAULT
	,"orc barracks (swamp).png" }
    , 96, 96			// graphic size
   ,_BarracksOrcAnimations	// animations
	,{ "icon-orc-barracks" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    800,     0, {  200,   700,    450,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   30,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     35,      0,     160
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_BarracksOrcButtons	// buttons
   ,{		// sound
	 { "orc-barracks-selected" }
	,{ "orc-barracks-acknowledge" }
	,{ "orc-barracks-ready" }
	,{ "orc-barracks-help" }
	,{ "orc-barracks-dead" }
   },   {	 { "orc-barracks-attack" }
   } },

{   UnitTypeType, "unit-church"
    ,"Church"
    ,NULL, {
	"church (summer).png"
	,"church (winter).png"
	,DEFAULT
	,"church (swamp).png" }
    , 96, 96			// graphic size
   ,_ChurchAnimations	// animations
	,{ "icon-church" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    700,     0, {  175,   900,    500,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     35,      0,     240
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_ChurchButtons	// buttons
   ,{		// sound
	 { "church-selected" }
	,{ "church-acknowledge" }
	,{ "church-ready" }
	,{ "church-help" }
	,{ "church-dead" }
   },   {	 { "church-attack" }
   } },

{   UnitTypeType, "unit-altar-of-storms"
    ,"Altar of Storms"
    ,NULL, {
	"altar of storms (summer).png"
	,"altar of storms (winter).png"
	,DEFAULT
	,"altar of storms (swamp).png" }
    , 96, 96			// graphic size
   ,_AltarOfStormsAnimations	// animations
	,{ "icon-altar-of-storms" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    700,     0, {  175,   900,    500,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     35,      0,     240
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_AltarOfStormsButtons	// buttons
   ,{		// sound
	 { "altar-of-storms-selected" }
	,{ "altar-of-storms-acknowledge" }
	,{ "altar-of-storms-ready" }
	,{ "altar-of-storms-help" }
	,{ "altar-of-storms-dead" }
   },   {	 { "altar-of-storms-attack" }
   } },

{   UnitTypeType, "unit-human-watch-tower"
    ,"Scout Tower"
    ,NULL, {
	"human scout tower (summer).png"
	,"human scout tower (winter).png"
	,DEFAULT
	,"human scout tower (swamp).png" }
    , 64, 64			// graphic size
   ,_ScoutTowerHumanAnimations	// animations
	,{ "icon-human-watch-tower" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     9,    100,     0, {   60,   550,    200,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   55,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,     50,      0,      95
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_ScoutTowerHumanButtons	// buttons
   ,{		// sound
	 { "human-watch-tower-selected" }
	,{ "human-watch-tower-acknowledge" }
	,{ "human-watch-tower-ready" }
	,{ "human-watch-tower-help" }
	,{ "human-watch-tower-dead" }
   },   {	 { "human-watch-tower-attack" }
   } },

{   UnitTypeType, "unit-orc-watch-tower"
    ,"Scout Tower"
    ,NULL, {
	"orc scout tower (summer).png"
	,"orc scout tower (winter).png"
	,DEFAULT
	,"orc scout tower (swamp).png" }
    , 64, 64			// graphic size
   ,_ScoutTowerOrcAnimations	// animations
	,{ "icon-orc-watch-tower" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     9,    100,     0, {   60,   550,    200,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   55,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,     50,      0,      95
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_ScoutTowerOrcButtons	// buttons
   ,{		// sound
	 { "orc-watch-tower-selected" }
	,{ "orc-watch-tower-acknowledge" }
	,{ "orc-watch-tower-ready" }
	,{ "orc-watch-tower-help" }
	,{ "orc-watch-tower-dead" }
   },   {	 { "orc-watch-tower-attack" }
   } },

{   UnitTypeType, "unit-stables"
    ,"Stables"
    ,NULL, {
	"stables (summer).png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 96, 96			// graphic size
   ,_StablesAnimations	// animations
	,{ "icon-stables" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    500,     0, {  150,  1000,    300,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     15,      0,     210
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_StablesButtons	// buttons
   ,{		// sound
	 { "stables-selected" }
	,{ "stables-acknowledge" }
	,{ "stables-ready" }
	,{ "stables-help" }
	,{ "stables-dead" }
   },   {	 { "stables-attack" }
   } },

{   UnitTypeType, "unit-ogre-mound"
    ,"Ogre Mound"
    ,NULL, {
	"ogre mound (summer).png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 96, 96			// graphic size
   ,_OgreMoundAnimations	// animations
	,{ "icon-ogre-mound" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    500,     0, {  150,  1000,    300,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     15,      0,     210
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_OgreMoundButtons	// buttons
   ,{		// sound
	 { "ogre-mound-selected" }
	,{ "ogre-mound-acknowledge" }
	,{ "ogre-mound-ready" }
	,{ "ogre-mound-help" }
	,{ "ogre-mound-dead" }
   },   {	 { "ogre-mound-attack" }
   } },

{   UnitTypeType, "unit-gnomish-inventor"
    ,"Gnomish Inventor"
    ,NULL, {
	"gnomish inventor (summer).png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 96, 96			// graphic size
   ,_GnomishInventorAnimations	// animations
	,{ "icon-gnomish-inventor" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    500,     0, {  150,  1000,    400,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     230
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_GnomishInventorButtons	// buttons
   ,{		// sound
	 { "gnomish-inventor-selected" }
	,{ "gnomish-inventor-acknowledge" }
	,{ "gnomish-inventor-ready" }
	,{ "gnomish-inventor-help" }
	,{ "gnomish-inventor-dead" }
   },   {	 { "gnomish-inventor-attack" }
   } },

{   UnitTypeType, "unit-goblin-alchemist"
    ,"Goblin Alchemist"
    ,NULL, {
	"goblin alchemist (summer).png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 96, 96			// graphic size
   ,_GoblinAlchemistAnimations	// animations
	,{ "icon-goblin-alchemist" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    500,     0, {  150,  1000,    400,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     230
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_GoblinAlchemistButtons	// buttons
   ,{		// sound
	 { "goblin-alchemist-selected" }
	,{ "goblin-alchemist-acknowledge" }
	,{ "goblin-alchemist-ready" }
	,{ "goblin-alchemist-help" }
	,{ "goblin-alchemist-dead" }
   },   {	 { "goblin-alchemist-attack" }
   } },

{   UnitTypeType, "unit-gryphon-aviary"
    ,"Gryphon Aviary"
    ,NULL, {
	"gryphon aviary (summer).png"
	,"gryphon aviary (winter).png"
	,DEFAULT
	,"gryphon aviary (swamp).png" }
    , 96, 96			// graphic size
   ,_GryphonAviaryAnimations	// animations
	,{ "icon-gryphon-aviary" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    500,     0, {  150,  1000,    400,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     280
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_GryphonAviaryButtons	// buttons
   ,{		// sound
	 { "gryphon-aviary-selected" }
	,{ "gryphon-aviary-acknowledge" }
	,{ "gryphon-aviary-ready" }
	,{ "gryphon-aviary-help" }
	,{ "gryphon-aviary-dead" }
   },   {	 { "gryphon-aviary-attack" }
   } },

{   UnitTypeType, "unit-dragon-roost"
    ,"Dragon Roost"
    ,NULL, {
	"dragon roost (summer).png"
	,"dragon roost (winter).png"
	,DEFAULT
	,"dragon roost (swamp).png" }
    , 96, 96			// graphic size
   ,_DragonRoostAnimations	// animations
	,{ "icon-dragon-roost" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    500,     0, {  150,  1000,    400,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     280
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_DragonRoostButtons	// buttons
   ,{		// sound
	 { "dragon-roost-selected" }
	,{ "dragon-roost-acknowledge" }
	,{ "dragon-roost-ready" }
	,{ "dragon-roost-help" }
	,{ "dragon-roost-dead" }
   },   {	 { "dragon-roost-attack" }
   } },

{   UnitTypeType, "unit-human-shipyard"
    ,"Shipyard"
    ,NULL, {
	"human shipyard (summer).png"
	,"human shipyard (winter).png"
	,DEFAULT
	,"human shipyard (swamp).png" }
    , 96, 96			// graphic size
   ,_ShipyardHumanAnimations	// animations
	,{ "icon-human-shipyard" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      7,     3,   1100,     0, {  200,   800,    450,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   30,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     170
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    1,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    1,         0,     0,       0,     0
   ,_ShipyardHumanButtons	// buttons
   ,{		// sound
	 { "human-shipyard-selected" }
	,{ "human-shipyard-acknowledge" }
	,{ "human-shipyard-ready" }
	,{ "human-shipyard-help" }
	,{ "human-shipyard-dead" }
   },   {	 { "human-shipyard-attack" }
   } },

{   UnitTypeType, "unit-orc-shipyard"
    ,"Shipyard"
    ,NULL, {
	"orc shipyard (summer).png"
	,"orc shipyard (winter).png"
	,DEFAULT
	,"orc shipyard (swamp).png" }
    , 96, 96			// graphic size
   ,_ShipyardOrcAnimations	// animations
	,{ "icon-orc-shipyard" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      8,     3,   1100,     0, {  200,   800,    450,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   30,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     170
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    1,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    1,         0,     0,       0,     0
   ,_ShipyardOrcButtons	// buttons
   ,{		// sound
	 { "orc-shipyard-selected" }
	,{ "orc-shipyard-acknowledge" }
	,{ "orc-shipyard-ready" }
	,{ "orc-shipyard-help" }
	,{ "orc-shipyard-dead" }
   },   {	 { "orc-shipyard-attack" }
   } },

{   UnitTypeType, "unit-town-hall"
    ,"Town Hall"
    ,NULL, {
	"town hall (summer).png"
	,"town hall (winter).png"
	,DEFAULT
	,"town hall (swamp).png" }
    ,128,128			// graphic size
   ,_TownHallAnimations	// animations
	,{ "icon-town-hall" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     4,   1200,     0, {  255,  1200,    800,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     4,    4,   126,    126,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   35,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     45,      0,     200
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      1,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_TownHallButtons	// buttons
   ,{		// sound
	 { "town-hall-selected" }
	,{ "town-hall-acknowledge" }
	,{ "town-hall-ready" }
	,{ "town-hall-help" }
	,{ "town-hall-dead" }
   },   {	 { "town-hall-attack" }
   } },

{   UnitTypeType, "unit-great-hall"
    ,"Great Hall"
    ,NULL, {
	"great hall (summer).png"
	,"great hall (winter).png"
	,DEFAULT
	,"great hall (swamp).png" }
    ,128,128			// graphic size
   ,_GreatHallAnimations	// animations
	,{ "icon-great-hall" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     4,   1200,     0, {  255,  1200,    800,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     4,    4,   127,    127,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   35,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     45,      0,     200
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      1,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_GreatHallButtons	// buttons
   ,{		// sound
	 { "great-hall-selected" }
	,{ "great-hall-acknowledge" }
	,{ "great-hall-ready" }
	,{ "great-hall-help" }
	,{ "great-hall-dead" }
   },   {	 { "great-hall-attack" }
   } },

{   UnitTypeType, "unit-elven-lumber-mill"
    ,"Elven Lumber Mill"
    ,NULL, {
	"elven lumber mill (summer).png"
	,"elven lumber mill (winter).png"
	,"elven lumber mill (wasteland).png"
	,"elven lumber mill (swamp).png" }
    , 96, 96			// graphic size
   ,_ElvenLumberMillAnimations	// animations
	,{ "icon-elven-lumber-mill" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    600,     0, {  150,   600,    450,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   25,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     15,      0,     150
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      1,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_ElvenLumberMillButtons	// buttons
   ,{		// sound
	 { "elven-lumber-mill-selected" }
	,{ "elven-lumber-mill-acknowledge" }
	,{ "elven-lumber-mill-ready" }
	,{ "elven-lumber-mill-help" }
	,{ "elven-lumber-mill-dead" }
   },   {	 { "elven-lumber-mill-attack" }
   } },

{   UnitTypeType, "unit-troll-lumber-mill"
    ,"Troll Lumber Mill"
    ,NULL, {
	"troll lumber mill (summer).png"
	,"troll lumber mill (winter).png"
	,"troll lumber mill (wasteland).png"
	,"troll lumber mill (swamp).png" }
    , 96, 96			// graphic size
   ,_TrollLumberMillAnimations	// animations
	,{ "icon-troll-lumber-mill" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    600,     0, {  150,   600,    450,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   25,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     15,      0,     150
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      1,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_TrollLumberMillButtons	// buttons
   ,{		// sound
	 { "troll-lumber-mill-selected" }
	,{ "troll-lumber-mill-acknowledge" }
	,{ "troll-lumber-mill-ready" }
	,{ "troll-lumber-mill-help" }
	,{ "troll-lumber-mill-dead" }
   },   {	 { "troll-lumber-mill-attack" }
   } },

{   UnitTypeType, "unit-human-foundry"
    ,"Foundry"
    ,NULL, {
	"human foundry (summer).png"
	,"human foundry (winter).png"
	,DEFAULT
	,"human foundry (swamp).png" }
    , 96, 96			// graphic size
   ,_FoundryHumanAnimations	// animations
	,{ "icon-human-foundry" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,     13,     3,    750,     0, {  175,   700,    400,   400 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     200
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    1,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_FoundryHumanButtons	// buttons
   ,{		// sound
	 { "human-foundry-selected" }
	,{ "human-foundry-acknowledge" }
	,{ "human-foundry-ready" }
	,{ "human-foundry-help" }
	,{ "human-foundry-dead" }
   },   {	 { "human-foundry-attack" }
   } },

{   UnitTypeType, "unit-orc-foundry"
    ,"Foundry"
    ,NULL, {
	"orc foundry (summer).png"
	,"orc foundry (winter).png"
	,DEFAULT
	,"orc foundry (swamp).png" }
    , 96, 96			// graphic size
   ,_FoundryOrcAnimations	// animations
	,{ "icon-orc-foundry" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,     14,     3,    750,     0, {  175,   700,    400,   400 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     200
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    1,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_FoundryOrcButtons	// buttons
   ,{		// sound
	 { "orc-foundry-selected" }
	,{ "orc-foundry-acknowledge" }
	,{ "orc-foundry-ready" }
	,{ "orc-foundry-help" }
	,{ "orc-foundry-dead" }
   },   {	 { "orc-foundry-attack" }
   } },

{   UnitTypeType, "unit-mage-tower"
    ,"Mage Tower"
    ,NULL, {
	"mage tower (summer).png"
	,"mage tower (winter).png"
	,DEFAULT
	,"mage tower (swamp).png" }
    , 96, 96			// graphic size
   ,_MageTowerAnimations	// animations
	,{ "icon-mage-tower" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    500,     0, {  125,  1000,    200,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   35,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     240
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_MageTowerButtons	// buttons
   ,{		// sound
	 { "mage-tower-selected" }
	,{ "mage-tower-acknowledge" }
	,{ "mage-tower-ready" }
	,{ "mage-tower-help" }
	,{ "mage-tower-dead" }
   },   {	 { "mage-tower-attack" }
   } },

{   UnitTypeType, "unit-temple-of-the-damned"
    ,"Temple of the Damned"
    ,NULL, {
	"temple of the damned (summer).png"
	,"temple of the damned (winter).png"
	,DEFAULT
	,"temple of the damned (swamp).png" }
    , 96, 96			// graphic size
   ,_TempleOfTheDamnedAnimations	// animations
	,{ "icon-temple-of-the-damned" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    500,     0, {  125,  1000,    200,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   35,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     240
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_TempleOfTheDamnedButtons	// buttons
   ,{		// sound
	 { "temple-of-the-damned-selected" }
	,{ "temple-of-the-damned-acknowledge" }
	,{ "temple-of-the-damned-ready" }
	,{ "temple-of-the-damned-help" }
	,{ "temple-of-the-damned-dead" }
   },   {	 { "temple-of-the-damned-attack" }
   } },

{   UnitTypeType, "unit-human-blacksmith"
    ,"Blacksmith"
    ,NULL, {
	"human blacksmith (summer).png"
	,"human blacksmith (winter).png"
	,DEFAULT
	,"human blacksmith (swamp).png" }
    , 96, 96			// graphic size
   ,_BlacksmithHumanAnimations	// animations
	,{ "icon-human-blacksmith" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    775,     0, {  200,   800,    450,   100 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     170
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_BlacksmithHumanButtons	// buttons
   ,{		// sound
	 { "human-blacksmith-selected" }
	,{ "human-blacksmith-acknowledge" }
	,{ "human-blacksmith-ready" }
	,{ "human-blacksmith-help" }
	,{ "human-blacksmith-dead" }
   },   {	 { "human-blacksmith-attack" }
   } },

{   UnitTypeType, "unit-orc-blacksmith"
    ,"Blacksmith"
    ,NULL, {
	"orc blacksmith (summer).png"
	,"orc blacksmith (winter).png"
	,DEFAULT
	,"orc blacksmith (swamp).png" }
    , 96, 96			// graphic size
   ,_BlacksmithOrcAnimations	// animations
	,{ "icon-orc-blacksmith" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,    775,     0, {  200,   800,    450,   100 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     170
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_BlacksmithOrcButtons	// buttons
   ,{		// sound
	 { "orc-blacksmith-selected" }
	,{ "orc-blacksmith-acknowledge" }
	,{ "orc-blacksmith-ready" }
	,{ "orc-blacksmith-help" }
	,{ "orc-blacksmith-dead" }
   },   {	 { "orc-blacksmith-attack" }
   } },

{   UnitTypeType, "unit-human-refinery"
    ,"Refinery"
    ,NULL, {
	"human refinery (summer).png"
	,"human refinery (winter).png"
	,DEFAULT
	,"human refinery (swamp).png" }
    , 96, 96			// graphic size
   ,_RefineryHumanAnimations	// animations
	,{ "icon-human-refinery" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,     11,     3,    600,     0, {  225,   800,    350,   200 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   25,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     200
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    1,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    1,         0,     0,       0,     0
   ,_RefineryHumanButtons	// buttons
   ,{		// sound
	 { "human-refinery-selected" }
	,{ "human-refinery-acknowledge" }
	,{ "human-refinery-ready" }
	,{ "human-refinery-help" }
	,{ "human-refinery-dead" }
   },   {	 { "human-refinery-attack" }
   } },

{   UnitTypeType, "unit-orc-refinery"
    ,"Refinery"
    ,NULL, {
	"orc refinery (summer).png"
	,"orc refinery (winter).png"
	,DEFAULT
	,"orc refinery (swamp).png" }
    , 96, 96			// graphic size
   ,_RefineryOrcAnimations	// animations
	,{ "icon-orc-refinery" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,     12,     3,    600,     0, {  225,   800,    350,   200 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   25,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     20,      0,     200
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    1,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    1,         0,     0,       0,     0
   ,_RefineryOrcButtons	// buttons
   ,{		// sound
	 { "orc-refinery-selected" }
	,{ "orc-refinery-acknowledge" }
	,{ "orc-refinery-ready" }
	,{ "orc-refinery-help" }
	,{ "orc-refinery-dead" }
   },   {	 { "orc-refinery-attack" }
   } },

{   UnitTypeType, "unit-human-oil-platform"
    ,"Oil Platform"
    ,NULL, {
	"human oil well (summer).png"
	,"human oil well (winter).png"
	,"human oil well (wasteland).png"
	,"human oil well (swamp).png" }
    , 96, 96			// graphic size
   ,_OilPlatformHumanAnimations	// animations
	,{ "icon-human-oil-platform" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      9,     3,    650,     0, {  200,   700,    450,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   20,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   2,       0,     20,      0,     160
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     1,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_OilPlatformHumanButtons	// buttons
   ,{		// sound
	 { "human-oil-platform-selected" }
	,{ "human-oil-platform-acknowledge" }
	,{ "human-oil-platform-ready" }
	,{ "human-oil-platform-help" }
	,{ "human-oil-platform-dead" }
   },   {	 { "human-oil-platform-attack" }
   } },

{   UnitTypeType, "unit-orc-oil-platform"
    ,"Oil Platform"
    ,NULL, {
	"orc oil well (summer).png"
	,"orc oil well (winter).png"
	,"orc oil well (wasteland).png"
	,"orc oil well (swamp).png" }
    , 96, 96			// graphic size
   ,_OilPlatformOrcAnimations	// animations
	,{ "icon-orc-oil-platform" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,     10,     3,    650,     0, {  200,   700,    450,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   20,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   2,       0,     20,      0,     160
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     1,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_OilPlatformOrcButtons	// buttons
   ,{		// sound
	 { "orc-oil-platform-selected" }
	,{ "orc-oil-platform-acknowledge" }
	,{ "orc-oil-platform-ready" }
	,{ "orc-oil-platform-help" }
	,{ "orc-oil-platform-dead" }
   },   {	 { "orc-oil-platform-attack" }
   } },

{   UnitTypeType, "unit-keep"
    ,"Keep"
    ,NULL, {
	"keep (summer).png"
	,"keep (winter).png"
	,DEFAULT
	,"keep (swamp).png" }
    ,128,128			// graphic size
   ,_KeepAnimations	// animations
	,{ "icon-keep" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     6,   1400,     0, {  200,  2000,   1000,   200 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     4,    4,   127,    127,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   37,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     40,      0,     600
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      1,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_KeepButtons	// buttons
   ,{		// sound
	 { "keep-selected" }
	,{ "keep-acknowledge" }
	,{ "keep-ready" }
	,{ "keep-help" }
	,{ "keep-dead" }
   },   {	 { "keep-attack" }
   } },

{   UnitTypeType, "unit-stronghold"
    ,"Stronghold"
    ,NULL, {
	"stronghold (summer).png"
	,"stronghold (winter).png"
	,DEFAULT
	,"stronghold (swamp).png" }
    ,128,128			// graphic size
   ,_StrongholdAnimations	// animations
	,{ "icon-stronghold" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     6,   1400,     0, {  200,  2000,   1000,   200 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     4,    4,   127,    127,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   37,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     40,      0,     600
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      1,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_StrongholdButtons	// buttons
   ,{		// sound
	 { "stronghold-selected" }
	,{ "stronghold-acknowledge" }
	,{ "stronghold-ready" }
	,{ "stronghold-help" }
	,{ "stronghold-dead" }
   },   {	 { "stronghold-attack" }
   } },

{   UnitTypeType, "unit-castle"
    ,"Castle"
    ,NULL, {
	"castle (summer).png"
	,"castle (winter).png"
	,DEFAULT
	,"castle (swamp).png" }
    ,128,128			// graphic size
   ,_CastleAnimations	// animations
	,{ "icon-castle" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     9,   1600,     0, {  200,  2500,   1200,   500 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     4,    4,   127,    127,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   40,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     50,      0,    1500
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      1,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_CastleButtons	// buttons
   ,{		// sound
	 { "castle-selected" }
	,{ "castle-acknowledge" }
	,{ "castle-ready" }
	,{ "castle-help" }
	,{ "castle-dead" }
   },   {	 { "castle-attack" }
   } },

{   UnitTypeType, "unit-fortress"
    ,"Fortress"
    ,NULL, {
	"fortress (summer).png"
	,"fortress (winter).png"
	,DEFAULT
	,"fortress (swamp).png" }
    ,128,128			// graphic size
   ,_FortressAnimations	// animations
	,{ "icon-fortress" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     9,   1600,     0, {  200,  2500,   1200,   500 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     4,    4,   127,    127,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   40,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     50,      0,    1500
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      1,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_FortressButtons	// buttons
   ,{		// sound
	 { "fortress-selected" }
	,{ "fortress-acknowledge" }
	,{ "fortress-ready" }
	,{ "fortress-help" }
	,{ "fortress-dead" }
   },   {	 { "fortress-attack" }
   } },

{   UnitTypeType, "unit-gold-mine"
    ,"Gold Mine"
    ,NULL, {
	"gold mine (summer).png"
	,"gold mine (winter).png"
	,"gold mine (wasteland).png"
	,"gold mine (swamp).png" }
    , 96, 96			// graphic size
   ,_GoldMineAnimations	// animations
	,{ "icon-gold-mine" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     3,  25500,     0, {  150,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       1
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_GoldMineButtons	// buttons
   ,{		// sound
	 { "gold-mine-selected" }
	,{ "gold-mine-acknowledge" }
	,{ "gold-mine-ready" }
	,{ "gold-mine-help" }
	,{ "gold-mine-dead" }
   },   {	 { "gold-mine-attack" }
   } },

{   UnitTypeType, "unit-oil-patch"
    ,"Oil Patch"
    ,NULL, {
	"oil patch (summer).png"
	,DEFAULT
	,"oil patch (wasteland).png"
	,"oil patch (swamp).png" }
    , 96, 96			// graphic size
   ,_OilPatchAnimations	// animations
	,{ "icon-oil-patch" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      0,     0,      0,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   2,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        1,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_OilPatchButtons	// buttons
   ,{		// sound
	 { "oil-patch-selected" }
	,{ "oil-patch-acknowledge" }
	,{ "oil-patch-ready" }
	,{ "oil-patch-help" }
	,{ "oil-patch-dead" }
   },   {	 { "oil-patch-attack" }
   } },

{   UnitTypeType, "unit-human-start-location"
    ,"Start Location"
    ,NULL, {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 32, 32			// graphic size
   ,_StartLocationHumanAnimations	// animations
	,{ "icon-cancel" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      0,     0,      0,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_StartLocationHumanButtons	// buttons
   ,{		// sound
	 { "human-start-location-selected" }
	,{ "human-start-location-acknowledge" }
	,{ "human-start-location-ready" }
	,{ "human-start-location-help" }
	,{ "human-start-location-dead" }
   },   {	 { "human-start-location-attack" }
   } },

{   UnitTypeType, "unit-orc-start-location"
    ,"Start Location"
    ,NULL, {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 32, 32			// graphic size
   ,_StartLocationOrcAnimations	// animations
	,{ "icon-cancel" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      0,     0,      0,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_StartLocationOrcButtons	// buttons
   ,{		// sound
	 { "orc-start-location-selected" }
	,{ "orc-start-location-acknowledge" }
	,{ "orc-start-location-ready" }
	,{ "orc-start-location-help" }
	,{ "orc-start-location-dead" }
   },   {	 { "orc-start-location-attack" }
   } },

{   UnitTypeType, "unit-human-guard-tower"
    ,"Guard Tower"
    ,NULL, {
	"human guard tower (summer).png"
	,"human guard tower (winter).png"
	,DEFAULT
	,"human guard tower (swamp).png" }
    , 64, 64			// graphic size
   ,_GuardTowerHumanAnimations	// animations
	,{ "icon-human-guard-tower" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     9,    130,     0, {  140,   500,    150,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        6,      6,      6
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   40,       4,    12,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    15,   0,       0,     50,      0,     200
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     1,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_GuardTowerHumanButtons	// buttons
   ,{		// sound
	 { "human-guard-tower-selected" }
	,{ "human-guard-tower-acknowledge" }
	,{ "human-guard-tower-ready" }
	,{ "human-guard-tower-help" }
	,{ "human-guard-tower-dead" }
   },   {	 { "human-guard-tower-attack" }
   } },

{   UnitTypeType, "unit-orc-guard-tower"
    ,"Guard Tower"
    ,NULL, {
	"orc guard tower (summer).png"
	,"orc guard tower (winter).png"
	,DEFAULT
	,"orc guard tower (swamp).png" }
    , 64, 64			// graphic size
   ,_GuardTowerOrcAnimations	// animations
	,{ "icon-orc-guard-tower" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     9,    130,     0, {  140,   500,    150,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        6,      6,      6
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   40,       4,    12,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    15,   0,       0,     50,      0,     200
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    7,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     1,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_GuardTowerOrcButtons	// buttons
   ,{		// sound
	 { "orc-guard-tower-selected" }
	,{ "orc-guard-tower-acknowledge" }
	,{ "orc-guard-tower-ready" }
	,{ "orc-guard-tower-help" }
	,{ "orc-guard-tower-dead" }
   },   {	 { "orc-guard-tower-attack" }
   } },

{   UnitTypeType, "unit-human-cannon-tower"
    ,"Cannon Tower"
    ,NULL, {
	"human cannon tower (summer).png"
	,"human cannon tower (winter).png"
	,DEFAULT
	,"human cannon tower (swamp).png" }
    , 64, 64			// graphic size
   ,_CannonTowerHumanAnimations	// animations
	,{ "icon-human-cannon-tower" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     9,    160,     0, {  190,  1000,    300,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        7,      7,      7
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   40,      50,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    24,   0,       0,     50,      0,     250
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    3,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     1,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_CannonTowerHumanButtons	// buttons
   ,{		// sound
	 { "human-cannon-tower-selected" }
	,{ "human-cannon-tower-acknowledge" }
	,{ "human-cannon-tower-ready" }
	,{ "human-cannon-tower-help" }
	,{ "human-cannon-tower-dead" }
   },   {	 { "human-cannon-tower-attack" }
   } },

{   UnitTypeType, "unit-orc-cannon-tower"
    ,"Cannon Tower"
    ,NULL, {
	"orc cannon tower (summer).png"
	,"orc cannon tower (winter).png"
	,DEFAULT
	,"orc cannon tower (swamp).png" }
    , 64, 64			// graphic size
   ,_CannonTowerOrcAnimations	// animations
	,{ "icon-orc-cannon-tower" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     9,    160,     0, {  190,  1000,    300,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        7,      7,      7
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   40,      50,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    24,   0,       0,     50,      0,     250
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    3,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     1,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     1,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_CannonTowerOrcButtons	// buttons
   ,{		// sound
	 { "orc-cannon-tower-selected" }
	,{ "orc-cannon-tower-acknowledge" }
	,{ "orc-cannon-tower-ready" }
	,{ "orc-cannon-tower-help" }
	,{ "orc-cannon-tower-dead" }
   },   {	 { "orc-cannon-tower-attack" }
   } },

{   UnitTypeType, "unit-circle-of-power"
    ,"Circle of Power"
    ,NULL, {
	"circle of power.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 64, 64			// graphic size
   ,_CircleofPowerAnimations	// animations
	,{ "icon-circle-of-power" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      0,     0,      0,     0, {    0,     1,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_CircleofPowerButtons	// buttons
   ,{		// sound
	 { "circle-of-power-selected" }
	,{ "circle-of-power-acknowledge" }
	,{ "circle-of-power-ready" }
	,{ "circle-of-power-help" }
	,{ "circle-of-power-dead" }
   },   {	 { "circle-of-power-attack" }
   } },

{   UnitTypeType, "unit-dark-portal"
    ,"Dark Portal"
    ,NULL, {
	"dark portal (summer).png"
	,"dark portal (winter).png"
	,"dark portal (wasteland).png"
	,"dark portal (swamp).png" }
    ,128,128			// graphic size
   ,_DarkPortalAnimations	// animations
	,{ "icon-dark-portal" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     0,   5000,     0, {  100,  3000,   3000,  1000 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     4,    4,   127,    127,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_DarkPortalButtons	// buttons
   ,{		// sound
	 { "dark-portal-selected" }
	,{ "dark-portal-acknowledge" }
	,{ "dark-portal-ready" }
	,{ "dark-portal-help" }
	,{ "dark-portal-dead" }
   },   {	 { "dark-portal-attack" }
   } },

{   UnitTypeType, "unit-runestone"
    ,"Runestone"
    ,NULL, {
	"runestone (summer,wasteland).png"
	,"runestone (winter).png"
	,DEFAULT
	,"runestone (swamp).png" }
    , 64, 64			// graphic size
   ,_RunestoneAnimations	// animations
	,{ "icon-runestone" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      6,     4,   5000,     0, {  175,   900,    500,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    20,   15,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     35,      0,     150
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_RunestoneButtons	// buttons
   ,{		// sound
	 { "runestone-selected" }
	,{ "runestone-acknowledge" }
	,{ "runestone-ready" }
	,{ "runestone-help" }
	,{ "runestone-dead" }
   },   {	 { "runestone-attack" }
   } },

{   UnitTypeType, "unit-human-wall"
    ,"Wall H - not yet build"
    ,NULL, {
	"wall (summer).png"
	,"wall (winter).png"
	,"wall (wasteland).png"
	,DEFAULT }
    , 32, 32			// graphic size
   ,_WallHumanAnimations	// animations
	,{ "icon-human-wall" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,     15,     1,     40,     0, {   30,    20,     10,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     45,      0,       1
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_WallHumanButtons	// buttons
   ,{		// sound
	 { "human-wall-selected" }
	,{ "human-wall-acknowledge" }
	,{ "human-wall-ready" }
	,{ "human-wall-help" }
	,{ "human-wall-dead" }
   },   {	 { "human-wall-attack" }
   } },

{   UnitTypeType, "unit-orc-wall"
    ,"Wall O - not yet build"
    ,"unit-human-wall", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 32, 32			// graphic size
   ,_WallOrcAnimations	// animations
	,{ "icon-orc-wall" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,     15,     1,     40,     0, {   30,    20,     10,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,     45,      0,       1
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_WallOrcButtons	// buttons
   ,{		// sound
	 { "orc-wall-selected" }
	,{ "orc-wall-acknowledge" }
	,{ "orc-wall-ready" }
	,{ "orc-wall-help" }
	,{ "orc-wall-dead" }
   },   {	 { "orc-wall-attack" }
   } },

{   UnitTypeType, "unit-dead-body"
    ,"Dead Body"
    ,NULL, {
	"corpses.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_DeadBodyAnimations	// animations
	,{ "icon-peasant" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      0,     1,    255,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    71,     71,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        1,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_DeadBodyButtons	// buttons
   ,{		// sound
	 { "dead-body-selected" }
	,{ "dead-body-acknowledge" }
	,{ "dead-body-ready" }
	,{ "dead-body-help" }
	,{ "dead-body-dead" }
   },   {	 { "dead-body-attack" }
   } },

{   UnitTypeType, "unit-1x1-destroyed-place"
    ,"1x1 Destroyed Place"
    ,NULL, {
	"small destroyed site (summer).png"
	,"small destroyed site (winter).png"
	,"small destroyed site (wasteland).png"
	,"small destroyed site (swamp).png" }
    , 32, 32			// graphic size
   ,_1x1DestroyedPlaceAnimations	// animations
	,{ "icon-peasant" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      0,     2,    255,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        1,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_1x1DestroyedPlaceButtons	// buttons
   ,{		// sound
	 { "1x1-destroyed-place-selected" }
	,{ "1x1-destroyed-place-acknowledge" }
	,{ "1x1-destroyed-place-ready" }
	,{ "1x1-destroyed-place-help" }
	,{ "1x1-destroyed-place-dead" }
   },   {	 { "1x1-destroyed-place-attack" }
   } },

{   UnitTypeType, "unit-2x2-destroyed-place"
    ,"2x2 Destroyed Place"
    ,NULL, {
	"destroyed site (summer).png"
	,"destroyed site (winter).png"
	,"destroyed site (wasteland).png"
	,"destroyed site (swamp).png" }
    , 64, 64			// graphic size
   ,_2x2DestroyedPlaceAnimations	// animations
	,{ "icon-peasant" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      0,     2,    255,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     2,    2,    63,     63,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        1,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_2x2DestroyedPlaceButtons	// buttons
   ,{		// sound
	 { "2x2-destroyed-place-selected" }
	,{ "2x2-destroyed-place-acknowledge" }
	,{ "2x2-destroyed-place-ready" }
	,{ "2x2-destroyed-place-help" }
	,{ "2x2-destroyed-place-dead" }
   },   {	 { "2x2-destroyed-place-attack" }
   } },

{   UnitTypeType, "unit-3x3-destroyed-place"
    ,"3x3 Destroyed Place"
    ,"unit-2x2-destroyed-place", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 96, 96			// graphic size
   ,_3x3DestroyedPlaceAnimations	// animations
	,{ "icon-peasant" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      0,     3,    255,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     3,    3,    95,     95,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        1,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_3x3DestroyedPlaceButtons	// buttons
   ,{		// sound
	 { "3x3-destroyed-place-selected" }
	,{ "3x3-destroyed-place-acknowledge" }
	,{ "3x3-destroyed-place-ready" }
	,{ "3x3-destroyed-place-help" }
	,{ "3x3-destroyed-place-dead" }
   },   {	 { "3x3-destroyed-place-attack" }
   } },

{   UnitTypeType, "unit-4x4-destroyed-place"
    ,"4x4 Destroyed Place"
    ,"unit-2x2-destroyed-place", {
	DEFAULT
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    ,128,128			// graphic size
   ,_4x4DestroyedPlaceAnimations	// animations
	,{ "icon-peasant" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,     0,      0,     3,    255,     0, {    0,     0,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     4,    4,   127,    127,        0,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,    0,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,     0,   0,       0,      0,      0,       0
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      0,          0,      0,    1,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      0,     0,     0,      0,        1,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     0
   ,_4x4DestroyedPlaceButtons	// buttons
   ,{		// sound
	 { "4x4-destroyed-place-selected" }
	,{ "4x4-destroyed-place-acknowledge" }
	,{ "4x4-destroyed-place-ready" }
	,{ "4x4-destroyed-place-help" }
	,{ "4x4-destroyed-place-dead" }
   },   {	 { "4x4-destroyed-place-attack" }
   } },

{   UnitTypeType, "unit-peasant-with-gold"
    ,"Peasant"
    ,NULL, {
	"peasant with gold.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_PeasantWithGoldAnimations	// animations
	,{ "icon-peasant" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     30,     0, {   45,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   50,       3,     2,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      3,      30
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     1,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_PeasantWithGoldButtons	// buttons
   ,{		// sound
	 { "peasant-with-gold-selected" }
	,{ "peasant-with-gold-acknowledge" }
	,{ "peasant-with-gold-ready" }
	,{ "peasant-with-gold-help" }
	,{ "peasant-with-gold-dead" }
   },   {	 { "peasant-with-gold-attack" }
   } },

{   UnitTypeType, "unit-peon-with-gold"
    ,"Peon"
    ,NULL, {
	"peon with gold.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_PeonWithGoldAnimations	// animations
	,{ "icon-peon" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     30,     0, {   45,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   50,       3,     2,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      3,      30
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     1,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_PeonWithGoldButtons	// buttons
   ,{		// sound
	 { "peon-with-gold-selected" }
	,{ "peon-with-gold-acknowledge" }
	,{ "peon-with-gold-ready" }
	,{ "peon-with-gold-help" }
	,{ "peon-with-gold-dead" }
   },   {	 { "peon-with-gold-attack" }
   } },

{   UnitTypeType, "unit-peasant-with-wood"
    ,"Peasant"
    ,NULL, {
	"peasant with wood.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_PeasantWithWoodAnimations	// animations
	,{ "icon-peasant" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     30,     0, {   45,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   50,       3,     2,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      3,      30
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     1,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_PeasantWithWoodButtons	// buttons
   ,{		// sound
	 { "peasant-with-wood-selected" }
	,{ "peasant-with-wood-acknowledge" }
	,{ "peasant-with-wood-ready" }
	,{ "peasant-with-wood-help" }
	,{ "peasant-with-wood-dead" }
   },   {	 { "peasant-with-wood-attack" }
   } },

{   UnitTypeType, "unit-peon-with-wood"
    ,"Peon"
    ,NULL, {
	"peon with wood.png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_PeonWithWoodAnimations	// animations
	,{ "icon-peon" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     30,     0, {   45,   400,      0,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    31,     31,        1,      6,      4
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,     0,   50,       3,     2,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   0,       0,      0,      3,      30
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    1,    1,     0,      0,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     1,      0,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       1,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       1,     1
   ,_PeonWithWoodButtons	// buttons
   ,{		// sound
	 { "peon-with-wood-selected" }
	,{ "peon-with-wood-acknowledge" }
	,{ "peon-with-wood-ready" }
	,{ "peon-with-wood-help" }
	,{ "peon-with-wood-dead" }
   },   {	 { "peon-with-wood-attack" }
   } },

{   UnitTypeType, "unit-human-oil-tanker-full"
    ,"Oil tanker"
    ,NULL, {
	"human tanker (full).png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_TankerHumanFullAnimations	// animations
	,{ "icon-human-oil-tanker" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     90,     0, {   50,   400,    200,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    10,   50,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   2,       0,     10,      4,      40
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      1,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_TankerHumanFullButtons	// buttons
   ,{		// sound
	 { "human-oil-tanker-full-selected" }
	,{ "human-oil-tanker-full-acknowledge" }
	,{ "human-oil-tanker-full-ready" }
	,{ "human-oil-tanker-full-help" }
	,{ "human-oil-tanker-full-dead" }
   },   {	 { "human-oil-tanker-full-attack" }
   } },

{   UnitTypeType, "unit-orc-oil-tanker-full"
    ,"Oil tanker"
    ,NULL, {
	"orc tanker (full).png"
	,DEFAULT
	,DEFAULT
	,DEFAULT }
    , 72, 72			// graphic size
   ,_TankerOrcFullAnimations	// animations
	,{ "icon-orc-oil-tanker" }
	//Speed	OvFrame	SightR	Hitpnt	Magic	BTime	Gold	Wood	Oil
	,    10,      0,     4,     90,     0, {   50,   400,    200,     0 }
	//TileW	TileH	BoxW	BoxH	Attack	ReactC	ReactH
	,     1,    1,    63,     63,        1,      0,      0
	//Armor	Prior	Damage	Pierc	WUpgr	AUpgr
	,    10,   50,       0,     0,      0,      0
	//Weap.	Type	Decay	Annoy	Mouse	Points
	,    29,   2,       0,     10,      4,      40
	//Targ	Land	Air	Sea	Explode	Critter	Build	Submarine
	,    0,    0,     0,      1,          0,      0,    0,          0
	//SeeSu	CowerP	Tanker	Trans	GOil	SOil	Vanish	GAttack
	,     0,     0,      1,     0,     0,      0,        0,       0
	//Udead	Shore	Spell	SWood	CanAtt	Tower	OlPatch	Goldmine
	,     0,    0,      0,      0,       0,     0,        0,       0
	//Hero	SOil	Explode	CowerM	Organic	Select
	,    0,    0,         0,     0,       0,     1
   ,_TankerOrcFullButtons	// buttons
   ,{		// sound
	 { "orc-oil-tanker-full-selected" }
	,{ "orc-oil-tanker-full-acknowledge" }
	,{ "orc-oil-tanker-full-ready" }
	,{ "orc-oil-tanker-full-help" }
	,{ "orc-oil-tanker-full-dead" }
   },   {	 { "orc-oil-tanker-full-attack" }
   } },
// Warning this is generated!!
// * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING *
};

//@}
