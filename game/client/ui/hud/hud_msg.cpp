/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
//  hud_msg.cpp
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "r_efx.h"

#include "effects/CEnvironment.h"

#include "CHudStatusIcons.h"

#if !defined( _TFC )
extern BEAM *pBeam;
extern BEAM *pBeam2;
#endif 

#if defined( _TFC )
void ClearEventList( void );
#endif

/// USER-DEFINED SERVER MESSAGE HANDLERS

void CHLHud::MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf )
{
	//This used to be 0, but the server sends a byte over, so it's changed now. - Solokiller
	ASSERT( iSize == 1 );

	ResetHud();
}

void CAM_ToFirstPerson(void);

void CHLHud :: MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf )
{
	CAM_ToFirstPerson();
}

void CHLHud::InitHud()
{
	BaseClass::InitHud();

#if defined( _TFC )
	ClearEventList();

	// catch up on any building events that are going on
	gEngfuncs.pfnServerCmd( "sendevents" );
#endif

#if !defined( _TFC )
	//Probably not a good place to put this.
	pBeam = pBeam2 = NULL;
#endif
}

void CHLHud::MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf )
{
	InitHud();
}

void CHLHud::MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf )
{
	CBufferReader reader( pbuf, iSize );
	Hud().SetIsTeamplay( reader.ReadByte() != 0 );
}

void CHLHud::MsgFunc_GameState( const char *pszName, int iSize, void *pbuf )
{
	CBufferReader reader( pbuf, iSize );

	Hud().SetIsMultiplayer( reader.ReadByte() != 0 );
}

void CHLHud::MsgFunc_Damage(const char *pszName, int iSize, void *pbuf )
{
	int		armor, blood;
	Vector	from;
	int		i;
	float	count;
	
	CBufferReader reader( pbuf, iSize );
	armor = reader.ReadByte();
	blood = reader.ReadByte();

	for (i=0 ; i<3 ; i++)
		from[i] = reader.ReadCoord();

	count = (blood * 0.5) + (armor * 0.5);

	if (count < 10)
		count = 10;

	// TODO: kick viewangles,  show damage visually
}

void CHLHud::MsgFunc_Concuss( const char *pszName, int iSize, void *pbuf )
{
	CBufferReader reader( pbuf, iSize );
	m_iConcussionEffect = reader.ReadByte();

	auto pStatusIcons = GETHUDCLASS( CHudStatusIcons );

	if (m_iConcussionEffect)
	{
		const auto& color = GetPrimaryColor();
		pStatusIcons->EnableIcon("dmg_concuss", color.r, color.g, color.b );
	}
	else
		pStatusIcons->DisableIcon("dmg_concuss");
}

void CHLHud::MsgFunc_ReceiveW( const char* pszName, int iSize, void* pBuf )
{
	CBufferReader reader( pBuf, iSize );

	g_Environment.SetWeatherType( static_cast<WeatherType::WeatherType>( reader.ReadByte() ) );
}

void CHLHud::MsgFunc_HudColors( const char* pszName, int iSize, void* pBuf )
{
	CBufferReader reader( pBuf, iSize );

	m_HudColors.m_PrimaryColor.r = reader.ReadByte();
	m_HudColors.m_PrimaryColor.g = reader.ReadByte();
	m_HudColors.m_PrimaryColor.b = reader.ReadByte();

	m_HudColors.m_EmptyItemColor.r = reader.ReadByte();
	m_HudColors.m_EmptyItemColor.g = reader.ReadByte();
	m_HudColors.m_EmptyItemColor.b = reader.ReadByte();

	m_HudColors.m_AmmoBarColor.r = reader.ReadByte();
	m_HudColors.m_AmmoBarColor.g = reader.ReadByte();
	m_HudColors.m_AmmoBarColor.b = reader.ReadByte();
}
