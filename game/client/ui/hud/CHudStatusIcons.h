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
#ifndef GAME_CLIENT_UI_HUD_CHUDSTATUSICONS_H
#define GAME_CLIENT_UI_HUD_CHUDSTATUSICONS_H

#include "shared/hud/CHudElement.h"
#include "hud.h"

class CHudStatusIcons : public CBaseHudElement<CHLHud>
{
public:
	DECLARE_CLASS( CHudStatusIcons, CBaseHudElement<CHLHud> );

	CHudStatusIcons( const char* const pszName, CHLHud& hud );

	void Init() override;
	void VidInit() override;
	void Reset() override;
	bool Draw( float flTime ) override;
	void MsgFunc_StatusIcon( const char *pszName, int iSize, void *pbuf );

	enum {
		MAX_ICONSPRITENAME_LENGTH = MAX_SPRITE_NAME_LENGTH,
		MAX_ICONSPRITES = 4,
	};


	//had to make these public so CHLHud could access them (to enable concussion icon)
	//could use a friend declaration instead...
	void EnableIcon( const char* const pszIconName, unsigned char red, unsigned char green, unsigned char blue );
	void DisableIcon( const char* const pszIconName );

private:

	struct icon_sprite_t
	{
		char szSpriteName[ MAX_ICONSPRITENAME_LENGTH ];
		HSPRITE spr;
		wrect_t rc;
		unsigned char r, g, b;
	};

	icon_sprite_t m_IconList[ MAX_ICONSPRITES ];

};

#endif //GAME_CLIENT_UI_HUD_CHUDSTATUSICONS_H