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
// Train.cpp
//
// implementation of CHudAmmo class
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"

#include "CHudTrain.h"

CHudTrain::CHudTrain( const char* const pszName, CHLHud& hud )
	: BaseClass( pszName, hud )
{
}

void CHudTrain::Init()
{
	HOOK_MESSAGE( Train );

	m_iPos = 0;
	GetFlags() = 0;
}

void CHudTrain::VidInit()
{
	m_hSprite = 0;
}

bool CHudTrain::Draw(float fTime)
{
	if ( !m_hSprite )
		m_hSprite = LoadSprite("sprites/%d_train.spr");

	if (m_iPos)
	{
		int r, g, b, x, y;

		GetHud().GetPrimaryColor().UnpackRGB(r,g,b);
		SPR_Set(m_hSprite, r, g, b );

		// This should show up to the right and part way up the armor number
		y = ScreenHeight - SPR_Height(m_hSprite,0) - GetHud().GetFontHeight();
		x = ScreenWidth/3 + SPR_Width(m_hSprite,0)/4;

		SPR_DrawAdditive( m_iPos - 1,  x, y, NULL);

	}

	return true;
}

void CHudTrain::MsgFunc_Train(const char *pszName,  int iSize, void *pbuf)
{
	CBufferReader reader( pbuf, iSize );

	// update Train data
	m_iPos = reader.ReadByte();

	if (m_iPos)
		GetFlags() |= HUD_ACTIVE;
	else
		GetFlags() &= ~HUD_ACTIVE;
}
