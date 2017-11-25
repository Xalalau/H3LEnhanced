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
#ifndef GAME_SERVER_ENTITIES_TANKS_CFUNCTANK_H
#define GAME_SERVER_ENTITIES_TANKS_CFUNCTANK_H

#define SF_TANK_ACTIVE			0x0001
#define SF_TANK_PLAYER			0x0002
#define SF_TANK_HUMANS			0x0004
#define SF_TANK_ALIENS			0x0008
#define SF_TANK_LINEOFSIGHT		0x0010
#define SF_TANK_CANCONTROL		0x0020
#define SF_TANK_SOUNDON			0x8000

enum TANKBULLET
{
	TANK_BULLET_NONE = 0,
	TANK_BULLET_9MM = 1,
	TANK_BULLET_MP5 = 2,
	TANK_BULLET_12MM = 3,
};

extern const Vector gTankSpread[];

//			Custom damage
//			env_laser (duration is 0.5 rate of fire)
//			rockets
//			explosion?
class CFuncTank : public CBaseEntity
{
public:
	DECLARE_CLASS( CFuncTank, CBaseEntity );
	DECLARE_DATADESC();

	void	Spawn( void ) override;
	void	Precache( void ) override;
	void	KeyValue( KeyValueData *pkvd ) override;
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void	Think( void ) override;
	void	TrackTarget( void );

	virtual void Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity* pAttacker );
	virtual Vector UpdateTargetPosition( CBaseEntity *pTarget )
	{
		return pTarget->BodyTarget( GetAbsOrigin() );
	}

	void	StartRotSound( void );
	void	StopRotSound( void );

	// Bmodels don't go across transitions
	virtual int	ObjectCaps() const override { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	inline bool IsActive() const { return GetSpawnFlags().Any( SF_TANK_ACTIVE ); }
	inline void TankActivate( void ) { GetSpawnFlags() |= SF_TANK_ACTIVE; SetNextThink( GetLastThink() + 0.1 ); m_fireLast = 0; }
	inline void TankDeactivate( void ) { GetSpawnFlags().ClearFlags( SF_TANK_ACTIVE ); m_fireLast = 0; StopRotSound(); }
	inline bool CanFire() const { return ( gpGlobals->time - m_lastSightTime ) < m_persist; }
	bool		InRange( float range ) const;

	// Acquire a target.  pPlayer is a player in the PVS
	CBaseEntity* FindTarget( CBaseEntity* pPlayer );

	void		TankTrace( const Vector &vecStart, const Vector &vecForward, const Vector &vecSpread, TraceResult &tr );

	Vector		BarrelPosition( void )
	{
		Vector forward, right, up;
		UTIL_MakeVectorsPrivate( GetAbsAngles(), &forward, &right, &up );
		return GetAbsOrigin() + ( forward * m_barrelPos.x ) + ( right * m_barrelPos.y ) + ( up * m_barrelPos.z );
	}

	void		AdjustAnglesForBarrel( Vector &angles, float distance );

	bool OnControls( const CBaseEntity* const pTest ) const override;
	bool StartControl( CBasePlayer* pController );
	void StopControl( void );
	void ControllerPostFrame( void );


protected:
	CBasePlayer* m_pController;
	float		m_flNextAttack;
	Vector		m_vecControllerUsePos;

	float		m_yawCenter;	// "Center" yaw
	float		m_yawRate;		// Max turn rate to track targets
	float		m_yawRange;		// Range of turning motion (one-sided: 30 is +/- 30 degress from center)
								// Zero is full rotation
	float		m_yawTolerance;	// Tolerance angle

	float		m_pitchCenter;	// "Center" pitch
	float		m_pitchRate;	// Max turn rate on pitch
	float		m_pitchRange;	// Range of pitch motion as above
	float		m_pitchTolerance;	// Tolerance angle

	float		m_fireLast;		// Last time I fired
	float		m_fireRate;		// How many rounds/second
	float		m_lastSightTime;// Last time I saw target
	float		m_persist;		// Persistence of firing (how long do I shoot when I can't see)
	float		m_minRange;		// Minimum range to aim/track
	float		m_maxRange;		// Max range to aim/track

	Vector		m_barrelPos;	// Length of the freakin barrel
	float		m_spriteScale;	// Scale of any sprites we shoot
	int			m_iszSpriteSmoke;
	int			m_iszSpriteFlash;
	TANKBULLET	m_bulletType;	// Bullet type
	int			m_iBulletDamage; // 0 means use Bullet type's default damage

	Vector		m_sightOrigin;	// Last sight of target
	int			m_spread;		// firing spread
	int			m_iszMaster;	// Master entity (game_team_master or multisource)
};

#endif //GAME_SERVER_ENTITIES_TANKS_CFUNCTANK_H