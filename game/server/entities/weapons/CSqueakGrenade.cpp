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
#include "entities/weapons/CSqueak.h"
#include "entities/CSoundEnt.h"
#include "gamerules/GameRules.h"

#include "CSqueakGrenade.h"

float CSqueakGrenade::m_flNextBounceSoundTime = 0;

BEGIN_DATADESC( CSqueakGrenade )
	DEFINE_FIELD( m_flDie, FIELD_TIME ),
	DEFINE_FIELD( m_vecTarget, FIELD_VECTOR ),
	DEFINE_FIELD( m_flNextHunt, FIELD_TIME ),
	DEFINE_FIELD( m_flNextHit, FIELD_TIME ),
	DEFINE_FIELD( m_posPrev, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_hOwner, FIELD_EHANDLE ),
	DEFINE_TOUCHFUNC( SuperBounceTouch ),
	DEFINE_THINKFUNC( HuntThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_snark, CSqueakGrenade );

EntityClassification_t CSqueakGrenade::GetClassification()
{
	if( m_iMyClass != EntityClassifications().GetNoneId() )
		return m_iMyClass; // protect against recursion

	if( CBaseEntity* pEnemy = m_hEnemy )
	{
		m_iMyClass = EntityClassifications().GetClassificationId( classify::INSECT ); // no one cares about it

		const auto classId = pEnemy->Classify();

		if( classId == EntityClassifications().GetClassificationId( classify::PLAYER ) ||
			classId == EntityClassifications().GetClassificationId( classify::HUMAN_PASSIVE ) ||
			classId == EntityClassifications().GetClassificationId( classify::HUMAN_MILITARY ) )
		{
			m_iMyClass = EntityClassifications().GetNoneId();
			return EntityClassifications().GetClassificationId( classify::ALIEN_MILITARY ); // barney's get mad, grunts get mad at it
		}

		m_iMyClass = EntityClassifications().GetNoneId();
	}

	return EntityClassifications().GetClassificationId( classify::ALIEN_BIOWEAPON );
}

void CSqueakGrenade::Spawn( void )
{
	Precache();
	// motor
	SetMoveType( MOVETYPE_BOUNCE );
	SetSolidType( SOLID_BBOX );

	SetModel( "models/w_squeak.mdl" );
	SetSize( Vector( -4, -4, 0 ), Vector( 4, 4, 8 ) );
	SetAbsOrigin( GetAbsOrigin() );

	SetTouch( &CSqueakGrenade::SuperBounceTouch );
	SetThink( &CSqueakGrenade::HuntThink );
	SetNextThink( gpGlobals->time + 0.1 );
	m_flNextHunt = gpGlobals->time + 1E6;

	GetFlags() |= FL_MONSTER;
	SetTakeDamageMode( DAMAGE_AIM );
	SetHealth( gSkillData.GetSnarkHealth() );
	SetGravity( 0.5 );
	SetFriction( 0.5 );

	SetDamage( gSkillData.GetSnarkDmgPop() );

	m_flDie = gpGlobals->time + SQUEEK_DETONATE_DELAY;

	m_flFieldOfView = 0; // 180 degrees

	if( GetOwner() )
		m_hOwner = GetOwner();

	m_flNextBounceSoundTime = gpGlobals->time;// reset each time a snark is spawned.

	SetSequence( WSQUEAK_RUN );
	ResetSequenceInfo();
}

void CSqueakGrenade::Precache( void )
{
	PRECACHE_MODEL( "models/w_squeak.mdl" );
	PRECACHE_SOUND( "squeek/sqk_blast1.wav" );
	PRECACHE_SOUND( "common/bodysplat.wav" );
	PRECACHE_SOUND( "squeek/sqk_die1.wav" );
	PRECACHE_SOUND( "squeek/sqk_hunt1.wav" );
	PRECACHE_SOUND( "squeek/sqk_hunt2.wav" );
	PRECACHE_SOUND( "squeek/sqk_hunt3.wav" );
	PRECACHE_SOUND( "squeek/sqk_deploy1.wav" );
}


void CSqueakGrenade::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	SetModelName( iStringNull );// make invisible
	SetThink( &CSqueakGrenade::SUB_Remove );
	SetTouch( NULL );
	SetNextThink( gpGlobals->time + 0.1 );

	// since squeak grenades never leave a body behind, clear out their takedamage now.
	// Squeaks do a bit of radius damage when they pop, and that radius damage will
	// continue to call this function unless we acknowledge the Squeak's death now. (sjb)
	SetTakeDamageMode( DAMAGE_NO );

	// play squeek blast
	EMIT_SOUND_DYN( this, CHAN_ITEM, "squeek/sqk_blast1.wav", 1, 0.5, 0, PITCH_NORM );

	CSoundEnt::InsertSound( bits_SOUND_COMBAT, GetAbsOrigin(), SMALL_EXPLOSION_VOLUME, 3.0 );

	UTIL_BloodDrips( GetAbsOrigin(), g_vecZero, BloodColor(), 80 );

	if( m_hOwner != NULL )
		RadiusDamage( this, m_hOwner, GetDamage(), EntityClassifications().GetNoneId(), DMG_BLAST );
	else
		RadiusDamage( this, this, GetDamage(), EntityClassifications().GetNoneId(), DMG_BLAST );

	// reset owner so death message happens
	if( m_hOwner != NULL )
		SetOwner( m_hOwner );

	CBaseMonster::Killed( info, GIB_ALWAYS );
}

void CSqueakGrenade::GibMonster( void )
{
	EMIT_SOUND_DYN( this, CHAN_VOICE, "common/bodysplat.wav", 0.75, ATTN_NORM, 0, 200 );
}



void CSqueakGrenade::HuntThink( void )
{
	// ALERT( at_console, "think\n" );

	if( !IsInWorld() )
	{
		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}

	StudioFrameAdvance();
	SetNextThink( gpGlobals->time + 0.1 );

	// explode when ready
	if( gpGlobals->time >= m_flDie )
	{
		g_vecAttackDir = GetAbsVelocity().Normalize();
		// ############ hu3lifezado ############ //
		// Barriguinha mole nao morre mais por tempo
		//SetHealth( -1 );
		//Killed( CTakeDamageInfo( this, 0, 0 ), GIB_NORMAL );
		m_flDie = gpGlobals->time + SQUEEK_DETONATE_DELAY;
		// ############ //
		return;
	}

	// float
	if( GetWaterLevel() != WATERLEVEL_DRY )
	{
		if( GetMoveType() == MOVETYPE_BOUNCE )
		{
			SetMoveType( MOVETYPE_FLY );
		}

		Vector vecVelocity = GetAbsVelocity() * 0.9;
		vecVelocity.z += 8.0;
		SetAbsVelocity( vecVelocity );
	}
	else if( GetMoveType() == MOVETYPE_FLY )
	{
		SetMoveType( MOVETYPE_BOUNCE );
	}

	// return if not time to hunt
	if( m_flNextHunt > gpGlobals->time )
		return;

	m_flNextHunt = gpGlobals->time + 2.0;

	Vector vecDir;
	TraceResult tr;

	Vector vecFlat = GetAbsVelocity();
	vecFlat.z = 0;
	vecFlat = vecFlat.Normalize();

	UTIL_MakeVectors( GetAbsAngles() );

	if( m_hEnemy == NULL || !m_hEnemy->IsAlive() )
	{
		// find target, bounce a bit towards it.
		Look( 512 );
		m_hEnemy = BestVisibleEnemy();
	}

	// squeek if it's about time blow up
	if( ( m_flDie - gpGlobals->time <= 0.5 ) && ( m_flDie - gpGlobals->time >= 0.3 ) )
	{
		EMIT_SOUND_DYN( this, CHAN_VOICE, "squeek/sqk_die1.wav", 1, ATTN_NORM, 0, 100 + RANDOM_LONG( 0, 0x3F ) );
		CSoundEnt::InsertSound( bits_SOUND_COMBAT, GetAbsOrigin(), 256, 0.25 );
	}

	// higher pitch as squeeker gets closer to detonation time
	float flpitch = 155.0 - 60.0 * ( ( m_flDie - gpGlobals->time ) / SQUEEK_DETONATE_DELAY );
	if( flpitch < 80 )
		flpitch = 80;

	if( m_hEnemy != NULL )
	{
		if( FVisible( m_hEnemy ) )
		{
			vecDir = m_hEnemy->EyePosition() - GetAbsOrigin();
			m_vecTarget = vecDir.Normalize();
		}

		float flVel = GetAbsVelocity().Length();
		float flAdj = 50.0 / ( flVel + 10.0 );

		if( flAdj > 1.2 )
			flAdj = 1.2;

		// ALERT( at_console, "think : enemy\n");

		// ALERT( at_console, "%.0f %.2f %.2f %.2f\n", flVel, m_vecTarget.x, m_vecTarget.y, m_vecTarget.z );

		SetAbsVelocity( GetAbsVelocity() * flAdj + m_vecTarget * 300 );
	}

	if( GetFlags().Any( FL_ONGROUND ) )
	{
		SetAngularVelocity( g_vecZero );
	}
	else
	{
		if( GetAngularVelocity() == g_vecZero )
		{
			Vector vecAVelocity = GetAngularVelocity();
			vecAVelocity.x = RANDOM_FLOAT( -100, 100 );
			vecAVelocity.z = RANDOM_FLOAT( -100, 100 );
			SetAngularVelocity( vecAVelocity );
		}
	}

	if( ( GetAbsOrigin() - m_posPrev ).Length() < 1.0 )
	{
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.x = RANDOM_FLOAT( -100, 100 );
		vecVelocity.y = RANDOM_FLOAT( -100, 100 );
		SetAbsVelocity( vecVelocity );
	}
	m_posPrev = GetAbsOrigin();

	SetAbsAngles( UTIL_VecToAngles( GetAbsVelocity() ) );
	Vector vecAngles = GetAbsAngles();
	vecAngles.z = 0;
	vecAngles.x = 0;
	SetAbsAngles( vecAngles );
}


void CSqueakGrenade::SuperBounceTouch( CBaseEntity *pOther )
{
	float	flpitch;

	TraceResult tr = UTIL_GetGlobalTrace();

	// don't hit the guy that launched this grenade
	if( GetOwner() && pOther == GetOwner() )
		return;

	// at least until we've bounced once
	SetOwner( NULL );

	Vector vecAngles = GetAbsAngles();
	vecAngles.x = 0;
	vecAngles.z = 0;
	SetAbsAngles( vecAngles );

	// avoid bouncing too much
	if( m_flNextHit > gpGlobals->time )
		return;

	// higher pitch as squeeker gets closer to detonation time
	flpitch = 155.0 - 60.0 * ( ( m_flDie - gpGlobals->time ) / SQUEEK_DETONATE_DELAY );

	if( pOther->GetTakeDamageMode() != DAMAGE_NO && m_flNextAttack < gpGlobals->time )
	{
		// attack!

		// make sure it's me who has touched them
		if( tr.pHit == pOther->edict() )
		{
			// and it's not another squeakgrenade
			if( tr.pHit->v.modelindex != GetModelIndex() )
			{
				// ALERT( at_console, "hit enemy\n");
				g_MultiDamage.Clear();
				pOther->TraceAttack( CTakeDamageInfo( this, gSkillData.GetSnarkDmgBite(), DMG_SLASH ), gpGlobals->v_forward, tr );
				if( m_hOwner != NULL )
					g_MultiDamage.ApplyMultiDamage( this, m_hOwner );
				else
					g_MultiDamage.ApplyMultiDamage( this, this );

				SetDamage( GetDamage() + gSkillData.GetSnarkDmgPop() ); // add more explosion damage
														 // m_flDie += 2.0; // add more life

														 // make bite sound
				EMIT_SOUND_DYN( this, CHAN_WEAPON, "squeek/sqk_deploy1.wav", 1.0, ATTN_NORM, 0, ( int ) flpitch );
				m_flNextAttack = gpGlobals->time + 0.5;
			}
		}
		else
		{
			// ALERT( at_console, "been hit\n");
		}
	}

	m_flNextHit = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time;

	if( g_pGameRules->IsMultiplayer() )
	{
		// in multiplayer, we limit how often snarks can make their bounce sounds to prevent overflows.
		if( gpGlobals->time < m_flNextBounceSoundTime )
		{
			// too soon!
			return;
		}
	}

	if( !GetFlags().Any( FL_ONGROUND ) )
	{
		// play bounce sound
		float flRndSound = RANDOM_FLOAT( 0, 1 );

		if( flRndSound <= 0.33 )
			EMIT_SOUND_DYN( this, CHAN_VOICE, "squeek/sqk_hunt1.wav", 1, ATTN_NORM, 0, ( int ) flpitch );
		else if( flRndSound <= 0.66 )
			EMIT_SOUND_DYN( this, CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, ( int ) flpitch );
		else
			EMIT_SOUND_DYN( this, CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, ( int ) flpitch );
		CSoundEnt::InsertSound( bits_SOUND_COMBAT, GetAbsOrigin(), 256, 0.25 );
	}
	else
	{
		// skittering sound
		CSoundEnt::InsertSound( bits_SOUND_COMBAT, GetAbsOrigin(), 100, 0.1 );
	}

	m_flNextBounceSoundTime = gpGlobals->time + 0.5;// half second.
}
