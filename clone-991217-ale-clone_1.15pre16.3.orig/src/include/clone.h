/*
**	A clone of a famous game.
*/
/**@name clone.h	-	The main header file. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: clone.h,v 1.43 1999/12/09 16:27:04 root Exp $
*/

#ifndef __CLONE_H__
#define __CLONE_H__

//@{

/*============================================================================
==	Config definitions
============================================================================*/

#define DEBUG				/// Define to include debug code
#define USE_HICOLOR			/// Define to use 16-bit color
#define noUSE_CCL			/// Remove no for version with guile
#define noUSE_THREAD			/// Remove no for version with thread
#define noUSE_SDL			/// Remove no for sdl support
#define noUSE_X11			/// Remove no for x11 support

/**
**	Define this to support load of compressed (gzip) pud files
**	and other data files. (If defined you need libz)
**	Comment if you have problems with gzseek, ... and other gz functions.
*/
#define noUSE_ZLIB

/**
**	Define this to support load of compressed (libbz2) pud files
**	and other data files. (If defined you need libbz2)
*/
#define noUSE_BZ2LIB

#define SPEED_MINE	1		/// speed factor for mine gold
#define SPEED_GOLD	1		/// speed factor for getting gold
#define SPEED_CHOP	1		/// speed factor for chop
#define SPEED_WOOD	1		/// speed factor for getting wood
#define SPEED_HAUL	1		/// speed factor for haul oil
#define SPEED_OIL	1		/// speed factor for getting oil
#define SPEED_BUILD	1		/// speed factor for building
#define SPEED_TRAIN	1		/// speed factor for training
#define SPEED_UPGRADE	1		/// speed factor for upgrading
#define SPEED_RESEARCH	1		/// speed factor for researching

/*============================================================================
==	Debug definitions
============================================================================*/

#ifdef DEBUG	// {

/**
**	Include code only if debugging.
*/
#define IfDebug(code)	code

/**
**	Debug check condition
*/
#define DebugCheck(cond)	do{ if( cond ) { \
	printf("DebugCheck at %s:%d\n",__FILE__,__LINE__); \
	abort(); } }while( 0 )

/**
**	Print debug information of level 0.
*/
#define DebugLevel0(fmt...)	printf(fmt##)

/**
**	Print debug information of level 1.
*/
#define DebugLevel1(fmt...)	printf(fmt##)

/**
**	Print debug information of level 2.
*/
#define DebugLevel2(fmt...)	printf(fmt##)

/**
**	Print debug information of level 3.
*/
#define DebugLevel3(fmt...)	/* TURNED OFF: printf(fmt##) */

#else	// }{ DEBUG

#define IfDebug(code)
#define DebugCheck(cond)
#define DebugLevel0(fmt...)
#define DebugLevel1(fmt...)
#define DebugLevel2(fmt...)
#define DebugLevel3(fmt...)

#endif	// } !DEBUG

/*============================================================================
==	Storage types
============================================================================*/

#define global				/// defines global visible names

#ifdef DEBUG
#define local				/// defines local visible names
#else
#define local static
#endif

/*============================================================================
==	Definitions
============================================================================*/

/*----------------------------------------------------------------------------
--	General
----------------------------------------------------------------------------*/

#ifndef CLONE_LIB_PATH
#define CLONE_LIB_PATH	"data"		/// where to find the data files
#endif
#ifndef CLONE_HOME_PATH
#define CLONE_HOME_PATH	".clone"	/// data files in user home dir
#endif

#define MAGIC_FOR_NEW_UNITS	85	/// magic value, new units start with
#define DEMOLISH_DAMAGE		400	/// damage for demolish attack

/*----------------------------------------------------------------------------
--	Screen
----------------------------------------------------------------------------*/

// FIXME: this values should go into a general ui structure.

#define noGRID		1		/// Map is show with a grid, if 1

#define MAP_X		176		/// map X position on screen
#define MAP_Y		16		/// map Y position on screen
#define MAXMAP_W	26		/// maximum map width in tiles
#define MAXMAP_H	23		/// maximum map height in tiles

#define MINIMAP_X	24		/// minimap X origin
#define MINIMAP_Y	26		/// minimap Y origin
#define MINIMAP_W	128		/// minimap width in pixels
#define MINIMAP_H	128		/// minimap height in pixels

    /// scrolling area (<= 10 y)
#define SCROLL_UP	10
    /// scrolling area (>= VideoHeight-11 y)
#define SCROLL_DOWN	(VideoHeight-11)
    /// scrolling area (<= 10 y)
#define SCROLL_LEFT	10
    /// scrolling area (>= VideoWidth-11 x)
#define SCROLL_RIGHT	(VideoWidth-11)

    /// mouse scrolling magnify
#define MOUSE_SCROLL_SPEED	3

    /// keyboard scrolling magnify
#define KEY_SCROLL_SPEED	1

    /// frames per second to display (original 30-40)
#define FRAMES_PER_SECOND	30	// 1/30s

    /// must redraw flags
enum MustRedraw_e {
    RedrawEverything	= -1,		/// must redraw everything
    RedrawNothing	= 0,		/// nothing to do
    RedrawMinimap	= 1,		/// Minimap area
    RedrawMap		= 2,		/// Map area
    RedrawCursor	= 4,		/// Cursor changed
    RedrawResources	= 8,		/// Resources
    RedrawMessage	= 16,		/// Message
    RedrawStatusLine	= 32,		/// Statusline
    RedrawTopPanel	= 64,		/// Unit description
    RedrawBottomPanel	= 128,		/// Unit buttons
    RedrawRightBorder	= 256,		/// Border on right side
    RedrawMinimapBorder	= 512,		/// Area around minimap
    RedrawCosts		= 1024,		/// Costs in status line
    RedrawMinimapTop	= 2048,		/// Area above minimap
    RedrawMinimapCursor	= 4096		/// Minimap cursor changed
};

    /// Must redraw all maps
#define RedrawMaps		(RedrawMinimap|RedrawMap)
    /// Must redraw all panels
#define RedrawPanels		(RedrawTopPanel|RedrawBottomPanel)

/**
**	Show load progress.
**	FIXME: Some time this should be shown in tile screen.
*/
#define ShowLoadProgress(fmt...)	//printf(fmt##)

    /// mainscreen width (default 640)
extern int VideoWidth;

    /// mainscreen height (default 480)
extern int VideoHeight;

    /// invalidated map
extern enum MustRedraw_e MustRedraw;

    /// counts frames
extern int FrameCounter;

    /// counts quantity of slow frames
extern int SlowFrameCounter;

/*----------------------------------------------------------------------------
--	Convert
----------------------------------------------------------------------------*/

extern int Screen2MapX(int x);		/// Convert screen pixel to map tile
extern int Screen2MapY(int y);		/// Convert screen pixel to map tile
extern int Map2ScreenX(int x);		/// Convert map tile to screen pixel
extern int Map2ScreenY(int y);		/// Convert map tile to screen pixel

/*----------------------------------------------------------------------------
--	clone.c
----------------------------------------------------------------------------*/

/**
**	SyncRand():	should become a syncron rand on all machines
**			for network play.
*/
#define NoSyncRand()	rand()

/**
**	MyRand():	rand only used on this computer.
*/
#define MyRand()	rand()

extern int FlagRevealMap;		/// Reveal map immediately
extern char* TitleScreen;		/// file for title screen

extern int SpeedMine;			/// speed factor for mine gold
extern int SpeedGold;			/// speed factor for getting gold
extern int SpeedChop;			/// speed factor for chop
extern int SpeedWood;			/// speed factor for getting wood
extern int SpeedHaul;			/// speed factor for haul oil
extern int SpeedOil;			/// speed factor for getting oil
extern int SpeedBuild;			/// speed factor for building
extern int SpeedTrain;			/// speed factor for training
extern int SpeedUpgrade;		/// speed factor for upgrading
extern int SpeedResearch;		/// speed factor for researching

extern void SaveAll(void);		/// Call all modules to save states

extern int SyncRand(void);

extern int main1(int argc,char* argv[]);/// init clone.
extern volatile void Exit(int err);	/// exit clone.

extern void SetMessage(char* message);
extern void SetMessageDup(char* message);
extern void ClearMessage(void);
extern void SetStatusLine(char* status);
extern void ClearStatusLine(void);
extern void CloneMainLoop(void);

/*============================================================================
==	Misc
============================================================================*/

    /// max macro
#define max(n1,n2)	(((n1)<(n2)) ? (n2) : (n1))

    /// bits macro
#define BitsOf(n)	(sizeof(n)*8)

    /// How long stay in a gold-mine
#define MINE_FOR_GOLD	(UnitTypes[UnitGoldMine]._Costs[TimeCost]/SpeedMine)
    /// How long stay in a gold-deposit
#define WAIT_FOR_GOLD	(UnitTypes[UnitGoldMine]._Costs[TimeCost]/SpeedGold)
    /// How much I must chop for 1 wood
#define CHOP_FOR_WOOD	(52/SpeedChop)
    /// How long stay in a wood-deposit
#define WAIT_FOR_WOOD	(100/SpeedWood)
    /// How long stay in a oil-well
#define HAUL_FOR_OIL	(100/SpeedHaul)
    /// How long stay in a oil-deposit
#define WAIT_FOR_OIL	(100/SpeedOil)

    /// How near could a hall or gold-depot be build to a goldmine.
#define GOLDMINE_DISTANCE	3
    /// How near could a oil-depot be build to a oil-patch.
#define OILPATCH_DISTANCE	3

//@}

#endif	// !__CLONE_H__
