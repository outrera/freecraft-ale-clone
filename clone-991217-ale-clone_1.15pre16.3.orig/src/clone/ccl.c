/*
**	A clone of a famous game.
*/
/**@name ccl.c		-	The clone configuration language. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: ccl.c,v 1.24 1999/12/03 09:18:09 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "clone.h"

#ifdef USE_CCL

#include "video.h"
#include "tileset.h"
#include "sound_id.h"
#include "icons.h"
#include "button.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "map.h"
#include "ui.h"
#include "minimap.h"
#include "ccl.h"
#include "pud.h"
#include "missile.h"
#include "ccl_sound.h"

#include <guile/gh.h>			// I use guile for a quick hack

#ifdef USE_SVGALIB
#undef GUILE_GTK
#endif

#ifdef GUILE_GTK			// experimental guile-gtk support

#include <guile-gtk.h>

extern void sgtk_init_gtk_gtk_glue();
extern void sgtk_init_gtk_gdk_glue();

#endif

#ifdef LIBGUILE12			// my snapshot version 19980430
					// didn't need it
#define gh_vector_ref(vector,index) gh_vref(vector,index)
#endif

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

global char* CclStartFile;		/// CCL start file
global int CclInConfigFile;		/// True while config file parsing

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

// FIXME: memory loose, if you define the same thing again.

/*............................................................................
..	Config
............................................................................*/

/**
**	Clone library path.
**
**	@return		Current clone libray path.
*/
local SCM CclCloneLibraryPath(void)
{
    return gh_str02scm(CLONE_LIB_PATH);
}

/**
**	Default title-screen.
**
**	@param title	SCM title. (nil reports only)
**
**	@return		Current title screen.
*/
local SCM CclTitleScreen(SCM title)
{
    if( !gh_null_p(title) ) {
	if( TitleScreen ) {
	    free(TitleScreen);
	    TitleScreen=NULL;
	}

	TitleScreen=gh_scm2newstr(title,NULL);
    } else {
	title=gh_str02scm(TitleScreen);
    }
    return title;
}

/**
**	Define mana sprite.
**
**	@param x	Mana X position.
**	@param y	Mana Y position.
**	@param w	Mana width.
**	@param h	Mana height.
*/
local SCM CclManaSprite(SCM file,SCM x,SCM y,SCM w,SCM h)
{
    DebugLevel0("FIXME: mana-sprite not written\n");

    return SCM_UNSPECIFIED;
}

/**
**	Define health sprite.
**
**	@param x	Mana X position.
**	@param y	Mana Y position.
**	@param w	Mana width.
**	@param h	Mana height.
*/
local SCM CclHealthSprite(SCM file,SCM x,SCM y,SCM w,SCM h)
{
    DebugLevel0("FIXME: health-sprite not written\n");

    return SCM_UNSPECIFIED;
}

/**
**	Enable display health as health-bar.
*/
local SCM CclShowHealthBar(void)
{
    ShowHealthBar=1;
    ShowHealthDot=0;

    return SCM_UNSPECIFIED;
}

/**
**	Enable display health as health-dot.
*/
local SCM CclShowHealthDot(void)
{
    ShowHealthBar=0;
    ShowHealthDot=1;

    return SCM_UNSPECIFIED;
}

/**
**	Enable display mana as mana-bar.
*/
local SCM CclShowManaBar(void)
{
    ShowManaBar=1;
    ShowManaDot=0;

    return SCM_UNSPECIFIED;
}

/**
**	Enable display mana as mana-dot.
*/
local SCM CclShowManaDot(void)
{
    ShowManaBar=0;
    ShowManaDot=1;

    return SCM_UNSPECIFIED;
}

/**
**	Enable display of full bars/dots.
*/
local SCM CclShowFull(void)
{
    ShowNoFull=0;

    return SCM_UNSPECIFIED;
}

/**
**	Disable display of full bars/dots.
*/
local SCM CclShowNoFull(void)
{
    ShowNoFull=1;

    return SCM_UNSPECIFIED;
}

/**
**	Draw decorations always on top.
*/
local SCM CclDecorationOnTop(void)
{
    DecorationOnTop=1;

    return SCM_UNSPECIFIED;
}

/**
**	Enable display of sight range.
*/
local SCM CclShowSightRange(void)
{
    ShowSightRange=1;

    return SCM_UNSPECIFIED;
}

/**
**	Enable display of react range.
*/
local SCM CclShowReactRange(void)
{
    ShowReactRange=1;

    return SCM_UNSPECIFIED;
}

/**
**	Enable display of attack range.
*/
local SCM CclShowAttackRange(void)
{
    ShowAttackRange=1;

    return SCM_UNSPECIFIED;
}

/**
**	Enable display of orders.
*/
local SCM CclShowOrders(void)
{
    ShowOrders=1;

    return SCM_UNSPECIFIED;
}

/*
**	For debug increase mining speed.
*/
local SCM CclSpeedMine(SCM speed)
{
    SpeedMine=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase gold delivery speed.
*/
local SCM CclSpeedGold(SCM speed)
{
    SpeedGold=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase wood chopping speed.
*/
local SCM CclSpeedChop(SCM speed)
{
    SpeedChop=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase wood delivery speed.
*/
local SCM CclSpeedWood(SCM speed)
{
    SpeedWood=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase haul speed.
*/
local SCM CclSpeedHaul(SCM speed)
{
    SpeedHaul=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase oil delivery speed.
*/
local SCM CclSpeedOil(SCM speed)
{
    SpeedOil=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase building speed.
*/
local SCM CclSpeedBuild(SCM speed)
{
    SpeedBuild=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase training speed.
*/
local SCM CclSpeedTrain(SCM speed)
{
    SpeedTrain=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase upgrading speed.
*/
local SCM CclSpeedUpgrade(SCM speed)
{
    SpeedUpgrade=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase researching speed.
*/
local SCM CclSpeedResearch(SCM speed)
{
    SpeedResearch=gh_scm2int(speed);

    return speed;
}

/*
**	For debug increase all speeds.
*/
local SCM CclSpeeds(SCM speed)
{
    SpeedMine=SpeedGold=
	SpeedChop=SpeedWood=
	SpeedHaul=SpeedOil=
	SpeedBuild=
	SpeedTrain=
	SpeedUpgrade=
	SpeedResearch=gh_scm2int(speed);

    return speed;
}

/*............................................................................
..	Tables
............................................................................*/

/**
**	Parse missile-type.
**
**	@param list	List describing missile.
*/
local SCM CclMissileType(SCM list)
{
    SCM value;
    int type;

    //	Slot
    value=gh_car(list);
    type=gh_scm2int(value);
    if( type>=MissileTypeMax ) {
	fprintf(stderr,"Wrong type %d\n",type);
	return list;
    }
    list=gh_cdr(list);
    DebugLevel3("MissileType: %d\n",type);

    //	Name
    value=gh_car(list);
    MissileTypes[type].Name=gh_scm2newstr(value,NULL);
    list=gh_cdr(list);

    //	File
    value=gh_car(list);
    MissileTypes[type].File=gh_scm2newstr(value,NULL);
    list=gh_cdr(list);

    // Width,Height
    value=gh_car(list);
    MissileTypes[type].Width=gh_scm2int(value);
    list=gh_cdr(list);
    value=gh_car(list);
    MissileTypes[type].Height=gh_scm2int(value);
    list=gh_cdr(list);

    // Sound impact
    value=gh_car(list);
#ifdef WITH_SOUND
    if (ccl_sound_p(value)) {
	MissileTypes[type].ImpactSound.Sound=ccl_sound_id(value);
    } else
#endif
    if (!gh_boolean_p(value) ||  gh_scm2bool(value) ) {
	fprintf(stderr,"Wrong argument in MissileType\n");
    }
    list=gh_cdr(list);

    // FIXME: class, speed not written!!!

    return list;
}

/**
**	Load a pud.
**
**	@param file	filename of pud.
**
**	@return		FIXME: Nothing.
*/
local SCM CclLoadPud(SCM file)
{
    char* name;

    name=gh_scm2newstr(file,NULL);
    LoadPud(name,&TheMap);
    free(name);

    // FIXME: LoadPud should return an error
    return SCM_UNSPECIFIED;
}

/**
**	Define a map.
**
**	@param width	Map width.
**	@param height	Map height.
*/
local SCM CclDefineMap(SCM width,SCM height)
{
    TheMap.Width=gh_scm2int(width);
    TheMap.Height=gh_scm2int(height);

    TheMap.Fields=calloc(TheMap.Width*TheMap.Height,sizeof(*TheMap.Fields));
#ifdef MOVE_MAP
    TheMap.MovementMap=calloc(TheMap.Width*TheMap.Height
	    ,sizeof(*TheMap.MovementMap));
#endif
#ifdef ACTION_MAP
    TheMap.ActionMap=calloc(TheMap.Width*TheMap.Height
	    ,sizeof(*TheMap.ActionMap));
#endif
    InitUnitCache();

    MapX=MapY=0;

    return SCM_UNSPECIFIED;
}

/**
**	Parse a clone map.
**
**	@param list	list of tuples keyword data
*/
local SCM CclCloneMap(SCM list)
{
    SCM value;
    SCM name;
    SCM data;

    //
    //	Parse the list:	(still everything could be changed!)
    //
    while( !gh_null_p(list) ) {

	value=gh_car(list);
	//gh_display(value);
	//gh_newline();
	if( gh_list_p(value) ) {
	    name=gh_car(value);
	    data=gh_cdr(value);
	    if( !gh_symbol_p(name) ) {
		fprintf(stderr,"symbol expected\n");
		return list;
	    }
	    if( gh_eq_p(name,gh_symbol2scm("version")) ) {
		DebugLevel1("VERSION:\n");
		gh_display(data);
		gh_newline();
		// FIXME:
	    } else if( gh_eq_p(name,gh_symbol2scm("description")) ) {
		DebugLevel1("DESCRIPTION:\n");
		gh_display(data);
		gh_newline();
		// FIXME:
	    } else if( gh_eq_p(name,gh_symbol2scm("terrain")) ) {
		int terrain;

		DebugLevel1("TERRAIN:\n");
		gh_display(data);
		gh_newline();
		value=gh_car(data);
		data=gh_cdr(data);
		terrain=gh_scm2int(value);
		TheMap.Terrain=terrain;
		// FIXME:
	    } else if( gh_eq_p(name,gh_symbol2scm("dimension")) ) {
		int width;
		int height;

		DebugLevel1("DIMENSION:\n");
		gh_display(data);
		gh_newline();
		value=gh_car(data);
		width=gh_scm2int(value);
		data=gh_cdr(data);
		value=gh_car(data);
		height=gh_scm2int(value);
		TheMap.Width=width;
		TheMap.Height=height;

		TheMap.Fields=calloc(width*height,sizeof(*TheMap.Fields));
#ifdef MOVE_MAP
		TheMap.MovementMap=calloc(width*height
			,sizeof(*TheMap.MovementMap));
#endif
#ifdef ACTION_MAP
		TheMap.ActionMap=calloc(width*height
			,sizeof(*TheMap.ActionMap));
#endif
		InitUnitCache();

	    } else if( gh_eq_p(name,gh_symbol2scm("tiles")) ) {
		int i;
		int l;

		DebugLevel1("TILES:\n");
		value=gh_car(data);
		if( !gh_vector_p(value) ) {
		    fprintf(stderr,"vector expected\n");
		    return SCM_UNSPECIFIED;
		}
		l=gh_vector_length(value);
		if( l!=TheMap.Width*TheMap.Height ) {
		    fprintf(stderr,"Wrong tile table length %d\n",l);
		}
		for( i=0; i<l; ++i ) {
		    TheMap.Fields[i].Tile=
			    Tilesets[TilesetSummer].Table[
				gh_scm2int(gh_vector_ref(value,gh_int2scm(i)))
			    ];
		}
	    } else {
		;
	    }
	} else {
	    fprintf(stderr,"list expected\n");
	    return list;
	}

	list=gh_cdr(list);
    }

    return list;
}

/*............................................................................
..	Commands
............................................................................*/

/**
**	Send command to ccl.
*/
global void CclCommand(char* command)
{
    gh_display(gh_eval_str_with_standard_handler(command));
    gh_newline();
}

/**
**	Callback to c-main1.
*/
local SCM CclMain1(void)
{
    main1(0,NULL);

    DebugLevel0("Shouldn't be reached\n");

    return SCM_UNSPECIFIED;
}

/*............................................................................
..	Setup
............................................................................*/

/**
**	Define a vararg scheme procedure.
*/
global SCM gh_new_procedureN(char* proc_name, SCM (*fn) ())
{
    return gh_new_procedure(proc_name,fn,0,0,1);
}

/**
**	Called from scheme.
*/
local void gh_main_prog(int argc,char* argv[])
{
    gh_new_procedure0_0("clone-library-path",CclCloneLibraryPath);
    gh_new_procedure1_0("title-screen",CclTitleScreen);

    gh_new_procedure5_0("mana-sprite",CclManaSprite);
    gh_new_procedure5_0("health-sprite",CclHealthSprite);
    gh_new_procedure0_0("show-health-bar",CclShowHealthBar);
    gh_new_procedure0_0("show-health-dot",CclShowHealthDot);
    gh_new_procedure0_0("show-mana-bar",CclShowManaBar);
    gh_new_procedure0_0("show-mana-dot",CclShowManaDot);
    gh_new_procedure0_0("show-full",CclShowFull);
    gh_new_procedure0_0("show-no-full",CclShowNoFull);
    gh_new_procedure0_0("decoration-on-top",CclDecorationOnTop);
    gh_new_procedure0_0("show-sight-range",CclShowSightRange);
    gh_new_procedure0_0("show-react-range",CclShowReactRange);
    gh_new_procedure0_0("show-attack-range",CclShowAttackRange);
    gh_new_procedure0_0("show-orders",CclShowOrders);

    gh_new_procedure1_0("speed-mine",CclSpeedMine);
    gh_new_procedure1_0("speed-gold",CclSpeedGold);
    gh_new_procedure1_0("speed-chop",CclSpeedChop);
    gh_new_procedure1_0("speed-wood",CclSpeedWood);
    gh_new_procedure1_0("speed-haul",CclSpeedHaul);
    gh_new_procedure1_0("speed-oil",CclSpeedOil);
    gh_new_procedure1_0("speed-build",CclSpeedBuild);
    gh_new_procedure1_0("speed-train",CclSpeedTrain);
    gh_new_procedure1_0("speed-upgrade",CclSpeedUpgrade);
    gh_new_procedure1_0("speed-research",CclSpeedResearch);
    gh_new_procedure1_0("speeds",CclSpeeds);

    gh_new_procedureN("missile-type",CclMissileType);

    MapCclRegister();
    UnitButtonCclRegister();
    UnitTypeCclRegister();
    SoundCclRegister();
    UserInterfaceCclRegister();

    gh_new_procedure1_0("load-pud",CclLoadPud);
    gh_new_procedure2_0("define-map",CclDefineMap);

    gh_new_procedureN("clone-map",CclCloneMap);

    gh_new_procedure0_0("c-main1",CclMain1);

#ifdef GUILE_GTK
    SGTK_REGISTER_GLUE(sgtk_init_gtk_gtk_glue);
    SGTK_REGISTER_GLUE(sgtk_init_gtk_gdk_glue);
    // ALL Stupid
    {
    int argc;
    char** argv;
    static char* args[2] = { "clone", NULL };

    argc=1;
    argv=args;
    sgtk_init_with_args(&argc,&argv);
    }
    //sgtk_init_with_args(NULL,NULL);
#endif

    //
    //	Load and evaluate configuration file
    //
    CclInConfigFile=1;
    gh_eval_file(CclStartFile);
    CclInConfigFile=0;

    // FIXME: guile didn't cleanup, all memory is lost!

    main1(argc,argv);			// continue with setup
}

/**
**	Initialize ccl.
*/
global void volatile CclInit(void)
{
    gh_enter(0,0,gh_main_prog);		// guile didn't return!
}

#endif

//@}
