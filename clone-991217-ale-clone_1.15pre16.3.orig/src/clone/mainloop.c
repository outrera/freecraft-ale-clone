/*
**	A clone of a famous game.
*/
/**@name mainloop.c	-	The main game loop. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: mainloop.c,v 1.13 1999/12/09 16:27:08 root Exp $
*/

//@{

#include <stdio.h>

#include "clone.h"
#include "video.h"
#include "tileset.h"
#include "map.h"
#include "image.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "cursor.h"
#include "minimap.h"
#include "actions.h"
#include "missile.h"
#include "interface.h"
#include "network.h"
#include "goal.h"

/**
**	Handle mouse in scroll area.
*/
local void DoScrollArea(void)
{
    switch( CursorOn ) {
	case CursorOnScrollUp:
	    if( MapY ) {
		--MapY;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		CloneCursor=&Cursors[CursorTypeArrowN];
	    }
	    break;
	case CursorOnScrollDown:
	    if( MapY<TheMap.Height-MapHeight ) {
		++MapY;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		CloneCursor=&Cursors[CursorTypeArrowS];
	    }
	    break;
	case CursorOnScrollLeft:
	    if( MapX ) {
		--MapX;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		CloneCursor=&Cursors[CursorTypeArrowW];
	    }
	    break;
	case CursorOnScrollLeftUp:
	    if( MapX ) {
		--MapX;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		CloneCursor=&Cursors[CursorTypeArrowW];
	    }
	    if( MapY ) {
		--MapY;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		if( CloneCursor==&Cursors[CursorTypeArrowW] ) {
		    CloneCursor=&Cursors[CursorTypeArrowNW];
		} else {
		    CloneCursor=&Cursors[CursorTypeArrowN];
		}
	    }
	    break;
	case CursorOnScrollLeftDown:
	    if( MapX ) {
		--MapX;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		CloneCursor=&Cursors[CursorTypeArrowW];
	    }
	    if( MapY<TheMap.Height-MapHeight ) {
		++MapY;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		if( CloneCursor==&Cursors[CursorTypeArrowW] ) {
		    CloneCursor=&Cursors[CursorTypeArrowSW];
		} else {
		    CloneCursor=&Cursors[CursorTypeArrowS];
		}
	    }
	    break;
	case CursorOnScrollRight:
	    if( MapX<TheMap.Width-MapWidth ) {
		++MapX;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		CloneCursor=&Cursors[CursorTypeArrowE];
	    }
	    break;
	case CursorOnScrollRightUp:
	    if( MapX<TheMap.Width-MapWidth ) {
		++MapX;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		CloneCursor=&Cursors[CursorTypeArrowE];
	    }
	    if( MapY ) {
		--MapY;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		if( CloneCursor==&Cursors[CursorTypeArrowE] ) {
		    CloneCursor=&Cursors[CursorTypeArrowNE];
		} else {
		    CloneCursor=&Cursors[CursorTypeArrowN];
		}
	    }
	    break;
	case CursorOnScrollRightDown:
	    if( MapX<TheMap.Width-MapWidth ) {
		++MapX;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		CloneCursor=&Cursors[CursorTypeArrowE];
	    }
	    if( MapY<TheMap.Height-MapHeight ) {
		++MapY;
		MustRedraw|=RedrawMaps|RedrawCursor|RedrawMinimapCursor;
		if( CloneCursor==&Cursors[CursorTypeArrowE] ) {
		    CloneCursor=&Cursors[CursorTypeArrowSE];
		} else {
		    CloneCursor=&Cursors[CursorTypeArrowS];
		}
	    }
	    break;
	default:
	    break;
    }
}

/**
**	Display update.
*/
local void UpdateDisplay(void)
{
    int update_old_cursor;

    update_old_cursor=HideAnyCursor();	// remove cursor

    if( MustRedraw&RedrawMap ) {
	int i;

	for( i=0; i<MapHeight; ++i ) {
	    MustRedrawRow[i]=1;
	}
	for( i=0; i<MapHeight*MapWidth; ++i ) {
	    MustRedrawTile[i]=1;
	}

	SetClipping(MAP_X,MAP_Y
		,MAP_X+MapWidth*TileSizeX,MAP_Y+MapHeight*TileSizeY);
	DrawMapBackground(MapX,MapY);
	DrawUnits();
	DrawMapFogOfWar(MapX,MapY);
	DrawMissiles();
	SetClipping(0,0,VideoWidth,VideoHeight);
    }

    if( MustRedraw&(RedrawMessage|RedrawMap) ) {
	DrawMessage();
    }

    if( MustRedraw&RedrawRightBorder ) {
	DrawImage(ImageRightBorder,0,0,MAP_X+MapWidth*TileSizeX,0);
    }

    if( MustRedraw&RedrawMinimapTop ) {
	DrawImage(ImageMinimapTop,0,0,0,0);
    }
    if( MustRedraw&RedrawMinimapBorder ) {
	DrawImage(ImageMinimap,0,0,0,24);
    }

#if 1
    if( MustRedraw&RedrawMinimap ) {
	// FIXME: redraw only 1* per second!
	// HELPME: Viewpoint rectangle must be drawn faster (if implemented) ?
	PlayerPixels(Players);		// why color 0?
	DrawMinimap(MapX,MapY);
	DrawMinimapCursor(MapX,MapY);
    } else if( MustRedraw&RedrawMinimapCursor ) {
	HideMinimapCursor();
	DrawMinimapCursor(MapX,MapY);
    }
#endif

    if( MustRedraw&RedrawTopPanel ) {
	DrawTopPanel();
    }
    if( MustRedraw&RedrawBottomPanel ) {
	PlayerPixels(ThisPlayer);
	DrawBottomPanel();
    }
    if( MustRedraw&RedrawResources ) {
	DrawResources();
    }
    if( MustRedraw&RedrawStatusLine ) {
	DrawStatusLine();
	MustRedraw|=RedrawCosts;
    }
    if( MustRedraw&RedrawCosts ) {
	DrawCosts();
    }

    // FIXME: this could be written better, less drawing
    if( update_old_cursor && MustRedraw!=-1  ) {
	InvalidateArea(OldCursorX,OldCursorY,OldCursorW,OldCursorH);
    }

    DrawAnyCursor();

    //
    //	Update changes to X11.
    //
    if( MustRedraw==-1 ) {
	Invalidate();
    } else {
	if( MustRedraw&RedrawMap ) {
	    InvalidateArea(MAP_X,MAP_Y,MapWidth*TileSizeX,MapHeight*TileSizeY);
	}
	if( MustRedraw&RedrawRightBorder ) {
	    InvalidateArea(MAP_X+MapWidth*TileSizeX,0
		    ,VideoWidth-MAP_X-MapWidth*TileSizeX,VideoHeight);
	}
	if( MustRedraw&RedrawMinimapTop ) {
	    InvalidateArea(0,0,176,24);
	}
	if( MustRedraw&RedrawMinimapBorder ) {
	    InvalidateArea(0,24,176,136);
	} else if( (MustRedraw&RedrawMinimap)
		|| (MustRedraw&RedrawMinimapCursor) ) {
	    // FIXME: Redraws too much!
	    InvalidateArea(MINIMAP_X,MINIMAP_Y,MINIMAP_W,MINIMAP_H);
	}
	if( MustRedraw&RedrawTopPanel ) {
	    InvalidateArea(0,24+136,176,176);
	}
	if( MustRedraw&RedrawBottomPanel ) {
	    InvalidateArea(0,24+136+176,176,144);
	}
	if( MustRedraw&RedrawResources ) {
	    InvalidateArea(MAP_X,0,VideoWidth-MAP_X,16);
	}
	if( MustRedraw&RedrawStatusLine || MustRedraw&RedrawCosts ) {
	    InvalidateArea(MAP_X,MAP_Y+MapHeight*TileSizeY
		    ,VideoWidth-MAP_X,VideoHeight-MAP_Y-MapHeight*TileSizeY);
	}
	InvalidateArea(CursorX-CloneCursor->HotX,CursorY-CloneCursor->HotY
		,CloneCursor->RleSprite->Width,CloneCursor->RleSprite->Height);
    }
}

/**
**	Main loop.
**
**	Unit actions.
**	Missile actions.
**	Players (AI).
**	Cyclic events (color cycle,...)
**	Display update.
**	Input/Network/Sound.
*/
global void CloneMainLoop(void)
{
    InitVideoSync();
    MustRedraw=RedrawEverything;
    CloneCursor=&Cursors[CursorTypePoint];

    for( ;; ) {
	if(!GamePaused) {
	    ++FrameCounter;
	    UnitActions();		// handle units
	    MissileActions();		// handle missiles
	    PlayersEachFrame();		// handle players

	    MustRedraw&=~RedrawMinimap;	// FIXME: this a little hack!

	    /*
	    **	Called each second. Split into different frames.
	    **		Increment mana of magic units.
	    **		Update mini-map.
	    **		Update map fog of war.
	    **		Call AI.
	    **		Check game goals.
	    */
	    switch( FrameCounter%FRAMES_PER_SECOND ) {
		case 0:
		    UnitIncrementMana();	// magic units
		    break;
		case 1:
		    //UnitIncrementHealth();// berserker healing
		    break;
		case 2:
		    MapUpdateVisible();
		    break;
		case 3:
		    MustRedraw|=RedrawMinimap;
		    break;
		case 4:
		    PlayersEachSecond();
		    break;
		case 5:
		    CheckGoals();
		    break;
	    }
	}
	if( !(FrameCounter%MOUSE_SCROLL_SPEED) ) {
	    DoScrollArea();
	}

	if( !(FrameCounter%COLOR_CYCLE_SPEED) ) {
	    ColorCycle();
	}

	if( MustRedraw /* && !VideoInterrupts */ ) {
	    UpdateDisplay();
	    //
	    // If double-buffered mode, we will display the contains of
	    // VideoMemory. If direct mode this does nothing. In X11 it does
	    // XFlush
	    //
	    RealizeVideoMemory();
	    MustRedraw=0;
	}

	CheckVideoInterrupts();

	WaitEventsAndKeepSync();

	NetworkSync();

#if 0
	//
	//	Sync:	not needed done by DoEvent
	//
	while( VideoSyncSpeed && VideoInterrupts<1 ) {
	    sigpause(0);
	}
#endif

	VideoInterrupts=0;
    }
}

//@}
