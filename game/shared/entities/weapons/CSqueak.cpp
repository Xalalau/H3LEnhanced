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
#include "CSqueak.h"
#include "CBasePlayer.h"

LINK_ENTITY_TO_CLASS( weapon_snark, CSqueak );

CSqueak::CSqueak()
	: BaseClass( WEAPON_SNARK )
{
}

void CSqueak::Spawn( )
{
	Precache( );
	SetModel( "models/w_sqknest.mdl");

	FallInit();//get ready to fall down.
		
	SetSequence( 1 );
	SetAnimTime( gpGlobals->time );
	SetFrameRate( 1.0 );
}


void CSqueak::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/w_sqknest.mdl");
	PRECACHE_MODEL("models/v_squeak.mdl");
	PRECACHE_MODEL("models/p_squeak.mdl");
	PRECACHE_SOUND("squeek/sqk_hunt2.wav");
	PRECACHE_SOUND("squeek/sqk_hunt3.wav");
	UTIL_PrecacheOther("monster_snark");

	m_usSnarkFire = PRECACHE_EVENT ( 1, "events/snarkfire.sc" );
}

bool CSqueak::Deploy()
{
	// play hunt sound
	float flRndSound = RANDOM_FLOAT ( 0 , 1 );

	if ( flRndSound <= 0.5 )
		EMIT_SOUND_DYN( this, CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, 100);
	else 
		EMIT_SOUND_DYN( this, CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, 100);

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

	return DefaultDeploy( "models/v_squeak.mdl", "models/p_squeak.mdl", SQUEAK_UP, "squeak" );
}


void CSqueak::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	
	if ( !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
	{
		m_pPlayer->GetWeapons().ClearFlags( 1 << m_iId );
		SetThink( &CSqueak::DestroyItem );
		SetNextThink( gpGlobals->time + 0.1 );
		return;
	}
	
	SendWeaponAnim( SQUEAK_DOWN );
	EMIT_SOUND( m_pPlayer, CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}


void CSqueak::PrimaryAttack()
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
	{
		UTIL_MakeVectors( m_pPlayer->GetViewAngle() );
		TraceResult tr;
		Vector trace_origin;

		// HACK HACK:  Ugly hacks to handle change in origin based on new physics code for players
		// Move origin up if crouched and start trace a bit outside of body ( 20 units instead of 16 )
		trace_origin = m_pPlayer->GetAbsOrigin();
		if ( m_pPlayer->GetFlags().Any( FL_DUCKING ) )
		{
			trace_origin = trace_origin - ( VEC_HULL_MIN - VEC_DUCK_HULL_MIN );
		}

		// find place to toss monster
		UTIL_TraceLine( trace_origin + gpGlobals->v_forward * 20, trace_origin + gpGlobals->v_forward * 64, dont_ignore_monsters, NULL, &tr );

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	    PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSnarkFire, 0.0, g_vecZero, g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

		if ( tr.fAllSolid == 0 && tr.fStartSolid == 0 && tr.flFraction > 0.25 )
		{
			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

#ifndef CLIENT_DLL
			CBaseEntity *pSqueak = CBaseEntity::Create( "monster_snark", tr.vecEndPos, m_pPlayer->GetViewAngle(), m_pPlayer->edict() );
			pSqueak->SetAbsVelocity( gpGlobals->v_forward * 200 + m_pPlayer->GetAbsVelocity() );
#endif

			// play hunt sound
			float flRndSound = RANDOM_FLOAT ( 0 , 1 );

			if ( flRndSound <= 0.5 )
				EMIT_SOUND_DYN( this, CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, 105);
			else 
				EMIT_SOUND_DYN( this, CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, 105);

			m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

			m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ]--;

			m_fJustThrown = 1;

			m_flNextPrimaryAttack = GetNextAttackDelay(0.3);
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		}
	}
}


void CSqueak::SecondaryAttack( void )
{

}


void CSqueak::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if (m_fJustThrown)
	{
		m_fJustThrown = 0;

		if ( !m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] )
		{
			RetireWeapon();
			return;
		}

		SendWeaponAnim( SQUEAK_UP );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		return;
	}

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
	if (flRand <= 0.75)
	{
		iAnim = SQUEAK_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 30.0 / 16 * (2);
	}
	else if (flRand <= 0.875)
	{
		iAnim = SQUEAK_FIDGETFIT;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 70.0 / 16.0;
	}
	else
	{
		iAnim = SQUEAK_FIDGETNIP;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 80.0 / 16.0;
	}
	SendWeaponAnim( iAnim );
}