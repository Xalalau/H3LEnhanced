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
#include "entities/weapons/CTripmine.h"

#include "CTripmineGrenade.h"

BEGIN_DATADESC( CTripmineGrenade )
	DEFINE_FIELD( m_flPowerUp, FIELD_TIME ),
	DEFINE_FIELD( m_vecDir, FIELD_VECTOR ),
	DEFINE_FIELD( m_vecEnd, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_flBeamLength, FIELD_FLOAT ),
	DEFINE_FIELD( m_hOwner, FIELD_EHANDLE ),
	DEFINE_FIELD( m_pBeam, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_posOwner, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_angleOwner, FIELD_VECTOR ),
	DEFINE_FIELD( m_pRealOwner, FIELD_EDICT ),
	DEFINE_THINKFUNC( WarningThink ),
	DEFINE_THINKFUNC( PowerupThink ),
	DEFINE_THINKFUNC( BeamBreakThink ),
	DEFINE_THINKFUNC( DelayDeathThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_tripmine, CTripmineGrenade );

void CTripmineGrenade::Spawn()
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_NOT;

	SetModel( "models/v_tripmine.mdl" );
	pev->frame = 0;
	pev->body = 3;
	pev->sequence = TRIPMINE_WORLD;
	ResetSequenceInfo();
	pev->framerate = 0;

	SetSize( Vector( -8, -8, -8 ), Vector( 8, 8, 8 ) );
	SetAbsOrigin( GetAbsOrigin() );

	if( pev->spawnflags & 1 )
	{
		// power up quickly
		m_flPowerUp = gpGlobals->time + 1.0;
	}
	else
	{
		// power up in 2.5 seconds
		m_flPowerUp = gpGlobals->time + 2.5;
	}

	SetThink( &CTripmineGrenade::PowerupThink );
	pev->nextthink = gpGlobals->time + 0.2;

	pev->takedamage = DAMAGE_YES;
	pev->dmg = gSkillData.GetPlrDmgTripmine();
	pev->health = 1; // don't let die normally

	if( pev->owner != NULL )
	{
		// play deploy sound
		EMIT_SOUND( this, CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM );
		EMIT_SOUND( this, CHAN_BODY, "weapons/mine_charge.wav", 0.2, ATTN_NORM ); // chargeup

		m_pRealOwner = pev->owner;// see CTripmineGrenade for why.
	}

	UTIL_MakeAimVectors( pev->angles );

	m_vecDir = gpGlobals->v_forward;
	m_vecEnd = GetAbsOrigin() + m_vecDir * 2048;
}


void CTripmineGrenade::Precache()
{
	PRECACHE_MODEL( "models/v_tripmine.mdl" );
	PRECACHE_SOUND( "weapons/mine_deploy.wav" );
	PRECACHE_SOUND( "weapons/mine_activate.wav" );
	PRECACHE_SOUND( "weapons/mine_charge.wav" );
}

void CTripmineGrenade::UpdateOnRemove()
{
	BaseClass::UpdateOnRemove();

	KillBeam();
}

void CTripmineGrenade::WarningThink()
{
	// play warning sound
	// EMIT_SOUND( this, CHAN_VOICE, "buttons/Blip2.wav", 1.0, ATTN_NORM );

	// set to power up
	SetThink( &CTripmineGrenade::PowerupThink );
	pev->nextthink = gpGlobals->time + 1.0;
}


void CTripmineGrenade::PowerupThink()
{
	TraceResult tr;

	if( m_hOwner == NULL )
	{
		// find an owner
		edict_t *oldowner = pev->owner;
		pev->owner = NULL;
		UTIL_TraceLine( GetAbsOrigin() + m_vecDir * 8, GetAbsOrigin() - m_vecDir * 32, dont_ignore_monsters, ENT( pev ), &tr );
		if( tr.fStartSolid || ( oldowner && tr.pHit == oldowner ) )
		{
			pev->owner = oldowner;
			m_flPowerUp += 0.1;
			pev->nextthink = gpGlobals->time + 0.1;
			return;
		}
		if( tr.flFraction < 1.0 )
		{
			pev->owner = tr.pHit;
			m_hOwner = CBaseEntity::Instance( pev->owner );
			m_posOwner = m_hOwner->GetAbsOrigin();
			m_angleOwner = m_hOwner->pev->angles;
		}
		else
		{
			STOP_SOUND( this, CHAN_VOICE, "weapons/mine_deploy.wav" );
			STOP_SOUND( this, CHAN_BODY, "weapons/mine_charge.wav" );
			SetThink( &CTripmineGrenade::SUB_Remove );
			pev->nextthink = gpGlobals->time + 0.1;
			ALERT( at_console, "WARNING:Tripmine at %.0f, %.0f, %.0f removed\n", GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z );
			KillBeam();
			return;
		}
	}
	else if( m_posOwner != m_hOwner->GetAbsOrigin() || m_angleOwner != m_hOwner->pev->angles )
	{
		// disable
		STOP_SOUND( this, CHAN_VOICE, "weapons/mine_deploy.wav" );
		STOP_SOUND( this, CHAN_BODY, "weapons/mine_charge.wav" );
		CBaseEntity *pMine = Create( "weapon_tripmine", GetAbsOrigin() + m_vecDir * 24, pev->angles );
		pMine->pev->spawnflags |= SF_NORESPAWN;

		SetThink( &CTripmineGrenade::SUB_Remove );
		KillBeam();
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}
	// ALERT( at_console, "%d %.0f %.0f %0.f\n", pev->owner, m_pOwner->GetAbsOrigin().x, m_pOwner->GetAbsOrigin().y, m_pOwner->GetAbsOrigin().z );

	if( gpGlobals->time > m_flPowerUp )
	{
		// make solid
		pev->solid = SOLID_BBOX;
		SetAbsOrigin( GetAbsOrigin() );

		MakeBeam();

		// play enabled sound
		EMIT_SOUND_DYN( this, CHAN_VOICE, "weapons/mine_activate.wav", 0.5, ATTN_NORM, 1.0, 75 );
	}
	pev->nextthink = gpGlobals->time + 0.1;
}


void CTripmineGrenade::KillBeam()
{
	if( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
}


void CTripmineGrenade::MakeBeam()
{
	TraceResult tr;

	// ALERT( at_console, "serverflags %f\n", gpGlobals->serverflags );

	UTIL_TraceLine( GetAbsOrigin(), m_vecEnd, dont_ignore_monsters, ENT( pev ), &tr );

	m_flBeamLength = tr.flFraction;

	// set to follow laser spot
	SetThink( &CTripmineGrenade::BeamBreakThink );
	pev->nextthink = gpGlobals->time + 0.1;

	Vector vecTmpEnd = GetAbsOrigin() + m_vecDir * 2048 * m_flBeamLength;

	m_pBeam = CBeam::BeamCreate( g_pModelNameLaser, 10 );
	m_pBeam->PointEntInit( vecTmpEnd, entindex() );
	// ############ hu3lifezado ############ //
	// Jato de porra ( 0, 214, 198 )
	m_pBeam->SetColor( 255, 255, 255 );
	// Esporrada na parede
	CBaseEntity *pEntity = NULL;
	TraceResult *pTrace;
	pTrace = &tr;
	if ( !FNullEnt(pTrace->pHit) )
		pEntity = CBaseEntity::Instance(pTrace->pHit);
	UTIL_GunshotDecalTrace( &tr, RANDOM_LONG(33,34) ); // DECAL_SPIT1 e 2
	// ############ //
	m_pBeam->SetScrollRate( 255 );
	m_pBeam->SetBrightness( 64 );
}


void CTripmineGrenade::BeamBreakThink()
{
	bool bBlowup = false;

	TraceResult tr;

	// HACKHACK Set simple box using this really nice global!
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
	UTIL_TraceLine( GetAbsOrigin(), m_vecEnd, dont_ignore_monsters, ENT( pev ), &tr );

	// ALERT( at_console, "%f : %f\n", tr.flFraction, m_flBeamLength );

	// respawn detect. 
	if( !m_pBeam )
	{
		MakeBeam();
		if( tr.pHit )
			m_hOwner = CBaseEntity::Instance( tr.pHit );	// reset owner too
	}

	if( fabs( m_flBeamLength - tr.flFraction ) > 0.001 )
	{
		bBlowup = true;
	}
	else
	{
		if( m_hOwner == NULL )
			bBlowup = true;
		else if( m_posOwner != m_hOwner->GetAbsOrigin() )
			bBlowup = true;
		else if( m_angleOwner != m_hOwner->pev->angles )
			bBlowup = true;
	}

	if( bBlowup )
	{
		// a bit of a hack, but all CGrenade code passes pev->owner along to make sure the proper player gets credit for the kill
		// so we have to restore pev->owner from pRealOwner, because an entity's tracelines don't strike it's pev->owner which meant
		// that a player couldn't trigger his own tripmine. Now that the mine is exploding, it's safe the restore the owner so the 
		// CGrenade code knows who the explosive really belongs to.
		pev->owner = m_pRealOwner;
		pev->health = 0;
		Killed( CTakeDamageInfo( Instance( pev->owner ), 0, 0 ), GIB_NORMAL );
		return;
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

void CTripmineGrenade::OnTakeDamage( const CTakeDamageInfo& info )
{
	if( gpGlobals->time < m_flPowerUp && info.GetDamage() < pev->health )
	{
		// disable
		// Create( "weapon_tripmine", GetAbsOrigin() + m_vecDir * 24, pev->angles );
		SetThink( &CTripmineGrenade::SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.1;
		KillBeam();
		return;
	}
	CGrenade::OnTakeDamage( info );
}

void CTripmineGrenade::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	pev->takedamage = DAMAGE_NO;

	if( info.GetAttacker() && ( info.GetAttacker()->pev->flags & FL_CLIENT ) )
	{
		// some client has destroyed this mine, he'll get credit for any kills
		SetOwner( info.GetAttacker() );
	}

	SetThink( &CTripmineGrenade::DelayDeathThink );
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0.1, 0.3 );

	EMIT_SOUND( this, CHAN_BODY, "common/null.wav", 0.5, ATTN_NORM ); // shut off chargeup
}


void CTripmineGrenade::DelayDeathThink()
{
	KillBeam();
	TraceResult tr;
	UTIL_TraceLine( GetAbsOrigin() + m_vecDir * 8, GetAbsOrigin() - m_vecDir * 64, dont_ignore_monsters, ENT( pev ), &tr );

	Explode( &tr, DMG_BLAST );
}
