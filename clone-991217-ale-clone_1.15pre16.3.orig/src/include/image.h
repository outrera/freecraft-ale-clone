/*
**	A clone of a famous game.
*/
/**@name image.h	-	The standard images headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: image.h,v 1.4 1999/06/19 22:21:50 root Exp $
*/

#ifndef __IMAGE_H__
#define __IMAGE_H__

//@{

/*============================================================================
==	IMAGE
============================================================================*/

/*
**	Constant graphics
*/
#define ImageTopBorder		0
#define ImageBottomBorder	1
#define ImageRightBorder	2
#define ImageMinimapTop		3
#define ImageMinimap		4
#define ImagePanelTop		5
#define ImagePanelBottom	6
#define ImageResources		7
#define ImageAttributes		8
//#define ImageShadows		9

extern void DrawImage(int image,int row,int frame,int x,int y);
extern void LoadImages(void);

//@}

#endif	// !__IMAGE_H__
