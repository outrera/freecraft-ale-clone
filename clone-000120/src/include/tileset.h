/*
**	A clone of a famous game.
*/
/**@name tileset.h	-	The tileset headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: tileset.h,v 1.11 2000/01/03 02:30:15 root Exp $
*/

#ifndef __TILESET_H__
#define __TILESET_H__

//@{

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

#define TileSizeX	32		/// Size of a tile in X
#define TileSizeY	32		/// Size of a tile in Y

#define TILE_PER_ROW	16		/// tiles stored in an image row
#define TILE_ROWS	24		/// tiles rows in the image
#define TILE_COUNT	(TILE_PER_ROW*TILE_ROWS)

/**
**   These are used for lookup tiles types
**   mainly used for the FOW implementation of the seen woods/rocks
*/
enum _tile_type_ {
    TileTypeUnknown,			/// unknown tile type
    TileTypeNoWood,			/// no wood tile
    TileTypeWood,			/// any wood tile
    TileTypeGrass,			/// any grass tile
    TileTypeNoRock,			/// no rock tile
    TileTypeRock,			/// any rock tile
    TileTypeCoast,			/// any coast tile
    TileTypeHWall,			/// any human wall tile
    TileTypeOWall,			/// any orc wall tile
    TileTypeNoWall,			/// no wall tile
    TileTypeWater			/// any water tile
};

/**
**	Tileset definition.
*/
typedef struct _tileset_ {
    char*	Name;			/// name for future extensions
    char*	File;			/// file containing image data
    const unsigned short* Table;	/// pud to internal conversion table
    unsigned char* TileTypeTable;	/// lookup tile type
    unsigned	NoWoodTile;		/// tile placed where wood is gone
    unsigned	FirstRockTile;		/// first rock tile :)
    unsigned	NoRockTile;		/// tile placed where rocks are gone
    unsigned	HumanWall100Tile;	/// 100% wall
    unsigned	HumanWall50Tile;	/// 100% wall
    unsigned	HumanNoWallTile;	/// tile placed where walls are gone
    unsigned	OrcWall100Tile;		/// 100% wall
    unsigned	OrcWall50Tile;		/// 100% wall
    unsigned	OrcNoWallTile;		/// tile placed where walls are gone
} Tileset;

// FIXME: this #define's should be removed

#define TilesetSummer		0	/// Reference number
#define TilesetWinter		1	/// Reference number
#define TilesetWasteland	2	/// Reference number
#define TilesetSwamp		3	/// Reference number

// FIXME: allow more tilesets

#define TilesetMax		4	/// Biggest supported tilset number

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

extern Tileset Tilesets[TilesetMax];	/// Tileset information

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern void LoadTileset(void);		/// Load tileset definition

//@}

#endif // !__TILESET_H__
