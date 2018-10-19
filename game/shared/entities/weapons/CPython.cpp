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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"

// ############ hu3lifezado ############ //
// Entidade generica da mira em terceira pessoa
#ifndef CLIENT_DLL
#include "entities/CHu3XSpot.h"
#endif
// ############ //

#include "CPython.h"
#include "entities/NPCs/Monsters.h"
#include "CBasePlayer.h"
#include "gamerules/GameRules.h"

LINK_ENTITY_TO_CLASS( weapon_python, CPython );
LINK_ENTITY_TO_CLASS( weapon_357, CPython );

CPython::CPython()
	: BaseClass( WEAPON_PYTHON )
{
}

bool CPython::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return true;
	}
	return false;
}

void CPython::Spawn( )
{
	SetClassname("weapon_357"); // hack to allow for old names
	Precache( );
	SetModel( "models/w_357.mdl");

	FallInit();// get ready to fall down.
}


void CPython::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/v_357.mdl");
	PRECACHE_MODEL("models/w_357.mdl");
	PRECACHE_MODEL("models/p_357.mdl");

	PRECACHE_MODEL("models/w_357ammobox.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/357_reload1.wav");
	PRECACHE_SOUND ("weapons/357_cock1.wav");
	PRECACHE_SOUND ("weapons/357_shot1.wav");
	PRECACHE_SOUND ("weapons/357_shot2.wav");

	m_usFirePython = PRECACHE_EVENT( 1, "events/python.sc" );
}

// ############ hu3lifezado ############ //
// Chamada do ponto de mira da terceira pessoa
void CPython::ItemPreFrame(void)
{
#ifndef CLIENT_DLL
	if (m_pPlayer->hu3_cam_crosshair == 0)
	{
		if (m_pLaser)
		{
			m_pLaser->RemoveSpot(m_pLaser);
			m_pLaser = nullptr;
		}
	}
	else
	{
		if (!m_pLaser)
			m_pLaser = CHu3XSpot::CreateSpot();

		m_pLaser->UpdateSpot(m_pPlayer, m_pLaser);
	}
#endif	
}
// ############ //

bool CPython::Deploy()
{
	if ( bIsMultiplayer() )
	{
		// enable laser sight geometry.
		SetBody( 1 );
	}
	else
	{
		SetBody( 0 );
	}

	return DefaultDeploy( "models/v_357.mdl", "models/p_357.mdl", PYTHON_DRAW, "python", GetBody() );
}


void CPython::Holster()
{
	// ############ hu3lifezado ############ //
	// Remocao da mira em terceira pessoa
#ifndef CLIENT_DLL
	if (m_pLaser)
	{
		m_pLaser->RemoveSpot(m_pLaser);
		m_pLaser = nullptr;
	}
#endif
	// ############ //

	m_fInReload = false;// cancel any reload in progress.

	if ( m_fInZoom )
	{
		SecondaryAttack();
	}

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	SendWeaponAnim( PYTHON_HOLSTER );
}

void CPython::SecondaryAttack( void )
{
	if ( !bIsMultiplayer() )
	{
		return;
	}

	if ( m_pPlayer->GetFOV() != 0 )
	{
		m_fInZoom = false;
		m_pPlayer->SetFOV( 0 );
		m_pPlayer->m_iFOV = 0;  // 0 means reset to default fov
	}
	else if ( m_pPlayer->GetFOV() != 40 )
	{
		m_fInZoom = true;
		m_pPlayer->SetFOV( 40 );
		m_pPlayer->m_iFOV = 40;
	}

	m_flNextSecondaryAttack = 0.5;
}

void CPython::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->GetWaterLevel() == WATERLEVEL_HEAD )
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		if (!m_bFireOnEmpty )
			Reload( );
		else
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = 0.15;
		}

		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_iClip--;

	m_pPlayer->GetEffects().AddFlags( EF_MUZZLEFLASH );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );


	UTIL_MakeVectors( m_pPlayer->GetViewAngle() + m_pPlayer->GetPunchAngle() );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_357, 0, 0, m_pPlayer, m_pPlayer->random_seed );

    int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFirePython, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, 0);

	m_flNextPrimaryAttack = 0.75;
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}


void CPython::Reload( void )
{
	if ( m_pPlayer->GetAmmoCountByID( PrimaryAmmoIndex() ) <= 0 )
		return;

	// ############ hu3lifezado ############ //
	// Remocao da mira em terceira pessoa
#ifndef CLIENT_DLL
	if (m_pLaser)
	{
		m_pLaser->RemoveSpot(m_pLaser);
		m_pLaser = nullptr;
	}
#endif
	// ############ //

	if ( m_pPlayer->GetFOV() != 0 )
	{
		m_fInZoom = false;
		m_pPlayer->SetFOV( 0 );
		m_pPlayer->m_iFOV = 0;  // 0 means reset to default fov
	}

	//TODO: same code is below this - Solokiller
	const int iScopeBody = bIsMultiplayer() ? 1 : 0;

	DefaultReload( PYTHON_RELOAD, 2.0, iScopeBody );
}


void CPython::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
	if (flRand <= 0.5)
	{
		iAnim = PYTHON_IDLE1;
		m_flTimeWeaponIdle = (70.0/30.0);
	}
	else if (flRand <= 0.7)
	{
		iAnim = PYTHON_IDLE2;
		m_flTimeWeaponIdle = (60.0/30.0);
	}
	else if (flRand <= 0.9)
	{
		iAnim = PYTHON_IDLE3;
		m_flTimeWeaponIdle = (88.0/30.0);
	}
	else
	{
		iAnim = PYTHON_FIDGET;
		m_flTimeWeaponIdle = (170.0/30.0);
	}
	
	const int iScopeBody = bIsMultiplayer() ? 1 : 0;
	
	SendWeaponAnim( iAnim, iScopeBody );
}


class CPythonAmmo : public CBasePlayerAmmo
{
public:
	DECLARE_CLASS( CPythonAmmo, CBasePlayerAmmo );

	void Spawn( void ) override
	{ 
		Precache( );
		SetModel( "models/w_357ammobox.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void ) override
	{
		PRECACHE_MODEL ("models/w_357ammobox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo( CBaseEntity *pOther ) override
	{ 
		return UTIL_GiveAmmoToPlayer( this, pOther, AMMO_357BOX_GIVE, "357" );
	}
};
LINK_ENTITY_TO_CLASS( ammo_357, CPythonAmmo );