/*
**	A clone of a famous game.
*/
/**@name ccl_sound.h	-	The Ccl sound header file. */
/*
**	(c) Copyright 1999 by Lutz Sammer and Fabrice Rossi
**
**	$Id: ccl_sound.h,v 1.6 1999/12/03 09:18:06 root Exp $
*/

#ifndef __CCL_SOUND_H__
#define __CCL_SOUND_H__

//@{

#if defined(USE_CCL) && defined(WITH_SOUND)	// {

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <guile/gh.h>			// for SCM

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern int ccl_sound_p(SCM sound);	/// is it a ccl sound?

extern SoundId ccl_sound_id(SCM sound);	/// scheme -> sound id

extern void SoundCclRegister(void);	/// register ccl features

#else 	// }{ defined(USE_CCL) && defined(WITH_SOUND)

#define SoundCclRegister()		/// dummy function for without sound

#endif 	// } !defined(USE_CCL) && defined(WITH_SOUND)

//@}

#endif	// !__CCL_SOUND_H__
