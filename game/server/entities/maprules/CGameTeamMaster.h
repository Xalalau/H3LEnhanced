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
#ifndef GAME_SERVER_ENTITIES_MAPRULES_CGAMETEAMMASTER_H
#define GAME_SERVER_ENTITIES_MAPRULES_CGAMETEAMMASTER_H

#include "CRulePointEntity.h"

#define SF_TEAMMASTER_FIREONCE			0x0001
#define SF_TEAMMASTER_ANYTEAM			0x0002

//
// CGameTeamMaster / game_team_master	-- "Masters" like multisource, but based on the team of the activator
// Only allows mastered entity to fire if the team matches my team
//
// team index (pulled from server team list "mp_teamlist"
// Flag: Remove on Fire
// Flag: Any team until set?		-- Any team can use this until the team is set (otherwise no teams can use it)
//

class CGameTeamMaster : public CRulePointEntity
{
public:
	DECLARE_CLASS( CGameTeamMaster, CRulePointEntity );

	void		KeyValue( KeyValueData *pkvd ) override;
	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	int			ObjectCaps() const override { return CRulePointEntity::ObjectCaps() | FCAP_MASTER; }

	bool		IsTriggered( const CBaseEntity* const pActivator ) const override;
	const char	*TeamID() const override;
	inline bool RemoveOnFire() const { return GetSpawnFlags().Any( SF_TEAMMASTER_FIREONCE ); }
	inline bool AnyTeam() const { return GetSpawnFlags().Any( SF_TEAMMASTER_ANYTEAM ); }

private:
	bool		TeamMatch( const CBaseEntity* const pActivator ) const;

	int			m_teamIndex;
	USE_TYPE	triggerType;
};

#endif //GAME_SERVER_ENTITIES_MAPRULES_CGAMETEAMMASTER_H