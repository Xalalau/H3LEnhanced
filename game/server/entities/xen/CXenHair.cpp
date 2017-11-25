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

#include "CXenHair.h"

LINK_ENTITY_TO_CLASS( xen_hair, CXenHair );

void CXenHair::Spawn( void )
{
	Precache();
	SetModel( "models/hair.mdl" );
	SetSize( Vector( -4, -4, 0 ), Vector( 4, 4, 32 ) );
	SetSequence( 0 );

	if( !GetSpawnFlags().Any( SF_HAIR_SYNC ) )
	{
		SetFrame( RANDOM_FLOAT( 0, 255 ) );
		SetFrameRate( RANDOM_FLOAT( 0.7, 1.4 ) );
	}
	ResetSequenceInfo();

	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	SetNextThink( gpGlobals->time + RANDOM_FLOAT( 0.1, 0.4 ) );	// Load balance these a bit
}

void CXenHair::Precache( void )
{
	PRECACHE_MODEL( "models/hair.mdl" );
}

void CXenHair::Think( void )
{
	StudioFrameAdvance();
	SetNextThink( gpGlobals->time + 0.5 );
}