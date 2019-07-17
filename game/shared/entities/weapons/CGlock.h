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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CGLOCK_H
#define GAME_SHARED_ENTITIES_WEAPONS_CGLOCK_H

enum glock_e
{
	GLOCK_IDLE1 = 0,
	GLOCK_IDLE2,
	GLOCK_IDLE3,
	GLOCK_SHOOT,
	GLOCK_SHOOT_EMPTY,
	GLOCK_RELOAD,
	GLOCK_RELOAD_NOT_EMPTY,
	GLOCK_DRAW,
	GLOCK_HOLSTER,
	GLOCK_ADD_SILENCER
};

class CGlock : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CGlock, CBasePlayerWeapon );

	CGlock();

	void Spawn( void ) override;
	void Precache( void ) override;

	void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	void GlockFire( float flSpread, float flCycleTime, const bool fUseAutoAim );
	bool Deploy() override;
	void Reload( void ) override;
	void WeaponIdle( void ) override;

	// ############ hu3lifezado ############ //
	// [Terceira Pessoa]
	// Funcoes para renderizar a mira em terceira pessoa
	void ItemPreFrame(void);
	void Holster() override;
	// ############ //

private:
	int m_iShell;

	// ############ hu3lifezado ############ //
	// [Terceira Pessoa]
	// Mira em terceira pessoa
#ifndef CLIENT_DLL
	CHu3XSpot* m_pLaser;
#endif
	// ############ //

	unsigned short m_usFireGlock1;
	unsigned short m_usFireGlock2;
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CGLOCK_H