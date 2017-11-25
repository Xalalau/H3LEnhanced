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

#include "TempEntity.h"

#include "CSpiral.h"

LINK_ENTITY_TO_CLASS( streak_spiral, CSpiral );

void CSpiral::Spawn( void )
{
	SetMoveType( MOVETYPE_NONE );
	SetNextThink( gpGlobals->time );
	SetSolidType( SOLID_NOT );
	SetSize( g_vecZero, g_vecZero );
	GetEffects() |= EF_NODRAW;
	SetAbsAngles( g_vecZero );
}

void CSpiral::Think( void )
{
	float time = gpGlobals->time - GetDamageTime();

	while( time > SPIRAL_INTERVAL )
	{
		Vector position = GetAbsOrigin();
		Vector direction = Vector( 0, 0, 1 );

		float fraction = 1.0 / GetSpeed();

		float radius = ( GetScale() * GetHealth() ) * fraction;

		position.z += ( GetHealth() * GetDamage() ) * fraction;
		Vector vecAngles = GetAbsAngles();
		vecAngles.y = ( GetHealth() * 360 * 8 ) * fraction;
		SetAbsAngles( vecAngles );
		UTIL_MakeVectors( GetAbsAngles() );
		position = position + gpGlobals->v_forward * radius;
		direction = ( direction + gpGlobals->v_forward ).Normalize();

		StreakSplash( position, Vector( 0, 0, 1 ), RANDOM_LONG( 8, 11 ), 20, RANDOM_LONG( 50, 150 ), 400 );

		// Jeez, how many counters should this take ? :)
		SetDamageTime( GetDamageTime() + SPIRAL_INTERVAL );
		SetHealth( GetHealth() + SPIRAL_INTERVAL );
		time -= SPIRAL_INTERVAL;
	}

	SetNextThink( gpGlobals->time );

	if( GetHealth() >= GetSpeed() )
		UTIL_Remove( this );
}

CSpiral *CSpiral::Create( const Vector &origin, float height, float radius, float duration )
{
	if( duration <= 0 )
		return NULL;

	auto pSpiral = static_cast<CSpiral*>( UTIL_CreateNamedEntity( "streak_spiral" ) );
	pSpiral->Spawn();
	pSpiral->SetDamageTime( pSpiral->GetNextThink() );
	pSpiral->SetAbsOrigin( origin );
	pSpiral->SetScale( radius );
	pSpiral->SetDamage( height );
	pSpiral->SetSpeed( duration );
	pSpiral->SetHealth( 0 );
	pSpiral->SetAbsAngles( g_vecZero );

	return pSpiral;
}