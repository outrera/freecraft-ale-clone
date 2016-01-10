/*
**	A clone of a famous game.
*/
/**@name video.h	-	The video headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: video.h,v 1.21 1999/12/10 01:33:39 root Exp $
*/

#ifndef __VIDEO_H__
#define __VIDEO_H__

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include "new_video.h"

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

typedef unsigned long VMemType32;	/// 32 bpp modes pointer
typedef unsigned short VMemType16;	/// 16 bpp modes pointer
typedef unsigned char VMemType8;	///  8 bpp modes pointer

#ifdef USE_HICOLOR
typedef unsigned short VMemType;	/// video memory access type
#else
typedef unsigned char VMemType;		/// video memory access type
#endif

typedef unsigned char GraphicData;	/// generic graphic data type

    /// MACRO defines speed of colorcycling
#define COLOR_CYCLE_SPEED	(FRAMES_PER_SECOND/4)

/// System-Wide used colors.
enum SysColors {
    /** */ ColorBlack = 0,
    /** */ ColorDarkGreen = 149,
    /** */ ColorBlue = 206,
    /** */ ColorWhite = 246,
    /** */ ColorNPC = 247,
    /** */ ColorGray = 248,
    /** */ ColorRed = 249,
    /** */ ColorGreen = 250,
    /** */ ColorYellow = 251,
    /** */ ColorBlinkRed = 252,
    /** */ ColorViolett = 253,
    /** */ ColorWaterCycleStart = 38,
    /** */ ColorWaterCycleEnd = 47,
    /** */ ColorIconCycleStart = 240,
    /** */ ColorIconCycleEnd = 244
};

typedef enum SysColors SysColors;

/**@name OldGraphic */
//@{

/**
**	Graphic structure. Non-transparent, single frame, not packed image.
**	Very fast. Useful for large & beautiful backgrounds, etc.
**
**	@see LoadPNG
**	@see NewOldGraphic
**	@see DrawOldGraphic
**	@see FreeOldGraphic
*/
struct OldGraphic {		// hardware/system dependend
    unsigned		Width;		/// width of bitmap
    int			Height;		/// height of bitmap
	/// bitmap itself (8-bit for Pixels[] conversion)
    GraphicData*	Data;
};

typedef struct OldGraphic OldGraphic;

/** Load Graphic from PNG file. This graphic should be freed then using
  FreeGraphic. [ Valery - Imho, this function should be eliminated by use
  of the LoadOldGraphic() ].
  @param name resource file name 
  @param graphic uninitialized instance of Graphic object
  @see FreeOldGraphic
 */
extern int LoadPNG(const char* name,OldGraphic* graphic);

/** Make new OldGraphic. Use FreeOldGraphic to free.
  @param w width
  @param h height
  @param graphic unitialized instance of OldGraphic object
  @see FreeOldGraphic
 */
extern void NewOldGraphic(int w,int h,OldGraphic* graphic);

/** Load the Graphic from resource. Use FreeOldGraphic to free.
  @param name resource file name 
  @param graphic uninitialized instance of Graphic object
  @see FreeOldGraphic
*/
extern void LoadOldGraphic(const char* name,OldGraphic* graphic);

/** Draw the Graphic on videoscreen. WARNING, gx,gy,w,h,x,y must contain correct
  values, because of performance no painful checking was made.
  @param graphic initialized instance of Graphic object
  @param gx x origin inside of the Graphic object
  @param gy y origin inside of the Graphic object
  @param w width to read from the Graphic object
  @param h height to read from the Graphic object
  @param x x destination coordinate on the screen
  @param y y destination coordinate on the screen
 */
extern void DrawOldGraphic(OldGraphic* graphic,int gx,int gy,int w,int h,int x,int y);
/** Free previously initialized instance of Graphic. Please don't forget, that
  pointer `graphic' is not freed, only bitmap data is freed.
  @param graphic initialized instance of the Graphic object
 */
extern void FreeOldGraphic(OldGraphic* graphic);

/**
**	Load a picture and display it on the screen (full screen),
**	changing the colormap and so on..
**
**	@param name name of the picture (file) to display
*/
extern void DisplayPicture(const char *name);

//@}

/**@name Sprite */
//@{

/** Sprite structure. Transparent, single frame, not packed image.
  Fast enough. Primarily used for fonts. color 255 is transparent.
  @see LoadSprite
  @see DrawSprite
  @see DrawSpriteClipped
  @see DrawSpriteX
  @see FreeOldGraphic
 */
struct Sprite {		// hardware/system dependend
    /// width of sprite
    int			Width;
    /// height of sprite
    int			Height;
    /// bitmap itself (converted via Pixels[])
    unsigned char*	Data;
};

typedef struct Sprite Sprite;

/** Load the Sprite from resource. Use FreeOldGraphic to free.
  @param name resource file name 
  @param sprite uninitialized instance of Sprite object
  @see FreeOldGraphic
*/
extern void LoadSprite(const char* name,Sprite* sprite);

/** Draw the Sprite on videoscreen CLIPPED. WARNING, gx,gy,w,h,x,y must contain
  correct values, because of performance no painful checking was made.
  @param sprite initialized instance of Sprite object
  @param gx x origin inside of the Sprite object
  @param gy y origin inside of the Sprite object
  @param w width to read from the Sprite object
  @param h height to read from the Sprite object
  @param x x destination coordinate on the screen
  @param y y destination coordinate on the screen
 */
extern void DrawSpriteClipped(Sprite* sprite
		,int gx,int gy,int w,int h,int x,int y);

/** Draw the Sprite on videoscreen. WARNING, gx,gy,w,h,x,y must contain correct
  values, because of performance no painful checking was made.
  @param sprite initialized instance of Sprite object
  @param gx x origin inside of the Sprite object
  @param gy y origin inside of the Sprite object
  @param w width to read from the Sprite object
  @param h height to read from the Sprite object
  @param x x destination coordinate on the screen
  @param y y destination coordinate on the screen
 */
extern void DrawSprite(Sprite* sprite,int gx,int gy,int w,int h,int x,int y);

/** Draw the Sprite on videoscreen with flipped X coordinate.
  WARNING, gx,gy,w,h,x,y must contain correct values, because of
  performance no painful checking was made.
  @param sprite initialized instance of Sprite object
  @param gx x origin inside of the Sprite object
  @param gy y origin inside of the Sprite object
  @param w width to read from the Sprite object
  @param h height to read from the Sprite object
  @param x x destination coordinate on the screen
  @param y y destination coordinate on the screen
 */
extern void DrawSpriteX(Sprite* sprite,int gx,int gy,int w,int h,int x,int y);

//@}

/**@name RleSprite */
//@{

/** RleSprite structure. Transparent, single frame, not packed image.
  Fast enough. Primarily used for fonts. color 255 is transparent.
  @see LoadRleSprite
  @see DrawRleSprite
  @see DrawRleSpriteClipped
  @see DrawRleSpriteClippedX
  @see DrawRleSpriteX
  @see FreeRleSprite
 */
struct RleSprite {
    /// width of a frame
    unsigned		Width;
    /// height of a frame
    unsigned		Height;
    /// palette FUTURE extension
    VMemType*		Pixels;
    /// number of frames
    int			NumFrames;
    /** pointer to the frames. frames are bitmaps converted via global
      Pixels[] structure or local object Pixels (if local Pixels != 0). */
    unsigned char*	Frames[0];
};

typedef struct RleSprite RleSprite;

/** Load RLE Sprite. new memory block is allocated.
  @param name resource file name
  @param w width of a frame
  @param h height of a frame
 */
extern RleSprite* LoadRleSprite(const char* name,unsigned w,unsigned h);

/** Draw RLE Sprite..
  @param sprite pointer to RLE Sprite OBJECT
  @param frame number of frame
  @param x x coordinate on the screen
  @param y y coordinate on the screen
 */
extern void DrawRleSprite(RleSprite* sprite,unsigned frame,int x,int y);

/** Draw RLE Sprite FLIPPED X..
  @param sprite pointer to RLE Sprite OBJECT
  @param frame number of frame
  @param x x coordinate on the screen
  @param y y coordinate on the screen
 */
extern void DrawRleSpriteX(RleSprite* sprite,unsigned frame,int x,int y);

/** Draw RLE Sprite CLIPPED..
  @param sprite pointer to RLE Sprite OBJECT
  @param frame number of frame
  @param x x coordinate on the screen
  @param y y coordinate on the screen
 */
extern void DrawRleSpriteClipped(RleSprite* sprite,unsigned frame,int x,int y);

/** Draw RLE Sprite CLIPPED FLIPPED X..
  @param sprite pointer to RLE Sprite OBJECT
  @param frame number of frame
  @param x x coordinate on the screen
  @param y y coordinate on the screen
 */
extern void DrawRleSpriteClippedX(RleSprite* sprite,unsigned frame,int x,int y);

/** Free pointer returned by LoadRleSprite. Dereferencing pointer is no longer
  correct.
  @param sprite pointer returned by LoadRleSprite
 */
extern void FreeRleSprite(RleSprite *sprite);
//@}

/**@name Vector graphics primitives */

//@{
/** Set clipping for nearly all vector primitives. Functions which support
  clipping will be marked CLIPPED. Set system-wide clipping rectangle.
  @param left left x coordinate
  @param top  top y coordinate
  @param right right x coordinate
  @param bottom bottom y coordinate
 */
extern void SetClipping(int left,int top,int right,int bottom);

/** Draws point..
  @param color system-defined color
  @param x x screen coordinate
  @param y y screen coordinate
 */
extern void DrawPointUnclipped(SysColors color,int x,int y);

/** Draws point CLIPPED..
  @param color system-defined color
  @param x x screen coordinate
  @param y y screen coordinate
 */
extern void DrawPoint(SysColors color,int x,int y);

/** Draws vertical line CLIPPED..
  @param color system-defined color
  @param x x coordinate
  @param y y coordinate
  @param height height of the line
 */
extern void DrawVLine(SysColors color,int x,int y,int height);

/** Draws horizontal line CLIPPED..
  @param color system-defined color
  @param x x coordinate
  @param y y coordinate
  @param width width of the line
 */
extern void DrawHLine(SysColors color,int x,int y,int width);

/** Draws not filled rectangle CLIPPED..
  @param color system-defined color
  @param x x coordinate
  @param y y coordinate
  @param w width
  @param h height
 */
extern void DrawRectangle(SysColors color,int x,int y,int w,int h);

/** Draws filled rectangle CLIPPED..
  @param color system-defined color
  @param x x coordinate
  @param y y coordinate
  @param w width
  @param h height
 */
extern void FillRectangle(SysColors color,int x,int y,int w,int h);

/** Draw line CLIPPED..
  @param color system-defined color
  @param x1 x starting coordinate
  @param y1 y starting coordinate
  @param x2 x ending coordinate
  @param y2 y ending coordinate
 */
extern void DrawLine(SysColors color,int x1,int y1,int x2,int y2);

/** MACRO for fast drawing unclipped points..
  @param color system-defined color
  @param x x coordinate
  @param y y coordinate
 */
#define DrawPointUnclipped(color,x,y)	\
    (VideoMemory[(x)+(y)*VideoWidth]=Pixels[(int)(color)])
//@}


/** Architecture-dependant videomemory. Set by CloneInitDisplay.
  @see CloneInitDisplay
 */
extern VMemType* VideoMemory;

/** Architecture-dependant video depth. Set by CloneInitDisplay.
  @see CloneInitDisplay
*/
extern int VideoDepth;

    /// 32 bpp modes video memory address
#define VideoMemory32	((VMemType32*)VideoMemory)
    /// 16 bpp modes video memory address
#define VideoMemory16	((VMemType16*)VideoMemory)
    ///8 bpp modes video memory address
#define VideoMemory8	((VMemType8*)VideoMemory)

/** Architecture-dependant system palette. Applies as conversion between
  GlobalPalette colors and their representation in videomemory. Set by
  VideoCreatePalette.
  @see VideoCreatePalette
 */
extern VMemType Pixels[256];		// standard colors
#define Pixels32	Pixels		/// 32 bpp not written :(
#define Pixels16	Pixels		/// 16 bpp default
#define Pixels8		Pixels		///  8 bpp not written :(


/// Palette structure.
struct Palette {
    /// RED COMPONENT
    unsigned char r;
    /// GREEN COMPONENT
    unsigned char g;
    /// BLUE COMPONENT
    unsigned char b;
};

typedef struct Palette Palette;

/** Set videomode. Tries to set choosen videomode. Only 640x480, 800x600
  and 1024x768 are available. If videoadapter is not good enough module will
  return failure. Default mode is 640x480.
  @return 1 if videomode set correctly; 0 otherwise. */
extern int SetVideoMode(int width);

/** Loaded system palette. 256-entries long, active system palette. */
extern struct Palette GlobalPalette[256];

/** Load palette from resource. Just loads palette, to set it use
  VideoCreatePalette, which sets system palette.
  @param pal buffer to store palette (256-entries long)
  @param name resource file name
 */
extern void LoadRGB(struct Palette *pal, const char *name);

/// Initialize Pixels[] for all players (bring Players[] in sync with Pixels[])
extern void SetPlayersPalette(void);

/**@name Architecture-dependant video functions */
//@{

/// Initializes video synchronization..
extern void InitVideoSync(void);

/// Prints warning if video is too slow..
extern void CheckVideoInterrupts(void);

/// Does ColorCycling..
extern void ColorCycle(void);

/** Initializes system palette. Also calls SetPlayersPalette to set palette for
  all players.
  @param palette Palette structure
  @see SetPlayersPalette
 */
extern void VideoCreatePalette(struct Palette *palette);

/// Process all system events. This function also keeps synchronization of game.
extern void WaitEventsAndKeepSync(void);

/** Realize videomemory. X11 implemenataion just does XFlush. SVGALIB without
 linear addressing should use this. */
extern void RealizeVideoMemory(void);

/// Initialize video hardware..
extern void CloneInitDisplay(void);

/** Invalidates selected area on window or screen. Use for accurate redrawing.
  in so
  @param x x coordinate
  @param y y coordinate
  @param w width
  @param h height
 */
extern void InvalidateArea(int x,int y,int w,int h);

/// Simply invalidates whole window or screen.. 
extern void Invalidate(void);

//@}

/** Counter. Counts how many video interrupts occured, while proceed event
  queue. If <1 simply do nothing, =1 means that we should redraw screen. >1
  means that framerate is too slow.
  @see CheckVideoInterrupts
 */
extern volatile int VideoInterrupts;

/**
** Video synchronization speed. Synchronization time in milliseconds.
** If =0, video framerate is not synchronized. Clone will try to redraw screen
** within intervals of VideoSyncSpeed, not more, not less
*/
extern int VideoSyncSpeed;

/**
**	Wanted videomode, fullscreen or windowed.
*/
extern int VideoFullScreen;

//@}

#endif	// !__VIDEO_H__
