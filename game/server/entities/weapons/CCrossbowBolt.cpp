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
#include "Weapons.h"
#include "gamerules/GameRules.h"

#include "CCrossbowBolt.h"

BEGIN_DATADESC( CCrossbowBolt )
	DEFINE_THINKFUNC( BubbleThink ),
	DEFINE_TOUCHFUNC( BoltTouch ),
	DEFINE_THINKFUNC( ExplodeThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( crossbow_bolt, CCrossbowBolt );

CCrossbowBolt *CCrossbowBolt::BoltCreate()
{
	// Create a new entity with CCrossbowBolt private data
	auto pBolt = static_cast<CCrossbowBolt*>( UTIL_CreateNamedEntity( "crossbow_bolt" ) );
	pBolt->Spawn();

	return pBolt;
}

void CCrossbowBolt::Spawn()
{
	Precache();
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	pev->gravity = 0.5;

	SetModel( "models/crossbow_bolt.mdl" );

	SetAbsOrigin( GetAbsOrigin() );
	SetSize( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );

	SetTouch( &CCrossbowBolt::BoltTouch );
	SetThink( &CCrossbowBolt::BubbleThink );
	pev->nextthink = gpGlobals->time + 0.2;
}


void CCrossbowBolt::Precache()
{
	PRECACHE_MODEL( "models/crossbow_bolt.mdl" );
	PRECACHE_SOUND( "weapons/xbow_hitbod1.wav" );
	PRECACHE_SOUND( "weapons/xbow_hitbod2.wav" );
	PRECACHE_SOUND( "weapons/xbow_fly1.wav" );
	PRECACHE_SOUND( "weapons/xbow_hit1.wav" );
	PRECACHE_SOUND( "fvox/beep.wav" );
	m_iTrail = PRECACHE_MODEL( "sprites/streak.spr" );
}


EntityClassification_t CCrossbowBolt::GetClassification()
{
	return EntityClassifications().GetNoneId();
}

void CCrossbowBolt::BoltTouch( CBaseEntity *pOther )
{
	SetTouch( NULL );
	SetThink( NULL );

	if( pOther->pev->takedamage )
	{
		TraceResult tr = UTIL_GetGlobalTrace();

		auto pOwner = Instance( pev->owner );

		// UNDONE: this needs to call TraceAttack instead
		g_MultiDamage.Clear();

		if( pOther->IsPlayer() )
		{
			pOther->TraceAttack( CTakeDamageInfo( pOwner, gSkillData.GetPlrDmgCrossbowClient(), DMG_NEVERGIB ), pev->velocity.Normalize(), &tr );
		}
		else
		{
			pOther->TraceAttack( CTakeDamageInfo( pOwner, gSkillData.GetPlrDmgCrossbowMonster(), DMG_BULLET | DMG_NEVERGIB ), pev->velocity.Normalize(), &tr );
		}

		g_MultiDamage.ApplyMultiDamage( this, pOwner );

		pev->velocity = Vector( 0, 0, 0 );
		// play body "thwack" sound
		switch( RANDOM_LONG( 0, 1 ) )
		{
		case 0:
			EMIT_SOUND( this, CHAN_BODY, "weapons/xbow_hitbod1.wav", 1, ATTN_NORM ); break;
		case 1:
			EMIT_SOUND( this, CHAN_BODY, "weapons/xbow_hitbod2.wav", 1, ATTN_NORM ); break;
		}

		if( !g_pGameRules->IsMultiplayer() )
		{
			Killed( CTakeDamageInfo( this, 0, 0 ), GIB_NEVER );
		}
	}
	else
	{
		EMIT_SOUND_DYN( this, CHAN_BODY, "weapons/xbow_hit1.wav", RANDOM_FLOAT( 0.95, 1.0 ), ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 7 ) );

		SetThink( &CCrossbowBolt::SUB_Remove );
		pev->nextthink = gpGlobals->time;// this will get changed below if the bolt is allowed to stick in what it hit.

		if( pOther->ClassnameIs( "worldspawn" ) )
		{
			// if what we hit is static architecture, can stay around for a while.
			Vector vecDir = pev->velocity.Normalize();
			SetAbsOrigin( GetAbsOrigin() - vecDir * 12 );
			pev->angles = UTIL_VecToAngles( vecDir );
			pev->solid = SOLID_NOT;
			pev->movetype = MOVETYPE_FLY;
			pev->velocity = Vector( 0, 0, 0 );
			pev->avelocity.z = 0;
			pev->angles.z = RANDOM_LONG( 0, 360 );
			pev->nextthink = gpGlobals->time + 10.0;
		}

		if( UTIL_PointContents( GetAbsOrigin() ) != CONTENTS_WATER )
		{
			UTIL_Sparks( GetAbsOrigin() );
		}
	}

	if( g_pGameRules->IsMultiplayer() )
	{
		SetThink( &CCrossbowBolt::ExplodeThink );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CCrossbowBolt::BubbleThink()
{
	pev->nextthink = gpGlobals->time + 0.1;

	if( GetWaterLevel() == WATERLEVEL_DRY )
		return;

	UTIL_BubbleTrail( GetAbsOrigin() - pev->velocity * 0.1, GetAbsOrigin(), 1 );
}

void CCrossbowBolt::ExplodeThink()
{
	int iContents = UTIL_PointContents( GetAbsOrigin() );
	int iScale;

	pev->dmg = 40;
	iScale = 10;

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, GetAbsOrigin() );
	WRITE_BYTE( TE_EXPLOSION );
	WRITE_COORD( GetAbsOrigin().x );
	WRITE_COORD( GetAbsOrigin().y );
	WRITE_COORD( GetAbsOrigin().z );
	if( iContents != CONTENTS_WATER )
	{
		WRITE_SHORT( g_sModelIndexFireball );
	}
	else
	{
		WRITE_SHORT( g_sModelIndexWExplosion );
	}
	WRITE_BYTE( iScale ); // scale * 10
	WRITE_BYTE( 15 ); // framerate
	WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();

	CBaseEntity* pOwner = pev->owner ? Instance( pev->owner ) : nullptr;

	pev->owner = nullptr; // can't traceline attack owner if this is set

	::RadiusDamage( GetAbsOrigin(), CTakeDamageInfo( this, pOwner, pev->dmg, DMG_BLAST | DMG_ALWAYSGIB ), 128, EntityClassifications().GetNoneId() );

	UTIL_Remove( this );
}
