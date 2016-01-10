/*
**	A clone of a famous game.
*/
/**@name icons.c	-	The icons. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: icons.c,v 1.9 1999/11/21 22:00:20 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clone.h"
#include "tileset.h"
#include "video.h"
#include "map.h"
#include "icons.h"

#include "etlib/hash.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	Unit icons
*/
local struct _icons_ {
    char*	File[TilesetMax];	/// file name
    unsigned	Width;			/// icon width
    unsigned	Height;			/// icon height

// --- FILLED UP ---
    OldGraphic	Icon;			/// graphic data loaded
} Icons[] = {
    { {  "icons (summer).png"
	,"icons (winter).png"
	,"icons (wasteland).png"
	,"icons (swamp).png" }
		, ICON_WIDTH, ICON_HEIGHT },
};

local hashtable(int,61) IconHash;	/// lookup table for icon names

/**
**	Table icon id -> string.
*/
local const char* IconNames[] = {
    "icon-peasant",
    "icon-peon",
    "icon-footman",
    "icon-grunt",
    "icon-archer",
    "icon-axethrower",
    "icon-ranger",
    "icon-berserker",
    "icon-knight",
    "icon-ogre",
    "icon-paladin",
    "icon-ogre-mage",
    "icon-dwarves",
    "icon-goblin-sappers",
    "icon-mage",
    "icon-death-knight",
    "icon-ballista",
    "icon-catapult",
    "icon-human-oil-tanker",
    "icon-orc-oil-tanker",
    "icon-human-transport",
    "icon-orc-transport",
    "icon-elven-destroyer",
    "icon-troll-destroyer",
    "icon-battleship",
    "icon-ogre-juggernaught",
    "icon-gnomish-submarine",
    "icon-giant-turtle",
    "icon-gnomish-flying-machine",
    "icon-goblin-zeppelin",
    "icon-gryphon-rider",
    "icon-dragon",
    "icon-lothar",
    "icon-gul'dan",
    "icon-uther-lightbringer",
    "icon-zuljin",
    "icon-cho'gall",
    "icon-daemon",
    "icon-farm",
    "icon-pig-farm",
    "icon-town-hall",
    "icon-great-hall",
    "icon-human-barracks",
    "icon-orc-barracks",
    "icon-elven-lumber-mill",
    "icon-troll-lumber-mill",
    "icon-human-blacksmith",
    "icon-orc-blacksmith",
    "icon-human-shipyard",
    "icon-orc-shipyard",
    "icon-human-refinery",
    "icon-orc-refinery",
    "icon-human-foundry",
    "icon-orc-foundry",
    "icon-human-oil-platform",
    "icon-orc-oil-platform",
    "icon-stables",
    "icon-ogre-mound",
    "icon-gnomish-inventor",
    "icon-goblin-alchemist",
    "icon-human-watch-tower",
    "icon-orc-watch-tower",
    "icon-church",
    "icon-altar-of-storms",
    "icon-mage-tower",
    "icon-temple-of-the-damned",
    "icon-keep",
    "icon-stronghold",
    "icon-castle-upgrade",
    "icon-fortress-upgrade",
    "icon-castle",
    "icon-fortress",
    "icon-gryphon-aviary",
    "icon-dragon-roost",
    "icon-gold-mine",
    "icon-human-guard-tower",
    "icon-human-cannon-tower",
    "icon-orc-guard-tower",
    "icon-orc-cannon-tower",
    "icon-oil-patch",
    "icon-dark-portal",
    "icon-circle-of-power",
    "icon-runestone",
    "icon-move-peasant",
    "icon-move-peon",
    "icon-repair",
    "icon-harvest",
    "icon-build-basic",
    "icon-build-advanced",
    "icon-return-goods-peasant",
    "icon-return-goods-peon",
    "icon-cancel",
    "icon-human-wall",
    "icon-orc-wall",
    "icon-slow",
    "icon-invisibility",
    "icon-haste",
    "icon-runes",
    "icon-unholy-armor",
    "icon-lightning",
    "icon-flame-shield", // 100
    "icon-fireball",
    "icon-touch-of-darkness",
    "icon-death-coil",
    "icon-whirlwind",
    "icon-blizzard",
    "icon-holy-vision",
    "icon-heal",
    "icon-death-and-decay",
    "icon-109",
    "icon-exorcism",
    "icon-eye-of-kilrogg",
    "icon-bloodlust",
    "icon-unknown113",
    "icon-skeleton",		// RaiseDead and Skeleton are equal
    "icon-critter",		// Polymorph and Critter are equal
    "icon-sword1",
    "icon-sword2",
    "icon-sword3",
    "icon-battle-axe1",
    "icon-battle-axe2",
    "icon-battle-axe3",
    "icon-122",
    "icon-123",
    "icon-arrow1",
    "icon-arrow2",
    "icon-arrow3",
    "icon-throwing-axe1",
    "icon-throwing-axe2",
    "icon-throwing-axe3",
    "icon-horse1",
    "icon-horse2",
    "icon-longbow",
    "icon-ranger-scouting",
    "icon-ranger-marksmanship",
    "icon-light-axes",
    "icon-berserker-scouting",
    "icon-berserker-regeneration",
    "icon-catapult1",
    "icon-catapult2",
    "icon-ballista1",
    "icon-ballista2",
    "icon-human-demolish",
    "icon-orc-demolish",
    "icon-human-ship-cannon1",
    "icon-human-ship-cannon2",
    "icon-human-ship-cannon3",
    "icon-orc-ship-cannon1",
    "icon-orc-ship-cannon2",
    "icon-orc-ship-cannon3",
    "icon-orc-ship-armor1",
    "icon-orc-ship-armor2",
    "icon-orc-ship-armor3",
    "icon-human-ship-armor1",
    "icon-human-ship-armor2",
    "icon-human-ship-armor3",
    "icon-orc-ship-move",
    "icon-human-ship-move",
    "icon-orc-ship-return-oil",
    "icon-human-ship-return-oil",
    "icon-orc-ship-haul-oil",
    "icon-human-ship-haul-oil",
    "icon-human-unload",
    "icon-orc-unload",
    "icon-human-shield1",
    "icon-human-shield2",
    "icon-human-shield3",
    "icon-orc-shield1",
    "icon-orc-shield2",
    "icon-orc-shield3",
    "icon-170",
    "icon-171",
    "icon-172",
    "icon-173",
    "icon-174",
    "icon-175",
    "icon-176",
    "icon-177",
    "icon-human-patrol-land",
    "icon-orc-patrol-land",
    "icon-human-stand-ground",
    "icon-orc-stand-ground",
    "icon-human-attack-ground",
    "icon-orc-attack-ground",
    "icon-human-patrol-naval",
    "icon-orc-patrol-naval",
    "icon-korgath-bladefist",
    "icon-alleria",
    "icon-danath",
    "icon-teron-gorefiend",
    "icon-grom-hellscream",
    "icon-kurdan-and-sky'ree",
    "icon-deathwing",
    "icon-khadgar",
    "icon-dentarg",
    "icon-turalyon"
};

/**
**	Table aliases for icons.
*/
local const char* IconAliases[][2] = {
    { "icon-raise-dead",	"icon-skeleton" },
    { "icon-polymorph",		"icon-critter" },
};

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Load the graphics for the icons.
*/
global void LoadIcons(void)
{
    unsigned i;
    const char* file;

    //
    //	Load all icon files.
    //
    for( i=0; i<sizeof(Icons)/sizeof(*Icons); ++i ) {
	file=Icons[i].File[TheMap.Terrain];
	if( !file ) {			// default one
	    file=Icons[i].File[0];
	}
	if( *file ) {
	    char* buf;

	    buf=alloca(strlen(file)+9+1);
	    file=strcat(strcpy(buf,"graphic/"),file);
	    ShowLoadProgress("\tIcons %s\n",file);
	    LoadOldGraphic(file,&Icons[i].Icon);
	}
    }

    //
    //	Add icons name to hash table
    //
    for( i=0; i<sizeof(IconNames)/sizeof(*IconNames); ++i ) {
	*(IconId*)hash_add(IconHash,(char*)IconNames[i])=i;
    }

    //
    //	Different names for the same thing
    //
#if 0
    *(unsigned*)hash_add(IconHash,"icon-raise-dead")
	    =IconByIdent("icon-skeleton");
    *(unsigned*)hash_add(IconHash,"icon-polymorph")
	    =IconByIdent("icon-critter");
#endif
    for( i=0; i<sizeof(IconAliases)/sizeof(*IconAliases); ++i ) {
	IconId id;

	id=IconByIdent(IconAliases[i][1]);
	if( id==NoIcon ) {
	    abort();
	}

	*(IconId*)hash_add(IconHash,(char*)IconAliases[i][0])=id;
    }
}

/**
**	Find icon by identifier.
**
**	@param ident	The icon identifier.
**	@return		Icon id or -1 if not found.
*/
global IconId IconByIdent(const char* ident)
{
    const IconId* icon;

    icon=(const IconId*)hash_find(IconHash,(char*)ident);

    if( icon ) { 
	return *icon;
    }

    DebugLevel0(__FUNCTION__": Icon %s not found\n",ident);
    return NoIcon;
}

/**
**	Get identifier of icon.
**
**	@param icon	Icon identifier
**	@return		The identifier for the icon
**
*/
global const char* IdentOfIcon(IconId icon)
{
    DebugCheck( icon<0 || icon>sizeof(IconNames)/sizeof(*IconNames) );

    return IconNames[icon];
}

/**
**	Draw icon 'icon' with border on x,y
**
**	@param icon	Icon identifier
**	@param flags	State of icon (clicked, mouse over...)
**	@param x	X display position
**	@param y	Y display position
*/
global void DrawUnitIcon(IconId icon,unsigned flags,unsigned x,unsigned y)
{
    int color;

    DebugCheck( icon<0 || icon>sizeof(IconNames)/sizeof(*IconNames) );

    color= (flags&IconActive) ? ColorGray : ColorBlack;

    DrawRectangle(color,x,y,ICON_WIDTH+7,ICON_HEIGHT+7);
    DrawRectangle(ColorBlack,x+1,y+1,ICON_WIDTH+5,ICON_HEIGHT+5);

    DrawVLine(ColorGray,x+ICON_WIDTH+4,y+5,ICON_HEIGHT-1);	// _|
    DrawVLine(ColorGray,x+ICON_WIDTH+5,y+5,ICON_HEIGHT-1);
    DrawHLine(ColorGray,x+5,y+ICON_HEIGHT+4,ICON_WIDTH+1);
    DrawHLine(ColorGray,x+5,y+ICON_HEIGHT+5,ICON_WIDTH+1);

    color= (flags&IconClicked) ? ColorGray : ColorWhite;	// |~
    DrawHLine(color,x+5,y+3,ICON_WIDTH+1);
    DrawHLine(color,x+5,y+4,ICON_WIDTH+1);
    DrawVLine(color,x+3,y+3,ICON_HEIGHT+3);
    DrawVLine(color,x+4,y+3,ICON_HEIGHT+3);

    if( (flags&IconClicked) ) {
	++x; ++y;
    }

    DrawOldGraphic(&Icons[0].Icon
	,(icon%5)*Icons[0].Width,(icon/5)*Icons[0].Height
	,Icons[0].Width,Icons[0].Height,x+4,y+4);

    if( flags&IconSelected ) {
	DrawRectangle(ColorGreen,x+4,y+4,ICON_WIDTH-1,ICON_HEIGHT-1);
    }
}

// FIXME:	The icon files must be configurable by ccl
// FIXME:	The icon names must be configurable by ccl
// FIXME:	The icon alias must be configurable by ccl

/*
**	FIXME:
**		I want to support more icon files, this should make it
**		possible that the user can use the original icons and
**		his own icons for his extensions.
*/

//@}
