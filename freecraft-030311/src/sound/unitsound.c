//   ___________		     _________		      _____  __
//   \_	  _____/______	 ____	____ \_	  ___ \____________ _/ ____\/  |_
//    |	   __) \_  __ \_/ __ \_/ __ \/	  \  \/\_  __ \__  \\	__\\   __|
//    |	    \	|  | \/\  ___/\	 ___/\	   \____|  | \// __ \|	|   |  |
//    \___  /	|__|	\___  >\___  >\______  /|__|  (____  /__|   |__|
//	  \/		    \/	   \/	     \/		   \/
//  ______________________			     ______________________
//			  T H E	  W A R	  B E G I N S
//	   FreeCraft - A free fantasy real time strategy game engine
//
/**@name unitsound.c	-	The unit sounds. */
//
//	(c) Copyright 1999-2001 by Fabrice Rossi
//
//	FreeCraft is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published
//	by the Free Software Foundation; only version 2 of the License.
//
//	FreeCraft is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	$Id: unitsound.c,v 1.17 2002/12/17 07:39:08 nehalmistry Exp $

//@{

/*----------------------------------------------------------------------------
--	Include
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freecraft.h"

#ifdef WITH_SOUND	// {

#include "video.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "sound_server.h"
#include "tileset.h"
#include "map.h"

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

/**
**	Simple sound definition:
**		There is only one sound/voice that could be used for this
**		sound identifier.
*/
typedef struct _simple_sound_ {
    char *Name;				/// name of the sound
    char *File;				/// corresponding sound file
} SimpleSound;

/**
**	Structure for remaping a sound to a new name.
*/
typedef struct _sound_remap_ {
    char* NewName;			/// Name in unit-type definition
    char* BaseName;			/// Name used in sound definition
} SoundRemap;

#define MaxSimpleGroups 7		/// maximal number of sounds pro group

/**
**	Sound group definition:
**		There is a collection of sounds/voices that could be randomly
**		be used fot this sound identifier.
*/
typedef struct _sound_group_ {
    char* Name;				/// name of the group
    char* Sounds[MaxSimpleGroups];	/// list of sound files
} SoundGroup;

/**
**	Selection structure:
**
**	Special sound structure currently used for the selection of an unit.
**	For a special number of the uses the first group is used, after this
**	the second groups is played.
*/
typedef struct _selection_group_ {
    char* Name;				/// name of the selection sound
    char* First;			/// name of the sound
    char* Second;			/// name of the annoyed sound
} SelectionGroup;

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/


/**
**	Simple sounds currently available.
*/
local SimpleSound* SimpleSounds;

/**
**	Sound remaping currently available.
*/
local SoundRemap* SoundRemaps;

/**
**	Sound-groups currently available
*/
local SoundGroup* SoundGroups;

/**
**	Selection-groups currently available
*/
local SelectionGroup* SelectionGroups;

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Computes the number of sounds in a sound group
**
**	@param group	list of file names
**
**	@return		number of sounds in group
*/
local int NbSoundsInGroup(char* const* const group)
{
    int i;

    for(i=0;i<MaxSimpleGroups;i++) {
	if ( !group[i] ) {
	    return i;
	}
    }
    return i;
}


/**
**	Loads all simple sounds (listed in the SimpleSounds array).
*/
local void LoadSimpleSounds(void)
{
    int i;

    if( SimpleSounds ) {
	for(i=0;SimpleSounds[i].Name;i++) {
	    MakeSound(SimpleSounds[i].Name,&(SimpleSounds[i].File),1);
	}
    }
}

/**
**	Loads all sound groups.
**	Special groups are created.
*/
local void LoadSoundGroups(void)
{
    int i;

    if( SoundGroups ) {
	for(i=0;SoundGroups[i].Name;i++) {
	    MakeSound(SoundGroups[i].Name,SoundGroups[i].Sounds,
		    NbSoundsInGroup(SoundGroups[i].Sounds));
	}
    }
    if( SelectionGroups ) {
	for(i=0;SelectionGroups[i].Name;i++) {
	    //FIXME: might be more efficient
	    MakeSoundGroup(SelectionGroups[i].Name,
		    SoundIdForName(SelectionGroups[i].First),
		    SoundIdForName(SelectionGroups[i].Second));
	}
    }
}

/**
**	Performs remaping listed in the Remaps array. Maps also critter
**	sounds to their correct values.
*/
local void RemapSounds(void)
{
    int i;

    if( SoundRemaps ) {
	for(i=0;SoundRemaps[i].NewName;i++) {
	    //FIXME: should be more efficient
	    MapSound(SoundRemaps[i].NewName,
		    SoundIdForName(SoundRemaps[i].BaseName));
	}
    }

    //
    //	Make some general sounds.
    //
    // FIXME: move to config CCL
    MapSound("gold-mine-help",SoundIdForName("basic orc voices help 1"));

    // critter mapping FIXME: must support more terrains.

    switch( TheMap.Terrain ) {
    case TilesetSummer:
	MakeSoundGroup("critter-selected",
		       SoundIdForName("sheep selected"),
		       SoundIdForName("sheep annoyed"));
	break;
    case TilesetWinter:
	MakeSoundGroup("critter-selected",
		       SoundIdForName("seal selected"),
		       SoundIdForName("seal annoyed"));
	break;
    case TilesetWasteland:
	MakeSoundGroup("critter-selected",
		       SoundIdForName("pig selected"),
		       SoundIdForName("pig annoyed"));
	break;
    case TilesetSwamp:
	MakeSoundGroup("critter-selected",
		       SoundIdForName("warthog selected"),
		       SoundIdForName("warthog annoyed"));
	break;
    default:
	DebugLevel2("Unknown Terrain %d\n" _C_ TheMap.Terrain);
    }
}

/**
**	Load all sounds for units.
*/
global void LoadUnitSounds(void)
{
    if( SoundFildes!=-1 ) {
	LoadSimpleSounds();
	LoadSoundGroups();
	RemapSounds();
    }
}

/**
**	Map the sounds of all unit-types to the correct sound id.
**	And overwrite the sound ranges. @todo the sound ranges should be
**	configurable by user with CCL.
*/
global void MapUnitSounds(void)
{
    UnitType* type;

    if( SoundFildes!=-1 ) {
	SetSoundRange(SoundIdForName("tree chopping"),32);

	//
	//	Parse all units sounds.
	//
	for( type=UnitTypes; type->OType; type++) {
	    if( type->Sound.Selected.Name ) {
		type->Sound.Selected.Sound=
			SoundIdForName(type->Sound.Selected.Name);
	    }
	    if( type->Sound.Acknowledgement.Name ) {
		type->Sound.Acknowledgement.Sound=
			SoundIdForName(type->Sound.Acknowledgement.Name);
		/*
		// Acknowledge sounds have infinite range
		SetSoundRange(type->Sound.Acknowledgement.Sound,
			INFINITE_SOUND_RANGE);
		*/
	    }
	    if( type->Sound.Ready.Name ) {
		type->Sound.Ready.Sound=
		    SoundIdForName(type->Sound.Ready.Name);
		// Ready sounds have infinite range
		SetSoundRange(type->Sound.Ready.Sound,
			INFINITE_SOUND_RANGE);
	    }
	    // FIXME: will be modified, attack sound be moved to missile/weapon
	    if( type->Weapon.Attack.Name ) {
		type->Weapon.Attack.Sound=
			SoundIdForName(type->Weapon.Attack.Name);
	    }
	    if( type->Sound.Help.Name ) {
		type->Sound.Help.Sound=
			SoundIdForName(type->Sound.Help.Name);
		// Help sounds have infinite range
		SetSoundRange(type->Sound.Help.Sound,
			INFINITE_SOUND_RANGE);
	    }
	    if( type->Sound.Dead.Name ) {
		type->Sound.Dead.Sound=
			SoundIdForName(type->Sound.Dead.Name);
	    }
	}
    }
}

#endif	// } WITH_SOUND

//@}
