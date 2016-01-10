/**
**	A clone of a famous game.
*/
/**@name svgalib.c		-	svgalib support. */
/*
**	(c) Copyright 1999 by Jarek Sobieszek
*/

//@{

#ifdef USE_SVGALIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <vga.h>
#include <vgamouse.h>
#include <vgakeyboard.h>

#include "clone.h"
#include "video.h"
#include "tileset.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "map.h"
#include "minimap.h"
#include "font.h"
#include "image.h"
#include "sound_server.h"
#include "missile.h"
#include "sound.h"
#include "cursor.h"
#include "interface.h"
#include "network.h"
#include "ui.h"

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

local int old_button;
local int mouse_x;
local int mouse_y;

local void MouseEvent(int button, int dx, int dy, int dz, int drx, int dry, int drz);
local void KeyboardEvent(int scancode, int press);

/*----------------------------------------------------------------------------
--	Sync
----------------------------------------------------------------------------*/

global int VideoSyncSpeed=100;		// 0 disable interrupts
volatile int VideoInterrupts;		// be happy, were are quicker

/**
**	Called from SIGALRM.
*/
local void VideoSyncHandler(int unused)
{
    DebugLevel3("Interrupt\n");
    ++VideoInterrupts;
}

/**
**	Initialise video sync.
*/
global void InitVideoSync(void)
{
    struct sigaction sa;
    struct itimerval itv;

    if( !VideoSyncSpeed ) {
	return;
    }

    sa.sa_handler=VideoSyncHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    if( sigaction(SIGALRM,&sa,NULL) ) {
	fprintf(stderr,"Can't set signal\n");
    }

    itv.it_interval.tv_sec=itv.it_value.tv_sec=
	(100/FRAMES_PER_SECOND)/VideoSyncSpeed;
    itv.it_interval.tv_usec=itv.it_value.tv_usec=
	(100000000/FRAMES_PER_SECOND)/VideoSyncSpeed-
	itv.it_value.tv_sec*100000;
    if( setitimer(ITIMER_REAL,&itv,NULL) ) {
	fprintf(stderr,"Can't set itimer\n");
    }

    // DebugLevel1("Timer installed\n");
}

local void CloseDisplay(void)
{
    free(VideoMemory);
    vga_setmode(TEXT);
}

/**
**	X11 initialize.
*/
global void CloneInitDisplay(void)
{
    int real_uid;

    VideoDepth = 16;

    VideoWidth = 640;
    VideoHeight = 480;
    MapWidth = 14;			// FIXME: Not the correct way
    MapHeight = 14;

    real_uid = getuid();
    setuid(geteuid());

    if(vga_init() == -1) {
	fprintf(stderr, "Cannot initialize svgalib.\n");
	exit(-1);
    }
    VideoMemory = (VMemType *)malloc(VideoWidth * VideoHeight * (VideoDepth >> 3));
    if(VideoMemory == NULL) {
	fprintf(stderr, "Cannot allocate virtual screen.\n");
	exit(-1);
    }
    DebugLevel0("OK\n");
    if(atexit(CloseDisplay) == -1) {
	fprintf(stderr, "Cannot register CloseDisplay.\n");
	free(VideoMemory);
	exit(-1);
    }
    DebugLevel0("OK\n");
    if(vga_setmode(G640x480x64K) == -1) {
	fprintf(stderr, "16bpp 640x480 mode is not available.\n");
	exit(-1);
    }
    DebugLevel0("OK\n");

#ifdef USE_SVGALINEAR
    if(vga_setlinearaddressing() == -1) {
	fprintf(stderr, "Cannot enable linear addressing.\n");
	exit(-1);
    }
    DebugLevel0("OK\n");
#endif

    if(mouse_init("/dev/mouse", vga_getmousetype(),
		MOUSE_DEFAULTSAMPLERATE) == -1) {
	fprintf(stderr, "Cannot enable mouse.\n");
	exit(-1);
    }
    if(atexit(mouse_close) == -1) {
	fprintf(stderr, "Cannot register mouse_close.\n");
	exit(-1);
    }
    if(keyboard_init() == -1) {
	fprintf(stderr, "Cannot switch keyboard to raw mode.\n");
	exit(-1);
    }
    if(atexit(keyboard_close) == -1) {
	keyboard_close();
	fprintf(stderr, "Cannot register keyboard_close.\n");
	exit(-1);
    }

    setuid(real_uid);

    mouse_setposition(VideoWidth/2 , VideoHeight/2);
    mouse_seteventhandler(MouseEvent);
    mouse_setscale(TheUI.MouseScale);
    old_button = 0;
    mouse_x = VideoWidth/2;
    mouse_y = VideoHeight/2;
    HandleMouseMove(mouse_x, mouse_y);
    keyboard_seteventhandler(KeyboardEvent);
}

/**
**	Change video mode to new width.
*/
global int SetVideoMode(int width)
{
    if (width == 640) return 1;
    return 0;
}

/**
**	Invalidate some area
*/
global void InvalidateArea(int x,int y,int w,int h)
{
}

/**
**	Invalidate whole window
*/
global void Invalidate(void)
{
}

/**
**	Handle interactive input events.
*/
local void MouseEvent(int button, int dx, int dy, int dz, int drx, int dry, int drz) {
    if((old_button == 0) && (button == MOUSE_LEFTBUTTON)) {
//	printf("first down\n");
	HandleButtonDown(1);
    }
    if((old_button == 0) && (button == (MOUSE_LEFTBUTTON + MOUSE_RIGHTBUTTON))) {
//	printf("second down\n");
	HandleButtonDown(2);
    }
    if((old_button == 0) && (button == MOUSE_RIGHTBUTTON)) {
//	printf("third down\n");
	HandleButtonDown(3);
    }
    if((old_button == MOUSE_LEFTBUTTON) && (button == 0)) {
//	printf("first up\n");
	HandleButtonUp(1);
    }
    if((old_button == (MOUSE_LEFTBUTTON + MOUSE_RIGHTBUTTON)) && (button == 0)) {
//	printf("second up\n");
	HandleButtonUp(2);
    }
    if((old_button == MOUSE_RIGHTBUTTON) && (button == 0)) {
//	printf("third up\n");
	HandleButtonUp(3);
    }
    old_button = button;

    if(dx != 0 || dy != 0) {
        if(mouse_x + dx/TheUI.MouseAdjust >= 0
		&& mouse_x + dx/TheUI.MouseAdjust <= VideoWidth)
	    mouse_x += dx/TheUI.MouseAdjust;
	if(mouse_y + dy/TheUI.MouseAdjust >= 0
		&& mouse_y + dy/TheUI.MouseAdjust <= VideoHeight)
	    mouse_y += dy/TheUI.MouseAdjust;
	HandleMouseMove(mouse_x, mouse_y);
	MustRedraw |= RedrawCursor;
    }
}

local void KeyboardEvent(int scancode, int press) {
    int icode;

    IfDebug( icode=0; );		// keeps the compiler happy
    if(press == KEY_EVENTPRESS) {
	switch(scancode) {
	    case SCANCODE_1:
		if(KeyModifiers & ModifierShift) {
		    icode = '!';
		} else {
		    icode = '1';
		}
		break;
	    case SCANCODE_2:
		if(KeyModifiers & ModifierShift) {
		    icode = '@';
		} else {
		    icode = '2';
		}
		break;
	    case SCANCODE_3:
		if(KeyModifiers & ModifierShift) {
		    icode = '#';
		} else {
		    icode = '3';
		}
		break;
	    case SCANCODE_4:
		if(KeyModifiers & ModifierShift) {
		    icode = '$';
		} else {
		    icode = '4';
		}
		break;
	    case SCANCODE_5:
		if(KeyModifiers & ModifierShift) {
		    icode = '%';
		} else {
		    icode = '5';
		}
		break;
	    case SCANCODE_6:
		if(KeyModifiers & ModifierShift) {
		    icode = '^';
		} else {
		    icode = '6';
		}
		break;
	    case SCANCODE_7:
		if(KeyModifiers & ModifierShift) {
		    icode = '&';
		} else {
		    icode = '7';
		}
		break;
	    case SCANCODE_8:
		if(KeyModifiers & ModifierShift) {
		    icode = '*';
		} else {
		    icode = '8';
		}
		break;
	    case SCANCODE_9:
		if(KeyModifiers & ModifierShift) {
		    icode = '(';
		} else {
		    icode = '9';
		}
		break;
	    case SCANCODE_0:
		if(KeyModifiers & ModifierShift) {
		    icode = ')';
		} else {
		    icode = '0';
		}
		break;
	    case SCANCODE_A:
		icode = 'a';
		break;
	    case SCANCODE_B:
		icode = 'b';
		break;
	    case SCANCODE_C:
		icode = 'c';
		break;
	    case SCANCODE_D:
		icode = 'd';
		break;
	    case SCANCODE_E:
		icode = 'e';
		break;
	    case SCANCODE_F:
		icode = 'f';
		break;
	    case SCANCODE_G:
		icode = 'g';
		break;
	    case SCANCODE_H:
		icode = 'h';
		break;
	    case SCANCODE_I:
		icode = 'i';
		break;
	    case SCANCODE_J:
		icode = 'j';
		break;
	    case SCANCODE_K:
		icode = 'k';
		break;
	    case SCANCODE_L:
		icode = 'l';
		break;
	    case SCANCODE_M:
		icode = 'm';
		break;
	    case SCANCODE_N:
		icode = 'n';
		break;
	    case SCANCODE_O:
		icode = 'o';
		break;
	    case SCANCODE_P:
		icode = 'p';
		break;
	    case SCANCODE_Q:
		icode = 'q';
		break;
	    case SCANCODE_R:
		icode = 'r';
		break;
	    case SCANCODE_S:
		icode = 's';
		break;
	    case SCANCODE_T:
		icode = 't';
		break;
	    case SCANCODE_U:
		icode = 'u';
		break;
	    case SCANCODE_V:
		icode = 'v';
		break;
	    case SCANCODE_W:
		icode = 'w';
		break;
	    case SCANCODE_X:
		icode = 'x';
		break;
	    case SCANCODE_Y:
		icode = 'y';
		break;
	    case SCANCODE_Z:
		icode = 'z';
		break;
	    case SCANCODE_SPACE:
		icode = ' ';
		break;
	    case SCANCODE_MINUS:
		if(KeyModifiers & ModifierShift) {
		    icode = '_';
		} else {
		    icode = '-';
		}
		break;
	    case SCANCODE_EQUAL:
		if(KeyModifiers & ModifierShift) {
		    icode = '+';
		} else {
		    icode = '=';
		}
		break;
	    case SCANCODE_ESCAPE:
		icode = '\e';
		break;
	    case SCANCODE_ENTER:
		icode = '\r';
		break;
	    case SCANCODE_BACKSPACE:
		icode = '\b';
		break;
	    case SCANCODE_TAB:
		icode = '\t';
		break;
	    case SCANCODE_CURSORBLOCKUP:
	    case SCANCODE_CURSORUP:
		icode = KeyCodeUp;
		break;
	    case SCANCODE_CURSORBLOCKDOWN:
	    case SCANCODE_CURSORDOWN:
		icode = KeyCodeDown;
		break;
	    case SCANCODE_CURSORBLOCKLEFT:
	    case SCANCODE_CURSORLEFT:
		icode = KeyCodeLeft;
		break;
	    case SCANCODE_CURSORBLOCKRIGHT:
	    case SCANCODE_CURSORRIGHT:
		icode = KeyCodeRight;
		break;
	    // KeyCodePause
	    case SCANCODE_LEFTSHIFT:
	    case SCANCODE_RIGHTSHIFT:
		KeyModifiers |= ModifierShift;
		break;
	    case SCANCODE_LEFTCONTROL:
	    case SCANCODE_RIGHTCONTROL:
		KeyModifiers |= ModifierControl;
		break;
	    case SCANCODE_LEFTALT:
	    case SCANCODE_RIGHTALT:
		KeyModifiers |= ModifierAlt;
	    // Super, Hyper
	}
	if(KeyModifiers&ModifierShift){
	    if(icode <= 'z' && icode >= 'a')
		icode -= 32;
	}
	if(HandleKeyDown(icode)) {
	    return;
	}
	DoBottomPanelKey(icode);
    } else if(press == KEY_EVENTRELEASE) {
	switch(scancode) {
	    case SCANCODE_LEFTSHIFT:
	    case SCANCODE_RIGHTSHIFT:
		KeyModifiers &= ~ModifierShift;
		break;
	    case SCANCODE_LEFTCONTROL:
	    case SCANCODE_RIGHTCONTROL:
		KeyModifiers &= ~ModifierControl;
		break;
	    case SCANCODE_LEFTALT:
	    case SCANCODE_RIGHTALT:
		KeyModifiers &= ~ModifierAlt;
	    // Super ???
	}
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
    struct timeval tv;
    fd_set rfds;
    fd_set wfds;
    int ret;

    for(;;) {
	//
	//	Prepare select
	//
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);

	//
	//	Network
	//
	if(NetworkFildes != -1) {
	    FD_SET(NetworkFildes, &rfds);
	}

	//
	//	Sound
	//
	if(!SoundOff && !SoundThreadRunning) {
	    FD_SET(SoundFildes, &wfds);
	}

	ret = vga_waitevent(VGA_MOUSEEVENT | VGA_KEYEVENT, &rfds, &wfds, NULL, &tv);

	if(ret >= 0) {
	    //
	    //	Network
	    //
	    if(NetworkFildes != -1 && FD_ISSET(NetworkFildes, &rfds)) {
		NetworkEvent();
	    }

	    //
	    //	Network in sync and time for frame over: return
	    //
	    if(NetworkInSync && VideoInterrupts) {
		return;
	    }

	    //
	    //	Sound
	    //
	    if(!SoundOff && !SoundThreadRunning
			&& FD_ISSET(SoundFildes, &wfds)) {
		WriteSound();
	    }
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
	switch( VideoDepth ) {
	    case 15:
		Pixels[i] = ((palette[i].r >> 1) << 10)
			+ ((palette[i].g >>1) << 5)
			+ (palette[i].b >> 1);
	    case 16:
		Pixels[i] = ((palette[i].r >> 1) << 11)
			+ (palette[i].g << 5)
			+ (palette[i].b >> 1);
		break;
	    default:
		DebugLevel0(__FUNCTION__": Depth not written\n");
	}
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
    int i;

    vga_waitretrace();
    for(i = 0; i < 480; i++) {
	vga_drawscansegment((void*)&VideoMemory[i * 640], 0, i, 1280);
    }
}

#endif	// USE_SVGALIB

//@}
