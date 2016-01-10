/*
**	A clone of a famous game.
*/
/**@name image.h	-	The standard images headerfile. */
/*
**	(c) Copyright 1998,1999,2000 by Lutz Sammer
**
**	$Id: image.h,v 1.6 2000/01/16 23:37:52 root Exp $
*/

#ifndef __IMAGE_H__
#define __IMAGE_H__

//@{

/*----------------------------------------------------------------------------
--	Images
----------------------------------------------------------------------------*/

/*
**	Constant graphics
*/
#define ImageNone		(-1)

#define ImageTopBorder		0
#define ImageBottomBorder	1
#define ImageRightBorder	2
#define ImageMinimapTop		3
#define ImageMinimap		4
#define ImagePanelTop		5
#define ImagePanelBottom	6
#define ImageResources		7
#define ImageAttributes		8

#define ImageButtons		9
#define ImagePanel1		10
#define ImagePanel2		11
#define ImagePanel3		12
#define ImagePanel4		13
#define ImagePanel5		14


/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

extern Sprite MenubuttonSprite;		/// Sprite data for menu buttons

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern void DrawImage(int image,int row,int frame,int x,int y);
extern void LoadImages(void);

//@}

#endif	// !__IMAGE_H__
