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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CGAUSS_H
#define GAME_SHARED_ENTITIES_WEAPONS_CGAUSS_H

enum gauss_e
{
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW,
	// ############ hu3lifezado ############ //
	// Pequena gambiarra para consertar o saque da arma
	GAUSS_FIRE_HU3_FIX
	// ############ //
};

#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged

class CGauss : public CBasePlayerWeapon
{
public:
	enum class AttackState
	{
		NOT_ATTACKING = 0,
		CHARGING_START = 1,
		CHARGING = 2,
	};

	/**
	*	Amount of ammo to use per primary attack shot.
	*/
	static const int AMMO_PER_PRIMARY_SHOT = 2;

public:
	DECLARE_CLASS( CGauss, CBasePlayerWeapon );
#ifndef CLIENT_DLL
	DECLARE_DATADESC();
#endif

	CGauss();

	void Spawn() override;
	void Precache() override;
	bool AddToPlayer( CBasePlayer *pPlayer ) override;

	/**
	*	Overridden for the gauss because the case where a 1 ammo left secondary attack occurs requires a special check.
	*	- Solokiller
	*/
	bool IsUseable() override;

	bool Deploy() override;
	void Holster() override;

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void WeaponIdle() override;

	void StartFire();
	void Fire( Vector vecOrigSrc, Vector vecDirShooting, float flDamage );
	float GetFullChargeTime();

	void GetWeaponData( weapon_data_t& data ) override
	{
		BaseClass::GetWeaponData( data );

		data.iuser2 = static_cast<int>( m_InAttack );
	}

	void SetWeaponData( const weapon_data_t& data ) override
	{
		BaseClass::SetWeaponData( data );

		m_InAttack = static_cast<AttackState>( data.iuser2 );
	}

	int m_iBalls;
	int m_iGlow;
	int m_iBeam;
	int m_iSoundState; // don't save this

					   // was this weapon just fired primary or secondary?
					   // we need to know so we can pick the right set of effects. 
	bool m_fPrimaryFire;

	// ############ hu3lifezado ############ //
	// Funcoes para renderizar a mira em terceira pessoa
	void ItemPreFrame(void);
	void UpdateSpot();
	void RemoveSpot();
	// Codigo dos aneis de poder
	void SonicAttack(int force);
	void WriteBeamColor(int force);
	// Textura dos aneis
	int m_iSpriteTexture;
	// Tamanho dos aneis
	int m_iSpriteTextureRange;
	// ############ //

private:
	unsigned short m_usGaussFire;
	unsigned short m_usGaussSpin;

	// ############ hu3lifezado ############ //
	// Mira em terceira pessoa
	CHu3XSpot* m_pLaser;
	// ############ //

	AttackState m_InAttack;
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CGAUSS_H