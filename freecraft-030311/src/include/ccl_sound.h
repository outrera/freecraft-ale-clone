//   ___________		     _________		      _____  __
//   \_	  _____/______   ____   ____ \_   ___ \____________ _/ ____\/  |_
//    |    __) \_  __ \_/ __ \_/ __ \/    \  \/\_  __ \__  \\   __\\   __|
//    |     \   |  | \/\  ___/\  ___/\     \____|  | \// __ \|  |   |  |
//    \___  /   |__|    \___  >\___  >\______  /|__|  (____  /__|   |__|
//	  \/		    \/	   \/	     \/		   \/
//  ______________________                           ______________________
//			  T H E   W A R   B E G I N S
//	   FreeCraft - A free fantasy real time strategy game engine
//
/**@name ccl_sound.h	-	The Ccl sound header file. */
//
//	(c) Copyright 1999-2001 by Lutz Sammer and Fabrice Rossi
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
//	$Id: ccl_sound.h,v 1.14 2002/12/17 06:40:43 nehalmistry Exp $

#ifndef __CCL_SOUND_H__
#define __CCL_SOUND_H__

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#ifdef WITH_SOUND	// {

#include "ccl.h"

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern int ccl_sound_p(SCM sound);	/// is it a ccl sound?

extern SoundId ccl_sound_id(SCM sound);	/// scheme -> sound id

extern void SoundCclRegister(void);	/// register ccl features

#else	// }{ defined(WITH_SOUND)

//-----------------------------------------------------------------------------

extern void SoundCclRegister(void);	/// register ccl features

#endif	// } !defined(WITH_SOUND)

//@}

#endif	// !__CCL_SOUND_H__
