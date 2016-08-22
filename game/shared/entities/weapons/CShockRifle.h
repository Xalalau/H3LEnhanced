#if USE_OPFOR
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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CSHOCKRIFLE_H
#define GAME_SHARED_ENTITIES_WEAPONS_CSHOCKRIFLE_H

enum ShockRifleAnim
{
	SHOCKRIFLE_IDLE1 = 0,
	SHOCKRIFLE_FIRE,
	SHOCKRIFLE_DRAW,
	SHOCKRIFLE_HOLSTER,
	SHOCKRIFLE_IDLE3
};

class CShockRifle : public CBasePlayerWeapon
{
private:
	static const size_t NUM_BEAMS = 3;

public:
	DECLARE_CLASS( CShockRifle, CBasePlayerWeapon );

	CShockRifle();

	void Precache() override;

	void Spawn() override;

	bool AddToPlayer( CBasePlayer* pPlayer ) override;

	bool CanDeploy() const override;

	bool Deploy() override;

	void Holster() override;

	void WeaponIdle() override;

	void PrimaryAttack() override;

	void SecondaryAttack() override;

	void Reload() override;

	void ItemPostFrame() override;

private:
	void CreateChargeEffect();

	void EXPORT DestroyChargeEffect();

private:
	int m_iSpriteTexture;

	unsigned short m_usShockRifle;

	float m_flRechargeTime;
	float m_flSoundDelay;

	CBeam* m_pNoseBeam[ NUM_BEAMS ];
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CSHOCKRIFLE_H
#endif //USE_OPFOR
