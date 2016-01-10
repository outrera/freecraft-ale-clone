/**
**	A clone of a famous game.
*/
/**@name sdl.c		-	SDL video support. */
/*
**	(c) Copyright 1999 by Lutz Sammer
**
**	$Id: sdl.c,v 1.6 2000/01/16 23:38:15 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include "clone.h"

#ifdef USE_SDL	// {

#include <stdlib.h>
#include <sys/time.h>
#include <SDL/SDL.h>

#include "video.h"
#include "font.h"
#include "map.h"
#include "interface.h"
#include "network.h"
#include "ui.h"
#include "new_video.h"
#include "sound_server.h"
#include "sound.h"

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	Architecture-dependant videomemory. Set by CloneInitDisplay.
*/
global VMemType* VideoMemory;

/**
**	Architecture-dependant video depth. Set by CloneInitDisplay.
**
**	@see CloneInitDisplay
*/
global int VideoDepth;

global VMemType Pixels[256];
global struct Palette GlobalPalette[256];

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Sync
----------------------------------------------------------------------------*/

global int VideoSyncSpeed=100;		// 0 disable interrupts
volatile int VideoInterrupts;		// be happy, were are quicker

/**
**	Called from SIGALRM.
*/
local Uint32 VideoSyncHandler(Uint32 unused)
{
    DebugLevel3("Interrupt\n");

    ++VideoInterrupts;

    return (100*1000/FRAMES_PER_SECOND)/VideoSyncSpeed;
}

/**
**	Initialise video sync.
*/
global void InitVideoSync(void)
{
    if( !VideoSyncSpeed ) {
	return;
    }

    if( SDL_SetTimer(
		(100*1000/FRAMES_PER_SECOND)/VideoSyncSpeed,
		VideoSyncHandler) ) {
	fprintf(stderr,"Can't set itimer\n");
    }

    // DebugLevel1("Timer installed\n");
}

/*----------------------------------------------------------------------------
--	Video
----------------------------------------------------------------------------*/

local SDL_Surface *Screen;		/// internal screen

/**
**	Initialze the video part for SDL.
*/
global void InitVideoSdl(void)
{

    //	Initialize the SDL library

    if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0 ) {
	fprintf(stderr,"Couldn't initialize SDL: %s\n", SDL_GetError());
	exit(1);
    }

    //	Clean up on exit

    atexit(SDL_Quit);

    // Initialize the display in a 640x480 16-bit palettized mode

    Screen = SDL_SetVideoMode(640, 480, 0,SDL_HWSURFACE|SDL_HWPALETTE
	    | (VideoFullScreen ? SDL_FULLSCREEN : 0));
    if ( Screen == NULL ) {
	fprintf(stderr, "Couldn't set 640x480x16 video mode: %s\n"
		,SDL_GetError());
	exit(1);
    }

#if 0
    // Lock the surface, the hard quick and dirty way
    if( SDL_LockSurface(Screen) ) {
	fprintf(stderr, "Couldn't lock the surface: %s\n"
		,SDL_GetError());
	exit(1);
    }
#endif

    // Turn cursor off, we use our own.
    SDL_ShowCursor(0);

    VideoWidth=640;			// FIXME: should get this
    VideoHeight=480;
    MapWidth = DEFAULT_MAP_WIDTH;	// FIXME: Not the correct way
    MapHeight = DEFAULT_MAP_HEIGHT;

    VideoMemory=Screen->pixels;
    //
    //	I need the used bits per pixel.
    //	You see it's better making all self, than using wired libaries
    //
    {
	int i;
	int j;

	j=Screen->format->Rmask;
	j|=Screen->format->Gmask;
	j|=Screen->format->Bmask;

	for( i=0; j&(1<<i); ++i ) {
	}
	VideoDepth=i;
    }

    DebugLevel3(__FUNCTION__": Video init ready %d\n",VideoDepth);
}

/**
**	Invalidate some area
*/
global void InvalidateArea(int x,int y,int w,int h)
{
    // FIXME: This checks should be done at hight level
    if( x<0 ) {
	w+=x;
	x=0;
    }
    if( y<0 ) {
	h+=y;
	y=0;
    }
    if( !w<=0 && !h<=0 ) {
	SDL_UpdateRect(Screen,x,y,w,h);
    }
}

/**
**	Invalidate whole window
*/
global void Invalidate(void)
{
    SDL_UpdateRect(Screen,0,0,VideoWidth,VideoHeight);
}

/**
**	Handle keyboard!
*/
local void SdlHandleKey(const SDL_keysym* code)
{
    int icode;

    /*
    **	Convert SDL keycodes into internal keycodes.
    */
    KeyModifiers=0;
    switch( (icode=code->sym) ) {
	case SDLK_ESCAPE:
	    icode='\e';
	    break;
	case SDLK_RETURN:
	    icode='\r';
	    break;
	case SDLK_BACKSPACE:
	    icode='\b';
	    break;
	case SDLK_TAB:
	    icode='\t';
	    break;
	case SDLK_UP:
	    icode=KeyCodeUp;
	    break;
	case SDLK_DOWN:
	    icode=KeyCodeDown;
	    break;
	case SDLK_LEFT:
	    icode=KeyCodeLeft;
	    break;
	case SDLK_RIGHT:
	    icode=KeyCodeRight;
	    break;
	case SDLK_PAUSE:
	    icode=KeyCodePause;
	    break;
	case SDLK_F1:
	    icode=KeyCodeF1;
	    break;
	case SDLK_F2:
	    icode=KeyCodeF2;
	    break;
	case SDLK_F3:
	    icode=KeyCodeF3;
	    break;
	case SDLK_F4:
	    icode=KeyCodeF4;
	    break;
	case SDLK_F5:
	    icode=KeyCodeF5;
	    break;
	case SDLK_F6:
	    icode=KeyCodeF6;
	    break;
	case SDLK_F7:
	    icode=KeyCodeF7;
	    break;
	case SDLK_F8:
	    icode=KeyCodeF8;
	    break;
	case SDLK_F9:
	    icode=KeyCodeF9;
	    break;
	case SDLK_F10:
	    icode=KeyCodeF10;
	    break;
	case SDLK_F11:
	    icode=KeyCodeF11;
	    break;
	case SDLK_F12:
	    icode=KeyCodeF12;
	    break;

        // We need these because if you only hit a modifier key,
        // the *ots from SDL didn't report correct modifiers
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
	    KeyModifiers|=ModifierShift;
	    break;
	case SDLK_LCTRL:
	case SDLK_RCTRL:
	    KeyModifiers|=ModifierControl;
	    break;
	case SDLK_LALT:
	case SDLK_RALT:
	case SDLK_LMETA:
	case SDLK_RMETA:
	    KeyModifiers|=ModifierAlt;
	    break;
	case SDLK_LSUPER:
	case SDLK_RSUPER:
	    KeyModifiers|=ModifierSuper;
	    break;
	default:
	    if( code->mod&(KMOD_LSHIFT|KMOD_RSHIFT) ) {
		if(icode <= 'z' && icode >= 'a') {
		    icode -= 32;
		}
	    }
	    break;
    }

    if( code->mod&(KMOD_LCTRL|KMOD_RCTRL) ) {
	KeyModifiers|=ModifierControl;
    }
    if( code->mod&(KMOD_LSHIFT|KMOD_RSHIFT) ) {
	KeyModifiers|=ModifierShift;
    }
    if( code->mod&(KMOD_LALT|KMOD_RALT) ) {
	KeyModifiers|=ModifierAlt;
    }
    DebugLevel3("%d\n",KeyModifiers);

    if( HandleKeyDown(icode) ) {
	return;
    }
    DoBottomPanelKey(icode);
}

/**
**	Handle interactive input event.
*/
local void DoEvent(SDL_Event* event)
{
    switch( event->type ) {
	case SDL_MOUSEBUTTONDOWN:
	    DebugLevel3("\tbutton press %d\n",event->button.button);
	    HandleButtonDown(event->button.button);
	    break;

	case SDL_MOUSEBUTTONUP:
	    DebugLevel3("\tbutton release %d\n",event->button.button);
	    HandleButtonUp(event->button.button);
	    break;

	    // FIXME SDL: check if this is only usefull for the cursor
	    //            if this is the case we don't need this.
	case SDL_MOUSEMOTION:
	    DebugLevel3("\tmotion notify %d,%d\n",event->motion.x,event->motion.y);
	    HandleMouseMove(event->motion.x,event->motion.y);
	    if (TheUI.WarpX != -1 || TheUI.WarpY != -1) {
		int xw;
		int yw;

		xw = TheUI.WarpX;
		yw = TheUI.WarpY;
		TheUI.WarpX = -1;
		TheUI.WarpY = -1;
		SDL_WarpMouse(xw,yw);
	    }
	    MustRedraw|=RedrawCursor;
	    break;

	case SDL_ACTIVEEVENT:
	    DebugLevel3("\tFocus changed\n");
	    if( !event->active.state ) {
		CursorOn=-1;
	    }
	    break;

	case SDL_KEYDOWN:
	    DebugLevel3("\tKey press\n");
	    SdlHandleKey(&event->key.keysym);
	    break;

	case SDL_KEYUP:
	    DebugLevel3("\tKey release\n");
	    break;

	case SDL_QUIT:
	    Exit(0);
	    break;
    }
}

/**
**	Wait for interactive input event.
**
**	Handles X11 events, keyboard, mouse.
**	Video interrupt for sync.
**	Network messages.
**	Sound queue.
**
**	We must handle atlast one X11 event
**
**	FIXME:	the initialition could be moved out of the loop
*/
global void WaitEventsAndKeepSync(void)
{
#ifndef USE_WIN32
    struct timeval tv;
    fd_set rfds;
    fd_set wfds;
    int maxfd;
#endif
    SDL_Event event[1];

    for(;;) {
	// Not very nice, but this is the problem if you use other libraries
	// The event handling of SDL is very buggy and wrong designed.
	if( SDL_PollEvent(event) ) {
	    // Handle SDL event
	    DoEvent(event);
	} else {
#ifndef USE_WIN32
	    //
	    //	Prepare select
	    //
	    tv.tv_sec=0;
	    tv.tv_usec=0;
	    FD_ZERO(&rfds);
	    FD_ZERO(&wfds);
	    maxfd=0;

	    //
	    //	Network
	    //
	    if( NetworkFildes!=-1 ) {
		if( NetworkFildes>maxfd ) {
		    maxfd=NetworkFildes;
		}
		FD_SET(NetworkFildes,&rfds);
	    }

	    //
	    //	Sound
	    //
	    if( !SoundOff && !SoundThreadRunning ) {
		if( SoundFildes>maxfd ) {
		    maxfd=SoundFildes;
		}
		FD_SET(SoundFildes,&wfds);
	    }

	    maxfd=select(maxfd+1,&rfds,&wfds,NULL
		    ,SDL_PollEvent(NULL) ? &tv : NULL);

	    if( maxfd>0 ) {
		//
		//	Network
		//
		if( NetworkFildes!=-1 && FD_ISSET(NetworkFildes,&rfds) ) {
		    NetworkEvent();
		}

		//
		//	Network in sync and time for frame over: return
		//
		if( NetworkInSync && VideoInterrupts ) {
		    return;
		}

		//
		//	Sound
		//
		if( !SoundOff && !SoundThreadRunning
			    && FD_ISSET(SoundFildes,&wfds) ) {
		    WriteSound();
		}
	    }
#endif
	}

	//
	//	Network in sync and time for frame over: return
	//
	if(NetworkInSync && VideoInterrupts) {
	    return;
	}
    }
}

/**
**	Create palette.
*/
global void VideoCreatePalette(struct Palette* palette)
{
    int i;

    for(i = 0; i < 256; i++) {
	Pixels[i]=SDL_MapRGB(Screen->format,
		(palette[i].r<<2)&0xFF,
		(palette[i].g<<2)&0xFF,
		(palette[i].b<<2)&0xFF);
	DebugLevel3(__FUNCTION__": %02x %02x %02x\n",
		(palette[i].r<<2)&0xFF,
		(palette[i].g<<2)&0xFF,
		(palette[i].b<<2)&0xFF);
    }
    SetPlayersPalette();
}

/**
**	Load a picture and display it on the screen (full screen),
**	changing the colormap and so on..
**
**	@param name name of the picture (file) to display
*/
global void DisplayPicture(const char *name)
{
    OldGraphic title;
    extern unsigned char PalettePNG[];

    LoadOldGraphic(name,&title);
    memcpy(GlobalPalette,PalettePNG,sizeof(GlobalPalette));
    VideoCreatePalette(GlobalPalette);
    DrawOldGraphic(&title,0,0,VideoWidth,VideoHeight,0,0);
    FreeOldGraphic(&title);
}

/**
**	Color cycle.
*/
global void ColorCycle(void)
{
    int i;
    int x;

    // FIXME: this isn't 100% correct
    // Color cycling info - forest:
    // 3	flash red/green	(attacked building on minimap)
    // 38-47	cycle		(water)
    // 48-56	cycle		(water-coast boundary)
    // 202	pulsates red	(Circle of Power)
    // 240-244	cycle		(water around ships, Runestone, Dark Portal)
    // Color cycling info - swamp:
    // 3	flash red/green	(attacked building on minimap)
    // 4	pulsates red	(Circle of Power)
    // 5-9	cycle		(Runestone, Dark Portal)
    // 38-47	cycle		(water)
    // 88-95	cycle		(waterholes in coast and ground)
    // 240-244	cycle		(water around ships)
    // Color cycling info - wasteland:
    // 3	flash red/green	(attacked building on minimap)
    // 38-47	cycle		(water)
    // 64-70	cycle		(coast)
    // 202	pulsates red	(Circle of Power)
    // 240-244	cycle		(water around ships, Runestone, Dark Portal)
    // Color cycling info - winter:
    // 3	flash red/green	(attacked building on minimap)
    // 40-47	cycle		(water)
    // 48-54	cycle		(half-sunken ice-floe)
    // 202	pulsates red	(Circle of Power)
    // 205-207	cycle		(lights on christmas tree)
    // 240-244	cycle		(water around ships, Runestone, Dark Portal)

    x=Pixels[38];
    for( i=38; i<47; ++i ) {		// tileset color cycle
	Pixels[i]=Pixels[i+1];
    }
    Pixels[47]=x;

    x=Pixels[240];
    for( i=240; i<244; ++i ) {		// units/icons color cycle
	Pixels[i]=Pixels[i+1];
    }
    Pixels[244]=x;

    MapColorCycle();		// FIXME: could be little more informativer
    MustRedraw|=RedrawMap|RedrawTopPanel;
}

/**
**	Check video interrupt.
**
**	Display and count too slow frames.
*/
global void CheckVideoInterrupts(void)
{
    if( VideoInterrupts ) {
        //DebugLevel1("Slow frame\n");
	IfDebug(
	    DrawText(MAP_X+10,MAP_Y+10,SMALL_FONT,"SLOW FRAME!!");
	);
        ++SlowFrameCounter;
    }
}

/**
**	Realize video memory.
*/
global void RealizeVideoMemory(void)
{
}

#endif // } USE_SDL

//@}
