/*
**	A clone of a famous game.
*/
/**@name ccl_unittype.c	-	The unit-type ccl functions. */
/*
**	(c) Copyright 1999 by Lutz Sammer
**
**	$Id: ccl_unittype.c,v 1.4 1999/03/01 00:21:42 root Exp $
*/

//@{

#include <stdio.h>

#include "clone.h"

#ifdef USE_CCL	// {

#include "video.h"
#include "tileset.h"
#include "map.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "icons.h"
#include "ccl.h"

#include <guile/gh.h>			// I use guile for a quick hack

extern SCM gh_new_procedureN(char* proc_name, SCM (*fn) ());

/**
**	Parse unit-type.
**
**	@param list	List describing missile.
*/
local SCM CclUnitType(SCM list)
{
    SCM value;
    int type;

    return list;

    //	Slot
    value=gh_car(list);
    type=gh_scm2int(value);
    list=gh_cdr(list);
    DebugLevel3("UnitType: %d\n",type);

    //	Name
    value=gh_car(list);
    UnitTypes[type].Name=gh_scm2newstr(value,NULL);
    list=gh_cdr(list);

    //	Graphics
    value=gh_car(list);

    //	FIXME: more ...

    return list;
}

/**
**	Generate C - table for UnitTypes.
*/
local SCM CclPrintUnitTypeTable(void)
{
    PrintUnitTypeTable();

    return SCM_UNSPECIFIED;
}

/**
**	Register CCL features for unit-type.
*/
global void UnitTypeCclRegister(void)
{
    gh_new_procedureN("unit-type",CclUnitType);
    gh_new_procedure0_0("print-unit-type-table",CclPrintUnitTypeTable);
}

#endif	// } USE_CCL

//@}
