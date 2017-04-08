/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#ifndef GAME_SERVER_ENTITIES_NPCS_CAPACHE_H
#define GAME_SERVER_ENTITIES_NPCS_CAPACHE_H

#define SF_WAITFORTRIGGER	(0x04 | 0x40) // UNDONE: Fix!
#define SF_NOWRECKAGE		0x08

class CApache : public CBaseMonster
{
public:
	DECLARE_CLASS( CApache, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	EntityClassification_t GetClassification() override { return EntityClassifications().GetClassificationId( classify::HUMAN_MILITARY ); }
	int  BloodColor() const override { return DONT_BLEED; }
	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;
	void GibMonster( void ) override;

	void SetObjectCollisionBox( void ) override
	{
		pev->absmin = GetAbsOrigin() + Vector( -300, -300, -172 );
		pev->absmax = GetAbsOrigin() + Vector( 300, 300, 8 );
	}

	void HuntThink( void );
	void FlyTouch( CBaseEntity *pOther );
	void CrashTouch( CBaseEntity *pOther );
	void DyingThink( void );
	void StartupUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void NullThink( void );

	void ShowDamage( void );
	void Flight( void );
	void FireRocket( void );
	bool FireGun();

	void OnTakeDamage( const CTakeDamageInfo& info ) override;
	void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr ) override;

	int m_iRockets;
	float m_flForce;
	float m_flNextRocket;

	Vector m_vecTarget;
	Vector m_posTarget;

	Vector m_vecDesired;
	Vector m_posDesired;

	Vector m_vecGoal;

	Vector m_angGun;
	float m_flLastSeen;
	float m_flPrevSeen;

	int m_iSoundState; // don't save this

	int m_iSpriteTexture;
	int m_iExplode;
	int m_iBodyGibs;

	float m_flGoalSpeed;

	int m_iDoSmokePuff;
	CBeam *m_pBeam;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CAPACHE_H