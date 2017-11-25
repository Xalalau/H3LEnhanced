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

#include "CFuncConveyor.h"

LINK_ENTITY_TO_CLASS( func_conveyor, CFuncConveyor );

void CFuncConveyor::Spawn( void )
{
	SetMovedir( this );
	CFuncWall::Spawn();

	if( !GetSpawnFlags().Any( SF_CONVEYOR_VISUAL ) )
		GetFlags().AddFlags( FL_CONVEYOR );

	// HACKHACK - This is to allow for some special effects
	if( GetSpawnFlags().Any( SF_CONVEYOR_NOTSOLID ) )
	{
		SetSolidType( SOLID_NOT );
		SetSkin( 0 );		// Don't want the engine thinking we've got special contents on this brush
	}

	if( GetSpeed() == 0 )
		SetSpeed( 100 );

	UpdateSpeed( GetSpeed() );
}

void CFuncConveyor::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetSpeed( -GetSpeed() );
	UpdateSpeed( GetSpeed() );
}

// HACKHACK -- This is ugly, but encode the speed in the rendercolor to avoid adding more data to the network stream
void CFuncConveyor::UpdateSpeed( float speed )
{
	// Encode it as an integer with 4 fractional bits
	const int speedCode = ( int ) ( fabs( speed ) * 16.0 );

	SetRenderColor( Vector( ( speed < 0 ) ? 1 : 0, speedCode >> 8, speedCode & 0xFF ) );
}