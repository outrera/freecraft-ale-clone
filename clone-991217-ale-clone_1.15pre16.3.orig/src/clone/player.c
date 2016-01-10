/*
**	A clone of a famous game.
*/
/**@name player.c	-	The players. */
/*
**	(c) Copyright 1998 by Lutz Sammer
**
**	$Id: player.c,v 1.24 1999/12/09 16:27:08 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clone.h"
#include "video.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "ai.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

global int NumPlayers;			/// How many player used
global int NetPlayers;			/// How many network players
global Player Players[PlayerMax];	/// All players in play
global Player* ThisPlayer;		/// Player on this computer

/**
**	Colors used for minimap.
*/
local int PlayerColors[PlayerMax] = {
    208,	// red
    1,		// blue
    216,	// green
    220,	// violett
    224,	// orange
    228,	// black
    255,	// white
    2,		// yellow
    251,	// yellow
    251,	// yellow
    251,	// yellow
    251,	// yellow
    251,	// yellow
    251,	// yellow
    251,	// yellow
    251,	// yellow
};

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Create a new player.
**
**	@param name	Player name.
**	@param type	Player type (Computer,Human,...).
*/
global void CreatePlayer(char* name,int type)
{
    int team;
    Player* player;

    DebugLevel3("Player %d, type %d\n",NumPlayers,type);

    if( NumPlayers==PlayerMax ) {	// already done for bigmaps!
	return;
    }
    player=&Players[NumPlayers];
    player->Player=NumPlayers;

    //  Allocate memory for the "list" of this player's units.
    //  FIXME: brutal way, as we won't need MAX_UNITS for this player...
    if( !(player->Units=(Unit**)calloc(MAX_UNITS,sizeof(Unit*))) ) {
	DebugLevel0("Not enough memory to create player %d.\n",NumPlayers);
	return;
    }
    player->TotalNumUnits=0;

    //
    //	Take first slot for human on this computer,
    //	fill other with computer players.
    //
    if( type==PlayerHuman ) {
	if( !ThisPlayer ) {
	    ThisPlayer=player;
	} else if( !NetPlayers ) {
	    // FIXME: only for single players
	    type=PlayerComputer;
	}
    }

    //
    //	Make simple teams.
    //
    switch( type ) {
	case PlayerNeutral:
	case PlayerNobody:
	default:
	    team=0;
	    break;
	case PlayerComputer:
	    team=1;
	    break;
	case PlayerHuman:
	    team=2+NumPlayers;
	    break;
	case PlayerRescuePassive:
	case PlayerRescueActive:
	    // FIXME: correct for multiplayer games?
	    team=2+NumPlayers;
	    break;
    }

    if( NumPlayers==PlayerMax ) {
	static int already_warned;

	if( !already_warned ) {
	    DebugLevel0("Too many players\n");
	    already_warned=1;
	}
	return;
    }

    player->Name=name;
    player->Type=type;
    player->Race=PlayerRaceHuman;
    player->Team=team;
    player->Ai=PlayerAiUniversal;
    // FIXME: Should become Resource[]...
    player->Gold=3000;
    player->Wood=1000;
    player->Oil=1000;

    // FIXME: Should become Income[]...
    player->GoldPerMine=GOLD_PER_MINE;
    player->WoodPerChop=WOOD_PER_CHOP;
    player->OilPerHaul=OIL_PER_HAUL;

    /*
    for( i=0; i<UnitTypeMax/32; ++i ) {
	player->UnitFlags[i]=0;
    }
    */
    memset( &(player->UnitTypesCount), 0, sizeof(player->UnitTypesCount));

    player->Food=0;
    player->NumUnits=0;
    player->NumBuildings=0;
    player->Score=0;

    player->Color=PlayerColors[NumPlayers];

    if( Players[NumPlayers].Type==PlayerComputer ) {
	player->AiEnabled=1;
    } else {
	player->AiEnabled=0;
    }

    ++NumPlayers;
}

/**
**	Change player side.
**
**	@param player	Pointer to player.
**	@param side	New side (Race).
*/
global void PlayerSetSide(Player* player,int side)
{
    player->Race=side;
}

/**
**	Change player ai.
**
**	@param player	Pointer to player.
**	@param ai	AI type.
*/
global void PlayerSetAiNum(Player* player,int ai)
{
    player->Ai=ai;
}

/**
**	Change player gold.
**
**	@param player	Pointer to player.
**	@param gold	How many gold.
*/
global void PlayerSetGold(Player* player,int gold)
{
    player->Gold=gold;
}

/*
**	Change player wood.
**
**	@param player	Pointer to player.
**	@param wood	How many wood.
*/
global void PlayerSetWood(Player* player,int wood)
{
    player->Wood=wood;
}

/**
**	Change player oil.
**
**	@param player	Pointer to player.
**	@param oil	How many oil.
*/
global void PlayerSetOil(Player* player,int oil)
{
    player->Oil=oil;
}

/**
**	Check if enough food for new unit is available.
**
**	@param player	Pointer to player.
**	@param type	Type of unit.
**	@return		True if enought, false otherwise.
*/
global int PlayerCheckFood(const Player* player,const UnitType* type)
{
    // FIXME: currently all units costs 1 food

    if( player->Food<=player->NumUnits ) {
	if( player==ThisPlayer ) {
	    SetMessage("Not enough food...build more farms.");
	} else {
	    // FIXME: message to AI
	    DebugLevel3("Ai: Not enough food...build more farms.\n");
	}
	return 0;
    }
    return 1;
}

/**
**	Check if enough gold for new unit is available.
**
**	@param player	Pointer to player.
**	@param type	Type of unit.
**	@return		True if enought, false otherwise.
*/
global int PlayerCheckGoldCost(const Player* player,const UnitType* type)
{
    return player->Gold>type->Stats[player->Player].Costs[GoldCost];
}

/**
**	Check if enough wood for new unit is available.
**
**	@param player	Pointer to player.
**	@param type	Type of unit.
**	@return		True if enought, false otherwise.
*/
global int PlayerCheckWoodCost(const Player* player,const UnitType* type)
{
    return player->Wood>type->Stats[player->Player].Costs[WoodCost];
}

/**
**	Check if enough oil for new unit is available.
**
**	@param player	Pointer to player.
**	@param type	Type of unit.
**	@return		True if enought, false otherwise.
*/
global int PlayerCheckOilCost(const Player* player,const UnitType* type)
{
    return player->Oil>type->Stats[player->Player].Costs[OilCost];
}

/**
**	Check if enough resources for are available.
**
**	@param player	Pointer to player.
**	@param gold	How many gold.
**	@param wood	How many wood.
**	@param oil	How many oil.
**	@return		True if all enought, false otherwise.
*/
global int PlayerCheckCosts(const Player* player,int gold,int wood,int oil)
{
    if( player->Gold<gold ) {
	if( player==ThisPlayer ) {
	    SetMessage("Not enough gold...mine more gold.");
	} else {
	    DebugLevel3("Ai: Not enough gold...mine more gold.\n");
	}
	return 0;
    }
    if( player->Wood<wood ) {
	if( player==ThisPlayer ) {
	    SetMessage("Not enough wood...chop more trees.");
	} else {
	    DebugLevel3("Ai: Not enough wood...chop more trees.\n");
	}
	return 0;
    }
    if( player->Oil<oil ) {
	if( player==ThisPlayer ) {
	    SetMessage("Not enough oil...drill more oil.");
	} else {
	    DebugLevel3("Ai: Not enough oil...drill more oil.\n");
	}
	return 0;
    }
    return 1;
}

/**
**	Check if enough resources for new unit is available.
**
**	@param player	Pointer to player.
**	@param type	Type of unit.
**	@return		True if enought, false otherwise.
*/
global int PlayerCheckResources(const Player* player,const UnitType* type)
{
    return PlayerCheckCosts(player
	    ,type->Stats[player->Player].Costs[GoldCost]
	    ,type->Stats[player->Player].Costs[WoodCost]
	    ,type->Stats[player->Player].Costs[OilCost]);
}

/**
**	Subtract costs from resources.
**
**	@param player	Pointer to player.
**	@param gold	How many gold.
**	@param wood	How many wood.
**	@param oil	How many oil.
*/
global void PlayerSubCosts(Player* player,int gold,int wood,int oil)
{
    player->Gold-=gold;
    player->Wood-=wood;
    player->Oil-=oil;

    if( player==ThisPlayer ) {
	MustRedraw|=RedrawResources;
    }
}

/**
**	Add costs from resources.
**
**	@param player	Pointer to player.
**	@param gold	How many gold.
**	@param wood	How many wood.
**	@param oil	How many oil.
*/
global void PlayerAddCosts(Player* player,int gold,int wood,int oil)
{
    player->Gold+=gold;
    player->Wood+=wood;
    player->Oil+=oil;

    if( player==ThisPlayer ) {
	MustRedraw|=RedrawResources;
    }
}

/**
**	Subtract resources for new unit.
**
**	@param player	Pointer to player.
**	@param type	Pointer to unit type.
*/
global void PlayerSubResources(Player* player,const UnitType* type)
{
    player->Gold-=type->Stats[player->Player].Costs[GoldCost];
    player->Wood-=type->Stats[player->Player].Costs[WoodCost];
    player->Oil-=type->Stats[player->Player].Costs[OilCost];

    if( player==ThisPlayer ) {
	MustRedraw|=RedrawResources;
    }
}

/**
**	Add resources for canceled unit.
**
**	@param player	Pointer to player.
**	@param type	Pointer to unit type.
*/
global void PlayerAddResources(Player* player,const UnitType* type)
{
    player->Gold+=type->Stats[player->Player].Costs[GoldCost];
    player->Wood+=type->Stats[player->Player].Costs[WoodCost];
    player->Oil+=type->Stats[player->Player].Costs[OilCost];

    if( player==ThisPlayer ) {
	MustRedraw|=RedrawResources;
    }
}

/**
**	Have unit of type.
**
**	@param player	Pointer to player.
**	@param type	Type of unit.
**	@return		How many exists, false otherwise.
*/
global int HaveUnitTypeByType(const Player* player,const UnitType* type)
{
    return player->UnitTypesCount[type->Type];
}

/**
**	Have unit of type.
**
**	@param player	Pointer to player.
**	@param type	Type of unit.
**	@return		How many exists, false otherwise.
*/
global int HaveUnitTypeByIdent(const Player* player,const char* ident)
{
    return player->UnitTypesCount[UnitTypeByIdent(ident)->Type];
}

/**
**	Initialize the Ai for all players.
*/
global void PlayersInitAi(void)
{
    int player;

    for( player=0; player<NumPlayers; ++player ) {
	if( Players[player].AiEnabled ) {
	    AiInit(player);
	}
    }
}

/**
**	Handle AI of all players each frame.
*/
global void PlayersEachFrame(void)
{
    int player;

    for( player=0; player<NumPlayers; ++player ) {
	if( Players[player].AiEnabled ) {
	    AiEachFrame(player);
	}
    }
}

/**
**	Handle AI of all players each second.
*/
global void PlayersEachSecond(void)
{
    int player;

    for( player=0; player<NumPlayers; ++player ) {
	if( Players[player].AiEnabled ) {
	    AiEachSecond(player);
	}
    }
}

/**
**	Change current color set to new player.
**
**	@param player	Pointer to player.
*/
global void PlayerPixels(const Player* player)
{
    Pixels[208]=player->UnitColor1;
    Pixels[209]=player->UnitColor2;
    Pixels[210]=player->UnitColor3;
    Pixels[211]=player->UnitColor4;
}

/**
**	Setup the player colors for the current palette.
*/
global void SetPlayersPalette(void)
{
    int i;

    for( i=0; i<7; ++i ) {
	Players[i].UnitColor1=Pixels[i*4+208];
	Players[i].UnitColor2=Pixels[i*4+209];
	Players[i].UnitColor3=Pixels[i*4+210];
	Players[i].UnitColor4=Pixels[i*4+211];
    }
    Players[i].UnitColor1=Pixels[12];
    Players[i].UnitColor2=Pixels[13];
    Players[i].UnitColor3=Pixels[14];
    Players[i].UnitColor4=Pixels[15];

    // FIXME: nice colors for this players
    for( ++i; i<15; ++i ) {
	Players[i].UnitColor1=Pixels[i*4+208];
	Players[i].UnitColor2=Pixels[i*4+209];
	Players[i].UnitColor3=Pixels[i*4+210];
	Players[i].UnitColor4=Pixels[i*4+211];
    }
    Players[i].UnitColor1=Pixels[12];
    Players[i].UnitColor2=Pixels[13];
    Players[i].UnitColor3=Pixels[14];
    Players[i].UnitColor4=Pixels[15];
}

/**
**	Output debug informations for players.
*/
global void DebugPlayers(void)
{
    int i;

    DebugLevel0("Nr  I Name     Type         Race     Ai\n");
    DebugLevel0("--  - -------- ------------ -------- --\n");
    for( i=0; i<PlayerMax; ++i ) {
	if( Players[i].Type==PlayerNobody ) {
	    continue;
	}
	DebugLevel0("%2d: %c %-8.8s ",i
		,ThisPlayer==&Players[i] ? '*'
			: Players[i].AiEnabled ? '+' : ' '
		,Players[i].Name);
	switch( Players[i].Type ) {
	    case 0: DebugLevel0("Don't know 0 ");	break;
	    case 1: DebugLevel0("Don't know 1 ");	break;
	    case 2: DebugLevel0("neutral      ");	break;
	    case 3: DebugLevel0("nobody       ");	break;
	    case 4: DebugLevel0("computer     ");	break;
	    case 5: DebugLevel0("human        ");	break;
	    case 6: DebugLevel0("rescue pas.  ");	break;
	    case 7: DebugLevel0("rescue akt.  ");	break;
	}
	switch( Players[i].Race ) {
	    case PlayerRaceHuman:   DebugLevel0("human   ");	break;
	    case PlayerRaceOrc:     DebugLevel0("orc     ");	break;
	    case PlayerRaceNeutral: DebugLevel0("neutral ");	break;
	    default: DebugLevel0("what %d ",Players[i].Race);	break;
	}
	DebugLevel0("%2d ",Players[i].Ai);
	switch( Players[i].Ai ) {
	    case PlayerAiLand:	  DebugLevel0("(land)");	break;
	    case PlayerAiPassive: DebugLevel0("(passive)");	break;
	    case PlayerAiAir:	  DebugLevel0("(air)");		break;
	    case PlayerAiSea:	  DebugLevel0("(sea)");		break;
	    default:		  DebugLevel0("?unknown?");	break;
	}
	DebugLevel0("\n");
    }
}

//@}
