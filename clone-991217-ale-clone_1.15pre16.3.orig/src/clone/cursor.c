/*
**	A clone of a famous game.
*/
/**@name cursor.c	-	The cursors. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: cursor.c,v 1.23 1999/12/09 16:27:08 root Exp $
*/

//@{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clone.h"
#include "video.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "cursor.h"
#include "tileset.h"
#include "map.h"
#include "interface.h"

/*============================================================================
==	CURSOR
============================================================================*/

/**
**	Define cursor-types.
*/
global CursorType Cursors[CursorMax] = {
    { { "human gauntlet.png"
	,"orcish claw.png" }
	, 3, 2,	28,32 },
    { { "magnifying glass.png"
	,NULL }
	,11,11,	34,35 },
    { { "small green cross.png"
	,NULL }
	, 8, 8,	18,18 },
    { { "yellow eagle.png"
	,"yellow crosshairs.png" }
	,15,15,	32,32 },
    { { "green eagle.png"
	,"green crosshairs.png" }
	,15,15,	32,32 },
    { { "red eagle.png"
	,"red crosshairs.png" }
	,15,15,	32,32 },
    { { "cross.png"
	,NULL }
	,15,15,	32,32 },

    { { "arrow E.png"
	,NULL }
	,22,10,	32,24 },
    { { "arrow N.png"
	,NULL }
	,12, 2,	32,24 },
    { { "arrow NE.png"
	,NULL }
	,20, 2,	32,24 },
    { { "arrow NW.png"
	,NULL }
	, 2, 2,	32,24 },
    { { "arrow S.png"
	,NULL }
	,12,22,	32,24 },
    { { "arrow SE.png"
	,NULL }
	,20,18,	32,24 },
    { { "arrow SW.png"
	,NULL }
	, 2,18,	32,24 },
    { { "arrow W.png"
	,NULL }
	, 4,10,	32,24 },
};

global enum CursorState_e CursorState;	// cursor state
global int CursorAction;		// action for selection
global UnitType* CursorBuilding;	// building cursor

global CursorType* CloneCursor;		// cursor type
global int CursorX;			// cursor position
global int CursorY;
global int CursorStartX;		// rectangle started
global int CursorStartY;

global int OldCursorX;
global int OldCursorY;
global int OldCursorW;
global int OldCursorH;
global int OldCursorSize;
global VMemType* OldCursorImage;

/**
**	Load all cursor sprites.
*/
global void LoadCursors(void)
{
    int i;
    const char* file;

    for( i=0; i<sizeof(Cursors)/sizeof(*Cursors); ++i ) {
	if( !(file=Cursors[i].File[ThisPlayer->Race]) ) {
	    file=Cursors[i].File[0];	// default one, no race specific
	}
	// FIXME: size and hot-point extra!
	if( file ) {
	    char* buf;

	    buf=alloca(strlen(file)+9+1);
	    file=strcat(strcpy(buf,"graphic/"),file);
	    ShowLoadProgress("\tCursor %s\n",file);
	    Cursors[i].RleSprite=LoadRleSprite(file
		    ,0,0);
	    // FIXME: this is hack!!
		    //,Cursors[i].Width,Cursors[i].Height);
	}
    }
}

/**
**	Save image behind cursor.
*/
local void SaveCursor(void)
{
    VMemType *dp, *sp;
    int w,h,i;
    int x;
    int y;

    x=OldCursorX;
    w=OldCursorW;
    if( x<0 ) {
	w-=x;
	x=0;
    }
    if( w>VideoWidth-x) {	// normalize width
	w=VideoWidth-x;
    }
    if( !w ) {
	return;
    }

    y=OldCursorY;
    h=OldCursorH;
    if( y<0 ) {
	w-=y;
	y=0;
    }
    if( h>VideoHeight-y ) {	// normalize height
	h=VideoHeight-y;
    }
    if( !h ) {
	return;
    }

    i=w*h*sizeof(VMemType);
    if( OldCursorSize<i ) {
	if( OldCursorImage ) {
	    OldCursorImage=realloc(OldCursorImage,i);
	} else {
	    OldCursorImage=malloc(i);
	}
	DebugLevel3("Cursor memory %d\n",i);
	OldCursorSize=i;
    }
    dp=OldCursorImage;
    sp=VideoMemory+y*VideoWidth+x;
    while( h-- ) {
	memcpy(dp,sp,w*sizeof(VMemType));
	dp+=w;
	sp+=VideoWidth;
    }
}

/**
**	Restore image behind cursor.
*/
local void RestoreCursor(void)
{
    VMemType *dp, *sp;
    int w;
    int h;
    int x;
    int y;

    if( !(sp=OldCursorImage) ) {	// no cursor saved
	return;
    }

    x=OldCursorX;
    w=OldCursorW;
    if( x<0 ) {
	w-=x;
	x=0;
    }
    if( w>VideoWidth-x) {	// normalize width
	w=VideoWidth-x;
    }
    if( !w ) {
	return;
    }

    y=OldCursorY;
    h=OldCursorH;
    if( y<0 ) {
	w-=y;
	y=0;
    }
    if( h>VideoHeight-y ) {	// normalize height
	h=VideoHeight-y;
    }
    if( !h ) {
	return;
    }

    dp=VideoMemory+y*VideoWidth+x;
    while( h-- ) {
	memcpy(dp,sp,w*sizeof(VMemType));
	sp+=w;
	dp+=VideoWidth;
    }
}

/**
**	Draw cursor.
*/
global void DrawCursor(CursorType* type,int x,int y,int frame)
{
    OldCursorX=x-=type->HotX;
    OldCursorY=y-=type->HotY;
    OldCursorW=type->RleSprite->Width;
    OldCursorH=type->RleSprite->Height;

    SaveCursor();
    DrawRleSpriteClipped(type->RleSprite,frame,x,y);
}

/**
**	Hide cursor.
*/
global void HideCursor(void)
{
    RestoreCursor();
}

/*----------------------------------------------------------------------------
--	DRAW CURSOR
----------------------------------------------------------------------------*/

local int RectangleCursor;		/// Flag: last cursor was rectangle
local int BuildingCursor;		/// Flag: last cursor was building

/**
**	Draw cursor for selecting building position.
*/
local void DrawBuildingCursor(void)
{
    int x;
    int y;
    int x1;
    int y1;
    int mx;
    int my;
    int color;
    int f;
    int w;
    int w0;
    int h;
    int mask;

    x=((CursorX-MAP_X)/TileSizeX)*TileSizeX+MAP_X;	// Align to grid
    y=((CursorY-MAP_Y)/TileSizeY)*TileSizeY+MAP_Y;
    mx=Screen2MapX(x);
    my=Screen2MapY(y);

    //
    //	Draw building
    //
    PlayerPixels(ThisPlayer);
    SetClipping(MAP_X,MAP_Y,MAP_X+MapWidth*TileSizeX,MAP_Y+MapHeight*TileSizeY);
    DrawUnitType(CursorBuilding,0,x,y);
    // FIXME: This is dangerous here
    SetClipping(0,0,VideoWidth,VideoHeight);

    //
    //	Draw the allow overlay
    //
    f=CanBuildHere(CursorBuilding,mx,my);
#ifdef MOVE_MAP
    if( CursorBuilding->ShoreBuilding ) {
	mask=MapMaskShoreBuilding;
    } else switch( CursorBuilding->UnitType ) {
	case UnitTypeLand:
	    mask=MapMaskLandBuilding;
	    break;
	case UnitTypeNaval:
	    mask=MapMaskWaterBuilding;
	    break;
	case UnitTypeFly:
	default:
	    DebugLevel1("Were moves this unit?\n");
	    return;
    }
#else
    // FIXME: Should be moved into unittype structure, and allow more types.
    if( CursorBuilding->ShoreBuilding ) {
	mask=MapFieldLandUnit
		| MapFieldSeaUnit
		| MapFieldBuilding	// already occuppied
		| MapFieldWall
		| MapFieldRocks
		| MapFieldForest	// wall,rock,forest not 100% clear?
		| MapFieldLandAllowed	// can't build on this
		//| MapFieldUnpassable	// FIXME: I think shouldn't be used
		| MapFieldNoBuilding;
    } else switch( CursorBuilding->UnitType ) {
	case UnitTypeLand:
	    mask=MapFieldLandUnit
		| MapFieldBuilding	// already occuppied
		| MapFieldWall
		| MapFieldRocks
		| MapFieldForest	// wall,rock,forest not 100% clear?
		| MapFieldCoastAllowed
		| MapFieldWaterAllowed	// can't build on this
		| MapFieldUnpassable	// FIXME: I think shouldn't be used
		| MapFieldNoBuilding;
	    break;
	case UnitTypeNaval:
	    mask=MapFieldSeaUnit
		| MapFieldBuilding	// already occuppied
		| MapFieldCoastAllowed
		| MapFieldLandAllowed	// can't build on this
		| MapFieldUnpassable	// FIXME: I think shouldn't be used
		| MapFieldNoBuilding;
	    break;
	case UnitTypeFly:
	default:
	    DebugLevel1(__FUNCTION__": Were moves this unit?\n");
	    return;
    }
#endif

    h=CursorBuilding->TileHeight;
    if( my+h>MapY+MapHeight ) {		// reduce to view limits
	h=MapY+MapHeight-my;
    }
    w0=CursorBuilding->TileWidth;	// reduce to view limits
    if( mx+w0>MapX+MapWidth ) {
	w0=MapX+MapWidth-mx;
    }
    while( h-- ) {
	w=w0;
	while( w-- ) {
	    if( f && (CanBuildOn(mx+w,my+h,mask) ||
		    (Selected[0]->X==mx+w && Selected[0]->Y==my+h))
                  && (TheMap.Fields[mx+w+(my+h)*TheMap.Width].Flags
			&MapFieldExplored) ) {
		color=ColorGreen;
	    } else {
		color=ColorRed;
	    }
	    // FIXME: Could do this faster+better
	    for( y1=0; y1<TileSizeY; ++y1 ) {
		for( x1=y1&1; x1<TileSizeX; x1+=2 ) {
		    DrawPointUnclipped(color
			    ,x+w*TileSizeX+x1,y+h*TileSizeY+y1);
		}
	    }
	}
    }
}

/**
**	Draw rectangle cursor.
*/
global void DrawRectangleCursor(void)
{
    int x;
    int y;
    int w;
    int h;
    int x1;
    int y1;

    //
    //	Clip to map window.
    //
    x1=CursorX;
    if( x1<MAP_X ) {
	x1=MAP_X;
    } else if( x1>=MAP_X+MapWidth*TileSizeX ) {
	x1=MAP_X+MapWidth*TileSizeX-1;
    }
    y1=CursorY;
    if( y1<MAP_Y ) {
	y1=MAP_Y;
    } else if( y1>=MAP_Y+MapHeight*TileSizeY ) {
	y1=MAP_Y+MapHeight*TileSizeY-1;
    }

    x=CursorStartX;
    if( x>x1 ) {
	x=x1;
	w=CursorStartX-x;
    } else {
	w=x1-x;
    }
    y=CursorStartY;
    if( y>y1 ) {
	y=y1;
	h=CursorStartY-y;
    } else {
	h=y1-y;
    }

    DrawRectangle(ColorGreen,x,y,w,h);
}

/**
**	Draw the cursor
*/
global void DrawAnyCursor(void)
{
    RectangleCursor=BuildingCursor=0;

    //
    //	Selecting rectangle
    //
    if( CursorState==CursorStateRectangle
	     && (CursorStartX!=CursorX || CursorStartY!=CursorY) ) {
	DrawRectangleCursor();
	RectangleCursor=1;
    } else

    //
    //	Selecting position for building
    //
    if( CursorBuilding && CursorOn==CursorOnMap ) {
	DrawBuildingCursor();
	BuildingCursor=1;
    }

    //
    //	Normal cursor.
    //
    DrawCursor(CloneCursor,CursorX,CursorY,0);
}

/**
**	Remove old cursor from display.
*/
global int HideAnyCursor(void)
{
    if( RectangleCursor || BuildingCursor ) {
	MustRedraw|=RedrawMap;
    }

    //
    //	Cursor complete on map and map must be redrawn, no restore.
    //
    if( OldCursorX>MAP_X
	    && OldCursorX+OldCursorW<MAP_X+MapWidth*TileSizeX
	    && OldCursorY>MAP_Y
	    && OldCursorY+OldCursorH<MAP_Y+MapHeight*TileSizeY ) {
	if( !(MustRedraw&RedrawMap) ) {
	    HideCursor();
	    return 1;
	}
	return 0;
    }
    HideCursor();
    return 1;
}

//@}
