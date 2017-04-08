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
#ifndef GAME_SERVER_ENTITIES_NPCS_CBASETURRET_H
#define GAME_SERVER_ENTITIES_NPCS_CBASETURRET_H

#define TURRET_SHOTS	2
#define TURRET_RANGE	(100 * 12)
#define TURRET_SPREAD	Vector( 0, 0, 0 )
#define TURRET_TURNRATE	30		//angles per 0.1 second
#define TURRET_MAXWAIT	15		// seconds turret will stay active w/o a target
#define TURRET_MAXSPIN	5		// seconds turret barrel will spin w/o a target
#define TURRET_MACHINE_VOLUME	0.5

enum TURRET_ANIM
{
	TURRET_ANIM_NONE = 0,
	TURRET_ANIM_FIRE,
	TURRET_ANIM_SPIN,
	TURRET_ANIM_DEPLOY,
	TURRET_ANIM_RETIRE,
	TURRET_ANIM_DIE,
};

enum BaseTurretSpawnFlag
{
	SF_MONSTER_TURRET_AUTOACTIVATE	= 32,
	SF_MONSTER_TURRET_STARTINACTIVE	= 64,
};

#define TURRET_GLOW_SPRITE "sprites/flare3.spr"

class CBaseTurret : public CBaseMonster
{
public:
	DECLARE_CLASS( CBaseTurret, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	virtual void Precache( void ) override;

	void UpdateOnRemove() override;

	void KeyValue( KeyValueData *pkvd ) override;
	void TurretUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr ) override;
	virtual void OnTakeDamage( const CTakeDamageInfo& info ) override;
	virtual EntityClassification_t GetClassification() override;

	int BloodColor() const override { return DONT_BLEED; }
	void GibMonster( void ) override {}	// UNDONE: Throw turret gibs?

										// Think functions

	void ActiveThink( void );
	void SearchThink( void );
	void AutoSearchThink( void );
	void TurretDeath( void );

	virtual void SpinDownCall( void ) { m_iSpin = 0; }
	virtual void SpinUpCall( void ) { m_iSpin = 1; }

	// void SpinDown(void);
	// float SpinDownCall( void ) { return SpinDown(); }

	// virtual float SpinDown(void) { return 0;}
	// virtual float Retire(void) { return 0;}

	void Deploy( void );
	void Retire( void );

	void Initialize( void );

	virtual void Ping( void );
	virtual void EyeOn( void );
	virtual void EyeOff( void );

	// other functions
	void SetTurretAnim( TURRET_ANIM anim );
	int MoveTurret( void );
	virtual void Shoot( Vector &vecSrc, Vector &vecDirToEnemy ) { };

	float m_flMaxSpin;		// Max time to spin the barrel w/o a target
	int m_iSpin;

	CSprite *m_pEyeGlow;
	int		m_eyeBrightness;

	int	m_iDeployHeight;
	int	m_iRetractHeight;
	int m_iMinPitch;

	int m_iBaseTurnRate;	// angles per second
	float m_fTurnRate;		// actual turn rate
	int m_iOrientation;		// 0 = floor, 1 = Ceiling
	bool m_bOn;
	bool m_fBeserk;			// Sometimes this bitch will just freak out
	bool m_bAutoStart;		// true if the turret auto deploys when a target
							// enters its range

	Vector m_vecLastSight;
	float m_flLastSight;	// Last time we saw a target
	float m_flMaxWait;		// Max time to seach w/o a target
	int m_iSearchSpeed;		// Not Used!

							// movement
	float	m_flStartYaw;
	Vector	m_vecCurAngles;
	Vector	m_vecGoalAngles;


	float	m_flPingTime;	// Time until the next ping, used when searching
	float	m_flSpinUpTime;	// Amount of time until the barrel should spin down when searching
};

#endif //GAME_SERVER_ENTITIES_NPCS_CBASETURRET_H