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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CCROSSBOW_H
#define GAME_SHARED_ENTITIES_WEAPONS_CCROSSBOW_H

enum crossbow_e
{
	CROSSBOW_IDLE1 = 0,	// full
	CROSSBOW_IDLE2,		// empty
	CROSSBOW_FIDGET1,	// full
	CROSSBOW_FIDGET2,	// empty
	CROSSBOW_FIRE1,		// full
	CROSSBOW_FIRE2,		// reload
	CROSSBOW_FIRE3,		// empty
	CROSSBOW_RELOAD,	// from empty
	CROSSBOW_DRAW1,		// full
	CROSSBOW_DRAW2,		// empty
	CROSSBOW_HOLSTER1,	// full
	CROSSBOW_HOLSTER2,	// empty
};

class CCrossbow : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CCrossbow, CBasePlayerWeapon );

	CCrossbow();

	void Spawn( void ) override;
	void Precache( void ) override;

	void FireBolt( void );
	void FireSniperBolt( void );
	void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	bool AddToPlayer( CBasePlayer *pPlayer ) override;
	bool Deploy() override;
	void Holster() override;
	void Reload( void ) override;
	void WeaponIdle( void ) override;

	int m_fInZoom; // don't save this

	// ############ hu3lifezado ############ //
	// [Terceira Pessoa]
	// Funcoes para renderizar a mira em terceira pessoa
	void ItemPreFrame(void);
	// ############ //

private:
	unsigned short m_usCrossbow;
	unsigned short m_usCrossbow2;

	// ############ hu3lifezado ############ //
	// [Terceira Pessoa]
	// Mira em terceira pessoa
#ifndef CLIENT_DLL
	CHu3XSpot* m_pLaser;
#endif
	// ############ //
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CCROSSBOW_H