/*
**	A clone of a famous game.
*/
/**@name image.c	-	The standard images. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: image.c,v 1.15 2000/01/16 23:38:08 root Exp $
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
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "image.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

global Sprite MenubuttonSprite;

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Constant graphics
*/
struct _images_ {
    char*	File[PlayerMaxRaces];	/// one file for each race
    unsigned	Width;			/// graphic size width
    unsigned	Height;			/// graphic size height

// --- FILLED UP ---
    OldGraphic	Image;			/// graphic image (filled)
} Images[] = {
    { { "Map border (Top,Humans).png"
	,"Map border (Top,Orcs).png" }
		,448,16 },
    { { "Map border (Bottom,Humans).png"
	,"Map border (Bottom,Orcs).png" }
		,448,16 },
    { { "Map border (Right,Humans).png"
	,"Map border (Right,Orcs).png" }
		,16,480 },
    { { "Minimap border (Top,Humans).png"
	,"Minimap border (Top,Orcs).png" }
		,176,24 },
    { { "Minimap (Humans).png"
	,"Minimap (Orcs).png" }
		,176,136 },
    { { "human panel.png"
	,"orcish panel.png" }
		,176,176 },
    { { "Panel (Bottom,Humans).png"
	,"Panel (Bottom,Orcs).png" }
		,176,144 },
    { { "gold,wood,oil,mana.png"
	,NULL }
		, 14, 14 },
    { { "bloodlust,haste,slow,invis.,shield.png"
	,NULL }
		, 16, 16 },
    { { "interface/buttons 1.png"
	,"interface/buttons 2.png" }
		, 300, 7632 },
    { { "interface/panel 1 (humans).png"
	,"interface/panel 1 (orcs).png" }
		, 256, 288 },
    { { "interface/panel 2 (humans).png"
	,"interface/panel 2 (orcs).png" }
		, 288, 256 },
    { { "interface/panel 3 (humans).png"
	,"interface/panel 3 (orcs).png" }
		, 384, 256 },
    { { "interface/panel 4 (humans).png"
	,"interface/panel 4 (orcs).png" }
		, 288, 128 },
    { { "interface/panel 5 (humans).png"
	,"interface/panel 5 (orcs).png" }
		, 352, 352 },
};

/**
**	Draw image on screen.
**
**	@param image	Image number (=index).
**	@param row	Image row
**	@param frame	Image frame
**	@param X	X position.
**	@param Y	Y position.
*/
global void DrawImage(int image,int row,int frame,int x,int y)
{
    DrawOldGraphic(&Images[image].Image
	,frame*Images[image].Width,row*Images[image].Height
	,Images[image].Width,Images[image].Height,x,y);
}

/**
**	Load all images.
*/
global void LoadImages(void)
{
    int i;
    const char* file;
    char* buf;

    //
    //	Load all images, depends on the race of the player on this computer.
    //
    for( i=0; i<sizeof(Images)/sizeof(*Images); ++i ) {
	file=Images[i].File[ThisPlayer->Race];
	if( !file ) {			// default one
	    file=Images[i].File[0];
	}

	buf=alloca(strlen(file)+9+1);
	file=strcat(strcpy(buf,"graphic/"),file);
	ShowLoadProgress("\tImage %s\n",file);
	LoadOldGraphic(file,&Images[i].Image);
    }

    //
    //	Preset the menu button sprite with the just loaded image.
    //
    MenubuttonSprite.Width=Images[ImageButtons].Image.Width;
    MenubuttonSprite.Height=Images[ImageButtons].Image.Height;
    MenubuttonSprite.Data=Images[ImageButtons].Image.Data;
}

//@}
