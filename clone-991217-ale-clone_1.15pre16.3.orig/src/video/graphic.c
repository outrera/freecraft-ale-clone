/*
**	A clone of a famous game.
*/
/**@name graphic.c	-	The general graphic functions. */
/*
**
**	(c) Copyright 1999 by Lutz Sammer
**
**	$Id: graphic.c,v 1.3 1999/12/09 16:27:19 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "clone.h"
#include "video.h"

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Externals
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

local GraphicType GraphicImage8Type;	/// image type 8bit palette
local GraphicType GraphicImage16Type;	/// image type 16bit palette

/*----------------------------------------------------------------------------
--	Local functions
----------------------------------------------------------------------------*/

/**
**	Video draw 8bit graphic into 16 bit framebuffer.
**
**	@param graphic	Pointer to object
**	@param gx	X offset into object
**	@param gy	Y offset into object
**	@param w	width to display
**	@param h	height to display
**	@param x	X screen position
**	@param y	Y screen position
*/
local void VideoDrawGraphic8to16(
	OldGraphic* graphic,unsigned gx,unsigned gy,unsigned w,unsigned h,
	unsigned x,unsigned y)
{
    unsigned char* sp;
    unsigned char* lp;
    unsigned char* gp;
    int sa;
    VMemType16* dp;
    int da;

    sp=graphic->Data+gx+gy*graphic->Width;
    gp=sp+graphic->Width*h;
    sa=graphic->Width-w;
    dp=VideoMemory+x+y*VideoWidth;
    da=VideoWidth-w;
    --w;
    while( sp<gp ) {
	lp=sp+w;
	while( sp<lp ) {
	    *dp++=Pixels16[*sp++];	// unroll
	    *dp++=Pixels16[*sp++];
	}
	if( sp<=lp ) {
	    *dp++=Pixels16[*sp++];
	}
	sp+=sa;
	dp+=da;
    }
}

/*----------------------------------------------------------------------------
--	Global functions
----------------------------------------------------------------------------*/

/**
**	Make a graphic object.
**
**	@param depth	Pixel depth of the object (8,16,32)
**	@param width	Pixel width.
**	@param height	Pixel height.
**	@param data	Object data.
*/
global Graphic* MakeGraphic(
	unsigned depth,unsigned width,unsigned height,void* data)
{
    return NULL;
}

/**
**	Generate a filename into library.
**
**	Try current directory, user home directory, global directory.
**
**	FIXME: I want also support files stored into tar/zip archives.
**
**	@param file	Filename to open.
**	@param buffer	Allocated buffer for generated filename.
**
**	@return		Pointer to buffer.
*/
global char* LibraryFileName(const char* file,char* buffer)
{
    //
    //	Absolute path or in current directory.
    //
    strcpy(buffer,file);
    if( *buffer=='/' || !access(buffer,R_OK) ) {
	return buffer;
    }

    //
    //	In user home directory
    //
    sprintf(buffer,"%s/%s/%s",getenv("HOME"),CLONE_HOME_PATH,file);
    if( !access(buffer,R_OK) ) {
	return buffer;
    }

    //
    //	In global shared directory
    //
    sprintf(buffer,"%s/%s",CLONE_LIB_PATH,file);
    if( !access(buffer,R_OK) ) {
	return buffer;
    }
    DebugLevel0(__FUNCTION__": File `%s' couldn't found\n",file);

    strcpy(buffer,file);
    return buffer;
}

/**
**	Load graphic from file.
**
**	@param name	File name.
**
**	@return		Graphic object.
*/
global Graphic* LoadGraphic(const char* name)
{
    Graphic* graphic;
    char buf[1024];

    // FIXME: I want also support JPG file format!

    if( (graphic=LoadGraphicPNG(LibraryFileName(name,buf))) ) {
	printf("Can't load the graphic `%s'\n",name);
	exit(-1);
    }
    return graphic;
}

/**
**	Init graphic
*/
global void InitGraphic(void)
{
    switch( VideoDepth ) {
	case 15:
	case 16:
	    break;

	case 8:
	case 24:
	case 32:

	default:
	    DebugLevel0(__FUNCTION__": unsupported %d bpp\n",VideoDepth);
	    abort();
    }
}

//@}
