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
#ifndef GAME_SERVER_ENTITIES_CYCLER_CCYCLER_H
#define GAME_SERVER_ENTITIES_CYCLER_CCYCLER_H

class CCycler : public CBaseMonster
{
public:
	DECLARE_CLASS( CCycler, CBaseMonster );
	DECLARE_DATADESC();

	void GenericCyclerSpawn( const char* szModel, Vector vecMin, Vector vecMax );
	virtual int	ObjectCaps() const override { return ( CBaseEntity::ObjectCaps() | FCAP_IMPULSE_USE ); }
	void OnTakeDamage( const CTakeDamageInfo& info ) override;
	void Spawn() override;
	void Think() override;
	//void Pain( float flDamage );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;

	// Don't treat as a live target
	virtual bool IsAlive() const override { return false; }

	int			m_animate;
};

#endif //GAME_SERVER_ENTITIES_CYCLER_CCYCLER_H