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
#include "CTripmine.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"
#include "Effects.h"
#include "gamerules/GameRules.h"

#define	TRIPMINE_PRIMARY_VOLUME		450

LINK_ENTITY_TO_CLASS( weapon_tripmine, CTripmine );

CTripmine::CTripmine()
	: BaseClass( WEAPON_TRIPMINE )
{
}

void CTripmine::Spawn( )
{
	Precache( );
	SetModel( "models/v_tripmine.mdl");
	SetFrame(0);
	// ############ hu3lifezado ############ //
	// Posicao extra para evitar que a mina pinto de parede entre na chao na forma world (3)
	SetBody(4);
	// ############ //
	SetSequence( TRIPMINE_GROUND );
	// ResetSequenceInfo( );
	SetFrameRate( 0 );

	FallInit();// get ready to fall down

	if ( !bIsMultiplayer() )
	{
		SetSize( Vector(-16, -16, 0), Vector(16, 16, 28) ); 
	}
}

void CTripmine::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL ("models/v_tripmine.mdl");
	PRECACHE_MODEL ("models/p_tripmine.mdl");
	UTIL_PrecacheOther( "monster_tripmine" );

	m_usTripFire = PRECACHE_EVENT( 1, "events/tripfire.sc" );
}

bool CTripmine::Deploy()
{
	SetBody( 0 );
	return DefaultDeploy( "models/v_tripmine.mdl", "models/p_tripmine.mdl", TRIPMINE_DRAW, "trip" );
}


void CTripmine::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if (!m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ])
	{
		// out of mines
		m_pPlayer->GetWeapons().ClearFlags( 1 << m_iId );
		SetThink( &CTripmine::DestroyItem );
		SetNextThink( gpGlobals->time + 0.1 );
	}

	SendWeaponAnim( TRIPMINE_HOLSTER );
	EMIT_SOUND( m_pPlayer, CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}

void CTripmine::PrimaryAttack( void )
{
	if (m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0)
		return;

	UTIL_MakeVectors( m_pPlayer->GetViewAngle() + m_pPlayer->GetPunchAngle() );
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usTripFire, 0.0, g_vecZero, g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

	if (tr.flFraction < 1.0)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if ( pEntity && !pEntity->GetFlags().Any( FL_CONVEYOR ) )
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			/*CBaseEntity *pEnt = */CBaseEntity::Create( "monster_tripmine", tr.vecEndPos + tr.vecPlaneNormal * 8, angles, m_pPlayer->edict() );

			m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ]--;

			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			
			if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0 )
			{
				// no more mines! 
				RetireWeapon();
				return;
			}
		}
		else
		{
			// ALERT( at_console, "no deploy\n" );
		}
	}
	else
	{

	}
	
	m_flNextPrimaryAttack = GetNextAttackDelay(0.3);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

void CTripmine::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 )
	{
		SendWeaponAnim( TRIPMINE_DRAW );
	}
	else
	{
		RetireWeapon(); 
		return;
	}

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
	if (flRand <= 0.25)
	{
		iAnim = TRIPMINE_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 90.0 / 30.0;
	}
	else if (flRand <= 0.75)
	{
		iAnim = TRIPMINE_IDLE2;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 30.0;
	}
	else
	{
		iAnim = TRIPMINE_FIDGET;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 100.0 / 30.0;
	}

	SendWeaponAnim( iAnim );
}




