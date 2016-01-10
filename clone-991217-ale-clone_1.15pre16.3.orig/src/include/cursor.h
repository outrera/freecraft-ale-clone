/*
**	A clone of a famous game.
*/
/**@name cursor.h	-	The cursors header file. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: cursor.h,v 1.7 1999/06/19 22:21:49 root Exp $
*/

#ifndef __CURSOR_H__
#define __CURSOR_H__

//@{

/*----------------------------------------------------------------------------
--	Definitions
----------------------------------------------------------------------------*/

    /// cursor type typedef
typedef struct _cursor_type_ CursorType;

    /// private type which specifies current cursor type
struct _cursor_type_ {
	/// resource filename one for each race
    const char*	File[PlayerMaxRaces];

// FIXME: this must be extra for each file (different sizes for the races)
// FIXME: or must define that each image has the same size
	/// hot point x
    int		HotX;
	/// hot point y
    int		HotY;
	/// width of cursor
    int		Width;
	/// height of cursor
    int		Height;

	/// sprite image of cursor : FILLED
    RleSprite*	RleSprite;
};

    /// cursor type (enumerated)
enum CursorType_e {
    CursorTypePoint = 0,
    CursorTypeGlass,
    CursorTypeCross,
    CursorTypeYellowHair,
    CursorTypeGreenHair,
    CursorTypeRedHair,
    CursorTypeMove,
    CursorTypeArrowE,
    CursorTypeArrowN,
    CursorTypeArrowNE,
    CursorTypeArrowNW,
    CursorTypeArrowS,
    CursorTypeArrowSE,
    CursorTypeArrowSW,
    CursorTypeArrowW,
    CursorMax
};

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

    /// cursor types description
extern CursorType Cursors[CursorMax];

extern int OldCursorX;			// last cursor data
extern int OldCursorY;
extern int OldCursorW;
extern int OldCursorH;

extern int CursorAction;		// action for selection
extern UnitType* CursorBuilding;	// building cursor

    /// current cursor type (shape)
extern CursorType* CloneCursor;
extern int CursorX;			// cursor position
extern int CursorY;
extern int CursorStartX;		// rectangle started
extern int CursorStartY;

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

    /// initialize all cursor
extern void LoadCursors(void);

    /** Draw cursor on screen in position x,y..
    **	@param type	cursor type pointer
    **	@param x	x coordinate on the screen
    **	@param y	y coordinate on the screen
    **	@param frame	sprite animation frame
    */
extern void DrawCursor(CursorType* type,int x,int y,int frame);

    /// hide the cursor
extern void HideCursor(void);

    /// draw any cursor 
extern void DrawAnyCursor(void);

    /// hide any cursor 
extern int HideAnyCursor(void);

//@}

#endif	// !__CURSOR_H__
