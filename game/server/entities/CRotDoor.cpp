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

#include "CRotDoor.h"

LINK_ENTITY_TO_CLASS( func_door_rotating, CRotDoor );

void CRotDoor::Spawn( void )
{
	Precache();
	// set the axis of rotation
	CBaseToggle::AxisDir( this );

	// check for clockwise rotation
	if( GetSpawnFlags().Any( SF_DOOR_ROTATE_BACKWARDS ) )
		SetMoveDir( GetMoveDir() * -1 );

	//m_flWait			= 2; who the hell did this? (sjb)
	m_vecAngle1 = GetAbsAngles();
	m_vecAngle2 = GetAbsAngles() + GetMoveDir() * m_flMoveDistance;

	ASSERTSZ( m_vecAngle1 != m_vecAngle2, "rotating door start/end positions are equal" );

	if( GetSpawnFlags().Any( SF_DOOR_PASSABLE ) )
		SetSolidType( SOLID_NOT );
	else
		SetSolidType( SOLID_BSP );

	SetMoveType( MOVETYPE_PUSH );
	SetAbsOrigin( GetAbsOrigin() );
	SetModel( GetModelName() );

	if( GetSpeed() == 0 )
		SetSpeed( 100 );

	// DOOR_START_OPEN is to allow an entity to be lighted in the closed position
	// but spawn in the open position
	if( GetSpawnFlags().Any( SF_DOOR_START_OPEN ) )
	{	// swap pos1 and pos2, put door at pos2, invert movement direction
		SetAbsAngles( m_vecAngle2 );
		Vector vecSav = m_vecAngle1;
		m_vecAngle2 = m_vecAngle1;
		m_vecAngle1 = vecSav;
		SetMoveDir( GetMoveDir() * -1 );
	}

	m_toggle_state = TS_AT_BOTTOM;

	if( GetSpawnFlags().Any( SF_DOOR_USE_ONLY ) )
	{
		SetTouch( NULL );
	}
	else // touchable button
		SetTouch( &CRotDoor::DoorTouch );
}


void CRotDoor::SetToggleState( int state )
{
	if( state == TS_AT_TOP )
		SetAbsAngles( m_vecAngle2 );
	else
		SetAbsAngles( m_vecAngle1 );

	SetAbsOrigin( GetAbsOrigin() );
}
