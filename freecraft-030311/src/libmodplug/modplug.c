/*
 * This source code is public domain.
 *
 * Authors: Kenton Varda <temporal@gauge3d.org> (C interface wrapper)
 *
 * $Id: modplug.c,v 1.1 2002/03/20 22:39:06 johns Exp $
 */

/**@name modplug.c		-	libmodplug external API file. */

#include "modplug.h"
#include "stdafx.h"
#include "sndfile.h"

/**
**	Modplug file, holds all data for a file playing with libmodplug.
*/
struct _ModPlugFile
{
    CSoundFile mSoundFile;
};

/**
**	 Default global lib modplug settings.
*/
static ModPlug_Settings ModPlugSettings = {
    MODPLUG_ENABLE_OVERSAMPLING | MODPLUG_ENABLE_NOISE_REDUCTION,

    2,
    16,
    44100,
    MODPLUG_RESAMPLE_LINEAR,

    0,
    0,
    0,
    0,
    0,
    0,
    0
};

/**
**	Current sample size.
*/
static int ModPlugSampleSize;

/**
**	Update setting changes to next (CSoundFile) layer.
*/
static void ModPlugUpdateSettings(CSoundFile* file, int updateBasicConfig)
{
    if (ModPlugSettings.mFlags & MODPLUG_ENABLE_REVERB) {
	CSoundFile_SetReverbParameters(file, ModPlugSettings.mReverbDepth,
	    ModPlugSettings.mReverbDelay);
    }

    if (ModPlugSettings.mFlags & MODPLUG_ENABLE_MEGABASS) {
	CSoundFile_SetXBassParameters(file, ModPlugSettings.mBassAmount,
	    ModPlugSettings.mBassRange);
    } else {
	// modplug seems to ignore the SetWaveConfigEx() setting for bass boost
	CSoundFile_SetXBassParameters(file, 0, 0);
    }

    if (ModPlugSettings.mFlags & MODPLUG_ENABLE_SURROUND) {
	CSoundFile_SetSurroundParameters(file, ModPlugSettings.mSurroundDepth,
	    ModPlugSettings.mSurroundDelay);
    }

    if (updateBasicConfig) {
	CSoundFile_SetWaveConfig(file, ModPlugSettings.mFrequency,
	    ModPlugSettings.mBits, ModPlugSettings.mChannels, 0);

	ModPlugSampleSize =
	    ModPlugSettings.mBits / 8 * ModPlugSettings.mChannels;
    }

    CSoundFile_SetWaveConfigEx(file,
	ModPlugSettings.mFlags & MODPLUG_ENABLE_SURROUND,
	!(ModPlugSettings.mFlags & MODPLUG_ENABLE_OVERSAMPLING),
	ModPlugSettings.mFlags & MODPLUG_ENABLE_REVERB, 1,
	ModPlugSettings.mFlags & MODPLUG_ENABLE_MEGABASS,
	ModPlugSettings.mFlags & MODPLUG_ENABLE_NOISE_REDUCTION, 0);
    CSoundFile_SetResamplingMode(file, ModPlugSettings.mResamplingMode);
}

ModPlugFile* ModPlug_Load(const void* data, int size)
{
    ModPlugFile* result;

    result = (ModPlugFile*) malloc(sizeof(ModPlugFile));
    ModPlugUpdateSettings(&result->mSoundFile, 1);
    if (CSoundFile_Create(&result->mSoundFile, data, size)) {
	CSoundFile_SetRepeatCount(&result->mSoundFile,
	    ModPlugSettings.mLoopCount);
	return result;
    } else {
	free(result);
	return NULL;
    }
}

void ModPlug_Unload(ModPlugFile* file)
{
    CSoundFile_Destroy(&file->mSoundFile);
    free(file);
}

int ModPlug_Read(ModPlugFile* file, void* buffer, int size)
{
    return CSoundFile_Read(&file->mSoundFile, buffer,
	size) * ModPlugSampleSize;
}

const char* ModPlug_GetName(ModPlugFile* file)
{
    return CSoundFile_GetTitle(&file->mSoundFile);
}

int ModPlug_GetLength(ModPlugFile* file)
{
    return CSoundFile_GetSongTime(&file->mSoundFile) * 1000;
}

void ModPlug_Seek(ModPlugFile* file, int millisecond)
{
    int maxpos;
    int maxtime;
    float postime;

    maxtime = CSoundFile_GetSongTime(&file->mSoundFile) * 1000;
    if (millisecond > maxtime) {
	millisecond = maxtime;
    }
    maxpos = CSoundFile_GetMaxPosition(&file->mSoundFile);
    postime = (float)maxpos / (float)maxtime;

    CSoundFile_SetCurrentPos(&file->mSoundFile, (int)(millisecond * postime));
}

void ModPlug_GetSettings(ModPlug_Settings* settings)
{
    *settings=ModPlugSettings;
}

void ModPlug_SetSettings(const ModPlug_Settings* settings)
{
    ModPlugSettings=*settings;
    ModPlugUpdateSettings(NULL, 0);
}
