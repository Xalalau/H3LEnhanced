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
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"

#include "CAirtank.h"

const float CAirtank::DEFAULT_AIR_TIME = 12.0f;
const float CAirtank::DEFAULT_BASE_RECHARGE_TIME = 18.0f;

BEGIN_DATADESC(	CAirtank )
	DEFINE_FIELD( m_bState, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bDisableAccumRecharge, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flAirTimeToGive, FIELD_FLOAT ),
	DEFINE_FIELD( m_flBaseRechargeTime, FIELD_FLOAT ),
	DEFINE_THINKFUNC( TankThink ),
	DEFINE_TOUCHFUNC( TankTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_airtank, CAirtank );

void CAirtank::KeyValue( KeyValueData* pkvd )
{
	if( FStrEq( pkvd->szKeyName, "DisableAccumRecharge" ) )
	{
		m_bDisableAccumRecharge = atoi( pkvd->szValue ) != 0;

		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "AirTime" ) )
	{
		m_flAirTimeToGive = fabs( atof( pkvd->szValue ) );

		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "BaseRechargeTime" ) )
	{
		m_flBaseRechargeTime = fabs( atof( pkvd->szValue ) );

		pkvd->fHandled = true;
	}
	else
		BaseClass::KeyValue( pkvd );
}

void CAirtank::Spawn()
{
	Precache( );
	// motor
	SetMoveType( MOVETYPE_FLY );
	SetSolidType( SOLID_BBOX );

	SetModel( "models/w_oxygen.mdl");
	SetSize( Vector( -16, -16, 0), Vector(16, 16, 36) );
	SetAbsOrigin( GetAbsOrigin() );

	SetTouch( &CAirtank::TankTouch );
	SetThink( &CAirtank::TankThink );

	GetFlags() |= FL_MONSTER;
	SetTakeDamageMode( DAMAGE_YES );
	SetHealth( 20 );
	SetDamage( 50 );
	m_bState = true;
}

void CAirtank::Precache()
{
	PRECACHE_MODEL("models/w_oxygen.mdl");
	PRECACHE_SOUND("doors/aliendoor3.wav");
}

void CAirtank::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	SetOwner( info.GetAttacker() );

	// UNDONE: this should make a big bubble cloud, not an explosion

	Explode( GetAbsOrigin(), Vector( 0, 0, -1 ) );
}


void CAirtank::TankThink()
{
	// Fire trigger
	m_bState = true;
	SUB_UseTargets( this, USE_TOGGLE, 0 );
}

void CAirtank::TankTouch( CBaseEntity *pOther )
{
	if( !pOther->IsPlayer() )
		return;

	if( !m_bState )
	{
		// "no oxygen" sound
		EMIT_SOUND( this, CHAN_BODY, "player/pl_swim2.wav", 1.0, ATTN_NORM );
		return;
	}
		
	// give player N more seconds of air
	pOther->SetAirFinishedTime( gpGlobals->time + m_flAirTimeToGive );

	// suit recharge sound
	EMIT_SOUND( this, CHAN_VOICE, "doors/aliendoor3.wav", 1.0, ATTN_NORM );

	// recharge airtank in M seconds
	float flRechargeTime = m_flBaseRechargeTime;

	//This lets mappers recharge the tank before players have run out of air.
	//Useful when they want to force them to stay in place. - Solokiller
	if( !m_bDisableAccumRecharge )
	{
		flRechargeTime += m_flAirTimeToGive;
	}

	ALERT( at_console, "giving %f more seconds of air\n", m_flAirTimeToGive );

	SetNextThink( gpGlobals->time + flRechargeTime );
	m_bState = false;
	SUB_UseTargets( this, USE_TOGGLE, 1 );
}
