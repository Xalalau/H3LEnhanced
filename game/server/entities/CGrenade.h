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
#ifndef GAME_SERVER_ENTITIES_CGRENADE_H
#define GAME_SERVER_ENTITIES_CGRENADE_H

// Contact Grenade / Timed grenade / Satchel Charge
class CGrenade : public CBaseMonster
{
public:
	DECLARE_CLASS( CGrenade, CBaseMonster );
#ifndef CLIENT_DLL
	DECLARE_DATADESC();
#endif

	void Spawn( void ) override;

	static CGrenade* GrenadeCreate();

	static CGrenade* ShootTimed( CBaseEntity* pOwner, Vector vecStart, Vector vecVelocity, float time );
	static CGrenade* ShootContact( CBaseEntity* pOwner, Vector vecStart, Vector vecVelocity );

	void Explode( Vector vecSrc, Vector vecAim );
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void Smoke( void );

	void BounceTouch( CBaseEntity *pOther );
	void SlideTouch( CBaseEntity *pOther );
	void ExplodeTouch( CBaseEntity *pOther );
	void DangerSoundThink( void );
	void PreDetonate( void );
	void Detonate( void );
	void DetonateUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void TumbleThink( void );

	virtual void BounceSound( void );
	virtual int	BloodColor() const override { return DONT_BLEED; }
	virtual void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;

	bool m_fRegisteredSound;// whether or not this grenade has issued its DANGER sound to the world sound list yet.
};

#endif //GAME_SERVER_ENTITIES_CGRENADE_H