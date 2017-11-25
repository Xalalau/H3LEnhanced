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
#include "animation.h"

#include "CWeaponCycler.h"

LINK_ENTITY_TO_CLASS( cycler_weapon, CWeaponCycler );

CWeaponCycler::CWeaponCycler()
	: BaseClass( WEAPON_NONE )
{
}

void CWeaponCycler::Spawn()
{
	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_NONE );

	PRECACHE_MODEL( GetModelName() );
	SetModel( GetModelName() );
	m_iszModel = MAKE_STRING( GetModelName() );
	m_iModel = GetModelIndex();

	SetAbsOrigin( GetAbsOrigin() );
	SetSize( Vector( -16, -16, 0 ), Vector( 16, 16, 16 ) );
	SetTouch( &CWeaponCycler::DefaultTouch );
}

void CWeaponCycler::PrimaryAttack()
{

	SendWeaponAnim( GetSequence() );

	m_flNextPrimaryAttack = gpGlobals->time + 0.3;
}

void CWeaponCycler::SecondaryAttack( void )
{
	float flFrameRate, flGroundSpeed;

	SetSequence( ( GetSequence() + 1 ) % 8 );

	SetModelIndex( m_iModel );
	void *pmodel = GET_MODEL_PTR( ENT( pev ) );
	GetSequenceInfo( pmodel, pev, flFrameRate, flGroundSpeed );
	SetModelIndex( 0 );

	if( flFrameRate == 0.0 )
	{
		SetSequence( 0 );
	}

	SendWeaponAnim( GetSequence() );

	m_flNextSecondaryAttack = gpGlobals->time + 0.3;
}

bool CWeaponCycler::Deploy()
{
	m_pPlayer->SetViewModelName( m_iszModel );
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	SendWeaponAnim( 0 );
	m_iClip = 0;
	return true;
}

void CWeaponCycler::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
}