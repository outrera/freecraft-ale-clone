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
/**@name ui.c		-	The user interface globals. */
//
//	(c) Copyright 1999-2002 by Lutz Sammer and Andreas Arens
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
//	$Id: ui.c,v 1.44 2003/01/29 04:25:49 jsalmon3 Exp $

//@{

/*----------------------------------------------------------------------------
--	Includes
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freecraft.h"
#include "video.h"
#include "font.h"
#include "interface.h"
#include "map.h"
#include "ui.h"

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

global char RightButtonAttacks;		/// right button 0 move, 1 attack
global char FancyBuildings;		/// Mirror buildings 1 yes, 0 now.

    /// keyboard scroll speed
global int SpeedKeyScroll=KEY_SCROLL_SPEED;
    /// mouse scroll speed
global int SpeedMouseScroll=MOUSE_SCROLL_SPEED;

/**
**	The user interface configuration
*/
global UI TheUI;

/**
**	The available user interfaces.
*/
global UI** UI_Table;

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

local void ClipViewport(Viewport* vp, int ClipX, int ClipY);
local void FinishViewportModeConfiguration(Viewport new_vps[], int num_vps);


/**
**	Initialize the user interface.
**
**	The function looks through ::UI_Table, to find a matching user
**	interface. It uses the race_name and the current video window sizes to
**	find it.
**
**	@param race_name	The race identifier, to select the interface.
*/
global void InitUserInterface(const char *race_name)
{
    int i;
    int best;
    int num_vps;
    int vp_mode;
    Viewport vps[MAX_NUM_VIEWPORTS];

    // select the correct slot
    best=0;
    for( i=0; UI_Table[i]; ++i ) {
	if( !strcmp(race_name,UI_Table[i]->Name) ) {
	    // perfect
	    if( VideoWidth==UI_Table[i]->Width
		    && VideoHeight==UI_Table[i]->Height  ) {
		best=i;
		break;
	    }
	    // too big
	    if( VideoWidth<UI_Table[i]->Width
		    || VideoHeight<UI_Table[i]->Height  ) {
		continue;
	    }
	    // best smaller
	    if( UI_Table[i]->Width*UI_Table[i]->Height>=
		    UI_Table[best]->Width*UI_Table[best]->Height  ) {
		best=i;
	    }
	}
    }

    num_vps=TheUI.NumViewports;
    vp_mode=TheUI.ViewportMode;
    for( i=0; i<num_vps; ++i ) {
	vps[i].MapX=TheUI.Viewports[i].MapX;
	vps[i].MapY=TheUI.Viewports[i].MapY;
    }

    // FIXME: overwrites already set slots?
    // ARI: Yes, it does :(((
    TheUI=*UI_Table[best];

    TheUI.Offset640X=(VideoWidth-640)/2;
    TheUI.Offset480Y=(VideoHeight-480)/2;

    //
    //	Calculations
    //
    if( TheUI.MapArea.EndX > TheMap.Width*TileSizeX-1 ) {
	TheUI.MapArea.EndX = TheMap.Width*TileSizeX-1;
    }
    if( TheUI.MapArea.EndY > TheMap.Height*TileSizeY-1 ) {
	TheUI.MapArea.EndY = TheMap.Height*TileSizeY-1;
    }

    TheUI.SelectedViewport=TheUI.Viewports;

    if( num_vps ) {
	SetViewportMode(vp_mode);
	for( i=0; i<num_vps; ++i ) {
	    TheUI.Viewports[i].MapX=vps[i].MapX;
	    TheUI.Viewports[i].MapY=vps[i].MapY;
	}
	FinishViewportModeConfiguration(TheUI.Viewports,num_vps);
    } else {
	SetViewportMode(VIEWPORT_SINGLE);
    }

    // FIXME: Can be removed after new config is working
    if( !strcmp(race_name,"human") || !strcmp(race_name,"alliance") ) {
	TheUI.NormalFontColor=FontWhite;
	TheUI.ReverseFontColor=FontYellow;
    } else {
	TheUI.NormalFontColor=FontYellow;
	TheUI.ReverseFontColor=FontWhite;
    }
    TheUI.ViewportCursorColor=ColorWhite;
}

/**
**	Load the user interface graphics.
**
**	@todo	If sub images of the same graphic are used, they are loaded
**		multiple into memory. Use the IconFile code and perhaps build
**		a new layer, which supports image sharing.
*/
global void LoadUserInterface(void)
{
    int i;

    //
    //	Load graphics
    //
    if( TheUI.Filler1.File ) {
	TheUI.Filler1.Graphic=LoadGraphic(TheUI.Filler1.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.Filler1.Graphic,TheUI.Filler1.Graphic->Width,TheUI.Filler1.Graphic->Height);
#endif
    }
    if( TheUI.Resource.File ) {
	TheUI.Resource.Graphic=LoadGraphic(TheUI.Resource.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.Resource.Graphic,TheUI.Resource.Graphic->Width,TheUI.Resource.Graphic->Height);
#endif
    }

    for( i=0; i<MaxCosts; ++i ) {
	// FIXME: reuse same graphics?
	if( TheUI.Resources[i].Icon.File ) {
	    TheUI.Resources[i].Icon.Graphic
		    =LoadGraphic(TheUI.Resources[i].Icon.File);
#ifdef USE_OPENGL
	    MakeTexture(TheUI.Resources[i].Icon.Graphic,TheUI.Resources[i].Icon.Graphic->Width,TheUI.Resources[i].Icon.Graphic->Height);
#endif
	}
    }

    // FIXME: reuse same graphics?
    if( TheUI.FoodIcon.File ) {
	TheUI.FoodIcon.Graphic=LoadGraphic(TheUI.FoodIcon.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.FoodIcon.Graphic,TheUI.FoodIcon.Graphic->Width,TheUI.FoodIcon.Graphic->Height);
#endif
    }
    // FIXME: reuse same graphics?
    if( TheUI.ScoreIcon.File ) {
	TheUI.ScoreIcon.Graphic=LoadGraphic(TheUI.ScoreIcon.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.ScoreIcon.Graphic,TheUI.ScoreIcon.Graphic->Width,TheUI.ScoreIcon.Graphic->Height);
#endif
    }

    if( TheUI.InfoPanel.File ) {
	TheUI.InfoPanel.Graphic=LoadGraphic(TheUI.InfoPanel.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.InfoPanel.Graphic,TheUI.InfoPanel.Graphic->Width,TheUI.InfoPanel.Graphic->Height);
#endif
    }
    if( TheUI.ButtonPanel.File ) {
	TheUI.ButtonPanel.Graphic=LoadGraphic(TheUI.ButtonPanel.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.ButtonPanel.Graphic,TheUI.ButtonPanel.Graphic->Width,TheUI.ButtonPanel.Graphic->Height);
#endif
    }
    if( TheUI.MenuButton.File ) {
	TheUI.MenuButton.Graphic=LoadGraphic(TheUI.MenuButton.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.MenuButton.Graphic,TheUI.MenuButton.Graphic->Width,TheUI.MenuButton.Graphic->Height);
#endif
    }
    if( TheUI.Minimap.File ) {
	TheUI.Minimap.Graphic=LoadGraphic(TheUI.Minimap.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.Minimap.Graphic,TheUI.Minimap.Graphic->Width,TheUI.Minimap.Graphic->Height);
#endif
    }
    if( TheUI.StatusLine.File ) {
	TheUI.StatusLine.Graphic=LoadGraphic(TheUI.StatusLine.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.StatusLine.Graphic,TheUI.StatusLine.Graphic->Width,TheUI.StatusLine.Graphic->Height);
#endif
    }

    //
    //	Resolve cursors
    //
    TheUI.Point.Cursor=CursorTypeByIdent(TheUI.Point.Name);
    TheUI.Glass.Cursor=CursorTypeByIdent(TheUI.Glass.Name);
    TheUI.Cross.Cursor=CursorTypeByIdent(TheUI.Cross.Name);
    TheUI.YellowHair.Cursor=CursorTypeByIdent(TheUI.YellowHair.Name);
    TheUI.GreenHair.Cursor=CursorTypeByIdent(TheUI.GreenHair.Name);
    TheUI.RedHair.Cursor=CursorTypeByIdent(TheUI.RedHair.Name);
    TheUI.Scroll.Cursor=CursorTypeByIdent(TheUI.Scroll.Name);

    TheUI.ArrowE.Cursor=CursorTypeByIdent(TheUI.ArrowE.Name);
    TheUI.ArrowNE.Cursor=CursorTypeByIdent(TheUI.ArrowNE.Name);
    TheUI.ArrowN.Cursor=CursorTypeByIdent(TheUI.ArrowN.Name);
    TheUI.ArrowNW.Cursor=CursorTypeByIdent(TheUI.ArrowNW.Name);
    TheUI.ArrowW.Cursor=CursorTypeByIdent(TheUI.ArrowW.Name);
    TheUI.ArrowSW.Cursor=CursorTypeByIdent(TheUI.ArrowSW.Name);
    TheUI.ArrowS.Cursor=CursorTypeByIdent(TheUI.ArrowS.Name);
    TheUI.ArrowSE.Cursor=CursorTypeByIdent(TheUI.ArrowSE.Name);

    if( TheUI.GameMenuePanel.File ) {
	TheUI.GameMenuePanel.Graphic=LoadGraphic(TheUI.GameMenuePanel.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.GameMenuePanel.Graphic,TheUI.GameMenuePanel.Graphic->Width,TheUI.GameMenuePanel.Graphic->Height);
#endif
    }
    if( TheUI.Menue1Panel.File ) {
	TheUI.Menue1Panel.Graphic=LoadGraphic(TheUI.Menue1Panel.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.Menue1Panel.Graphic,TheUI.Menue1Panel.Graphic->Width,TheUI.Menue1Panel.Graphic->Height);
#endif
    }
    if( TheUI.Menue2Panel.File ) {
	TheUI.Menue2Panel.Graphic=LoadGraphic(TheUI.Menue2Panel.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.Menue2Panel.Graphic,TheUI.Menue2Panel.Graphic->Width,TheUI.Menue2Panel.Graphic->Height);
#endif
    }
    if( TheUI.VictoryPanel.File ) {
	TheUI.VictoryPanel.Graphic=LoadGraphic(TheUI.VictoryPanel.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.VictoryPanel.Graphic,TheUI.VictoryPanel.Graphic->Width,TheUI.VictoryPanel.Graphic->Height);
#endif
    }
    if( TheUI.ScenarioPanel.File ) {
	TheUI.ScenarioPanel.Graphic=LoadGraphic(TheUI.ScenarioPanel.File);
#ifdef USE_OPENGL
	MakeTexture(TheUI.ScenarioPanel.Graphic,TheUI.ScenarioPanel.Graphic->Width,TheUI.ScenarioPanel.Graphic->Height);
#endif
    }
}

/**
**	Save the UI structure.
**
**	@param file	Save file handle
**	@param ui	User interface to save
*/
local void OldSaveUi(FILE* file,const UI* ui)
{
    int i;

    fprintf(file,"(define-old-ui '%s %d %d\t; Selector\n",
	    ui->Name,ui->Width,ui->Height);
    fprintf(file,"  ; Filler 1\n");
    fprintf(file,"  (list \"%s\" %d %d)\n",
	    ui->Filler1.File,ui->Filler1X,ui->Filler1Y);
    fprintf(file,"  ; Resource line\n");
    fprintf(file,"  (list \"%s\" %d %d)\n",
	    ui->Resource.File,ui->ResourceX,ui->ResourceY);

    for( i=1; i<MaxCosts; ++i ) {
	fprintf(file,"  ; Resource %s\n",DefaultResourceNames[i]);
	fprintf(file,"  (list \"%s\" %d\n    %d %d %d %d  %d %d)\n",
		ui->Resources[i].Icon.File,ui->Resources[i].IconRow,
		ui->Resources[i].IconX,ui->Resources[i].IconY,
		ui->Resources[i].IconW,ui->Resources[i].IconH,
		ui->Resources[i].TextX,ui->Resources[i].TextY);
    }
    fprintf(file,"  ; Food\n");
    fprintf(file,"  (list \"%s\" %d\n    %d %d %d %d  %d %d)\n",
	    ui->FoodIcon.File,ui->FoodIconRow,
	    ui->FoodIconX,ui->FoodIconY,
	    ui->FoodIconW,ui->FoodIconH,
	    ui->FoodTextX,ui->FoodTextY);
    fprintf(file,"  ; Score\n");
    fprintf(file,"  (list \"%s\" %d\n    %d %d %d %d  %d %d)\n",
	    ui->ScoreIcon.File,ui->ScoreIconRow,
	    ui->ScoreIconX,ui->ScoreIconY,
	    ui->ScoreIconW,ui->ScoreIconH,
	    ui->ScoreTextX,ui->ScoreTextY);

    fprintf(file,"  ; Info panel\n");
    fprintf(file,"  (list \"%s\" %d %d %d %d)\n",
	    ui->InfoPanel.File,
	    ui->InfoPanelX,ui->InfoPanelY,
	    ui->InfoPanelW,ui->InfoPanelH);

    fprintf(file,"  ; Complete bar\n");
    fprintf(file,"  (list %d %d %d %d %d)\n",
	    ui->CompleteBarColor,
	    ui->CompleteBarX,ui->CompleteBarY,
	    ui->CompleteTextX,ui->CompleteTextY);

    fprintf(file,"  ; Button panel\n");
    fprintf(file,"  (list \"%s\" %d %d)\n",
	    ui->ButtonPanel.File,ui->ButtonPanelX,ui->ButtonPanelY);

    fprintf(file,"  ; The map area\n");
    fprintf(file,"  (list %d %d %d %d)\n",
	    ui->MapArea.X, ui->MapArea.Y,
	    ui->MapArea.EndX+1,ui->MapArea.EndY+1);

    fprintf(file,"  ; Menu button background\n");
    fprintf(file,"  (list \"%s\" %d %d)\n",
	    ui->MenuButton.File,ui->MenuButtonX,ui->MenuButtonY);

    fprintf(file,"  ; Minimap background\n");
    fprintf(file,"  (list \"%s\" %d %d)\n",
	    ui->Minimap.File,ui->MinimapX,ui->MinimapY);

    fprintf(file,"  ; Status line\n");
    fprintf(file,"  (list \"%s\" %d %d)\n",
	    ui->StatusLine.File,ui->StatusLineX,ui->StatusLineY);

    fprintf(file,"  ; Buttons\n");
    for( i=0; i<MaxButtons; ++i ) {
	fprintf(file,"  (list %3d %3d %4d %3d)\n",
		ui->Buttons[i].X,ui->Buttons[i].Y,
		ui->Buttons[i].Width,ui->Buttons[i].Height);
    }

    fprintf(file,"  ; Buttons II\n");
    for( i=0; i<6; ++i ) {
	fprintf(file,"  (list %3d %3d %4d %3d)\n",
		ui->Buttons2[i].X,ui->Buttons2[i].Y,
		ui->Buttons2[i].Width,ui->Buttons2[i].Height);
    }

    fprintf(file,"  ; Cursors\n");
    fprintf(file,"  (list");
    fprintf(file," '%s",ui->Point.Name);
    fprintf(file," '%s",ui->Glass.Name);
    fprintf(file," '%s\n",ui->Cross.Name);
    fprintf(file,"    '%s",ui->YellowHair.Name);
    fprintf(file," '%s",ui->GreenHair.Name);
    fprintf(file,"    '%s\n",ui->RedHair.Name);
    fprintf(file,"    '%s\n",ui->Scroll.Name);

    fprintf(file,"    '%s",ui->ArrowE.Name);
    fprintf(file," '%s",ui->ArrowNE.Name);
    fprintf(file," '%s",ui->ArrowN.Name);
    fprintf(file," '%s\n",ui->ArrowNW.Name);
    fprintf(file,"    '%s",ui->ArrowW.Name);
    fprintf(file," '%s",ui->ArrowSW.Name);
    fprintf(file," '%s",ui->ArrowS.Name);
    fprintf(file," '%s)\n",ui->ArrowSE.Name);

    fprintf(file,"  (list \"%s\")\n",ui->GameMenuePanel.File);
    fprintf(file,"  (list \"%s\")\n",ui->Menue1Panel.File);
    fprintf(file,"  (list \"%s\")\n",ui->Menue2Panel.File);
    fprintf(file,"  (list \"%s\")\n",ui->VictoryPanel.File);
    fprintf(file,"  (list \"%s\")",ui->ScenarioPanel.File);

    fprintf(file," )\n\n");
}

/**
**	Save the UI structure.
**
**	@param file	Save file handle
**	@param ui	User interface to save
*/
local void NewSaveUi(FILE * file, const UI * ui)
{
    int i;

    fprintf(file, "(define-ui '%s %d %d\t; Selector\n",
	ui->Name, ui->Width, ui->Height);

    fprintf(file, "  'normal-font-color %d 'reverse-font-color %d\n",
	ui->NormalFontColor, ui->ReverseFontColor);

    fprintf(file, "  'filler-1 '(pos (%d %d) image \"%s\")\n",
	ui->Filler1X, ui->Filler1Y, ui->Filler1.File);

    fprintf(file, "  'resources '(pos (%d %d) image \"%s\"",
	ui->ResourceX, ui->ResourceY, ui->Resource.File);
    for (i = 1; i < MaxCosts; ++i) {
	// FIXME: use slot 0 for time displays!
	fprintf(file, "\n    %s (icon-pos (%d %d) icon-file \"%s\"\n",
	    DefaultResourceNames[i],
	    ui->Resources[i].IconX, ui->Resources[i].IconY,
	    ui->Resources[i].Icon.File);
	fprintf(file,"      icon-frame %d icon-size (%d %d) text-pos (%d %d))",
	    ui->Resources[i].IconRow,
	    ui->Resources[i].IconW, ui->Resources[i].IconH,
	    ui->Resources[i].TextX, ui->Resources[i].TextY);
    }
    fprintf(file, "\n    food (icon-pos (%d %d) icon-file \"%s\"\n",
	ui->FoodIconX, ui->FoodIconY, ui->FoodIcon.File);
    fprintf(file,"      icon-frame %d icon-size (%d %d) text-pos (%d %d))",
	ui->FoodIconRow,
	ui->FoodIconW, ui->FoodIconH, ui->FoodTextX, ui->FoodTextY);
    fprintf(file, "\n    score (icon-pos (%d %d) icon-file \"%s\"\n",
	ui->ScoreIconX, ui->ScoreIconY, ui->ScoreIcon.File);
    fprintf(file,"      icon-frame %d icon-size (%d %d) text-pos (%d %d))",
	ui->ScoreIconRow,
	ui->ScoreIconW, ui->ScoreIconH, ui->ScoreTextX, ui->ScoreTextY);
    fprintf(file, ")\n");

    fprintf(file, "  'info-panel '(pos (%d %d) image \"%s\"\n",
	ui->InfoPanelX, ui->InfoPanelY,
	ui->InfoPanel.File);
    fprintf(file, "    size (%d %d)\n",
	ui->InfoPanelW, ui->InfoPanelH);

    fprintf(file, "    complete-bar (color %d pos (%d %d) text-pos (%d %d)))\n",
	ui->CompleteBarColor,
	ui->CompleteBarX, ui->CompleteBarY,
	ui->CompleteTextX, ui->CompleteTextY);

    fprintf(file, "  'button-panel '(pos (%d %d) image \"%s\")\n",
	ui->ButtonPanelX, ui->ButtonPanelY, ui->ButtonPanel.File);

    fprintf(file, "  'map-area '(pos (%d %d) size (%d %d))\n",
	ui->MapArea.X, ui->MapArea.Y,
	ui->MapArea.EndX + 1, ui->MapArea.EndY + 1);

    fprintf(file, "  'menu-button '(pos (%d %d) image \"%s\")\n",
	ui->MenuButtonX, ui->MenuButtonY, ui->MenuButton.File);

    fprintf(file, "  'minimap '(pos (%d %d) image \"%s\" 'viewport-color %d)\n",
	ui->MinimapX, ui->MinimapY, ui->Minimap.File, ui->ViewportCursorColor);

    fprintf(file, "  'status-line '(pos (%d %d) image \"%s\")\n",
	ui->StatusLineX, ui->StatusLineY, ui->StatusLine.File);

    fprintf(file, "; 0 Menu 1-9 Info 10-19 Button\n");
    fprintf(file, "  'buttons '(");
    for (i = 0; i < MaxButtons; ++i) {
	fprintf(file, "\n    (pos (%3d %3d) size (%4d %3d))",
	    ui->Buttons[i].X, ui->Buttons[i].Y,
	    ui->Buttons[i].Width, ui->Buttons[i].Height);
    }

    fprintf(file, ")\n; 0-5 Training\n");
    fprintf(file, "  'buttons-2 '(");
    for (i = 0; i < 6; ++i) {
	fprintf(file, "\n    (pos (%3d %3d) size (%4d %3d))",
	    ui->Buttons2[i].X, ui->Buttons2[i].Y,
	    ui->Buttons2[i].Width, ui->Buttons2[i].Height);
    }

    fprintf(file, ")\n  'cursors '(point %s\n", ui->Point.Name);
    fprintf(file, "    glass %s\n", ui->Glass.Name);
    fprintf(file, "    cross %s\n", ui->Cross.Name);
    fprintf(file, "    yellow %s\n", ui->YellowHair.Name);
    fprintf(file, "    green %s\n", ui->GreenHair.Name);
    fprintf(file, "    red %s\n", ui->RedHair.Name);
    fprintf(file, "    scroll %s\n", ui->Scroll.Name);

    fprintf(file, "    arrow-e %s\n", ui->ArrowE.Name);
    fprintf(file, "    arrow-ne %s\n", ui->ArrowNE.Name);
    fprintf(file, "    arrow-n %s\n", ui->ArrowN.Name);
    fprintf(file, "    arrow-nw %s\n", ui->ArrowNW.Name);
    fprintf(file, "    arrow-w %s\n", ui->ArrowW.Name);
    fprintf(file, "    arrow-sw %s\n", ui->ArrowSW.Name);
    fprintf(file, "    arrow-s %s\n", ui->ArrowS.Name);
    fprintf(file, "    arrow-se %s)\n", ui->ArrowSE.Name);

    fprintf(file, "  'panels '(game-menu \"%s\"\n", ui->GameMenuePanel.File);
    fprintf(file, "    menue-1 \"%s\"\n", ui->Menue1Panel.File);
    fprintf(file, "    menue-2 \"%s\"\n", ui->Menue2Panel.File);
    fprintf(file, "    victory \"%s\"\n", ui->VictoryPanel.File);
    fprintf(file, "    scenario \"%s\")", ui->ScenarioPanel.File);

    fprintf(file, " )\n\n");
}

/**
**	Save the viewports.
**
**	@param file	Save file handle
**	@param ui	User interface to save
*/
local void SaveViewports(FILE* file,const UI* ui)
{
    int i;
    const Viewport* vp;

    fprintf(file, "(define-viewports 'mode %d",ui->ViewportMode);
    for (i = 0; i < ui->NumViewports; ++i) {
	vp = &ui->Viewports[i];
	fprintf(file, "\n  'viewport '(%d %d)",vp->MapX,vp->MapY);
    }
    fprintf(file, ")\n\n");
}

/**
**	Save the user interface module.
**
**	@param file	Save file handle
*/
global void SaveUserInterface(FILE* file)
{
    fprintf(file,"\n;;; -----------------------------------------\n");
    fprintf(file,";;; MODULE: ui $Id: ui.c,v 1.44 2003/01/29 04:25:49 jsalmon3 Exp $\n\n");

    // Contrast, Brightness, Saturation
    fprintf(file,"(set-contrast! %d)\n",TheUI.Contrast);
    fprintf(file,"(set-brightness! %d)\n",TheUI.Brightness);
    fprintf(file,"(set-saturation! %d)\n\n",TheUI.Saturation);
    // Scrolling
    fprintf(file,"(set-mouse-scroll! %s)\n",TheUI.MouseScroll ? "#t" : "#f");
    fprintf(file,"(set-mouse-scroll-speed! %d)\n",SpeedMouseScroll);
    fprintf(file,"(set-key-scroll! %s)\n",TheUI.KeyScroll ? "#t" : "#f");
    fprintf(file,"(set-key-scroll-speed! %d)\n",SpeedKeyScroll);
    fprintf(file,"(set-reverse-map-move! %s)\n\n",
	    TheUI.ReverseMouseMove ? "#t" : "#f");

    fprintf(file,"(set-mouse-adjust! %d)\n",TheUI.MouseAdjust);
    fprintf(file,"(set-mouse-scale! %d)\n\n",TheUI.MouseScale);

    fprintf(file,"(set-original-resources! %s)\n\n",
	    TheUI.OriginalResources ? "#t" : "#f");

    // Save the current UI
    OldSaveUi(file,&TheUI);
    // NewSaveUi(file,&TheUI);
    SaveViewports(file,&TheUI);
}

/**
**	Clean up the user interface module.
*/
global void CleanUserInterface(void)
{
    int i;

    //
    //	Free the graphics. FIXME: if they are shared this will crash.
    //
    VideoSaveFree(TheUI.Filler1.Graphic);
    VideoSaveFree(TheUI.Resource.Graphic);

    for( i=0; i<MaxCosts; ++i ) {
	VideoSaveFree(TheUI.Resources[i].Icon.Graphic);
    }

    VideoSaveFree(TheUI.FoodIcon.Graphic);
    VideoSaveFree(TheUI.ScoreIcon.Graphic);
    VideoSaveFree(TheUI.InfoPanel.Graphic);
    VideoSaveFree(TheUI.ButtonPanel.Graphic);
    VideoSaveFree(TheUI.MenuButton.Graphic);
    VideoSaveFree(TheUI.Minimap.Graphic);
    VideoSaveFree(TheUI.StatusLine.Graphic);

    VideoSaveFree(TheUI.GameMenuePanel.Graphic);
    VideoSaveFree(TheUI.Menue1Panel.Graphic);
    VideoSaveFree(TheUI.Menue2Panel.Graphic);
    VideoSaveFree(TheUI.VictoryPanel.Graphic);
    VideoSaveFree(TheUI.ScenarioPanel.Graphic);

#if 0
    //
    //	Free the available user interfaces.
    //
    if( UI_Table ) {
	for( i=0; UI_Table[i]; ++i ) {
	    DebugLevel0Fn("FIXME: not completely written\n");
	}
	free(UI_Table);
	UI_Table=NULL;
    }
#endif

    // FIXME: Johns: Implement this correctly or we will lose memory!
    DebugLevel0Fn("FIXME: not completely written\n");

    memset(&TheUI,0,sizeof(TheUI));
}

/**
**	Takes coordinates of a pixel in freecraft's window and computes
**	the map viewport which contains this pixel.
**
**	@param x	x pixel coordinate with origin at UL corner of screen
**	@param y	y pixel coordinate with origin at UL corner of screen
**
**	@return		viewport pointer or NULL if this pixel is not inside
**			any of the viewports.
**
**	@note	This functions only works with rectangular viewports, when
**		we support shaped map window, this must be rewritten.
*/
global Viewport* GetViewport(int x, int y)
{
    Viewport* vp;

    for (vp = TheUI.Viewports; vp < TheUI.Viewports + TheUI.NumViewports;
	    vp++) {
	if (x >= vp->X && x <= vp->EndX && y >= vp->Y && y <= vp->EndY) {
	    return vp;
	}
    }
    return NULL;
}

/**
**	Takes coordinates of a map tile and computes the number of the map
**	viewport (if any) inside which the tile is displayed.
**
**	@param tx	x coordinate of the map tile
**	@param ty	y coordinate of the map tile
**
**	@return		viewport pointer (index into TheUI.Viewports) or NULL
**			if this map tile is not displayed in any of
**			the viewports.
**
**	@note		If the tile (tx,ty) is currently displayed in more
**			than one viewports (may well happen) this function
**			returns the first one it finds.
*/
global Viewport* MapTileGetViewport(int tx, int ty)
{
    Viewport* vp;

    for (vp = TheUI.Viewports; vp < TheUI.Viewports + TheUI.NumViewports;
		vp++) {
	if (tx >= vp->MapX && tx < vp->MapX + vp->MapWidth
		&& ty >= vp->MapY && ty < vp->MapY + vp->MapHeight) {
	    return vp;
	}
    }
    return NULL;
}

/**
**	Takes an array of new Viewports which are supposed to have their
**	pixel geometry (Viewport::[XY] and Viewport::End[XY]) already
**	computed. Using this information as well as old viewport's
**	parameters fills in new viewports' Viewport::Map* parameters.
**	Then it replaces the old viewports with the new ones and finishes
**	the set-up of the new mode.
**
**	@param new_vps	The array of the new viewports
**	@param num_vps	The number of elements in the new_vps[] array.
*/
local void FinishViewportModeConfiguration(Viewport new_vps[], int num_vps)
{
    int i;

    // If the number of viewports increases we need to compute what to display
    // in the newly created ones.  We need to do this before we store new
    // geometry information in the TheUI.Viewports field because we use the old
    // geometry information for map origin computation.
    if (TheUI.NumViewports < num_vps) {
	for (i = 0; i < num_vps; i++) {
	    const Viewport* vp;

	    vp = GetViewport(new_vps[i].X, new_vps[i].Y);
	    if (vp) {
		TheUI.Viewports[i].MapX = Viewport2MapX(vp, new_vps[i].X);
		TheUI.Viewports[i].MapY = Viewport2MapY(vp, new_vps[i].Y);
	    } else {
		TheUI.Viewports[i].MapX = 0;
		TheUI.Viewports[i].MapY = 0;
	    }
	}
    }

    for (i = 0; i < num_vps; i++) {
	TheUI.Viewports[i].X = new_vps[i].X;
	TheUI.Viewports[i].EndX = new_vps[i].EndX;
	TheUI.Viewports[i].Y = new_vps[i].Y;
	TheUI.Viewports[i].EndY = new_vps[i].EndY;
	TheUI.Viewports[i].MapWidth =
	    (new_vps[i].EndX - new_vps[i].X + TileSizeX) / TileSizeX;
	TheUI.Viewports[i].MapHeight =
	    (new_vps[i].EndY - new_vps[i].Y + TileSizeY) / TileSizeY;

	if (TheUI.Viewports[i].MapWidth + TheUI.Viewports[i].MapX >
	    TheMap.Width) {
	    TheUI.Viewports[i].MapX -=
		(TheUI.Viewports[i].MapWidth + TheUI.Viewports[i].MapX) -
		TheMap.Width;
	}
	if (TheUI.Viewports[i].MapHeight + TheUI.Viewports[i].MapY >
	    TheMap.Height) {
	    TheUI.Viewports[i].MapY -=
		(TheUI.Viewports[i].MapHeight + TheUI.Viewports[i].MapY) -
		TheMap.Height;
	}
    }
    TheUI.NumViewports = num_vps;

    //
    //  Update the viewport pointers
    //
    TheUI.MouseViewport = GetViewport(CursorX, CursorY);
    if (TheUI.SelectedViewport > TheUI.Viewports + TheUI.NumViewports - 1) {
	TheUI.SelectedViewport = TheUI.Viewports + TheUI.NumViewports - 1;
    }
}

/**
**	Takes a viewport which is supposed to have its Viewport::[XY]
**	correctly filled-in and computes Viewport::End[XY] attributes
**	according to clipping information passed in other two arguments.
**
**	@param vp	The viewport.
**	@param ClipX	Maximum x-coordinate of the viewport's right side
**			as dictated by current UI's geometry and ViewportMode.
**	@param ClipY	Maximum y-coordinate of the viewport's bottom side
**			as dictated by current UI's geometry and ViewportMode.
**
**	@note		It is supposed that values passed in Clip[XY] will
**			never be greater than TheUI::MapArea::End[XY].
**			However, they can be smaller according to the place
**			the viewport vp takes in context of current ViewportMode.
*/
local void ClipViewport(Viewport* vp, int ClipX, int ClipY)
{
    // begin with maximum possible viewport size
    vp->EndX = vp->X + TheMap.Width * TileSizeX - 1;
    vp->EndY = vp->Y + TheMap.Height * TileSizeY - 1;

    // first clip it to MapArea size if necessary
    if (vp->EndX > ClipX) {
	vp->EndX = ClipX;
    }
    // then clip it to the nearest lower TileSize boundary if necessary
    vp->EndX -= (vp->EndX - vp->X + 1) % TileSizeX;

    // the same for y
    if (vp->EndY > ClipY) {
	vp->EndY = ClipY;
    }
    vp->EndY -= (vp->EndY - vp->Y + 1) % TileSizeY;
}

/**
**	Compute viewport parameters for single viewport mode.
**
**	The parameters 	include viewport's width and height expressed
**	in pixels, its position with respect to FreeCraft's window
**	origin, and the corresponding map parameters expressed in map
**	tiles with origin at map origin (map tile (0,0)).
*/
local void SetViewportModeSingle (void)
{
    Viewport new_vps[MAX_NUM_VIEWPORTS];

    DebugLevel0 ("Single viewport set\n");

    new_vps[0].X = TheUI.MapArea.X;
    new_vps[0].Y = TheUI.MapArea.Y;
    ClipViewport (new_vps, TheUI.MapArea.EndX, TheUI.MapArea.EndY);

    FinishViewportModeConfiguration (new_vps, 1);
}

/**
**	Compute viewport parameters for horizontally split viewport mode.
**	This mode splits the TheUI::MapArea with a horizontal line to
**	2 (approximately) equal parts.
**
**	The parameters 	include viewport's width and height expressed
**	in pixels, its position with respect to FreeCraft's window
**	origin, and the corresponding map parameters expressed in map
**	tiles with origin at map origin (map tile (0,0)).
*/
local void SetViewportModeSplitHoriz (void)
{
    Viewport new_vps[MAX_NUM_VIEWPORTS];

    DebugLevel0 ("Two horizontal viewports set\n");

    new_vps[0].X = TheUI.MapArea.X;
    new_vps[0].Y = TheUI.MapArea.Y;
    ClipViewport (new_vps, TheUI.MapArea.EndX,
		    TheUI.MapArea.Y+(TheUI.MapArea.EndY-TheUI.MapArea.Y+1)/2);

    new_vps[1].X = TheUI.MapArea.X;
    new_vps[1].Y = new_vps[0].EndY + 1;
    ClipViewport (new_vps+1, TheUI.MapArea.EndX, TheUI.MapArea.EndY);

    FinishViewportModeConfiguration (new_vps, 2);
}

/**
**	Compute viewport parameters for horizontal 3-way split viewport mode.
**	This mode splits the TheUI::MapArea with a horizontal line to
**	2 (approximately) equal parts, then splits the bottom part vertically
**	to another 2 parts.
**
**	The parameters 	include viewport's width and height expressed
**	in pixels, its position with respect to FreeCraft's window
**	origin, and the corresponding map parameters expressed in map
**	tiles with origin at map origin (map tile (0,0)).
*/
local void SetViewportModeSplitHoriz3 (void)
{
    Viewport new_vps[MAX_NUM_VIEWPORTS];

    DebugLevel0 ("Horizontal 3-way viewport division set\n");

    new_vps[0].X = TheUI.MapArea.X;
    new_vps[0].Y = TheUI.MapArea.Y;
    ClipViewport (new_vps, TheUI.MapArea.EndX,
		    TheUI.MapArea.Y+(TheUI.MapArea.EndY-TheUI.MapArea.Y+1)/2);

    new_vps[1].X = TheUI.MapArea.X;
    new_vps[1].Y = new_vps[0].EndY + 1;
    ClipViewport (new_vps+1,
		TheUI.MapArea.X +(TheUI.MapArea.EndX-TheUI.MapArea.X+1)/2,
		TheUI.MapArea.EndY);

    new_vps[2].X = new_vps[1].EndX + 1;
    new_vps[2].Y = new_vps[0].EndY + 1;
    ClipViewport (new_vps+2, TheUI.MapArea.EndX, TheUI.MapArea.EndY);

    FinishViewportModeConfiguration (new_vps, 3);
}

/**
**	Compute viewport parameters for vertically split viewport mode.
**	This mode splits the TheUI::MapArea with a vertical line to
**	2 (approximately) equal parts.
**
**	The parameters 	include viewport's width and height expressed
**	in pixels, its position with respect to FreeCraft's window
**	origin, and the corresponding map parameters expressed in map
**	tiles with origin at map origin (map tile (0,0)).
*/
local void SetViewportModeSplitVert (void)
{
    Viewport new_vps[MAX_NUM_VIEWPORTS];

    DebugLevel0 ("Two vertical viewports set\n");

    new_vps[0].X = TheUI.MapArea.X;
    new_vps[0].Y = TheUI.MapArea.Y;
    ClipViewport (new_vps,
		TheUI.MapArea.X +(TheUI.MapArea.EndX-TheUI.MapArea.X+1)/2,
		TheUI.MapArea.EndY);

    new_vps[1].X = new_vps[0].EndX + 1;
    new_vps[1].Y = TheUI.MapArea.Y;
    ClipViewport (new_vps+1, TheUI.MapArea.EndX, TheUI.MapArea.EndY);

    FinishViewportModeConfiguration (new_vps, 2);
}

/**
**	Compute viewport parameters for 4-way split viewport mode.
**	This mode splits the TheUI::MapArea vertically *and* horizontally
**	to 4 (approximately) equal parts.
**
**	The parameters 	include viewport's width and height expressed
**	in pixels, its position with respect to FreeCraft's window
**	origin, and the corresponding map parameters expressed in map
**	tiles with origin at map origin (map tile (0,0)).
*/
local void SetViewportModeQuad (void)
{
    Viewport new_vps[MAX_NUM_VIEWPORTS];

    DebugLevel0 ("Four viewports set\n");

    new_vps[0].X = TheUI.MapArea.X;
    new_vps[0].Y = TheUI.MapArea.Y;
    ClipViewport (new_vps,
		TheUI.MapArea.X +(TheUI.MapArea.EndX-TheUI.MapArea.X+1)/2,
		TheUI.MapArea.Y +(TheUI.MapArea.EndY-TheUI.MapArea.Y+1)/2);

    new_vps[1].X = new_vps[0].EndX + 1;
    new_vps[1].Y = TheUI.MapArea.Y;
    ClipViewport (new_vps+1,
		TheUI.MapArea.EndX,
		TheUI.MapArea.Y +(TheUI.MapArea.EndY-TheUI.MapArea.Y+1)/2);

    new_vps[2].X = TheUI.MapArea.X;
    new_vps[2].Y = new_vps[0].EndY + 1;
    ClipViewport (new_vps+2,
		TheUI.MapArea.X +(TheUI.MapArea.EndX-TheUI.MapArea.X+1)/2,
		TheUI.MapArea.EndY);

    new_vps[3].X = new_vps[1].X;
    new_vps[3].Y = new_vps[2].Y;
    ClipViewport (new_vps+3, TheUI.MapArea.EndX, TheUI.MapArea.EndY);

    FinishViewportModeConfiguration (new_vps, 4);
}

/**
**	Sets up (calls geometry setup routines for) a new viewport mode.
**
**	@param new_mode		New mode's number.
*/
global void SetViewportMode(ViewportMode new_mode)
{
    switch (TheUI.ViewportMode = new_mode) {
	case VIEWPORT_SINGLE:
	    SetViewportModeSingle();
	    break;
	case VIEWPORT_SPLIT_HORIZ:
	    SetViewportModeSplitHoriz();
	    break;
	case VIEWPORT_SPLIT_HORIZ3:
	    SetViewportModeSplitHoriz3();
	    break;
	case VIEWPORT_SPLIT_VERT:
	    SetViewportModeSplitVert();
	    break;
	case VIEWPORT_QUAD:
	    SetViewportModeQuad();
	    break;
	default:
	    DebugLevel0Fn("trying to set an unknown mode!!\n");
	    break;
    }
}

/**
**	Cycles through predefined viewport modes (geometry configurations)
**	in order defined by the ViewportMode enumerated type.
**
**	@param step	The size of step used for cycling. Values that
**			make sense are mostly 1 (next viewport mode) and
*			-1 (previous viewport mode).
*/
global void CycleViewportMode(int step)
{
    int new_mode;

    new_mode = TheUI.ViewportMode + step;
    if (new_mode >= NUM_VIEWPORT_MODES) {
	new_mode = 0;
    }
    if (new_mode < 0) {
	new_mode = NUM_VIEWPORT_MODES - 1;
    }
    SetViewportMode(new_mode);
}

//@}
