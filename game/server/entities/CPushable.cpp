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

#include "CPushable.h"

const char* const CPushable::m_soundNames[ 3 ] = { "debris/pushbox1.wav", "debris/pushbox2.wav", "debris/pushbox3.wav" };

BEGIN_DATADESC( CPushable )
	DEFINE_FIELD( m_maxSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_soundTime, FIELD_TIME ),
#if 0
	DEFINE_THINKFUNC( StopSound ),
#endif
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_pushable, CPushable );

void CPushable::Spawn( void )
{
	if( GetSpawnFlags().Any( SF_PUSH_BREAKABLE ) )
		CBreakable::Spawn();
	else
		Precache();

	SetMoveType( MOVETYPE_PUSHSTEP );
	SetSolidType( SOLID_BBOX );
	SetModel( GetModelName() );

	if( GetFriction() > 399 )
		SetFriction( 399 );

	m_maxSpeed = 400 - GetFriction();
	GetFlags() |= FL_FLOAT;
	SetFriction( 0 );

	Vector vecOrigin = GetAbsOrigin();
	vecOrigin.z += 1;	// Pick up off of the floor
	SetAbsOrigin( vecOrigin );

	// Multiply by area of the box's cross-section (assume 1000 units^3 standard volume)
	SetSkin( ( GetSkin() * ( GetRelMax().x - GetRelMin().x ) * ( GetRelMax().y - GetRelMin().y ) ) * 0.0005 );
	m_soundTime = 0;
}

void CPushable::Precache( void )
{
	for( int i = 0; i < 3; i++ )
		PRECACHE_SOUND( m_soundNames[ i ] );

	if( GetSpawnFlags().Any( SF_PUSH_BREAKABLE ) )
		CBreakable::Precache();
}

void CPushable::Touch( CBaseEntity *pOther )
{
	if( pOther->ClassnameIs( "worldspawn" ) )
		return;

	Move( pOther, 1 );
}

void CPushable::Move( CBaseEntity *pOther, int push )
{
	int playerTouch = 0;

	// Is entity standing on this pushable ?
	if( pOther->GetFlags().Any( FL_ONGROUND ) && pOther->GetGroundEntity() == this )
	{
		// Only push if floating
		if( GetWaterLevel() > WATERLEVEL_DRY )
		{
			Vector vecVelocity = GetAbsVelocity();
			vecVelocity.z += pOther->GetAbsVelocity().z * 0.1;
			SetAbsVelocity( vecVelocity );
		}

		return;
	}


	if( pOther->IsPlayer() )
	{
		if( push && !pOther->GetButtons().Any( IN_FORWARD | IN_USE ) )	// Don't push unless the player is pushing forward and NOT use (pull)
			return;
		playerTouch = 1;
	}

	float factor;

	if( playerTouch )
	{
		if( !pOther->GetFlags().Any( FL_ONGROUND ) )	// Don't push away from jumping/falling players unless in water
		{
			if( GetWaterLevel() < WATERLEVEL_FEET )
				return;
			else
				factor = 0.1;
		}
		else
			factor = 1;
	}
	else
		factor = 0.25;

	Vector vecVelocity = GetAbsVelocity();

	vecVelocity.x += pOther->GetAbsVelocity().x * factor;
	vecVelocity.y += pOther->GetAbsVelocity().y * factor;

	float length = sqrt( vecVelocity.x * vecVelocity.x + vecVelocity.y * vecVelocity.y );
	if( push && ( length > MaxSpeed() ) )
	{
		vecVelocity.x = ( vecVelocity.x * MaxSpeed() / length );
		vecVelocity.y = ( vecVelocity.y * MaxSpeed() / length );
	}

	SetAbsVelocity( vecVelocity );

	if( playerTouch )
	{
		vecVelocity = pOther->GetAbsVelocity();

		vecVelocity.x = GetAbsVelocity().x;
		vecVelocity.y = GetAbsVelocity().y;

		pOther->SetAbsVelocity( vecVelocity );

		if( ( gpGlobals->time - m_soundTime ) > 0.7 )
		{
			m_soundTime = gpGlobals->time;
			if( length > 0 && GetFlags().Any( FL_ONGROUND ) )
			{
				m_lastSound = RANDOM_LONG( 0, 2 );
				EMIT_SOUND( this, CHAN_WEAPON, m_soundNames[ m_lastSound ], 0.5, ATTN_NORM );
				//			SetThink( StopSound );
				//			SetNextThink( GetLastThink() + 0.1 );
			}
			else
				STOP_SOUND( this, CHAN_WEAPON, m_soundNames[ m_lastSound ] );
		}
	}
}

void CPushable::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "size" ) )
	{
		int bbox = atoi( pkvd->szValue );
		pkvd->fHandled = true;

		switch( bbox )
		{
		case 0:	// Point
			SetSize( Vector( -8, -8, -8 ), Vector( 8, 8, 8 ) );
			break;

		case 2: // Big Hull!?!?	!!!BUGBUG Figure out what this hull really is
			SetSize( VEC_DUCK_HULL_MIN * 2, VEC_DUCK_HULL_MAX * 2 );
			break;

		case 3: // Player duck
			SetSize( VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX );
			break;

		default:
		case 1: // Player
			SetSize( VEC_HULL_MIN, VEC_HULL_MAX );
			break;
		}

	}
	else if( FStrEq( pkvd->szKeyName, "buoyancy" ) )
	{
		SetSkin( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CBreakable::KeyValue( pkvd );
}

// Pull the func_pushable
void CPushable::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( !pActivator || !pActivator->IsPlayer() )
	{
		if( GetSpawnFlags().Any( SF_PUSH_BREAKABLE ) )
			this->CBreakable::Use( pActivator, pCaller, useType, value );
		return;
	}

	if( pActivator->GetAbsVelocity() != g_vecZero )
		Move( pActivator, 0 );
}

#if 0
void CPushable::StopSound( void )
{
	Vector dist = GetOldOrigin() - GetAbsOrigin();
	if( dist.Length() <= 0 )
		STOP_SOUND( this, CHAN_WEAPON, m_soundNames[ m_lastSound ] );
}
#endif

void CPushable::OnTakeDamage( const CTakeDamageInfo& info )
{
	if( GetSpawnFlags().Any( SF_PUSH_BREAKABLE ) )
		CBreakable::OnTakeDamage( info );
}
