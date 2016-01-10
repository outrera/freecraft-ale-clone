/*
**	A clone of a famous game.
*/
/**@name minimap.h	-	The minimap headerfile. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: minimap.h,v 1.6 1999/06/19 22:21:51 root Exp $
*/

#ifndef __MINIMAP_H__
#define __MINIMAP_H__

//@{

/*----------------------------------------------------------------------------
--	Minimap
----------------------------------------------------------------------------*/

#define MINIMAP_FAC	(16*3)		/// integer scale factor

/**
**	Convert minimap cursor X position to map coordinate.
*/
#define Minimap2MapX(x)	\
    ((((x)-MINIMAP_X-MinimapX)*MINIMAP_FAC)/MinimapScale)

/**
**	Convert minimap cursor Y position to map coordinate.
*/
#define Minimap2MapY(y)	\
    ((((y)-MINIMAP_Y-MinimapY)*MINIMAP_FAC)/MinimapScale)

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

extern int MinimapScale;		/// Minimap scale to fit into window
extern int MinimapX;			/// Minimap drawing position x offset.
extern int MinimapY;			/// Minimap drawing position y offset.

extern int MinimapWithTerrain;		/// display minimap with terrain
extern int MinimapFriendly;		/// switch colors of friendly units
extern int MinimapShowSelected;		/// highlight selected units

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern void UpdateMinimapXY(int tx,int ty);	///
extern void UpdateMinimap(void);
extern void CreateMinimap(void);
extern void DrawMinimap(int vx,int vy);
extern void HideMinimapCursor(void);
extern void DrawMinimapCursor(int vx,int vy);

//@}

#endif	// !__MINIMAP_H__
