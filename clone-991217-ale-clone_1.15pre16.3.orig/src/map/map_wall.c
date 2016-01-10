/*
**	A clone of a famous game.
*/
/**@name map_wall.c	-	The map wall handling. */
/*
**
**	(c) Copyright 1999 by Vladi
**
**	$Id: map_wall.c,v 1.3 1999/06/21 21:30:08 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>

#include "clone.h"
#include "map.h"
#include "minimap.h"

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

#define MAPHWALL100  16
#define MAPOWALL100  34
#define MAPHWALL50   52
#define MAPOWALL50   70
#define MAPNOWALL    88

#define WALL_100HP   40 // FIXME: Vladi: should be fixed!!!
#define WALL_50HP    20 // FIXME: Vladi: should be fixed!!!

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

local int WallTable[16] = {
//   0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  A,  B,  C,  D,  E,  F
     0,  4,  2,  7,  1,  5,  3,  8,  9, 14, 11, 16, 10, 15, 13, 17
};

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Fix walls (connections)
----------------------------------------------------------------------------*/

/*
  Vladi:
  NOTE: this is not the original behaviour of the wall demolishing,
  instead I'm replacing tiles just as the wood fixing, so if part of
  a wall is down side neighbours are fixed just as current tile is
  empty one ( current is first NoWall tile ). It is still nice... :)

  For the connecting new walls -- all's fine.
*/

global int MapWallChk(int x,int y,int walltype) // used by FixWall, walltype==-1 for auto
{
  if( !INMAP(x,y) ) return 1; // outside considered wall
  // return !!(MAPFIELD(x,y).Flags & MapFieldForest);
  if (walltype == -1)
    return (TILETYPE(MAPSEENTILE(x,y)) == TileTypeHWall ||
            TILETYPE(MAPSEENTILE(x,y)) == TileTypeOWall );
  else
    return (TILETYPE(MAPSEENTILE(x,y)) == walltype);
};

local int FixWall(int x,int y) // used by MapRemoveWall and PreprocessMap
{
  int tile;
  int walltype;

  if ( !INMAP(x,y) ) return 0;
  walltype = TILETYPE(MAPSEENTILE(x,y));
  if ( walltype != TileTypeHWall && walltype != TileTypeOWall ) return 0;

  #define WALL(xx,yy) (MapWallChk(xx,yy,walltype) != 0)
  tile = 0;
  if (WALL(x  ,y-1)) tile |= 1<<0;
  if (WALL(x+1,y  )) tile |= 1<<1;
  if (WALL(x  ,y+1)) tile |= 1<<2;
  if (WALL(x-1,y  )) tile |= 1<<3;

  tile = WallTable[tile];

  if (walltype == TileTypeHWall)
     {
     if (MAPFIELD(x,y).Value < WALL_50HP)
        tile += MAPHWALL50;
      else
        tile += MAPHWALL100;
     }
   else
     {
     if (MAPFIELD(x,y).Value < WALL_50HP)
        tile += MAPOWALL50;
      else
        tile += MAPOWALL100;
     }

// FIXME: Johns, Is this correct? Could this function be called under fog of war
  if (MAPFIELD(x,y).SeenTile == tile) return 0;
  MAPFIELD(x,y).SeenTile =  tile;

  UpdateMinimapXY(x,y);
  return 1;
};

// this one should be called and from the HitUnit() or similar func,
// when the HP(Value) goes below 50%
global void MapFixWall(int x,int y)
{
    // side neighbors
    FixWall( x+1, y   );
    FixWall( x-1, y   );
    FixWall( x  , y+1 );
    FixWall( x  , y-1 );
};

global void MapRemoveWall(int x,int y)
{
    MAPFIELD(x,y).Tile = MAPNOWALL;
    MAPFIELD(x,y).Flags &= ~MapFieldWall;
    MAPFIELD(x,y).Flags &= ~MapFieldUnpassable;
#ifdef MOVE_MAP
    MAPMOVE(x,y) &= ~MapMoveUnpassable;
#endif
    MAPFIELD(x,y).Value = 0;
    UpdateMinimapXY(x,y);
//    MustRedraw|=RedrawMinimap;
};

global void MapSetWall(int x,int y,int humanwall)
{

    MAPFIELD(x,y).Tile   = humanwall?MAPHWALL100:MAPOWALL100;
    MAPFIELD(x,y).Flags |= humanwall?MapFieldHuman:0;
    MAPFIELD(x,y).Flags |= MapFieldWall;
    MAPFIELD(x,y).Flags |= MapFieldUnpassable;
#ifdef MOVE_MAP
    MAPMOVE(x,y) |= MapMoveUnpassable;
#endif
    MAPFIELD(x,y).Value = 40; // FIXME: Vladi: this HP should be talen from the unit type stats!!!
    UpdateMinimapXY(x,y);
//    MustRedraw|=RedrawMinimap;
//    DebugLevel0("Wall %d,%d=%d\n",x,y,MAPFIELD(x,y).Tile);

    MapMarkSeenTile(x,y);
};

//@}
