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
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"

// ############ hu3lifezado ############ //
// [Terceira Pessoa]
// Entidade generica da mira em terceira pessoa
#ifndef CLIENT_DLL
#include "entities/CHu3XSpot.h"
#endif
// ############ //

#include "CGlock.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"

LINK_ENTITY_TO_CLASS( weapon_glock, CGlock );
LINK_ENTITY_TO_CLASS( weapon_9mmhandgun, CGlock );

CGlock::CGlock()
	: BaseClass( WEAPON_GLOCK )
{
}

void CGlock::Spawn( )
{
	SetClassname("weapon_9mmhandgun"); // hack to allow for old names
	Precache( );

	SetModel( "models/w_9mmhandgun.mdl");

	FallInit();// get ready to fall down.
}


void CGlock::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/v_9mmhandgun.mdl");
	PRECACHE_MODEL("models/w_9mmhandgun.mdl");
	PRECACHE_MODEL("models/p_9mmhandgun.mdl");

	m_iShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell

	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/9mmclip2.wav");

	PRECACHE_SOUND ("weapons/pl_gun1.wav");//silenced handgun
	PRECACHE_SOUND ("weapons/pl_gun2.wav");//silenced handgun
	PRECACHE_SOUND ("weapons/pl_gun3.wav");//handgun

	m_usFireGlock1 = PRECACHE_EVENT( 1, "events/glock1.sc" );
	m_usFireGlock2 = PRECACHE_EVENT( 1, "events/glock2.sc" );
}

// ############ hu3lifezado ############ //
// [Terceira Pessoa]
// Chamada do ponto de mira da terceira pessoa
void CGlock::ItemPreFrame(void)
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

// Recolha da arma
void CGlock::Holster()
{
#ifndef CLIENT_DLL
	if (m_pLaser)
	{
		m_pLaser->RemoveSpot(m_pLaser);
		m_pLaser = nullptr;
	}
#endif	
}
// ############ //

bool CGlock::Deploy()
{
	// SetBody( 1 );
	return DefaultDeploy( "models/v_9mmhandgun.mdl", "models/p_9mmhandgun.mdl", GLOCK_DRAW, "onehanded" );
}

void CGlock::SecondaryAttack( void )
{
	GlockFire( 0.1, 0.2, false );
}

void CGlock::PrimaryAttack( void )
{
	GlockFire( 0.01, 0.3, true );
}

void CGlock::GlockFire( float flSpread , float flCycleTime, const bool fUseAutoAim )
{
	if (m_iClip <= 0)
	{
		if ( m_bFireOnEmpty )
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = GetNextAttackDelay(0.2);
		}

		return;
	}

	m_iClip--;

	m_pPlayer->GetEffects().AddFlags( EF_MUZZLEFLASH );

	int flags;

#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	// silenced
	if (GetBody() == 1)
	{
		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
	}
	else
	{
		// non-silenced
		m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	}

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming;
	
	if ( fUseAutoAim )
	{
		vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );
	}
	else
	{
		vecAiming = gpGlobals->v_forward;
	}

	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 8192, BULLET_PLAYER_9MM, 0, 0, m_pPlayer, m_pPlayer->random_seed );

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), fUseAutoAim ? m_usFireGlock1 : m_usFireGlock2, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, ( m_iClip == 0 ) ? 1 : 0, 0 );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	if (!m_iClip && m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, 0);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}


void CGlock::Reload( void )
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0 )
		 return;

	bool bResult;

	// ############ hu3lifezado ############ //
	// [Terceira Pessoa]
	// Mira na terceira pessoa removida
#ifndef CLIENT_DLL
	if (m_pLaser)
	{
		m_pLaser->RemoveSpot(m_pLaser);
		m_pLaser = nullptr;
	}
#endif	
	// Tempo de recarga aumentado para acertar com a nova animacao (1.5)
	if (m_iClip != 0)
		bResult = DefaultReload(GLOCK_RELOAD_NOT_EMPTY, 2.2);
	else
		bResult = DefaultReload(GLOCK_RELOAD, 2.2);
	// ############ //

	if (m_iClip == 0)
		bResult = DefaultReload( GLOCK_RELOAD, 1.5 );
	else
		bResult = DefaultReload( GLOCK_RELOAD_NOT_EMPTY, 1.5 );

	if ( bResult )
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
}



void CGlock::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	// only idle if the slid isn't back
	if (m_iClip != 0)
	{
		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.0, 1.0 );

		if (flRand <= 0.3 + 0 * 0.75)
		{
			iAnim = GLOCK_IDLE3;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 49.0 / 16;
		}
		else if (flRand <= 0.6 + 0 * 0.875)
		{
			iAnim = GLOCK_IDLE1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 16.0;
		}
		else
		{
			iAnim = GLOCK_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
		}
		SendWeaponAnim( iAnim );
	}
}








class CGlockAmmo : public CBasePlayerAmmo
{
	void Spawn( void ) override
	{ 
		Precache( );
		SetModel( "models/w_9mmclip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void ) override
	{
		PRECACHE_MODEL ("models/w_9mmclip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo( CBaseEntity *pOther ) override
	{ 
		return UTIL_GiveAmmoToPlayer( this, pOther, AMMO_GLOCKCLIP_GIVE, "9mm" );
	}
};
LINK_ENTITY_TO_CLASS( ammo_glockclip, CGlockAmmo );
LINK_ENTITY_TO_CLASS( ammo_9mmclip, CGlockAmmo );















