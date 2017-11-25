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
#ifndef GAME_SERVER_ENTITIES_EFFECTS_CBLOOD_H
#define GAME_SERVER_ENTITIES_EFFECTS_CBLOOD_H

#define SF_BLOOD_RANDOM		0x0001
#define SF_BLOOD_STREAM		0x0002
#define SF_BLOOD_PLAYER		0x0004
#define SF_BLOOD_DECAL		0x0008

// Blood effects
class CBlood : public CPointEntity
{
public:
	DECLARE_CLASS( CBlood, CPointEntity );

	void	Spawn( void ) override;
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void	KeyValue( KeyValueData *pkvd ) override;

	inline	int		Color( void ) { return GetImpulse(); }
	inline	float 	BloodAmount( void ) { return GetDamage(); }

	inline	void SetColor( int color ) { SetImpulse( color ); }
	inline	void SetBloodAmount( float amount ) { SetDamage( amount ); }

	Vector	Direction( void );
	Vector	BloodPosition( CBaseEntity *pActivator );

private:
};

#endif //GAME_SERVER_ENTITIES_EFFECTS_CBLOOD_H