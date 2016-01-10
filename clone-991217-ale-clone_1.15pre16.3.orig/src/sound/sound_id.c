/*
**	A clone of a famous game.
*/
/**@name sound_id.c	-	The sound id. */
/*
**	(c) Copyright 1998 by Lutz Sammer and Fabrice Rossi
**
**	$Id: sound_id.c,v 1.7 1999/12/09 16:27:18 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include "clone.h"

#ifdef WITH_SOUND	// {

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "video.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "sound_server.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/**
**	hash table used to store the mapping between sound name and sound id
*/
local GHashTable* SoundNameToId;

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
 ** a wrapper for g_str_equal (defined in glib): only for debug
*/
local gint  my_g_str_equal (const gpointer v,const gpointer v2) {
    gint result;
    result=g_str_equal(v,v2);
    DebugLevel0("Comparing %s to %s, result %d\n",(const char *)v,
		(const char *)v2,result);
    return result;
}

/**
 ** a wrapper for g_str_hash (defined in glib): only for debug
*/
local guint my_g_str_hash  (const gpointer v) {
    guint result=g_str_hash(v);
    DebugLevel0("Hashing %s, result %d\n",(const char *)v,result);
    return result;
}

/**
** allows to display an element of the hash table (for debug)
*/
local void DisplayHashTableIterator(gpointer key,
				    gpointer value,
				    gpointer user_data)
{
    fprintf(stderr,"\tKey `%s' maps to `%p'\n",(char *)key,value);
}

/**
 ** display the sound name hash table. 
*/
global void DisplaySoundHashTable() {
    fprintf(stderr,"Sound HashTable Begin\n");
    g_hash_table_foreach(SoundNameToId,DisplayHashTableIterator,NULL);
    fprintf(stderr,"Sound HashTable End\n");
}

/**
** Ask the sound server to register a sound ans tore the mapping
** between its name and its id.
*/
global SoundId MakeSound(char* sound_name,char* file[],unsigned char nb) {
    SoundId id;

    // ask the server to register the sound
    id=RegisterSound(file,nb);
    // save the mapping from sound_name to id in the hash table.
    MapSound(sound_name,id);
    return id;
}

/**
 ** maps a sound name to its id
*/
global SoundId SoundIdForName(char* sound_name) {
    gpointer *result;
    result=g_hash_table_lookup(SoundNameToId,sound_name);
    if (result) {
	return (SoundId)result;
    } else {
	DebugLevel0("Can't find sound %s in sound table\n",sound_name);
	return NULL;
    }
}

/**
 ** add a new mapping (sound name to sound id) in the hash table
*/
global void MapSound(char* sound_name,SoundId id) {
    if (!SoundNameToId) {
	// Hash Table creation. Only needed prior to insertion, as other
	// functions check for nullness
	//FIXME: is this really true? (Fabrice)
	SoundNameToId=g_hash_table_new(g_str_hash,g_str_equal);
    }
    g_hash_table_insert(SoundNameToId,sound_name,id);
}

/**
 ** ask the sound server to build a special sound group.
*/
global SoundId MakeSoundGroup(char* group_name,SoundId first,SoundId second) {
    SoundId sound;
    
    sound=RegisterTwoGroups(first,second);
    MapSound(group_name,sound);
    return sound;
}

#endif	// } WITH_SOUND

//@}
