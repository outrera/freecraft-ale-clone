/*
**	A clone of a famous game.
*/
/**@name minimap.c	-	The minimap. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: minimap.c,v 1.27 1999/12/09 16:27:13 root Exp $
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
#include "minimap.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

local OldGraphic Minimap;			/// generated minimap
local int Minimap2MapX[MINIMAP_W];	/// fast conversion table
local int Minimap2MapY[MINIMAP_H];	/// fast conversion table
local int Map2MinimapX[MaxMapWidth];	/// fast conversion table
local int Map2MinimapY[MaxMapHeight];	/// fast conversion table

//	MinimapScale:
//	32x32 64x64 96x96 128x128 256x256 512x512 ...
//	  *4    *2    *4/3   *1	     *1/2    *1/4
global int MinimapScale;		/// Minimap scale to fit into window
global int MinimapX;			/// Minimap drawing position x offset
global int MinimapY;			/// Minimap drawing position y offset

global int MinimapWithTerrain=1;	/// display minimap with terrain
global int MinimapFriendly=1;		/// switch colors of friendly units
global int MinimapShowSelected=1;	/// highlight selected units

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Update minimap at map position x,y.
**
**	@param tx	Tile X position, where the map changed.
**	@param ty	Tile Y position, where the map changed.
**
**	FIXME: this can surely speeded up??
*/
global void UpdateMinimapXY(int tx,int ty)
{
    int mx;
    int my;
    int x;
    int y;
    int scale;

    DebugLevel3("Update minimap\n");

    scale=(MinimapScale/MINIMAP_FAC);
    if( !scale ) {
	scale=1;
    }
    //
    //	Pixel 7,6 7,14, 15,6 15,14 are taken for the minimap picture.
    //
    ty*=TheMap.Width;
    for( my=0; my<MINIMAP_H; my++ ) {
	y=Minimap2MapY[my];
	if( y<ty ) {
	    continue;
	}
	if( y>ty ) {
	    break;
	}

	for( mx=0; mx<MINIMAP_W; mx++ ) {
	    int tile;

	    x=Minimap2MapX[mx];
	    if( x<tx ) {
		continue;
	    }
	    if( x>tx ) {
		break;
	    }

	    tile=TheMap.Fields[x+y].Tile;
	    Minimap.Data[mx+my*MINIMAP_W]=
		TheMap.TileData.Data[
		    (tile%TILE_PER_ROW)*TileSizeX+7
			+(mx%scale)*8
		    +((tile/TILE_PER_ROW)*TileSizeX+6
			+(my%scale)*8)*TheMap.TileData.Width];
	}
    }
}

/**
**	Update a mini map from the tiles of the map.
**
**	FIXME: this can surely speeded up??
*/
global void UpdateMinimap(void)
{
    int mx;
    int my;
    int scale;

    DebugLevel3("Update minimap\n");

    scale=(MinimapScale/MINIMAP_FAC);
    if( !scale ) {
	scale=1;
    }
    //
    //	Pixel 7,6 7,14, 15,6 15,14 are taken for the minimap picture.
    //
    for( my=0; my<MINIMAP_H; my++ ) {
	for( mx=0; mx<MINIMAP_W; mx++ ) {
	    int tile;

	    tile=TheMap.Fields[Minimap2MapX[mx]+Minimap2MapY[my]].Tile;
	    Minimap.Data[mx+my*MINIMAP_W]=
		TheMap.TileData.Data[
		    (tile%TILE_PER_ROW)*TileSizeX+7
			+(mx%scale)*8
		    +((tile/TILE_PER_ROW)*TileSizeX+6
			+(my%scale)*8)*TheMap.TileData.Width];
	}
    }
}

/**
**	Create a mini map from the tiles of the map.
*/
global void CreateMinimap(void)
{
    int n;

    if( TheMap.Width>TheMap.Height ) {
	n=TheMap.Width;
    } else {
	n=TheMap.Height;
    }
    MinimapScale=(MINIMAP_W*MINIMAP_FAC)/n;
    DebugLevel3("Minimap Scale: %d\n",MinimapScale);

    // FIXME: X,Y offset not supported!!
    MinimapX=0;
    MinimapY=0;

    //
    //	Calculate minimap fast lookup tables.
    //
    for( n=0; n<MINIMAP_W; ++n ) {
	Minimap2MapX[n]=(n*MINIMAP_FAC)/MinimapScale;
    }
    for( n=0; n<MINIMAP_H; ++n ) {
	Minimap2MapY[n]=((n*MINIMAP_FAC)/MinimapScale)*TheMap.Width;
    }
    for( n=0; n<TheMap.Width; ++n ) {
	Map2MinimapX[n]=(n*MinimapScale)/MINIMAP_FAC;
    }
    for( n=0; n<TheMap.Height; ++n ) {
	Map2MinimapY[n]=(n*MinimapScale)/MINIMAP_FAC;
    }

    NewOldGraphic(MINIMAP_W,MINIMAP_H,&Minimap);

    UpdateMinimap();
}

/**
**	Draw the mini map with current viewpoint.
**
**	@param vx	View point X position.
**	@param vy	View point Y position.
**
**	This one of the hot-points in the program optimize and optimize!
*/
global void DrawMinimap(int vx,int vy)
{
    Unit* unit;
    int mx;
    int my;
    int w;
    int h;
    int h0;
    UnitType* type;
    int flags;
    static int RedPhase;
    int i;

    RedPhase^=1;

    //
    //	Draw the terrain
    //
    if( MinimapWithTerrain ) {
	for( my=0; my<MINIMAP_H; ++my ) {
	    for( mx=0; mx<MINIMAP_W; ++mx ) {
		flags=TheMap.Fields[Minimap2MapX[mx]+Minimap2MapY[my]].Flags;
		if( flags&MapFieldExplored &&
			( (flags&MapFieldVisible)
			    || TheMap.NoFogOfWar || ((mx&1)==(my&1)) ) ) {
		    DrawPointUnclipped(Minimap.Data[mx+my*MINIMAP_W]
			    ,MINIMAP_X+mx,MINIMAP_Y+my);
		} else {
		    DrawPointUnclipped(ColorBlack,MINIMAP_X+mx,MINIMAP_Y+my);
		}
	    }
	}
    } else {
	 FillRectangle(ColorBlack,MINIMAP_X,MINIMAP_Y,MINIMAP_W,MINIMAP_H);
    }

    //
    //	Draw units on map
    //
    for( i=0; i<NumUnits; i++ ) {
	SysColors color;

	unit=Units[i];
	type=unit->Type;

	flags=TheMap.Fields[unit->X+unit->Y*TheMap.Width].Flags;
	// Draw only units on explored fields
	if( !(flags&MapFieldExplored) ) {
	    continue;
	}
	// Draw only units on visible fields
	if( !TheMap.NoFogOfWar && !(flags&MapFieldVisible) ) {
	    continue;
	}
	// FIXME: buildings under fog of war.

	if( unit->Player->Player==PlayerNumNeutral ) {
	    if( type->Critter ) {
		color=ColorNPC;
	    } else if( type->OilPatch ) {
		color=ColorBlack;
	    } else {
		color=ColorYellow;
	    }
	} else if( unit->Player==ThisPlayer ) {
	    if( unit->Attacked && RedPhase ) {
		color=ColorRed;
	    } else if( MinimapShowSelected && unit->Selected ) {
		color=ColorWhite;
	    } else {
		color=ColorGreen;
	    }
	} else {
	    color=unit->Player->Color;
	}

	mx=MINIMAP_X+1+MinimapX+Map2MinimapX[unit->X];
	my=MINIMAP_Y+1+MinimapY+Map2MinimapY[unit->Y];
	w=Map2MinimapX[type->TileWidth];
	if( mx+w>=MINIMAP_X+MINIMAP_W ) {	// clip right side
	    w=MINIMAP_X+MINIMAP_W-mx;
	}
	h0=Map2MinimapY[type->TileHeight];
	if( my+h0>=MINIMAP_Y+MINIMAP_H ) {	// clip bottom side
	    h0=MINIMAP_Y+MINIMAP_H-my;
	}
	while( w-->=0 ) {
	    h=h0;
	    while( h-->=0 ) {
		DrawPointUnclipped(color,mx+w,my+h);
	    }
	}
    }
}

local int OldMinimapCursorX;		/// Save MinimapCursorX
local int OldMinimapCursorY;		/// Save MinimapCursorY
local int OldMinimapCursorW;		/// Save MinimapCursorW
local int OldMinimapCursorH;		/// Save MinimapCursorH
local int OldMinimapCursorSize;		/// Saved image size
local VMemType* OldMinimapCursorImage;	/// Saved image behind cursor

/**
**	Hide minimap cursor.
*/
global void HideMinimapCursor(void)
{
    int i;
    int w;
    int h;
    VMemType* sp;
    VMemType* dp;

    if( !(sp=OldMinimapCursorImage) ) {
	return;
    }

    w=OldMinimapCursorW;
    h=OldMinimapCursorH;

    sp=OldMinimapCursorImage;
    dp=VideoMemory+OldMinimapCursorY*VideoWidth+OldMinimapCursorX;
    memcpy(dp,sp,w*sizeof(VMemType));
    sp+=w;
    for( i=0; i<h; ++i ) {
	*dp=*sp++;
	dp[w]=*sp++;
	dp+=VideoWidth;
    }
    memcpy(dp,sp,(w+1)*sizeof(VMemType));
}

/**
**	Draw minimap cursor.
**
**	@param vx	View point X position.
**	@param vy	View point Y position.
*/
global void DrawMinimapCursor(int vx,int vy)
{
    int x;
    int y;
    int w;
    int h;
    int i;
    VMemType* sp;
    VMemType* dp;

    OldMinimapCursorX=x=MINIMAP_X+(vx*MinimapScale)/MINIMAP_FAC;
    OldMinimapCursorY=y=MINIMAP_Y+(vy*MinimapScale)/MINIMAP_FAC;
    OldMinimapCursorW=w=(MapWidth*MinimapScale)/MINIMAP_FAC-1;
    OldMinimapCursorH=h=(MapHeight*MinimapScale)/MINIMAP_FAC-1;

    i=(w+1+h)*2*sizeof(VMemType);
    if( OldMinimapCursorSize<i ) {
	if( OldMinimapCursorImage ) {
	    OldMinimapCursorImage=realloc(OldMinimapCursorImage,i);
	} else {
	    OldMinimapCursorImage=malloc(i);
	}
	DebugLevel3("Cursor memory %d\n",i);
	OldMinimapCursorSize=i;
    }

    // FIXME: not 100% correct
    dp=OldMinimapCursorImage;
    sp=VideoMemory+OldMinimapCursorY*VideoWidth+OldMinimapCursorX;
    memcpy(dp,sp,w*sizeof(VMemType));
    dp+=w;
    for( i=0; i<h; ++i ) {
	*dp++=*sp;
	*dp++=sp[w];
	sp+=VideoWidth;
    }
    memcpy(dp,sp,(w+1)*sizeof(VMemType));

    // FIXME: The viewpoint color should be configurable
    switch( ThisPlayer->Race ) {
	case PlayerRaceHuman:
	    DrawRectangle(ColorWhite,x,y,w,h);
	    break;
	case PlayerRaceOrc:
	    DrawRectangle(ColorGray,x,y,w,h);
	    break;
    }
}

//@}
