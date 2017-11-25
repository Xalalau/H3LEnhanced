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
/*

===== generic grenade.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"
#include "nodes/Nodes.h"
#include "entities/CSoundEnt.h"
#include "Decals.h"


//===================grenade

BEGIN_DATADESC( CGrenade )
	DEFINE_THINKFUNC( Smoke ),
	DEFINE_TOUCHFUNC( BounceTouch ),
	DEFINE_TOUCHFUNC( SlideTouch ),
	DEFINE_TOUCHFUNC( ExplodeTouch ),
	DEFINE_THINKFUNC( DangerSoundThink ),
	DEFINE_THINKFUNC( PreDetonate ),
	DEFINE_THINKFUNC( Detonate ),
	DEFINE_USEFUNC( DetonateUse ),
	DEFINE_THINKFUNC( TumbleThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( grenade, CGrenade );

// Grenades flagged with this will be triggered when the owner calls detonateSatchelCharges
#define SF_DETONATE		0x0001

//
// Grenade Explode
//
void CGrenade::Explode( Vector vecSrc, Vector vecAim )
{
	TraceResult tr;
	UTIL_TraceLine ( GetAbsOrigin(), GetAbsOrigin() + Vector ( 0, 0, -32 ),  ignore_monsters, ENT(pev), & tr);

	Explode( &tr, DMG_BLAST );
}

// UNDONE: temporary scorching for PreAlpha - find a less sleazy permenant solution.
void CGrenade::Explode( TraceResult *pTrace, int bitsDamageType )
{
	SetModelName( iStringNull );//invisible
	SetSolidType( SOLID_NOT );// intangible

	SetTakeDamageMode( DAMAGE_NO );

	// Pull out of the wall a bit
	if ( pTrace->flFraction != 1.0 )
	{
		SetAbsOrigin( pTrace->vecEndPos + (pTrace->vecPlaneNormal * ( GetDamage() - 24 ) * 0.6) );
	}

	int iContents = UTIL_PointContents ( GetAbsOrigin() );
	
	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, GetAbsOrigin() );
		WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD( GetAbsOrigin().x );	// Send to PAS because of the sound
		WRITE_COORD( GetAbsOrigin().y );
		WRITE_COORD( GetAbsOrigin().z );
		if (iContents != CONTENTS_WATER)
		{
			WRITE_SHORT( g_sModelIndexFireball );
		}
		else
		{
			WRITE_SHORT( g_sModelIndexWExplosion );
		}
		WRITE_BYTE( ( GetDamage() - 50 ) * .60  ); // scale * 10
		WRITE_BYTE( 15  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();

	CSoundEnt::InsertSound ( bits_SOUND_COMBAT, GetAbsOrigin(), NORMAL_EXPLOSION_VOLUME, 3.0 );

	CBaseEntity* pOwner = GetOwner();

	SetOwner( nullptr ); // can't traceline attack owner if this is set

	RadiusDamage( this, pOwner, GetDamage(), EntityClassifications().GetNoneId(), bitsDamageType );

	if ( RANDOM_FLOAT( 0 , 1 ) < 0.5 )
	{
		UTIL_DecalTrace( pTrace, DECAL_SCORCH1 );
	}
	else
	{
		UTIL_DecalTrace( pTrace, DECAL_SCORCH2 );
	}

	//float flRndSound = RANDOM_FLOAT( 0, 1 );// sound randomizer

	switch ( RANDOM_LONG( 0, 2 ) )
	{
		case 0:	EMIT_SOUND( this, CHAN_VOICE, "weapons/debris1.wav", 0.55, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( this, CHAN_VOICE, "weapons/debris2.wav", 0.55, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( this, CHAN_VOICE, "weapons/debris3.wav", 0.55, ATTN_NORM);	break;
	}

	GetEffects() |= EF_NODRAW;
	SetThink( &CGrenade::Smoke );
	SetAbsVelocity( g_vecZero );
	SetNextThink( gpGlobals->time + 0.3 );

	if (iContents != CONTENTS_WATER)
	{
		int sparkCount = RANDOM_LONG(0,3);
		for ( int i = 0; i < sparkCount; i++ )
			Create( "spark_shower", GetAbsOrigin(), pTrace->vecPlaneNormal, NULL );
	}
}


void CGrenade::Smoke( void )
{
	if (UTIL_PointContents ( GetAbsOrigin() ) == CONTENTS_WATER)
	{
		UTIL_Bubbles( GetAbsOrigin() - Vector( 64, 64, 64 ), GetAbsOrigin() + Vector( 64, 64, 64 ), 100 );
	}
	else
	{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, GetAbsOrigin() );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( GetAbsOrigin().x );
			WRITE_COORD( GetAbsOrigin().y );
			WRITE_COORD( GetAbsOrigin().z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( ( GetDamage() - 50 ) * 0.80 ); // scale * 10
			WRITE_BYTE( 12  ); // framerate
		MESSAGE_END();
	}
	UTIL_Remove( this );
}

void CGrenade::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	Detonate( );
}


// Timed grenade, this think is called when time runs out.
void CGrenade::DetonateUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink( &CGrenade::Detonate );
	SetNextThink( gpGlobals->time );
}

void CGrenade::PreDetonate( void )
{
	CSoundEnt::InsertSound ( bits_SOUND_DANGER, GetAbsOrigin(), 400, 0.3 );

	SetThink( &CGrenade::Detonate );
	SetNextThink( gpGlobals->time + 1 );
}


void CGrenade::Detonate( void )
{
	TraceResult tr;
	Vector		vecSpot;// trace starts here!

	vecSpot = GetAbsOrigin() + Vector ( 0 , 0 , 8 );
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -40 ),  ignore_monsters, ENT(pev), & tr);

	Explode( &tr, DMG_BLAST );
}


//
// Contact grenade, explode when it touches something
// 
void CGrenade::ExplodeTouch( CBaseEntity *pOther )
{
	TraceResult tr;
	Vector		vecSpot;// trace starts here!

	pev->enemy = pOther->edict();

	vecSpot = GetAbsOrigin() - GetAbsVelocity().Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + GetAbsVelocity().Normalize() * 64, ignore_monsters, ENT(pev), &tr );

	Explode( &tr, DMG_BLAST );
}


void CGrenade::DangerSoundThink( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	CSoundEnt::InsertSound ( bits_SOUND_DANGER, GetAbsOrigin() + GetAbsVelocity() * 0.5, GetAbsVelocity().Length(), 0.2 );
	SetNextThink( gpGlobals->time + 0.2 );

	if ( GetWaterLevel() != WATERLEVEL_DRY)
	{
		SetAbsVelocity( GetAbsVelocity() * 0.5 );
	}
}


void CGrenade::BounceTouch( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther == GetOwner() )
		return;

	// only do damage if we're moving fairly fast
	if (m_flNextAttack < gpGlobals->time && GetAbsVelocity().Length() > 100)
	{
		if ( auto pOwner = GetOwner() )
		{
			TraceResult tr = UTIL_GetGlobalTrace( );
			g_MultiDamage.Clear( );
			pOther->TraceAttack( CTakeDamageInfo( pOwner, 1, DMG_CLUB ), gpGlobals->v_forward, tr );
			g_MultiDamage.ApplyMultiDamage( this, pOwner );
		}
		m_flNextAttack = gpGlobals->time + 1.0; // debounce
	}

	// SetAngularVelocity( Vector (300, 300, 300) );

	// this is my heuristic for modulating the grenade velocity because grenades dropped purely vertical
	// or thrown very far tend to slow down too quickly for me to always catch just by testing velocity. 
	// trimming the Z velocity a bit seems to help quite a bit.
	Vector vecTestVelocity = GetAbsVelocity();
	vecTestVelocity.z *= 0.45;

	if ( !m_fRegisteredSound && vecTestVelocity.Length() <= 60 )
	{
		//ALERT( at_console, "Grenade Registered!: %f\n", vecTestVelocity.Length() );

		// grenade is moving really slow. It's probably very close to where it will ultimately stop moving. 
		// go ahead and emit the danger sound.
		
		// register a radius louder than the explosion, so we make sure everyone gets out of the way
		CSoundEnt::InsertSound ( bits_SOUND_DANGER, GetAbsOrigin(), GetDamage() / 0.4, 0.3 );
		m_fRegisteredSound = true;
	}

	if( GetFlags().Any( FL_ONGROUND ) )
	{
		// add a bit of static friction
		SetAbsVelocity( GetAbsVelocity() * 0.8 );

		SetSequence( RANDOM_LONG( 1, 1 ) );
	}
	else
	{
		// play bounce sound
		BounceSound();
	}
	SetFrameRate( GetAbsVelocity().Length() / 200.0 );
	if( GetFrameRate() > 1.0 )
		SetFrameRate( 1 );
	else if( GetFrameRate() < 0.5 )
		SetFrameRate( 0 );

}



void CGrenade::SlideTouch( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther == GetOwner() )
		return;

	// SetAngularVelocity( Vector (300, 300, 300) );

	if( GetFlags().Any( FL_ONGROUND ) )
	{
		// add a bit of static friction
		SetAbsVelocity( GetAbsVelocity() * 0.95 );

		if ( GetAbsVelocity().x != 0 || GetAbsVelocity().y != 0)
		{
			// maintain sliding sound
		}
	}
	else
	{
		BounceSound();
	}
}

void CGrenade :: BounceSound( void )
{
	switch ( RANDOM_LONG( 0, 2 ) )
	{
	case 0:	EMIT_SOUND( this, CHAN_VOICE, "weapons/grenade_hit1.wav", 0.25, ATTN_NORM);	break;
	case 1:	EMIT_SOUND( this, CHAN_VOICE, "weapons/grenade_hit2.wav", 0.25, ATTN_NORM);	break;
	case 2:	EMIT_SOUND( this, CHAN_VOICE, "weapons/grenade_hit3.wav", 0.25, ATTN_NORM);	break;
	}
}

void CGrenade :: TumbleThink( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.1 );

	if ( GetDamageTime() - 1 < gpGlobals->time)
	{
		CSoundEnt::InsertSound ( bits_SOUND_DANGER, GetAbsOrigin() + GetAbsVelocity() * ( GetDamageTime() - gpGlobals->time), 400, 0.1 );
	}

	if ( GetDamageTime() <= gpGlobals->time)
	{
		SetThink( &CGrenade::Detonate );
	}
	if ( GetWaterLevel() != WATERLEVEL_DRY )
	{
		SetAbsVelocity( GetAbsVelocity() * 0.5 );
		SetFrameRate( 0.2 );
	}
}


void CGrenade:: Spawn( void )
{
	SetMoveType( MOVETYPE_BOUNCE );
	
	SetSolidType( SOLID_BBOX );

	SetModel( "models/grenade.mdl");
	SetSize( Vector( 0, 0, 0), Vector(0, 0, 0) );

	SetDamage( 100 );
	m_fRegisteredSound = false;
}

CGrenade* CGrenade::GrenadeCreate()
{
	auto pGrenade = static_cast<CGrenade*>( UTIL_CreateNamedEntity( "grenade" ) );

	return pGrenade;
}

CGrenade* CGrenade::ShootContact( CBaseEntity* pOwner, Vector vecStart, Vector vecVelocity )
{
	auto pGrenade = GrenadeCreate();
	pGrenade->Spawn();
	// contact grenades arc lower
	pGrenade->SetGravity( 0.5 );// lower gravity since grenade is aerodynamic and engine doesn't know it.
	pGrenade->SetAbsOrigin( vecStart );
	pGrenade->SetAbsVelocity( vecVelocity );
	pGrenade->SetAbsAngles( UTIL_VecToAngles (pGrenade->GetAbsVelocity() ) );
	pGrenade->SetOwner( pOwner );
	
	// make monsters afaid of it while in the air
	pGrenade->SetThink( &CGrenade::DangerSoundThink );
	pGrenade->SetNextThink( gpGlobals->time );
	
	// Tumble in air
	pGrenade->SetAngularVelocity( Vector( RANDOM_FLOAT( -100, -500 ), 0, 0 ) );
	
	// Explode on contact
	pGrenade->SetTouch( &CGrenade::ExplodeTouch );

	pGrenade->SetDamage( gSkillData.GetPlrDmgM203Grenade() );

	return pGrenade;
}


CGrenade* CGrenade::ShootTimed( CBaseEntity* pOwner, Vector vecStart, Vector vecVelocity, float time )
{
	auto pGrenade = GrenadeCreate();
	pGrenade->Spawn();
	pGrenade->SetAbsOrigin( vecStart );
	pGrenade->SetAbsVelocity( vecVelocity );
	pGrenade->SetAbsAngles( UTIL_VecToAngles(pGrenade->GetAbsVelocity() ) );
	pGrenade->SetOwner( pOwner );
	
	pGrenade->SetTouch( &CGrenade::BounceTouch );	// Bounce if touched
	
	// Take one second off of the desired detonation time and set the think to PreDetonate. PreDetonate
	// will insert a DANGER sound into the world sound list and delay detonation for one second so that 
	// the grenade explodes after the exact amount of time specified in the call to ShootTimed(). 

	pGrenade->SetDamageTime( gpGlobals->time + time );
	pGrenade->SetThink( &CGrenade::TumbleThink );
	pGrenade->SetNextThink( gpGlobals->time + 0.1 );
	if (time < 0.1)
	{
		pGrenade->SetNextThink( gpGlobals->time );
		pGrenade->SetAbsVelocity( Vector( 0, 0, 0 ) );
	}
		
	pGrenade->SetSequence( RANDOM_LONG( 3, 6 ) );
	pGrenade->SetFrameRate( 1.0 );

	// Tumble through the air
	// pGrenade->SetAngularVelocity( Vector( -400, 0, 0 ) );

	pGrenade->SetGravity( 0.5 );
	pGrenade->SetFriction( 0.8 );

	pGrenade->SetModel( "models/w_grenade.mdl");
	pGrenade->SetDamage( 100 );

	return pGrenade;
}

//======================end grenade

