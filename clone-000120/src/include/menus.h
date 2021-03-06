/*
**	A clone of a famous game.
*/
/**@name menus.h	-	The menu headerfile. */
/*
**	(c) Copyright 1999,2000 by Andreas Arens
**
**	$Id: menus.h,v 1.1 2000/01/16 23:37:56 root Exp $
*/

#ifndef __MENUS_H__
#define __MENUS_H__

//@{

/*----------------------------------------------------------------------------
--	Menubuttons
----------------------------------------------------------------------------*/

#define MenuButtonActive	1	/// cursor on button
#define MenuButtonClicked	2	/// mouse button down on button
#define MenuButtonSelected	4	/// selected button
#define MenuButtonDisabled	8	/// button cannot be depressed

#define MENUBUTTONHEIGHT 144
#define MENUBUTTONWIDTH 300
/**
**	Menu button referencing
**	Each button is 300 x 144  =>	53 buttons
**	For multi-version buttons: button - 1 == disabled, + 1 == depressed
*/
typedef unsigned MenuButtonId;

/// FIXME: FILL IN THIS TABLE!!!!

#define MBUTTON_MAIN	4
#define MBUTTON_GM_HALF	10
#define MBUTTON_GM_FULL	16

/*----------------------------------------------------------------------------
--	Menus
----------------------------------------------------------------------------*/

/**
**	Menuitem definition.
*/
typedef struct _menuitem_ {
    int mitype;
    union {
	struct {
	    unsigned xofs;
	    unsigned yofs;
	    unsigned flags;
	    int font;
	    unsigned char *text;
	} text;
	struct {
	    unsigned xofs;
	    unsigned yofs;
	    unsigned flags;
	    int font;
	    unsigned char *text;
	    unsigned xsize;
	    unsigned ysize;
	    MenuButtonId button;
	    int hotkey;
	    void (*handler)(void);
	} button;
	/// ... add here ...
    } d;
} Menuitem;

#define MI_TYPE_TEXT 1
#define MI_TYPE_BUTTON 2

    /// for MI_TYPE_TEXT
#define MI_FLAGS_CENTERED 1

/**
**	Menu definition.
*/
typedef struct _menus_ {
    unsigned x;				/// panel x pos
    unsigned y;				/// panel y pos
    int	image;				/// panel image
    enum MustRedraw_e area;		/// invalidate area for redraws
    int nitems;				/// number of items to follow
    Menuitem items[0];			/// buttons, etc
} Menu;


#define MENU_GAME 0

/// FIXME: FILL IN THIS TABLE!!!!

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

    /// draw menu button
extern void DrawMenuButton(MenuButtonId button,unsigned flags,unsigned w,unsigned h,unsigned x,unsigned y,const int font,const unsigned char *text);
    /// Draw and process a menu
extern void ProcessMenu(int MenuId);
    /// Keyboard handler for menus
extern int MenuKey(int key);
    /// Called if the mouse is moved in Menu interface state
extern void MenuHandleMouseMove(int x,int y);
    /// Called if any mouse button is pressed down
extern void MenuHandleButtonDown(int b);
    /// Called if any mouse button is released up
extern void MenuHandleButtonUp(int b);
    /// Called if the mouse is moved


//@}

#endif	// !__MENUS_H__
