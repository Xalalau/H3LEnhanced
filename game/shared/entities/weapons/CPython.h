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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CPYTHON_H
#define GAME_SHARED_ENTITIES_WEAPONS_CPYTHON_H

enum python_e
{
	PYTHON_IDLE1 = 0,
	PYTHON_FIDGET,
	PYTHON_FIRE1,
	PYTHON_RELOAD,
	PYTHON_HOLSTER,
	PYTHON_DRAW,
	PYTHON_IDLE2,
	PYTHON_IDLE3
};

class CPython : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CPython, CBasePlayerWeapon );

	CPython();

	void Spawn( void ) override;
	void Precache( void ) override;
	bool AddToPlayer( CBasePlayer *pPlayer ) override;
	void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	bool Deploy() override;
	void Holster() override;
	void Reload( void ) override;
	void WeaponIdle( void ) override;

	bool m_fInZoom;// don't save this. 

	// ############ hu3lifezado ############ //
	// [Terceira Pessoa]
	// Funcoes para renderizar a mira em terceira pessoa
	void ItemPreFrame(void);
	// ############ //

private:
	unsigned short m_usFirePython;

	// ############ hu3lifezado ############ //
	// [Terceira Pessoa]
	// Mira em terceira pessoa
#ifndef CLIENT_DLL
	CHu3XSpot* m_pLaser;
#endif
	// ############ //
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CPYTHON_H