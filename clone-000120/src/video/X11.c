/**
**	A clone of a famous game.
*/
/**@name X11.c		-	XWindows support. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer and Valery Shchedrin
**
**	$Id: X11.c,v 1.32 2000/01/16 23:38:15 root Exp $
*/

//@{

#ifdef USE_X11

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

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
#include "new_video.h"

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

local Display* TheDisplay;
local int TheScreen;
local Pixmap TheMainDrawable;
local Window TheMainWindow;
local GC GcLine;

    /// Atom for WM_DELETE_WINDOW
local Atom WmDeleteWindowAtom;


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

/**
**	Watch opening/closing of X11 connections
*/
local void MyConnectionWatch
	(Display* display,XPointer client,int fd,Bool flag,XPointer* data)
{
    DebugLevel0(__FUNCTION__": fildes %d flag %d\n",fd,flag);
    if( flag ) {			// file handle opened
    } else {				// file handle closed
    }
}

/**
**	X11 initialize.
*/
global void CloneInitDisplay(void)
{
    int i;
    Window window;
    XGCValues gcvalue;
    XSizeHints hints;
    XWMHints wmhints;
    XClassHint classhint;
    XSetWindowAttributes attributes;
    int shm_major,shm_minor;
    Bool pixmap_support;
    XShmSegmentInfo shminfo;
    XVisualInfo xvi;
    XPixmapFormatValues *xpfv;

    if( !(TheDisplay=XOpenDisplay(NULL)) ) {
	fprintf(stderr,"Cannot connect to X-Server.\n");
	exit(-1);
    }

    TheScreen=DefaultScreen(TheDisplay);

    //	I need shared memory pixmap extension.

    if( !XShmQueryVersion(TheDisplay,&shm_major,&shm_minor,&pixmap_support) ) {
	fprintf(stderr,"SHM-Extensions required.\n");
	exit(-1);
    }
    if( !pixmap_support ) {
	fprintf(stderr,"SHM-Extensions with pixmap supported required.\n");
	exit(-1);
    }

    //  Look for a nice visual
#ifdef USE_HICOLOR
    if(XMatchVisualInfo(TheDisplay, TheScreen, 16, TrueColor, &xvi))
	goto foundvisual;
    if(XMatchVisualInfo(TheDisplay, TheScreen, 15, TrueColor, &xvi))
	goto foundvisual;
    fprintf(stderr,"Sorry, this version is for 15/16-bit displays only.\n");
#else
    if(XMatchVisualInfo(TheDisplay, TheScreen, 8, PseudoColor, &xvi))
	goto foundvisual;
    fprintf(stderr,"Sorry, this version is for 8-bit displays only.\n");
#endif
#if 0
    if(XMatchVisualInfo(TheDisplay, TheScreen, 24, TrueColor, &xvi))
	goto foundvisual;
#endif
    exit(-1);
foundvisual:

    xpfv=XListPixmapFormats(TheDisplay, &i);
    for(i--;i>=0;i--)  if(xpfv[i].depth==xvi.depth) break;
    if(i<0)  {
	fprintf(stderr,"No Pixmap format for visual depth?\n");
	exit(-1);
    }
    VideoDepth=xvi.depth;

    VideoWidth = 640;
    VideoHeight = 480;
    MapWidth = DEFAULT_MAP_WIDTH;		// FIXME: Not the correct way
    MapHeight = DEFAULT_MAP_HEIGHT;
    shminfo.shmid=shmget(IPC_PRIVATE,
	(VideoWidth*xpfv[i].bits_per_pixel+xpfv[i].scanline_pad-1) /
	xpfv[i].scanline_pad * xpfv[i].scanline_pad * VideoHeight / 8,
	IPC_CREAT|0777);

    XFree(xpfv);

    if( !shminfo.shmid==-1 ) {
	fprintf(stderr,"shmget failed.\n");
	exit(-1);
    }
    VideoMemory=(VMemType*)shminfo.shmaddr=shmat(shminfo.shmid,0,0);
    if( shminfo.shmaddr==(void*)-1 ) {
	shmctl(shminfo.shmid,IPC_RMID,0);
	fprintf(stderr,"shmat failed.\n");
	exit(-1);
    }
    shminfo.readOnly=False;

    if( !XShmAttach(TheDisplay,&shminfo) ) {
	shmctl(shminfo.shmid,IPC_RMID,0);
	fprintf(stderr,"XShmAttach failed.\n");
	exit(-1);
    }
    // Mark segment as deleted as soon as both clone and the X server have
    // attached to it.  The POSIX spec says that a segment marked as deleted
    // can no longer have addition processes attach to it, but Linux will let
    // them anyway.
    shmctl(shminfo.shmid,IPC_RMID,0);

    TheMainDrawable=attributes.background_pixmap=
	    XShmCreatePixmap(TheDisplay,DefaultRootWindow(TheDisplay)
		,shminfo.shmaddr,&shminfo
		,VideoWidth,VideoHeight
		,xvi.depth);
    attributes.cursor = XCreateFontCursor(TheDisplay,XC_tcross-1);
    attributes.backing_store = NotUseful;
    attributes.save_under = False;
    attributes.event_mask = KeyPressMask|KeyReleaseMask|/*ExposureMask|*/
	FocusChangeMask|ButtonPressMask|PointerMotionMask|ButtonReleaseMask;
    i = CWBackPixmap|CWBackingStore|CWSaveUnder|CWEventMask|CWCursor;

    if(xvi.class==PseudoColor)  {
	i|=CWColormap;
	attributes.colormap =
	    XCreateColormap( TheDisplay, xvi.screen, xvi.visual, AllocNone);
	// FIXME:  Really should fill in the colormap right now
    }
    window=XCreateWindow(TheDisplay,DefaultRootWindow(TheDisplay)
	    ,0,0,VideoWidth,VideoHeight,3
	    ,xvi.depth,InputOutput,xvi.visual ,i,&attributes);
    TheMainWindow=window;

    gcvalue.graphics_exposures=False;
    GcLine=XCreateGC(TheDisplay,window,GCGraphicsExposures,&gcvalue);

    //
    //	Clear initial window.
    //
    XSetForeground(TheDisplay,GcLine,BlackPixel(TheDisplay,TheScreen));
    XFillRectangle(TheDisplay,TheMainDrawable,GcLine,0,0
	    ,VideoWidth,VideoHeight);

    WmDeleteWindowAtom=XInternAtom(TheDisplay,"WM_DELETE_WINDOW",False);

    //
    //	Set some usefull min/max sizes as well as a 1.3 aspect
    //
#if 0
    if( geometry ) {
	hints.flags=0;
	f=XParseGeometry(geometry
		,&hints.x,&hints.y,&hints.width,&hints.height);

	if( f&XValue ) {
	    if( f&XNegative ) {
		hints.x+=DisplayWidth-hints.width;
	    }
	    hints.flags|=USPosition;
	    // FIXME: win gravity
	}
	if( f&YValue ) {
	    if( f&YNegative ) {
		hints.y+=DisplayHeight-hints.height;
	    }
	    hints.flags|=USPosition;
	    // FIXME: win gravity
	}
	if( f&WidthValue ) {
	    hints.flags|=USSize;
	}
	if( f&HeightValue ) {
	    hints.flags|=USSize;
	}
    } else {
#endif
	hints.width=VideoWidth;
	hints.height=VideoHeight;
	hints.flags=PSize;
#if 0
    }
#endif
    hints.min_width=VideoWidth;
    hints.min_height=VideoHeight;
    hints.max_width=VideoWidth;
    hints.max_height=VideoHeight;
    hints.min_aspect.x=4;
    hints.min_aspect.y=3;

    hints.max_aspect.x=4;
    hints.max_aspect.y=3;
    hints.width_inc=4;
    hints.height_inc=3;

    hints.flags|=PMinSize|PMaxSize|PAspect|PResizeInc;

    wmhints.input=True;
    wmhints.initial_state=NormalState;
    wmhints.window_group=window;
    wmhints.flags=InputHint|StateHint|WindowGroupHint;

    classhint.res_name="aleclone";
    classhint.res_class="AleClone";

    XSetStandardProperties(TheDisplay,window
	,"ALE Clone","ALE Clone",None,(char**)0,0,&hints);
    XSetClassHint(TheDisplay,window,&classhint);
    XSetWMHints(TheDisplay,window,&wmhints);

    XSetWMProtocols(TheDisplay,window,&WmDeleteWindowAtom,1);

    XMapWindow(TheDisplay,window);

    //
    //	Input handling.
    //
    XAddConnectionWatch(TheDisplay,MyConnectionWatch,NULL);
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
	XClearArea(TheDisplay,TheMainWindow,x,y,w,h,False);
    }
}

/**
**	Invalidate whole window
*/
global void Invalidate(void)
{
    XClearWindow(TheDisplay,TheMainWindow);
}

/**
**      Handle keyboard modifiers
*/
local void X11HandleModifiers(XKeyEvent* keyevent)
{
    int mod=keyevent->state;

    // Here we use an ideous hack to avoid X keysyms mapping.
    // What we need is to know that the player hit key 'x' with
    // the control modifier; we don't care if he typed `key mapped
    // on Ctrl-x` with control modifier...
    // Note that we don't use this hack for "shift", because shifted
    // keys can be useful (to get numbers on my french keybord
    // for exemple :)).
    KeyModifiers=0;
    if( mod&ShiftMask ) {
        KeyModifiers|=ModifierShift;
    }
    if( mod&ControlMask ) {
        KeyModifiers|=ModifierControl;
        keyevent->state&=~ControlMask;  // Hack Attack!
    }
    if( mod&Mod1Mask ) {
        KeyModifiers|=ModifierAlt;
        keyevent->state&=~Mod1Mask;     // Hack Attack!
    }
}

/**
**	Handle keyboard!
*/
local void X11HandleKey(KeySym code)
{
    int icode;

    /*
    **	Convert X11 keycodes into internal keycodes.
    */
    switch( (icode=code) ) {
	case XK_Escape:
	    icode='\e';
	    break;
	case XK_Return:
	    icode='\r';
	    break;
	case XK_BackSpace:
	    icode='\b';
	    break;
	case XK_Tab:
	    icode='\t';
	    break;
	case XK_Up:
	    icode=KeyCodeUp;
	    break;
	case XK_Down:
	    icode=KeyCodeDown;
	    break;
	case XK_Left:
	    icode=KeyCodeLeft;
	    break;
	case XK_Right:
	    icode=KeyCodeRight;
	    break;
	case XK_Pause:
	    icode=KeyCodePause;
	    break;
	case XK_F1:
	    icode=KeyCodeF1;
	    break;
	case XK_F2:
	    icode=KeyCodeF2;
	    break;
	case XK_F3:
	    icode=KeyCodeF3;
	    break;
	case XK_F4:
	    icode=KeyCodeF4;
	    break;
	case XK_F5:
	    icode=KeyCodeF5;
	    break;
	case XK_F6:
	    icode=KeyCodeF6;
	    break;
	case XK_F7:
	    icode=KeyCodeF7;
	    break;
	case XK_F8:
	    icode=KeyCodeF8;
	    break;
	case XK_F9:
	    icode=KeyCodeF9;
	    break;
	case XK_F10:
	    icode=KeyCodeF10;
	    break;
	case XK_F11:
	    icode=KeyCodeF11;
	    break;
	case XK_F12:
	    icode=KeyCodeF12;
	    break;
        // We need these because if you only hit a modifier key,
        // X doesn't set its state (modifiers) field in the keyevent.
	case XK_Shift_L:
	case XK_Shift_R:
	    KeyModifiers|=ModifierShift;
	    break;
	case XK_Control_L:
	case XK_Control_R:
	    KeyModifiers|=ModifierControl;
	    break;
	case XK_Alt_L:
	case XK_Alt_R:
	case XK_Meta_L:
	case XK_Meta_R:
	    KeyModifiers|=ModifierAlt;
	    break;
	case XK_Super_L:
	case XK_Super_R:
	    KeyModifiers|=ModifierSuper;
	    break;
	case XK_Hyper_L:
	case XK_Hyper_R:
	    KeyModifiers|=ModifierHyper;
	    break;
	default:
	    break;
    }
    if( HandleKeyDown(icode) ) {
	return;
    }
    DoBottomPanelKey(icode);
}

/**
**	Handle keyboard! (release)
*/
local void X11HandleKeyUp(KeySym code)
{
    switch( code ) {
	case XK_Shift_L:
	case XK_Shift_R:
	    KeyModifiers&=~ModifierShift;
	    break;
	case XK_Control_L:
	case XK_Control_R:
	    KeyModifiers&=~ModifierControl;
	    break;
	case XK_Alt_L:
	case XK_Alt_R:
	case XK_Meta_L:
	case XK_Meta_R:
	    KeyModifiers&=~ModifierAlt;
	    break;
	case XK_Super_L:
	case XK_Super_R:
	    KeyModifiers&=~ModifierHyper;
	    break;

	default:
	    DebugLevel3("\tUnknown key %x\n",code);
	    break;
    }
}

#if 0
/**
**	Check for pending events.
*/
global int CheckEvent(void)
{
    return XPending(TheDisplay);
}
#endif

/**
**	Handle interactive input event.
*/
local void DoEvent(void)
{
    XEvent event;
    int xw, yw;

    XNextEvent(TheDisplay,&event);

    switch( event.type ) {
	case ButtonPress:
	    DebugLevel3("\tbutton press %d\n",event.xbutton.button);
	    HandleButtonDown(event.xbutton.button);
	    break;

	case ButtonRelease:
	    DebugLevel3("\tbutton release %d\n",event.xbutton.button);
	    HandleButtonUp(event.xbutton.button);
	    break;

	case Expose:
	    DebugLevel1("\texpose\n");
	    MustRedraw=-1;
	    break;

	case MotionNotify:
	    DebugLevel3("\tmotion notify %d,%d\n"
		,event.xbutton.x,event.xbutton.y);
	    HandleMouseMove(event.xbutton.x,event.xbutton.y);
	    if (TheUI.WarpX != -1 || TheUI.WarpY != -1) {
		xw = TheUI.WarpX;
		yw = TheUI.WarpY;
		TheUI.WarpX = -1;
		TheUI.WarpY = -1;
		XWarpPointer(TheDisplay,TheMainWindow,TheMainWindow,
				0,0,0,0,xw,yw);
	    }
	    MustRedraw|=RedrawCursor;
	    break;

	case FocusIn:
	    DebugLevel3("\tfocus in\n");
	    break;

	case FocusOut:
	    DebugLevel3("\tfocus out\n");
	    CursorOn=-1;
	    break;

	case ClientMessage:
	    DebugLevel3("\tclient message\n");
            if (event.xclient.format == 32) {
                if ((Atom)event.xclient.data.l[0] == WmDeleteWindowAtom) {
		    Exit(0);
		}
	    }
	    break;

	case KeyPress:
	    DebugLevel3("\tKey press\n");
{
	    char buf[128];
	    int num;
	    KeySym keysym;

            X11HandleModifiers((XKeyEvent*)&event);
	    num=XLookupString((XKeyEvent*)&event,buf,sizeof(buf),&keysym,0);
	    DebugLevel3("\tKey %lx `%s'\n",keysym,buf);
	    if( num==1 ) {
		X11HandleKey(*buf);
	    } else {
		X11HandleKey(keysym);
	    }
}
	    break;

	case KeyRelease:
	    DebugLevel3("\tKey release\n");
	    X11HandleKeyUp(XLookupKeysym((XKeyEvent*)&event,0));
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
    struct timeval tv;
    fd_set rfds;
    fd_set wfds;
    int maxfd;
    int* xfd;
    int n;
    int i;
    int morex;
    int connection;

    connection=ConnectionNumber(TheDisplay);

    for( ;; ) {
	//
	//	Prepare select
	//
	tv.tv_sec=0;
	tv.tv_usec=0;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	maxfd=0;

	//
	//	X11 how many events already in que
	//
	xfd=NULL;
	morex=XQLength(TheDisplay);
	if( !morex ) {
	    //
	    //	X11 connections number
	    //
	    maxfd=connection;
	    FD_SET(connection,&rfds);

	    //
	    //	Get all X11 internal connections
	    //
	    if( !XInternalConnectionNumbers(TheDisplay,&xfd,&n) ) {
		DebugLevel0(__FUNCTION__": out of memory\n");
		abort();
	    }
	    for( i=n; i--; ) {
		FD_SET(xfd[i],&rfds);
		if( xfd[i]>maxfd ) {
		    maxfd=xfd[i];
		}
	    }
	}

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

	maxfd=select(maxfd+1,&rfds,&wfds,NULL,morex ? &tv : NULL);

	//
	//	X11
	//
	if( maxfd>0 ) {
	    if( !morex ) {
		for( i=n; i--; ) {
		    if( FD_ISSET(xfd[i],&rfds) ) {
			XProcessInternalConnection(TheDisplay,xfd[i]);
		    }
		}
		XFree(xfd);
		if( FD_ISSET(connection,&rfds) ) {
		    XEventsQueued(TheDisplay,QueuedAfterReading);
		}
		morex=XQLength(TheDisplay);
	    }
	}

	if( morex ) {			// handle new + *OLD* x11 events
	    DoEvent();
	}

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
	    if( !morex && NetworkInSync && VideoInterrupts ) {
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

	//
	//	Network in sync and time for frame over: return
	//
	if( !morex && NetworkInSync && VideoInterrupts ) {
	    return;
	}
    }
}

/**
**	Create palette.
*/
global void VideoCreatePalette(struct Palette* palette)
{
    XColor color;
    XWindowAttributes xwa;
    int i;

    //
    //	Get some colors:
    //
    XGetWindowAttributes(TheDisplay,TheMainWindow,&xwa);
    for( i=0; i<256; ++i ) {
	color.red=palette[i].r*65535/63;
	color.green=palette[i].g*65535/63;
	color.blue=palette[i].b*65535/63;
	color.flags=DoRed|DoGreen|DoBlue;
	if( !XAllocColor(TheDisplay,xwa.colormap,&color) ) {
	    fprintf(stderr,"Cannot allocate color\n");
	    exit(-1);
	}
	Pixels[i]=color.pixel;
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
    //Palette palette[256];
    OldGraphic title;
    extern unsigned char PalettePNG[];

    LoadOldGraphic(name,&title);
    //GetCurrentImagePalette(palette);
    //SetPalette(palette);
    //DrawOldGraphic(title,0,0,VideoWidth,VideoHeight,0,0);
    //FreeOldGraphic(title);
    memcpy(GlobalPalette,PalettePNG,sizeof(GlobalPalette));
    VideoCreatePalette(GlobalPalette);
    // FIXME: bigger window ?
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
	    XClearArea(TheDisplay,TheMainWindow
		,MAP_X+10,MAP_X+10,13*13,13
		,False);
	);
        ++SlowFrameCounter;
#if 0
    } else {
        struct itimerval itv;

        getitimer(ITIMER_REAL,&itv);
        DebugLevel1("Remain %d\n",itv.it_value.tv_usec);
#endif
    }
}

/**
**	Realize video memory.
*/
global void RealizeVideoMemory(void)
{
    // in X11 it does flushing the output que
    XFlush(TheDisplay);
}

#endif	// USE_X11

//@}
