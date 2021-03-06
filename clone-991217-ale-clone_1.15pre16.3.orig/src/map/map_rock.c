/*
**	A clone of a famous game.
*/
/**@name map_rock.c	-	The map rock handling. */
/*
**
**	(c) Copyright 1999 by Vladi
**
**	$Id: map_rock.c,v 1.1 1999/05/29 21:32:48 root Exp $
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

 // -1 is hack should be fixed later
#define FIRST_ROCK_TILE  (TheMap.Tileset->FirstRockTile-1)

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

local const int RockTable[16] = {
//  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  A,  B,  C,  D,  E,  F
   -1, 22, -1,  1, 20, 21,  3,  2, -1,  9, -1, 23,  6,  8,  5, 36
};

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

global void MapRemoveRock(int x,int y);

local int RockChk(int x,int y) // used by FixRock and PreprocessMap
{
  if( !INMAP(x,y) ) return 1; // outside considered rock
  return (TILETYPE(MAPSEENTILE(x,y)) == TileTypeRock);
};

local int FixRock(int x,int y) // used by MapRemoveRock and PreprocessMap
{
  int tile = 0;

  if ( !INMAP(x,y) ) return 0;
  if ( RockChk(x,y) == 0 ) return 0;

  #define ROCK(xx,yy) (RockChk(xx,yy) != 0)
  if (ROCK(x  ,y-1)) tile |= 1<<0;
  if (ROCK(x+1,y  )) tile |= 1<<1;
  if (ROCK(x  ,y+1)) tile |= 1<<2;
  if (ROCK(x-1,y  )) tile |= 1<<3;

  tile = RockTable[tile];
  if (tile == -1)
    MapRemoveRock(x,y);
  else
    {
    if (tile == RockTable[15])
      {
      // Vladi: still to filter tiles w. corner empties -- note: the original
      // tiles and order are not perfect either. It's a hack but is enough and
      // looks almost fine.
      if (RockChk(x+1,y-1) == 0) tile =  7; else
      if (RockChk(x+1,y+1) == 0) tile = 10; else
      if (RockChk(x-1,y+1) == 0) tile = 11; else
      if (RockChk(x-1,y-1) == 0) tile =  4; else
                                 tile = RockTable[15]; // not required really
      };

    tile += FIRST_ROCK_TILE;
    if (MAPFIELD(x,y).SeenTile == tile) return 0;
    MAPFIELD(x,y).SeenTile =  tile;
    }
  UpdateMinimapXY(x,y);
  return 1;
}

global void MapFixRock(int x,int y)
{
    // side neighbors
    FixRock( x+1, y   );
    FixRock( x-1, y   );
    FixRock( x  , y+1 );
    FixRock( x  , y-1 );
}

global void MapRemoveRock(int x,int y)
{
    MAPFIELD(x,y).Tile=TheMap.Tileset->NoRockTile;
    MAPFIELD(x,y).Flags &= ~MapFieldRocks;
    MAPFIELD(x,y).Flags &= ~MapFieldUnpassable;
#ifdef MOVE_MAP
    MAPMOVE(x,y) &= ~MapMoveUnpassable;
#endif
    UpdateMinimapXY(x,y);
    MustRedraw|=RedrawMaps;
}

//@}
