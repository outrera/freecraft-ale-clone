/*
**	A clone of a famous game.
*/
/**@name upgrade_structs.h	-	The upgrade/allow headerfile. */
/*
**	(c) Copyright 1999 by Vladi Belperchinov-Shabanski
**
**	$Id: upgrade_structs.h,v 1.2 1999/11/21 21:59:53 root Exp $
*/

#ifndef __UPGRADE_STRUCTS_H__
#define __UPGRADE_STRUCTS_H__

//@{

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

/**
**	Indices into costs/resource array. (A litte future here :)
*/
enum _costs_ {
    TimeCost,				/// time in frames
    GoldCost,				/// gold resource
    WoodCost,				/// wood resource
    OilCost,				/// oil resource
    OgreCost,				/// ogre resource

    MaxCosts				/// how many different costs
};

/**
**	This are the current stats of an unit. Upgraded or downgraded.
*/
typedef struct _unit_stats_ {
    int		AttackRange;		/// how far can the unit attack
    int		SightRange;		/// how far can the unit see
    int		Armor;			/// armor strength
    int		BasicDamage;		/// weapon basic damage
    int		PiercingDamage;		/// weapon piercing damage
    int		Speed;			/// movement speed
    int		HitPoints;		/// hit points
    int		Costs[MaxCosts];	/// Current costs of the unit.
    int		Level;			/// unit level (upgrades)
} UnitStats;










//Cleaning this
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

/*----------------------------------------------------------------------------
--	upgrade id's
----------------------------------------------------------------------------*/

#if 0
//       NOTE: this id's are used for the current ( 07.jun.99 )
//             buttons code, perhaps will be removed later
//             You can get upgrade id with
//             int us2 = UpgradeIdByIdent( "UpgradeShield2" )
// well.. on second thought -- it can stay... :)
#define UpgradeSword1			0x00
#define UpgradeSword2			0x01
#define UpgradeAxe1			0x02
#define UpgradeAxe2			0x03
#define UpgradeArrow1			0x04
#define UpgradeArrow2			0x05
#define UpgradeThrowingAxe1		0x06
#define UpgradeThrowingAxe2		0x07
#define UpgradeHumanShield1		0x08
#define UpgradeHumanShield2		0x09
#define UpgradeOrcShield1		0x0A
#define UpgradeOrcShield2		0x0B
#define UpgradeHumanShipCannon1		0x0C
#define UpgradeHumanShipCannon2		0x0D
#define UpgradeOrcShipCannon1		0x0E
#define UpgradeOrcShipCannon2		0x0F
#define UpgradeHumanShipArmor1		0x10
#define UpgradeHumanShipArmor2		0x11
#define UpgradeOrcShipArmor1		0x12
#define UpgradeOrcShipArmor2		0x13
#define UpgradeCatapult1		0x14
#define UpgradeCatapult2		0x15
#define UpgradeBallista1		0x16
#define UpgradeBallista2		0x17
#define UpgradeTrainRangers		0x18
#define UpgradeLongBow			0x19
#define UpgradeRangerScouting		0x1A
#define UpgradeRangerMarksmanship	0x1B
#define UpgradeTrainBerserkers		0x1C
#define UpgradeLighterAxes		0x1D
#define UpgradeBerserkerScouting	0x1E
#define UpgradeBerserkerRegeneration	0x1F
#define UpgradeTrainOgreMages		0x20
#define UpgradeTrainPaladins		0x21
#define UpgradeHolyVision		0x22
#define UpgradeHealing			0x23
#define UpgradeExorcism			0x24
#define UpgradeFlameShield		0x25
#define UpgradeFireball			0x26
#define UpgradeSlow			0x27
#define UpgradeInvisibility		0x28
#define UpgradePolymorph		0x29
#define UpgradeBlizzard			0x2A
#define UpgradeEyeOfKilrogg		0x2B
#define UpgradeBloodlust		0x2C
#define UpgradeRaiseDead		0x2D
#define UpgradeDeathCoil		0x2E
#define UpgradeWhirlwind		0x2F
#define UpgradeHaste			0x30
#define UpgradeUnholyArmor		0x31
#define UpgradeRunes			0x32
#define UpgradeDeathAndDecay		0x33
#endif

/*----------------------------------------------------------------------------
--	upgrades and modifiers
----------------------------------------------------------------------------*/

// max upgrade/allow item/types count
#define MAXUACOUNT	256

// should keep with the same count ( even in reserve ) just to keep
// compatibility if new units added.
#define MAXUNITTYPES	MAXUACOUNT
#define MAXUPGRADES	MAXUACOUNT
#define MAXACTIONS	MAXUACOUNT // include spells

typedef struct _modifiers_
{
  int level; // unit level (upgrades) used not in the same context -- ignore

  // all the following are modifiers not values!
  int attack_range;
  int sight_range;
  int basic_damage;
  int piercing_damage;
  int armor;
  int speed;
  int hit_points;

  int Costs[MaxCosts];
} Modifiers;

typedef struct _upgrade_modifier_
{

  int uid; // used to filter required by upgrade modifiers

  Modifiers mods;

  // allow/forbid bitmaps -- used as chars for example:
  // `?' -- leave as is, `F' -- forbid, `A' -- allow
  char af_units[MAXUNITTYPES];   // allow/forbid units
  char af_actions[MAXACTIONS]; // allow/forbid actions
  char af_upgrades[MAXUPGRADES]; // allow/forbid upgrades
  char apply_to[MAXUNITTYPES]; // which unit types are affected

} UpgradeModifier;

typedef struct _upgrade_
{

  char	Ident[32];			/// identifier

  // int Uid; // used to map modifiers required

  int Costs[MaxCosts];			/// costs for the upgrade

  int Icon; // perhaps will be removed later

} Upgrade;

typedef struct _upgrade_timers_
{

  // all 0 at the beginning, all upgrade actions do increment values in
  // this struct, every player has own UpgradeTimers struct
  int upgrades[MAXUPGRADES];

} UpgradeTimers;

/*----------------------------------------------------------------------------
--	allow
----------------------------------------------------------------------------*/

typedef struct _allow_
{

  // values are `A' -- allowed, `F' -- forbidden, `R' -- acquired, perhaps
  // other value: `Q' -- acquired but forbidden (does it make sense?:))
  char units[MAXUNITTYPES];
  char actions[MAXACTIONS];
  char upgrades[MAXUPGRADES];
  // every player has own Allow struct

} Allow;

/*----------------------------------------------------------------------------
--	structs
----------------------------------------------------------------------------*/

extern Upgrade Upgrades[MAXUACOUNT];

//@}

#endif // !__UPGRADE_STRUCTS_H__
