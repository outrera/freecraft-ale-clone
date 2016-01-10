/*
**	A clone of a famous game.
*/
/**@name pud.c		-	The pud. */
/*
**	(c) Copyright 1998-2000 by Lutz Sammer
**
**	$Id: pud.c,v 1.35 2000/01/19 09:54:15 root Exp $
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
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "pud.h"

#ifdef USE_ZLIB
#include <zlib.h>
#endif

#ifdef USE_BZ2LIB
#include <bzlib.h>
#endif

#if !defined(__CYGWIN__) && !defined(__MINGW32__)
#include <endian.h>
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
#include <byteswap.h>
#endif
#endif

/*----------------------------------------------------------------------------
--	Defines
----------------------------------------------------------------------------*/

#if !defined(USE_ZLIB) && !defined(USE_BZ2LIB) 
// FIXME: Move this to central point,

#define gzFile				FILE
#define gzopen				fopen
#define gzread(file,buf,len)		fread(buf,1,len,file)
#define gzseek(file,offset,whence)	fseek(file,offset,whence)
#define gzclose(file)			fclose(file)

#endif	// !USE_ZLIB && !USE_BZ2LIB

#ifdef USE_BZ2LIB

#define gzFile				BZFILE
#define gzopen				bzopen
#define gzread(file,buf,len)		bzread(file,buf,len)
#define gzseek(file,offset,whence)	bzseek(file,offset,whence)
#define gzclose(file)			bzclose(file)

#endif	// !USE_BZ2LIB

//
//	Macros for byte/word/long operations
//
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN

#define DoWord(v)	bswap_16((v))
#define DoLong(v)	bswap_32((v))
#define GetByte(p)	(*((unsigned char*)(p)))
#define GetWord(p)	bswap_16(*((unsigned short*)(p)))
#define GetLong(p)	bswap_32(*((unsigned long*)(p)))
#define FetchByte(p)	(*((unsigned char*)(p))++)
#define FetchWord(p)	bswap_16(*((unsigned short*)(p))++)
#define FetchLong(p)	bswap_32(*((unsigned long*)(p))++)

#else

#define DoWord(v)	(v)
#define DoLong(v)	(v)
#define GetByte(p)	(*((unsigned char*)(p)))
#define GetWord(p)	(*((unsigned short*)(p)))
#define GetLong(p)	(*((unsigned long*)(p)))
#define FetchByte(p)	(*((unsigned char*)(p))++)
#define FetchWord(p)	(*((unsigned short*)(p))++)
#define FetchLong(p)	(*((unsigned long*)(p))++)

#endif

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

local int MapOffsetX;			/// Offset X for combined maps
local int MapOffsetY;			/// Offset Y for combined maps

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

#ifdef USE_ZLIB

#ifndef z_off_t				// { ZLIB_VERSION<="1.0.4"

/**
**	Seek on compressed input. (Newer libs support it directly)
**
**	@param file	File handle
**	@param offset	Seek position
**	@param whence	How to seek
*/
local void gzseek(gzFile* file,unsigned offset,int whence)
{
    char buf[32];

    while( offset>sizeof(buf) ) {
	gzread(file,buf,sizeof(buf));
	offset-=sizeof(buf);
    }
    gzread(file,buf,offset);
}

#endif	// } ZLIB_VERSION<="1.0.4"

#endif	// USE_ZLIB

#ifdef USE_BZ2LIB

/**
**	Seek on compressed input. (I hope newer libs support it directly)
**
**	@param file	File handle
**	@param offset	Seek position
**	@param whence	How to seek
*/
local void bzseek(BZFILE* file,unsigned offset,int whence)
{
    char buf[32];

    while( offset>sizeof(buf) ) {
	bzread(file,buf,sizeof(buf));
	offset-=sizeof(buf);
    }
    bzread(file,buf,offset);
}

#endif	// USE_BZ2LIB

/*============================================================================
==	Convert
============================================================================*/

/**
**	Convert puds MTXM section into internal format.
**
**	@param mtxm	Section data
**	@param width	Section width
**	@param height	Section height
**	@param map	Map to store into
*/
local void ConvertMTXM(const unsigned short* mtxm,int width,int height
	    ,WorldMap* map)
{
    const unsigned short* ctab;
    int h;
    int w;

    DebugCheck( UnitTypes[UnitWallOrc]._HitPoints>=256
	    || UnitTypes[UnitWallHuman]._HitPoints>=256 );

    if( map->Terrain<TilesetMax ) {
	ctab=Tilesets[map->Terrain].Table;
    } else {
	DebugLevel1("Unknown terrain!\n");
	ctab=Tilesets[TilesetSummer].Table;
    }

    for( h=0; h<height; ++h ) {
	for( w=0; w<width; ++w ) {
	    int v;

	    v=DoWord(mtxm[h*width+w]);
	    map->Fields[MapOffsetX+w+(MapOffsetY+h)*TheMap.Width].Tile=ctab[v];
	    //
	    //	Walls are handled special (very ugly).
	    //
	    if( (v&0xFFF0)==0x00A0
		    || (v&0xFFF0)==0x00C0
		    || (v&0xFF00)==0x0900 ) {
		map->Fields[MapOffsetX+w+(MapOffsetY+h)*TheMap.Width].Value=
			UnitTypes[UnitWallOrc]._HitPoints;
	    } else if( (v&0x00F0)==0x0090
		    || (v&0xFFF0)==0x00B0
		    || (v&0xFF00)==0x0800 ) {
		map->Fields[MapOffsetX+w+(MapOffsetY+h)*TheMap.Width].Value=
			UnitTypes[UnitWallHuman]._HitPoints;
	    }
	}
    }
}

/**
**	Convert puds SQM section into our internal format.
**
**	@param sqm	Section data
**	@param width	Section width
**	@param height	Section height
**	@param map	Map to store into
*/
local void ConvertSQM(const unsigned short* sqm,int width,int height
	,WorldMap* map)
{
    int h;
    int w;
    int i;
    int v;

    for( h=0; h<height; ++h ) {
	for( w=0; w<width; ++w ) {
	    v=DoWord(sqm[w+h*width]);
	    i=MapOffsetX+w+(MapOffsetY+h)*TheMap.Width;
	    if( v&MapMoveOnlyLand ) {
		map->Fields[i].Flags|=MapFieldLandAllowed;
	    }
	    if( v&MapMoveCoast ) {
		map->Fields[i].Flags|=MapFieldCoastAllowed;
	    }
	    if( v&MapMoveWallO ) {
		if( !map->Fields[i].Flags&MapFieldWall ) {
		    DebugLevel0("Should already be wall %d\n",i);
		    map->Fields[i].Flags|=MapFieldWall;
		}
	    }
	    if( v&MapMoveHuman ) {
		if( !map->Fields[i].Flags&MapFieldWall ) {
		    DebugLevel0("Should already be wall %d\n",i);
		    map->Fields[i].Flags|=MapFieldWall;
		}
		map->Fields[i].Flags|=MapFieldHuman;
	    }
	    if( v&MapMoveDirt ) {
		map->Fields[i].Flags|=MapFieldNoBuilding;
	    }
	    if( v&MapMoveOnlyWater ) {
		map->Fields[i].Flags|=MapFieldWaterAllowed;
	    }
	    if( v&MapMoveUnpassable ) {
		map->Fields[i].Flags|=MapFieldUnpassable;
	    }
	    if( v&MapMoveLandUnit ) {
		map->Fields[i].Flags|=MapFieldLandUnit;
	    }
	    if( v&MapMoveAirUnit ) {
		map->Fields[i].Flags|=MapFieldAirUnit;
	    }
	    if( v&MapMoveSeaUnit ) {
		map->Fields[i].Flags|=MapFieldSeaUnit;
	    }
	    if( v&MapMoveBuildingUnit ) {
		map->Fields[i].Flags|=MapFieldBuilding;
	    }
	    if( v&0x20 ) {
		DebugLevel0("SQM: contains unknown action %#04X\n",v);
	    }
	}
    }
}

/**
**	Convert puds REGM section into internal format.
**
**	@param regm	Section data
**	@param width	Section width
**	@param height	Section height
**	@param map	Map to store into
*/
local void ConvertREGM(const unsigned short* regm,int width,int height
	,WorldMap* map)
{
    int h;
    int w;
    int i;
    int v;

    for( h=0; h<height; ++h ) {
	for( w=0; w<width; ++w ) {
	    v=DoWord(regm[w+h*width]);
	    i=MapOffsetX+w+(MapOffsetY+h)*TheMap.Width;
	    if( v==MapActionForest ) {	// forest could be chopped
		map->Fields[i].Flags|=MapFieldForest;
		continue;
	    }
	    if( v==MapActionRocks ) {	// rocks could be blown away
		map->Fields[i].Flags|=MapFieldRocks;
		continue;
	    }
	    if( v==MapActionWall ) {	// wall could be destoryed
		map->Fields[i].Flags|=MapFieldWall;
		continue;
	    }
	    if( v==MapActionIsland ) {	// island no transporter
		// FIXME: don't know what todo here
		//map->Fields[i].Flags|=MapFieldWall;
		DebugLevel0(__FUNCTION__": %d,%d %d\n",w,h,v);
		continue;
	    }
	    v&=~0xFF;			// low byte is region
	    if( v==MapActionWater ) {	// water
		continue;
	    }
	    if( v==MapActionLand ) {	// land
		continue;
	    }
	    DebugLevel0("REGM: contains unknown action %#04X at %d,%d\n"
		,v,w,h);
	}
    }
}

/*============================================================================
==	Read
============================================================================*/

/**
**	Read header of pud:
**
**	@param input	Input file
**	@param header	Header is filled in.
**	@param length	Length is filled in.
**
**		4 bytes header tag (TYPE )...
**		long	length
*/
local int PudReadHeader(gzFile* input,char* header,long* length)
{
    long len;

    if( gzread(input,header,4)!=4 ) {
	return 0;
    }
    if( gzread(input,&len,4)!=4 ) {
	perror("gzread()");
	exit(-1);
    }
    *length=DoLong(len);
    return 1;
}

/**
**	Read word from pud.
**
**	@param input	Input file
*/
local int PudReadWord(gzFile* input)
{
    unsigned short temp_short;

    if( gzread(input,&temp_short,2)!=2 ) {
	perror("gzread()");
	exit(-1);
    }

    return DoWord(temp_short);
}

/**
**	Read byte from pud.
**
**	@param input	Input file
*/
local int PudReadByte(gzFile* input)
{
    unsigned char temp_char;

    if( gzread(input,&temp_char,1)!=1 ) {
	perror("gzread()");
	exit(-1);
    }

    return temp_char;
}

/**
**	Load pud.
**
**	@param pud	File name.
**	@param map	Map filled in.
*/
global void LoadPud(const char* pud,WorldMap* map)
{
    gzFile* input;
    long length;
    char header[5];
    char buf[1024];
    int width;
    int height;

    // FIXME: this should become this
#if 0
    if( !(input=LibOpen(pud)) ) {
	perror("pud: LibOpen()");
	exit(-1);
    }
#endif

    if( !(input=gzopen(pud,"rb")) ) {		// try direct
#ifdef USE_ZLIB
	sprintf(buf,"%s.gz",pud);
	if( !(input=gzopen(buf,"rb")) )
#endif
#ifdef USE_BZ2LIB
	sprintf(buf,"%s.bz2",pud);
	if( !(input=gzopen(buf,"rb")) )
#endif
	{
	    //fprintf(stderr,"%s in ",buf);
	    perror("pud: fopen,gzopen,bzopen()");
	    exit(-1);
	}
    }
    header[4]='\0';
    if( !PudReadHeader(input,header,&length) ) {
	fprintf(stderr,"invalid pud\n");
	exit(-1);
    }
    if( memcmp(header,"TYPE",4) || length!=16 ) {
	fprintf(stderr,"invalid pud\n");
	exit(-1);
    }
    if( gzread(input,buf,16)!=16 ) {	// IGNORE TYPE
	perror("gzread()");
	exit(-1);
    }
    if( strcmp(buf,"WAR2 MAP") ) {	// ONLY CHECK STRING
	fprintf(stderr,"invalid pud\n");
	exit(-1);
    }

    width=height=0;

    //
    //	Parse all sections.
    //
    while( PudReadHeader(input,header,&length) ) {
	DebugLevel3("\tSection: %4.4s\n",header);

	//
	//	PUD version
	//
	if( !memcmp(header,"VER ",4) ) {
	    if( length==2 ) {
		int v;

		v=PudReadWord(input);
		DebugLevel1("\tVER: %d.%d\n",(v&0xF0)>>4,v&0xF);
		continue;
	    }
	    DebugLevel1("Wrong version length\n");
	}

	//
	//	Map description
	//
	if( !memcmp(header,"DESC",4) ) {
	    if( gzread(input,buf,length)!=length ) {
		perror("gzread()");
		exit(-1);
	    }
	    DebugLevel1("\tDESC: %s\n",buf);
	    strncpy(map->Description,buf,sizeof(map->Description));
	    map->Description[sizeof(map->Description)-1]='\0';
	    continue;
	}

	//
	//	Player definitons.
	//
	if( !memcmp(header,"OWNR",4) ) {
	    if( length==16 ) {
		int i;
		int p;

		for( i=0; i<16; ++i ) {
		    p=PudReadByte(input);
		    CreatePlayer("Computer",p);
		}
		continue;
	    } else {
		DebugLevel1("Wrong player length\n");
	    }
	}

	//
	//	Terrain type or extended terrain type.
	//
	if( !memcmp(header,"ERA ",4) || !memcmp(header,"ERAX",4) ) {
	    if( length==2 ) {
		int t;

		t=PudReadWord(input);
		switch( t ) {
		    case TilesetSummer:
			DebugLevel3("\tTerrain: SUMMER\n");
			break;
		    case TilesetWinter:
			break;
		    case TilesetWasteland:
			DebugLevel3("\tTerrain: WASTELAND\n");
			break;
		    case TilesetSwamp:
			DebugLevel3("\tTerrain: SWAMP\n");
			break;
		    default:
			DebugLevel1("Unknown terrain %d\n",t);
			t=TilesetSummer;
			break;
		}
		map->Terrain=t;
		continue;
	    } else {
		DebugLevel1("Wrong terrain type length\n");
	    }
	}


	//
	//	Dimension
	//
	if( !memcmp(header,"DIM ",4) ) {

	    width=PudReadWord(input);
	    height=PudReadWord(input);

	    DebugLevel2("\tMap %d x %d\n",width,height);

	    if( !map->Fields ) {
		map->Width=width;
		map->Height=height;

		map->Fields=calloc(width*height,sizeof(*map->Fields));
		if( !map->Fields ) {
		    perror("calloc()");
		    exit(-1);
		}
		InitUnitCache();
	    }
	    continue;
	}

	//
	//	Unit data (optional)
	//
	if( !memcmp(header,"UDTA",4) ) {
	    char* bufp;

	    length-=2;
	    if( PudReadWord(input) ) {
		DebugLevel3("\tUsing default data\n");
		gzseek(input,length,SEEK_CUR);
	    } else {
		if( length<sizeof(buf) ) {
		    bufp=buf;
		} else if( !(bufp=alloca(length)) ) {
		    perror("alloca()");
		    exit(-1);
		}
		if( gzread(input,bufp,length)!=length ) {
		    perror("gzread()");
		    exit(-1);
		}
		ParsePudUDTA(bufp,length);
	    }
	    continue;
	}

	//
	//	Pud restrictions (optional)
	//
	if( !memcmp(header,"ALOW",4) ) {
	    if( PudReadWord(input) ) {
		DebugLevel3("\tUsing default data\n");
	    } else {
		DebugLevel1("// FIXME: ALOW support not written\n");
	    }
	    gzseek(input,length-2,SEEK_CUR);
	    continue;
	}

	//
	//	Upgrade data (optional)
	//
	if( !memcmp(header,"UGRD",4) ) {
	    if( PudReadWord(input) ) {
		DebugLevel3("\tUsing default data\n");
	    } else {
		DebugLevel1("// FIXME: UGRD support not written\n");
	    }
	    gzseek(input,length-2,SEEK_CUR);
	    continue;
	}

	//
	//	Identifies race of each player
	//
	if( !memcmp(header,"SIDE",4) ) {
	    if( length==16 ) {
		int i;
		int v;

		for( i=0; i<16; ++i ) {
		    v=PudReadByte(input);
		    switch( v ) {
			case PlayerRaceHuman:
			case PlayerRaceOrc:
			case PlayerRaceNeutral:
			    break;
			default:
			    DebugLevel1("Unknown race %d\n",v);
			    v=PlayerRaceNeutral;
			    break;
		    }
		    PlayerSetSide(&Players[i],v);
		}
		continue;
	    } else {
		DebugLevel1("Wrong side length\n");
	    }
	}

	//
	//	Starting gold
	//
	if( !memcmp(header,"SGLD",4) ) {
	    if( length==32 ) {
		int i;
		int v;

		for( i=0; i<16; ++i ) {
		    v=PudReadWord(input);
		    PlayerSetGold(&Players[i],v);
		}
		continue;
	    } else {
		DebugLevel1("Wrong starting gold length\n");
	    }
	}

	//
	//	Starting lumber
	//
	if( !memcmp(header,"SLBR",4) ) {
	    if( length==32 ) {
		int i;
		int v;

		for( i=0; i<16; ++i ) {
		    v=PudReadWord(input);
		    PlayerSetWood(&Players[i],v);
		}
		continue;
	    } else {
		DebugLevel1("Wrong starting lumber length\n");
	    }
	}

	//
	//	Starting oil
	//
	if( !memcmp(header,"SOIL",4) ) {
	    if( length==32 ) {
		int i;
		int v;

		for( i=0; i<16; ++i ) {
		    v=PudReadWord(input);
		    PlayerSetOil(&Players[i],v);
		}
		continue;
	    } else {
		DebugLevel1("Wrong starting oil length\n");
	    }
	}

	//
	//	AI for each player
	//
	if( !memcmp(header,"AIPL",4) ) {
	    if( length==16 ) {
		int i;
		int v;

		for( i=0; i<16; ++i ) {
		    v=PudReadByte(input);
		    PlayerSetAiNum(&Players[i],v);
		}
		continue;
	    } else {
		DebugLevel1("Wrong AI player length\n");
	    }
	}

	//
	//	obsolete oil map
	//
	if( !memcmp(header,"OILM",4) ) {
	    gzseek(input,length,SEEK_CUR);	// skip section
	    continue;
	}

	//
	//	Tiles MAP
	//
	if( !memcmp(header,"MTXM",4) ) {
	    unsigned short* mtxm;

	    if( length!=width*height*2 ) {
		DebugLevel1("wrong length of MTXM section %ld\n",length);
		exit(-1);
	    }
	    if( !(mtxm=alloca(length)) ) {
		perror("alloca()");
		exit(-1);
	    }
	    if( gzread(input,mtxm,length)!=length ) {
		perror("gzread()");
		exit(-1);
	    }

	    ConvertMTXM(mtxm,width,height,map);

	    continue;
	}

	//
	//	Movement MAP
	//
	if( !memcmp(header,"SQM ",4) ) {
	    unsigned short* sqm;

	    if( length!=width*height*sizeof(short) ) {
		DebugLevel1("wrong length of SQM  section %ld\n",length);
		exit(-1);
	    }
	    if( !(sqm=alloca(length)) ) {
		perror("alloca()");
		exit(-1);
	    }
	    if( gzread(input,sqm,length)!=length ) {
		perror("gzread()");
		exit(-1);
	    }

	    ConvertSQM(sqm,width,height,map);

	    continue;
	}

	//
	//	Action MAP
	//
	if( !memcmp(header,"REGM",4) ) {
	    unsigned short* regm;

	    if( length!=width*height*sizeof(short) ) {
		DebugLevel1("wrong length of REGM section %ld\n",length);
		exit(-1);
	    }
	    if( !(regm=alloca(length)) ) {
		perror("alloca()");
		exit(-1);
	    }
	    if( gzread(input,regm,length)!=length ) {
		perror("gzread()");
		exit(-1);
	    }

	    ConvertREGM(regm,width,height,map);

	    continue;
	}

	//
	//	Units
	//
	if( !memcmp(header,"UNIT",4) ) {
	    int x;
	    int y;
	    int t;
	    int o;
	    int v;
	    Unit* unit;

	    while( length>=8 ) {
		x=PudReadWord(input);
		y=PudReadWord(input);
		t=PudReadByte(input);
		o=PudReadByte(input);
		v=PudReadWord(input);

		if( t==UnitStartLocationHuman
			|| t==UnitStartLocationOrc ) {	// starting points?

		    Players[o].X=MapOffsetX+x;
		    Players[o].Y=MapOffsetY+y;
		} else {
		    unit=MakeUnitAndPlace(MapOffsetX+x,MapOffsetY+y
			    ,UnitTypeByWcNum(t),&Players[o]);
		    if( unit->Type->GoldMine || unit->Type->OilPatch ) {
			unit->Value=v*2500;
		    } else {
			// FIXME: active/inactive AI units!!
		    }
		    UpdateForNewUnit(unit,0);
		}

		length-=8;
	    }
	    continue;
	}

	DebugLevel2("Unsupported Section: %4.4s\n",header);

	gzseek(input,length,SEEK_CUR);
    }

    gzclose(input);

    DebugLevel3("Memory for pud %d\n"
	    ,width*height*sizeof(*map->Fields)
// FIXME: remove this
	    +width*height*sizeof(short)
	    +width*height*sizeof(short) );

    MapOffsetX+=width;
    if( MapOffsetX>=map->Width ) {
	MapOffsetX=0;
	MapOffsetY+=height;
    }
}

//@}
