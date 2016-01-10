/*
**	A clone of a famous game.
*/
/**@name menubuttons.c	-	The menu buttons. */
/*
**	(c) Copyright 1999 by Andreas Arens
**
**	$Id: menus.c,v 1.1 2000/01/16 23:38:08 root Exp $
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
#include "player.h"
#include "image.h"
#include "font.h"
#include "tileset.h"
#include "interface.h"
#include "menus.h"
#include "cursor.h"
#include "new_video.h"


/*----------------------------------------------------------------------------
--	Prototypes for action handlers
----------------------------------------------------------------------------*/

local void GameMenuSave(void);
local void GameMenuEnd(void);
local void GameMenuReturn(void);


/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	The currently process menu
*/
local int CurrentMenu = -1;
local int MenuButtonUnderCursor = -1;

global Menu Menus[] = {
    {
	/// Game Menu
	MAP_X+(DEFAULT_MAP_WIDTH*TileSizeX-256)/2,
	MAP_Y+(DEFAULT_MAP_HEIGHT*TileSizeY-288)/2,
	ImagePanel1,
	RedrawMapOverlay,
	8,
	{
	    { MI_TYPE_TEXT, { text:{ 128, 11, MI_FLAGS_CENTERED, LARGE_FONT, "Game Menu"} } },
	    { MI_TYPE_BUTTON, { button:{ 16, 40, MenuButtonDisabled, LARGE_FONT,
		 "Save (~F~1~1)", 106, 27, MBUTTON_GM_HALF, KeyCodeF11, GameMenuSave} } },
	    { MI_TYPE_BUTTON, { button:{ 16 + 12 + 106, 40, MenuButtonDisabled, LARGE_FONT,
		 "Load (~F~1~2)", 106, 27, MBUTTON_GM_HALF, KeyCodeF12, NULL} } },
	    { MI_TYPE_BUTTON, { button:{ 16, 40 + 36, MenuButtonDisabled, LARGE_FONT,
		 "Options (~F~5)", 224, 27, MBUTTON_GM_FULL, KeyCodeF5, NULL} } },
	    { MI_TYPE_BUTTON, { button:{ 16, 40 + 36 + 36, MenuButtonDisabled, LARGE_FONT,
		 "Help (~F~1)", 224, 27, MBUTTON_GM_FULL, KeyCodeF1, NULL} } },
	    { MI_TYPE_BUTTON, { button:{ 16, 40 + 36 + 36 + 36, MenuButtonDisabled, LARGE_FONT,
		 "Scenario ~Objectives", 224, 27, MBUTTON_GM_FULL, 'o', NULL} } },
	    { MI_TYPE_BUTTON, { button:{ 16, 40 + 36 + 36 + 36 + 36, 0, LARGE_FONT,
		 "~End Scenario", 224, 27, MBUTTON_GM_FULL, 'e', GameMenuEnd} } },
	    { MI_TYPE_BUTTON, { button:{ 16, 288-40, MenuButtonSelected, LARGE_FONT,
		 "Return to Game (~E~s~c)", 224, 27, MBUTTON_GM_FULL, '\033', GameMenuReturn} } },
	},
    },
};

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Draw menu button 'button' on x,y
**
**	@param button	Button identifier
**	@param flags	State of Button (clicked, mouse over...)
**	@param w	Button width (for border)
**	@param h	Button height (for border)
**	@param x	X display position
**	@param y	Y display position
**	@param font	font number for text
**	@param text	text to print on button
*/
global void DrawMenuButton(MenuButtonId button,unsigned flags,unsigned w,unsigned h,unsigned x,unsigned y,
	const int font,const unsigned char *text)
{
    MenuButtonId rb;
    unsigned char *sp, *dp, *tbuf;
    int s = 0;

    if (flags&MenuButtonDisabled)
	rb = button - 1;
    else if (flags&MenuButtonClicked)
	rb = button + 1;
    else
	rb = button;

    if (text && ((flags&MenuButtonActive) || (flags&MenuButtonClicked) || (flags&MenuButtonDisabled))) {
	dp = tbuf = alloca(strlen(text)+1);
	sp = (unsigned char *)text;
	while (*sp) {
	    if (*sp != '~')
		*dp++ = *sp;
	    sp++;
	}
	*dp = 0;
	/// FIXME: Define and use GCharColors in font.c for grey text on diabled buttons!
	DefaultReverseText=!DefaultReverseText;
    } else
	tbuf = (unsigned char *)text;

    if( (flags&MenuButtonClicked) ) {
	s = 2;
    }
    DrawSprite(&MenubuttonSprite, 0, MENUBUTTONHEIGHT * rb, MENUBUTTONWIDTH, MENUBUTTONHEIGHT, x, y);
    if (tbuf) {
	DrawTextCentered(s+x+w/2,s+y+(font == SMALL_FONT ? 4 : 7),font,tbuf);
	if (((flags&MenuButtonActive) || (flags&MenuButtonClicked) || (flags&MenuButtonDisabled))) {
	    /// FIXME: Define and use GCharColors in font.c for grey text on diabled buttons!
	    DefaultReverseText=!DefaultReverseText;
	}
    }
    if (flags&MenuButtonSelected) {
	DrawRectangle(ColorYellow,x,y,w,h);
    }
}


/**
**	Draw menu  'menu'
**
**	@param Menu	The menu number to display
*/
local void DrawMenu(int MenuId)
{
    int i;
    Menu *menu = Menus + MenuId;
    Menuitem *mi;
    
    HideAnyCursor();
    if (menu->image != ImageNone) {
	DrawImage(menu->image,0,0,menu->x,menu->y);
    }
    for (i = 0; i < menu->nitems; i++) {
	mi = &menu->items[i];
	switch (mi->mitype) {
	    case MI_TYPE_TEXT:
		if (mi->d.text.flags&MI_FLAGS_CENTERED)
		    DrawTextCentered(menu->x+mi->d.text.xofs,menu->y+mi->d.text.yofs,
			    mi->d.text.font,mi->d.text.text);
		else
		    DrawText(menu->x+mi->d.text.xofs,menu->y+mi->d.text.yofs,
			    mi->d.text.font,mi->d.text.text);
		break;
	    case MI_TYPE_BUTTON:
		    DrawMenuButton(mi->d.button.button,mi->d.button.flags,
			    mi->d.button.xsize,mi->d.button.ysize,
			    menu->x+mi->d.button.xofs,menu->y+mi->d.button.yofs,
			    mi->d.button.font,mi->d.button.text);
		break;
	    default:
		break;
	}
    }
    MustRedraw |= menu->area;
    DrawAnyCursor();
}

/*----------------------------------------------------------------------------
--	Button action handler functions
----------------------------------------------------------------------------*/

local void GameMenuReturn(void)
{
    ClearStatusLine();
    InterfaceState=IfaceStateNormal;
    MustRedraw&=~RedrawMapOverlay;
    MustRedraw|=RedrawMap;
    GamePaused=0;
    CursorOn=CursorOnUnknown;
    /// FIXME: restore mouse pointer to sane state (call fake mouse move?)
}

local void GameMenuSave(void)
{
    SaveAll();	/// FIXME: Sample code
}

local void GameMenuEnd(void)
{
    Exit(0);
}

/**
**	Handle keys in menu mode.
**
**	@param key	Key scancode.
**	@return		True, if key is handled; otherwise false.
*/
global int MenuKey(int key)
{
    int i;
    Menuitem *mi;
    Menu *menu = Menus + CurrentMenu;

    i = menu->nitems;
    mi = menu->items;
    while (i--) {
	switch (mi->mitype) {
	    case MI_TYPE_BUTTON:
		if (key == mi->d.button.hotkey) {
		    if (mi->d.button.handler) {
			(*mi->d.button.handler)();
		    }
		    return 1;
		}
	    default:
		break;
	}
	mi++;
    }
    /// FIXME: ADD <RETURN-KEY> HANDLER HERE!
    switch (key) {
	case 'q':
	    Exit(0);
	default:
	    DebugLevel3("Key %d\n",key);
	    return 0;
    }
    return 1;
}


/**
**	Handle movement of the cursor.
**
**	@param x	Screen X position.
**	@param y	Screen Y position.
*/
global void MenuHandleMouseMove(int x,int y)
{
    int i, n, xs, ys;
    Menuitem *mi;
    Menu *menu = Menus + CurrentMenu;
    int RedrawFlag = 0;

    n = menu->nitems;
    MenuButtonUnderCursor = -1;
    for (i = 0; i < n; ++i) {
	mi = menu->items + i;
	switch (mi->mitype) {
	    case MI_TYPE_BUTTON:
		if (!(mi->d.button.flags&MenuButtonDisabled)) {
		    xs = menu->x + mi->d.button.xofs;
		    ys = menu->y + mi->d.button.yofs;
		    if (x < xs || x > xs + mi->d.button.xsize || y < ys || y > ys + mi->d.button.ysize) {
			if (!(mi->d.button.flags&MenuButtonClicked)) {
			    if (mi->d.button.flags&MenuButtonActive) {
				RedrawFlag = 1;
				mi->d.button.flags &= ~MenuButtonActive;
			    }
			}
			continue;
		    }
		    if (!(mi->d.button.flags&MenuButtonActive)) {
			RedrawFlag = 1;
			mi->d.button.flags |= MenuButtonActive;
		    }
		    DebugLevel3("On menu button %d\n", i);
		    MenuButtonUnderCursor = i;
		}
		break;
	    default:
		break;
	}
    }
    if (RedrawFlag) {
	DrawMenu(CurrentMenu);
    }
}

/**
**	Called if mouse button pressed down.
**
**	@param b	button code
*/
global void MenuHandleButtonDown(int b)
{
    Menuitem *mi;

    if (MouseButtons&LeftButton) {
	if (MenuButtonUnderCursor != -1) {
	    mi = Menus[CurrentMenu].items + MenuButtonUnderCursor;
	    if (!(mi->d.button.flags&MenuButtonClicked)) {
		    mi->d.button.flags |= MenuButtonClicked; /// FIXME: | MenuButtonSelected (like original!)
		DrawMenu(CurrentMenu);
	    }
	}
    }
}

/**
**	Called if mouse button released.
**
**	@param b	button code
*/
global void MenuHandleButtonUp(int b)
{
    int i, n;
    Menuitem *mi;
    Menu *menu = Menus + CurrentMenu;
    int RedrawFlag = 0;

    if ((1<<b) == LeftButton) {
	n = menu->nitems;
	for (i = 0; i < n; ++i) {
	    mi = menu->items + i;
	    switch (mi->mitype) {
		case MI_TYPE_BUTTON:
		    if (mi->d.button.flags&MenuButtonClicked) {
			RedrawFlag = 1;
			mi->d.button.flags &= ~MenuButtonClicked;
			if (MenuButtonUnderCursor == i) {
			    MenuButtonUnderCursor = -1;
			    if (mi->d.button.handler) {
				(*mi->d.button.handler)();
			    }
			}
		    }
		    break;
		default:
		    break;
	    }
	}
    }
    if (RedrawFlag) {
	DrawMenu(CurrentMenu);
    }
}


/**
**	Process menu  'menu'
**
**	@param Menu	The menu number to process
*/
global void ProcessMenu(int MenuId)
{
    /// FIXME: Recursion!

    HideCursor();
    CursorState = CursorStatePoint;
    CloneCursor = &Cursors[CursorTypePoint];
    CurrentMenu = MenuId;
    DrawMenu(CurrentMenu);
}

//@}
