/*
**	A clone of a famous game.
**
**	video.c		-	The video.
**
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: video.c,v 1.18 1999/12/09 16:27:19 root Exp $
*/

#include <stdio.h>
#include <stdlib.h>

#include "clone.h"
#include "video.h"

//	FIXME: this functions only supports 16 bit displays!!!!

#ifdef DEBUG
global unsigned AllocatedGraphicMemory;
#endif

/*----------------------------------------------------------------------------
--	Clipping
----------------------------------------------------------------------------*/

local int ClipX1;
local int ClipY1;
local int ClipX2;
local int ClipY2;

/*
**	Set clipping for sprite/line routines.
*/
global void SetClipping(int left,int top,int right,int bottom)
{
    if( left>right ) { left^=right; right^=left; left^=right; }
    if( top>bottom ) { top^=bottom; bottom^=top; top^=bottom; }
    
    if( left<0 )    left=0;
    if( top<0 )	    top=0;
    if( right<0 )   right=0;
    if( bottom<0 )  bottom=0;

    if( left>=VideoWidth )	left=VideoWidth-1;
    if( right>=VideoWidth )	right=VideoWidth-1;
    if( bottom>=VideoHeight ) bottom=VideoHeight-1;
    if( top>=VideoHeight )	top=VideoHeight-1;
    
    ClipX1=left;
    ClipY1=top;
    ClipX2=right;
    ClipY2=bottom;
}

/*----------------------------------------------------------------------------
--	Graphics
----------------------------------------------------------------------------*/

/*
**	Graphic: Uncompressed images without transparens.
*/

/*
**	New graphic.
*/
global void NewOldGraphic(int w,int h,OldGraphic* graphic)
{
    graphic->Width=w;
    graphic->Height=h;
    graphic->Data=malloc(w*h);

    IfDebug( AllocatedGraphicMemory+=w*h; );
}

/*
**	Draw graphic.
*/
global void DrawOldGraphic(OldGraphic* graphic,int gx,int gy,int w,int h,int x,int y)
{
    unsigned char* sp;
    unsigned char* lp;
    unsigned char* gp;
    int sa;
    VMemType* dp;
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
	    *dp++=Pixels[*sp++];	// unroll
	    *dp++=Pixels[*sp++];
	}
	if( sp<=lp ) {
	    *dp++=Pixels[*sp++];
	}
	sp+=sa;
	dp+=da;
    }
}

/*
**	Load graphic from file.
*/
global void LoadOldGraphic(const char* name,OldGraphic* graphic)
{
    char buf[strlen(name)+strlen(CLONE_LIB_PATH)+3];

    sprintf(buf,"%s/%s",CLONE_LIB_PATH,name);
    if( LoadPNG(buf,graphic) ) {
	printf("Can't load the graphic `%s'\n",name);
	exit(-1);
    }
}

/*
**	Free graphic.
*/
global void FreeOldGraphic(OldGraphic* graphic)
{
    IfDebug( AllocatedGraphicMemory-=graphic->Width*graphic->Height; );

    graphic->Width=0;
    graphic->Height=0;
    free(graphic->Data);
}

/*----------------------------------------------------------------------------
--	Sprites
----------------------------------------------------------------------------*/

/*
**	Draw sprite.
*/
global void DrawSprite(Sprite* sprite,int gx,int gy,int w,int h,int x,int y)
{
    int p;
    unsigned char* sp;
    unsigned char* lp;
    unsigned char* gp;
    int sa;
    VMemType* dp;
    int da;

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
	    *dp=Pixels[p];
	}
	sp+=sa;
	dp+=da;
    }
}

/*
**	Draw sprite clipped.
*/
global void DrawSpriteClipped(Sprite* sprite
	,int gx,int gy,int w,int h,int x,int y)
{
    // FIXME: Clipped not 100% correct!
    if( x<ClipX1 ) {
	gx+=ClipX1-x;
	w-=ClipX1-x;
	x=ClipX1;
	DebugLevel3("Reduce %d,%d, %d,%d\n",x,y,w,h);
    }
    if( x+w>ClipX2 ) {
	w=ClipX2-x;
	DebugLevel3("Reduce %d,%d, %d,%d\n",x,y,w,h);
    }
    if( y<ClipY1 ) {
	gy+=ClipY1-y;
	h-=ClipY1-y;
	y=ClipY1;
	DebugLevel3("Reduce %d,%d, %d,%d\n",x,y,w,h);
    }
    if( y+h>ClipY2 ) {
	h=ClipY2-y;
	DebugLevel3("Reduce %d,%d, %d,%d\n",x,y,w,h);
    }
    if( w>0 && h>0 ) {
	DrawSprite(sprite,gx,gy,w,h,x,y);
    }
}

/*
**	Draw sprite. Flip in X.
*/
global void DrawSpriteX(Sprite* sprite,int gx,int gy,int w,int h,int x,int y)
{
    int p;
    unsigned char* sp;
    unsigned char* lp;
    unsigned char* gp;
    int sa;
    VMemType* dp;
    int da;

    sp=sprite->Data+sprite->Width-gx+gy*sprite->Width-1;
    gp=sp+sprite->Width*h;
    sa=sprite->Width+w;
    dp=VideoMemory+x+y*VideoWidth-1;
    da=VideoWidth-w;
    while( sp<gp ) {
	lp=sp-w;
	while( sp>lp ) {
	    --sp;
	    ++dp;
	    p=*sp;
	    if( p==255 ) {
		continue;
	    }
	    *dp=Pixels[p];
	}
	sp+=sa;
	dp+=da;
    }
}

/*
**	Load a sprite from file.
*/
global void LoadSprite(const char* name,Sprite* sprite)
{
    OldGraphic graphic;

    LoadOldGraphic(name,&graphic);

    sprite->Width=graphic.Width;
    sprite->Height=graphic.Height;
    sprite->Data=graphic.Data;
}

/*----------------------------------------------------------------------------
--	RLE Sprites
----------------------------------------------------------------------------*/

//	FIXME: can also compress same bytes
//	Count:	transparent

global unsigned CompressedGraphicMemory;

/*
**	Draw rle compressed sprite.
*/
global void DrawRleSprite(RleSprite* sprite,unsigned frame,int x,int y)
{
    const unsigned char* sp;
    unsigned w;
    VMemType* dp;
    VMemType* lp;
    VMemType* ep;
    VMemType* pp;
    unsigned da;

    sp=sprite->Frames[frame];
    w=sprite->Width;
    da=VideoWidth-w;
    dp=VideoMemory+x+y*VideoWidth;
    ep=dp+VideoWidth*sprite->Height;

    while( dp<ep ) {			// all lines
	lp=dp+w;
	do {				// 1 line
	    dp+=*sp++;			// transparent
	    if( dp>=lp ) {
		break;
	    }
	    pp=dp-1+*sp++;		// non-transparent
	    while( dp<pp ) {
		*dp++=Pixels[*sp++];
		*dp++=Pixels[*sp++];
	    }
	    if( dp<=pp ) {
		*dp++=Pixels[*sp++];
	    }
	} while( dp<lp );
	IfDebug( 
	    if( dp!=lp )
		printf("ERROR: %s\n",__FUNCTION__);
	)
	dp+=da;
    }
}

/*
**	Draw rle compressed sprite with clipping.
*/
global void DrawRleSpriteClipped(RleSprite* sprite,unsigned frame,int x,int y)
{
    int ox;
    int oy;
    int w;
    int h;
    const unsigned char* sp;
    unsigned sw;
    VMemType* dp;
    VMemType* lp;
    VMemType* ep;
    VMemType* pp;
    unsigned da;

    ox=oy=0;
    sw=w=sprite->Width;
    h=sprite->Height;

    if( x<ClipX1 ) {			// reduce to visible range
	ox=ClipX1-x;
	w-=ox;
	x=ClipX1;
    }
    if( x+w>ClipX2 ) {
	w=ClipX2-x;
    }

    if( y<ClipY1 ) {
	oy=ClipY1-y;
	h-=oy;
	y=ClipY1;
    }
    if( y+h>ClipY2 ) {
	h=ClipY2-y;
    }

    if( w<=0 || h<=0 ) {		// nothing to draw
	return;
    }

    //
    //	Draw the clipped sprite
    //
    sp=sprite->Frames[frame];

    //
    // Skip top lines
    //
    while( oy-- ) {
	da=0;
	do {
	    da+=*sp++;			// transparent
	    if( da>=sw ) {
		break;
	    }
	    da+=*sp;			// non-transparent
	    sp+=*sp+1;
	} while( da<sw );
    }

    da=VideoWidth-sw;
    dp=VideoMemory+x+y*VideoWidth;
    ep=dp+VideoWidth*h;

    if( w==sw ) {			// Unclipped horizontal

	while( dp<ep ) {		// all lines
	    lp=dp+sw;
	    do {			// 1 line
		dp+=*sp++;		// transparent
		if( dp>=lp ) {
		    break;
		}
		pp=dp-1+*sp++;		// non-transparent
		while( dp<pp ) {
		    *dp++=Pixels[*sp++];
		    *dp++=Pixels[*sp++];
		}
		if( dp<=pp ) {
		    *dp++=Pixels[*sp++];
		}
	    } while( dp<lp );
	    IfDebug( 
		if( dp!=lp )
		    printf("ERROR: %s\n",__FUNCTION__);
	    )
	    dp+=da;
	}

    } else {				// Clip horizontal
	//printf("CLIPPING ox %d w %d\n",ox,w);

	da+=ox;
	while( dp<ep ) {		// all lines
	    lp=dp+w;
	    //
	    //	Clip left
	    //
	    pp=dp-ox;
	    for( ;; ) {
		pp+=*sp++;		// transparent
		//printf("T%d-",sp[-1]);
		if( pp>=dp ) {
		    dp=pp;
		    //printf("C");
		    goto middle_trans;
		}
		pp+=*sp;		// non-transparent
		//printf("P%d-",sp[0]);
		if( pp>=dp ) {
		    sp+=*sp-(pp-dp)+1;
		    //printf("C");
		    goto middle_pixel;
		}
		sp+=*sp+1;
	    }

	    //
	    //	Draw middle
	    //
	    for( ;; ) {
		dp+=*sp++;		// transparent
		//printf("T%d-",sp[-1]);
middle_trans:
		if( dp>=lp ) {
		    lp+=sw-w-ox;
		    //printf("C");
		    goto right_trans;
		}
		pp=dp+*sp++;		// non-transparent
		//printf("P%d-",sp[-1]);
middle_pixel:
		//printf("%p, %p, %p\n",dp,pp,lp);
		if( pp<lp ) {
		    while( dp<pp ) {
			*dp++=Pixels[*sp++];
		    }
		    continue;
		}
		while( dp<lp ) {
		    *dp++=Pixels[*sp++];
		}
		sp+=pp-dp;
		dp=pp;
		//printf("C");
		break;
	    }

	    //
	    //	Clip right
	    //
	    lp+=sw-w-ox;
	    while( dp<lp ) {
		dp+=*sp++;		// transparent
right_trans:
		if( dp>=lp ) {
		    break;
		}
		dp+=*sp;		// non-transparent
		sp+=*sp+1;
	    }
	    //printf("\n");
	    IfDebug( 
		if( dp!=lp )
		    printf("ERROR: %s\n",__FUNCTION__);
	    )
	    dp+=da;
	}
    }
}

/*
**	Draw rle compressed sprite, flipped in X.
*/
global void DrawRleSpriteX(RleSprite* sprite,unsigned frame,int x,int y)
{
    const unsigned char* sp;
    unsigned w;
    VMemType* dp;
    VMemType* lp;
    VMemType* ep;
    VMemType* pp;
    unsigned da;

    sp=sprite->Frames[frame];
    w=sprite->Width;
    dp=VideoMemory+x+y*VideoWidth+w;
    da=VideoWidth+w;
    ep=dp+VideoWidth*sprite->Height;

    while( dp<ep ) {			// all lines
	lp=dp-w;
	do {				// 1 line
	    dp-=*sp++;			// transparent
	    if( dp<=lp ) {
		break;
	    }
	    pp=dp+1-*sp++;		// non-transparent
	    while( dp>pp ) {
		*dp--=Pixels[*sp++];
		*dp--=Pixels[*sp++];
	    }
	    if( dp>=pp ) {
		*dp--=Pixels[*sp++];
	    }
	} while( dp>lp );
	IfDebug( 
	    if( dp!=lp )
		printf("ERROR: %s\n",__FUNCTION__);
	)
	dp+=da;
    }
}

/*
**	Draw rle compressed sprite with clipping, flipped in X.
*/
global void DrawRleSpriteClippedX(RleSprite* sprite,unsigned frame,int x,int y)
{
    int ox;
    int oy;
    int w;
    int h;
    const unsigned char* sp;
    unsigned sw;
    VMemType* dp;
    VMemType* lp;
    VMemType* ep;
    VMemType* pp;
    unsigned da;

    ox=oy=0;
    sw=w=sprite->Width;
    h=sprite->Height;

    if( x<ClipX1 ) {			// reduce to visible range
	ox=ClipX1-x;
	w-=ox;
	x=ClipX1;
    }
    if( x+w>ClipX2 ) {
	w=ClipX2-x;
    }

    if( y<ClipY1 ) {
	oy=ClipY1-y;
	h-=oy;
	y=ClipY1;
    }
    if( y+h>ClipY2 ) {
	h=ClipY2-y;
    }

    if( w<=0 || h<=0 ) {		// nothing to draw
	return;
    }

    //
    //	Draw the clipped sprite
    //
    sp=sprite->Frames[frame];

    //
    // Skip top lines
    //
    while( oy-- ) {
	da=0;
	do {
	    da+=*sp++;			// transparent
	    if( da>=sw ) {
		break;
	    }
	    da+=*sp;			// non-transparent
	    sp+=*sp+1;
	} while( da<sw );
    }

    da=VideoWidth+sw;
    dp=VideoMemory+x+y*VideoWidth+w;
    ep=dp+VideoWidth*h;

    if( w==sw ) {			// Unclipped horizontal

	while( dp<ep ) {		// all lines
	    lp=dp-w;
	    do {			// 1 line
		dp-=*sp++;		// transparent
		if( dp<=lp ) {
		    break;
		}
		pp=dp+1-*sp++;		// non-transparent
		while( dp>pp ) {
		    *dp--=Pixels[*sp++];
		    *dp--=Pixels[*sp++];
		}
		if( dp>=pp ) {
		    *dp--=Pixels[*sp++];
		}
	    } while( dp>lp );
	    IfDebug( 
		if( dp!=lp )
		    printf("ERROR: %s\n",__FUNCTION__);
	    )
	    dp+=da;
	}

    } else {				// Clip horizontal
	//printf("CLIPPING %d %d\n",ox,w);

	da-=sw-w-ox;
	while( dp<ep ) {		// all lines
	    lp=dp-w;
	    //
	    //	Clip right side
	    //
	    pp=dp+sw-w-ox;
	    for( ;; ) {
		pp-=*sp++;		// transparent
		//printf("T%d ",sp[-1]);
		if( pp<=dp ) {
		    dp=pp;
		    goto middle_trans;
		}
		pp-=*sp;		// non-transparent
		//printf("P%d ",sp[0]);
		if( pp<=dp ) {
		    sp+=*sp-(dp-pp)+1;
		    goto middle_pixel;
		}
		sp+=*sp+1;
	    }

	    //
	    //	Draw middle
	    //
	    for( ;; ) {
		dp-=*sp++;		// transparent
		//printf("T%d ",sp[-1]);
middle_trans:
		if( dp<=lp ) {
		    //printf("CLIP TRANS\n");
		    lp-=ox;
		    goto right_trans;
		}
		pp=dp-*sp++;		// non-transparent
		//printf("P%d ",sp[-1]);
middle_pixel:
		if( pp>lp ) {
		    while( dp>pp ) {
			*dp--=Pixels[*sp++];
		    }
		    continue;
		}
		//printf("%d ",sp[-1]);
		while( dp>lp ) {
		    *dp--=Pixels[*sp++];
		}
		//printf("%d: ",dp-pp);
		sp+=dp-pp;
		//printf("CLIP PIXEL %d,%d,%d\n",*sp,sp[-1],sp[1]);
		dp=pp;
		break;
	    }

	    //
	    //	Clip left side
	    //
	    lp-=ox;
	    while( dp>lp ) {
		dp-=*sp++;		// transparent
right_trans:
		if( dp<=lp ) {
		    break;
		}
		dp-=*sp;		// non-transparent
		sp+=*sp+1;
	    }
	    IfDebug( 
		if( dp!=lp )
		    printf("ERROR: %s\n",__FUNCTION__);
	    )
	    dp+=da;
	}
    }
}

/*
**	Load a sprite from file.
*/
global RleSprite* LoadRleSprite(const char* name,unsigned width,unsigned height)
{
    RleSprite* sprite;
    OldGraphic graphic;
    unsigned char** frames;
    unsigned char* data;
    unsigned char* sp;
    unsigned char* dp;
    unsigned char* cp;
    int fl;
    int n;
    int counter;
    int i;
    int h;
    int w;

    LoadOldGraphic(name,&graphic);
    if( !width ) {			// FIXME: this is hack for cursors!
	width=graphic.Width;
    }
    if( !height ) {
	height=graphic.Height;
    }

    n=(graphic.Width/width)*(graphic.Height/height);
    DebugLevel3("%s: %dx%d in %dx%d = %d frames.\n",__FUNCTION__
	    ,width,height
	    ,graphic.Width,graphic.Height,n);

    // FIXME: new internal compressed sprite format!
    frames=alloca(sizeof(unsigned char*)*n);
    dp=data=alloca(graphic.Width*graphic.Height*2);

    //
    //	Compress all frames of the sprite.
    //
    fl=graphic.Width/width;
    for( i=0; i<n; ++i ) {
	frames[i]=dp;
	for( h=0; h<height; ++h ) {
	    sp=graphic.Data+(i%fl)*width+((i/fl)*height+h)*graphic.Width;

	    for( counter=w=0; w<width; ++w ) {
		if( *sp==255 ) {	// transparent
		    ++sp;
		    if( ++counter==256 ) {
			*dp++=255;
			*dp++=0;
			counter=1;
		    }
		    continue;
		}
		*dp++=counter;

		cp=dp++;
		counter=0;
#if 1
		for( ; w<width; ++w ) {	// non-transparent
		    *dp++=*sp++;
		    if( ++counter==256 ) {
			*cp=255;
			*dp++=0;
			cp=dp++;
			counter=1;
		    }
		    if( w+1!=width && *sp==255 ) {	// transparent
			break;
		    }
		}
#else
		for( ; w<width && *sp!=255 ; ++w ) {	// non-transparent
		    *dp++=*sp++;
		    if( ++counter==256 ) {
			*cp=255;
			*dp++=0;
			cp=dp++;
			counter=1;
		    }
		}
#endif
		*cp=counter;
		counter=0;
	    }
	    if( counter ) {
		*dp++=counter;
	    }
	}
    }

    DebugLevel3("\t%d => %d RLE compressed\n"
	    ,graphic.Width*graphic.Height,dp-data);

    i=sizeof(*sprite)+n*sizeof(unsigned char*)+dp-data;
    CompressedGraphicMemory+=i;
    sprite=malloc(i);
    sprite->Width=width;
    sprite->Height=height;
    sprite->Pixels=NULL;		// FIXME: future extensions
    sprite->NumFrames=n;

    for( h=0; h<n; ++h ) {		// convert adress
	sprite->Frames[h]=(unsigned char*)(&sprite->Frames[n])+(frames[h]-data);
    }
    memcpy(&sprite->Frames[n],data,dp-data);

    FreeOldGraphic(&graphic);

    return sprite;
}

/*----------------------------------------------------------------------------
--	Lines
----------------------------------------------------------------------------*/

#ifndef DrawPointUnclipped
/*
**	Draw point unclipped.
*/
global void DrawPointUnclipped(SysColors color,int x,int y)
{
    VideoMemory[x+y*VideoWidth]=Pixels[color];
}
#endif

/*
**	Draw point.
*/
global void DrawPoint(SysColors color,int x,int y)
{
    //	Clipping:
    if( x<ClipX1 || x>=ClipX2 ) {
	return;
    }
    if( y<ClipY1 || y>=ClipY2 ) {
	return;
    }
    VideoMemory[x+y*VideoWidth]=Pixels[color];
}

/*
**	Draw vertical line.
*/
global void DrawVLine(SysColors color,int x,int y,int height)
{
    VMemType* p;
    VMemType* e;
    int w;
    int f;

    //	Clipping:
    if( x<ClipX1 || x>=ClipX2 ) {
	return;
    }
    if( y<ClipY1 ) {
	f=ClipY1-y;
	y=ClipY1;
	height-=f;
    }
    if( (y+height)>ClipY2 ) {
	height=ClipY2-y;
    }
    if( height<0 )	return;

    w=VideoWidth;
    p=VideoMemory+y*w+x;
    e=p+height*w;
    f=Pixels[color];
    while( p<e ) {
	*p=f;
	p+=w;
    }
}

/*
**	Draw horizontal line unclipped.
*/
global void DrawHLineUnclipped(SysColors color,int x,int y,int width)
{
    VMemType* p;
    VMemType* e;
    int w;
    unsigned long f;

    w=VideoWidth;
    p=VideoMemory+y*w+x;
    e=p+width;
#if 0
    f=Pixels[color];
    while( p<e ) {
	*(p++)=f;
    }
#endif
    f=(Pixels[color]<<16)|Pixels[color];
    while( p<e-1 ) {			// draw 2 pixels
	*((unsigned long*)p)++=f;
    }
    if( p<e ) {
	*p=f;
    }
}

/*
**	Draw horizontal line.
*/
global void DrawHLine(SysColors color,int x,int y,int width)
{
    int f;

    if( y<ClipY1 || y>=ClipY2 ) {	//	Clipping:
	return;
    }
    if( x<ClipX1 ) {
	f=ClipX1-x;
	x=ClipX1;
	width-=f;
    }
    if( (x+width)>ClipX2 ) {
	width=ClipX2-x;
    }
    if( width<0 ) {
	return;
    }

    DrawHLineUnclipped(color,x,y,width);
}

/*
**	Draw rectangle.
*/
global void DrawRectangle(SysColors color,int x,int y,int w,int h)
{
    //	FIXME: Clip here
    DrawHLine(color,x,y,w);
    DrawVLine(color,x,y+1,h);
    DrawHLine(color,x+1,y+h,w);
    DrawVLine(color,x+w,y,h);
}

/*
**	Fill rectangle.
*/
global void FillRectangle(SysColors color,int x,int y,int w,int h)
{
    //	FIXME: Clip here
    while( h-- ) {
	DrawHLine(color,x,y++,w);
    }
}

/*
**	General line drawing.
*/
global void DrawLine(SysColors color,int x1,int y1,int x2,int y2)
{
    // FIXME: write it, if needed!
}

global void LoadRGB(Palette *pal, const char *name)
{
    FILE *fp;
    int i;
    
    if((fp=fopen(name,"rb")) == NULL) {
	printf("Can't load palette %s\n", name);
	exit(-1);
    }

    for(i=0;i<256;i++){
	pal[i].r=fgetc(fp);
	pal[i].g=fgetc(fp);
	pal[i].b=fgetc(fp);
    }
    
    fclose(fp);
}
