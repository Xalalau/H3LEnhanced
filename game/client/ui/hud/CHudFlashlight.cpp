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
// flashlight.cpp
//
// implementation of CHudFlashlight class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include "WeaponsConst.h"

#include <string.h>
#include <stdio.h>

#include "CHudFlashlight.h"

#define BAT_NAME "sprites/%d_Flashlight.spr"

CHudFlashlight::CHudFlashlight( const char* const pszName, CHLHud& hud )
	: BaseClass( pszName, hud )
{
}

void CHudFlashlight::Init()
{
	m_fFade = 0;
	m_fOn = 0;

	HOOK_MESSAGE(Flashlight);
	HOOK_MESSAGE(FlashBat);

	GetFlags() |= HUD_ACTIVE;
}

void CHudFlashlight::Reset()
{
	m_fFade = 0;
	m_fOn = 0;
}

void CHudFlashlight::VidInit()
{
	int HUD_flash_empty = GetHud().GetSpriteIndex( "flash_empty" );
	int HUD_flash_full = GetHud().GetSpriteIndex( "flash_full" );
	int HUD_flash_beam = GetHud().GetSpriteIndex( "flash_beam" );

	m_hSprite1 = GetHud().GetSprite(HUD_flash_empty);
	m_hSprite2 = GetHud().GetSprite(HUD_flash_full);
	m_hBeam = GetHud().GetSprite(HUD_flash_beam);
	m_prc1 = &GetHud().GetSpriteRect(HUD_flash_empty);
	m_prc2 = &GetHud().GetSpriteRect(HUD_flash_full);
	m_prcBeam = &GetHud().GetSpriteRect(HUD_flash_beam);
	m_iWidth = m_prc2->right - m_prc2->left;
}

void CHudFlashlight:: MsgFunc_FlashBat(const char *pszName,  int iSize, void *pbuf )
{
	CBufferReader reader( pbuf, iSize );
	int x = reader.ReadByte();
	m_iBat = x;
	m_flBat = ((float)x)/100.0;
}

void CHudFlashlight:: MsgFunc_Flashlight(const char *pszName,  int iSize, void *pbuf )
{
	CBufferReader reader( pbuf, iSize );
	m_fOn = reader.ReadByte();
	int x = reader.ReadByte();
	m_iBat = x;
	m_flBat = ((float)x)/100.0;
}

bool CHudFlashlight::Draw(float flTime)
{
	if ( GetHud().GetHideHudBits().Any( HIDEHUD_FLASHLIGHT | HIDEHUD_ALL ) )
		return true;

	int r, g, b, x, y, a;
	wrect_t rc;

	if (!( Hud().GetWeaponBits() & (1<<(WEAPON_SUIT)) ))
		return true;

	if (m_fOn)
		a = 225;
	else
		a = MIN_ALPHA;

	if (m_flBat < 0.20)
		GetHud().GetEmptyItemColor().UnpackRGB(r,g,b);
	else
		// ############ hu3lifezado ############ //
		// Mudei a cor do HUD (RGB_YELLOWISH)
		//GetHud().GetPrimaryColor().UnpackRGB(r,g,b);
		UnpackRGB(r,g,b, RGB_WHITEISH);
		// ############ //

	ScaleColors(r, g, b, a);

	// ############ hu3lifezado ############ //
	// Icone da lanterna foi movido para a regiao armadura
	//y = (m_prc1->bottom - m_prc2->top) / 2;
	//x = ScreenWidth - m_iWidth - m_iWidth / 2;
	int distancia_do_lado = 130;
	y = ScreenHeight - 32 - 12;
	x = distancia_do_lado;
	// ############ //

	// Draw the flashlight casing
	// ############ hu3lifezado ############ //
	// Desativei o sprite de lanterna desligada
	//SPR_Set(m_hSprite1, r, g, b );
	//SPR_DrawAdditive( 0,  x, y, m_prc1);
	// ############ //

	if ( m_fOn )
	{  // draw the flashlight beam
	   // ############ hu3lifezado ############ //
	   // Gambiarra para o icone da lanterna ligada aparecer por inteiro
		SPR_Set(m_hBeam, r, g, b);
		SPR_DrawAdditive(0, x, y, m_prc1);
		// ############ //

		// ############ hu3lifezado ############ //
		// Nova dimensao lateral da lanterna ligada
		// x = ScreenWidth - m_iWidth/2;
		x = distancia_do_lado + m_iWidth;
		// ############ //

		SPR_Set( m_hBeam, r, g, b );
		SPR_DrawAdditive( 0, x, y, m_prcBeam );
	}

	// draw the flashlight energy level
	// ############ hu3lifezado ############ //
	// Icone da energia da lanterna foi movido para a regiao armadura
	//x = ScreenWidth - m_iWidth - m_iWidth/2 ;
	x = distancia_do_lado;
	// ############ //
	int iOffset = m_iWidth * (1.0 - m_flBat);
	if (iOffset < m_iWidth)
	{
		rc = *m_prc2;
		rc.left += iOffset;

		SPR_Set(m_hSprite2, r, g, b );
		SPR_DrawAdditive( 0, x + iOffset, y, &rc);
	}


	return true;
}