/*
**	A clone of a famous game.
*/
/**@name ccl.h		-	The clone configuration language headerfile. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: ccl.h,v 1.11 1999/07/08 21:09:18 root Exp $
*/

#ifndef __CCL_H__
#define __CCL_H__

//@{

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

extern char* CclStartFile;		/// CCL start file
extern int CclInConfigFile;		/// True while config file parsing

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern void volatile CclInit(void);	/// Initialise ccl
extern void CclCommand(char* command);	/// Execute a ccl command
// register vararg procedure
//extern void gh_new_procedureN(char* proc_name, SCM (*fn) ());

//@}

#endif	// !__CCL_H__
