/*
**	A clone of a famous game.
*/
/**@name video.h	-	The video headerfile. */
/*
**	(c) Copyright 1999 by Lutz Sammer
**
**	$Id: new_video.h,v 1.5 1999/12/10 01:33:46 root Exp $
*/

#ifndef __NEW_VIDEO_H__
#define __NEW_VIDEO_H__

//@{

// Little note: this should become the new video headerfile

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

#if 0
Note:	This new graphic object should generalize all the different objects
	currently used in ALE Clone, Graphic Image, Sprite, RLESprite.
	Also should generalize the handling of different hardwares.
	(8bit,16bit,...)
#endif

/**
**	General graphic object typedef. (forward)
*/
typedef struct __graphic__ Graphic;

/**
**	General graphic object type.
*/
typedef struct __graphic_type__ {
	/**
	**	Draw the object.
	**
	**	@param o	pointer to object
	**	@param f	number of frame (object index)
	**	@param x	x coordinate on the screen
	**	@param y	y coordinate on the screen
	*/
    void (*Draw)	(Graphic* o,unsigned f,unsigned x,unsigned y);
    void (*DrawX)	(Graphic* o,unsigned f,unsigned x,unsigned y);
    void (*DrawClip)	(Graphic* o,unsigned f,unsigned x,unsigned y);
    void (*DrawClipX)	(Graphic* o,unsigned f,unsigned x,unsigned y);
	/**
	**	Draw part of the object.
	**
	**	@param o	pointer to object
	**	@param gx	X offset into object
	**	@param gy	Y offset into object
	**	@param w	width to display
	**	@param h	height to display
	**	@param x	x coordinate on the screen
	**	@param y	y coordinate on the screen
	*/
    void (*DrawSub)	(Graphic* o,unsigned gx,unsigned gy
			,unsigned w,unsigned h,unsigned x,unsigned y);
    void (*DrawSubX)	(Graphic* o,unsigned gx,unsigned gy
			,unsigned w,unsigned h,unsigned x,unsigned y);
    void (*DrawSubClip)	(Graphic* o,unsigned gx,unsigned gy
			,unsigned w,unsigned h,unsigned x,unsigned y);
    void (*DrawSubClipX)(Graphic* o,unsigned gx,unsigned gy
			,unsigned w,unsigned h,unsigned x,unsigned y);

    void (*Free)	(Graphic* o);
} GraphicType;

/**
**	General graphic object
*/
struct __graphic__ {
    GraphicType*	Type;		/// object type dependend
    unsigned		Width;		/// Width of the object
    unsigned		Height;		/// Height of the object
    unsigned		NumFrames;	/// Number of frames
    void*		Frames;		/// Frames of the object
};

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	Wanted videomode, fullscreen or windowed.
*/
extern int VideoFullScreen;

/*----------------------------------------------------------------------------
--	Macros
----------------------------------------------------------------------------*/

/**
**	Draw a graphic object.
*/
#define VideoDraw(o,f,x,y) \
    ((o)->Type->Draw)(o,(f),(x),(y))

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern void InitVideo(void);		/// initialize the video part

    ///	Load graphic from PNG file.
extern Graphic* LoadGraphicPNG(const char* name);

    /// Make graphic
extern Graphic* MakeGraphic(unsigned d,unsigned w,unsigned h,void* p);

    /// Load graphic
extern Graphic* LoadGraphic(const char* file);

    /// Init graphic
extern void InitGraphic(void);

//@}

#endif	// !__NEW_VIDEO_H__
