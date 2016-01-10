/*
**	A clone of a famous game.
*/
/**@name font.c		-	The color fonts. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: font.c,v 1.4 1999/12/09 16:27:08 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>

#include "clone.h"
#include "video.h"
#include "font.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	Fonts table
**		FIXME: human / orcish
*/
global ColorFont Fonts[] = {
    { "graphic/small font.png", 13,14 ,{
	 6, 2, 4, 8, 5, 7, 7, 3, 4, 4, 8, 6, 3, 5, 2, 6,
	 6, 3, 6, 6, 7, 6, 7, 6, 6, 6, 2, 3, 5, 4, 5, 6,
	11, 8, 8, 7, 8, 7, 7, 7, 8, 4, 7,10, 7,10, 8, 7,
	 8, 8, 9, 6, 8, 8, 8,12, 8,11, 7, 3, 5, 3, 6, 8,
	 3, 7, 6, 6, 7, 6, 6, 8, 8, 4, 4, 7, 4,11, 8, 7,
	 7, 8, 6, 6, 5, 8, 8,11, 7, 7, 6, 3, 2, 3, 7, 8,
	 7, 8, 6, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 4, 9, 9,
	 7,10,13, 7, 7, 7, 8, 8, 7, 7, 8, 6, 8,11, 8, 7,
	 7, 5, 7, 8, 8, 8, 4, 6, 7, 8, 8, 8, 8, 8, 8, 8,
	11,13,13,12, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 },
    },
    { "graphic/game font.png", 7,6 ,{
	 6, 2, 4, 6, 6, 6, 5, 3, 3, 3, 4, 4, 3, 4, 2, 6,
	 5, 4, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 4, 4, 4, 5,
	 6, 5, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 6, 6, 5,
	 5, 5, 5, 5, 4, 5, 6, 6, 6, 6, 6, 3, 6, 3, 4, 5,
	 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 6, 6, 5,
	 5, 5, 5, 5, 4, 5, 6, 6, 6, 6, 6, 4, 2, 4, 6, 6,
	 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 5, 5,
	 5, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 4, 5, 6, 6, 5,
	 4, 4, 5, 5, 6, 6, 4, 4, 5, 6, 6, 6, 6, 6, 6, 2,
	 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	 6, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 },
    },
};

// FIXME: move the next to video !!

local unsigned char CharColors[7]  = { 246, 200, 199, 197, 192, 0, 104 };
local unsigned char RCharColors[7] = { 0, 246, 246, 246, 104, 0, 0 };

local int ReverseText;
global int DefaultReverseText=1;

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Draw char.
*/
local void DrawChar(Sprite* sprite,int gx,int gy,int w,int h,int x,int y)
{
    int p;
    unsigned char* sp;
    unsigned char* lp;
    unsigned char* gp;
    int sa;
    VMemType* dp;
    int da;
    unsigned char* colors;

    colors=ReverseText ? RCharColors : CharColors;

    sp=sprite->Data+gx+gy*sprite->Width-1;
    gp=sp+sprite->Width*h;
    sa=sprite->Width-w;
    dp=VideoMemory+x+y*VideoWidth-1;
    da=VideoWidth-w;
    while( sp<gp ) {
	lp=sp+w;
	while( sp<lp ) {
	    ++dp;
	    ++sp;
	    p=*sp;
	    if( p==255 ) {
		continue;
	    }
	    *dp=Pixels[colors[p]];
	}
	sp+=sa;
	dp+=da;
    }
}

/**
**	Draw text with font at x,y.
*/
global void DrawText(int x,int y,int font,const unsigned char* text)
{
    int w;
    ColorFont* fp;

    fp=Fonts+font;
    for( ; *text; ++text ) {
	ReverseText=!DefaultReverseText;
	if( *text=='~' ) {
	    ReverseText=DefaultReverseText;
	    ++text;
	}
	w=fp->CharWidth[*text-32];
	DrawChar(&fp->Sprite,0,fp->Height*(*text-32),w,fp->Height,x,y);
	x+=w+1;
    }
}

/**
**	Returns the length of a text.
*/
global int TextLength(int font,const unsigned char* text)
{
    int width;
    const unsigned char* s;
    const ColorFont* fp;

    fp=Fonts+font;
    for( width=0,s=text; *s; ++s ) {
	if( *text=='~' ) {
	    continue;
	}
	width+=fp->CharWidth[*s-32]+1;
    }
    return width;
}

/**
**	Draw text with font at x,y centered.
*/
global void DrawTextCentered(int x,int y,int font,const unsigned char* text)
{
    int dx;
    const unsigned char* s;
    const ColorFont* fp;

    fp=Fonts+font;
    for( dx=0,s=text; *s; ++s ) {
	if( *text=='~' ) {
	    continue;
	}
	dx+=fp->CharWidth[*s-32]+1;
    }
    DrawText(x-dx/2,y,font,text);
}

/**
**	Draw number with font at x,y.
*/
global void DrawNumber(int x,int y,int font,int number)
{
    char buf[sizeof(int)*10+2];

    sprintf(buf,"%d",number);
    DrawText(x,y,font,buf);
}

/**
**	Load all fonts.
*/
global void LoadFonts(void)
{
    unsigned i;

    for( i=0; i<sizeof(Fonts)/sizeof(*Fonts); ++i ) {
	ShowLoadProgress("\tFonts %s\n",Fonts[i].File);
	LoadSprite(Fonts[i].File,&Fonts[i].Sprite);
    }
}

//@}
