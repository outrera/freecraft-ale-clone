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
/**@name editor.c	-	Editor functions. */
//
//	(c) Copyright 2002 by Lutz Sammer
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
//	$Id: editor.c,v 1.7 2002/12/17 06:40:42 nehalmistry Exp $

//@{

//----------------------------------------------------------------------------
//	Documentation
//----------------------------------------------------------------------------

/**
**      @page EditorModule Module - Editor
**
**	This is a very simple editor for the FreeCraft engine.
**
**      @section Missing Missing features
**
**	@li Edit upgrade section
**	@li Edit allow section
**	@li Edit .cm files
**	@li Upgraded unit-types should be shown different on map
**	@li Good keyboard bindings
**	@li Script support
**	@li Commandline support
**	@li Cut&Paste
**	@li More random map functions.
*/

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>

#include "freecraft.h"

/*----------------------------------------------------------------------------
--	Defines
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

global const char* EditorStartFile;	/// Editor CCL start file

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

//@}
