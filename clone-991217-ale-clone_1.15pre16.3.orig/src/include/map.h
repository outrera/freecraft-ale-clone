/*
**	A clone of a famous game.
*/
/**@name map.h		-	The map headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: map.h,v 1.34 1999/12/05 18:36:54 root Exp $
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

#define noACTION_MAP			/// only needed for debug
#define noMOVE_MAP			/// still in use, removed soon

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

    // FIXME: next integrated into fields
#ifdef MOVE_MAP
    unsigned short*	MovementMap;	/// the movement on map
#endif
#ifdef ACTION_MAP
    unsigned short*	ActionMap;	/// the action on map
#endif

    unsigned		NoFogOfWar;	/// fog of war disabled

    unsigned		Terrain;	/// terrain type (summer,winter,...)
    Tileset*		Tileset;	/// tileset data

    unsigned 		TileCount;	/// how many tiles are available
    unsigned char**	Tiles;		/// pointer to tile data
    OldGraphic		TileData;	/// tiles graphic for map
} WorldMap;

//
//	Used to read puds. FIXME: move to pud.h or into pud.c
//
#define MapMoveOnlyLand		0x01	/// only land units
#define MapMoveCoast		0x02	/// only water transporters
#define MapMoveHuman		0x04	/// marks human wall on map
#define MapMoveWallO		0x08	/// orcish wall on map
// 1101
#define MapMoveWallH		0x0D	/// human wall on map
#define MapMoveDirt		0x10	/// no buildings allowed
// 20
#define MapMoveOnlyWater	0x40	/// only water units
#define MapMoveUnpassable	0x80	/// blocked

#define MapMoveForestOrRock	0x81	/// Forest or Rock on map
#define MapMoveCoast2		0x82	/// unknown
#define MapMoveWall		0x8D	/// Wall on map

#define MapMoveLandUnit		0x100	/// Land unit
#define MapMoveAirUnit		0x200	/// Air unit
#define MapMoveSeaUnit		0x400	/// Water unit
#define MapMoveBuildingUnit	0x800	/// Building unit
#define MapMoveBlocked		0xF00	/// No unit allowed

#ifdef MOVE_LAND
/// Mask for land buildings
#define MapMaskLandBuilding	\
    (MapMoveBuildingUnit|MapMoveLandUnit \
	|MapMoveUnpassable|MapMoveOnlyWater|MapMoveDirt|MapMoveCoast)

/// Mask for shore buildings
#define MapMaskShoreBuilding	\
    (MapMoveBuildingUnit|MapMoveSeaUnit \
	|MapMoveOnlyLand|MapMoveDirt)

/// Mask for water buildings
#define MapMaskWaterBuilding	\
    (MapMoveBuildingUnit|MapMoveSeaUnit \
	|MapMoveOnlyLand|MapMoveDirt|MapMoveCoast)
#endif

// LOW BITS: Area connected!
#define MapActionWater		0x0000	/// water on map
#define MapActionLand		0x4000	/// land on map
#define MapActionIsland		0xFFFA	/// no trans, no land on map
#define MapActionWall		0xFFFB	/// wall on map
#define MapActionRocks		0xFFFD	/// rocks on map
#define MapActionForest		0xFFFE	/// forest on map

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
extern void MapRemoveWall(int x,int y);
    /// correct wall on tile
extern int FixWall(int x,int y);
    /// correct wall on tile side neighbors
extern void MapFixWall(int x,int y);
    /// check wall on tile
extern int MapWallChk(int x,int y,int walltype);

//
//	in map_wood.c
//
    /// remove wood on tile
extern void MapRemoveWood(int x,int y);
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

extern void MapRemoveRock(int x,int y);
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

extern void PreprocessMap();

extern void MapSetWall(int x,int y,int humanwall);

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
