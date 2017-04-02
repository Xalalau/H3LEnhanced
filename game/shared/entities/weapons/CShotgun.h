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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CSHOTGUN_H
#define GAME_SHARED_ENTITIES_WEAPONS_CSHOTGUN_H

enum shotgun_e
{
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP
};

class CShotgun : public CBasePlayerWeapon
{
public:
	enum class ReloadState
	{
		NOT_RELOADING = 0,
		DO_RELOAD_EFFECTS,
		RELOAD_ONE
	};

public:
	DECLARE_CLASS( CShotgun, CBasePlayerWeapon );
#ifndef CLIENT_DLL
	DECLARE_DATADESC();
#endif

	CShotgun();

	void Spawn( void ) override;
	void Precache( void ) override;
	bool AddToPlayer( CBasePlayer *pPlayer ) override;

	void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	bool Deploy() override;
	void Reload( void ) override;

	void WeaponTick() override;

	void WeaponIdle( void ) override;

	float m_flNextReload;
	int m_iShell;

	// ############ hu3lifezado ############ //
	// Funcoes para renderizar a mira em terceira pessoa
	void ItemPreFrame(void);
	void UpdateSpot();
	void RemoveSpot();
	void Holster() override;
	// ############ //

	void GetWeaponData( weapon_data_t& data ) override
	{
		BaseClass::GetWeaponData( data );

		data.m_fInSpecialReload = static_cast<int>( m_InSpecialReload );

		//data.m_flPumpTime		= max( m_flPumpTime, -0.001 );
	}

	void SetWeaponData( const weapon_data_t& data ) override
	{
		BaseClass::SetWeaponData( data );

		m_InSpecialReload = static_cast<ReloadState>( data.m_fInSpecialReload );
		//m_flPumpTime		= data.m_flPumpTime;
	}

private:
	unsigned short m_usDoubleFire;
	unsigned short m_usSingleFire;

	ReloadState m_InSpecialReload;		// Are we in the middle of a reload for the shotguns
	float m_flPumpTime;

	// ############ hu3lifezado ############ //
	// Mira em terceira pessoa
	CHu3XSpot* m_pLaser;
	// ############ //
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CSHOTGUN_H