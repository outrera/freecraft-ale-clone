/*
**	A clone of a famous game.
*/
/**@name goal.h		-	The game goal headerfile. */
/*
**	(c) Copyright 1999 by Lutz Sammer
**
**	$Id: goal.h,v 1.2 1999/06/19 22:21:57 root Exp $
*/

#ifndef __GOAL_H__
#define __GOAL_H__

//@{

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

/**
**	All possible game goals.
*/
enum _game_goal_ {
    GoalLastSideWins,			/// the last player with units wins
};

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

global void SetGlobalGoal(int goal);	/// set global game goal
global void CheckGoals(void);		/// test if goals reached

//@}

#endif	// !__GOAL_H__
