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

#include "CFuncRotating.h"

BEGIN_DATADESC( CFuncRotating )
	DEFINE_FIELD( m_flFanFriction, FIELD_FLOAT ),
	DEFINE_FIELD( m_flAttenuation, FIELD_FLOAT ),
	DEFINE_FIELD( m_flVolume, FIELD_FLOAT ),
	DEFINE_FIELD( m_pitch, FIELD_FLOAT ),
	DEFINE_FIELD( m_sounds, FIELD_INTEGER ),
	DEFINE_THINKFUNC( SpinUp ),
	DEFINE_THINKFUNC( SpinDown ),
	DEFINE_TOUCHFUNC( HurtTouch ),
	DEFINE_USEFUNC( RotatingUse ),
	DEFINE_THINKFUNC( Rotate ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_rotating, CFuncRotating );

void CFuncRotating::Spawn()
{
	// set final pitch.  Must not be PITCH_NORM, since we
	// plan on pitch shifting later.

	m_pitch = PITCH_NORM - 1;

	// maintain compatibility with previous maps
	if( m_flVolume == 0.0 )
		m_flVolume = 1.0;

	// if the designer didn't set a sound attenuation, default to one.
	m_flAttenuation = ATTN_NORM;

	if( GetSpawnFlags().Any( SF_BRUSH_ROTATE_SMALLRADIUS ) )
	{
		m_flAttenuation = ATTN_IDLE;
	}
	else if( GetSpawnFlags().Any( SF_BRUSH_ROTATE_MEDIUMRADIUS ) )
	{
		m_flAttenuation = ATTN_STATIC;
	}
	else if( GetSpawnFlags().Any( SF_BRUSH_ROTATE_LARGERADIUS ) )
	{
		m_flAttenuation = ATTN_NORM;
	}

	// prevent divide by zero if level designer forgets friction!
	if( m_flFanFriction == 0 )
	{
		m_flFanFriction = 1;
	}

	if( GetSpawnFlags().Any( SF_BRUSH_ROTATE_Z_AXIS ) )
		SetMoveDir( Vector( 0, 0, 1 ) );
	else if( GetSpawnFlags().Any( SF_BRUSH_ROTATE_X_AXIS ) )
		SetMoveDir( Vector( 1, 0, 0 ) );
	else
		SetMoveDir( Vector( 0, 1, 0 ) );	// y-axis

											// check for reverse rotation
	if( GetSpawnFlags().Any( SF_BRUSH_ROTATE_BACKWARDS ) )
		SetMoveDir( GetMoveDir() * -1 );

	// some rotating objects like fake volumetric lights will not be solid.
	if( GetSpawnFlags().Any( SF_ROTATING_NOT_SOLID ) )
	{
		SetSolidType( SOLID_NOT );
		SetSkin( CONTENTS_EMPTY );
		SetMoveType( MOVETYPE_PUSH );
	}
	else
	{
		SetSolidType( SOLID_BSP );
		SetMoveType( MOVETYPE_PUSH );
	}

	SetAbsOrigin( GetAbsOrigin() );
	SetModel( GetModelName() );

	SetUse( &CFuncRotating::RotatingUse );
	// did level designer forget to assign speed?
	if( GetSpeed() <= 0 )
		SetSpeed( 0 );

	// Removed this per level designers request.  -- JAY
	//	if (GetDamage() == 0)
	//		SetDamage( 2 );

	// instant-use brush?
	if( GetSpawnFlags().Any( SF_BRUSH_ROTATE_INSTANT ) )
	{
		SetThink( &CFuncRotating::SUB_CallUseToggle );
		SetNextThink( GetLastThink() + 1.5 );	// leave a magic delay for client to start up
	}
	// can this brush inflict pain?
	if( GetSpawnFlags().Any( SF_BRUSH_HURT ) )
	{
		SetTouch( &CFuncRotating::HurtTouch );
	}

	Precache();
}

void CFuncRotating::Precache( void )
{
	const char* szSoundFile = GetMessage();

	// set up fan sounds

	if( HasMessage() && strlen( szSoundFile ) > 0 )
	{
		// if a path is set for a wave, use it

		PRECACHE_SOUND( szSoundFile );

		pev->noiseRunning = ALLOC_STRING( szSoundFile );
	}
	else
	{
		// otherwise use preset sound
		switch( m_sounds )
		{
		case 1:
			PRECACHE_SOUND( "fans/fan1.wav" );
			pev->noiseRunning = ALLOC_STRING( "fans/fan1.wav" );
			break;
		case 2:
			PRECACHE_SOUND( "fans/fan2.wav" );
			pev->noiseRunning = ALLOC_STRING( "fans/fan2.wav" );
			break;
		case 3:
			PRECACHE_SOUND( "fans/fan3.wav" );
			pev->noiseRunning = ALLOC_STRING( "fans/fan3.wav" );
			break;
		case 4:
			PRECACHE_SOUND( "fans/fan4.wav" );
			pev->noiseRunning = ALLOC_STRING( "fans/fan4.wav" );
			break;
		case 5:
			PRECACHE_SOUND( "fans/fan5.wav" );
			pev->noiseRunning = ALLOC_STRING( "fans/fan5.wav" );
			break;

		case 0:
		default:
			if( HasMessage() && strlen( szSoundFile ) > 0 )
			{
				PRECACHE_SOUND( szSoundFile );

				pev->noiseRunning = ALLOC_STRING( szSoundFile );
				break;
			}
			else
			{
				pev->noiseRunning = ALLOC_STRING( "common/null.wav" );
				break;
			}
		}
	}

	if( GetAngularVelocity() != g_vecZero )
	{
		// if fan was spinning, and we went through transition or save/restore,
		// make sure we restart the sound.  1.5 sec delay is magic number. KDB

		SetThink( &CFuncRotating::SpinUp );
		SetNextThink( GetLastThink() + 1.5 );
	}
}

//
// SpinUp - accelerates a non-moving func_rotating up to it's speed
//
void CFuncRotating::SpinUp( void )
{
	Vector	vecAVel;//rotational velocity

	SetNextThink( GetLastThink() + 0.1 );
	SetAngularVelocity( GetAngularVelocity() + ( GetMoveDir() * ( GetSpeed() * m_flFanFriction ) ) );

	vecAVel = GetAngularVelocity();// cache entity's rotational velocity

							 // if we've met or exceeded target speed, set target speed and stop thinking
	if( fabs( vecAVel.x ) >= fabs( GetMoveDir().x * GetSpeed() ) &&
		fabs( vecAVel.y ) >= fabs( GetMoveDir().y * GetSpeed() ) &&
		fabs( vecAVel.z ) >= fabs( GetMoveDir().z * GetSpeed() ) )
	{
		SetAngularVelocity( GetMoveDir() * GetSpeed() );// set speed in case we overshot
		EMIT_SOUND_DYN( this, CHAN_STATIC, ( char * ) STRING( pev->noiseRunning ),
						m_flVolume, m_flAttenuation, SND_CHANGE_PITCH | SND_CHANGE_VOL, FANPITCHMAX );

		SetThink( &CFuncRotating::Rotate );
		Rotate();
	}
	else
	{
		RampPitchVol( true );
	}
}

//
// SpinDown - decelerates a moving func_rotating to a standstill.
//
void CFuncRotating::SpinDown( void )
{
	Vector	vecAVel;//rotational velocity
	vec_t vecdir;

	SetNextThink( GetLastThink() + 0.1 );

	SetAngularVelocity( GetAngularVelocity() - ( GetMoveDir() * ( GetSpeed() * m_flFanFriction ) ) );//spin down slower than spinup

	vecAVel = GetAngularVelocity();// cache entity's rotational velocity

	if( GetMoveDir().x != 0 )
		vecdir = GetMoveDir().x;
	else if( GetMoveDir().y != 0 )
		vecdir = GetMoveDir().y;
	else
		vecdir = GetMoveDir().z;

	// if we've met or exceeded target speed, set target speed and stop thinking
	// (note: must check for movedir > 0 or < 0)
	if( ( ( vecdir > 0 ) && ( vecAVel.x <= 0 && vecAVel.y <= 0 && vecAVel.z <= 0 ) ) ||
		( ( vecdir < 0 ) && ( vecAVel.x >= 0 && vecAVel.y >= 0 && vecAVel.z >= 0 ) ) )
	{
		SetAngularVelocity( g_vecZero );// set speed in case we overshot

								   // stop sound, we're done
		EMIT_SOUND_DYN( this, CHAN_STATIC, ( char * ) STRING( pev->noiseRunning /* Stop */ ),
						0, 0, SND_STOP, m_pitch );

		SetThink( &CFuncRotating::Rotate );
		Rotate();
	}
	else
	{
		RampPitchVol( false );
	}
}

void CFuncRotating::KeyValue( KeyValueData* pkvd )
{
	if( FStrEq( pkvd->szKeyName, "fanfriction" ) )
	{
		m_flFanFriction = atof( pkvd->szValue ) / 100;
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "Volume" ) )
	{
		m_flVolume = atof( pkvd->szValue ) / 10.0;

		if( m_flVolume > 1.0 )
			m_flVolume = 1.0;
		if( m_flVolume < 0.0 )
			m_flVolume = 0.0;
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "spawnorigin" ) )
	{
		Vector tmp;
		UTIL_StringToVector( tmp, pkvd->szValue );
		if( tmp != g_vecZero )
			SetAbsOrigin( tmp );
	}
	else if( FStrEq( pkvd->szKeyName, "sounds" ) )
	{
		m_sounds = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

//
// Touch - will hurt others based on how fast the brush is spinning
//
void CFuncRotating::HurtTouch( CBaseEntity *pOther )
{
	// we can't hurt this thing, so we're not concerned with it
	if( pOther->GetTakeDamageMode() == DAMAGE_NO )
		return;

	// calculate damage based on rotation speed
	SetDamage( GetAngularVelocity().Length() / 10 );

	pOther->TakeDamage( this, this, GetDamage(), DMG_CRUSH );

	pOther->SetAbsVelocity( ( pOther->GetAbsOrigin() - VecBModelOrigin( this ) ).Normalize() * GetDamage() );
}

//=========================================================
// Rotating Use - when a rotating brush is triggered
//=========================================================
void CFuncRotating::RotatingUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// is this a brush that should accelerate and decelerate when turned on/off (fan)?
	if( GetSpawnFlags().Any( SF_BRUSH_ACCDCC ) )
	{
		// fan is spinning, so stop it.
		if( GetAngularVelocity() != g_vecZero )
		{
			SetThink( &CFuncRotating::SpinDown );
			//EMIT_SOUND_DYN( this, CHAN_WEAPON, (char *)STRING(pev->noiseStop), 
			//	m_flVolume, m_flAttenuation, 0, m_pitch);

			SetNextThink( GetLastThink() + 0.1 );
		}
		else// fan is not moving, so start it
		{
			SetThink( &CFuncRotating::SpinUp );
			EMIT_SOUND_DYN( this, CHAN_STATIC, ( char * ) STRING( pev->noiseRunning ),
							0.01, m_flAttenuation, 0, FANPITCHMIN );

			SetNextThink( GetLastThink() + 0.1 );
		}
	}
	else if( !GetSpawnFlags().Any( SF_BRUSH_ACCDCC ) )//this is a normal start/stop brush.
	{
		if( GetAngularVelocity() != g_vecZero )
		{
			// play stopping sound here
			SetThink( &CFuncRotating::SpinDown );

			// EMIT_SOUND_DYN( this, CHAN_WEAPON, (char *)STRING(pev->noiseStop), 
			//	m_flVolume, m_flAttenuation, 0, m_pitch);

			SetNextThink( GetLastThink() + 0.1 );
			// SetAngularVelocity( g_vecZero );
		}
		else
		{
			EMIT_SOUND_DYN( this, CHAN_STATIC, ( char * ) STRING( pev->noiseRunning ),
							m_flVolume, m_flAttenuation, 0, FANPITCHMAX );
			SetAngularVelocity( GetMoveDir() * GetSpeed() );

			SetThink( &CFuncRotating::Rotate );
			Rotate();
		}
	}
}

void CFuncRotating::Rotate( void )
{
	SetNextThink( GetLastThink() + 10 );
}

//
// RampPitchVol - ramp pitch and volume up to final values, based on difference
// between how fast we're going vs how fast we plan to go
//
void CFuncRotating::RampPitchVol( const bool bUp )
{
	Vector vecAVel = GetAngularVelocity();
	vec_t vecCur;
	vec_t vecFinal;
	float fpct;
	float fvol;
	float fpitch;
	int pitch;

	// get current angular velocity

	vecCur = fabs( vecAVel.x != 0 ? vecAVel.x : ( vecAVel.y != 0 ? vecAVel.y : vecAVel.z ) );

	// get target angular velocity

	vecFinal = ( GetMoveDir().x != 0 ? GetMoveDir().x : ( GetMoveDir().y != 0 ? GetMoveDir().y : GetMoveDir().z ) );
	vecFinal *= GetSpeed();
	vecFinal = fabs( vecFinal );

	// calc volume and pitch as % of final vol and pitch

	fpct = vecCur / vecFinal;
	//	if (bUp)
	//		fvol = m_flVolume * (0.5 + fpct/2.0); // spinup volume ramps up from 50% max vol
	//	else
	fvol = m_flVolume * fpct;			  // slowdown volume ramps down to 0

	fpitch = FANPITCHMIN + ( FANPITCHMAX - FANPITCHMIN ) * fpct;

	pitch = ( int ) fpitch;
	if( pitch == PITCH_NORM )
		pitch = PITCH_NORM - 1;

	// change the fan's vol and pitch

	EMIT_SOUND_DYN( this, CHAN_STATIC, ( char * ) STRING( pev->noiseRunning ),
					fvol, m_flAttenuation, SND_CHANGE_PITCH | SND_CHANGE_VOL, pitch );

}

//
// RotatingBlocked - An entity has blocked the brush
//
void CFuncRotating::Blocked( CBaseEntity *pOther )
{
	pOther->TakeDamage( this, this, GetDamage(), DMG_CRUSH );
}