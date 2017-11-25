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
#include "CBaseButton.h"

#include "CRotButton.h"

LINK_ENTITY_TO_CLASS( func_rot_button, CRotButton );

void CRotButton::Spawn( void )
{
	const char* pszSound;
	//----------------------------------------------------
	//determine sounds for buttons
	//a sound of 0 should not make a sound
	//----------------------------------------------------
	pszSound = ButtonSound( m_sounds );
	PRECACHE_SOUND( pszSound );
	pev->noise = ALLOC_STRING( pszSound );

	// set the axis of rotation
	CBaseToggle::AxisDir( this );

	// check for clockwise rotation
	if( GetSpawnFlags().Any( SF_DOOR_ROTATE_BACKWARDS ) )
		SetMoveDir( GetMoveDir() * -1 );

	SetMoveType( MOVETYPE_PUSH );

	if( GetSpawnFlags().Any( SF_ROTBUTTON_NOTSOLID ) )
		SetSolidType( SOLID_NOT );
	else
		SetSolidType( SOLID_BSP );

	SetModel( GetModelName() );

	if( GetSpeed() == 0 )
		SetSpeed( 40 );

	if( m_flWait == 0 )
		m_flWait = 1;

	if( GetHealth() > 0 )
	{
		SetTakeDamageMode( DAMAGE_YES );
	}

	m_toggle_state = TS_AT_BOTTOM;
	m_vecAngle1 = GetAbsAngles();
	m_vecAngle2 = GetAbsAngles() + GetMoveDir() * m_flMoveDistance;
	ASSERTSZ( m_vecAngle1 != m_vecAngle2, "rotating button start/end positions are equal" );

	m_fStayPushed = m_flWait == -1;
	m_fRotating = true;

	// if the button is flagged for USE button activation only, take away it's touch function and add a use function
	if( !GetSpawnFlags().Any( SF_BUTTON_TOUCH_ONLY ) )
	{
		SetTouch( NULL );
		SetUse( &CRotButton::ButtonUse );
	}
	else // touchable button
		SetTouch( &CRotButton::ButtonTouch );

	//SetTouch( ButtonTouch );
}