/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
#ifndef GAME_SERVER_ENTITIES_MAPRULES_CGAMEPLAYERTEAM_H
#define GAME_SERVER_ENTITIES_MAPRULES_CGAMEPLAYERTEAM_H

#include "CRulePointEntity.h"

#define SF_PTEAM_FIREONCE			0x0001
#define SF_PTEAM_KILL    			0x0002
#define SF_PTEAM_GIB     			0x0004

//
// CGamePlayerTeam / game_player_team	-- Changes the team of the player who fired it
// Flag: Fire once
// Flag: Kill Player
// Flag: Gib Player

class CGamePlayerTeam : public CRulePointEntity
{
public:
	DECLARE_CLASS( CGamePlayerTeam, CRulePointEntity );

	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;

private:

	inline bool RemoveOnFire() const { return GetSpawnFlags().Any( SF_PTEAM_FIREONCE ); }
	inline bool ShouldKillPlayer() const { return GetSpawnFlags().Any( SF_PTEAM_KILL ); }
	inline bool ShouldGibPlayer() const { return GetSpawnFlags().Any( SF_PTEAM_GIB ); }

	const char *TargetTeamName( const char *pszTargetName );
};

#endif //GAME_SERVER_ENTITIES_MAPRULES_CGAMEPLAYERTEAM_H