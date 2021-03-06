/*
**	A clone of a famous game.
*/
/**@name construct.c	-	The constructions. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: construct.c,v 1.10 1999/11/21 21:59:58 root Exp $
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
#include "tileset.h"
#include "map.h"
#include "construct.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

#define DEFAULT	NULL

/**
**	Constructions.
*/
local Construction Constructions[] = {
{ { "",
  DEFAULT,
  DEFAULT,
  DEFAULT },
	0,0 },
{ { "",
  DEFAULT,
  DEFAULT,
  DEFAULT },
	0,0 },
{ { "",
  DEFAULT,
  DEFAULT,
  DEFAULT },
	0,0 },
{ { "",
  DEFAULT,
  DEFAULT,
  DEFAULT },
	0,0 },
{ { "",
  DEFAULT,
  DEFAULT,
  DEFAULT },
	0,0 },
{ { "",
  DEFAULT,
  DEFAULT,
  DEFAULT },
	0,0 },
{ { "land construction site (summer,wasteland).png",
  "land construction site (winter).png",
  DEFAULT,
  DEFAULT },
	64,64 },
{ { "human shipyard construction site (summer,wasteland).png",
  "human shipyard construction site (winter).png",
  DEFAULT,
  "human shipyard construction site (swamp).png" },
	96,96 },
{ { "orc shipyard construction site (summer,wasteland).png",
  "orc shipyard construction site (winter).png",
  DEFAULT,
  "orc shipyard construction site (swamp).png" },
	96,96 },
{ { "human oil well construction site (summer).png",
  "human oil well construction site (winter).png",
  "human oil well construction site (wasteland).png",
  "human oil well construction site (swamp).png" },
	96,96 },
{ { "orc oil well construction site (summer).png",
  "orc oil well construction site (winter).png",
  "orc oil well construction site (wasteland).png",
  "orc oil well construction site (swamp).png" },
	96,96 },
{ { "human refinery construction site (summer,wasteland).png",
  "human refinery construction site (winter).png",
  DEFAULT,
  "human refinery construction site (swamp).png" },
	96,96 },
{ { "orc refinery construction site (summer,wasteland).png",
  "orc refinery construction site (winter).png",
  DEFAULT,
  "orc refinery construction site (swamp).png" },
	96,96 },
{ { "human foundry construction site (summer,wasteland).png",
  "human foundry construction site (winter).png",
  DEFAULT,
  "human foundry construction site (swamp).png" },
	96,96 },
{ { "orc foundry construction site (summer,wasteland).png",
  "orc foundry construction site (winter).png",
  DEFAULT,
  "orc foundry construction site (swamp).png" },
	96,96 },
{ { "wall construction site (summer).png",
  "wall construction site (winter).png",
  "wall construction site (wasteland).png",
  DEFAULT },
	32,32 },
};

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Load the graphics for the constructions.
**
**	HELPME:	who make this better terrain depended and extendable
**	HELPME: filename constuction.
*/
global void LoadConstructions(void)
{
    int i;
    const char* file;

    for( i=0; i<sizeof(Constructions)/sizeof(*Constructions); ++i ) {
	file=Constructions[i].File[TheMap.Terrain];
	if( !file ) {			// default one
	    file=Constructions[i].File[0];
	}
	if( *file ) {
	    char* buf;

	    buf=alloca(strlen(file)+9+1);
	    file=strcat(strcpy(buf,"graphic/"),file);
	    ShowLoadProgress("\tConstruction %s\n",file);
	    Constructions[i].RleSprite=LoadRleSprite(file
		    ,Constructions[i].Width,Constructions[i].Height);
	}
    }
}

/**
**	Draw construction.
**
**	@param type	Type number of construction.
**	@param frame	Frame number to draw.
**	@param x	X position.
**	@param y	Y position.
*/
global void DrawConstruction(int type,int frame,int x,int y)
{
    x-=Constructions[type].Width/2;
    y-=Constructions[type].Height/2;

    DrawRleSpriteClipped(Constructions[type].RleSprite,frame,x,y);
}

//@}
