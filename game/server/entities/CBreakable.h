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
#ifndef FUNC_BREAK_H
#define FUNC_BREAK_H

#define SF_BREAK_TRIGGER_ONLY	1// may only be broken by trigger
#define	SF_BREAK_TOUCH			2// can be 'crashed through' by running player (plate glass)
#define SF_BREAK_PRESSURE		4// can be broken by a player standing on it
#define SF_BREAK_CROWBAR		256// instant break if hit with crowbar

enum Explosions
{
	expRandom,
	expDirected
};

enum Materials
{
	matGlass = 0,
	matWood,
	matMetal,
	matFlesh,
	matCinderBlock,
	matCeilingTile,
	matComputer,
	matUnbreakableGlass,
	matRocks,
	matNone,
	matLastMaterial
};

#define	NUM_SHARDS 6 // this many shards spawned when breakable objects break;

//
// func_breakable - bmodel that breaks into pieces after taking damage
//
class CBreakable : public CBaseDelay
{
public:
	DECLARE_CLASS( CBreakable, CBaseDelay );
	DECLARE_DATADESC();

	// basic functions
	void Spawn( void ) override;
	void Precache( void ) override;
	void KeyValue( KeyValueData* pkvd) override;
	void BreakTouch( CBaseEntity *pOther );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void DamageSound( void );

	// breakables use an overridden takedamage
	virtual void OnTakeDamage( const CTakeDamageInfo& info ) override;
	// To spark when hit
	void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr ) override;

	bool IsBreakable() const;
	bool SparkWhenHit() const;

	int	 DamageDecal( int bitsDamageType ) const override;

	void 	Die( void );
	virtual int		ObjectCaps() const override { return ( CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ); }

	inline bool		Explodable() const { return ExplosionMagnitude() > 0; }
	inline int		ExplosionMagnitude() const { return GetImpulse(); }
	inline void		ExplosionSetMagnitude( int magnitude ) { SetImpulse( magnitude ); }

	static void MaterialSoundPrecache( Materials precacheMaterial );
	static void MaterialSoundRandom( CBaseEntity* pEntity, Materials soundMaterial, float volume );
	static const char **MaterialSoundList( Materials precacheMaterial, int &soundCount );

	static const char *pSoundsWood[];
	static const char *pSoundsFlesh[];
	static const char *pSoundsGlass[];
	static const char *pSoundsMetal[];
	static const char *pSoundsConcrete[];
	static const char *pSpawnObjects[];

	Materials	m_Material;
	Explosions	m_Explosion;
	int			m_idShard;
	float		m_angle;
	int			m_iszGibModel;
	int			m_iszSpawnObject;
};

#endif	// FUNC_BREAK_H
