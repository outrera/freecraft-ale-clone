/*
**	A clone of a famous game.
*/
/**@name missile.c	-	The missiles. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: missile.c,v 1.35 1999/07/08 21:09:27 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clone.h"
#include "video.h"
#include "tileset.h"
#include "map.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "missile.h"
#include "sound.h"

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

/**
**	Missile does nothing
*/
#define MissileClassNone			0
/**
**	Missile flies from x,y to x1,y1
*/
#define MissileClassPointToPoint		1
/**
**	Missile flies from x,y to x1,y1 and stays there for a moment
*/
#define MissileClassPointToPointWithDelay	2
/**
**	Missile don't move, than disappears
*/
#define MissileClassStayWithDelay		3
/**
**	Missile flies from x,y to x1,y1 than bounces three times.
*/
#define MissileClassPointToPoint3Bounces	4
/**
**	Missile flies from x,y to x1,y1 than changes into flame shield
*/
#define MissileClassFireball			5
/**
**	Missile surround x,y
*/
#define MissileClassFlameShield			6
/**
**	Missile appears at x,y, is blizzard
*/
#define MissileClassBlizzard			7
/**
**	Missile appears at x,y, is death and decay
*/
#define MissileClassDeathDecay			8
/**
**	Missile appears at x,y, is whirlwind
*/
#define MissileClassWhirlwind			9
/**
**	Missile appears at x,y, than cycle through the frames up and down.
*/
#define MissileClassCycleOnce			10
/**
**	Missile flies from x,y to x1,y1 than shows hit animation.
*/
#define MissileClassPointToPointWithHit		11

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	Missile type type definition
*/
global char MissileTypeType[] = "missile-type";

/**
**	Define missile types.
*/
global MissileType MissileTypes[] = {
{ MissileTypeType,
    "Lightning",
    "lightning.png",
    32,32,
    { NULL },
    MissileClassPointToPointWithHit,	},
{ MissileTypeType,
    "GriffonHammer",
    "gryphon hammer.png",
    32,32,
    { "fireball hit" },
    MissileClassPointToPoint3Bounces,	},
{ MissileTypeType,
    "DragonBreath",
    "dragon breath.png",
    32,32,
    { "fireball hit" },
    MissileClassPointToPoint3Bounces,	},
{ MissileTypeType,
    "Fireball",
    "fireball.png",
    32,32,
    { "fireball hit" },
    MissileClassFireball,		},
{ MissileTypeType,
    "FlameShield",
    "flame shield.png",
    32,32,
    { NULL },
    MissileClassFlameShield,		},
{ MissileTypeType,
    "Blizzard",
    "blizzard.png",
    32,32,
    { NULL },
    MissileClassBlizzard,		},
{ MissileTypeType,
    "DeathAndDecay",
    "death and decay.png",
    32,32,
    { NULL },
    MissileClassDeathDecay,		},
{ MissileTypeType,
    "BigCannon",
    "big cannon.png",
    16,16,
    { "explosion" },
    MissileClassPointToPoint,		},
{ MissileTypeType,
    "Exorcism",
    "exorcism.png",
    32,32,
    { NULL },
    MissileClassPointToPoint,		},
{ MissileTypeType,
    "HealEffect",
    "heal effect.png",
    32,32,
    { NULL },
    MissileClassPointToPoint,		},
{ MissileTypeType,
    "TouchOfDeath",
    "touch of death.png",
    32,32,
    { NULL },
    MissileClassPointToPointWithHit,	},
{ MissileTypeType,
    "Rune",
    "rune.png",
    32,32,
    { NULL },
    MissileClassStayWithDelay,		},
{ MissileTypeType,
    "Whirlwind",
    "tornado.png",
    56,56,
    { NULL },
    MissileClassWhirlwind,		},
{ MissileTypeType,
    "CatapultRock",
    "catapult rock.png",
    32,32,
    { "explosion" },
    MissileClassPointToPointWithDelay,	},
{ MissileTypeType,
    "BallistaBolt",
    "ballista bolt.png",
    64,64,
    { "explosion" },
    MissileClassPointToPoint,		},
{ MissileTypeType,
    "Arrow",
    "arrow.png",
    40,40,
    { "bow-hit" },
    MissileClassPointToPoint,		},
{ MissileTypeType,
    "Axe",
    "axe.png",
    32,32,
    { "bow-hit" },
    MissileClassPointToPoint,		},
{ MissileTypeType,
    "SubmarineMissile",
    "submarine missile.png",
    40,40,
    { "explosion" },
    MissileClassPointToPoint,		},
{ MissileTypeType,
    "TurtleMissile",
    "turtle missile.png",
    40,40,
    { "explosion" },
    MissileClassPointToPoint,		},
{ MissileTypeType,
    "SmallFire",
    "small fire.png",
    32,48,
    { NULL },
    MissileClassStayWithDelay,		},
{ MissileTypeType,
    "BigFire",
    "big fire.png",
    48,48,
    { NULL },
    MissileClassStayWithDelay,		},
{ MissileTypeType,
    "Impact",
    "ballista-catapult impact.png",
    48,48,
    { NULL },
    MissileClassStayWithDelay,		},
{ MissileTypeType,
    "NormalSpell",
    "normal spell.png",
    32,32,
    { NULL },
    MissileClassStayWithDelay,		},
{ MissileTypeType,
    "Explosion",
    "explosion.png",
    64,64,
    { NULL },
    MissileClassStayWithDelay,		},
{ MissileTypeType,
    "SmallCannon",
    "cannon.png",
    32,32,
    { "explosion" },
    MissileClassPointToPointWithDelay,	},
{ MissileTypeType,
    "CannonExplosion",
    "cannon explosion.png",
    32,32,
    { NULL },
    MissileClassStayWithDelay,		},
{ MissileTypeType,
    "CannonTowerExplosion",
    "cannon-tower explosion.png",
    32,32,
    { NULL },
    MissileClassStayWithDelay,		},
{ MissileTypeType,
    "DaemonFire",
    "daemon fire.png",
    32,32,
    { NULL },
    MissileClassPointToPoint,		},
{ MissileTypeType,
    "GreenCross",
    "green cross.png",
    32,32,
    { NULL },
    MissileClassCycleOnce,			FRAMES_PER_SECOND/30	},
{ MissileTypeType,
    "None",
    NULL,
    32,32,
    { NULL },
    MissileClassNone,			},
};

#define MAX_MISSILES	1800		/// maximum number of missiles

local int NumMissiles;			/// currently used missiles

local Missile Missiles[MAX_MISSILES];	/// all missiles on map

/*============================================================================
==	MISSILE FUNCTIONS
============================================================================*/

/**
**	Load the graphics for the missiles.
*/
global void LoadMissileSprites(void)
{
    int i;
    const char* file;

    for( i=0; i<sizeof(MissileTypes)/sizeof(*MissileTypes); ++i ) {
	if( (file=MissileTypes[i].File) ) {
	    char* buf;

	    buf=alloca(strlen(file)+9+1);
	    file=strcat(strcpy(buf,"graphic/"),file);
	    ShowLoadProgress("\tMissile %s\n",file);
	    MissileTypes[i].RleSprite=LoadRleSprite(
		    file,MissileTypes[i].Width,MissileTypes[i].Height);
	}
	MissileTypes[i].Type=i;
    }
}

/**
**	Create a new missile at (x,y).
*/
global Missile* MakeMissile(int missile_type,int sx,int sy,int dx,int dy)
{
    int missile;

    DebugLevel3("%s: type %d(%s) at %d,%d to %d,%d\n",__FUNCTION__
	    ,missile_type,MissileTypes[missile_type].Name
	    ,sx,sy,dx,dy);

    if( missile_type>sizeof(MissileTypes)/sizeof(*MissileTypes) ) {
	DebugLevel0("%s: INTERNAL: Illegal type\n",__FUNCTION__);
	return NULL;
    }

    //
    //	Find free slot, FIXME: see MakeUnit for better code
    //
    for( missile=0; missile<NumMissiles; ++missile ) {
	if( Missiles[missile].Type==MissileFree ) {
	    goto found;
	}
    }

    //	Check maximum missiles!
    if( NumMissiles==MAX_MISSILES ) {
	printf("Maximum of missiles reached\n");
	abort();
	return NULL;
    }

    missile=NumMissiles++;

found:
    Missiles[missile].X=sx;
    Missiles[missile].Y=sy;
    Missiles[missile].DX=dx;
    Missiles[missile].DY=dy;
    Missiles[missile].Type=&MissileTypes[missile_type];
    Missiles[missile].Frame=0;
    Missiles[missile].State=0;
    Missiles[missile].Wait=1;

    Missiles[missile].SourceUnit=NULL;
    Missiles[missile].SourceType=NULL;
    Missiles[missile].SourceStats=NULL;
    Missiles[missile].SourcePlayer=NULL;

    return &Missiles[missile];
}

/**
**	Calculate damage.
**
**	Damage calculation:
**		(BasicDamage-Armor)+PiercingDamage
**	damage =----------------------------------
**				    2
**	damage is multiplied by random 1 or 2.
*/
local int CalculateDamage(UnitStats* attacker_stats,Unit* goal)
{
    UnitStats* stats;
    int damage;

    stats=goal->Stats;
    damage=-stats->Armor;
    damage+=attacker_stats->BasicDamage;
    if( damage<0 ) {
	damage=0;
    }
    damage+=attacker_stats->PiercingDamage+1;	// round up
    damage/=2;
    damage*=((SyncRand()>>15)&1)+1;
    DebugLevel3("Damage done %d\n",damage);

    return damage;
}

/**
**	Fire missile.
*/
global void FireMissile(Unit* unit)
{
    int x;
    int y;
    int dx;
    int dy;
    Unit* goal;
    Missile* missile;

    DebugLevel3("%s:\n",__FUNCTION__);

    if( unit->Type->MissileWeapon==MissileNone ) {
	// FIXME: must hit now!!!
	if( !(goal=unit->Command.Data.Move.Goal) ) {
	    DebugLevel1("Missile-none hits no unit, shouldn't happen!\n");
	    return;
	}

	// FIXME: make sure thats the correct unit.
	if( !goal->HP || goal->Command.Action==UnitActionDie ) {
	    return;
	}

	HitUnit(goal,CalculateDamage(unit->Stats,goal));

	return;
    }

    x=unit->X*TileSizeX+TileSizeX/2;
    y=unit->Y*TileSizeY+TileSizeY/2;
    if( (goal=unit->Command.Data.Move.Goal) ) {
	// Fire to nearest point of unit!
	if( goal->Type ) {
	    NearestOfUnit(goal,unit->X,unit->Y,&dx,&dy);
	} else {
	    // FIXME: unit killed?
	    dx=goal->X;
	    dy=goal->Y;
	}
	DebugLevel3("Fire to unit at %d,%d\n",dx,dy);
	dx=dx*TileSizeX+TileSizeX/2;
	dy=dy*TileSizeY+TileSizeY/2;
    } else {
	dx=unit->Command.Data.Move.DX*TileSizeX+TileSizeX/2;
	dy=unit->Command.Data.Move.DY*TileSizeY+TileSizeY/2;
    }

    missile=MakeMissile(unit->Type->MissileWeapon,x,y,dx,dy);
    //
    //	Damage of missile
    //
    // missile->Damage=CalculateDamage(unit,goal);
    missile->SourceUnit=unit;
    missile->SourceType=unit->Type;
    missile->SourceStats=unit->Stats;
    missile->SourcePlayer=unit->Player;
}

/**
**	Draw missile.
*/
global void DrawMissile(MissileType* missile_type,int frame,int x,int y)
{
    // FIXME: remove this here, move to higher functions
    x-=missile_type->Width/2;
    y-=missile_type->Height/2;

    // FIXME: This is a hack for mirrored sprites
    if( frame&128 ) {
	DrawRleSpriteClippedX(missile_type->RleSprite,frame&127,x,y);
    } else {
	DrawRleSpriteClipped(missile_type->RleSprite,frame,x,y);
    }
}

/**
**	Draw all missiles on map.
*/
global void DrawMissiles(void)
{
    int missile;
    int x;
    int y;

    for( missile=0; missile<NumMissiles; ++missile ) {
	if( Missiles[missile].Type==MissileFree ) {
	    continue;
	}
	// FIXME: is this correct???
	x=Missiles[missile].X-MapX*TileSizeX+MAP_X;
	y=Missiles[missile].Y-MapY*TileSizeY+MAP_Y;
	if( x<MAP_X || x>MAP_X+TileSizeX*MapWidth
		|| y<MAP_Y || y>MAP_Y+TileSizeY*MapHeight ) {
	    continue;
	}
	DrawMissile(Missiles[missile].Type,Missiles[missile].Frame,x,y);
    }
}

/**
**	Change missile heading from x,y.
*/
local void MissileNewHeadingFromXY(int missile,int x,int y)
{
    int heading;
    int frame;

    // Set new heading:
    if( x==0 ) {
	if( y<0 ) {
	    heading=HeadingN;
	} else {
	    heading=HeadingS;
	}
	frame=heading;
    } else if( x>0 ) {
	if( y<0 ) {
	    heading=HeadingNE;
	} else if( y==0 ) {
	    heading=HeadingE;
	} else {
	    heading=HeadingSE;
	}
	frame=heading;
    } else {
	if( y<0 ) {
	    heading=HeadingNW;
	} else if( y==0 ) {
	    heading=HeadingW;
	} else {
	    heading=HeadingSW;
	}
	frame=128+1+HeadingNW-heading;
    }
    Missiles[missile].Frame=frame;
}

#define MISSILE_STEPS	16		// How much did a missile move??

/**
**	Handle point to point missile.
*/
local int PointToPointMissile(int missile)
{
    int dx;
    int dy;
    int xstep;
    int ystep;
    int i;

    if( !(Missiles[missile].State&1) ) {
	// initialize
	dy=Missiles[missile].DY-Missiles[missile].Y;
	ystep=1;
	if( dy<0 ) {
	    dy=-dy;
	    ystep=-1;
	}
	dx=Missiles[missile].DX-Missiles[missile].X;
	xstep=1;
	if( dx<0 ) {
	    dx=-dx;
	    xstep=-1;
	}

	// FIXME: could be better written
	MissileNewHeadingFromXY(missile,dx*xstep,dy*ystep);

	if( dy==0 ) {		// horizontal line
	    if( dx==0 ) {
		return 1;
	    }
	} else if( dx==0 ) {	// vertical line
	} else if( dx<dy ) {	// step in vertical direction
	    Missiles[missile].D=dy-1;
	    dx+=dx;
	    dy+=dy;
        } else if( dx>dy ) {	// step in horizontal direction
	    Missiles[missile].D=dx-1;
	    dx+=dx;
	    dy+=dy;
	}

	Missiles[missile].Dx=dx;
	Missiles[missile].Dy=dy;
	Missiles[missile].Xstep=xstep;
	Missiles[missile].Ystep=ystep;
	++Missiles[missile].State;
	DebugLevel3("Init: %d,%d, %d,%d, =%d\n"
		,dx,dy,xstep,ystep,Missiles[missile].D);
	return 0;
    } else {
	dx=Missiles[missile].Dx;
	dy=Missiles[missile].Dy;
	xstep=Missiles[missile].Xstep;
	ystep=Missiles[missile].Ystep;
    }


    //
    //	Move missile
    //
    if( dy==0 ) {		// horizontal line
	for( i=0; i<MISSILE_STEPS; ++i ) {
	    if( Missiles[missile].X==Missiles[missile].DX ) {
		return 1;
	    }
	    Missiles[missile].X+=xstep;
	}
	return 0;
    }

    if( dx==0 ) {		// vertical line
	for( i=0; i<MISSILE_STEPS; ++i ) {
	    if( Missiles[missile].Y==Missiles[missile].DY ) {
		return 1;
	    }
	    Missiles[missile].Y+=ystep;
	}
	return 0;
    }

    if( dx<dy ) {		// step in vertical direction
	for( i=0; i<MISSILE_STEPS; ++i ) {
	    if( Missiles[missile].Y==Missiles[missile].DY ) {
		return 1;
	    }
	    Missiles[missile].Y+=ystep;
	    Missiles[missile].D-=dx;
	    if( Missiles[missile].D<0 ) {
		Missiles[missile].D+=dy;
		Missiles[missile].X+=xstep;
	    }
	}
	return 0;
    }

    if( dx>dy ) {		// step in horizontal direction
	for( i=0; i<MISSILE_STEPS; ++i ) {
	    if( Missiles[missile].X==Missiles[missile].DX ) {
		return 1;
	    }
	    Missiles[missile].X+=xstep;
	    Missiles[missile].D-=dy;
	    if( Missiles[missile].D<0 ) {
		Missiles[missile].D+=dx;
		Missiles[missile].Y+=ystep;
	    }
	}
	return 0;
    }
				// diagonal line
    for( i=0; i<MISSILE_STEPS; ++i ) {
	if( Missiles[missile].Y==Missiles[missile].DY ) {
	    return 1;
	}
	Missiles[missile].X+=xstep;
	Missiles[missile].Y+=ystep;
    }
    return 0;
}

/**
**	Work for missile hit.
*/
global void MissileHit(int missile)
{
    Unit* goal;

    // FIXME: should I move the PlayMissileSound to here?
    // FIXME: And should the the connected missile be defined in the missile
    // FIXME: structure

    switch( Missiles[missile].Type->Type ) {
	case MissileArrow:
	case MissileAxe:
	    PlayMissileSound(Missiles+missile,
			     Missiles[missile].Type->ImpactSound.Sound);
	    break;
	case MissileBallistaBolt:
	case MissileBigCannon:
	    PlayMissileSound(Missiles+missile,
			     Missiles[missile].Type->ImpactSound.Sound);
	    MakeMissile(MissileImpact
		,Missiles[missile].X
		,Missiles[missile].Y
		,0,0);
	    break;

	case MissileSubmarineMissile:
	case MissileTurtleMissile:
	    PlayMissileSound(Missiles+missile,
			     Missiles[missile].Type->ImpactSound.Sound);
	    MakeMissile(MissileImpact
		,Missiles[missile].X
		,Missiles[missile].Y
		,0,0);
	    break;

	case MissileGreenCross:
	    break;
    }

    if( !Missiles[missile].SourceType ) {
	return;
    }

    // FIXME: must choose better goal!
    // FIXME: what can the missile hit?
    goal=UnitOnMapTile(Missiles[missile].X/TileSizeX
		,Missiles[missile].Y/TileSizeY);
    if( !goal || !goal->HP ) {
	return;
    }

    HitUnit(goal,CalculateDamage(Missiles[missile].SourceStats,goal));
}

/**
 ** Check missile visibility.
 */
//FIXME: is this correct?
//FIXME: should be exported?
local int MissileVisible(int missile)
{
    int tileMinX,tileMaxX;
    int tileMinY,tileMaxY;

    tileMinX=Missiles[missile].X/TileSizeX;
    tileMinY=Missiles[missile].Y/TileSizeY;
    tileMaxX=(Missiles[missile].X+Missiles[missile].Type->Width)/TileSizeX;
    tileMaxY=(Missiles[missile].Y+Missiles[missile].Type->Height)/TileSizeY;
    if ( (tileMinX>(MapX+MapWidth)) || (tileMaxX<MapX)
	 || (tileMinY>MapY+MapHeight) || (tileMaxY<MapY)) {
	return 0;
    }
    DebugLevel3("Missile bounding box %d %d %d %d (Map %d %d %d %d)\n",
		tileMinX,tileMaxX,tileMinY,tileMaxY,
		MapX,MapX+MapWidth,MapY,MapY+MapHeight);
    return 1;
}

/**
**	Handle all missile actions.
*/
//FIXME: (Fabrice) I don't know if my update for missile visibility is fully
//correct.
global void MissileActions(void)
{
    int missile;

    for( missile=0; missile<NumMissiles; ++missile ) {
	if( Missiles[missile].Type==MissileFree ) {
	    continue;
	}
	if( Missiles[missile].Wait-- ) {
	    continue;
	}
	if (MissileVisible(missile)) {
	    // check before movement
	    MustRedraw|=RedrawMap;
	}
	switch( Missiles[missile].Type->Class ) {

	    case MissileClassPointToPoint:
		Missiles[missile].Wait=1;
		if( PointToPointMissile(missile) ) {
		    MissileHit(missile);
		    Missiles[missile].Type=MissileFree;
		} else {
		    //
		    //	Animate missile, cycle through frames
		    //
		    Missiles[missile].Frame+=5;
		    if( (Missiles[missile].Frame&127)
			    >=Missiles[missile].Type->RleSprite->NumFrames ) {
			Missiles[missile].Frame=
			    // (Missiles[missile].Frame&128)|
				(Missiles[missile].Frame
				    -Missiles[missile].Type->RleSprite
					->NumFrames);
		    }
		    DebugLevel3("Frame %d of %d\n"
			,Missiles[missile].Frame
			,Missiles[missile].Type->RleSprite->NumFrames);

		}
		break;

	    case MissileClassPointToPointWithDelay:
		Missiles[missile].Wait=1;
		if( PointToPointMissile(missile) ) {
		    switch( Missiles[missile].State++ ) {
			case 1:
			    // FIXME: bounces up.
			    PlayMissileSound(Missiles+missile,
				     Missiles[missile].Type->ImpactSound.Sound);
			    // FIXME: make this configurable!!
			    switch( Missiles[missile].Type->Type ) {
				case MissileSmallCannon:
				    MakeMissile(MissileCannonExplosion
					,Missiles[missile].X
					,Missiles[missile].Y
					,0,0);
				    break;
				case MissileBigCannon:
				    MakeMissile(MissileCannonTowerExplosion
					,Missiles[missile].X
					,Missiles[missile].Y
					,0,0);
				    break;
				case MissileCatapultRock:
				case MissileBallistaBolt:
				    MakeMissile(MissileImpact
					,Missiles[missile].X
					,Missiles[missile].Y
					,0,0);
				    break;
			    }
			    break;
			default:
			    MissileHit(missile);
			    Missiles[missile].Type=MissileFree;
			    break;
		    }
		} else {
		    //
		    //	Animate missile, depends on the way.
		    //
		    // FIXME: how?
		}
		break;

	    case MissileClassPointToPoint3Bounces:
		Missiles[missile].Wait=1;
		if( PointToPointMissile(missile) ) {
		    //
		    //	3 Bounces.
		    //
		    switch( Missiles[missile].State ) {
			case 1:
			case 3:
			case 5:
			    Missiles[missile].State+=2;
			    Missiles[missile].DX+=
				Missiles[missile].Xstep*TileSizeX*2;
			    Missiles[missile].DY+=
				Missiles[missile].Ystep*TileSizeY*2;
			    PlayMissileSound(Missiles+missile,
				    Missiles[missile].Type->ImpactSound.Sound);
			    MakeMissile(MissileExplosion
				,Missiles[missile].X
				,Missiles[missile].Y
				,0,0);
			    MissileHit(missile);
			    // FIXME: hits to left and right
			    // FIXME: reduce damage
			    break;
			default:
			    Missiles[missile].Type=MissileFree;
			    break;
		    }
		} else {
		    //
		    //	Animate missile, cycle through frames
		    //
		    Missiles[missile].Frame+=5;
		    if( (Missiles[missile].Frame&127)
			    >=Missiles[missile].Type->RleSprite->NumFrames ) {
			Missiles[missile].Frame=
			    // (Missiles[missile].Frame&128)|
				(Missiles[missile].Frame
				    -Missiles[missile].Type->RleSprite
					->NumFrames);
		    }
		    DebugLevel3("Frame %d of %d\n"
			,Missiles[missile].Frame
			,Missiles[missile].Type->RleSprite->NumFrames);

		}
		break;

	    case MissileClassPointToPointWithHit:
		Missiles[missile].Wait=1;
		if( PointToPointMissile(missile) ) {
		    //
		    //	Animate hit
		    //
		    Missiles[missile].Frame+=5;
		    if( (Missiles[missile].Frame&127)
			    >=Missiles[missile].Type->RleSprite->NumFrames ) {
			MissileHit(missile);
			Missiles[missile].Type=MissileFree;
		    }
		}
		break;

	    case MissileClassStayWithDelay:
		Missiles[missile].Wait=1;
		if( ++Missiles[missile].Frame
			==Missiles[missile].Type->RleSprite
			    ->NumFrames ) {
		    MissileHit(missile);
		    Missiles[missile].Type=MissileFree;
		}
		break;

	    case MissileClassCycleOnce:
		Missiles[missile].Wait=Missiles[missile].Type->Speed;
		switch( Missiles[missile].State ) {
		    case 0:
		    case 2:
			++Missiles[missile].State;
			break;
		    case 1:
			if( ++Missiles[missile].Frame
				==Missiles[missile].Type->RleSprite
				    ->NumFrames ) {
			    --Missiles[missile].Frame;
			    ++Missiles[missile].State;
			}
			break;
		    case 3:
			if( !Missiles[missile].Frame-- ) {
			    MissileHit(missile);
			    Missiles[missile].Type=MissileFree;
			}
			break;
		}
		break;
	}
	if (Missiles[missile].Type!=MissileFree && MissileVisible(missile)) {
	    // check after movement
	    MustRedraw|=RedrawMap;
	}
    }
}

/**
**	Calculate distance from view-point to missle.
*/
global int ViewPointDistanceToMissile(Missile* dest)
{
    int x;
    int y;

    //FIXME: is it the correct formula?
    x=dest->X/TileSizeX;
    y=dest->Y/TileSizeY;
    DebugLevel3("Missile %p at %d %d\n",dest,x,y);
    return ViewPointDistance(x,y);
}

//@}
