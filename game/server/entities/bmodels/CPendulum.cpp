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
#include "entities/DoorConstants.h"

#include "CPendulum.h"

BEGIN_DATADESC( CPendulum )
	DEFINE_FIELD( m_accel, FIELD_FLOAT ),
	DEFINE_FIELD( m_distance, FIELD_FLOAT ),
	DEFINE_FIELD( m_time, FIELD_TIME ),
	DEFINE_FIELD( m_damp, FIELD_FLOAT ),
	DEFINE_FIELD( m_maxSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_dampSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_center, FIELD_VECTOR ),
	DEFINE_FIELD( m_start, FIELD_VECTOR ),
	DEFINE_THINKFUNC( Swing ),
	DEFINE_USEFUNC( PendulumUse ),
	DEFINE_THINKFUNC( Stop ),
	DEFINE_TOUCHFUNC( RopeTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_pendulum, CPendulum );

void CPendulum::Spawn( void )
{
	// set the axis of rotation
	CBaseToggle::AxisDir( this );

	if( GetSpawnFlags().Any( SF_DOOR_PASSABLE ) )
		SetSolidType( SOLID_NOT );
	else
		SetSolidType( SOLID_BSP );
	SetMoveType( MOVETYPE_PUSH );
	SetAbsOrigin( GetAbsOrigin() );
	SetModel( GetModelName() );

	if( m_distance == 0 )
		return;

	if( GetSpeed() == 0 )
		SetSpeed( 100 );

	m_accel = ( GetSpeed() * GetSpeed() ) / ( 2 * fabs( m_distance ) );	// Calculate constant acceleration from speed and distance
	m_maxSpeed = GetSpeed();
	m_start = GetAbsAngles();
	m_center = GetAbsAngles() + ( m_distance * 0.5 ) * GetMoveDir();

	if( GetSpawnFlags().Any( SF_BRUSH_ROTATE_INSTANT ) )
	{
		SetThink( &CPendulum::SUB_CallUseToggle );
		SetNextThink( gpGlobals->time + 0.1 );
	}
	SetSpeed( 0 );
	SetUse( &CPendulum::PendulumUse );

	if( GetSpawnFlags().Any( SF_PENDULUM_SWING ) )
	{
		SetTouch( &CPendulum::RopeTouch );
	}
}

void CPendulum::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "distance" ) )
	{
		m_distance = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "damp" ) )
	{
		m_damp = atof( pkvd->szValue ) * 0.001;
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CPendulum::Swing( void )
{
	float delta, dt;

	delta = CBaseToggle::AxisDelta( GetSpawnFlags().Get(), GetAbsAngles(), m_center );
	dt = gpGlobals->time - m_time;	// How much time has passed?
	m_time = gpGlobals->time;		// Remember the last time called

	if( delta > 0 && m_accel > 0 )
		SetSpeed( GetSpeed() - m_accel * dt );	// Integrate velocity
	else
		SetSpeed( GetSpeed() + m_accel * dt );

	if( GetSpeed() > m_maxSpeed )
		SetSpeed( m_maxSpeed );
	else if( GetSpeed() < -m_maxSpeed )
		SetSpeed( -m_maxSpeed );
	// scale the destdelta vector by the time spent traveling to get velocity
	SetAngularVelocity( GetSpeed() * GetMoveDir() );

	// Call this again
	SetNextThink( GetLastThink() + 0.1 );

	if( m_damp )
	{
		m_dampSpeed -= m_damp * m_dampSpeed * dt;
		if( m_dampSpeed < 30.0 )
		{
			SetAbsAngles( m_center );
			SetSpeed( 0 );
			SetThink( NULL );
			SetAngularVelocity( g_vecZero );
		}
		else if( GetSpeed() > m_dampSpeed )
			SetSpeed( m_dampSpeed );
		else if( GetSpeed() < -m_dampSpeed )
			SetSpeed( -m_dampSpeed );

	}
}

void CPendulum::PendulumUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( GetSpeed() )		// Pendulum is moving, stop it and auto-return if necessary
	{
		if( GetSpawnFlags().Any( SF_PENDULUM_AUTO_RETURN ) )
		{
			float	delta;

			delta = CBaseToggle::AxisDelta( GetSpawnFlags().Get(), GetAbsAngles(), m_start );

			SetAngularVelocity( m_maxSpeed * GetMoveDir() );
			SetNextThink( GetLastThink() + ( delta / m_maxSpeed ) );
			SetThink( &CPendulum::Stop );
		}
		else
		{
			SetSpeed( 0 );		// Dead stop
			SetThink( NULL );
			SetAngularVelocity( g_vecZero );
		}
	}
	else
	{
		SetNextThink( GetLastThink() + 0.1 );		// Start the pendulum moving
		m_time = gpGlobals->time;		// Save time to calculate dt
		SetThink( &CPendulum::Swing );
		m_dampSpeed = m_maxSpeed;
	}
}

void CPendulum::Stop( void )
{
	SetAbsAngles( m_start );
	SetSpeed( 0 );
	SetThink( NULL );
	SetAngularVelocity( g_vecZero );
}

void CPendulum::Touch( CBaseEntity *pOther )
{
	if( GetDamage() <= 0 )
		return;

	// we can't hurt this thing, so we're not concerned with it
	if( pOther->GetTakeDamageMode() == DAMAGE_NO )
		return;

	// calculate damage based on rotation speed
	float damage = GetDamage() * GetSpeed() * 0.01;

	if( damage < 0 )
		damage = -damage;

	pOther->TakeDamage( this, this, damage, DMG_CRUSH );

	pOther->SetAbsVelocity( ( pOther->GetAbsOrigin() - VecBModelOrigin( this ) ).Normalize() * damage );
}

void CPendulum::RopeTouch( CBaseEntity *pOther )
{
	if( !pOther->IsPlayer() )
	{// not a player!
		ALERT( at_console, "Not a client\n" );
		return;
	}

	if( ENT( pOther->pev ) == pev->enemy )
	{// this player already on the rope.
		return;
	}

	pev->enemy = pOther->edict();
	pOther->SetAbsVelocity( g_vecZero );
	pOther->SetMoveType( MOVETYPE_NONE );
}

void CPendulum::Blocked( CBaseEntity *pOther )
{
	m_time = gpGlobals->time;
}