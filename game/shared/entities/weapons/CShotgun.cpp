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
#include "entities/weapons/CHu3XSpot.h"
// ############ //

#include "CShotgun.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"
#include "gamerules/GameRules.h"

#ifdef SERVER_DLL
BEGIN_DATADESC( CShotgun )
	DEFINE_FIELD( m_flNextReload, FIELD_TIME ),
	DEFINE_FIELD( m_InSpecialReload, FIELD_INTEGER ),
	//DEFINE_FIELD( m_iShell, FIELD_INTEGER ),
	DEFINE_FIELD( m_flPumpTime, FIELD_TIME ),
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS( weapon_shotgun, CShotgun );

CShotgun::CShotgun()
	: BaseClass( WEAPON_SHOTGUN )
{
}

void CShotgun::Spawn( )
{
	Precache( );
	SetModel( "models/w_shotgun.mdl");

	FallInit();// get ready to fall
}


void CShotgun::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/v_shotgun.mdl");
	PRECACHE_MODEL("models/w_shotgun.mdl");
	PRECACHE_MODEL("models/p_shotgun.mdl");

	m_iShell = PRECACHE_MODEL ("models/shotgunshell.mdl");// shotgun shell

	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/dbarrel1.wav");//shotgun
	PRECACHE_SOUND ("weapons/sbarrel1.wav");//shotgun

	PRECACHE_SOUND ("weapons/reload1.wav");	// shotgun reload
	PRECACHE_SOUND ("weapons/reload3.wav");	// shotgun reload

//	PRECACHE_SOUND ("weapons/sshell1.wav");	// shotgun reload - played on client
//	PRECACHE_SOUND ("weapons/sshell3.wav");	// shotgun reload - played on client
	
	PRECACHE_SOUND ("weapons/357_cock1.wav"); // gun empty sound
	PRECACHE_SOUND ("weapons/scock1.wav");	// cock gun

	m_usSingleFire = PRECACHE_EVENT( 1, "events/shotgun1.sc" );
	m_usDoubleFire = PRECACHE_EVENT( 1, "events/shotgun2.sc" );
}

// ############ hu3lifezado ############ //
// Chamada do ponto de mira da terceira pessoa
void CShotgun::ItemPreFrame(void)
{
	UpdateSpot();
}

// Renderiza o ponto de mira da terceira pessoa 
void CShotgun::UpdateSpot()
{
#ifndef CLIENT_DLL
	if ((int)CVAR_GET_FLOAT("cam_hu3") != 0)
	{
		if (!m_pLaser)
		{
			m_pLaser = CHu3XSpot::CreateSpot();
		}

		UTIL_MakeVectors(m_pPlayer->GetViewAngle());

		Vector vecSrc = m_pPlayer->GetGunPosition();

		Vector vecEnd = vecSrc + gpGlobals->v_forward * 8192.0;

		TraceResult tr;

		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, m_pPlayer->edict(), &tr);

		m_pLaser->SetAbsOrigin(tr.vecEndPos);
	}
	else if (m_pLaser)
	{
		m_pLaser->Killed(CTakeDamageInfo(nullptr, 0, 0), GIB_NEVER);
		m_pLaser = nullptr;
	}
#endif
}

// Remove o ponto de mira da terceira pessoa 
void CShotgun::RemoveSpot()
{
#ifndef CLIENT_DLL
	if (m_pLaser)
	{
		m_pLaser->Killed(CTakeDamageInfo(nullptr, 0, 0), GIB_NEVER);
		m_pLaser = nullptr;
	}
#endif
}

// Recolha da arma
void CShotgun::Holster()
{
#ifndef CLIENT_DLL
	RemoveSpot();
#endif	
}
// ############ //

bool CShotgun::AddToPlayer( CBasePlayer *pPlayer )
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


bool CShotgun::Deploy()
{
	return DefaultDeploy( "models/v_shotgun.mdl", "models/p_shotgun.mdl", SHOTGUN_DRAW, "shotgun" );
}

void CShotgun::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->GetWaterLevel() == WATERLEVEL_HEAD )
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = GetNextAttackDelay(0.15);
		return;
	}

	if (m_iClip <= 0)
	{
		Reload( );
		if (m_iClip == 0)
			PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif


	m_pPlayer->GetEffects().AddFlags( EF_MUZZLEFLASH );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	Vector vecDir;

	if ( bIsMultiplayer() )
	{
		vecDir = m_pPlayer->FireBulletsPlayer( 4, vecSrc, vecAiming, VECTOR_CONE_DM_SHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer, m_pPlayer->random_seed );
	}
	else
	{
		// regular old, untouched spread. 
		vecDir = m_pPlayer->FireBulletsPlayer( 6, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer, m_pPlayer->random_seed );
	}

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSingleFire, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );


	if (!m_iClip && m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, 0);

	//if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.5;

	m_flNextPrimaryAttack = GetNextAttackDelay(0.75);
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
	m_InSpecialReload = ReloadState::NOT_RELOADING;
}


void CShotgun::SecondaryAttack( void )
{
	// don't fire underwater
	if (m_pPlayer->GetWaterLevel() == WATERLEVEL_HEAD )
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = GetNextAttackDelay(0.15);
		return;
	}

	if (m_iClip <= 1)
	{
		Reload( );
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip -= 2;


	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	m_pPlayer->GetEffects().AddFlags( EF_MUZZLEFLASH );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	Vector vecDir;
	
	if ( bIsMultiplayer() )
	{
		// tuned for deathmatch
		vecDir = m_pPlayer->FireBulletsPlayer( 8, vecSrc, vecAiming, VECTOR_CONE_DM_DOUBLESHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer, m_pPlayer->random_seed );
	}
	else
	{
		// untouched default single player
		vecDir = m_pPlayer->FireBulletsPlayer( 12, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer, m_pPlayer->random_seed );
	}
		
	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usDoubleFire, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, 0);

	//if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.95;

	m_flNextPrimaryAttack = GetNextAttackDelay(1.5);
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.5;
	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6.0;
	else
		m_flTimeWeaponIdle = 1.5;

	m_InSpecialReload = ReloadState::NOT_RELOADING;
}


void CShotgun::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0 || m_iClip == iMaxClip())
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_InSpecialReload == ReloadState::NOT_RELOADING )
	{
		SendWeaponAnim( SHOTGUN_START_RELOAD );
		m_InSpecialReload = ReloadState::DO_RELOAD_EFFECTS;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
		m_flNextPrimaryAttack = GetNextAttackDelay(1.0);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		return;
	}
	else if ( m_InSpecialReload == ReloadState::DO_RELOAD_EFFECTS )
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;
		// was waiting for gun to move to side
		m_InSpecialReload = ReloadState::RELOAD_ONE;

		if (RANDOM_LONG(0,1))
			EMIT_SOUND_DYN( m_pPlayer, CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));
		else
			EMIT_SOUND_DYN( m_pPlayer, CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));

		SendWeaponAnim( SHOTGUN_RELOAD );

		m_flNextReload = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;

		// ############ hu3lifezado ############ //
		// Remocao da mira em terceira pessoa
#ifndef CLIENT_DLL
		RemoveSpot();
#endif
		// ############ //
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] -= 1;
		m_InSpecialReload = ReloadState::DO_RELOAD_EFFECTS;

		// ############ hu3lifezado ############ //
		// Remocao da mira em terceira pessoa
#ifndef CLIENT_DLL
		RemoveSpot();
#endif
		// ############ //
	}
}

void CShotgun::WeaponTick()
{
	if( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{
		// play pumping sound
		EMIT_SOUND_DYN( m_pPlayer, CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG( 0, 0x1f ) );
		m_flPumpTime = 0;
	}
}

void CShotgun::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if (m_flTimeWeaponIdle <  UTIL_WeaponTimeBase() )
	{
		if (m_iClip == 0 && m_InSpecialReload == ReloadState::NOT_RELOADING && m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ])
		{
			Reload( );
		}
		else if (m_InSpecialReload != ReloadState::NOT_RELOADING )
		{
			if (m_iClip != iMaxClip() && m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ])
			{
				Reload( );
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim( SHOTGUN_PUMP );
				
				// play cocking sound
				EMIT_SOUND_DYN( m_pPlayer, CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
				m_InSpecialReload = ReloadState::NOT_RELOADING;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
			}
		}
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
			if (flRand <= 0.8)
			{
				iAnim = SHOTGUN_IDLE_DEEP;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0/12.0);// * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = SHOTGUN_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			else
			{
				iAnim = SHOTGUN_IDLE4;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			SendWeaponAnim( iAnim );
		}
	}
}



class CShotgunAmmo : public CBasePlayerAmmo
{
public:
	DECLARE_CLASS( CShotgunAmmo, CBasePlayerAmmo );

	void Spawn( void ) override
	{ 
		Precache( );
		SetModel( "models/w_shotbox.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void ) override
	{
		PRECACHE_MODEL ("models/w_shotbox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo( CBaseEntity *pOther ) override
	{ 
		return UTIL_GiveAmmoToPlayer( this, pOther, AMMO_BUCKSHOTBOX_GIVE, "buckshot" );
	}
};
LINK_ENTITY_TO_CLASS( ammo_buckshot, CShotgunAmmo );


