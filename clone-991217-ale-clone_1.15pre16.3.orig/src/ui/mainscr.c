/*
**	A clone of a famous game.
*/
/**@name mainscr.c	-	The main screen. */
/*
**	(c) Copyright 1998 by Lutz Sammer and Valery Shchedrin
**
**	$Id: mainscr.c,v 1.14 1999/11/21 22:00:20 root Exp $
*/

//@{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clone.h"
#include "video.h"
#include "image.h"
#include "font.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "upgrade.h"
#include "icons.h"
#include "interface.h"

#define OriginalResources	0	/// 1 for original resource display
#define OriginalTraining	0	/// 1 for the original training display

/*----------------------------------------------------------------------------
--	Icons
----------------------------------------------------------------------------*/

/**
**	Draw life bar of an unit at x,y.
**
**	Placed under icons on top-panel.
**
**	@param unit	Pointer to unit.
**	@param x	Screen X postion of icon
**	@param y	Screen Y postion of icon
*/
local void DrawLifeBar(const Unit* unit,int x,int y)
{
    int f;
    int color;

    y+=ICON_HEIGHT+8;
    FillRectangle(ColorBlack,x,y,ICON_WIDTH+8,7);
    if( unit->HP ) {
	f=(100*unit->HP)/unit->Stats->HitPoints;
	if( f>75) {
	    color=ColorDarkGreen;
	} else if( f>50 ) {
	    color=ColorYellow;
	} else {
	    color=ColorRed;
	}
	f=(f*(ICON_WIDTH+4))/100;
	FillRectangle(color,x+2,y,f,5);
    }
}

/**
**	Draw completed bar into top-panel.
**
**	@param full	the 100% value
**	@param ready	how much till now completed
*/
local void DrawCompleted(int full,int ready)
{
    int f;

    f=(100*ready)/full;
    f=(f*152)/100;
    FillRectangle(ColorDarkGreen,12,168+145,f,14);
    DrawText(50,168+145,SMALL_FONT,"% Complete");
}

/**
**	Draw the stat for an unit in top-panel.
**
**	@param x	Screen X position
**	@param y	Screen Y position
**	@param modified	The modified stat value
**	@param original	The original stat value
*/
local void DrawStats(int x,int y,int modified,int original)
{
    char buf[64];

    if( modified==original ) {
	DrawNumber(x,y,SMALL_FONT,modified);
    } else {
	sprintf(buf,"%d~+~%d",original,modified-original);
	// FIXME: 10 reverse???
	DrawText(x,y,SMALL_FONT,buf);
    }
}

/**
**	Draw the unit info into top-panel.
**
**	@param unit	Pointer to unit.
*/
global void DrawUnitInfo(Unit* unit)
{
    char buf[64];
    const UnitType* type;
    const UnitStats* stats;
    int i;
    int x;
    int y;

    type=unit->Type;
    stats=unit->Stats;
    IfDebug(
	if( !type ) {
	    DebugLevel1(__FUNCTION__": FIXME: free unit selected\n");
	    return;
	} );

    PlayerPixels(unit->Player);

    //
    //	Draw icon in upper left corner
    //
    x=Buttons[1].X;
    y=Buttons[1].Y;
    DrawUnitIcon(type->Icon.Icon
	    ,(ButtonUnderCursor==1)
		? (IconActive|(MouseButtons&LeftButton)) : 0
	    ,x,y);
    DrawLifeBar(unit,x,y);
    if( unit->HP && unit->HP<10000 ) {
	sprintf(buf,"%d/%d",unit->HP,stats->HitPoints);
	DrawTextCentered(12+23,168+53,GAME_FONT,buf);
    }

    //
    //	Draw unit name centered, if too long split at space.
    //
    i=TextLength(SMALL_FONT,type->Name);
    if( i>110 ) {			// didn't fit on line
	const char* s;

	s=strchr(type->Name,' ');
	DebugCheck( !s );
	i=s-type->Name;
	memcpy(buf,type->Name,i);
	buf[i]='\0';
	DrawTextCentered(114,168+ 3,SMALL_FONT,buf);
	DrawTextCentered(114,168+17,SMALL_FONT,s+1);
    } else {
	DrawTextCentered(114,168+17,SMALL_FONT,type->Name);
    }

    //
    //	Show progress for buildings only if they are selected.
    //
    if( type->Building && NumSelected==1 && Selected[0]==unit ) {
	if( unit->Command.Action==UnitActionBuilded ) {
	    // FIXME: not correct must use build time!!
	    DrawCompleted(stats->HitPoints,unit->HP);
	    return;
	}
	if( unit->Command.Action==UnitActionTrain ) {
	    if( OriginalTraining || unit->Command.Data.Train.Count==1 ) {
		DrawText(37,168+78,SMALL_FONT,"Training:");
		DrawUnitIcon(unit->Command.Data.Train.What[0]->Icon.Icon
			,0,107,238);

		DrawCompleted(
			unit->Command.Data.Train.What[0]
			    ->Stats[ThisPlayer->Player].Costs[TimeCost]
			,unit->Command.Data.Train.Ticks);
	    } else {
		DrawTextCentered(114,168+29,SMALL_FONT,"Training...");

		for( i = 0; i < unit->Command.Data.Train.Count; i++ ) {
		    DrawUnitIcon(unit->Command.Data.Train.What[i]->Icon.Icon
			    ,0,Buttons2[i].X,Buttons2[i].Y);
		}

		DrawCompleted(
			unit->Command.Data.Train.What[0]
			    ->Stats[ThisPlayer->Player].Costs[TimeCost]
			,unit->Command.Data.Train.Ticks);
	    }
	    return;
	}
	if( unit->Command.Action==UnitActionUpgradeTo ) {
	    DrawText(29,168+78,SMALL_FONT,"Upgrading:");
	    DrawUnitIcon(unit->Command.Data.UpgradeTo.What->Icon.Icon
		    ,0,107,238);

	    DrawCompleted(
		    unit->Command.Data.UpgradeTo.What
			->Stats[ThisPlayer->Player].Costs[TimeCost]
		    ,unit->Command.Data.UpgradeTo.Ticks);
	    return;
	}
	if( unit->Command.Action==UnitActionResearch ) {
	    DrawText(16,168+78,SMALL_FONT,"Researching:");
	    DrawUnitIcon(Upgrades[unit->Command.Data.Research.What].Icon
		,0,107,238);

	    DrawCompleted(
		    Upgrades[unit->Command.Data.Research.What].Costs[TimeCost]
		    ,unit->Command.Data.Research.Ticks);
	    return;
	}
    }

    if( type->GoldMine ) {
	DrawText(37,168+78,SMALL_FONT,"Gold Left:");
	DrawNumber(108,168+78,SMALL_FONT,unit->Value);
    } else if( type->GivesOil || type->OilPatch ) {
	DrawText(47,168+78,SMALL_FONT,"Oil Left:");
	DrawNumber(108,168+78,SMALL_FONT,unit->Value);
    } else if( type->StoresWood ) {
	DrawText(20,168+78,SMALL_FONT,"Production");
	DrawText(52,168+93,SMALL_FONT,"Lumber:");
	DrawText(108,168+93,SMALL_FONT,"100~+~2~5");
    } else if( type->StoresGold ) {
	DrawText(20,168+61,SMALL_FONT,"Production");
	DrawText(73,168+77,SMALL_FONT,"Gold:");
	DrawNumber(108,168+77,SMALL_FONT,GOLD_PER_MINE);
	// Keep/Stronghold, Castle/Fortress
	if( unit->Player->GoldPerMine==GOLD_PER_MINE+10 ) {
	    DrawText(126,168+77,SMALL_FONT,"~+~1~0");
	} else if( unit->Player->GoldPerMine==GOLD_PER_MINE+20 ) {
	    DrawText(126,168+77,SMALL_FONT,"~+~2~0");
	}
	DrawText(52,168+93,SMALL_FONT,"Lumber:");
	DrawNumber(108,168+93,SMALL_FONT,WOOD_PER_CHOP);
	// Lumber mill
	if( unit->Player->WoodPerChop!=WOOD_PER_CHOP ) {
	    DrawText(126,168+93,SMALL_FONT,"~+~2~5");
	}
	DrawText(84,168+109,SMALL_FONT,"Oil:");
	DrawNumber(108,168+109,SMALL_FONT,OIL_PER_HAUL);
	// FIXME: upgrade oil of refinery
	if( unit->Player->OilPerHaul!=OIL_PER_HAUL ) {
	    DebugLevel0("FIXME: oil-per-haul\n");
	    //DrawText(126,168+93,SMALL_FONT,"~+~2~5");
	}
    } else if( type->Transporter && unit->Value ) {
	// FIXME: Level was centered?
        sprintf(buf,"Level ~%d",stats->Level);
	DrawText(91,168+33,SMALL_FONT,buf);
	for( i=0; i<6; ++i ) {
	    if( unit->OnBoard[i]!=NoUnitP ) {
		DrawUnitIcon(unit->OnBoard[i]->Type->Icon.Icon
		    ,(ButtonUnderCursor==i+4)
			? (IconActive|(MouseButtons&LeftButton)) : 0
			    ,Buttons[i+4].X,Buttons[i+4].Y);
		DrawLifeBar(unit->OnBoard[i],Buttons[i+4].X,Buttons[i+4].Y);
		// FIXME: show also the magic bar :) I want this always.
		if( ButtonUnderCursor==1+4 ) {
		    SetStatusLine(unit->OnBoard[i]->Type->Name);
		}
	    }
	}
	return;
    }

    if( type->Building ) {
	if( type->Type==UnitFarm || type->Type==UnitPigFarm ) {
	    DrawText(16,168+63,SMALL_FONT,"Food Usage");
	    DrawText(58,168+78,SMALL_FONT,"Grown:");
	    DrawNumber(108,168+78,SMALL_FONT,unit->Player->Food);
	    DrawText(71,168+94,SMALL_FONT,"Used:");
	    if( unit->Player->Food<unit->Player->NumUnits ) {
		DefaultReverseText=!DefaultReverseText;
	    }
	    DrawNumber(108,168+94,SMALL_FONT,unit->Player->NumUnits);
	    if( unit->Player->Food<unit->Player->NumUnits ) {
		DefaultReverseText=!DefaultReverseText;
	    }
	}
    } else {
	// FIXME: Level was centered?
        sprintf(buf,"Level ~%d",stats->Level);
	DrawText(91,168+33,SMALL_FONT,buf);

	if( !type->Tanker && !type->Submarine ) {
	    DrawText(57,168+63,SMALL_FONT,"Armor:");
	    DrawStats(108,168+63,stats->Armor,type->_Armor);
	}

	DrawText(47,168+78,SMALL_FONT,"Damage:");
	if( (i=type->_BasicDamage+type->_PiercingDamage) ) {
	    // FIXME: this seems not correct
	    //		Catapult has 25-80
	    //		turtle has 10-50
	    //		jugger has 50-130
	    //		ship has 2-35
	    if( stats->PiercingDamage!=type->_PiercingDamage ) {
		// FIXME: more than 9 +
		sprintf(buf,"%d-%d~+~%d+~%d"
		    ,(stats->PiercingDamage+1)/2,i
		    ,stats->BasicDamage-type->_BasicDamage
		    ,stats->PiercingDamage-type->_PiercingDamage);
	    } else if( stats->PiercingDamage ) {
		sprintf(buf,"%d-%d"
		    ,(stats->PiercingDamage+1)/2,i);
	    } else {
		sprintf(buf,"%d-%d"
		    ,(stats->BasicDamage-30)/2,i);
	    }
	} else {
	    strcpy(buf,"0");
	}
	DrawText(108,168+78,SMALL_FONT,buf);

	DrawText(57,168+94,SMALL_FONT,"Range:");
	DrawStats(108,168+94,stats->AttackRange,type->_AttackRange);

	DrawText(64,168+110,SMALL_FONT,"Sight:");
	DrawStats(108,168+110,stats->SightRange,type->_SightRange);

	DrawText(63,168+125,SMALL_FONT,"Speed:");
	DrawStats(108,168+125,stats->Speed,type->_Speed);

        // Show how much wood is harvested already in percents! :) //vladi
        if( unit->Command.Action==UnitActionHarvest && unit->SubAction==1 ) {
	    sprintf(buf,"W%%:%d"
		    ,(100*(CHOP_FOR_WOOD-unit->Value))/CHOP_FOR_WOOD);
	    DrawText(120,168+140,SMALL_FONT,buf);
        }

	if( type->CanCastSpell ) {
	    DrawText(59,168+140+1,SMALL_FONT,"Magic:");
	    DrawRectangle(ColorGray,108,168+140,59,13);
	    DrawRectangle(ColorBlack,108+1,168+140+1,59-2,13-2);
	    i=(100*unit->Mana)/255;
	    i=(i*(59-3))/100;
	    FillRectangle(ColorBlue,108+2,168+140+2,i,13-3);

	    DrawNumber(128,168+140+1,SMALL_FONT,unit->Mana);
	}
    }
}

/*----------------------------------------------------------------------------
--	RESOURCES
----------------------------------------------------------------------------*/

/**
**	Draw the player resource in top line.
*/
global void DrawResources(void)
{
    char tmp[128];
    DrawImage(ImageTopBorder,0,0,MAP_X,0);

    // FIXME: support more and variable resources.
    if( OriginalResources ) {
	DrawImage(ImageResources,0,0,MAP_X+90,0);
	DrawNumber(MAP_X+107,1,SMALL_FONT,ThisPlayer->Gold);
	DrawImage(ImageResources,1,0,MAP_X+178,0);
	DrawNumber(MAP_X+195,1,SMALL_FONT,ThisPlayer->Wood);
	DrawImage(ImageResources,2,0,MAP_X+266,0);
	DrawNumber(MAP_X+283,1,SMALL_FONT,ThisPlayer->Oil);
    } else {
	DrawImage(ImageResources,0,0,MAP_X+20,0);
	DrawNumber(MAP_X+40,1,SMALL_FONT,ThisPlayer->Gold);
     
	DrawImage(ImageResources,1,0,MAP_X+100,0);
	DrawNumber(MAP_X+120,1,SMALL_FONT,ThisPlayer->Wood);
     
	DrawImage(ImageResources,2,0,MAP_X+180,0);
	DrawNumber(MAP_X+200,1,SMALL_FONT,ThisPlayer->Oil);

	if( ThisPlayer->Food<ThisPlayer->NumUnits ) {
	    DefaultReverseText=!DefaultReverseText;
	}
	sprintf(tmp, "Food: %d/%d"
		, ThisPlayer->NumUnits, ThisPlayer->Food);
	DrawText(MAP_X+260,1,SMALL_FONT,tmp);
	if( ThisPlayer->Food<ThisPlayer->NumUnits ) {
	    DefaultReverseText=!DefaultReverseText;
	}

	sprintf(tmp, "Score: %d"
		, ThisPlayer->Score );
	DrawText(MAP_X+360,1,SMALL_FONT,tmp);
    }
}

/*----------------------------------------------------------------------------
--	MESSAGE
----------------------------------------------------------------------------*/

// FIXME: need messages for chat!

local char* Message;			// message in map window
local int   MessageCounter;		// how long to display message

/**
**	Draw message.
*/
global void DrawMessage(void)
{
    if( Message ) {
	DrawText(MAP_X+10,420,SMALL_FONT,Message);
	if( !--MessageCounter ) {
	    ClearMessage();
	}
    }
}

/**
**	Set message to display.
**
**	@param message	To be displayed in text overlay.
*/
global void SetMessage(char* message)
{
    Message=message;
    MustRedraw|=RedrawMessage|RedrawMap;
    MessageCounter=FRAMES_PER_SECOND*2;
}

/**
**	Set message to display.
**
**	@param message	To be displayed in text overlay.
*/
global void SetMessageDup(char* message)
{
    static char buffer[40];

    strncpy(buffer,message,sizeof(buffer));
    buffer[sizeof(buffer)-1]='\0';

    SetMessage(buffer);
}

/**
**	Clear message to display.
*/
global void ClearMessage(void)
{
    Message=NULL;
    MustRedraw|=RedrawMessage|RedrawMap;
    MessageCounter=0;
}

/*----------------------------------------------------------------------------
--	STATUS LINE
----------------------------------------------------------------------------*/

local char* StatusLine;			// status line/hints

/**
**	Draw status line.
*/
global void DrawStatusLine(void)
{
    DrawImage(ImageBottomBorder,0,0,MAP_X,464);
    if( StatusLine ) {
	DrawText(MAP_X+2,464+2,SMALL_FONT,StatusLine);
    }
}

/**
**	Change status line to new text.
*/
global void SetStatusLine(char* status)
{
    if( StatusLine!=status ) {
	MustRedraw|=RedrawStatusLine;
	StatusLine=status;
    }
}

/**
**	Clear status line.
*/
global void ClearStatusLine(void)
{
    if( StatusLine ) {
	MustRedraw|=RedrawStatusLine;
	StatusLine=NULL;
    }
}

/*----------------------------------------------------------------------------
--	COSTS
----------------------------------------------------------------------------*/

local int CostsMana;			// costs to display in status line
local int CostsGold;
local int CostsWood;
local int CostsOil;

/**
**	Draw costs in status line.
*/
global void DrawCosts(void)
{
    if( CostsMana ) {
	DrawImage(ImageResources,3,0,MAP_X+300,466);
	DrawNumber(MAP_X+316,464+2,SMALL_FONT,CostsMana);
    }
    if( CostsGold ) {
	DrawImage(ImageResources,0,0,MAP_X+300,466);
	DrawNumber(MAP_X+316,464+2,SMALL_FONT,CostsGold);
    }
    if( CostsWood ) {
	DrawImage(ImageResources,1,0,MAP_X+350,466);
	DrawNumber(MAP_X+366,464+2,SMALL_FONT,CostsWood);
    }
    if( CostsOil ) {
	DrawImage(ImageResources,2,0,MAP_X+400,466);
	DrawNumber(MAP_X+416,464+2,SMALL_FONT,CostsOil);
    }
}

/**
**	Set costs in status line.
*/
global void SetCosts(int mana,int gold,int wood,int oil)
{
    CostsMana=mana;
    CostsGold=gold;
    CostsWood=wood;
    CostsOil=oil;
    MustRedraw|=RedrawCosts;
}

/**
**	Clear costs in status line.
*/
global void ClearCosts(void)
{
    CostsMana=0;
    CostsGold=0;
    CostsWood=0;
    CostsOil=0;
    MustRedraw|=RedrawCosts;
}

/*----------------------------------------------------------------------------
--	TOPPANEL
----------------------------------------------------------------------------*/

/**
**	Draw top panel.
*/
global void DrawTopPanel(void)
{
    int i;

    if( NumSelected ) {
	if( NumSelected>1 ) {
	    PlayerPixels(ThisPlayer);	// can only be own!
	    DrawImage(ImagePanelTop,0,0,0,160);
            for( i=0; i<NumSelected; ++i ) {
	        DrawUnitIcon(Selected[i]->Type->Icon.Icon
			     ,(ButtonUnderCursor==i+1)
			         ? (IconActive|(MouseButtons&LeftButton)) : 0
			     ,Buttons[i+1].X,Buttons[i+1].Y);
		DrawLifeBar(Selected[i],Buttons[i+1].X,Buttons[i+1].Y);

		if( ButtonUnderCursor==1+i ) {
		    SetStatusLine(Selected[i]->Type->Name);
		}
	    }
	    return;
	} else {
	    // FIXME: not correct for enemies units
	    if( Selected[0]->Type->Building
		    && (Selected[0]->Command.Action==UnitActionBuilded
		    || Selected[0]->Command.Action==UnitActionResearch
		    || Selected[0]->Command.Action==UnitActionUpgradeTo
		    || Selected[0]->Command.Action==UnitActionUpgrade
		    || Selected[0]->Command.Action==UnitActionTrain) ) {
		DrawImage(ImagePanelTop,3,0,0,160);
	    } else {
		DrawImage(ImagePanelTop,1,0,0,160);
	    }
	    DrawUnitInfo(Selected[0]);
	    if( ButtonUnderCursor==1 ) {
		SetStatusLine(Selected[0]->Type->Name);
	    }
	    return;
	}
    }

    DrawImage(ImagePanelTop,0,0,0,160);
    if( UnitUnderCursor ) {
	// FIXME: not correct for enemies units
	DrawUnitInfo(UnitUnderCursor);
    }
}

//@}
