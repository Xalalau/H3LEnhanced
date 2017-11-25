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
#include "CBaseDoor.h"

#include "CMomentaryDoor.h"

BEGIN_DATADESC( CMomentaryDoor )
	DEFINE_FIELD( m_bMoveSnd, FIELD_CHARACTER ),
	DEFINE_THINKFUNC( DoorMoveDone ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( momentary_door, CMomentaryDoor );

void CMomentaryDoor::Spawn( void )
{
	SetMovedir( this );

	SetSolidType( SOLID_BSP );
	SetMoveType( MOVETYPE_PUSH );

	SetAbsOrigin( GetAbsOrigin() );
	SetModel( GetModelName() );

	if( GetSpeed() == 0 )
		SetSpeed( 100 );
	if( GetDamage() == 0 )
		SetDamage( 2 );

	m_vecPosition1 = GetAbsOrigin();
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2 = m_vecPosition1 + ( GetMoveDir() * ( fabs( GetMoveDir().x * ( GetBounds().x - 2 ) ) + fabs( GetMoveDir().y * ( GetBounds().y - 2 ) ) + fabs( GetMoveDir().z * ( GetBounds().z - 2 ) ) - m_flLip ) );
	ASSERTSZ( m_vecPosition1 != m_vecPosition2, "door start/end positions are equal" );

	if( GetSpawnFlags().Any( SF_DOOR_START_OPEN ) )
	{	// swap pos1 and pos2, put door at pos2
		SetAbsOrigin( m_vecPosition2 );
		m_vecPosition2 = m_vecPosition1;
		m_vecPosition1 = GetAbsOrigin();
	}
	SetTouch( NULL );

	Precache();
}

void CMomentaryDoor::Precache( void )
{
	// set the door's "in-motion" sound
	//As a bonus, doormove9 and 10 are now available - Solokiller
	//Not much use here, but if people need it...
	const char* pszSound = DoorMoveSound( m_bMoveSnd );

	PRECACHE_SOUND( pszSound );

	pev->noiseMoving = MAKE_STRING( pszSound );
}

void CMomentaryDoor::KeyValue( KeyValueData *pkvd )
{

	if( FStrEq( pkvd->szKeyName, "movesnd" ) )
	{
		m_bMoveSnd = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "stopsnd" ) )
	{
		//		m_bStopSnd = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "healthvalue" ) )
	{
		//		m_bHealthValue = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CMomentaryDoor::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( useType != USE_SET )		// Momentary buttons will pass down a float in here
		return;

	if( value > 1.0 )
		value = 1.0;
	if( value < 0.0 )
		value = 0.0;

	Vector move = m_vecPosition1 + ( value * ( m_vecPosition2 - m_vecPosition1 ) );

	Vector delta = move - GetAbsOrigin();
	//float speed = delta.Length() * 10;
	float speed = delta.Length() / 0.1; // move there in 0.1 sec
	if( speed == 0 )
		return;

	// This entity only thinks when it moves, so if it's thinking, it's in the process of moving
	// play the sound when it starts moving (not yet thinking)
	if( GetNextThink() < GetLastThink() || GetNextThink() == 0 )
		EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMoving ), 1, ATTN_NORM );
	// If we already moving to designated point, return
	else if( move == m_vecFinalDest )
		return;

	SetMoveDone( &CMomentaryDoor::DoorMoveDone );
	LinearMove( move, speed );
}

//
// The door has reached needed position.
//
void CMomentaryDoor::DoorMoveDone( void )
{
	STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMoving ) );
	EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseArrived ), 1, ATTN_NORM );
}
