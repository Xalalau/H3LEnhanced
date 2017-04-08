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
#ifndef GAME_CLIENT_UI_HUD_CHUDGEIGER_H
#define GAME_CLIENT_UI_HUD_CHUDGEIGER_H

#include "shared/hud/CHudElement.h"
#include "hud.h"

class CHudGeiger : public CBaseHudElement<CHLHud>
{
public:
	DECLARE_CLASS( CHudGeiger, CBaseHudElement<CHLHud> );

	CHudGeiger( const char* const pszName, CHLHud& hud );

	void Init() override;
	void VidInit() override;
	bool Draw( float flTime ) override;
	void MsgFunc_Geiger( const char *pszName, int iSize, void *pbuf );

private:
	int m_iGeigerRange;

};

#endif //GAME_CLIENT_UI_HUD_CHUDGEIGER_H