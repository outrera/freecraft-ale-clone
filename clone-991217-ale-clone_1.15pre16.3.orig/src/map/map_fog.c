/*
**	A clone of a famous game.
*/
/**@name map_fog.c	-	The map fog of war handling. */
/*
**
**	(c) Copyright 1999 by Lutz Sammer and Vladi Shabanski
**
**	$Id: map_fog.c,v 1.10 1999/12/09 16:27:13 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "clone.h"
#include "player.h"
#include "unittype.h"
#include "unit.h"
#include "map.h"
#include "minimap.h"

#include "rdtsc.h"

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

#ifdef DEBUG
#define noTIMEIT			/// defines time function
#endif
#define OLD_FOG				/// use old not correct version

/**
**	Do unroll 8x
*/
#define UNROLL8(x)	\
    UNROLL2((x)+0);	\
    UNROLL2((x)+2);	\
    UNROLL2((x)+4);	\
    UNROLL2((x)+6)

/**
**	Do unroll 16x
*/
#define UNROLL16(x)	\
    UNROLL8((x)+ 0);	\
    UNROLL8((x)+ 8)

/**
**	Do unroll 32x
*/
#define UNROLL32(x)	\
    UNROLL8((x)+ 0);	\
    UNROLL8((x)+ 8);	\
    UNROLL8((x)+16);	\
    UNROLL8((x)+24)

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/


global int OriginalFogOfWar;		/// Use original style fog of war
global int FogOfWarContrast=50;		/// Contrast of fog of war
global int FogOfWarBrightness=10;	/// Brightness of fog of war
global int FogOfWarSaturation=40;	/// Saturation of fog of war

/**
**	Mapping for fog of war tiles.
*/
local int FogTable[16] = {
     0,11,10, 2,
    13, 6, 0, 3,
    12, 0, 4, 1,
     8, 9, 7, 0,
};

/**
**	Draw unexplored area function pointer. (display and video mode independ)
*/
local void (*VideoDrawUnexplored)(const GraphicData*,int,int);

/**
**	Draw fog of war function pointer. (display and video mode independ)
*/
local void (*VideoDrawFog)(const GraphicData*,int,int);

/**
**	Draw only fog of war function pointer. (display and video mode independ)
*/
local void (*VideoDrawOnlyFog)(const GraphicData*,int x,int y);

/**
**	Precalculated fog of war alpha table.
*/
local void* FogOfWarAlphaTable;


/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Mark the sight of unit. (Explore and make visibile.)
**
**	@param tx	X center position.
**	@param ty	Y center position.
**	@param range	Radius to mark.
*/
global void MapMarkSight(int tx,int ty,int range)
{
    int i;
    int x;
    int y;
    int height;
    int width;

#ifndef OLD_FOG
    range--;
#endif
    x=tx-range;
    y=ty-range;
    width=height=range+range;

    //	Clipping
    if( y<0 ) {
	height+=y;
	y=0;
    }
    if( x<0 ) {
	width+=x;
	x=0;
    }
    if( y+height>=TheMap.Height ) {
	height=TheMap.Height-y-1;
    }
    if( x+width>=TheMap.Width ) {
	width=TheMap.Width-x-1;
    }

    ++range;
    while( height-->=0 ) {
	for( i=x; i<=x+width; ++i ) {
	    // FIXME: Can use quadrat table!
	    if( ((i-tx)*(i-tx)+(y-ty)*(y-ty))<=range*range ) {
		TheMap.Fields[i+y*TheMap.Width].Flags
			|=MapFieldVisible|MapFieldExplored;
		MapMarkSeenTile(i,y);
	    }
	}
	++y;
    }
}

/**
**	Update visible of the map.
*/
global void MapUpdateVisible(void)
{
    int x;
    int y;
    int ye;
    int w;
    Unit* unit;
    Unit** units;
    Unit* mine;
    int nunits,i;

    // FIXME: rewrite this function, faster and better

    //
    //	Clear all visible flags.
    //
    w=TheMap.Width;
    ye=TheMap.Height*w;
    if (TheMap.NoFogOfWar) {
	for( x=0; x<TheMap.Width; ++x ) {
	    for( y=0; y<TheMap.Height; y++ ) {
		if ( MAPVISIBLE(x,y) ) {
		    MapMarkSeenTile( x,y );
		}
	    }
	}
    } else {
	for( y=0; y<ye; y+=w ) {
	    for( x=0; x<w; ++x ) {
		TheMap.Fields[x+y].Flags&=~MapFieldVisible;
	    }
	}
    }

    //
    //	Mark all units visible range.
    //
    nunits=ThisPlayer->TotalNumUnits;
    units=ThisPlayer->Units;
    for( i=0; i<nunits; i++ ) {
	unit=units[i];
	if( unit->Removed ) {
	    //
	    //	If peon is in the mine, the mine has a sight range too.
	    //  This is quite dirty code...
	    //  This is not a big deal as far as only mines are
	    //  concerned, but for more units (like parasited ones
	    //  in *craft), maybe we should create a dedicated queue...
	    if( unit->Command.Action==UnitActionMineGold ) {
	        mine=GoldMineOnMap(unit->X,unit->Y);
		if( mine ) {  // Somtimes, the peon is at home :).
		    MapMarkSight(mine->X+mine->Type->TileWidth/2
				 ,mine->Y+mine->Type->TileHeight/2
				 ,mine->Stats->SightRange);
		}
	    } else {
	        continue;
	    }
	}
	MapMarkSight(unit->X+unit->Type->TileWidth/2
		,unit->Y+unit->Type->TileHeight/2
		,unit->Stats->SightRange);
    }
}

/*----------------------------------------------------------------------------
--	Draw fog solid
----------------------------------------------------------------------------*/

/**
**	Fast draw solid fog of war 32x32 tile for 16 bpp video modes.
**
**	Color #239 in palette marks no transparent.
**
**	@param data	pointer to tile graphic data.
**	@param x	X position into video memory.
**	@param y	Y position into video memory.
*/
global void VideoDraw16Fog32Solid(const GraphicData* data,int x,int y)
{
    const unsigned char* sp;
    const unsigned char* gp;
    VMemType16* dp;
    int da;

    sp=data;
    gp=sp+TileSizeY*TileSizeX;
    dp=VideoMemory16+x+y*VideoWidth;
    da=VideoWidth;

    while( sp<gp ) {
#undef UNROLL1
#define UNROLL1(x)	\
	if( sp[x]==239 ) {		\
	    dp[x]=Pixels[239];		\
	}

#undef UNROLL2
#define UNROLL2(x)	\
	UNROLL1(x+0);

	UNROLL32(0);

	sp+=TileSizeX;
	dp+=da;

#undef UNROLL2
#define UNROLL2(x)	\
	UNROLL1(x+1);

	UNROLL32(0);

	sp+=TileSizeX;
	dp+=da;
    }
}

/**
**	Fast draw solid 100% fog of war 32x32 tile for 16 bpp video modes.
**
**	100% fog of war -- i.e. raster	10101.
**					01010 etc...
**
**	@param data	pointer to tile graphic data.
**	@param x	X position into video memory
**	@param y	Y position into video memory
*/
global void VideoDraw16OnlyFog32Solid(const GraphicData* data,int x,int y)
{
    const VMemType16* gp;
    VMemType16* dp;
    int da;

    dp=VideoMemory16+x+y*VideoWidth;
    gp=dp+VideoWidth*TileSizeX;
    da=VideoWidth;
    while( dp<gp ) {
#undef UNROLL2
#define UNROLL2(x)		\
	dp[x+0]=Pixels[239];
	UNROLL32(0);
	dp+=da;

#undef UNROLL2
#define UNROLL2(x)		\
	dp[x+1]=Pixels[239];
	UNROLL32(0);
	dp+=da;
    }
}

/**
**	Fast draw solid unexplored 32x32 tile for 16 bpp video modes.
**
**	Color #239 in palette marks no transparent.
**
**	@param x	X position into video memory
**	@param y	Y position into video memory
*/
global void VideoDraw16Unexplored32Solid(const GraphicData* data,int x,int y)
{
    const unsigned char* sp;
    const unsigned char* gp;
    VMemType16* dp;
    int da;

    sp=data;
    gp=sp+TileSizeY*TileSizeX;
    dp=VideoMemory16+x+y*VideoWidth;
    da=VideoWidth;

    while( sp<gp ) {
#undef UNROLL1
#define UNROLL1(x)	\
	if( sp[x]==239 ) {		\
	    dp[x]=Pixels[239];		\
	}

#undef UNROLL2
#define UNROLL2(x)	\
	UNROLL1(x+0);	\
	UNROLL1(x+1);

	UNROLL32(0);
	sp+=TileSizeX;
	dp+=da;
    }
}

/*----------------------------------------------------------------------------
--	Draw real fog :-)
----------------------------------------------------------------------------*/

#if 0

#define RMASK	(0x1F<< 0)
#define GMASK	(0x1F<< 5)
#define BMASK	(0x1F<<10)
#define RSHFT	( 0)
#define GSHFT	( 5)
#define BSHFT   (10)
#define RLOSS	( 3)
#define GLOSS	( 3)
#define BLOSS	( 3)

/**
**	Fast draw alpha fog of war 32x32 tile for 16 bpp video modes.
**
**	Color #239 in palette marks no transparent.
**
**	@param data	pointer to tile graphic data.
**	@param x	X position into video memory
**	@param y	Y position into video memory
*/
global void VideoDraw16Fog32Alpha(const GraphicData* data,int x,int y)
{
    const unsigned char* sp;
    const unsigned char* gp;
    VMemType16* dp;
    int da;
    int o;
    int p;
    int r;
    int g;
    int b;
    int v;

    sp=data;
    gp=sp+TileSizeY*TileSizeX;
    dp=VideoMemory16+x+y*VideoWidth;
    da=VideoWidth;

    while( sp<gp ) {
	o=rand();
#undef UNROLL1
#define UNROLL1(x)	\
	if( sp[x]==239 ) {		\
	    p=dp[x];			\
	    r=(p&RMASK)>>RSHFT<<RLOSS;	\
	    g=(p&GMASK)>>GSHFT<<GLOSS;	\
	    b=(p&BMASK)>>BSHFT<<BLOSS;	\
	    v=(r+g+b)/4+((o&1)<<RLOSS);	\
	    p=((v>>RLOSS)<<RSHFT)	\
	      |((v>>GLOSS)<<GSHFT)	\
	      |((v>>BLOSS)<<BSHFT);	\
	    dp[x]=p;			\
	    o>>=1;			\
	}

#undef UNROLL2
#define UNROLL2(x)	\
	UNROLL1(x+0);	\
	UNROLL1(x+1);

	UNROLL32(0);
	sp+=TileSizeX;
	dp+=da;
    }
}

/**
**	Fast draw 100% fog of war 32x32 tile for 16 bpp video modes.
**
**	100% fog of war -- i.e. raster	10101.
**					01010 etc...
**
**	@param data	pointer to tile graphic data.
**	@param x	X position into video memory
**	@param y	Y position into video memory
*/
global void VideoDraw16OnlyFog32Alpha(const GraphicData* data,int x,int y)
{
    const VMemType16* gp;
    VMemType16* dp;
    int da;
    int p;
    int r;
    int g;
    int b;
    int v;
    int o;

    dp=VideoMemory16+x+y*VideoWidth;
    gp=dp+VideoWidth*TileSizeX;
    da=VideoWidth;
    while( dp<gp ) {
	o=rand();
#undef UNROLL1
#define UNROLL1(x)	\
	p=dp[x];			\
	r=(p&RMASK)>>RSHFT<<RLOSS;	\
	g=(p&GMASK)>>GSHFT<<GLOSS;	\
	b=(p&BMASK)>>BSHFT<<BLOSS;	\
	v=(r+g+b)/4+((o&1)<<RLOSS);	\
	p=((v>>RLOSS)<<RSHFT)		\
	  |((v>>GLOSS)<<GSHFT)		\
	  |((v>>BLOSS)<<BSHFT);		\
	dp[x]=p;			\
	o>>=1;

#undef UNROLL2
#define UNROLL2(x)	\
	UNROLL1(x+0);	\
	UNROLL1(x+1);

	UNROLL32(0);
	dp+=da;
    }
}

#endif

/**
**	Fast draw alpha fog of war 32x32 tile for 16 bpp video modes.
**
**	Color #239 in palette marks no transparent.
**
**	@param data	pointer to tile graphic data.
**	@param x	X position into video memory
**	@param y	Y position into video memory
	    o=(o>>1)^((o&1)<<30);					\
*/
global void VideoDraw16Fog32Alpha(const GraphicData* data,int x,int y)
{
    const unsigned char* sp;
    const unsigned char* gp;
    VMemType16* dp;
    int da;
    int o;

    sp=data;
    gp=sp+TileSizeY*TileSizeX;
    dp=VideoMemory16+x+y*VideoWidth;
    da=VideoWidth;

    while( sp<gp ) {
	static int a=1234567;
	//o=rand();

#undef UNROLL1
#define UNROLL1(x)	\
	o=a=a*(123456*4+1)+1; \
	if( sp[x]==239 ) {		\
	    dp[x]=((VMemType16*)FogOfWarAlphaTable)[dp[x]/*^((o>>20)&4)*/];	\
	}

#undef UNROLL2
#define UNROLL2(x)	\
	UNROLL1(x+0);	\
	UNROLL1(x+1);	\
	//o=(o>>1)|((o&1)<<31);

	UNROLL32(0);
	sp+=TileSizeX;
	dp+=da;
    }
}

/**
**	Fast draw 100% fog of war 32x32 tile for 16 bpp video modes.
**
**	100% fog of war -- i.e. raster	10101.
**					01010 etc...
**
**	@param data	pointer to tile graphic data.
**	@param x	X position into video memory
**	@param y	Y position into video memory
	o=(o>>1)|((o&1)<<30);
*/
global void VideoDraw16OnlyFog32Alpha(const GraphicData* data,int x,int y)
{
    const VMemType16* gp;
    VMemType16* dp;
    int da;
    int o;

    dp=VideoMemory16+x+y*VideoWidth;
    gp=dp+VideoWidth*TileSizeX;
    da=VideoWidth;

    while( dp<gp ) {
	static int a=1234567;
	//o=rand();

#undef UNROLL1
#define UNROLL1(x)	\
	o=a=a*(123456*4+1)+1; \
	dp[x]=((VMemType16*)FogOfWarAlphaTable)[dp[x]/*^((o>>20)&4)*/];	\

#undef UNROLL2
#define UNROLL2(x)	\
	UNROLL1(x+0);	\
	UNROLL1(x+1);	\
	//o=(o>>1)|((o&1)<<31);

	UNROLL32(0);
	dp+=da;
    }
}

/*----------------------------------------------------------------------------
--	New better version, not working
----------------------------------------------------------------------------*/

#ifndef OLD_FOG

#if 0
	|--|--|--|
	| 1|  |2 |
	|--|--|--|
	|  |XX|  |
	|--|--|--|
	| 8|  |4 |
	|--|--|--|
#endif

static int NewFogTable[16] = {
      0,  9,  7,  8,	// 0-3
      1,  0,  4,  0,	// 4-7
      3,  6,  0,  0,    // 8-B
      2,  0,  0,  0,	// C-F
};

/**
**	Draw unexplored tiles.
**
**	@param sx	Offset into fields to current tile.
**	@param sy	Start of the current row.
**	@param dx	X position into video memory.
**	@param dy	Y position into video memory.
*/
local void DrawFogOfWarTile(int sx,int sy,int dx,int dy)
{
    int tile;
    int w;

    w=TheMap.Width;
    tile=0;
    //
    //	Check each field around it
    //
    if( sy ) {
	if( sx!=sy && !(TheMap.Fields[sx-w-1].Flags&MapFieldVisible) ) {
	    tile|=2;
	}
	if( !(TheMap.Fields[sx-w].Flags&MapFieldVisible) ) {
	    tile|=3;
	}
	if( sx!=sy+w-1 && !(TheMap.Fields[sx-w+1].Flags&MapFieldVisible) ) {
	    tile|=1;
	}
    }
    if( sx!=sy && !(TheMap.Fields[sx-1].Flags&MapFieldVisible) ) {
	tile|=10;
    }
    if( sx!=sy+w-1 && !(TheMap.Fields[sx+1].Flags&MapFieldVisible) ) {
	tile|=5;
    }
    if( sy+w<TheMap.Height*w ) {
	if( sx!=sy && !(TheMap.Fields[sx+w-1].Flags&MapFieldVisible) ) {
	    tile|=8;
	}
	if( !(TheMap.Fields[sx+w].Flags&MapFieldVisible) ) {
	    tile|=12;
	}
	if( sx!=sy+w-1 && !(TheMap.Fields[sx+w+1].Flags&MapFieldVisible) ) {
	    tile|=4;
	}
    }

    tile=NewFogTable[tile];
    if( tile ) {
	VideoDraw16Fog32(TheMap.Tiles[tile],dx,dy);
    } else {
	VideoDrawTile(TheMap.Tiles[UNEXPLORED_TILE],dx,dy);
    }
}

#endif

/*----------------------------------------------------------------------------
--	Old version correct working but not 100% original
----------------------------------------------------------------------------*/

#ifdef OLD_FOG

/**
**	Draw fog of war tile.
**
**	@param sx	Offset into fields to current tile.
**	@param sy	Start of the current row.
**	@param dx	X position into video memory.
**	@param dy	Y position into video memory.
*/
local void DrawFogOfWarTile(int sx,int sy,int dx,int dy)
{
    int w;
    int tile;
    int tile2;

    w=TheMap.Width;
    tile=tile2=0;
    //
    //	Check each field around it
    //
    if( sy ) {
	if( sx!=sy ) {
	    if( !(TheMap.Fields[sx-w-1].Flags&MapFieldExplored) ) {
		tile2|=2;
		tile|=2;
	    } else if( !(TheMap.Fields[sx-w-1].Flags&MapFieldVisible) ) {
		tile|=2;
	    }
	}
	if( !(TheMap.Fields[sx-w].Flags&MapFieldExplored) ) {
	    tile2|=3;
	    tile|=3;
	} else if( !(TheMap.Fields[sx-w].Flags&MapFieldVisible) ) {
	    tile|=3;
	}
	if( sx!=sy+w-1 ) {
	    if( !(TheMap.Fields[sx-w+1].Flags&MapFieldExplored) ) {
		tile2|=1;
		tile|=1;
	    } else if( !(TheMap.Fields[sx-w+1].Flags&MapFieldVisible) ) {
		tile|=1;
	    }
	}
    }

    if( sx!=sy ) {
	if( !(TheMap.Fields[sx-1].Flags&MapFieldExplored) ) {
	    tile2|=10;
	    tile|=10;
	} else if( !(TheMap.Fields[sx-1].Flags&MapFieldVisible) ) {
	    tile|=10;
	}
    }
    if( sx!=sy+w-1 ) {
	if( !(TheMap.Fields[sx+1].Flags&MapFieldExplored) ) {
	    tile2|=5;
	    tile|=5;
	} else if( !(TheMap.Fields[sx+1].Flags&MapFieldVisible) ) {
	    tile|=5;
	}
    }

    if( sy+w<TheMap.Height*w ) {
	if( sx!=sy ) {
	    if( !(TheMap.Fields[sx+w-1].Flags&MapFieldExplored) ) {
		tile2|=8;
		tile|=8;
	    } else if( !(TheMap.Fields[sx+w-1].Flags&MapFieldVisible) ) {
		tile|=8;
	    }
	}
	if( !(TheMap.Fields[sx+w].Flags&MapFieldExplored) ) {
	    tile2|=12;
	    tile|=12;
	} else if( !(TheMap.Fields[sx+w].Flags&MapFieldVisible) ) {
	    tile|=12;
	}
	if( sx!=sy+w-1 ) {
	    if( !(TheMap.Fields[sx+w+1].Flags&MapFieldExplored) ) {
		tile2|=4;
		tile|=4;
	    } else if( !(TheMap.Fields[sx+w+1].Flags&MapFieldVisible) ) {
		tile|=4;
	    }
	}
    }

    //
    //	Draw unexplored area
    //	If only partly or total invisible draw fog of war.
    //
    tile=FogTable[tile];
    tile2=FogTable[tile2];
    if( tile2 ) {
	VideoDrawUnexplored(TheMap.Tiles[tile2],dx,dy);
	if( tile2==tile ) {		// no same fog over unexplored
	    tile=0;
	}
    }
    if( !TheMap.NoFogOfWar ) {
	if( (TheMap.Fields[sx].Flags&MapFieldVisible) ) {
	    if( tile ) {
		VideoDrawFog(TheMap.Tiles[tile],dx,dy);
	    }
	} else {
	    VideoDrawOnlyFog(TheMap.Tiles[UNEXPLORED_TILE],dx,dy);
	}
    }
}

#endif

/**
**	Draw the map fog of war.
**
**	@param x	Map viewpoint x position.
**	@param y	Map viewpoint y position.
*/
global void DrawMapFogOfWar(int x,int y)
{
    int sx;
    int sy;
    int dx;
    int ex;
    int dy;
    int ey;
    const char* redraw_row;
    const char* redraw_tile;
#ifdef TIMEIT
    u_int64_t sv=rdtsc();
    u_int64_t ev;
    static long mv=9999999;
#endif
    redraw_row=MustRedrawRow;		// flags must redraw or not
    redraw_tile=MustRedrawTile;

    ex=MAP_X+MapWidth*TileSizeX;
    sy=y*TheMap.Width;
    dy=MAP_Y;
    ey=dy+MapHeight*TileSizeX;

    while( dy<ey ) {
	if( *redraw_row++ ) {		// row must be redrawn
	    sx=x+sy;
	    dx=MAP_X;
	    while( dx<ex ) {
		if( *redraw_tile++ ) {
#ifdef OLD_FOG
		    if( TheMap.Fields[sx].Flags&MapFieldExplored ) {
			DrawFogOfWarTile(sx,sy,dx,dy);
		    } else {
			VideoDrawTile(TheMap.Tiles[UNEXPLORED_TILE],dx,dy);
		    }
#else
		    if( !(TheMap.Fields[sx].Flags&MapFieldExplored) ) {
			DrawUnexploredTile(sx,sy,dx,dy);
		    } else if( !(TheMap.Fields[sx].Flags&MapFieldVisible) ) {
			DrawFogOfWarTile(sx,sy,dx,dy);
		    }
#endif
		}
		++sx;
		dx+=TileSizeX;
	    }
	} else {
	    redraw_tile+=MapWidth;
	}
	sy+=TheMap.Width;
	dy+=TileSizeY;
    }

#ifdef TIMEIT
    ev=rdtsc();
    sx=(ev-sv);
    if( sx<mv ) {
	mv=sx;
    }

    //DebugLevel0("%d\n",countit);
    DebugLevel1("%ld %ld %3ld\n",(long)sx,mv,(sx*100)/mv);
#endif
}

/**
**	Initialise the fog of war.
**	Build tables, setup functions.
*/
global void InitMapFogOfWar(void)
{
    if( !OriginalFogOfWar ) {
	int i;
	int n;
	int v;
	int r,g,b;
	int rmask,gmask,bmask;
	int rshft,gshft,bshft;
	int rloss,gloss,bloss;

	switch( VideoDepth ) {
	    case 15:			// 15 bpp video depth
		rmask=(0x1F<< 0);
		gmask=(0x1F<< 5);
		bmask=(0x1F<<10);
		rshft=( 0);
		gshft=( 5);
		bshft=(10);
		rloss=( 3);
		gloss=( 3);
		bloss=( 3);

		n=1<<(sizeof(VMemType16)*8);
		FogOfWarAlphaTable=malloc(n*sizeof(VMemType16));
		for( i=0; i<n; ++i ) {
		    r=(i&rmask)>>rshft<<rloss;
		    g=(i&gmask)>>gshft<<gloss;
		    b=(i&bmask)>>bshft<<bloss;
		    /*
		    v=((r+g+b)*FogOfWarContrast)/300
			+(256*FogOfWarBrightness)/100;

		    r=g=b=v;
		    */

		    v=r+g+b;

		    r= ((((r*3-v)*FogOfWarSaturation + v*100)
			*FogOfWarContrast)
			+FogOfWarBrightness*25600*3)/30000;
		    g= ((((g*3-v)*FogOfWarSaturation + v*100)
			*FogOfWarContrast)
			+FogOfWarBrightness*25600*3)/30000;
		    b= ((((b*3-v)*FogOfWarSaturation + v*100)
			*FogOfWarContrast)
			+FogOfWarBrightness*25600*3)/30000;

		    // Boundings
		    r= r<0 ? 0 : r>255 ? 255 : r;
		    g= g<0 ? 0 : g>255 ? 255 : g;
		    b= b<0 ? 0 : b>255 ? 255 : b;
		    ((VMemType16*)FogOfWarAlphaTable)[i]=((r>>rloss)<<rshft)
			    |((g>>gloss)<<gshft)
			    |((b>>bloss)<<bshft);
		}
		VideoDrawFog=VideoDraw16Fog32Alpha;
		VideoDrawOnlyFog=VideoDraw16OnlyFog32Alpha;
		VideoDrawUnexplored=VideoDraw16Unexplored32Solid;
		break;
	    case 16:			// 16 bpp video depth
		rmask=(0x1F<< 0);
		gmask=(0x3F<< 5);
		bmask=(0x1F<<11);
		rshft=( 0);
		gshft=( 5);
		bshft=(11);
		rloss=( 3);
		gloss=( 2);
		bloss=( 3);

		n=1<<(sizeof(VMemType16)*8);
		FogOfWarAlphaTable=malloc(n*sizeof(VMemType16));
		for( i=0; i<n; ++i ) {
		    r=(i&rmask)>>rshft<<rloss;
		    g=(i&gmask)>>gshft<<gloss;
		    b=(i&bmask)>>bshft<<bloss;
		    //v=(r+g+b)/4;
		    v=r+g+b;

		    r= ((((r*3-v)*FogOfWarSaturation + v*100)
			*FogOfWarContrast)
			+FogOfWarBrightness*25600*3)/30000;
		    g= ((((g*3-v)*FogOfWarSaturation + v*100)
			*FogOfWarContrast)
			+FogOfWarBrightness*25600*3)/30000;
		    b= ((((b*3-v)*FogOfWarSaturation + v*100)
			*FogOfWarContrast)
			+FogOfWarBrightness*25600*3)/30000;

		    // Boundings
		    r= r<0 ? 0 : r>255 ? 255 : r;
		    g= g<0 ? 0 : g>255 ? 255 : g;
		    b= b<0 ? 0 : b>255 ? 255 : b;
		    ((VMemType16*)FogOfWarAlphaTable)[i]=((v>>rloss)<<rshft)
			    |((v>>gloss)<<gshft)
			    |((v>>bloss)<<bshft);
		}
		VideoDrawFog=VideoDraw16Fog32Alpha;
		VideoDrawOnlyFog=VideoDraw16OnlyFog32Alpha;
		VideoDrawUnexplored=VideoDraw16Unexplored32Solid;
		break;
	    // FIXME: support for 8/32 bpp modes
	}
    } else {
	switch( VideoDepth ) {
	    case 15:			// 15 bpp video depth
	    case 16:			// 16 bpp video depth
		VideoDrawFog=VideoDraw16Fog32Solid;
		VideoDrawOnlyFog=VideoDraw16OnlyFog32Solid;
		VideoDrawUnexplored=VideoDraw16Unexplored32Solid;
		break;
	    // FIXME: support for 8/32 bpp modes
	}
    }
}

//@}
