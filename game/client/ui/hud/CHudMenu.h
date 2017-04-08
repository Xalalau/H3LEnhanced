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
#ifndef GAME_CLIENT_UI_HUD_CHUDMENU_H
#define GAME_CLIENT_UI_HUD_CHUDMENU_H

#include "shared/hud/CHudElement.h"
#include "hud.h"

class CHudMenu : public CBaseHudElement<CHLHud>
{
private:
	static const size_t MAX_MENU_STRING = 512;

public:
	DECLARE_CLASS( CHudMenu, CBaseHudElement<CHLHud> );

	CHudMenu( const char* const pszName, CHLHud& hud );

	void Init() override;
	void InitHUDData() override;
	void VidInit() override;
	void Reset()  override;
	bool Draw( float flTime ) override;
	void MsgFunc_ShowMenu( const char *pszName, int iSize, void *pbuf );

	void SelectMenuItem( int menu_item );

	bool m_fMenuDisplayed;
	int m_bitsValidSlots;
	float m_flShutoffTime;
	bool m_fWaitingForMore;

private:
	char m_szMenuString[ MAX_MENU_STRING ] = {};
	char m_szPrelocalisedMenuString[ MAX_MENU_STRING ] = {};
};

#endif //GAME_CLIENT_UI_HUD_CHUDMENU_H