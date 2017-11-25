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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CTRIPMINE_H
#define GAME_SHARED_ENTITIES_WEAPONS_CTRIPMINE_H

enum tripmine_e
{
	TRIPMINE_IDLE1 = 0,
	TRIPMINE_IDLE2,
	TRIPMINE_ARM1,
	TRIPMINE_ARM2,
	TRIPMINE_FIDGET,
	TRIPMINE_HOLSTER,
	TRIPMINE_DRAW,
	TRIPMINE_WORLD,
	TRIPMINE_GROUND,
};

class CTripmine : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CTripmine, CBasePlayerWeapon );

	CTripmine();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetObjectCollisionBox( void ) override
	{
		//!!!BUGBUG - fix the model!
		SetAbsMin( GetAbsOrigin() + Vector( -16, -16, -5 ) );
		SetAbsMax( GetAbsOrigin() + Vector( 16, 16, 28 ) );
	}

	void PrimaryAttack( void ) override;
	bool Deploy() override;
	void Holster() override;
	void WeaponIdle( void ) override;

private:
	unsigned short m_usTripFire;
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CTRIPMINE_H