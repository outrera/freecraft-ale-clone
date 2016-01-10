/*
**	A clone of a famous game.
*/
/**@name map_wood.c	-	The map wood handling. */
/*
**
**	(c) Copyright 1999 by Vladi
**
**	$Id: map_wood.c,v 1.2 1999/06/21 21:30:08 root Exp $
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

#define FIRST_WOOD_TILE  0x65 // first wood tile is +1
#define  LAST_WOOD_TILE  0x65 // well, -1 for the nowoodtile

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

local int WoodTable[16] = {
//  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  A,  B,  C,  D,  E,  F
   -1, 22, -1,  1, 20, 21,  3,  2, -1,  9, -1, 23,  6,  8,  5,  4
};

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

global int MapWoodChk(int x,int y) // used by FixWood
{
  if( !INMAP(x,y) ) return 1; // outside considered wood
  // return !!(MAPFIELD(x,y).Flags & MapFieldForest);
  return (TILETYPE(MAPSEENTILE(x,y)) == TileTypeWood);
};

global int FixWood(int x,int y) // used by MapRemoveWood2 and PreprocessMap
{
  int tile = 0;

  if ( !INMAP(x,y) ) return 0;
  if ( MapWoodChk(x,y) == 0 ) return 0;

  #define WOOD(xx,yy) (MapWoodChk(xx,yy) != 0)
  if (WOOD(x  ,y-1)) tile |= 1<<0;
  if (WOOD(x+1,y  )) tile |= 1<<1;
  if (WOOD(x  ,y+1)) tile |= 1<<2;
  if (WOOD(x-1,y  )) tile |= 1<<3;

  tile = WoodTable[tile];
  if (tile == -1)
    MapRemoveWood(x,y);
  else
    {
    tile += FIRST_WOOD_TILE;
    if (MAPFIELD(x,y).SeenTile == tile) return 0;
    MAPFIELD(x,y).SeenTile =  tile;
    }
  UpdateMinimapXY(x,y);
  return 1;
};

global void MapFixWood(int x,int y)
{
    // side neighbors
    FixWood( x+1, y   );
    FixWood( x-1, y   );
    FixWood( x  , y+1 );
    FixWood( x  , y-1 );
};

global void MapRemoveWood(int x,int y)
{
    MAPFIELD(x,y).Tile=TheMap.Tileset->NoWoodTile;
    MAPFIELD(x,y).Flags &= ~MapFieldForest;
    MAPFIELD(x,y).Flags &= ~MapFieldUnpassable;
#ifdef MOVE_MAP
    MAPMOVE(x,y) &= ~MapMoveUnpassable;
#endif
    UpdateMinimapXY(x,y);
//    MustRedraw|=RedrawMinimap;
};

//@}
