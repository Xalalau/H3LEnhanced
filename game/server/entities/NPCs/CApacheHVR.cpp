/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"

#include "CApacheHVR.h"

BEGIN_DATADESC( CApacheHVR )
	//DEFINE_FIELD( m_iTrail, FIELD_INTEGER ),	// Dont' save, precache
	DEFINE_FIELD( m_vecForward, FIELD_VECTOR ),
	DEFINE_THINKFUNC( IgniteThink ),
	DEFINE_THINKFUNC( AccelerateThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( hvr_rocket, CApacheHVR );

void CApacheHVR::Spawn( void )
{
	Precache();
	// motor
	SetMoveType( MOVETYPE_FLY );
	SetSolidType( SOLID_BBOX );

	SetModel( "models/HVR.mdl" );
	SetSize( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );
	SetAbsOrigin( GetAbsOrigin() );

	SetThink( &CApacheHVR::IgniteThink );
	SetTouch( &CApacheHVR::ExplodeTouch );

	UTIL_MakeAimVectors( GetAbsAngles() );
	m_vecForward = gpGlobals->v_forward;
	SetGravity( 0.5 );

	SetNextThink( gpGlobals->time + 0.1 );

	SetDamage( 150 );
}

void CApacheHVR::Precache( void )
{
	PRECACHE_MODEL( "models/HVR.mdl" );
	m_iTrail = PRECACHE_MODEL( "sprites/smoke.spr" );
	PRECACHE_SOUND( "weapons/rocket1.wav" );
}

void CApacheHVR::IgniteThink( void )
{
	// SetMoveType( MOVETYPE_TOSS );

	// SetMoveType( MOVETYPE_FLY );
	GetEffects() |= EF_LIGHT;

	// make rocket sound
	EMIT_SOUND( this, CHAN_VOICE, "weapons/rocket1.wav", 1, 0.5 );

	// rocket trail
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

	WRITE_BYTE( TE_BEAMFOLLOW );
	WRITE_SHORT( entindex() );	// entity
	WRITE_SHORT( m_iTrail );	// model
	WRITE_BYTE( 15 ); // life
	WRITE_BYTE( 5 );  // width
	WRITE_BYTE( 224 );   // r, g, b
	WRITE_BYTE( 224 );   // r, g, b
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 255 );	// brightness

	MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

					// set to accelerate
	SetThink( &CApacheHVR::AccelerateThink );
	SetNextThink( gpGlobals->time + 0.1 );
}

void CApacheHVR::AccelerateThink( void )
{
	// check world boundaries
	if( GetAbsOrigin().x < -4096 || GetAbsOrigin().x > 4096 || GetAbsOrigin().y < -4096 || GetAbsOrigin().y > 4096 || GetAbsOrigin().z < -4096 || GetAbsOrigin().z > 4096 )
	{
		UTIL_Remove( this );
		return;
	}

	// accelerate
	float flSpeed = GetAbsVelocity().Length();
	if( flSpeed < 1800 )
	{
		SetAbsVelocity( GetAbsVelocity() + m_vecForward * 200 );
	}

	// re-aim
	SetAbsAngles( UTIL_VecToAngles( GetAbsVelocity() ) );

	SetNextThink( gpGlobals->time + 0.1 );
}