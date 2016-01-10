/*
**	A clone of a famous game.
*/
/**@name interface.c	-	The interface. */
/*
**	(c) Copyright 1998,1999 by Lutz Sammer
**
**	$Id: interface.c,v 1.17 1999/11/25 00:59:30 root Exp $
*/

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>

#include "clone.h"
#include "video.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "interface.h"
#include "ccl.h"
#include "tileset.h"
#include "map.h"
#include "minimap.h"
#include "network.h"

/*----------------------------------------------------------------------------
--	Declaration
----------------------------------------------------------------------------*/

local enum _key_state_ {
    KeyStateCommand,		/// keys -> commands
    KeyStateInput,		/// keys -> line editor
} KeyState;			/// current keyboard state

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

local char Input[50];		/// line input for messages/long commands
local int InputIndex;		/// current index into input
local char InputStatusLine[80];	/// Last input status line
global char GamePaused;		/// Current pause state

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

/**
**	Show input.
*/
local void ShowInput(void)
{
    sprintf(InputStatusLine,"MESSAGE:%s~_",Input);
    ClearStatusLine();
    SetStatusLine(InputStatusLine);
}

/**
**	Handle keys in command mode.
**
**	@param key	Key scancode.
**	@return		True, if key is handles; otherwise false.
*/
local int CommandKey(int key)
{
    switch( key ) {
	case '\r':
	    KeyState=KeyStateInput;
	    Input[0]='\0';
	    InputIndex=0;
	    ShowInput();
	    return 1;
	case '^':
	    UnSelectAll();
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if( KeyModifiers&ModifierControl ) {
                //  dirty atoi version :)
                SetGroup(Selected,NumSelected,key-48);
            } else {
                SelectGroup(key-48);
            }
            UpdateBottomPanel();
            MustRedraw|=RedrawCursor|RedrawMap|RedrawPanels;
            break;
#if 0
    IfDebug(
	case '0':
	    ++ThisPlayer;
	    if( ThisPlayer==&Players[PlayerMax] ) {
		ThisPlayer=&Players[0];
	    }
	    MustRedraw=RedrawEverything;
	    break;

	case '1':
	    --ThisPlayer;
	    if( ThisPlayer<&Players[0] ) {
		ThisPlayer=&Players[PlayerMax-1];
	    }
	    MustRedraw=RedrawEverything;
	    break;
    );
#endif
        case KeyCodePause:
	case 'P':			// If pause-key didn't work
            if(GamePaused) {
                GamePaused=0;
                SetStatusLine("Game Resumed");
	    } else {
                GamePaused=1;
                SetStatusLine("Game Paused");
	    }
            break;

	case '+':
	    VideoSyncSpeed+=10;
	    InitVideoSync();
	    SetStatusLine("Faster");
	    break;

	case '-':
	    VideoSyncSpeed-=10;
	    InitVideoSync();
	    SetStatusLine("Slower");
	    break;

	case 'S':			// SMALL s is needed for panel
	    SaveAll();
	    break;

	case 'c':
	    if(	NumSelected==1 ) {
		MapCenter(Selected[0]->X,Selected[0]->Y);
	    }
	    break;

//	TAB toggles minimap.
//	FIXME: more...
	case '\t':
	    DebugLevel1("TAB\n");
	    MinimapWithTerrain^=1;
	    MustRedraw|=RedrawMinimap;
	    break;
	    // FIXME: shift+TAB

	case 'q':
	    Exit(0);
	case KeyCodeUp:
	    if( MapY ) {
		if( KeyModifiers&ModifierControl ) {
		    if( MapY<MapHeight/2 ) {
			MapY=0;
		    } else {
			MapY-=MapHeight/2;
		    }
		} else {
		    --MapY;
		}
		MustRedraw|=RedrawMaps|RedrawMinimapCursor;
	    }
	    break;
	case KeyCodeDown:
	    if( MapY<TheMap.Height-MapHeight ) {
		if( KeyModifiers&ModifierControl ) {
		    if( MapY<TheMap.Height-MapHeight-MapHeight/2 ) {
			MapY+=MapHeight/2;
		    } else {
			MapY=TheMap.Height-MapHeight;
		    }
		} else {
		    ++MapY;
		}
		MustRedraw|=RedrawMaps|RedrawMinimapCursor;
	    }
	    break;
	case KeyCodeLeft:
	    if( MapX ) {
		if( KeyModifiers&ModifierControl ) {
		    if( MapX<MapWidth/2 ) {
			MapX=0;
		    } else {
			MapX-=MapWidth/2;
		    }
		} else {
		    --MapX;
		}
		MustRedraw|=RedrawMaps|RedrawMinimapCursor;
	    }
	    break;
	case KeyCodeRight:
	    if( MapX<TheMap.Width-MapWidth ) {
		if( KeyModifiers&ModifierControl ) {
		    if( MapX<TheMap.Width-MapWidth-MapWidth/2 ) {
			MapX+=MapWidth/2;
		    } else {
			MapX=TheMap.Width-MapWidth;
		    }
		} else {
		    ++MapX;
		}
		MustRedraw|=RedrawMaps|RedrawMinimapCursor;
	    }
	    break;

	default:
	    DebugLevel3("Key %d\n",key);
	    return 0;
    }
    return 1;
}

/**
**	Handle keys in input mode.
**
**	@param key	Key scancode.
**	@return		True input finished.
*/
local int InputKey(int key)
{
    switch( key ) {
	case '\r':
#ifdef USE_CCL
	    if( Input[0]=='(' ) {
		CclCommand(Input);
	    } else {
#endif
                // Handle cheats
		// FIXME: disable cheats
                if (strcmp(Input, "hatchet") == 0)
                  {
                  SpeedChop = 52/2;
                  SetMessage( "Wow -- I got jigsaw!" );
                  } else
                if (strcmp(Input, "glittering prizes") == 0)
                  {
                  ThisPlayer->Gold += 12000;
                  ThisPlayer->Wood += 5000;
                  ThisPlayer->Oil  += 5000;
		  MustRedraw|=RedrawResources;
		  SetMessage( "!!! :)" );
                  } else
                if (strcmp(Input, "on screen") == 0)
                  {
                  RevealMap();
                  } else
                if (strcmp(Input, "fow on") == 0)
                  {
                  TheMap.NoFogOfWar = 0;
                  MapUpdateVisible();
                  SetMessage( "Fog Of War is now ON" );
                  } else
                if (strcmp(Input, "fow off") == 0)
                  {
                  TheMap.NoFogOfWar = 1;
                  MapUpdateVisible();
                  SetMessage( "Fog Of War is now OFF" );
                  } else
                if (strcmp(Input, "fast debug") == 0)
                  {
                  SpeedMine=10;         /// speed factor for mine gold
                  SpeedGold=10;         /// speed factor for getting gold
                  SpeedChop=10;         /// speed factor for chop
                  SpeedWood=10;         /// speed factor for getting wood
                  SpeedHaul=10;         /// speed factor for haul oil
                  SpeedOil=10;          /// speed factor for getting oil
                  SpeedBuild=10;        /// speed factor for building
                  SpeedTrain=10;        /// speed factor for training
                  SpeedUpgrade=10;      /// speed factor for upgrading
                  SpeedResearch=10;     /// speed factor for researching
                  SetMessage( "FAST DEBUG SPEED" );
                  } else
                if (strcmp(Input, "normal debug") == 0)
                  {
                  SpeedMine=1;         /// speed factor for mine gold
                  SpeedGold=1;         /// speed factor for getting gold
                  SpeedChop=1;         /// speed factor for chop
                  SpeedWood=1;         /// speed factor for getting wood
                  SpeedHaul=1;         /// speed factor for haul oil
                  SpeedOil=1;          /// speed factor for getting oil
                  SpeedBuild=1;        /// speed factor for building
                  SpeedTrain=1;        /// speed factor for training
                  SpeedUpgrade=1;      /// speed factor for upgrading
                  SpeedResearch=1;     /// speed factor for researching
                  SetMessage( "NORMAL DEBUG SPEED" );
                  }

		// FIXME: only to selected players
		NetworkChatMessage(Input);
#ifdef USE_CCL
	    }
#endif
	case '\e':
	    ClearStatusLine();
	    KeyState=KeyStateCommand;
	    return 1;
	case '\b':
	    DebugLevel3("Key <-\n");
	    if( InputIndex ) {
		Input[--InputIndex]='\0';
		ShowInput();
	    }
	    return 1;
	default:
	    if( key>=' ' && key<=256 ) {
		if( InputIndex<sizeof(Input) ) {
		    DebugLevel3("Key %c\n",key);
		    Input[InputIndex++]=key;
		    Input[InputIndex]='\0';
		    ShowInput();
		}
		return 1;
	    }
	    break;
    }
    return 0;
}

/**
**	Handle key down.
**
**	@param key	Key scancode.
**	@return		True, if key is handles; otherwise false.
*/
global int HandleKeyDown(int key)
{
    switch( KeyState ) {
	case KeyStateCommand:
	    return CommandKey(key);
	case KeyStateInput:
	    return InputKey(key);
    }
    return 0;
}

//@}
