/*
**	A clone of a famous game.
*/
/**@name map.c		-	The map. */
/*
**
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: map.c,v 1.36 1999/12/05 18:37:01 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "clone.h"
#include "map.h"
#include "minimap.h"
#include "player.h"
#include "unit.h"
#include "pathfinder.h"
#include "pud.h"

#ifdef USE_CCL
#include <guile/gh.h>
#endif

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

global WorldMap TheMap;			/// The current map

global unsigned MapX;			/// Map tile X start on display
global unsigned MapY;			/// Map tile Y start on display
global unsigned MapWidth;		/// map width in tiles
global unsigned MapHeight;		/// map height in tiles

/*----------------------------------------------------------------------------
--	Map loading/saving
----------------------------------------------------------------------------*/

/**
**	Load a clone map.
**
**	@param filename	map filename
**	@param map	map loaded
*/
local void LoadCloneMap(const char* filename,WorldMap* map)
{
    DebugLevel1(__FUNCTION__"%p \n",map);

#ifdef USE_CCL
    gh_eval_file((char*)filename);
#else
    fprintf(stderr,"Sorry, you need guile installed to use clone maps!\n");
#endif
}

/**
**	Load any map.
**
**	@param filename	map filename
**	@param map	map loaded
*/
global void LoadMap(const char* filename,WorldMap* map)
{
    const char* tmp;

    tmp=strrchr(filename,'.');
    if( tmp ) {
	if( !strcmp(tmp,".gz") ) {
	    while( tmp-1>filename && *--tmp!='.' ) {
	    }
	}
	if( !strcmp(tmp,".cm") || !strcmp(tmp,".cm.gz") ) {
	    LoadCloneMap(filename,map);
	    // return;
	} else
	if( !strcmp(tmp,".pud") || !strcmp(tmp,".pud.gz") ) {
	    LoadPud(filename,map);
	    // return;
	}
    } else
    LoadPud(filename,map);
}

/**
**	Save a map.
**
**	@param file	Output file.
*/
global void SaveMap(FILE* file)
{
    fprintf(file,"\n;;; -----------------------------------------\n");
    fprintf(file,";;; MODULE: map $Id: map.c,v 1.36 1999/12/05 18:37:01 root Exp $\n");

    fprintf(file,"(clone-map\n");

    // FIXME: Need version number here!
    fprintf(file,"  '(version %d.%d)\n",0,0);
    fprintf(file,"  '(description \"Saved\")\n");
    fprintf(file,"  '(terrain %d \"%s\")\n"
	    ,TheMap.Terrain,Tilesets[TheMap.Terrain].Name);
    fprintf(file,"  '(tiles #(\n");
    fprintf(file,"  )\n");

    fprintf(file,")\n");

    fprintf(file,"(the-map\n");
    fprintf(file,"  (%d %d)\n",TheMap.Width,TheMap.Height);
    fprintf(file,"  %d\n",TheMap.NoFogOfWar);
    fprintf(file,")\n");
}

/*----------------------------------------------------------------------------
--
----------------------------------------------------------------------------*/

/**
**      Marks seen tile -- used mainly for the Fog Of War
*/
global void MapMarkSeenTile( int x, int y )
{
  int t, st; // tile, seentile

  if (MAPFIELD(x,y).SeenTile == MAPFIELD(x,y).Tile) return;

  t  = MAPFIELD(x,y).Tile;
  st = MAPFIELD(x,y).SeenTile;
  MAPFIELD(x,y).SeenTile = MAPFIELD(x,y).Tile;

  if( !TheMap.Tileset ) return;	// FIXME: this is needed, because tileset
				// is loaded after this function is needed
				// LoadPud, PlaceUnit, ... MapMarkSeenTile

  // handle WOODs

  if ( st != TheMap.Tileset->NoWoodTile && t == TheMap.Tileset->NoWoodTile )
    MapFixWood( x, y );
  else
  if ( st == TheMap.Tileset->NoWoodTile && t != TheMap.Tileset->NoWoodTile )
    FixWood( x, y );
  else
  if ( MapWoodChk( x, y ) && t != st )
    {
    FixWood( x, y );
    MapFixWood( x, y );
    }

  // handle WALLs
#define ISTILEWALL(t) (TILETYPE(t) == TileTypeHWall || TILETYPE(t) == TileTypeOWall)
  if ( ISTILEWALL(st) && !ISTILEWALL(st) )
    MapFixWall( x, y );
  else
  if ( !ISTILEWALL(st) && ISTILEWALL(st) )
    FixWall( x, y );
  else
  if ( MapWallChk( x, y, -1 ) && t != st )
    {
    FixWall( x, y );
    MapFixWall( x, y );
    }
};

/**
**	Reveal the entire map.
*/
global void RevealMap(void)
{
    int ix, iy;

    for ( ix = 0; ix < TheMap.Width; ix++ ) {
	for ( iy = 0; iy < TheMap.Height; iy++ ) {
	    MAPFIELD(ix,iy).Flags |= MapFieldExplored;
	    if (TheMap.NoFogOfWar) {
		MAPFIELD(ix,iy).Flags |= MapFieldVisible;
	    }
	    MapMarkSeenTile(ix,iy);
	}
    }
}

/**
**	Change viewpoint of map to x,y
**
**	@param x	X map tile position.
**	@param y	Y map tile position.
*/
global void MapSetViewpoint(int x,int y)
{
    if( x<0 ) {
	MapX=0;
    } else if( x>TheMap.Width-MapWidth ) {
	MapX=TheMap.Width-MapWidth;
    } else {
	MapX=x;
    }
    if( y<0 ) {
	MapY=0;
    } else if( y>TheMap.Height-MapHeight ) {
	MapY=TheMap.Height-MapHeight;
    } else {
	MapY=y;
    }
    MustRedraw|=RedrawMaps|RedrawMinimapCursor;
}

/**
**	Center map viewpoint on x,y.
**
**	@param x	X map tile position.
**	@param y	Y map tile position.
*/
global void MapCenter(int x,int y)
{
    MapSetViewpoint(x-(MapWidth/2),y-(MapHeight/2));
}

/*----------------------------------------------------------------------------
--	Map queries
----------------------------------------------------------------------------*/

/**
**	Water on map tile.
**
**	@param x	X map tile position.
**	@param y	Y map tile position.
**	@return		True if water, false otherwise.
*/
global int WaterOnMap(int tx,int ty)
{
    return TheMap.Fields[tx+ty*TheMap.Width].Flags&MapFieldWaterAllowed;
}

/**
**	Coast on map tile.
**
**	@param x	X map tile position.
**	@param y	Y map tile position.
**	@return		True if coast, false otherwise.
*/
global int CoastOnMap(int tx,int ty)
{
    return TheMap.Fields[tx+ty*TheMap.Width].Flags&MapFieldCoastAllowed;
}

/**
**	Wall on map tile.
**
**	@param x	X map tile position.
**	@param y	Y map tile position.
**	@return		True if wall, false otherwise.
*/
global int WallOnMap(int tx,int ty)
{
    return TheMap.Fields[tx+ty*TheMap.Width].Flags&MapFieldWall;
}

/**
**	Human wall on map tile.
**
**	@param x	X map tile position.
**	@param y	Y map tile position.
**	@return		True if human wall, false otherwise.
*/
global int HumanWallOnMap(int tx,int ty)
{
    return (TheMap.Fields[tx+ty*TheMap.Width].Flags
	    &(MapFieldWall|MapFieldHuman))==(MapFieldWall|MapFieldHuman);
}

/**
**	Orc wall on map tile.
**
**	@param x	X map tile position.
**	@param y	Y map tile position.
**	@return		True if orcish wall, false otherwise.
*/
global int OrcWallOnMap(int tx,int ty)
{
    return (TheMap.Fields[tx+ty*TheMap.Width].Flags
	    &(MapFieldWall|MapFieldHuman))==MapFieldWall;
}

/**
**	Forest on map tile. Checking version.
*/
global int CheckedForestOnMap(int tx,int ty)
{
    if( tx<0 || ty<0 || tx>=TheMap.Width || ty>=TheMap.Height ) {
	return 0;
    }
    return TheMap.Fields[tx+ty*TheMap.Width].Flags&MapFieldForest;
}

/**
**	Forest on map tile.
*/
global int ForestOnMap(int tx,int ty)
{
    IfDebug(
	if( tx<0 || ty<0 || tx>=TheMap.Width || ty>=TheMap.Height ) {
	    // FIXME: must cleanup calling function !
	    fprintf(stderr,"Used x %d, y %d\n",tx,ty);
	    abort();
	    return 0;
	}
    );

    return TheMap.Fields[tx+ty*TheMap.Width].Flags&MapFieldForest;
}

/**
**	Can move to this point, applying mask.
*/
global int CheckedCanMoveToMask(int x,int y,int mask)
{
    if( x<0 || y<0 || x>=TheMap.Width || y>=TheMap.Height ) {
	return 0;
    }

#ifdef MOVE_MAP
    return !(TheMap.MovementMap[x+y*TheMap.Width]&mask);
#else
    return !(TheMap.Fields[x+y*TheMap.Width].Flags&mask);
#endif
}

/**
**	Can move to this point, applying mask.
*/
global int CanMoveToMask(int x,int y,int mask)
{
    IfDebug(
	if( x<0 || y<0 || x>=TheMap.Width || y>=TheMap.Height ) {
	    // FIXME: must cleanup calling function !
	    fprintf(stderr,"Used x %d, y %d, mask %x\n",x,y,mask);
	    abort();
	    return 0;
	}
    );

#ifdef MOVE_MAP
    return !(TheMap.MovementMap[x+y*TheMap.Width]&mask);
#else
    return !(TheMap.Fields[x+y*TheMap.Width].Flags&mask);
#endif
}

#ifdef MOVE_MAP
/**
**	Return units movement bits, for map field.
**
**	Can be used to mark the map.
**
**	@param unit	Pointer to unit.
**
**	@return		Field bits.
*/
global int UnitMoveBits(const Unit* unit)
{
    switch( unit->Type->UnitType ) {
	case UnitTypeLand:
	    return MapMoveLandUnit;
	case UnitTypeFly:
	    return MapMoveAirUnit;
	case UnitTypeNaval:
	    return MapMoveSeaUnit;
	default:
	    DebugLevel1(__FUNCTION__": Were moves this unit?\n");
	    return 0;
    }
}
#endif

/**
**	Return the units field flags.
**	This flags are used to mark the field for this unit.
**
**	@param unit	Pointer to unit.
**	@return		Field flags to be set.
*/
global unsigned UnitFieldFlags(const Unit* unit)
{
    // FIXME: Should be moved into unittype structure, and allow more types.
    switch( unit->Type->UnitType ) {
	case UnitTypeLand:		// on land
	    return MapFieldLandUnit;
	case UnitTypeFly:		// in air
	    return MapFieldAirUnit;
	case UnitTypeNaval:		// on water
	    return MapFieldSeaUnit;
	default:
	    DebugLevel1(__FUNCTION__": Were moves this unit?\n");
	    return 0;
    }
}

/**
**	Return the unit type movement mask.
**		TODO: Should add this to unit-type structure.
**
**	@param type	Unit type pointer.
**
**	@return		Movement mask of unit type.
*/
global int TypeMovementMask(const UnitType* type)
{
#ifdef MOVE_MAP
    switch( type->UnitType ) {
	case UnitTypeLand:
	    return MapMoveLandUnit
		| MapMoveCoast
		| MapMoveOnlyWater
		| MapMoveUnpassable
		| MapMoveBuildingUnit;
	case UnitTypeFly:
	    return MapMoveAirUnit;
	case UnitTypeNaval:
	    if( type->Transporter ) {
		return MapMoveSeaUnit
		    | MapMoveOnlyLand
		    | MapMoveBuildingUnit;
	    }
	    return MapMoveSeaUnit
		| MapMoveOnlyLand
		| MapMoveCoast
		| MapMoveUnpassable
		| MapMoveBuildingUnit;
	default:
	    DebugLevel1(__FUNCTION__": Were moves this unit?\n");
	    return 0;
    }
#else
    // FIXME: Should be moved into unittype structure, and allow more types.
    switch( type->UnitType ) {
	case UnitTypeLand:		// on land
	    return MapFieldLandUnit
		| MapFieldBuilding	// already occuppied
		| MapFieldWall
		| MapFieldRocks
		| MapFieldForest	// wall,rock,forest not 100% clear?
		| MapFieldCoastAllowed
		| MapFieldWaterAllowed	// can't move on this
		| MapFieldUnpassable;
	case UnitTypeFly:		// in air
	    return MapFieldAirUnit;	// already occuppied
	case UnitTypeNaval:		// on water
	    if( type->Transporter ) {	
		return MapFieldLandUnit
		    | MapFieldSeaUnit
		    | MapFieldBuilding	// already occuppied
		    | MapFieldLandAllowed;	// can't move on this
		    //| MapFieldUnpassable;	// FIXME: bug?
	    }
	    return MapFieldSeaUnit
		| MapFieldBuilding	// already occuppied
		| MapFieldCoastAllowed
		| MapFieldLandAllowed	// can't move on this
		| MapFieldUnpassable;
	default:
	    DebugLevel1(__FUNCTION__": Were moves this unit?\n");
	    return 0;
    }
#endif
}

/**
**	Return units movement mask.
**
**	@param unit	Unit pointer.
**
**	@return		Movement mask of unit.
*/
global int UnitMovementMask(const Unit* unit)
{
    return TypeMovementMask(unit->Type);
}

/*----------------------------------------------------------------------------
*/

/**
**	Fixes initially the wood and seen tiles.
*/
global void PreprocessMap(void)
{
  unsigned ix, iy;

    for ( ix = 0; ix < TheMap.Width; ix++ )
      for ( iy = 0; iy < TheMap.Height; iy++ )
        {
        MAPFIELD( ix, iy ).SeenTile = MAPFIELD( ix, iy ).Tile;
        }

    // it is required for fixing the wood that all tiles are marked as seen!
    for ( ix = 0; ix < TheMap.Width; ix++ )
      for ( iy = 0; iy < TheMap.Height; iy++ )
        {
        FixWood( ix, iy );
        FixWall( ix, iy );
        }
};


global int Screen2MapX(int x) { return (((x)-MAP_X)/TileSizeX+MapX); }
global int Screen2MapY(int x) { return (((x)-MAP_Y)/TileSizeY+MapY); }
global int Map2ScreenX(int x) { return (MAP_X+((x)-MapX)*TileSizeX); }
global int Map2ScreenY(int x) { return (MAP_Y+((x)-MapY)*TileSizeY); }

//@}
