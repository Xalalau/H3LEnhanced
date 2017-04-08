//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef GAME_CLIENT_UI_VGUI_VGUI_DEFS_H
#define GAME_CLIENT_UI_VGUI_VGUI_DEFS_H

/**
*	@file
*
*	VGUI1 definitions
*/

#define TF_DEFS_ONLY
#ifdef _TFC
#include "../tfc/tf_defs.h"
#else
//TODO: these constants are defined in StudioModelRenderer as well, but this one was incorrect. This should really be cleaned up - Solokiller
#define PC_LASTCLASS 12
#define PC_UNDEFINED 0
#define MENU_DEFAULT				1
#define MENU_TEAM 					2
#define MENU_CLASS 					3
#define MENU_MAPBRIEFING			4
#define MENU_INTRO 					5
#define MENU_CLASSHELP				6
#define MENU_CLASSHELP2 			7
#define MENU_REPEATHELP 			8
#define MENU_SPECHELP				9
#endif

// Command Menu positions 
#define MAX_MENUS				80
#define MAX_BUTTONS				100

#define BUTTON_SIZE_Y			YRES(30)
#define CMENU_SIZE_X			XRES(160)

#define SUBMENU_SIZE_X			(CMENU_SIZE_X / 8)
#define SUBMENU_SIZE_Y			(BUTTON_SIZE_Y / 6)

#define CMENU_TOP				(BUTTON_SIZE_Y * 4)

//#define MAX_TEAMNAME_SIZE		64
#define MAX_BUTTON_SIZE			32
#define MAX_COMMAND_SIZE		256
#define MAX_MAPNAME				256

// Map Briefing Window
#define MAPBRIEF_INDENT			30

// Team Menu
#define TMENU_INDENT_X			(30 * ((float)ScreenHeight / 640))
#define TMENU_HEADER			100
#define TMENU_SIZE_X			(ScreenWidth - (TMENU_INDENT_X * 2))
#define TMENU_SIZE_Y			(TMENU_HEADER + BUTTON_SIZE_Y * 7)
#define TMENU_PLAYER_INDENT		(((float)TMENU_SIZE_X / 3) * 2)
#define TMENU_INDENT_Y			(((float)ScreenHeight - TMENU_SIZE_Y) / 2)

// Class Menu
#define CLMENU_INDENT_X			(30 * ((float)ScreenHeight / 640))
#define CLMENU_HEADER			100
#define CLMENU_SIZE_X			(ScreenWidth - (CLMENU_INDENT_X * 2))
#define CLMENU_SIZE_Y			(CLMENU_HEADER + BUTTON_SIZE_Y * 11)
#define CLMENU_PLAYER_INDENT	(((float)CLMENU_SIZE_X / 3) * 2)
#define CLMENU_INDENT_Y			(((float)ScreenHeight - CLMENU_SIZE_Y) / 2)

// Arrows
enum
{
	ARROW_UP,
	ARROW_DOWN,
	ARROW_LEFT,
	ARROW_RIGHT,
};

// Scoreboard positions
#define SBOARD_INDENT_X			XRES(104)
#define SBOARD_INDENT_Y			YRES(40)

// low-res scoreboard indents
#define SBOARD_INDENT_X_512		30
#define SBOARD_INDENT_Y_512		30

#define SBOARD_INDENT_X_400		0
#define SBOARD_INDENT_Y_400		20

extern const char* const sTFClassSelection[];
extern int sTFValidClassInts[];
extern const char* const sLocalisedClasses[];
extern int iNumberOfTeamColors;
extern int iTeamColors[ 5 ][ 3 ];
extern const char* const sTFClasses[];

extern int iBuildingCosts[];
#define BUILDSTATE_HASBUILDING		(1<<0)		// Data is building ID (1 = Dispenser, 2 = Sentry, 3 = Entry Teleporter, 4 = Exit Teleporter)
#define BUILDSTATE_BUILDING			(1<<1)
#define BUILDSTATE_BASE				(1<<2)
#define BUILDSTATE_CANBUILD			(1<<3)		// Data is building ID (1 = Dispenser, 2 = Sentry, 3 = Entry Teleporter, 4 = Exit Teleporter)

#define DISGUISE_TEAM1		(1<<0)
#define DISGUISE_TEAM2		(1<<1)
#define DISGUISE_TEAM3		(1<<2)
#define DISGUISE_TEAM4		(1<<3)

enum class DetpackState
{
	CANNOT_DEPLOY = 0,	//!No detpack left
	IS_DEPLOYING,		//!Currently deploying a detpack
	IDLE				//!Has a detpack but isn't doing anything with it
};

#endif //GAME_CLIENT_UI_VGUI_VGUI_DEFS_H
