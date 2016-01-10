/*
**	A clone of a famous game.
*/
/**@name ccl_map.c	-	The map ccl functions. */
/*
**	(c) Copyright 1999 by Lutz Sammer
**
**	$Id: ccl_map.c,v 1.2 1999/09/26 01:57:34 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>

#include "clone.h"

#ifdef USE_CCL	// {

#include "ccl.h"
#include "tileset.h"
#include "map.h"
#include "minimap.h"

#include <guile/gh.h>			// I use guile for a quick hack

extern SCM gh_new_procedureN(char* proc_name, SCM (*fn) ());

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Reveal the complete map.
**	FIXME: only functional in init file!
*/
local SCM CclRevealMap(void)
{
    if( !CclInConfigFile ) {
	fprintf(stderr,__FUNCTION__": only in config file supported\n");
    }
    FlagRevealMap=1;

    return SCM_UNSPECIFIED;
}

/**
**	Enable fog of war.
*/
local SCM CclFogOfWar(void)
{
    TheMap.NoFogOfWar=0;

    return SCM_UNSPECIFIED;
}

/**
**	Disable fog of war.
*/
local SCM CclNoFogOfWar(void)
{
    TheMap.NoFogOfWar=1;

    return SCM_UNSPECIFIED;
}

/**
**	Enable display of terrain in minimap.
*/
local SCM CclMinimapTerrain(void)
{
    MinimapWithTerrain=1;

    return SCM_UNSPECIFIED;
}

/**
**	Disable display of terrain in minimap.
*/
local SCM CclNoMinimapTerrain(void)
{
    MinimapWithTerrain=0;

    return SCM_UNSPECIFIED;
}

/**
**	Original fog of war.
*/
local SCM CclOriginalFogOfWar(void)
{
    if( !CclInConfigFile ) {
	fprintf(stderr,__FUNCTION__": only in config file supported\n");
    }
    OriginalFogOfWar=1;

    return SCM_UNSPECIFIED;
}

/**
**	Gray style fog of war.
*/
local SCM CclGrayFogOfWar(void)
{
    if( !CclInConfigFile ) {
	fprintf(stderr,__FUNCTION__": only in config file supported\n");
    }
    OriginalFogOfWar=0;

    return SCM_UNSPECIFIED;
}

/**
**	Gray style fog of war contrast.
*/
local SCM CclFogOfWarContrast(SCM contrast)
{
    int i;

    if( !CclInConfigFile ) {
	fprintf(stderr,__FUNCTION__": only in config file supported\n");
    }
    i=gh_scm2int(contrast);
    if( i<0 || i>400 ) {
	fprintf(stderr,__FUNCTION__": contrast should be 0-400\n");
	i=100;
    }
    FogOfWarContrast=i;

    return SCM_UNSPECIFIED;
}

/**
**	Gray style fog of war brightness.
*/
local SCM CclFogOfWarBrightness(SCM brightness)
{
    int i;

    if( !CclInConfigFile ) {
	fprintf(stderr,__FUNCTION__": only in config file supported\n");
    }
    i=gh_scm2int(brightness);
    if( i<-100 || i>100 ) {
	fprintf(stderr,__FUNCTION__": brightness should be -100-100\n");
	i=0;
    }
    FogOfWarBrightness=i;

    return SCM_UNSPECIFIED;
}

/**
**	Gray style fog of war saturation.
*/
local SCM CclFogOfWarSaturation(SCM saturation)
{
    int i;

    if( !CclInConfigFile ) {
	fprintf(stderr,__FUNCTION__": only in config file supported\n");
    }
    i=gh_scm2int(saturation);
    if( i<-100 || i>200 ) {
	fprintf(stderr,__FUNCTION__": saturation should be -100-200\n");
	i=0;
    }
    FogOfWarSaturation=i;

    return SCM_UNSPECIFIED;
}

//.............................................................................
//.	Tables
//.............................................................................

/**
**	Parse tilset definition.
**
**	@param slot	Slot number
**	@param name	Reference name
**	@param file	Graphic file
**	@param table	Conversion table
*/
local SCM CclTileset(SCM slot,SCM name,SCM file,SCM table)
{
    int type;
    int i;
    unsigned short* wp;

    // FIXME: remove use of slot number!!
    type=gh_scm2int(slot);
    if( type>=TilesetMax ) {
	fprintf(stderr,"Wrong tileset number %d\n",type);
	return SCM_UNSPECIFIED;
    }
    Tilesets[type].Name=gh_scm2newstr(name,NULL);
    Tilesets[type].File=gh_scm2newstr(file,NULL);

    // CONVERT TABLE!!
    if( gh_vector_length(table)!=2528 ) {	// 0x9E0
	fprintf(stderr,"Wrong conversion table length\n");
	return SCM_UNSPECIFIED;
    }

    Tilesets[type].Table=wp=malloc(sizeof(*Tilesets[type].Table)*2528);
    for( i=0; i<2528; ++i ) {
	wp[i]=gh_scm2int(gh_vector_ref(table,gh_int2scm(i)));
    }

    return SCM_UNSPECIFIED;
}

/**
**	Register CCL features for map.
*/
global void MapCclRegister(void)
{
    gh_new_procedure0_0("reveal-map",CclRevealMap);

    gh_new_procedure0_0("fog-of-war",CclFogOfWar);
    gh_new_procedure0_0("no-fog-of-war",CclNoFogOfWar);

    gh_new_procedure0_0("minimap-terrain",CclMinimapTerrain);
    gh_new_procedure0_0("no-minimap-terrain",CclNoMinimapTerrain);

    gh_new_procedure0_0("original-fog-of-war",CclOriginalFogOfWar);
    gh_new_procedure0_0("gray-fog-of-war",CclGrayFogOfWar);
    gh_new_procedure0_1("fog-of-war-contrast",CclFogOfWarContrast);
    gh_new_procedure0_1("fog-of-war-brightness",CclFogOfWarBrightness);
    gh_new_procedure0_1("fog-of-war-saturation",CclFogOfWarSaturation);

    gh_new_procedure4_0("tileset",CclTileset);
}

#endif	// } USE_CCL

//@}
