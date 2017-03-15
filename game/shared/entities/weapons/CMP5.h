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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CMP5_H
#define GAME_SHARED_ENTITIES_WEAPONS_CMP5_H

enum mp5_e
{
	MP5_LONGIDLE = 0,
	MP5_IDLE1,
	MP5_LAUNCH,
	MP5_RELOAD,
	MP5_DEPLOY,
	MP5_FIRE1,
	MP5_FIRE2,
	MP5_FIRE3,
};

class CMP5 : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CMP5, CBasePlayerWeapon );

	CMP5();

	void Spawn( void ) override;
	void Precache( void ) override;
	bool AddToPlayer( CBasePlayer *pPlayer ) override;

	void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	bool Deploy() override;
	void Reload( void ) override;
	void WeaponIdle( void ) override;

	// ############ hu3lifezado ############ //
	// Funcao de CBasePlayerWeapon usada para renderizar a mira em terceira pessoa
	void ItemPreFrame(void);
	// Sprite da mira em terceira pessoa
	int hu3_spriteTexture;
	// ############ //

	bool IsUseable() override;

	float m_flNextAnimTime;
	int m_iShell;

private:
	unsigned short m_usMP5;
	unsigned short m_usMP52;
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CMP5_H