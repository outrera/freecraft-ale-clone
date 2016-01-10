/*
 * This source code is public domain.
 *
 * Authors: Kenton Varda <temporal@gauge3d.org> (C interface wrapper)
 *
 *	$Id: modplug.h,v 1.4 2002/03/20 22:39:06 johns Exp $
 */

/**@name modplug.h		-	libmodplug external header file. */

#ifndef MODPLUG_H__INCLUDED
#define MODPLUG_H__INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

    /**
    **	Holds the private data of a music file of ModPlug.
    */
typedef struct _ModPlugFile ModPlugFile;

    /**
    **	Load a mod (any by modplug supported) file.
    **
    **	@param data	Should point to a block of memory containing the
    **			complete file.
    **	@param size	Should be the size of that block.
    **
    **	@return		The loaded mod file on success, or NULL on failure.
    */
extern ModPlugFile* ModPlug_Load(const void* data, int size);

    /**
    **	Unload a mod file.
    **
    **	@param file	Modplug file, obtained by ::ModPlug_Load().
    */
extern void ModPlug_Unload(ModPlugFile* file);

    /**
    **	Read sample data into the buffer.
    **
    **	Returns the number of bytes read.  If the end of the mod has been
    **	reached, zero is returned.
    **
    **	@param file	Modplug file, contains the working data.
    **	@param buffer	Buffer filled up with generated samples.
    **	@param size	Number of bytes requested.
    **
    **	@return		Number of bytes processed, zero = EOF.
    */
extern int  ModPlug_Read(ModPlugFile* file, void* buffer, int size);

    /**
    **	Get the name of the mod.
    **
    **	The returned buffer is stored within the ModPlugFile structure and will
    **	remain valid until you unload the file.
    **
    **	@param file	Modplug file, contains the working data.
    **
    **	@return		Name of the file (static).
    */
extern const char* ModPlug_GetName(ModPlugFile* file);
    /**
    **	Get the length of the mod, in milliseconds.
    **
    **	@note	that this result is not always accurate, especially in the case
    **		of mods with loops.
    **
    **	@param file	Modplug file, contains the working data.
    **
    **	@return		Lenght in milliseconds.
    */
extern int ModPlug_GetLength(ModPlugFile* file);

    /**
    **	Seek to a particular position in the song.
    **
    **	@note	that seeking and MODs don't mix very well.  Some mods will be
    **		missing instruments for a short time after a seek, as ModPlug
    **		does not scan the sequence backwards to find out which
    **		instruments were supposed to be playing at that time.  (Doing
    **		so would be difficult and not very reliable.)	Also, note
    **		that seeking is not very exact in some mods -- especially those
    **		for which ModPlug_GetLength() does not report the full length.
    **
    **	@param file		Modplug file, contains the working data.
    **	@param millisecond	Position in milliseconds.
    */
extern void ModPlug_Seek(ModPlugFile* file, int millisecond);

    /**
    **	Possible flags.
    */
enum _ModPlug_Flags
{
    MODPLUG_ENABLE_OVERSAMPLING	    = 1 << 0,
	/// Enable oversampling (*highly* recommended)
    MODPLUG_ENABLE_NOISE_REDUCTION  = 1 << 1,
	/// Enable noise reduction
    MODPLUG_ENABLE_REVERB	    = 1 << 2,
	/// Enable reverb
    MODPLUG_ENABLE_MEGABASS	    = 1 << 3,
	/// Enable megabass
    MODPLUG_ENABLE_SURROUND	    = 1 << 4
	/// Enable surround sound
};

    /**
    **	Possible resampling modes.
    */
enum _ModPlug_ResamplingMode
{
    MODPLUG_RESAMPLE_NEAREST = 0,
	/// No interpolation (very fast, extremely bad sound quality)
    MODPLUG_RESAMPLE_LINEAR  = 1,
	/// Linear interpolation (fast, good quality)
    MODPLUG_RESAMPLE_SPLINE  = 2,
	/// Cubic spline interpolation (high quality)
    MODPLUG_RESAMPLE_FIR     = 3
	/// 8-tap fir filter (extremely high quality)
};

    /**
    **	Modplug configuration.
    **
    **	@note	that ModPlug always decodes sound at 44100kHz, 32 bit, stereo
    **		and then down-mixes to the settings you choose.
    */
typedef struct _ModPlug_Settings
{
    int mFlags;		/// One or more of the MODPLUG_ENABLE_* flags above, bitwise-OR'ed

    int mChannels;	/// Number of channels - 1 for mono or 2 for stereo
    int mBits;		/// Bits per sample - 8, 16, or 32
    int mFrequency;	/// Sampling rate - 11025, 22050, or 44100
    int mResamplingMode;/// One of MODPLUG_RESAMPLE_*, above

    int mReverbDepth;	/// Reverb level 0(quiet)-100(loud)
    int mReverbDelay;	/// Reverb delay in ms, usually 40-200ms
    int mBassAmount;	/// XBass level 0(quiet)-100(loud)
    int mBassRange;	/// XBass cutoff in Hz 10-100
    int mSurroundDepth;	/// Surround level 0(quiet)-100(heavy)
    int mSurroundDelay;	/// Surround delay in ms, usually 5-40ms
    int mLoopCount;	/// Number of times to loop.  Zero prevents looping.  -1 loops forever.
} ModPlug_Settings;

    /**
    **	Get the mod decoder settings.
    **
    **	@param settings	Filled with the current settings.
    */
extern void ModPlug_GetSettings(ModPlug_Settings* settings);

    /**
    **	Get and set the mod decoder settings.
    **
    **	All options, except for channels, bits-per-sample, sampling rate,
    **	and loop count, will take effect immediately.
    **	Those options which don't take effect immediately will take effect the
    **	next time you load a mod.
    **
    **	@param settings	Settings, which replace the current settings.
    */
extern void ModPlug_SetSettings(const ModPlug_Settings* settings);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
