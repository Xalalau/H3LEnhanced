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
#ifndef GAME_SERVER_ENTITIES_NPCS_CNIHILANTH_H
#define GAME_SERVER_ENTITIES_NPCS_CNIHILANTH_H

#define N_SCALE		15
#define N_SPHERES	20

class CNihilanth : public CBaseMonster
{
public:
	DECLARE_CLASS( CNihilanth, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;

	void UpdateOnRemove() override;

	EntityClassification_t GetClassification() override { return EntityClassifications().GetClassificationId( classify::ALIEN_MILITARY ); }
	int  BloodColor() const override { return BLOOD_COLOR_YELLOW; }
	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;
	void GibMonster( void ) override;

	void SetObjectCollisionBox( void ) override
	{
		pev->absmin = GetAbsOrigin() + Vector( -16 * N_SCALE, -16 * N_SCALE, -48 * N_SCALE );
		pev->absmax = GetAbsOrigin() + Vector( 16 * N_SCALE, 16 * N_SCALE, 28 * N_SCALE );
	}

	void HandleAnimEvent( AnimEvent_t& event ) override;

	void StartupThink( void );
	void HuntThink( void );
	void CrashTouch( CBaseEntity *pOther );
	void DyingThink( void );
	void StartupUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void NullThink( void );
	void CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void FloatSequence( void );
	void NextActivity( void );

	void Flight( void );

	bool AbsorbSphere();
	bool EmitSphere();
	void TargetSphere( USE_TYPE useType, float value );
	void ShootBalls( void );
	void MakeFriend( Vector vecPos );

	void OnTakeDamage( const CTakeDamageInfo& info ) override;
	void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr ) override;

	void PainSound( void ) override;
	void DeathSound( void ) override;

	static const char *pAttackSounds[];	// vocalization: play sometimes when he launches an attack
	static const char *pBallSounds[];	// the sound of the lightening ball launch
	static const char *pShootSounds[];	// grunting vocalization: play sometimes when he launches an attack
	static const char *pRechargeSounds[];	// vocalization: play when he recharges
	static const char *pLaughSounds[];	// vocalization: play sometimes when hit and still has lots of health
	static const char *pPainSounds[];	// vocalization: play sometimes when hit and has much less health and no more chargers
	static const char *pDeathSounds[];	// vocalization: play as he dies

										// x_teleattack1.wav	the looping sound of the teleport attack ball.

	float m_flForce;

	float m_flNextPainSound;

	Vector m_velocity;
	Vector m_avelocity;

	Vector m_vecTarget;
	Vector m_posTarget;

	Vector m_vecDesired;
	Vector m_posDesired;

	float  m_flMinZ;
	float  m_flMaxZ;

	Vector m_vecGoal;

	float m_flLastSeen;
	float m_flPrevSeen;

	int m_irritation;

	int m_iLevel;
	int m_iTeleport;

	EHANDLE m_hRecharger;

	EHANDLE m_hSphere[ N_SPHERES ];
	int	m_iActiveSpheres;

	float m_flAdj;

	CSprite *m_pBall;

	char m_szRechargerTarget[ 64 ];
	char m_szDrawUse[ 64 ];
	char m_szTeleportUse[ 64 ];
	char m_szTeleportTouch[ 64 ];
	char m_szDeadUse[ 64 ];
	char m_szDeadTouch[ 64 ];

	float m_flShootEnd;
	float m_flShootTime;

	EHANDLE m_hFriend[ 3 ];
};

#endif //GAME_SERVER_ENTITIES_NPCS_CNIHILANTH_H