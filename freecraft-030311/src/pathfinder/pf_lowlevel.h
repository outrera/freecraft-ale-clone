
/* $Id: pf_lowlevel.h,v 1.4 2002/05/27 22:12:41 latimerius Exp $ */

#ifndef PF_LOWLEVEL_H
#define PF_LOWLEVEL_H

#include "pf_highlevel.h"

typedef struct lowlevel_neighbor {
    int dx, dy;
    int Offset;
} LowlevelNeighbor;

extern LowlevelNeighbor Neighbor[8];

extern int LowlevelInit (void);
extern void LowlevelReset (void);
extern int LowlevelPath (Unit * , HighlevelPath * );
extern void LowlevelSetFieldSeen (int , int );
extern void LowlevelSetGoal (int x, int y);
extern void LowPrintStats (void);

#endif /* PF_LOWLEVEL_H */
