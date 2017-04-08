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
#ifndef GAME_CLIENT_UI_HUD_CHUDAMMOSECONDARY_H
#define GAME_CLIENT_UI_HUD_CHUDAMMOSECONDARY_H

#include "shared/hud/CHudElement.h"
#include "hud.h"

class CHudAmmoSecondary : public CBaseHudElement<CHLHud>
{
public:
	DECLARE_CLASS( CHudAmmoSecondary, CBaseHudElement<CHLHud> );

	CHudAmmoSecondary( const char* const pszName, CHLHud& hud );

	void Init() override;
	void VidInit() override;
	void Reset() override;
	bool Draw( float flTime ) override;

	void MsgFunc_SecAmmoVal( const char *pszName, int iSize, void *pbuf );
	void MsgFunc_SecAmmoIcon( const char *pszName, int iSize, void *pbuf );

private:
	enum {
		MAX_SEC_AMMO_VALUES = 4
	};

	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[ MAX_SEC_AMMO_VALUES ];
	float m_fFade;
};

#endif //GAME_CLIENT_UI_HUD_CHUDAMMOSECONDARY_H
