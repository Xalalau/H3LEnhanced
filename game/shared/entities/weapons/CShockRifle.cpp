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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"
#include "CBasePlayer.h"

#ifndef CLIENT_DLL
#include "entities/weapons/CShockBeam.h"
#include "gamerules/GameRules.h"
#endif

#include "CShockRifle.h"

LINK_ENTITY_TO_CLASS( weapon_shockrifle, CShockRifle );
LINK_ENTITY_TO_CLASS( weapon_shockroach, CShockRifle );

CShockRifle::CShockRifle()
	: BaseClass( WEAPON_SHOCKRIFLE )
{
}

void CShockRifle::Precache()
{
	BaseClass::Precache();

	PRECACHE_MODEL( "models/v_shock.mdl" );
	PRECACHE_MODEL( "models/w_shock_rifle.mdl" );
	PRECACHE_MODEL( "models/p_shock.mdl" );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	PRECACHE_MODEL( "sprites/lgtning.spr" );

	PRECACHE_SOUND( "weapons/shock_fire.wav" );
	PRECACHE_SOUND( "weapons/shock_draw.wav" );
	PRECACHE_SOUND( "weapons/shock_recharge.wav" );
	PRECACHE_SOUND( "weapons/shock_discharge.wav" );

	UTIL_PrecacheOther( "shock_beam" );

	m_usShockRifle = PRECACHE_EVENT( 1, "events/shock.sc" );
}

void CShockRifle::Spawn()
{
	// Hack to allow for old names.
	pev->classname = MAKE_STRING( "weapon_shockrifle" );

	Precache();

	SetModel( "models/w_shock_rifle.mdl" );

	FallInit();

	SetSequence( 0 );
	SetAnimTime( gpGlobals->time );
	SetFrameRate( 1 );
}

bool CShockRifle::AddToPlayer( CBasePlayer* pPlayer )
{
	if( BaseClass::AddToPlayer( pPlayer ) )
	{
#ifndef CLIENT_DLL
		pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = GetWeaponInfo()->GetDefaultAmmo();
#endif

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, nullptr, pPlayer );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return true;
	}

	return false;
}

bool CShockRifle::CanDeploy() const
{
	return true;
}

bool CShockRifle::Deploy()
{
	if( bIsMultiplayer() )
	{
		m_flRechargeTime = gpGlobals->time + 0.25;
	}
	else
	{
		m_flRechargeTime = gpGlobals->time + 0.5;
	}

	return DefaultDeploy( 
		"models/v_shock.mdl", "models/p_shock.mdl", 
		SHOCKRIFLE_DRAW, "bow" );
}

void CShockRifle::Holster()
{
	if( !bIsMultiplayer() )
	{
		for( auto& pBeam : m_pNoseBeam )
		{
			if( pBeam )
			{
				UTIL_Remove( pBeam );
				pBeam = nullptr;
			}
		}
	}

	m_fInReload = false;

	SetThink( nullptr );

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;

	SendWeaponAnim( SHOCKRIFLE_HOLSTER );

	//TODO: unnecessary if the shock rifle can regen while inactive. - Solokiller
	if( !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
	{
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = 1;
	}
}

void CShockRifle::WeaponIdle()
{
	//TODO: shouldn't this be using UTIL_WeaponTimeBase? - Solokiller
	float flNextIdle;

	if( m_flSoundDelay == 0 )
	{
		flNextIdle = gpGlobals->time;
	}
	else
	{
		if( gpGlobals->time >= m_flSoundDelay )
		{
			m_flSoundDelay = 0;
			flNextIdle = gpGlobals->time;
		}
	}

	if( m_flTimeWeaponIdle <= flNextIdle )
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1 / 3.0;

		SendWeaponAnim( SHOCKRIFLE_IDLE1 );
	}
}

void CShockRifle::PrimaryAttack()
{
	if( m_pPlayer->GetWaterLevel() == WATERLEVEL_HEAD )
	{
		//Water goes zap.
		const float flVolume = UTIL_RandomFloat( 0.8, 0.9 );

		EMIT_SOUND_DYN( 
			m_pPlayer, CHAN_ITEM, 
			"weapons/shock_discharge.wav", flVolume, ATTN_NONE, 0, PITCH_NORM );

		RadiusDamage(
			GetAbsOrigin(), 
			CTakeDamageInfo( 
				m_pPlayer, 
				m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] * 100.0, 
				DMG_ALWAYSGIB | DMG_BLAST ), 
			m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] * 150.0, 
			0 );

		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = 0;

		return;
	}

	if( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0 )
	{
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	--m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ];

	m_flRechargeTime = gpGlobals->time + 1.0;

	int flags;

#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT( flags, m_pPlayer->edict(), m_usShockRifle );

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_pPlayer->AddEffectsFlags( EF_MUZZLEFLASH );

#ifndef CLIENT_DLL
	const Vector vecAnglesAim = m_pPlayer->GetViewAngle() + m_pPlayer->GetPunchAngle();

	UTIL_MakeVectors( vecAnglesAim );

	Vector vecSrc = 
		m_pPlayer->GetGunPosition() + 
		gpGlobals->v_forward * 16 + 
		gpGlobals->v_right * 9 + 
		gpGlobals->v_up * -7;

	Vector vecAngles = m_pPlayer->GetAutoaimVectorFromPoint( vecSrc, AUTOAIM_10DEGREES );

	CShockBeam* pBeam = CShockBeam::CreateShockBeam( vecSrc, vecAnglesAim + vecAngles, m_pPlayer );
#endif

	if( bIsMultiplayer() )
	{
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;
	}
	else
	{
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.33;

	CreateChargeEffect();

	SetNextThink( gpGlobals->time + 0.08 );

	SetThink( &CShockRifle::DestroyChargeEffect );
}

void CShockRifle::SecondaryAttack()
{
	//Nothing
}

void CShockRifle::Reload()
{
	const int iMax = GetWeaponInfo()->GetDefaultAmmo();

	if( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] >= iMax )
	{
		return;
	}

	while( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] < iMax )
	{
		if( m_flRechargeTime >= gpGlobals->time )
			break;

		++m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ];

		EMIT_SOUND_DYN( 
			m_pPlayer, CHAN_WEAPON, 
			"weapons/shock_recharge.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );

		if( bIsMultiplayer() )
		{
			m_flRechargeTime += 0.25;
		}
		else
		{
			m_flRechargeTime += 0.5;
		}
	}
}

void CShockRifle::ItemPostFrame()
{
	BaseClass::ItemPostFrame();

	Reload();
}

void CShockRifle::CreateChargeEffect()
{
#ifndef CLIENT_DLL
	if( !bIsMultiplayer() )
	{
		//TODO: this can probably be done on the client side. - Solokiller
		const int iEntIndex = m_pPlayer->entindex();

		int iEnd = 2;

		for( auto& pBeam : m_pNoseBeam )
		{
			pBeam = CBeam::BeamCreate( "sprites/lgtning.spr", 16 );

			pBeam->EntsInit( iEntIndex, iEntIndex );

			pBeam->SetNoise( 75 );

			pBeam->SetWidth( 10 );

			pBeam->SetRenderColor( Vector( 0, 253, 253 ) );

			pBeam->SetScrollRate( 30 );
			pBeam->SetBrightness( 190 );

			pBeam->SetStartAttachment( 1 );
			pBeam->SetEndAttachment( iEnd++ );
		}
	}
#endif
}

void CShockRifle::DestroyChargeEffect()
{
	if( !bIsMultiplayer() )
	{
		for( auto& pBeam : m_pNoseBeam )
		{
			if( pBeam )
			{
				UTIL_Remove( pBeam );
				pBeam = nullptr;
			}
		}
	}

	SetThink( nullptr );
}
#endif //USE_OPFOR
