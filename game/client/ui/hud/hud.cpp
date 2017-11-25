/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// hud.cpp
//
// implementation of CHLHud class
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "strtools.h"
#include "vgui_TeamFortressViewport.h"

#include "demo.h"
#include "demo_api.h"
#include "vgui_ScorePanel.h"

#include "CHudAmmo.h"
#include "CHudHealth.h"
#include "CHudSayText.h"
#include "CHudSpectator.h"
#include "CHudGeiger.h"
#include "CHudTrain.h"
#include "CHudBattery.h"
#include "CHudFlashlight.h"
#include "CHudMessage.h"
#include "CHudStatusBar.h"
#include "CHudDeathNotice.h"
#include "CHudAmmoSecondary.h"
#include "CHudTextMessage.h"
#include "CHudStatusIcons.h"
#include "CHudMenu.h"

#include "effects/CEnvironment.h"

class CHLVoiceStatusHelper : public IVoiceStatusHelper
{
public:
	virtual void GetPlayerTextColor(int entindex, int color[3])
	{
		color[0] = color[1] = color[2] = 255;

		if( entindex >= 0 && static_cast<size_t>( entindex ) < ARRAYSIZE( g_PlayerExtraInfo ) )
		{
			int iTeam = g_PlayerExtraInfo[entindex].teamnumber;

			if ( iTeam < 0 )
			{
				iTeam = 0;
			}

			iTeam = iTeam % iNumberOfTeamColors;

			color[0] = iTeamColors[iTeam][0];
			color[1] = iTeamColors[iTeam][1];
			color[2] = iTeamColors[iTeam][2];
		}
	}

	virtual void UpdateCursorState()
	{
		gViewPort->UpdateCursorState();
	}

	virtual int	GetAckIconHeight()
	{
		return ScreenHeight - Hud().GetHud().GetFontHeight() *3 - 6;
	}

	virtual bool			CanShowSpeakerLabels()
	{
		if( gViewPort && gViewPort->m_pScoreBoard )
			return !gViewPort->m_pScoreBoard->isVisible();
		else
			return false;
	}
};
static CHLVoiceStatusHelper g_VoiceStatusHelper;
extern cvar_t* cl_lw;

float g_lastFOV = 0.0;

// TFFree Command Menu
void __CmdFunc_OpenCommandMenu(void)
{
	if ( gViewPort )
	{
		gViewPort->ShowCommandMenu( gViewPort->m_StandardMenu );
	}
}

// TFC "special" command
void __CmdFunc_InputPlayerSpecial(void)
{
	if ( gViewPort )
	{
		gViewPort->InputPlayerSpecial();
	}
}

void __CmdFunc_CloseCommandMenu(void)
{
	if ( gViewPort )
	{
		gViewPort->InputSignalHideCommandMenu();
	}
}

void __CmdFunc_ForceCloseCommandMenu( void )
{
	if ( gViewPort )
	{
		gViewPort->HideCommandMenu();
	}
}

// This is called every time the DLL is loaded
void CHLHud::PreInit()
{
	BaseClass::PreInit();

	HOOK_HUD_MESSAGE( Logo );
	HOOK_HUD_MESSAGE( ResetHUD );
	HOOK_HUD_MESSAGE( GameMode );
	HOOK_HUD_MESSAGE( GameState );
	HOOK_HUD_MESSAGE( InitHUD );
	HOOK_HUD_MESSAGE( ViewMode );
	HOOK_HUD_MESSAGE( Concuss );
	HOOK_HUD_MESSAGE( ReceiveW );
	HOOK_HUD_MESSAGE( HudColors );

	// TFFree CommandMenu
	HOOK_COMMAND( "+commandmenu", OpenCommandMenu );
	HOOK_COMMAND( "-commandmenu", CloseCommandMenu );
	HOOK_COMMAND( "ForceCloseCommandMenu", ForceCloseCommandMenu );
	HOOK_COMMAND( "special", InputPlayerSpecial );

	CVAR_CREATE( "hud_classautokill", "1", FCVAR_ARCHIVE | FCVAR_USERINFO );		// controls whether or not to suicide immediately on TF class switch

	m_pCvarStealMouse = CVAR_CREATE( "hud_capturemouse", "1", FCVAR_ARCHIVE );
	m_pCvarDraw = CVAR_CREATE( "hud_draw", "1", FCVAR_ARCHIVE );
	cl_weather = CVAR_CREATE( "cl_weather", "1", FCVAR_ARCHIVE );
}

void CHLHud::PostInit()
{
	BaseClass::PostInit();

	GetClientVoiceMgr()->Init( &g_VoiceStatusHelper, ( vgui::Panel** )&gViewPort );
}

void CHLHud::CreateHudElements()
{
	BaseClass::CreateHudElements();

	HudList().AddElement( CREATE_HUDELEMENT( CHudAmmo ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudHealth ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudSayText ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudSpectator ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudGeiger ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudTrain ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudBattery ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudFlashlight ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudMessage ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudStatusBar ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudDeathNotice ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudAmmoSecondary ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudTextMessage ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudStatusIcons ) );
	HudList().AddElement( CREATE_HUDELEMENT( CHudMenu ) );
}

void CHLHud::ResetHud()
{
	BaseClass::ResetHud();

	// reset concussion effect
	m_iConcussionEffect = 0;
}

void CHLHud::VidInit()
{
	BaseClass::VidInit();

	//Reset to defaults for new maps. - Solokiller
	SetPrimaryColor( HUD_DEFAULT_PRIMARY_COLOR );
	SetEmptyItemColor( HUD_DEFAULT_EMPTYITEM_COLOR );
	SetAmmoBarColor( HUD_DEFAULT_AMMOBAR_COLOR );

	GetClientVoiceMgr()->VidInit();
}

void CHLHud::LoadSprites()
{
	BaseClass::LoadSprites();

	// ----------
	// Load Sprites
	// ---------
	//	m_hsprFont = LoadSprite("sprites/%d_font.spr");

	m_hsprLogo = INVALID_HSPRITE;
	m_hsprCursor = INVALID_HSPRITE;
}

void CHLHud::MsgFunc_Logo(const char *pszName,  int iSize, void *pbuf)
{
	CBufferReader reader( pbuf, iSize );

	// update Train data
	m_bLogo = reader.ReadByte() != 0;
}

/*
=====================
HUD_GetFOV

Returns last FOV
=====================
*/
float HUD_GetFOV( void )
{
	if ( gEngfuncs.pDemoAPI->IsRecording() )
	{
		// Write it
		int i = 0;
		unsigned char buf[ 100 ];

		// Active
		*( float * )&buf[ i ] = g_lastFOV;
		i += sizeof( float );

		Demo_WriteBuffer( TYPE_ZOOM, i, buf );
	}

	if ( gEngfuncs.pDemoAPI->IsPlayingback() )
	{
		g_lastFOV = g_demozoom;
	}
	return g_lastFOV;
}
