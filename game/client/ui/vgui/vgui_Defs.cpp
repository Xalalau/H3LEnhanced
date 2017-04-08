//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "vgui_Defs.h"

const char* const sTFClassSelection[] =
{
	"civilian",
	"scout",
	"sniper",
	"soldier",
	"demoman",
	"medic",
	"hwguy",
	"pyro",
	"spy",
	"engineer",
	"randompc",
	"civilian",
};

#ifdef _TFC
int iBuildingCosts[] =
{
	BUILD_COST_DISPENSER,
	BUILD_COST_SENTRYGUN,
	BUILD_COST_TELEPORTER
};

// This maps class numbers to the Invalid Class bit.
// This is needed for backwards compatability in maps that were finished before
// all the classes were in TF. Hence the wacky sequence.
int sTFValidClassInts[] =
{
	0,
	TF_ILL_SCOUT,
	TF_ILL_SNIPER,
	TF_ILL_SOLDIER,
	TF_ILL_DEMOMAN,
	TF_ILL_MEDIC,
	TF_ILL_HVYWEP,
	TF_ILL_PYRO,
	TF_ILL_SPY,
	TF_ILL_ENGINEER,
	TF_ILL_RANDOMPC,
};
#endif

const char* const sLocalisedClasses[] =
{
	"#Civilian",
	"#Scout",
	"#Sniper",
	"#Soldier",
	"#Demoman",
	"#Medic",
	"#HWGuy",
	"#Pyro",
	"#Spy",
	"#Engineer",
	"#Random",
	"#Civilian",
};

// Team Colors
int iNumberOfTeamColors = 5;
int iTeamColors[ 5 ][ 3 ] =
{
	{ 255, 170, 0 },	// HL orange (default)
	{ 125, 165, 210 },	// Blue
	{ 200, 90, 70 },	// Red
	{ 225, 205, 45 },	// Yellow
	{ 145, 215, 140 },	// Green
};

// Used for Class specific buttons
const char* const sTFClasses[] =
{
	"",
	"SCOUT",
	"SNIPER",
	"SOLDIER",
	"DEMOMAN",
	"MEDIC",
	"HWGUY",
	"PYRO",
	"SPY",
	"ENGINEER",
	"CIVILIAN",
};
