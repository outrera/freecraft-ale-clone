/*
**	A clone of a famous game.
*/
/**@name icons.h	-	The icons headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: icons.h,v 1.13 1999/11/21 21:59:49 root Exp $
*/

#ifndef __ICONS_H__
#define __ICONS_H__

//@{

/*----------------------------------------------------------------------------
--	Icons
----------------------------------------------------------------------------*/

#define IconActive	1		/// cursor on icon
#define IconClicked	2		/// mouse button down on icon
#define IconSelected	4		/// this the selected icon

#define ICON_WIDTH	46		/// icon width in panels
#define ICON_HEIGHT	38		/// icon height in panels

/*----------------------------------------------------------------------------
--	Icons names for the slots
----------------------------------------------------------------------------*/

/**
**	Icon referencing
*/
typedef unsigned IconId;

#define NoIcon	-1			/// used for errors no valid icon

/**
**      Icon definition
*/
typedef struct _icon_config_ {
    char*	Name;			/// config icon name
    IconId	Icon;			/// identifier to use to run time
} IconConfig;

#define IconMax			195	/// Maximum used icon number

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern void LoadIcons(void);				/// load icons
extern void CleanIcons(void);				/// cleanup

extern IconId IconByIdent(const char* ident);		/// name -> icon
extern const char* IdentOfIcon(IconId icon);		/// icon -> name

    /// draw icons of an unit
extern void DrawUnitIcon(IconId num,unsigned flags,unsigned x,unsigned y);

//@}

#endif	// !__ICONS_H__
