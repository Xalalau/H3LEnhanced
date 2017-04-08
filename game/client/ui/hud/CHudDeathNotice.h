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
#ifndef GAME_CLIENT_UI_HUD_CHUDDEATHNOTICE_H
#define GAME_CLIENT_UI_HUD_CHUDDEATHNOTICE_H

#include "shared/hud/CHudElement.h"
#include "hud.h"

class CHudDeathNotice : public CBaseHudElement<CHLHud>
{
private:
	struct DeathNoticeItem
	{
		char szKiller[ MAX_PLAYER_NAME_LENGTH * 2 ];
		char szVictim[ MAX_PLAYER_NAME_LENGTH * 2 ];
		int iId;	// the index number of the associated sprite
		bool bSuicide;
		bool bTeamKill;
		bool bNonPlayerKill;
		float flDisplayTime;
		const Vector* KillerColor;
		const Vector* VictimColor;
	};

	static const size_t MAX_DEATHNOTICES = 4;

public:
	DECLARE_CLASS( CHudDeathNotice, CBaseHudElement<CHLHud> );

	CHudDeathNotice( const char* const pszName, CHLHud& hud );

	void Init() override;
	void InitHUDData() override;
	void VidInit() override;
	bool Draw( float flTime ) override;
	void MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf );

private:
	int m_HUD_d_skull;  // sprite index of skull icon

	cvar_t* m_phud_deathnotice_time = nullptr;

	DeathNoticeItem m_rgDeathNoticeList[ MAX_DEATHNOTICES + 1 ];
};

#endif //GAME_CLIENT_UI_HUD_CHUDDEATHNOTICE_H