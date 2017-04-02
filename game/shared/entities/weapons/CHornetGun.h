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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CHORNETGUN_H
#define GAME_SHARED_ENTITIES_WEAPONS_CHORNETGUN_H

enum hgun_e
{
	HGUN_IDLE1 = 0,
	HGUN_FIDGETSWAY,
	HGUN_FIDGETSHAKE,
	HGUN_DOWN,
	HGUN_UP,
	HGUN_SHOOT
};

class CHornetGun : public CBasePlayerWeapon
{
public:
	enum firemode_e
	{
		FIREMODE_TRACK = 0,
		FIREMODE_FAST
	};

public:
	DECLARE_CLASS( CHornetGun, CBasePlayerWeapon );
	DECLARE_DATADESC();

	CHornetGun();

	void Spawn( void ) override;
	void Precache( void ) override;
	bool AddToPlayer( CBasePlayer *pPlayer ) override;

	void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	bool Deploy() override;
	bool IsUseable() override;
	void Holster() override;
	void Reload( void ) override;
	void WeaponIdle( void ) override;
	float m_flNextAnimTime;

	float m_flRechargeTime;

	int m_iFirePhase;// don't save me.

	// ############ hu3lifezado ############ //
	// Funcoes para renderizar a mira em terceira pessoa
	void ItemPreFrame(void);
	void UpdateSpot();
	void RemoveSpot();
	// ############ //

private:
	unsigned short m_usHornetFire;

	// ############ hu3lifezado ############ //
	// Mira em terceira pessoa
	CHu3XSpot* m_pLaser;
	// ############ //
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CHORNETGUN_H