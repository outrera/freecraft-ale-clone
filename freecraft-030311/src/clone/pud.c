//   ___________		     _________		      _____  __
//   \_	  _____/______   ____   ____ \_   ___ \____________ _/ ____\/  |_
//    |    __) \_  __ \_/ __ \_/ __ \/    \  \/\_  __ \__  \\   __\\   __|
//    |     \   |  | \/\  ___/\  ___/\     \____|  | \// __ \|  |   |  |
//    \___  /   |__|    \___  >\___  >\______  /|__|  (____  /__|   |__|
//	  \/		    \/	   \/	     \/		   \/
//  ______________________                           ______________________
//			  T H E   W A R   B E G I N S
//	   FreeCraft - A free fantasy real time strategy game engine
//
/**@name pud.c		-	The pud. */
//
//	(c) Copyright 1998-2003 by Lutz Sammer
//
//	FreeCraft is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published
//	by the Free Software Foundation; only version 2 of the License.
//
//	FreeCraft is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	$Id: pud.c,v 1.98 2003/03/05 07:44:08 jsalmon3 Exp $

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "freecraft.h"
#include "video.h"
#include "tileset.h"
#include "map.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "upgrade.h"
#include "player.h"
#include "unit.h"
#include "pud.h"
#include "iolib.h"
#include "settings.h"
#include "network.h"
#include "commands.h"

#include "myendian.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

local int MapOffsetX;			/// Offset X for combined maps
local int MapOffsetY;			/// Offset Y for combined maps

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/
/*============================================================================
==	Simple Helper
============================================================================*/

/**
**	Checksum a memory area using a simple hash
**
**	@param adr	Address of data
**	@param length	length of area
*/
local unsigned int ChksumArea(const unsigned char* adr,int length)
{
    unsigned int res;
    unsigned int rem;

    res = 0U;
    while (length--) {
	rem = res & 0xff000000;
	res <<= 8;
	res += *adr++;
	res |= (rem >> 24);
    }
    return res;
}

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

    DebugCheck( UnitTypeOrcWall->_HitPoints>=256
	    || UnitTypeHumanWall->_HitPoints>=256 );

    if( map->Terrain<TilesetMax ) {
	// FIXME: should use terrain name or better map->Tileset!!
	//DebugCheck( map->Tileset->Table != Tilesets[map->Terrain]->Table );
	ctab=Tilesets[map->Terrain]->Table;
	DebugLevel0Fn("FIXME: %s <-> %s\n" _C_ Tilesets[map->Terrain]->Class _C_
		map->TerrainName);
    } else {
	DebugLevel1("Unknown terrain!\n");
	// FIXME: don't use TilesetSummer
	ctab=Tilesets[TilesetSummer]->Table;
    }

    for( h=0; h<height; ++h ) {
	for( w=0; w<width; ++w ) {
	    int v;

	    v=ConvertLE16(mtxm[h*width+w]);
	    map->Fields[MapOffsetX+w+(MapOffsetY+h)*TheMap.Width].Tile=ctab[v];
	    map->Fields[MapOffsetX+w+(MapOffsetY+h)*TheMap.Width].Value=0;
	    //
	    //	Walls are handled special (very ugly).
	    //
	    if( (v&0xFFF0)==0x00A0
		    || (v&0xFFF0)==0x00C0
		    || (v&0xFF00)==0x0900 ) {
		map->Fields[MapOffsetX+w+(MapOffsetY+h)*TheMap.Width].Value=
			UnitTypeOrcWall->_HitPoints;
	    } else if( (v&0x00F0)==0x0090
		    || (v&0xFFF0)==0x00B0
		    || (v&0xFF00)==0x0800 ) {
		map->Fields[MapOffsetX+w+(MapOffsetY+h)*TheMap.Width].Value=
			UnitTypeHumanWall->_HitPoints;
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
	    v=ConvertLE16(sqm[w+h*width]);
	    i=MapOffsetX+w+(MapOffsetY+h)*TheMap.Width;
	    if( v&MapMoveOnlyLand ) {
		map->Fields[i].Flags|=MapFieldLandAllowed;
	    }
	    if( v&MapMoveCoast ) {
		map->Fields[i].Flags|=MapFieldCoastAllowed;
	    }
	    if( v&MapMoveWallO ) {
		if( !map->Fields[i].Flags&MapFieldWall ) {
		    DebugLevel0("Should already be wall %d\n" _C_ i);
		    map->Fields[i].Flags|=MapFieldWall;
		}
	    }
	    if( v&MapMoveHuman ) {
		if( !map->Fields[i].Flags&MapFieldWall ) {
		    DebugLevel0("Should already be wall %d\n" _C_ i);
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
		DebugLevel0("SQM: contains unknown action %#04X\n" _C_ v);
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
	    v=ConvertLE16(regm[w+h*width]);
	    i=MapOffsetX+w+(MapOffsetY+h)*TheMap.Width;
	    if( v==MapActionForest ) {	// forest could be chopped
		map->Fields[i].Flags|=MapFieldForest;
		continue;
	    }
	    if( v==MapActionRocks ) {	// rocks could be blown away
		map->Fields[i].Flags|=MapFieldRocks;
		continue;
	    }
	    if( v==MapActionWall ) {	// wall could be destroyed
		map->Fields[i].Flags|=MapFieldWall;
		continue;
	    }
	    if( v==MapActionIsland ) {	// island no transporter
		// FIXME: don't know what todo here
		//map->Fields[i].Flags|=MapFieldWall;
		DebugLevel0Fn("%d,%d %d\n" _C_ w _C_ h _C_ v);
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
		_C_ v _C_ w _C_ h);
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
local int PudReadHeader(CLFile* input,char* header,long* length)
{
    long len;

    if( CLread(input,header,4)!=4 ) {
	return 0;
    }
    if( CLread(input,&len,4)!=4 ) {
	perror("CLread()");
	return 0;
	// ExitFatal(-1);
    }
    *length=ConvertLE32(len);
    return 1;
}

/**
**	Read word from pud.
**
**	@param input	Input file
*/
local int PudReadWord(CLFile* input)
{
    unsigned short temp_short;

    if( CLread(input,&temp_short,2)!=2 ) {
	perror("CLread()");
	ExitFatal(-1);
    }

    return ConvertLE16(temp_short);
}

/**
**	Read byte from pud.
**
**	@param input	Input file
*/
local int PudReadByte(CLFile* input)
{
    unsigned char temp_char;

    if( CLread(input,&temp_char,1)!=1 ) {
	perror("CLread()");
	ExitFatal(-1);
    }

    return temp_char;
}

/**
**	Get the info for a pud.
*/
global MapInfo* GetPudInfo(const char* pud)
{
    CLFile* input;
    long length;
    char header[5];
    char buf[1024];
    MapInfo* info;
    unsigned short temp_short;
    // FIXME: Reuse the temporary alloca buffer...

    if( !(input=CLopen(pud)) ) {
	fprintf(stderr,"Try ./path/name\n");
	fprintf(stderr,"pud: CLopen(%s): %s\n", pud, strerror(errno));
	return NULL;
    }
    header[4]='\0';
    if( !PudReadHeader(input,header,&length) ) {
	fprintf(stderr,"GetPudInfo: %s: invalid pud\n", pud);
	return NULL;
    }
    if( memcmp(header,"TYPE",4) || length!=16 ) {
	fprintf(stderr,"GetPudInfo: %s: invalid pud\n", pud);
	return NULL;
    }
    if( CLread(input,buf,16)!=16 ) {	// IGNORE TYPE
	perror("CLread()");
	return NULL;
    }
    buf[16] = 0;
    if( strcmp(buf,"WAR2 MAP") ) {	// ONLY CHECK STRING
	fprintf(stderr,"GetPudInfo: %s: invalid pud type [%s]\n", pud, buf);
	return NULL;
    }

    info=calloc(1, sizeof(MapInfo));	// clears with 0
    if (!info) {
	return NULL;
    }

    //
    //	Parse all sections.
    //
    while( PudReadHeader(input,header,&length) ) {
	DebugLevel3("\tSection: %4.4s\n" _C_ header);

	info->MapUID += ChksumArea(header, 4);

	//
	//	PUD version
	//
	if( !memcmp(header,"VER ",4) ) {
	    if( length==2 ) {
		int v;

		v=PudReadWord(input);
		DebugLevel3("\tVER: %d.%d\n" _C_ (v&0xF0)>>4 _C_ v&0xF);
		buf[0] = v & 0xFF;
		info->MapUID += ChksumArea(buf, 1);
		continue;
	    }
	    DebugLevel1("Wrong version length\n");
	}

	//
	//	Map description
	//
	if( !memcmp(header,"DESC",4) ) {
	    if( CLread(input,buf,length)!=length ) {
		perror("CLread()");
		FreeMapInfo(info);
		return NULL;
	    }
	    info->MapUID += ChksumArea(buf, length);
	    info->Description=strdup(buf);
	    continue;
	}

	//
	//	Player definitions.
	//
	if( !memcmp(header,"OWNR",4) ) {
	    if( length==16 ) {
		int i;
		int p;

		for( i=0; i<16; ++i ) {
		    p=PudReadByte(input);
		    buf[0] = p & 0xFF;
		    info->MapUID += ChksumArea(buf, 1);
		    info->PlayerType[i]=p;
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
		int i;

		t=PudReadWord(input);
		//
		//	Look if we have this as tileset.
		//
		for( i=0; i<t && TilesetWcNames[i]; ++i ) {
		}
		if( !TilesetWcNames[i] ) {
		    t=0;
		}
		info->MapTerrainName=strdup(TilesetWcNames[t]);
		info->MapTerrain=t;
		buf[0] = t & 0xFF;
		info->MapUID += ChksumArea(buf, 1);
		continue;
	    } else {
		DebugLevel1("Wrong terrain type length\n");
	    }
	}


	//
	//	Dimension
	//
	if( !memcmp(header,"DIM ",4) ) {

	    info->MapWidth=PudReadWord(input);
	    info->MapHeight=PudReadWord(input);
	    buf[0] = info->MapWidth & 0xFF;
	    buf[1] = (info->MapWidth >> 8) & 0xFF;
	    buf[2] = info->MapHeight & 0xFF;
	    buf[3] = (info->MapHeight >> 8) & 0xFF;
	    info->MapUID += ChksumArea(buf, 4);
	    continue;
	}

	//
	//	Unit data (optional)
	//
	if( !memcmp(header,"UDTA",4) ) {
	    char* bufp;

	    length-=2;
	    temp_short = PudReadWord(input);
	    buf[0] = temp_short & 0xFF;
	    buf[1] = (temp_short >> 8) & 0xFF;
	    info->MapUID += ChksumArea(buf, 2);
	    if( length<(long)sizeof(buf) ) {
		bufp=buf;
	    } else if( !(bufp=alloca(length)) ) {
		perror("alloca()");
		FreeMapInfo(info);
		return NULL;
	    }
	    if( CLread(input,bufp,length)!=length ) {
		perror("CLread()");
		FreeMapInfo(info);
		return NULL;
	    }
	    info->MapUID += ChksumArea(bufp, length);
	    continue;
	}

	//
	//	Pud restrictions (optional)
	//
	if( !memcmp(header,"ALOW",4) ) {
	    char* bufp;

	    if( length<(long)sizeof(buf) ) {
		bufp=buf;
	    } else if( !(bufp=alloca(length)) ) {
		perror("alloca()");
		FreeMapInfo(info);
		return NULL;
	    }
	    if( CLread(input,bufp,length)!=length ) {
		perror("CLread()");
		FreeMapInfo(info);
		return NULL;
	    }
	    info->MapUID += ChksumArea(bufp, length);
	    continue;
	}

	//
	//	Upgrade data (optional)
	//
	if( !memcmp(header,"UGRD",4) ) {
	    char* bufp;

	    length-=2;
	    temp_short = PudReadWord(input);
	    buf[0] = temp_short & 0xFF;
	    buf[1] = (temp_short >> 8) & 0xFF;
	    info->MapUID += ChksumArea(buf, 2);
	    if( length<(long)sizeof(buf) ) {
		bufp=buf;
	    } else if( !(bufp=alloca(length)) ) {
		perror("alloca()");
		FreeMapInfo(info);
		return NULL;
	    }
	    if( CLread(input,bufp,length)!=length ) {
		perror("CLread()");
		FreeMapInfo(info);
		return NULL;
	    }
	    info->MapUID += ChksumArea(bufp, length);
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
		    buf[0] = v & 0xFF;
		    info->MapUID += ChksumArea(buf, 1);
		    switch( v ) {
			case PlayerRaceHuman:
			case PlayerRaceOrc:
			case PlayerRaceNeutral:
			    break;
			default:
			    DebugLevel1("Unknown race %d\n" _C_ v);
			    v=PlayerRaceNeutral;
			    break;
		    }
		    info->PlayerSide[i]=v;
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
		    buf[0] = v & 0xFF;
		    buf[1] = (v >> 8) & 0xFF;
		    info->MapUID += ChksumArea(buf, 2);
		    info->PlayerGold[i]=v;
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
		    buf[0] = v & 0xFF;
		    buf[1] = (v >> 8) & 0xFF;
		    info->MapUID += ChksumArea(buf, 2);
		    info->PlayerWood[i]=v;
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
		    buf[0] = v & 0xFF;
		    buf[1] = (v >> 8) & 0xFF;
		    info->MapUID += ChksumArea(buf, 2);
		    info->PlayerOil[i]=v;
		}
		continue;
	    } else {
		DebugLevel1("Wrong starting oil length\n");
	    }
	}

	// FIXME: support the extended resources with puds?

	//
	//	AI for each player
	//
	if( !memcmp(header,"AIPL",4) ) {
	    if( length==16 ) {
		int i;
		int v;

		for( i=0; i<16; ++i ) {
		    v=PudReadByte(input);
		    buf[0] = v & 0xFF;
		    info->MapUID += ChksumArea(buf, 1);
		    info->PlayerAi[i]=v;
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
	    CLseek(input,length,SEEK_CUR);	// skip section
	    continue;
	}

	//
	//	Tiles MAP
	//
	if( !memcmp(header,"MTXM",4) ) {
	    unsigned short* mtxm;

	    // FIXME: linux didn't like this big alloca :((((((
	    if( !(mtxm=malloc(length)) ) {
		perror("malloc()");
		FreeMapInfo(info);
		return NULL;
	    }
	    if( CLread(input,mtxm,length)!=length ) {
		perror("CLread()");
		free(mtxm);
		FreeMapInfo(info);
		return NULL;
	    }
	    info->MapUID += ChksumArea((unsigned char *)mtxm, length);
	    free(mtxm);

	    continue;
	}

	//
	//	Movement MAP
	//
	if( !memcmp(header,"SQM ",4) ) {
	    unsigned short* sqm;

	    if( !(sqm=malloc(length)) ) {
		perror("malloc()");
		FreeMapInfo(info);
		return NULL;
	    }
	    if( CLread(input,sqm,length)!=length ) {
		perror("CLread()");
		free(sqm);
		FreeMapInfo(info);
		return NULL;
	    }
	    info->MapUID += ChksumArea((unsigned char *)sqm, length);
	    free(sqm);

	    continue;
	}

	//
	//	Action MAP
	//
	if( !memcmp(header,"REGM",4) ) {
	    unsigned short* regm;

	    if( !(regm=malloc(length)) ) {
		perror("malloc()");
		FreeMapInfo(info);
		return NULL;
	    }
	    if( CLread(input,regm,length)!=length ) {
		perror("CLread()");
		free(regm);
		FreeMapInfo(info);
		return NULL;
	    }
	    info->MapUID += ChksumArea((unsigned char *)regm, length);
	    free(regm);

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

	    while( length>=8 ) {
		x=PudReadWord(input);
		y=PudReadWord(input);
		t=PudReadByte(input);
		o=PudReadByte(input);
		v=PudReadWord(input);

		length-=8;

		buf[0] = x & 0xFF;
		buf[1] = (x >> 8) & 0xFF;
		buf[2] = y & 0xFF;
		buf[3] = (y >> 8) & 0xFF;
		buf[4] = t & 0xFF;
		buf[5] = o & 0xFF;
		buf[6] = v & 0xFF;
		buf[7] = (v >> 8) & 0xFF;
		info->MapUID += ChksumArea(buf, 8);
	    }
	    continue;
	}

	DebugLevel2("Unsupported Section: %4.4s\n" _C_ header);

	CLseek(input,length,SEEK_CUR);
    }

    CLclose(input);

    return info;
}

/**
**	Load pud.
**
**	@param pud	File name.
**	@param map	Map filled in.
*/
global void LoadPud(const char* pud,WorldMap* map)
{
    CLFile* input;
    long length;
    char header[5];
    char buf[1024];
    int width;
    int height;
    int aiopps;

    if (!map->Info) {
	map->Info = GetPudInfo(pud);
    }
    if( !(input=CLopen(pud)) ) {
	fprintf(stderr,"Try ./path/name\n");
	fprintf(stderr,"pud: CLopen(%s): %s\n", pud, strerror(errno));
	ExitFatal(-1);
    }
    header[4]='\0';
    if( !PudReadHeader(input,header,&length) ) {
	fprintf(stderr,"LoadPud: %s: invalid pud\n", pud);
	ExitFatal(-1);
    }
    if( memcmp(header,"TYPE",4) || length!=16 ) {
	fprintf(stderr,"LoadPud: %s: invalid pud\n", pud);
	ExitFatal(-1);
    }
    if( CLread(input,buf,16)!=16 ) {	// IGNORE TYPE
	perror("CLread()");
	ExitFatal(-1);
    }
    if( strcmp(buf,"WAR2 MAP") ) {	// ONLY CHECK STRING
	fprintf(stderr,"LoadPud: %s: invalid pud\n", pud);
	ExitFatal(-1);
    }

    aiopps=width=height=0;

    //
    //	Parse all sections.
    //
    while( PudReadHeader(input,header,&length) ) {
	DebugLevel3("\tSection: %4.4s\n" _C_ header);

	//
	//	PUD version
	//
	if( !memcmp(header,"VER ",4) ) {
	    if( length==2 ) {
		int v;

		v=PudReadWord(input);
		DebugLevel1("\tVER: %d.%d\n" _C_ (v&0xF0)>>4 _C_ v&0xF);
		continue;
	    }
	    DebugLevel1("Wrong version length\n");
	}

	//
	//	Map description
	//
	if( !memcmp(header,"DESC",4) ) {
	    if( CLread(input,buf,length)!=length ) {
		perror("CLread()");
		ExitFatal(-1);
	    }
	    DebugLevel1("\tDESC: %s\n" _C_ buf);
	    strncpy(map->Description,buf,sizeof(map->Description));
	    map->Description[sizeof(map->Description)-1]='\0';
	    continue;
	}

	//
	//	Player definitions.
	//
	if( !memcmp(header,"OWNR",4) ) {
	    if( length==16 ) {
		int i;
		int p;

		for( i=0; i<16; ++i ) {
		    p=PudReadByte(input);
		    // Single player games only:
		    // ARI: FIXME: convert to a preset array to share with network game code
		    if (GameSettings.Opponents != SettingsPresetMapDefault) {
			if (p == PlayerPerson && ThisPlayer != NULL) {
			    p = PlayerComputer;
			}
			if (p == PlayerComputer) {
			    if (aiopps < GameSettings.Opponents) {
				aiopps++;
			    } else {
				p = PlayerNobody;
			    }
			}
		    }
		    // Network games only:
		    if (GameSettings.Presets[i].Type != SettingsPresetMapDefault) {
			p = GameSettings.Presets[i].Type;
		    }
		    CreatePlayer(p);
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
		int i;

		t=PudReadWord(input);
		if (GameSettings.Terrain != SettingsPresetMapDefault) {
		    t = GameSettings.Terrain;
		}
		if( map->TerrainName ) {
		    free(map->TerrainName);
		}
		//
		//	Look if we have this as tileset.
		//
		for( i=0; i<t && TilesetWcNames[i]; ++i ) {
		}
		if( !TilesetWcNames[i] ) {
		    t=0;
		}
		map->TerrainName=strdup(TilesetWcNames[t]);
		map->Terrain=t;
		LoadTileset();
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

	    DebugLevel2("\tMap %d x %d\n" _C_ width _C_ height);

	    if( !map->Fields ) {
		map->Width=width;
		map->Height=height;

		map->Fields=calloc(width*height,sizeof(*map->Fields));
		if( !map->Fields ) {
		    perror("calloc()");
		    ExitFatal(-1);
		}
		TheMap.Visible[0]=calloc(TheMap.Width*TheMap.Height/8,1);
		if( !TheMap.Visible[0] ) {
		    perror("calloc()");
		    ExitFatal(-1);
		}
		InitUnitCache();
		// FIXME: this should be CreateMap or InitMap?
	    } else {			// FIXME: should do some checks here!
		DebugLevel0Fn("Warning: Fields already allocated\n");
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
		CLseek(input,length,SEEK_CUR);
	    } else {
		if( length<(long)sizeof(buf) ) {
		    bufp=buf;
		} else if( !(bufp=alloca(length)) ) {
		    perror("alloca()");
		    ExitFatal(-1);
		}
		if( CLread(input,bufp,length)!=length ) {
		    perror("CLread()");
		    ExitFatal(-1);
		}
		ParsePudUDTA(bufp,length);
	    }
	    continue;
	}

	//
	//	Pud restrictions (optional)
	//
	if( !memcmp(header,"ALOW",4) ) {
	    char* bufp;

	    if( length<(long)sizeof(buf) ) {
		bufp=buf;
	    } else if( !(bufp=alloca(length)) ) {
		perror("alloca()");
		ExitFatal(-1);
	    }
	    if( CLread(input,bufp,length)!=length ) {
		perror("CLread()");
		ExitFatal(-1);
	    }
	    ParsePudALOW(bufp,length);
	    continue;
	}

	//
	//	Upgrade data (optional)
	//
	if( !memcmp(header,"UGRD",4) ) {
	    char* bufp;

	    length-=2;
	    if( PudReadWord(input) ) {
		DebugLevel3("\tUsing default data\n");
		CLseek(input,length,SEEK_CUR);
	    } else {
		if( length<(long)sizeof(buf) ) {
		    bufp=buf;
		} else if( !(bufp=alloca(length)) ) {
		    perror("alloca()");
		    ExitFatal(-1);
		}
		if( CLread(input,bufp,length)!=length ) {
		    perror("CLread()");
		    ExitFatal(-1);
		}
		ParsePudUGRD(bufp,length);
	    }
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
			    DebugLevel1("Unknown race %d\n" _C_ v);
			    v=PlayerRaceNeutral;
			    break;
		    }
		    if (GameSettings.Presets[i].Race == SettingsPresetMapDefault) {
			PlayerSetSide(&Players[i],v);
		    } else {
			PlayerSetSide(&Players[i],GameSettings.Presets[i].Race);
		    }
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
		    PlayerSetResource(&Players[i],GoldCost,v);
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
		    PlayerSetResource(&Players[i],WoodCost,v);
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
		    PlayerSetResource(&Players[i],OilCost,v);
		}
		continue;
	    } else {
		DebugLevel1("Wrong starting oil length\n");
	    }
	}

	// FIXME: support the extended resources with puds?

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
	    CLseek(input,length,SEEK_CUR);	// skip section
	    continue;
	}

	//
	//	Tiles MAP
	//
	if( !memcmp(header,"MTXM",4) ) {
	    unsigned short* mtxm;

	    if( length!=width*height*2 ) {
		DebugLevel1("wrong length of MTXM section %ld\n" _C_ length);
		ExitFatal(-1);
	    }
	    if( !(mtxm=malloc(length)) ) {
		perror("malloc()");
		ExitFatal(-1);
	    }
	    if( CLread(input,mtxm,length)!=length ) {
		perror("CLread()");
		ExitFatal(-1);
	    }

	    ConvertMTXM(mtxm,width,height,map);
	    free(mtxm);

	    continue;
	}

	//
	//	Movement MAP
	//
	if( !memcmp(header,"SQM ",4) ) {
	    unsigned short* sqm;

	    if( length!=width*height*(int)sizeof(short) ) {
		DebugLevel1("wrong length of SQM  section %ld\n" _C_ length);
		ExitFatal(-1);
	    }
	    if( !(sqm=malloc(length)) ) {
		perror("malloc()");
		ExitFatal(-1);
	    }
	    if( CLread(input,sqm,length)!=length ) {
		perror("CLread()");
		ExitFatal(-1);
	    }

	    ConvertSQM(sqm,width,height,map);
	    free(sqm);

	    continue;
	}

	//
	//	Action MAP
	//
	if( !memcmp(header,"REGM",4) ) {
	    unsigned short* regm;

	    if( length!=width*height*(int)sizeof(short) ) {
		DebugLevel1("wrong length of REGM section %ld\n" _C_ length);
		ExitFatal(-1);
	    }
	    if( !(regm=malloc(length)) ) {
		perror("malloc()");
		ExitFatal(-1);
	    }
	    if( CLread(input,regm,length)!=length ) {
		perror("CLread()");
		ExitFatal(-1);
	    }

	    ConvertREGM(regm,width,height,map);
	    free(regm);

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
	    int s;
	    Unit* unit;

	    //
	    //	FIXME: should split this into parts.
	    //
	    while( length>=8 ) {
		x=PudReadWord(input);
		y=PudReadWord(input);
		t=PudReadByte(input);
		o=PudReadByte(input);
		v=PudReadWord(input);

		if( t==WC_StartLocationHuman
			|| t==WC_StartLocationOrc ) {	// starting points?

		    Players[o].StartX=MapOffsetX+x;
		    Players[o].StartY=MapOffsetY+y;
		    if (GameSettings.NumUnits == SettingsNumUnits1
			    && Players[o].Type != PlayerNobody ) {
			if (t == WC_StartLocationHuman) {
			    t = WC_UnitPeasant;
			} else {
			    t = WC_UnitPeon;
			}
			v = 1;
			goto pawn;
		    }
		} else {
		    if (GameSettings.NumUnits == SettingsNumUnitsMapDefault ||
			t == WC_UnitGoldMine || t == WC_UnitOilPatch || 
			(Players[o].Type != PlayerPerson &&
				 Players[o].Type != PlayerComputer) ) {
pawn:
			if (t != WC_UnitGoldMine && t != WC_UnitOilPatch) {
			    if (NetworkFildes == -1
				    && ReplayGameType != ReplayMultiPlayer
				    && o == ThisPlayer->Player) {
				s = GameSettings.Presets[0].Race;
			    } else {
				s = GameSettings.Presets[o].Race;
			    }
			    if (s != SettingsPresetMapDefault) {
				if (s == PlayerRaceHuman && (t & 1) == 1) {
				    t--;
				}
				if (s == PlayerRaceOrc && (t & 1) == 0) {
				    t++;
				}
				// FIXME: ARI: This is hard-coded WAR2 ... also: support more races?
			    }
			}
			if (Players[o].Type != PlayerNobody) {
			    unit=MakeUnitAndPlace(MapOffsetX+x,MapOffsetY+y
				    ,UnitTypeByWcNum(t),&Players[o]);
			    if( unit->Type->GoldMine || unit->Type->GivesOil
				    || unit->Type->OilPatch ) {
				DebugCheck( !v );
				unit->Value=v*2500;
			    } else {	
				// active/inactive AI units!!
				// Johns: it is better to have active buildings
				if( !unit->Type->Building ) {
				    unit->Active=!v;
				}
			    }
			    UpdateForNewUnit(unit,0);
			}
		    }
		}

		length-=8;
	    }
	    continue;
	}

	DebugLevel2("Unsupported Section: %4.4s\n" _C_ header);

	CLseek(input,length,SEEK_CUR);
    }

    CLclose(input);

    DebugLevel3("Memory for pud %d\n"
	    _C_ width*height*sizeof(*map->Fields)
// FIXME: remove this
	    +width*height*sizeof(short)
	    +width*height*sizeof(short) );

    MapOffsetX+=width;
    if( MapOffsetX>=map->Width ) {
	MapOffsetX=0;
	MapOffsetY+=height;
    }
}

/**
**	Convert the map to MTXM format and save in a buffer.
**
**	@param mtxm	Buffer to save MTXM data
**	@param map	Map to convert
**	@param tileset	Tileset used to convert
*/
local void PudConvertMTXM(unsigned char* mtxm,const WorldMap* map,
	    Tileset* tileset)
{
    int i;
    int n;

    n=map->Width*map->Height;
    for( i=0; i<n; ++i ) {
	int tile;
	int j;

	tile=map->Fields[i].Tile;
	// Find the tile in the conversion table, this can be done faster
	for( j=0; j<tileset->NumTiles ; ++j ) {
	    if( tile==tileset->Table[j] ) {
		break;
	    }
	}
	mtxm[i*2+0]=j >> 0;
	mtxm[i*2+1]=j >> 8;
    }
}

#ifdef USE_ZLIB

/**
**	Write pud header.
**
**	@param f	File handle
**	@param type	4 byte header
**	@param length	section length
*/
local void PudWriteHeader(gzFile f,char* type,int length)
{
    unsigned char buf[4];

    DebugCheck( strlen(type)!=4 );
    if( gzwrite(f,type,4)!=4 ) {
	ExitFatal(-1);
    }

    buf[0]=length>> 0;
    buf[1]=length>> 8;
    buf[2]=length>>16;
    buf[3]=length>>24;

    if( gzwrite(f,buf,4)!=4 ) {
	ExitFatal(-1);
    }
}

/**
**	Save the MTXM section
**
**	@param f	File handle
**	@param map	Map to save.
*/
local void PudWriteMTXM(gzFile f,const WorldMap* map)
{
    int n;
    unsigned char* mtxm;
    Tileset* tileset;

    tileset=map->Tileset;
    n=map->Width*map->Height;
    PudWriteHeader(f,"MTXM",n*2);
    mtxm=malloc(n*2);

    PudConvertMTXM(mtxm,map,tileset);

    gzwrite(f,mtxm,n*2);
    free(mtxm);
}

/**
**	Save the SQM section
**
**	@param f	File handle
**	@param map	Map to save.
*/
local void PudWriteSQM(gzFile f,const WorldMap* map)
{
    int i;
    int n;
    unsigned char* sqm;

    n=map->Width*map->Height;
    PudWriteHeader(f,"SQM ",n*2);
    sqm=malloc(n*2);

    for( i=0; i<n; ++i ) {
	int v;
	int f;

	f=map->Fields[i].Flags;
	v=0;
	if( f&MapFieldLandAllowed ) {
	    v|=MapMoveOnlyLand;
	}
	if( f&MapFieldCoastAllowed ) {
	    v|=MapMoveCoast;
	}
	if( (f&(MapFieldWall|MapMoveHuman))==MapFieldWall ) {
	    v|=MapMoveWallO;
	}
	if( (f&(MapFieldWall|MapMoveHuman))==(MapFieldWall|MapMoveHuman) ) {
	    v|=MapMoveWallH;
	}
	if( f&MapFieldNoBuilding ) {
	    v|=MapMoveDirt;
	}
	if( f&MapFieldWaterAllowed) {
	    v|=MapMoveOnlyWater ;
	}
	if( f&MapFieldUnpassable ) {
	    v|=MapMoveUnpassable;
	}
#if 0
	// These fields shouldn't be saved.  The engine sets them when loading
	// a pud and placing units.  Saving these creates problems with games
	// that don't use all of the units, for example using fewer players
	// or one peasant only.
	if( f&MapFieldLandUnit ) {
	    v|=MapMoveLandUnit;
	}
	if( f&MapFieldAirUnit ) {
	    v|=MapMoveAirUnit;
	}
	if( f&MapFieldSeaUnit ) {
	    v|=MapMoveSeaUnit;
	}
	if( f&MapFieldBuilding ) {
	    v|=MapMoveBuildingUnit;
	}
#endif
	sqm[i*2+0]=v >> 0;
	sqm[i*2+1]=v >> 8;
    }
    gzwrite(f,sqm,n*2);
    free(sqm);
}
 
/**
**	Save the REGM section
**
**	@param f	File handle
**	@param map	Map to save.
*/
local void PudWriteREGM(gzFile f,const WorldMap* map)
{
    int i;
    int n;
    unsigned char* regm;

    n=map->Width*map->Height;
    PudWriteHeader(f,"REGM",n*2);
    regm=malloc(n*2);
    for( i=0; i<n; ++i ) {
	int v;
	int f;

	f=map->Fields[i].Flags;
	v=-1;
	if( f&MapFieldForest ) {
	    v=MapActionForest;
	} else if( f&MapFieldRocks ) {
	    v=MapActionRocks;
	} else if( f&MapFieldWall ) {
	    v=MapActionWall;
	} else if( f&0 ) {		// FIXME: not supported
	    v=MapActionIsland;
	} else if( f&MapFieldWaterAllowed ) {
	    v=MapActionWater;
	} else if( (f&MapFieldLandAllowed) || (f&MapFieldCoastAllowed) ) {
	    v=MapActionLand;
	}
	regm[i*2+0]=v >> 0;
	regm[i*2+1]=v >> 8;
    }

    gzwrite(f,regm,n*2);
    free(regm);
}

/**
**	Save the units to pud.
**
**	@param f	File handle
*/
local void PudSaveUnits(gzFile f)
{
    char buf[256];
    int i;

    PudWriteHeader(f,"UNIT",8*NumUnits);
    for( i=0; i<NumUnits; ++i ) {
	int j;

	buf[0]=Units[i]->X >> 0;
	buf[1]=Units[i]->X >> 8;
	buf[2]=Units[i]->Y >> 0;
	buf[3]=Units[i]->Y >> 8;

	// Convert our name to pud number
	for( j=0; UnitTypeWcNames[j]; ++j ) {
	    if( !strcmp(UnitTypeWcNames[j],Units[i]->Type->Ident) ) {
		break;
	    }
	}
	buf[4]=j;
	buf[5]=Units[i]->Player->Player;
	if( Units[i]->Type->GoldMine
		|| Units[i]->Type->OilPatch
		|| Units[i]->Type->GivesOil ) {
	    buf[6]=(Units[i]->Value/2500) >> 0;
	    buf[7]=(Units[i]->Value/2500) >> 8;
	} else {
	    buf[6]=!Units[i]->Active;
	    buf[7]=0;
	}
	gzwrite(f,buf,8);
    }
}

/**
**	Save pud.
**
**	@param pud	File name.
**	@param map	Map to save.
*/
global int SavePud(const char* pud,const WorldMap* map)
{
    gzFile f;
    int i;
    unsigned char buf[5950];

    if( !(f=gzopen(pud,strcasestr(pud,".gz") ? "wb9" : "wb0")) ) {
	fprintf(stderr,"Can't save pud `%s'\n",pud);
	return -1;
    }

    PudWriteHeader(f,"TYPE",16);
    memcpy(buf,"WAR2 MAP",8);
    buf[0x8]=0x00;
    buf[0x9]=0x00;
    buf[0xA]=0x0A;
    buf[0xB]=0xFF;
    buf[0xC]=map->Info->MapUID >>  0;
    buf[0xD]=map->Info->MapUID >>  8;
    buf[0xE]=map->Info->MapUID >> 16;
    buf[0xF]=map->Info->MapUID >> 24;
    gzwrite(f,buf,16);

    PudWriteHeader(f,"VER ",2);
    buf[0]=0x13;	// 1.3
    buf[1]=0x00;
    gzwrite(f,buf,2);

    PudWriteHeader(f,"DESC",32);
    memset(buf,0,32);
    strncpy(buf,map->Info->Description,32);
    gzwrite(f,buf,32);

    PudWriteHeader(f,"OWNR",16);
    for( i=0; i<16; ++i ) {
	buf[i]=map->Info->PlayerType[i];
    }
    gzwrite(f,buf,16);

    PudWriteHeader(f,"ERAX",2);
    buf[0]=map->Info->MapTerrain >> 0;
    buf[1]=map->Info->MapTerrain >> 8;
    gzwrite(f,buf,2);

    PudWriteHeader(f,"DIM ",4);
    buf[0]=map->Width  >> 0;
    buf[1]=map->Width  >> 8;
    buf[2]=map->Height >> 0;
    buf[3]=map->Height >> 8;
    gzwrite(f,buf,4);

    PudWriteHeader(f,"UDTA",5950);
    buf[0]=1;
    buf[1]=0;
    memset(buf+2,0,5950-2);
    // FIXME:
    gzwrite(f,buf,5950);

#if 0
    PudWriteHeader(f,"ALOW",384);
    // FIXME:
    gzwrite(f,buf,384);
#endif

    PudWriteHeader(f,"UGRD",782);
    buf[0]=1;
    buf[1]=0;
    memset(buf+2,0,782-2);
    // FIXME:
    gzwrite(f,buf,782);

    PudWriteHeader(f,"SIDE",16);
    for( i=0; i<16; ++i ) {
	buf[i]=map->Info->PlayerSide[i];
    }
    gzwrite(f,buf,16);

    PudWriteHeader(f,"SGLD",32);
    for( i=0; i<16; ++i ) {
	buf[i*2+0]=map->Info->PlayerGold[i] >> 0;
	buf[i*2+1]=map->Info->PlayerGold[i] >> 8;
    }
    gzwrite(f,buf,32);

    PudWriteHeader(f,"SLBR",32);
    for( i=0; i<16; ++i ) {
	buf[i*2+0]=map->Info->PlayerWood[i] >> 0;
	buf[i*2+1]=map->Info->PlayerWood[i] >> 8;
    }
    gzwrite(f,buf,32);

    PudWriteHeader(f,"SOIL",32);
    for( i=0; i<16; ++i ) {
	buf[i*2+0]=map->Info->PlayerOil[i] >> 0;
	buf[i*2+1]=map->Info->PlayerOil[i] >> 8;
    }
    gzwrite(f,buf,32);

    PudWriteHeader(f,"AIPL",16);
    for( i=0; i<16; ++i ) {
	buf[i]=map->Info->PlayerAi[i];
    }
    gzwrite(f,buf,16);

    PudWriteMTXM(f,map);
    PudWriteSQM(f,map);
#if 0
    PudWriteHeader(f,"OILM",map->Width*map->Height*1);
    gzwrite(f,buf,map->Width*map->Height*1);
    // FIXME:
#endif
    PudWriteREGM(f,map);

    PudSaveUnits(f);

    gzclose(f);

    return 0;
}

#else

/**
**	Save pud.
**
**	@param pud	File name.
**	@param map	Map to save.
*/
global int SavePud(const char* pud __attribute__((unused)),
	const WorldMap* map __attribute__((unused)))
{
    DebugLevel0Fn("Only supported with ZLIB\n");
}

#endif

/**
**	Change a pud's tileset
**
**	@param old	    Number of old tileset
**	@param map	    Map to change
*/
global void ChangeTilesetPud(int old,WorldMap* map)
{
    unsigned char* mtxm;

    MapOffsetX=MapOffsetY=0;
    mtxm=malloc(map->Width*map->Height*2);
    PudConvertMTXM(mtxm,map,Tilesets[old]);
    ConvertMTXM((const unsigned short*)mtxm,map->Width,map->Height,map);
    free(mtxm);
}

/**
**	Clean pud module.
*/
global void CleanPud(void)
{
    MapOffsetX=MapOffsetY=0;
}

//@}
