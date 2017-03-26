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
#include "CBasePlayer.h"
#include "Weapons.h"

#include "gamerules/GameRules.h"

#include "CWeaponInfoCache.h"

#ifdef CLIENT_DLL
#include "hud.h"
#include "cl_util.h"

#include "com_weapons.h"
#endif

void CBasePlayerWeapon::SetObjectCollisionBox()
{
	pev->absmin = GetAbsOrigin() + Vector( -24, -24, 0 );
	pev->absmax = GetAbsOrigin() + Vector( 24, 24, 16 );
}

void CBasePlayerWeapon::Precache()
{
	BaseClass::Precache();

	m_pWeaponInfo = g_WeaponInfoCache.LoadWeaponInfo( m_iId, GetClassname() );

	m_iDefaultAmmo = GetWeaponInfo()->GetDefaultAmmo();
}

bool CBasePlayerWeapon::PlayEmptySound()
{
	if( m_bPlayEmptySound )
	{
#ifdef CLIENT_DLL
		HUD_PlaySound( "weapons/357_cock1.wav", 0.8 );
#else
		EMIT_SOUND( m_pPlayer, CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM );
#endif
		m_bPlayEmptySound = false;
		return false;
	}
	return false;
}

void CBasePlayerWeapon::ResetEmptySound()
{
	m_bPlayEmptySound = true;
}

bool CBasePlayerWeapon::CanDeploy() const
{
	// ############ hu3lifezado ############ //
	// Icone de recarga na terceira pessoa
	if (m_pPlayer->cam_hu3_reload_icon)
	{
		m_pPlayer->cam_hu3_reload_icon = false;
	}
	// ############

	bool bHasAmmo = false;

	if( !pszAmmo1() )
	{
		// this weapon doesn't use ammo, can always deploy.
		return true;
	}

	if( pszAmmo1() )
	{
		bHasAmmo |= ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] != 0 );
	}
	if( pszAmmo2() )
	{
		bHasAmmo |= ( m_pPlayer->m_rgAmmo[ SecondaryAmmoIndex() ] != 0 );
	}
	if( m_iClip > 0 )
	{
		bHasAmmo |= 1;
	}
	if( !bHasAmmo )
	{
		return false;
	}

	return true;
}

bool CBasePlayerWeapon::DefaultReload( int iAnim, float fDelay, int body )
{
	if( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0 )
		return false;

	int j = min( GetWeaponInfo()->GetMaxMagazine() - m_iClip, m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] );

	if( j == 0 )
		return false;

#ifdef CLIENT_DLL
	// ############ hu3lifezado ############ //
	// Icone de recarga na terceira pessoa
	if (gEngfuncs.pfnGetCvarFloat("cam_hu3") != 0)
	{
		m_pPlayer->cam_hu3_reload_icon = true;
	}
	// ############
#endif

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + fDelay;

#ifdef SERVER_DLL
	//The server doesn't pass this, so always 0 it out. - Solokiller
	//TODO: why? - Solokiller
	body = 0;
#endif

	//!!UNDONE -- reload sound goes here !!!
	SendWeaponAnim( iAnim, body );

	m_fInReload = true;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
	return true;
}

bool CanAttack( float attack_time, float curtime, const bool isPredicted )
{
	//Client always predicts, so skip this block. - Solokiller
#ifdef SERVER_DLL
#if defined( CLIENT_WEAPONS )
	if( !isPredicted )
#else
	if( 1 )
#endif
	{
		return attack_time <= curtime;
	}
	else
#endif
	{
		return attack_time <= 0.0;
	}
}

bool CanReload( float flAttackTime, float flCurrentTime, const bool bIsPredicted )
{
	//Client always predicts, so skip this block. - Solokiller
#ifdef SERVER_DLL
	if( !bIsPredicted )
	{
		return  flAttackTime < flCurrentTime;
	}
	else
#endif
	{
		return flAttackTime < 0.0;
	}
}

/*
=====================
CBasePlayerWeapon::ItemPostFrame

Handles weapon firing, reloading, etc.
=====================
*/
void CBasePlayerWeapon::ItemPostFrame( void )
{
	WeaponTick();

	if( ( m_fInReload ) && ( m_pPlayer->m_flNextAttack <= UTIL_WeaponTimeBase() ) )
	{
		// ############ hu3lifezado ############ //
		// Icone de recarga na terceira pessoa
		if (m_pPlayer->cam_hu3_reload_icon)
		{
			m_pPlayer->cam_hu3_reload_icon = false;
		}
		// ############
#ifdef SERVER_DLL // FIXME, need ammo on client to make this work right
		// complete the reload. 
		int j = min( iMaxClip() - m_iClip, m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] );

		// Add them to the clip
		m_iClip += j;
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] -= j;
#else	
		m_iClip += 10;
#endif

		m_fInReload = false;
	}

#ifdef SERVER_DLL
	if( !( m_pPlayer->pev->button & IN_ATTACK ) )
	{
		m_flLastFireTime = 0.0f;
	}
#endif

	if( ( m_pPlayer->pev->button & IN_ATTACK2 ) && CanAttack( m_flNextSecondaryAttack, gpGlobals->time, IsPredicted() ) )
	{
		if( pszAmmo2() && !m_pPlayer->m_rgAmmo[ SecondaryAmmoIndex() ] )
		{
			m_bFireOnEmpty = true;
		}

		SecondaryAttack();
		m_pPlayer->pev->button &= ~IN_ATTACK2;
	}
	else if( ( m_pPlayer->pev->button & IN_ATTACK ) && CanAttack( m_flNextPrimaryAttack, gpGlobals->time, IsPredicted() ) )
	{
		if( ( m_iClip == 0 && pszAmmo1() ) || ( iMaxClip() == -1 && !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] ) )
		{
			m_bFireOnEmpty = true;
		}

		PrimaryAttack();
	}
	else if( m_pPlayer->pev->button & IN_RELOAD && iMaxClip() != WEAPON_NOCLIP && !m_fInReload )
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
	}
	else if( !( m_pPlayer->pev->button & ( IN_ATTACK | IN_ATTACK2 ) ) )
	{
		// no fire buttons down

		m_bFireOnEmpty = false;

		//Only the server checks for weapon switching. - Solokiller
#ifdef SERVER_DLL
		if( !IsUseable() && m_flNextPrimaryAttack < ( IsPredicted() ? 0.0 : gpGlobals->time ) )
		{
			// weapon isn't useable, switch.
			if( !( iFlags() & ITEM_FLAG_NOAUTOSWITCHEMPTY ) && g_pGameRules->GetNextBestWeapon( m_pPlayer, this ) )
			{
				m_flNextPrimaryAttack = ( IsPredicted() ? 0.0 : gpGlobals->time ) + 0.3;
				return;
			}
		}
		else
#endif
		{
			// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			if( m_iClip == 0 && !( iFlags() & ITEM_FLAG_NOAUTORELOAD ) && CanReload( m_flNextPrimaryAttack, gpGlobals->time, IsPredicted() ) )
			{
				Reload();
				return;
			}
		}

		WeaponIdle();
		return;
	}

	// catch all
	if( ShouldWeaponIdle() )
	{
		WeaponIdle();
	}
}

void CBasePlayerWeapon::GetWeaponData( weapon_data_t& data )
{
	data.m_iClip = m_iClip;

	data.m_flTimeWeaponIdle = max( m_flTimeWeaponIdle, -0.001f );
	data.m_flNextPrimaryAttack = max( m_flNextPrimaryAttack, -0.001f );
	data.m_flNextSecondaryAttack = max( m_flNextSecondaryAttack, -0.001f );

	data.m_fInReload = m_fInReload;

	data.fuser1 = max( pev->fuser1, -0.001f );
}

void CBasePlayerWeapon::SetWeaponData( const weapon_data_t& data )
{
	m_iClip = data.m_iClip;

	m_flTimeWeaponIdle = data.m_flTimeWeaponIdle;
	m_flNextPrimaryAttack = data.m_flNextPrimaryAttack;
	m_flNextSecondaryAttack = data.m_flNextSecondaryAttack;

	m_fInReload = data.m_fInReload != 0;

	pev->fuser1 = data.fuser1;
}

int CBasePlayerWeapon::PrimaryAmmoIndex() const
{
	return GetWeaponInfo()->GetPrimaryAmmo() ? GetWeaponInfo()->GetPrimaryAmmo()->GetID() : WEAPON_NOCLIP;
}

int CBasePlayerWeapon::SecondaryAmmoIndex() const
{
	//Used to return -1 unconditionally. - Solokiller
	return GetWeaponInfo()->GetSecondaryAmmo() ? GetWeaponInfo()->GetSecondaryAmmo()->GetID() : WEAPON_NOCLIP;
}
