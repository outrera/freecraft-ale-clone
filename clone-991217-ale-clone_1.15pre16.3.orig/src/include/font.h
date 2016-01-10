/*
**	A clone of a famous game.
*/
/**@name font.h		-	The font headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: font.h,v 1.4 1999/07/08 21:09:19 root Exp $
*/

#ifndef __FONT_H__
#define __FONT_H__

//@{

/*----------------------------------------------------------------------------
--	Definitions
----------------------------------------------------------------------------*/

/// FIXME: Docu
typedef struct _color_font_ {
    char*	File;			/// file containing font data

    int		Width;
    int		Height;

    char	CharWidth[208];

// --- FILLED UP ---

    Sprite	Sprite;
} ColorFont;

/// FIXME: Docu
enum _game_font_ {
    SMALL_FONT,
    GAME_FONT,
    LARGE_FONT,
};

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

    /// FIXME: docu
extern ColorFont Fonts[];
extern int DefaultReverseText;		/// default reverse state

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

    /// FIXME: docu
extern void DrawText(int x,int y,int font,const unsigned char* text);
extern int TextLength(int font,const unsigned char* text);
extern void DrawTextCentered(int x,int y,int font,const unsigned char* text);
extern void DrawNumber(int x,int y,int font,int number);
extern void LoadFonts(void);

//@}

#endif	// !__FONT_H__
