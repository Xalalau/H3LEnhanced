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

void CBaseToggle::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "lip" ) )
	{
		m_flLip = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		m_flWait = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "master" ) )
	{
		m_sMaster = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "distance" ) )
	{
		m_flMoveDistance = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

/*
=============
LinearMove

calculate GetAbsVelocity() and GetNextThink() to reach vecDest from
GetAbsOrigin() traveling at flSpeed
===============
*/
void CBaseToggle::LinearMove( Vector	vecDest, float flSpeed )
{
	ASSERTSZ( flSpeed != 0, "LinearMove:  no speed is defined!" );
	//	ASSERTSZ(m_pfnCallWhenMoveDone != NULL, "LinearMove: no post-move function defined");

	m_vecFinalDest = vecDest;

	// Already there?
	if( vecDest == GetAbsOrigin() )
	{
		LinearMoveDone();
		return;
	}

	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDest - GetAbsOrigin();

	// divide vector length by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to LinearMoveDone when dest is reached
	SetNextThink( GetLastThink() + flTravelTime );
	SetThink( &CBaseToggle::LinearMoveDone );

	// scale the destdelta vector by the time spent traveling to get velocity
	SetAbsVelocity( vecDestDelta / flTravelTime );
}

/*
============
After moving, set origin to exact final destination, call "move done" function
============
*/
void CBaseToggle::LinearMoveDone( void )
{
	Vector delta = m_vecFinalDest - GetAbsOrigin();
	float error = delta.Length();
	if( error > 0.03125 )
	{
		LinearMove( m_vecFinalDest, 100 );
		return;
	}

	SetAbsOrigin( m_vecFinalDest );
	SetAbsVelocity( g_vecZero );
	SetNextThink( -1 );
	if( m_pfnCallWhenMoveDone )
		( this->*m_pfnCallWhenMoveDone )( );
}

/*
=============
AngularMove

calculate GetAbsVelocity() and GetNextThink() to reach vecDest from
GetAbsOrigin() traveling at flSpeed
Just like LinearMove, but rotational.
===============
*/
void CBaseToggle::AngularMove( Vector vecDestAngle, float flSpeed )
{
	ASSERTSZ( flSpeed != 0, "AngularMove:  no speed is defined!" );
	//	ASSERTSZ(m_pfnCallWhenMoveDone != NULL, "AngularMove: no post-move function defined");

	m_vecFinalAngle = vecDestAngle;

	// Already there?
	if( vecDestAngle == GetAbsAngles() )
	{
		AngularMoveDone();
		return;
	}

	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDestAngle - GetAbsAngles();

	// divide by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to AngularMoveDone when dest is reached
	SetNextThink( GetLastThink() + flTravelTime );
	SetThink( &CBaseToggle::AngularMoveDone );

	// scale the destdelta vector by the time spent traveling to get velocity
	SetAngularVelocity( vecDestDelta / flTravelTime );
}


/*
============
After rotating, set angle to exact final angle, call "move done" function
============
*/
void CBaseToggle::AngularMoveDone( void )
{
	SetAbsAngles( m_vecFinalAngle );
	SetAngularVelocity( g_vecZero );
	SetNextThink( -1 );
	if( m_pfnCallWhenMoveDone )
		( this->*m_pfnCallWhenMoveDone )( );
}

bool CBaseToggle::IsLockedByMaster() const
{
	return m_sMaster && !UTIL_IsMasterTriggered( m_sMaster, m_hActivator );
}

float CBaseToggle::AxisValue( int flags, const Vector &angles )
{
	if( FBitSet( flags, SF_DOOR_ROTATE_Z ) )
		return angles.z;
	if( FBitSet( flags, SF_DOOR_ROTATE_X ) )
		return angles.x;

	return angles.y;
}


void CBaseToggle::AxisDir( CBaseEntity* pEntity )
{
	if( pEntity->GetSpawnFlags().Any( SF_DOOR_ROTATE_Z ) )
		pEntity->SetMoveDir( Vector( 0, 0, 1 ) );	// around z-axis
	else if( pEntity->GetSpawnFlags().Any( SF_DOOR_ROTATE_X ) )
		pEntity->SetMoveDir( Vector( 1, 0, 0 ) );	// around x-axis
	else
		pEntity->SetMoveDir( Vector( 0, 1, 0 ) );	// around y-axis
}


float CBaseToggle::AxisDelta( int flags, const Vector &angle1, const Vector &angle2 )
{
	if( FBitSet( flags, SF_DOOR_ROTATE_Z ) )
		return angle1.z - angle2.z;

	if( FBitSet( flags, SF_DOOR_ROTATE_X ) )
		return angle1.x - angle2.x;

	return angle1.y - angle2.y;
}