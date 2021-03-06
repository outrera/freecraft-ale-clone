/*
**	A clone of a famous game.
*/
/**@name sound_id.h	-	Sound identifier client side header file. */
/*
**	(c) Copyright 1999 by Lutz Sammer and Fabrice Rossi
**
**	$Id: sound_id.h,v 1.4 1999/06/19 22:21:54 root Exp $
*/

#ifndef __SOUND_ID_H__
#define __SOUND_ID_H__

//@{

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

/**
**	Sound referencing.
**
**	Client side representation of the sound id.
**	Passed to the sound server API.
*/
typedef void* SoundId;			/// sound identifier

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/** Register a sound group (or an unique sound if nb==1) and get the
    corresponding sound id.
    @param sound_name name of this sound group. MUST BE A PERMAMNENT STRING.
    @param file list of sound file names
    @param nb number of sounds
    @return the sound id of the created group
*/
extern SoundId MakeSound(char* sound_name,char* file[],unsigned char nb);

/** Get a sound id for a given sound name. Returns NULL when no sound is
    found.
    @param sound_name name of the sound. MUST BE A PERMANENT STRING.
    @return the corresponding sound id
*/
extern SoundId SoundIdForName(char* sound_name);

/** Create a new mapping between a name and an already valid sound id
    @param sound_name the name. MUST BE A PERMANENT STRING.
    @param id the sound id.
 */
extern void MapSound(char* sound_name,SoundId id);

/** Register two sound groups together to make a special sound (for
    selection). Return the corresponding id after registering it under a given
    name. 
    @param group_name the name of the group. MUST BE A PERMANENT STRING.
    @param first id of the first group.
    @param second id of the second group.
    @return id of the created group.
*/
extern SoundId MakeSoundGroup(char* group_name,SoundId first,SoundId second);

/** Helper function, displays the mapping between sound names and sound ids
 */
extern void DisplaySoundHashTable(void);

//@}

#endif	// !__SOUND_ID_H__
