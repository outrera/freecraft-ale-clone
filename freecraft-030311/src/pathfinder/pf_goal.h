
/* $Id: pf_goal.h,v 1.1 2002/04/17 07:36:20 latimerius Exp $ */

#ifndef GOAL_H
#define GOAL_H

#include "pf_highlevel.h"

extern void ComputeGoalBoundaries (Unit * , int * , int * , int * , int * );
extern int GoalReached (Unit * );
extern unsigned short MarkHighlevelGoal (Unit * );
extern void MarkLowlevelGoal (Unit * , HighlevelPath * );

#endif /* GOAL_H */
