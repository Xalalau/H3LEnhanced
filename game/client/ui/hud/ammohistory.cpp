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
//  ammohistory.cpp
//


#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"
#include "CBasePlayer.h"

#include "CWeaponHUDInfo.h"

#include "hl/CClientPrediction.h"

#include "ammohistory.h"

HistoryResource gHR;

void HistoryResource::Init()
{
	m_phud_drawhistory_time = CVAR_CREATE( "hud_drawhistory_time", "5", 0 );

	Reset();
}

void HistoryResource::AddToHistory( int iType, int iId, int iCount )
{
	if ( iType == HISTSLOT_AMMO && !iCount )
		return;  // no amount, so don't add

	if ( ((( GetAmmoPickupGap() * iCurrentHistorySlot) + GetAmmoPickupPickHeight()) > GetAmmoPickupHeightMax()) || (iCurrentHistorySlot >= MAX_HISTORY) )
	{	// the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}
	
	HIST_ITEM *freeslot = &rgAmmoHistory[iCurrentHistorySlot++];  // default to just writing to the first slot

	freeslot->type = iType;
	freeslot->iId = iId;
	freeslot->iCount = iCount;
	freeslot->DisplayTime = Hud().GetTime() + m_phud_drawhistory_time->value;
}

void HistoryResource::AddToHistory( int iType, const char *szName, int iCount )
{
	if ( iType != HISTSLOT_ITEM )
		return;

	if ( ((( GetAmmoPickupGap() * iCurrentHistorySlot) + GetAmmoPickupPickHeight() ) > GetAmmoPickupHeightMax() ) || (iCurrentHistorySlot >= MAX_HISTORY) )
	{	// the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM *freeslot = &rgAmmoHistory[iCurrentHistorySlot++];  // default to just writing to the first slot

	// I am really unhappy with all the code in this file

	int i = Hud().GetHud().GetSpriteIndex( szName );
	if ( i == -1 )
		return;  // unknown sprite name, don't add it to history

	freeslot->iId = i;
	freeslot->type = iType;
	freeslot->iCount = iCount;

	freeslot->DisplayTime = Hud().GetTime() + m_phud_drawhistory_time->value;
}


void HistoryResource::CheckClearHistory()
{
	for ( int i = 0; i < MAX_HISTORY; i++ )
	{
		if ( rgAmmoHistory[i].type )
			return;
	}

	iCurrentHistorySlot = 0;
}

//
// Draw Ammo pickup history
//
int HistoryResource::DrawAmmoHistory( float flTime )
{
	CBasePlayer* pPlayer = g_Prediction.GetLocalPlayer();

	for ( int i = 0; i < MAX_HISTORY; i++ )
	{
		if ( rgAmmoHistory[i].type )
		{
			rgAmmoHistory[i].DisplayTime = min( rgAmmoHistory[i].DisplayTime, Hud().GetTime() + m_phud_drawhistory_time->value );

			if ( rgAmmoHistory[i].DisplayTime <= flTime )
			{  // pic drawing time has expired
				memset( &rgAmmoHistory[i], 0, sizeof(HIST_ITEM) );
				CheckClearHistory();
			}
			else if ( rgAmmoHistory[i].type == HISTSLOT_AMMO )
			{
				const WeaponHUDSprite* spr = CBasePlayer::GetAmmoPicFromWeapon( rgAmmoHistory[i].iId );

				int r, g, b;
				// ############ hu3lifezado ############ //
				// Mudei a cor do HUD (RGB_YELLOWISH)
				//gHUD.GetPrimaryColor().UnpackRGB(r,g,b);
				UnpackRGB(r, g, b, RGB_WHITEISH);
				// ############ //
				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				ScaleColors(r, g, b, min(scale, 255.0f) );

				// Draw the pic
				// ############ hu3lifezado ############ //
				// Inverti a ordem de exibicao dos icones de itens e armas coletados (comeca por cima agora)
				//int ypos = ScreenHeight - ( GetAmmoPickupPickHeight() + ( GetAmmoPickupGap() * i));
				int ypos = GetAmmoPickupPickHeight() + (GetAmmoPickupGap() * i);
				// ############ // 
				int xpos = ScreenWidth - 24;
				if ( spr && spr->hSprite )    // weapon isn't loaded yet so just don't draw the pic
				{ // the dll has to make sure it has sent info the weapons you need
					SPR_Set( spr->hSprite, r, g, b );
					SPR_DrawAdditive( 0, xpos, ypos, &spr->rect );
				}

				// Draw the number
				Hud().GetHud().DrawHudNumberString( xpos - 10, ypos, xpos - 100, rgAmmoHistory[i].iCount, r, g, b );
			}
			else if ( rgAmmoHistory[i].type == HISTSLOT_WEAP )
			{
				CBasePlayerWeapon *weap = g_Prediction.GetWeapon( rgAmmoHistory[i].iId );

				if ( !weap )
					return 1;  // we don't know about the weapon yet, so don't draw anything

				int r, g, b;
				// ############ hu3lifezado ############ //
				// Mudei a cor do HUD (RGB_YELLOWISH)
				//gHUD.GetPrimaryColor().UnpackRGB(r,g,b);
				UnpackRGB(r, g, b, RGB_WHITEISH);
				// ############ //

				if ( !pPlayer->HasAmmo( weap ) )
					gHUD.GetEmptyItemColor().UnpackRGB(r,g,b);	// if the weapon doesn't have ammo, display it as red

				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				ScaleColors(r, g, b, min(scale, 255.0f) );

				const auto& inactive = weap->GetWeaponInfo()->GetHUDInfo()->GetInactive();

				// ############ hu3lifezado ############ //
				// Inverti a ordem de exibicao dos icones de itens e armas coletados (comeca por cima agora)
				//int ypos = ScreenHeight - ( GetAmmoPickupPickHeight() + ( GetAmmoPickupGap() * i));
				int ypos = GetAmmoPickupPickHeight() + (GetAmmoPickupGap() * i);
				// ############ //  
				int xpos = ScreenWidth - ( inactive.rect.right - inactive.rect.left);
				SPR_Set( inactive.hSprite, r, g, b );
				SPR_DrawAdditive( 0, xpos, ypos, &inactive.rect );
			}
			else if ( rgAmmoHistory[i].type == HISTSLOT_ITEM )
			{
				int r, g, b;

				if ( !rgAmmoHistory[i].iId )
					continue;  // sprite not loaded

				wrect_t rect = Hud().GetHud().GetSpriteRect( rgAmmoHistory[i].iId );

				// ############ hu3lifezado ############ //
				// Mudei a cor do HUD (RGB_YELLOWISH)
				//gHUD.GetPrimaryColor().UnpackRGB(r,g,b);
				UnpackRGB(r, g, b, RGB_WHITEISH);
				// ############ //
				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				ScaleColors(r, g, b, min(scale, 255.0f) );

				// ############ hu3lifezado ############ //
				// Inverti a ordem de exibicao dos icones de itens e armas coletados (comeca por cima agora)
				//int ypos = ScreenHeight - ( GetAmmoPickupPickHeight() + ( GetAmmoPickupGap() * i));
				int ypos = GetAmmoPickupPickHeight() + (GetAmmoPickupGap() * i);
				// ############ //  
				int xpos = ScreenWidth - (rect.right - rect.left) - 10;

				SPR_Set( Hud().GetHud().GetSprite( rgAmmoHistory[i].iId ), r, g, b );
				SPR_DrawAdditive( 0, xpos, ypos, &rect );
			}
		}
	}


	return 1;
}


