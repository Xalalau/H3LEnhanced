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

#include "CShower.h"

LINK_ENTITY_TO_CLASS( spark_shower, CShower );

void CShower::Spawn( void )
{
	Vector vecVelocity = RANDOM_FLOAT( 200, 300 ) * GetAbsAngles();
	vecVelocity.x += RANDOM_FLOAT( -100.f, 100.f );
	vecVelocity.y += RANDOM_FLOAT( -100.f, 100.f );
	if( vecVelocity.z >= 0 )
		vecVelocity.z += 200;
	else
		vecVelocity.z -= 200;

	SetAbsVelocity( vecVelocity );

	SetMoveType( MOVETYPE_BOUNCE );
	SetGravity( 0.5 );
	SetNextThink( gpGlobals->time + 0.1 );
	SetSolidType( SOLID_NOT );
	SetModel( "models/grenade.mdl" );	// Need a model, just use the grenade, we don't draw it anyway
	SetSize( g_vecZero, g_vecZero );
	GetEffects() |= EF_NODRAW;
	SetSpeed( RANDOM_FLOAT( 0.5, 1.5 ) );

	SetAbsAngles( g_vecZero );
}

void CShower::Think( void )
{
	UTIL_Sparks( GetAbsOrigin() );

	SetSpeed( GetSpeed() - 0.1 );
	if( GetSpeed() > 0 )
		SetNextThink( gpGlobals->time + 0.1 );
	else
		UTIL_Remove( this );
	GetFlags().ClearFlags( FL_ONGROUND );
}

void CShower::Touch( CBaseEntity *pOther )
{
	if( GetFlags().Any( FL_ONGROUND ) )
		SetAbsVelocity( GetAbsVelocity() * 0.1 );
	else
		SetAbsVelocity( GetAbsVelocity() * 0.6 );

	if( ( GetAbsVelocity().x*GetAbsVelocity().x + GetAbsVelocity().y*GetAbsVelocity().y ) < 10.0 )
		SetSpeed( 0 );
}