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
// Entidade generica da mira em terceira pessoa
#ifndef CLIENT_DLL
#include "entities/CHu3XSpot.h"
#endif
// ############ //

#include "CHornetGun.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"
#include "entities/NPCs/CHornet.h"
#include "gamerules/GameRules.h"

BEGIN_DATADESC( CHornetGun )
	DEFINE_FIELD( m_flRechargeTime, FIELD_FLOAT ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( weapon_hornetgun, CHornetGun );

CHornetGun::CHornetGun()
	: BaseClass( WEAPON_HORNETGUN )
{
}

bool CHornetGun::IsUseable()
{
	return true;
}

void CHornetGun::Spawn( )
{
	Precache( );
	SetModel( "models/w_hgun.mdl");

	m_iFirePhase = 0;

	FallInit();// get ready to fall down.
}


void CHornetGun::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/v_hgun.mdl");
	PRECACHE_MODEL("models/w_hgun.mdl");
	PRECACHE_MODEL("models/p_hgun.mdl");

	m_usHornetFire = PRECACHE_EVENT ( 1, "events/firehornet.sc" );

	UTIL_PrecacheOther("hornet");
}

// ############ hu3lifezado ############ //
// Chamada do ponto de mira da terceira pessoa
void CHornetGun::ItemPreFrame(void)
{
#ifndef CLIENT_DLL
	if (m_pPlayer->cam_hu3_crosshair == 0)
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

bool CHornetGun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{

#ifndef CLIENT_DLL
		if ( g_pGameRules->IsMultiplayer() )
		{
			// in multiplayer, all hivehands come full. 
			pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = HORNET_MAX_CARRY;
		}
#endif

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return true;
	}
	return false;
}

bool CHornetGun::Deploy()
{
	return DefaultDeploy( "models/v_hgun.mdl", "models/p_hgun.mdl", HGUN_UP, "hive" );
}

void CHornetGun::Holster()
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

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( HGUN_DOWN );

	//!!!HACKHACK - can't select hornetgun if it's empty! no way to get ammo for it, either.
	if ( !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
	{
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = 1;
	}
}


void CHornetGun::PrimaryAttack()
{
	Reload( );

	if (m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0)
	{
		return;
	}

#ifndef CLIENT_DLL
	UTIL_MakeVectors( m_pPlayer->GetViewAngle() );

	CBaseEntity *pHornet = CBaseEntity::Create( "hornet", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->GetViewAngle(), m_pPlayer->edict() );
	pHornet->SetAbsVelocity( gpGlobals->v_forward * 300 );

	m_flRechargeTime = gpGlobals->time + 0.5;
#endif
	
	m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ]--;
	

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usHornetFire, 0.0, g_vecZero, g_vecZero, 0.0, 0.0, FIREMODE_TRACK, 0, 0, 0 );

	

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_flNextPrimaryAttack = GetNextAttackDelay(0.25);

	if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
	{
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25;
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}



void CHornetGun::SecondaryAttack( void )
{
	Reload();

	if (m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0)
	{
		return;
	}

	//Wouldn't be a bad idea to completely predict these, since they fly so fast...
#ifndef CLIENT_DLL
	CBaseEntity *pHornet;
	Vector vecSrc;

	UTIL_MakeVectors( m_pPlayer->GetViewAngle() );

	vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12;

	m_iFirePhase++;
	switch ( m_iFirePhase )
	{
	case 1:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		break;
	case 2:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 3:
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 4:
		vecSrc = vecSrc + gpGlobals->v_up * -8;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 5:
		vecSrc = vecSrc + gpGlobals->v_up * -8;
		break;
	case 6:
		vecSrc = vecSrc + gpGlobals->v_up * -8;
		vecSrc = vecSrc + gpGlobals->v_right * -8;
		break;
	case 7:
		vecSrc = vecSrc + gpGlobals->v_right * -8;
		break;
	case 8:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		vecSrc = vecSrc + gpGlobals->v_right * -8;
		m_iFirePhase = 0;
		break;
	}

	pHornet = CBaseEntity::Create( "hornet", vecSrc, m_pPlayer->GetViewAngle(), m_pPlayer->edict() );
	pHornet->SetAbsVelocity( gpGlobals->v_forward * 1200 );
	pHornet->SetAbsAngles( UTIL_VecToAngles( pHornet->GetAbsVelocity() ) );

	pHornet->SetThink( &CHornet::StartDart );

	m_flRechargeTime = gpGlobals->time + 0.5;
#endif

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usHornetFire, 0.0, g_vecZero, g_vecZero, 0.0, 0.0, FIREMODE_FAST, 0, 0, 0 );


	m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ]--;
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

		// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}


void CHornetGun::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] >= HORNET_MAX_CARRY)
		return;

	while (m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] < HORNET_MAX_CARRY && m_flRechargeTime < gpGlobals->time)
	{
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ]++;
		m_flRechargeTime += 0.5;
	}
}


void CHornetGun::WeaponIdle( void )
{
	Reload( );

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
	if (flRand <= 0.75)
	{
		iAnim = HGUN_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 30.0 / 16 * (2);
	}
	else if (flRand <= 0.875)
	{
		iAnim = HGUN_FIDGETSWAY;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
	}
	else
	{
		iAnim = HGUN_FIDGETSHAKE;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 35.0 / 16.0;
	}
	SendWeaponAnim( iAnim );
}

void CHornetGun::WeaponHolsterPreFrame()
{
	BaseClass::WeaponHolsterPreFrame();

	//Reload in the background when holstered. - Solokiller
	Reload();
}
