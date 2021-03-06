/*
**	A clone of a famous game.
*/
/**@name map.h		-	The map headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: map.h,v 1.36 2000/01/03 02:30:14 root Exp $
*/

#ifndef __MAP_H__
#define __MAP_H__

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include "video.h"
#include "tileset.h"

/*----------------------------------------------------------------------------
--	Map
----------------------------------------------------------------------------*/

// JOHNS:	only limited by computer memory
//	 512x512:	  2 MB
//	1024x1024:	  8 MB
//	2048*2048:	 32 MB
//	4096*4096:	128 MB
#define MaxMapWidth	1024		/// maximal map width supported
#define MaxMapHeight	1024		/// maximal map height supported

/*----------------------------------------------------------------------------
--	Map - field
----------------------------------------------------------------------------*/

/**
**	Describes a field of the map.
*/
typedef struct _map_field_ {
    unsigned short	Tile;		/// graphic tile number
    unsigned short	SeenTile;	/// last seen tile (FOW)
    unsigned char	Value;		/// HP for walls
    unsigned short	Flags;		/// field flags
} MapField;

#define MapFieldVisible		0x0001	/// Field visible
#define MapFieldExplored	0x0002	/// Field explored

#define MapFieldHuman		0x0008	/// Human is owner of the field (walls)

#define MapFieldLandAllowed	0x0010	/// Land units allowed
#define MapFieldCoastAllowed	0x0020	/// Coast (transporter) units allowed
#define MapFieldWaterAllowed	0x0040	/// Water units allowed
#define MapFieldNoBuilding	0x0080	/// No buildings allowed

#define MapFieldUnpassable	0x0100	/// Field is movement blocked
#define MapFieldWall		0x0200	/// Field contains wall
#define MapFieldRocks		0x0400	/// Field contains rocks
#define MapFieldForest		0x0800	/// Field contains forest

#define MapFieldLandUnit	0x1000	/// Land unit on field
#define MapFieldAirUnit		0x2000	/// Air unit on field
#define MapFieldSeaUnit		0x4000	/// Water unit on field
#define MapFieldBuilding	0x8000	/// Building on field

/*----------------------------------------------------------------------------
--	Map itself
----------------------------------------------------------------------------*/

/**
**	Describes the wold map.
*/
typedef struct _world_map_ {
    unsigned		Width;		/// the map width
    unsigned		Height;		/// the map height

    MapField*		Fields;		/// fields on map

    unsigned		NoFogOfWar;	/// fog of war disabled

    unsigned		Terrain;	/// terrain type (summer,winter,...)
    Tileset*		Tileset;	/// tileset data

    unsigned 		TileCount;	/// how many tiles are available
    unsigned char**	Tiles;		/// pointer to tile data
    OldGraphic		TileData;	/// tiles graphic for map

    char		Description[32];/// map description short
} WorldMap;

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

extern WorldMap TheMap;			/// The current map

extern unsigned MapX;			/// The map X start on display
extern unsigned MapY;			/// The map Y start on display
    /// map width in tiles for current mode (14 for 640x480)
extern unsigned MapWidth;
    /// map height in tiles for current mode (14 for 640x480)
extern unsigned MapHeight;

extern char MustRedrawRow[MAXMAP_W];		/// Flags must redraw map row.
extern char MustRedrawTile[MAXMAP_W*MAXMAP_H];	/// Flags must redraw tile.

    ///  Fast draw tile, display and video mode independ
extern void (*VideoDrawTile)(const GraphicData*,int,int);

    /// Use original style fog of war
extern int OriginalFogOfWar;
    /// Contrast of fog of war
extern int FogOfWarContrast;
    /// Brightness of fog of war
extern int FogOfWarBrightness;
    /// Saturation of fog of war
extern int FogOfWarSaturation;

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

//
//	in map_draw.c
//
    /// Fast draw 32x32 tile for 32 bpp video modes.
extern void VideoDraw32Tile32(const GraphicData* data,int x,int y);
    /// Fast draw 32x32 tile for 16 bpp video modes.
extern void VideoDraw16Tile32(const GraphicData* data,int x,int y);
    /// Fast draw 32x32 tile for  8 bpp video modes.
extern void VideoDraw8Tile32(const GraphicData* data,int x,int y);

    /// Called when the color cycles
extern void MapColorCycle(void);
    /// Draw the map background
extern void DrawMapBackground(int x,int y);

//
//	in map_fog.c
//
    /// Mark the sight in range
extern void MapMarkSight(int tx,int ty,int range);
    /// Update visible areas for fog of war
extern void MapUpdateVisible(void);
    /// Draw the map fog of war
extern void DrawMapFogOfWar(int x,int y);
    /// Build tables for fog of war
extern void InitMapFogOfWar(void);

//
//	in map_wall.c
//
    /// remove wall on tile
extern void MapRemoveWall(unsigned x,unsigned y);
    /// correct wall on tile
extern int FixWall(int x,int y);
    /// correct wall on tile side neighbors
extern void MapFixWall(int x,int y);
    /// check wall on tile
extern int MapWallChk(int x,int y,int walltype);
    /// wall is hit
extern void HitWall(unsigned x,unsigned y,unsigned damage);

//
//	in map_wood.c
//
    /// remove wood on tile
extern void MapRemoveWood(unsigned x,unsigned y);
    /// correct wood on tile
extern int FixWood(int x,int y);
    /// correct wood on tile side neighbors
extern void MapFixWood(int x,int y);
    /// check wood on tile
extern int MapWoodChk(int x,int y);

//
//	in ccl_map.c
//
    /// register ccl features
extern void MapCclRegister(void);

/*
**	mixed sources
*/
    /// Load a map
extern void LoadMap(const char* file,WorldMap* map);
    /// Save the map
extern void SaveMap(FILE* file);

    /// Remove rock from the map
extern void MapRemoveRock(unsigned x,unsigned y);

extern void MapMarkSeenTile(int x,int y);
extern void RevealMap(void);

extern void MapCenter(int x,int y);
extern void MapSetViewpoint(int x,int y);

extern int WaterOnMap(int x,int y);
extern int CoastOnMap(int x,int y);

extern int WallOnMap(int x,int y);
extern int HumanWallOnMap(int x,int y);
extern int OrcWallOnMap(int x,int y);

extern int CheckedForestOnMap(int x,int y);
extern int ForestOnMap(int x,int y);
extern int CheckedCanMoveToMask(int x,int y,int mask);
extern int CanMoveToMask(int x,int y,int mask);

extern void PreprocessMap(void);

    /// Set wall on field
extern void MapSetWall(unsigned x,unsigned y,int humanwall);

/*----------------------------------------------------------------------------
--	Defines
----------------------------------------------------------------------------*/

#define    MAPFIELD(xx,yy) TheMap.Fields[(xx)+(yy)*TheMap.Width]
#define     MAPMOVE(xx,yy) TheMap.MovementMap[(xx)+(yy)*TheMap.Width]
#define       INMAP(xx,yy) ((xx)>=0&&(yy)>=0&&(xx)<TheMap.Width&&(yy)<TheMap.Height)
#define  MAPVISIBLE(xx,yy) (INMAP(xx,yy) ? !!(TheMap.Fields[(yy)*TheMap.Width+(xx)].Flags&MapFieldVisible ) : 1)
#define MAPEXPLORED(xx,yy) (INMAP(xx,yy) ? !!(TheMap.Fields[(yy)*TheMap.Width+(xx)].Flags&MapFieldExplored) : 1)

#define MAPTILE(xx,yy) (MAPFIELD(xx,yy).Tile)
#define MAPSEENTILE(xx,yy) (MAPFIELD(xx,yy).SeenTile)
#define TILETYPE(tile) (TheMap.Tileset->TileTypeTable[tile])

#define UNEXPLORED_TILE 0

//@}

#endif	// !__MAP_H__
