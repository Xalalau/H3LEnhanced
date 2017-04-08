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
// battery.cpp
//
// implementation of CHudBattery class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include "WeaponsConst.h"

#include <string.h>
#include <stdio.h>

#include "CHudBattery.h"

CHudBattery::CHudBattery( const char* const pszName, CHLHud& hud )
	: BaseClass( pszName, hud )
{
}

void CHudBattery::Init()
{
	m_iBat = 0;
	m_fFade = 0;
	GetFlags() = 0;

	HOOK_MESSAGE(Battery);
}


void CHudBattery::VidInit()
{
	int HUD_suit_empty = GetHud().GetSpriteIndex( "suit_empty" );
	int HUD_suit_full = GetHud().GetSpriteIndex( "suit_full" );

	m_hSprite1 = m_hSprite2 = 0;  // delaying get sprite handles until we know the sprites are loaded
	m_prc1 = &GetHud().GetSpriteRect( HUD_suit_empty );
	m_prc2 = &GetHud().GetSpriteRect( HUD_suit_full );
	m_iHeight = m_prc2->bottom - m_prc1->top;
	m_fFade = 0;
}

void CHudBattery::MsgFunc_Battery(const char *pszName,  int iSize, void *pbuf )
{
	GetFlags() |= HUD_ACTIVE;
	
	CBufferReader reader( pbuf, iSize );
	int x = reader.ReadShort();

#if defined( _TFC )
	int y = reader.ReadShort();

	if ( x != m_iBat || y != m_iBatMax )
	{
		m_fFade = FADE_TIME;
		m_iBat = x;
		m_iBatMax = y;
	}
#else
	if ( x != m_iBat )
	{
		m_fFade = FADE_TIME;
		m_iBat = x;
	}
#endif
}


bool CHudBattery::Draw(float flTime)
{
	if ( GetHud().GetHideHudBits().Any( HIDEHUD_HEALTH ) )
		return true;

	wrect_t rc;

	rc = *m_prc2;

#if defined( _TFC )
	float fScale = 0.0;
	
	if ( m_iBatMax > 0 )
		fScale = 1.0 / (float)m_iBatMax;

	rc.top  += m_iHeight * ((float)(m_iBatMax-(min(m_iBatMax,m_iBat))) * fScale); // battery can go from 0 to m_iBatMax so * fScale goes from 0 to 1
#else
	// ############ hu3lifezado ############ //
	// Redimensionado para a nova armadura 666
	//rc.top  += m_iHeight * ((float)(100-(min(100,m_iBat))) * 0.01);	// battery can go from 0 to 100 so * 0.01 goes from 0 to 1
	rc.top += m_iHeight * ((float)(100 - (min(100, (m_iBat / 6 - 5)))) * 0.01);	// battery can go from 0 to 100 so * 0.01 goes from 0 to 1
	// ############ //
#endif

	if (!( Hud().GetWeaponBits() & (1<<(WEAPON_SUIT)) ))
		return true;

	int r, g, b, x, y, a = MIN_ALPHA;

	// ############ hu3lifezado ############ //
	// Mudei a cor do HUD (RGB_YELLOWISH)
	//GetHud().GetPrimaryColor().UnpackRGB( r, g, b );
	UnpackRGB(r, g, b, RGB_WHITEISH);
	// ############ //

	// Has health changed? Flash the health #
	if (m_fFade)
	{
		if (m_fFade > FADE_TIME)
			m_fFade = FADE_TIME;

		m_fFade -= ( Hud().GetTimeDelta() * 20);
		if (m_fFade <= 0)
		{
			m_fFade = 0;
		}

		// Fade the health number back to dim

		a += ( m_fFade / FADE_TIME ) * 128;

	}

	ScaleColors(r, g, b, a );
	
	int iOffset = (m_prc1->bottom - m_prc1->top)/6;

	// ############ hu3lifezado ############ //
	// Movi a armadura para baixo do sangue
	// y = ScreenHeight - GetHud().GetFontHeight() - GetHud().GetFontHeight() / 2;
	// x = ScreenWidth/5;
	y = ScreenHeight - 2 * GetHud().GetFontHeight();
	x = 12;
	// ############ //

	// make sure we have the right sprite handles
	if ( !m_hSprite1 )
		m_hSprite1 = GetHud().GetSprite( GetHud().GetSpriteIndex( "suit_empty" ) );
	if ( !m_hSprite2 )
		m_hSprite2 = GetHud().GetSprite( GetHud().GetSpriteIndex( "suit_full" ) );

	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y - iOffset, m_prc1);

	if (rc.bottom > rc.top)
	{
		SPR_Set(m_hSprite2, r, g, b );
		SPR_DrawAdditive( 0, x, y - iOffset + (rc.top - m_prc2->top), &rc);
	}

	// ############ hu3lifezado ############ //
	// Afastei os numeros um pouco para longe do icone de bateria
	// x += (m_prc1->right - m_prc1->left);
	x += (m_prc1->right - m_prc1->left) + 4;
	// ############ //
	x = GetHud().DrawHudNumber(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iBat, r, g, b);

	return true;
}