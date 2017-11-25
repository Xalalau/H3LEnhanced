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
#ifndef GAME_SERVER_ENTITIES_EFFECTS_CSHAKE_H
#define GAME_SERVER_ENTITIES_EFFECTS_CSHAKE_H

#define SF_SHAKE_EVERYONE	0x0001		// Don't check radius
// UNDONE: These don't work yet
#define SF_SHAKE_DISRUPT	0x0002		// Disrupt controls
#define SF_SHAKE_INAIR		0x0004		// Shake players in air

// Screen shake
// GetScale() is amplitude
// pev->dmg_save is frequency
// pev->dmg_take is duration
// GetDamage() is radius
// radius of 0 means all players
// NOTE: UTIL_ScreenShake() will only shake players who are on the ground
class CShake : public CPointEntity
{
public:
	DECLARE_CLASS( CShake, CPointEntity );

	void	Spawn( void ) override;
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void	KeyValue( KeyValueData *pkvd ) override;

	inline	float	Amplitude( void ) { return GetScale(); }
	inline	float	Frequency( void ) { return pev->dmg_save; }
	inline	float	Duration( void ) { return pev->dmg_take; }
	inline	float	Radius( void ) { return GetDamage(); }

	inline	void	SetAmplitude( float amplitude ) { SetScale( amplitude ); }
	inline	void	SetFrequency( float frequency ) { pev->dmg_save = frequency; }
	inline	void	SetDuration( float duration ) { pev->dmg_take = duration; }
	inline	void	SetRadius( float radius ) { SetDamage( radius ); }
private:
};

#endif //GAME_SERVER_ENTITIES_EFFECTS_CSHAKE_H