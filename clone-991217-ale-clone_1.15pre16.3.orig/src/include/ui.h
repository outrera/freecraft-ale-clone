/*
**	A clone of a famous game.
*/
/**@name ui.h		-	The user interface header file. */
/*
**	(c) Copyright 1999 by Lutz Sammer
**
**	$Id: ui.h,v 1.5 1999/12/03 09:18:08 root Exp $
*/

#ifndef __UI_H__
#define __UI_H__

//@{

// FIXME: this only the start of the new user interface
// FIXME: all user interface variables should go here and be configurable

/*----------------------------------------------------------------------------
--	Definitons
----------------------------------------------------------------------------*/

/**
**	Defines the SVGALIB mouse speed adjust (must be > 0)
*/
#define MOUSEADJUST 15

#define MOUSESCALE 1

/**
**	Defines the new user interface.
*/
typedef struct _ui_ {
// FIXME: could use different sounds/speach for the errors 
// Is in gamesounds?
//    SoundConfig	PlacementError;		/// played on placements errors
//    SoundConfig	PlacementSuccess;	/// played on placements success
//    SoundConfig	Click;			/// click noice used often

      unsigned		ReverseMouseMove;	/// Middle mouse button map move with reversed directions
      int		WarpX;
      int		WarpY;

      int		MouseAdjust;		/// Mouse speed adjust
      int		MouseScale;		/// Mouse speed scale
} UI;

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

extern UI TheUI;				/// The user interface

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern void UserInterfaceCclRegister(void);	/// register ccl features

//@}

#endif	// !__UI_H__
