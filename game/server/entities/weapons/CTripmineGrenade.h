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
#ifndef GAME_SERVER_ENTITIES_WEAPONS_CTRIPMINEGRENADE_H
#define GAME_SERVER_ENTITIES_WEAPONS_CTRIPMINEGRENADE_H

enum
{
	SF_TRIPMINE_INSTANT_ON = 1 << 0, //! Instantly turn on after spawning
};

class CTripmineGrenade : public CGrenade
{
public:
	DECLARE_CLASS( CTripmineGrenade, CGrenade );
	DECLARE_DATADESC();

	void Spawn() override;
	void Precache() override;

	void UpdateOnRemove() override;

	void OnTakeDamage( const CTakeDamageInfo& info ) override;

	void WarningThink();
	void PowerupThink();
	void BeamBreakThink();
	void DelayDeathThink();
	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;

	void MakeBeam();
	void KillBeam();

	float		m_flPowerUp;
	Vector		m_vecDir;
	Vector		m_vecEnd;
	float		m_flBeamLength;

	EHANDLE		m_hOwner;
	CBeam		*m_pBeam;
	Vector		m_posOwner;
	Vector		m_angleOwner;
	edict_t		*m_pRealOwner;// tracelines don't hit PEV->OWNER, which means a player couldn't detonate his own trip mine, so we store the owner here.
};

#endif //GAME_SERVER_ENTITIES_WEAPONS_CTRIPMINEGRENADE_H
