
/* $Id: region_set.h,v 1.3 2002/04/27 17:09:23 latimerius Exp $ */

#ifndef REGION_SET_H
#define REGION_SET_H

#include "region.h"

extern void RegionSetInitialize (void);
extern void RegionSetDestroy (void);
extern void RegionSetFindRegionsInArea (int , int );
extern void RegionSetCreateNeighborLists (int , int , int , int);
extern inline int RegionSetGetNumRegions (void);
extern Region *RegionSetFind (int );
extern void RegionSetDelete (Region * );

#endif /* REGION_SET_H */
