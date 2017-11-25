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
#include "Monsters.h"
#include "Weapons.h"
#include "nodes/Nodes.h"
#include "Effects.h"

#include "CApache.h"

BEGIN_DATADESC(	CApache )
	DEFINE_FIELD( m_iRockets, FIELD_INTEGER ),
	DEFINE_FIELD( m_flForce, FIELD_FLOAT ),
	DEFINE_FIELD( m_flNextRocket, FIELD_TIME ),
	DEFINE_FIELD( m_vecTarget, FIELD_VECTOR ),
	DEFINE_FIELD( m_posTarget, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecDesired, FIELD_VECTOR ),
	DEFINE_FIELD( m_posDesired, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecGoal, FIELD_VECTOR ),
	DEFINE_FIELD( m_angGun, FIELD_VECTOR ),
	DEFINE_FIELD( m_flLastSeen, FIELD_TIME ),
	DEFINE_FIELD( m_flPrevSeen, FIELD_TIME ),
//	DEFINE_FIELD( m_iSoundState, FIELD_INTEGER ),		// Don't save, precached
//	DEFINE_FIELD( m_iSpriteTexture, FIELD_INTEGER ),
//	DEFINE_FIELD( m_iExplode, FIELD_INTEGER ),
//	DEFINE_FIELD( m_iBodyGibs, FIELD_INTEGER ),
	DEFINE_FIELD( m_pBeam, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_flGoalSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_iDoSmokePuff, FIELD_INTEGER ),
	DEFINE_THINKFUNC( HuntThink ),
	DEFINE_TOUCHFUNC( FlyTouch ),
	DEFINE_TOUCHFUNC( CrashTouch ),
	DEFINE_THINKFUNC( DyingThink ),
	DEFINE_USEFUNC( StartupUse ),
	DEFINE_THINKFUNC( NullThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_apache, CApache );

void CApache :: Spawn( void )
{
	Precache( );
	// motor
	SetMoveType( MOVETYPE_FLY );
	SetSolidType( SOLID_BBOX );

	SetModel( "models/apache.mdl");
	SetSize( Vector( -32, -32, -64 ), Vector( 32, 32, 0 ) );
	SetAbsOrigin( GetAbsOrigin() );

	GetFlags() |= FL_MONSTER;
	SetTakeDamageMode( DAMAGE_AIM );
	SetHealth( gSkillData.GetApacheHealth() );

	m_flFieldOfView = -0.707; // 270 degrees

	SetSequence( 0 );
	ResetSequenceInfo( );
	SetFrame( RANDOM_LONG( 0, 0xFF ) );

	InitBoneControllers();

	if ( GetSpawnFlags().Any( SF_WAITFORTRIGGER ) )
	{
		SetUse( &CApache::StartupUse );
	}
	else
	{
		SetThink( &CApache::HuntThink );
		SetTouch( &CApache::FlyTouch );
		SetNextThink( gpGlobals->time + 1.0 );
	}

	m_iRockets = 10;
}


void CApache::Precache( void )
{
	PRECACHE_MODEL("models/apache.mdl");

	PRECACHE_SOUND("apache/ap_rotor1.wav");
	PRECACHE_SOUND("apache/ap_rotor2.wav");
	PRECACHE_SOUND("apache/ap_rotor3.wav");
	PRECACHE_SOUND("apache/ap_whine1.wav");

	PRECACHE_SOUND("weapons/mortarhit.wav");

	m_iSpriteTexture = PRECACHE_MODEL( "sprites/white.spr" );

	PRECACHE_SOUND("turret/tu_fire1.wav");

	PRECACHE_MODEL("sprites/lgtning.spr");

	m_iExplode	= PRECACHE_MODEL( "sprites/fexplo.spr" );
	m_iBodyGibs = PRECACHE_MODEL( "models/metalplategibs_green.mdl" );

	UTIL_PrecacheOther( "hvr_rocket" );
}



void CApache::NullThink( void )
{
	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.5 );
}


void CApache::StartupUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink( &CApache::HuntThink );
	SetTouch( &CApache::FlyTouch );
	SetNextThink( gpGlobals->time + 0.1 );
	SetUse( NULL );
}

void CApache::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	SetMoveType( MOVETYPE_TOSS );
	SetGravity( 0.3 );

	STOP_SOUND( this, CHAN_STATIC, "apache/ap_rotor2.wav" );

	SetSize( Vector( -32, -32, -64), Vector( 32, 32, 0) );
	SetThink( &CApache::DyingThink );
	SetTouch( &CApache::CrashTouch );
	SetNextThink( gpGlobals->time + 0.1 );
	SetHealth( 0 );
	SetTakeDamageMode( DAMAGE_NO );

	if ( GetSpawnFlags().Any( SF_NOWRECKAGE ) )
	{
		m_flNextRocket = gpGlobals->time + 4.0;
	}
	else
	{
		m_flNextRocket = gpGlobals->time + 15.0;
	}
}

void CApache :: DyingThink( void )
{
	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.1 );

	SetAngularVelocity( GetAngularVelocity() * 1.02 );

	// still falling?
	if (m_flNextRocket > gpGlobals->time )
	{
		// random explosions
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, GetAbsOrigin() );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( GetAbsOrigin().x + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( GetAbsOrigin().y + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( GetAbsOrigin().z + RANDOM_FLOAT( -150, -50 ));
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(0,29) + 30  ); // scale * 10
			WRITE_BYTE( 12  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();

		// lots of smoke
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, GetAbsOrigin() );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( GetAbsOrigin().x + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( GetAbsOrigin().y + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( GetAbsOrigin().z + RANDOM_FLOAT( -150, -50 ));
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 100 ); // scale * 10
			WRITE_BYTE( 10  ); // framerate
		MESSAGE_END();

		Vector vecSpot = GetAbsOrigin() + ( GetRelMin() + GetRelMax() ) * 0.5;
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_BREAKMODEL);

			// position
			WRITE_COORD( vecSpot.x );
			WRITE_COORD( vecSpot.y );
			WRITE_COORD( vecSpot.z );

			// size
			WRITE_COORD( 400 );
			WRITE_COORD( 400 );
			WRITE_COORD( 132 );

			// velocity
			WRITE_COORD_VECTOR( GetAbsVelocity() );

			// randomization
			WRITE_BYTE( 50 ); 

			// Model
			WRITE_SHORT( m_iBodyGibs );	//model id#

			// # of shards
			WRITE_BYTE( 4 );	// let client decide

			// duration
			WRITE_BYTE( 30 );// 3.0 seconds

			// flags

			WRITE_BYTE( BREAK_METAL );
		MESSAGE_END();

		// don't stop it we touch a entity
		GetFlags().ClearFlags( FL_ONGROUND );
		SetNextThink( gpGlobals->time + 0.2 );
		return;
	}
	else
	{
		Vector vecSpot = GetAbsOrigin() + ( GetRelMin() + GetRelMax() ) * 0.5;

		/*
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( vecSpot.x );
			WRITE_COORD( vecSpot.y );
			WRITE_COORD( vecSpot.z + 300 );
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 250 ); // scale * 10
			WRITE_BYTE( 8  ); // framerate
		MESSAGE_END();
		*/

		// fireball
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_SPRITE );
			WRITE_COORD( vecSpot.x );
			WRITE_COORD( vecSpot.y );
			WRITE_COORD( vecSpot.z + 256 );
			WRITE_SHORT( m_iExplode );
			WRITE_BYTE( 120 ); // scale * 10
			WRITE_BYTE( 255 ); // brightness
		MESSAGE_END();
		
		// big smoke
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( vecSpot.x );
			WRITE_COORD( vecSpot.y );
			WRITE_COORD( vecSpot.z + 512 );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 250 ); // scale * 10
			WRITE_BYTE( 5  ); // framerate
		MESSAGE_END();

		// blast circle
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, GetAbsOrigin() );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( GetAbsOrigin().x);
			WRITE_COORD( GetAbsOrigin().y);
			WRITE_COORD( GetAbsOrigin().z);
			WRITE_COORD( GetAbsOrigin().x);
			WRITE_COORD( GetAbsOrigin().y);
			WRITE_COORD( GetAbsOrigin().z + 2000 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 4 ); // life
			WRITE_BYTE( 32 );  // width
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 192 );   // r, g, b
			WRITE_BYTE( 128 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();

		EMIT_SOUND( this, CHAN_STATIC, "weapons/mortarhit.wav", 1.0, 0.3);

		RadiusDamage( GetAbsOrigin(), this, this, 300, EntityClassifications().GetNoneId(), DMG_BLAST );

		if (/*!GetSpawnFlags().Any( SF_NOWRECKAGE ) && */GetFlags().Any( FL_ONGROUND ) )
		{
			CBaseEntity *pWreckage = Create( "cycler_wreckage", GetAbsOrigin(), GetAbsAngles() );
			// pWreckage->SetModel( GetModelName() );
			pWreckage->SetSize( Vector( -200, -200, -128 ), Vector( 200, 200, -32 ) );
			pWreckage->SetFrame( GetFrame() );
			pWreckage->SetSequence( GetSequence() );
			pWreckage->SetFrameRate( 0 );
			pWreckage->SetDamageTime( gpGlobals->time + 5 );
		}

		// gibs
		vecSpot = GetAbsOrigin() + ( GetRelMin() + GetRelMax() ) * 0.5;
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_BREAKMODEL);

			// position
			WRITE_COORD( vecSpot.x );
			WRITE_COORD( vecSpot.y );
			WRITE_COORD( vecSpot.z + 64);

			// size
			WRITE_COORD( 400 );
			WRITE_COORD( 400 );
			WRITE_COORD( 128 );

			// velocity
			WRITE_COORD( 0 ); 
			WRITE_COORD( 0 );
			WRITE_COORD( 200 );

			// randomization
			WRITE_BYTE( 30 ); 

			// Model
			WRITE_SHORT( m_iBodyGibs );	//model id#

			// # of shards
			WRITE_BYTE( 200 );

			// duration
			WRITE_BYTE( 200 );// 10.0 seconds

			// flags

			WRITE_BYTE( BREAK_METAL );
		MESSAGE_END();

		SetThink( &CApache::SUB_Remove );
		SetNextThink( gpGlobals->time + 0.1 );
	}
}


void CApache::FlyTouch( CBaseEntity *pOther )
{
	// bounce if we hit something solid
	if ( pOther->GetSolidType() == SOLID_BSP) 
	{
		TraceResult tr = UTIL_GetGlobalTrace( );

		// UNDONE, do a real bounce
		SetAbsVelocity( GetAbsVelocity() + tr.vecPlaneNormal * ( GetAbsVelocity().Length() + 200) );
	}
}


void CApache::CrashTouch( CBaseEntity *pOther )
{
	// only crash if we hit something solid
	if ( pOther->GetSolidType() == SOLID_BSP) 
	{
		SetTouch( NULL );
		m_flNextRocket = gpGlobals->time;
		SetNextThink( gpGlobals->time );
	}
}



void CApache :: GibMonster( void )
{
	// EMIT_SOUND_DYN( this, CHAN_VOICE, "common/bodysplat.wav", 0.75, ATTN_NORM, 0, 200);		
}


void CApache :: HuntThink( void )
{
	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.1 );

	ShowDamage( );

	if ( m_hGoalEnt == NULL && HasTarget() )// this monster has a target
	{
		m_hGoalEnt = UTIL_FindEntityByTargetname( NULL, GetTarget() );
		if ( m_hGoalEnt )
		{
			m_posDesired = m_hGoalEnt->GetAbsOrigin();
			UTIL_MakeAimVectors( m_hGoalEnt->GetAbsAngles() );
			m_vecGoal = gpGlobals->v_forward;
		}
	}

	// if (m_hEnemy == NULL)
	{
		Look( 4092 );
		m_hEnemy = BestVisibleEnemy( );
	}

	// generic speed up
	if (m_flGoalSpeed < 800)
		m_flGoalSpeed += 5;

	if (m_hEnemy != NULL)
	{
		// ALERT( at_console, "%s\n", m_hEnemy->GetClassname() );
		if (FVisible( m_hEnemy ))
		{
			if (m_flLastSeen < gpGlobals->time - 5)
				m_flPrevSeen = gpGlobals->time;
			m_flLastSeen = gpGlobals->time;
			m_posTarget = m_hEnemy->Center( );
		}
		else
		{
			m_hEnemy = NULL;
		}
	}

	m_vecTarget = (m_posTarget - GetAbsOrigin()).Normalize();

	float flLength = (GetAbsOrigin() - m_posDesired).Length();

	if ( m_hGoalEnt )
	{
		// ALERT( at_console, "%.0f\n", flLength );

		if (flLength < 128)
		{
			m_hGoalEnt = UTIL_FindEntityByTargetname( NULL, m_hGoalEnt->GetTarget() );
			if ( m_hGoalEnt )
			{
				m_posDesired = m_hGoalEnt->GetAbsOrigin();
				UTIL_MakeAimVectors( m_hGoalEnt->GetAbsAngles() );
				m_vecGoal = gpGlobals->v_forward;
				flLength = (GetAbsOrigin() - m_posDesired).Length();
			}
		}
	}
	else
	{
		m_posDesired = GetAbsOrigin();
	}

	if (flLength > 250) // 500
	{
		// float flLength2 = (m_posTarget - GetAbsOrigin()).Length() * (1.5 - DotProduct((m_posTarget - GetAbsOrigin()).Normalize(), GetAbsVelocity().Normalize() ));
		// if (flLength2 < flLength)
		if (m_flLastSeen + 90 > gpGlobals->time && DotProduct( (m_posTarget - GetAbsOrigin()).Normalize(), (m_posDesired - GetAbsOrigin()).Normalize( )) > 0.25)
		{
			m_vecDesired = (m_posTarget - GetAbsOrigin()).Normalize( );
		}
		else
		{
			m_vecDesired = (m_posDesired - GetAbsOrigin()).Normalize( );
		}
	}
	else
	{
		m_vecDesired = m_vecGoal;
	}

	Flight( );

	// ALERT( at_console, "%.0f %.0f %.0f\n", gpGlobals->time, m_flLastSeen, m_flPrevSeen );
	if ((m_flLastSeen + 1 > gpGlobals->time) && (m_flPrevSeen + 2 < gpGlobals->time))
	{
		if (FireGun( ))
		{
			// slow down if we're fireing
			if (m_flGoalSpeed > 400)
				m_flGoalSpeed = 400;
		}

		// don't fire rockets and gun on easy mode
		if (gSkillData.GetSkillLevel() == SKILL_EASY)
			m_flNextRocket = gpGlobals->time + 10.0;
	}

	UTIL_MakeAimVectors( GetAbsAngles() );
	Vector vecEst = (gpGlobals->v_forward * 800 + GetAbsVelocity() ).Normalize( );
	// ALERT( at_console, "%d %d %d %4.2f\n", GetAbsAngles().x < 0, DotProduct( GetAbsVelocity(), gpGlobals->v_forward ) > -100, m_flNextRocket < gpGlobals->time, DotProduct( m_vecTarget, vecEst ) );

	if ((m_iRockets % 2) == 1)
	{
		FireRocket( );
		m_flNextRocket = gpGlobals->time + 0.5;
		if (m_iRockets <= 0)
		{
			m_flNextRocket = gpGlobals->time + 10;
			m_iRockets = 10;
		}
	}
	else if ( GetAbsAngles().x < 0 && DotProduct( GetAbsVelocity(), gpGlobals->v_forward ) > -100 && m_flNextRocket < gpGlobals->time)
	{
		if (m_flLastSeen + 60 > gpGlobals->time)
		{
			if (m_hEnemy != NULL)
			{
				// make sure it's a good shot
				if (DotProduct( m_vecTarget, vecEst) > .965)
				{
					TraceResult tr;
					
					UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() + vecEst * 4096, ignore_monsters, edict(), &tr );
					if ((tr.vecEndPos - m_posTarget).Length() < 512)
						FireRocket( );
				}
			}
			else
			{
				TraceResult tr;
				
				UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() + vecEst * 4096, dont_ignore_monsters, edict(), &tr );
				// just fire when close
				if ((tr.vecEndPos - m_posTarget).Length() < 512)
					FireRocket( );
			}
		}
	}
}


void CApache :: Flight( void )
{
	// tilt model 5 degrees
	Vector vecAdj = Vector( 5.0, 0, 0 );

	// estimate where I'll be facing in one seconds
	UTIL_MakeAimVectors( GetAbsAngles() + GetAngularVelocity() * 2 + vecAdj);
	// Vector vecEst1 = GetAbsOrigin() + GetAbsVelocity() + gpGlobals->v_up * m_flForce - Vector( 0, 0, 384 );
	// float flSide = DotProduct( m_posDesired - vecEst1, gpGlobals->v_right );
	
	float flSide = DotProduct( m_vecDesired, gpGlobals->v_right );

	Vector vecAVelocity = GetAngularVelocity();

	if (flSide < 0)
	{
		if( vecAVelocity.y < 60 )
		{
			vecAVelocity.y += 8; // 9 * (3.0/2.0);
		}
	}
	else
	{
		if( vecAVelocity.y > -60 )
		{
			vecAVelocity.y -= 8; // 9 * (3.0/2.0);
		}
	}
	vecAVelocity.y *= 0.98;

	// estimate where I'll be in two seconds
	UTIL_MakeAimVectors( GetAbsAngles() + vecAVelocity * 1 + vecAdj);
	Vector vecEst = GetAbsOrigin() + GetAbsVelocity() * 2.0 + gpGlobals->v_up * m_flForce * 20 - Vector( 0, 0, 384 * 2 );

	// add immediate force
	UTIL_MakeAimVectors( GetAbsAngles() + vecAdj);
	{
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.x += gpGlobals->v_up.x * m_flForce;
		vecVelocity.y += gpGlobals->v_up.y * m_flForce;
		vecVelocity.z += gpGlobals->v_up.z * m_flForce;
		// add gravity
		vecVelocity.z -= 38.4; // 32ft/sec

		SetAbsVelocity( vecVelocity );
	}

	float flSpeed = GetAbsVelocity().Length();
	float flDir = DotProduct( Vector( gpGlobals->v_forward.x, gpGlobals->v_forward.y, 0 ), Vector( GetAbsVelocity().x, GetAbsVelocity().y, 0 ) );
	if (flDir < 0)
		flSpeed = -flSpeed;

	float flDist = DotProduct( m_posDesired - vecEst, gpGlobals->v_forward );

	// float flSlip = DotProduct( GetAbsVelocity(), gpGlobals->v_right );
	float flSlip = -DotProduct( m_posDesired - vecEst, gpGlobals->v_right );

	// fly sideways
	if (flSlip > 0)
	{
		if ( GetAbsAngles().z > -30 && vecAVelocity.z > -15)
			vecAVelocity.z -= 4;
		else
			vecAVelocity.z += 2;
	}
	else
	{

		if ( GetAbsAngles().z < 30 && vecAVelocity.z < 15)
			vecAVelocity.z += 4;
		else
			vecAVelocity.z -= 2;
	}

	// sideways drag
	{
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.x = vecVelocity.x * (1.0 - fabs( gpGlobals->v_right.x ) * 0.05);
		vecVelocity.y = vecVelocity.y * (1.0 - fabs( gpGlobals->v_right.y ) * 0.05);
		vecVelocity.z = vecVelocity.z * (1.0 - fabs( gpGlobals->v_right.z ) * 0.05);

		// general drag
		vecVelocity = vecVelocity * 0.995;

		SetAbsVelocity( vecVelocity );
	}
	
	// apply power to stay correct height
	if (m_flForce < 80 && vecEst.z < m_posDesired.z) 
	{
		m_flForce += 12;
	}
	else if (m_flForce > 30)
	{
		if (vecEst.z > m_posDesired.z) 
			m_flForce -= 8;
	}

	// pitch forward or back to get to target
	if (flDist > 0 && flSpeed < m_flGoalSpeed /* && flSpeed < flDist */ && GetAbsAngles().x + vecAVelocity.x > -40)
	{
		// ALERT( at_console, "F " );
		// lean forward
		vecAVelocity.x -= 12.0;
	}
	else if (flDist < 0 && flSpeed > -50 && GetAbsAngles().x + vecAVelocity.x  < 20)
	{
		// ALERT( at_console, "B " );
		// lean backward
		vecAVelocity.x += 12.0;
	}
	else if ( GetAbsAngles().x + vecAVelocity.x > 0)
	{
		// ALERT( at_console, "f " );
		vecAVelocity.x -= 4.0;
	}
	else if ( GetAbsAngles().x + vecAVelocity.x < 0)
	{
		// ALERT( at_console, "b " );
		vecAVelocity.x += 4.0;
	}

	SetAngularVelocity( vecAVelocity );

	// ALERT( at_console, "%.0f %.0f : %.0f %.0f : %.0f %.0f : %.0f\n", GetAbsOrigin().x, GetAbsVelocity().x, flDist, flSpeed, GetAbsAngles().x, GetAngularVelocity().x, m_flForce ); 
	// ALERT( at_console, "%.0f %.0f : %.0f %0.f : %.0f\n", GetAbsOrigin().z, GetAbsVelocity().z, vecEst.z, m_posDesired.z, m_flForce ); 

	// make rotor, engine sounds
	if (m_iSoundState == 0)
	{
		EMIT_SOUND_DYN( this, CHAN_STATIC, "apache/ap_rotor2.wav", 1.0, 0.3, 0, 110 );
		// EMIT_SOUND_DYN( this, CHAN_STATIC, "apache/ap_whine1.wav", 0.5, 0.2, 0, 110 );

		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions
	}
	else
	{
		CBaseEntity *pPlayer = NULL;

		pPlayer = UTIL_FindEntityByClassname( NULL, "player" );
		// UNDONE: this needs to send different sounds to every player for multiplayer.	
		if (pPlayer)
		{

			float pitch = DotProduct( GetAbsVelocity() - pPlayer->GetAbsVelocity(), (pPlayer->GetAbsOrigin() - GetAbsOrigin()).Normalize() );

			pitch = (int)(100 + pitch / 50.0);

			if (pitch > 250) 
				pitch = 250;
			if (pitch < 50)
				pitch = 50;
			if (pitch == 100)
				pitch = 101;

			float flVol = (m_flForce / 100.0) + .1;
			if (flVol > 1.0) 
				flVol = 1.0;

			EMIT_SOUND_DYN( this, CHAN_STATIC, "apache/ap_rotor2.wav", 1.0, 0.3, SND_CHANGE_PITCH | SND_CHANGE_VOL, pitch);
		}
		// EMIT_SOUND_DYN( this, CHAN_STATIC, "apache/ap_whine1.wav", flVol, 0.2, SND_CHANGE_PITCH | SND_CHANGE_VOL, pitch);
	
		// ALERT( at_console, "%.0f %.2f\n", pitch, flVol );
	}
}


void CApache :: FireRocket( void )
{
	static float side = 1.0;
	//static int count;

	if (m_iRockets <= 0)
		return;

	UTIL_MakeAimVectors( GetAbsAngles() );
	Vector vecSrc = GetAbsOrigin() + 1.5 * (gpGlobals->v_forward * 21 + gpGlobals->v_right * 70 * side + gpGlobals->v_up * -79);

	switch( m_iRockets % 5)
	{
	case 0:	vecSrc = vecSrc + gpGlobals->v_right * 10; break;
	case 1: vecSrc = vecSrc - gpGlobals->v_right * 10; break;
	case 2: vecSrc = vecSrc + gpGlobals->v_up * 10; break;
	case 3: vecSrc = vecSrc - gpGlobals->v_up * 10; break;
	case 4: break;
	}

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE( TE_SMOKE );
		WRITE_COORD( vecSrc.x );
		WRITE_COORD( vecSrc.y );
		WRITE_COORD( vecSrc.z );
		WRITE_SHORT( g_sModelIndexSmoke );
		WRITE_BYTE( 20 ); // scale * 10
		WRITE_BYTE( 12 ); // framerate
	MESSAGE_END();

	CBaseEntity *pRocket = CBaseEntity::Create( "hvr_rocket", vecSrc, GetAbsAngles(), edict() );
	if (pRocket)
		pRocket->SetAbsVelocity( GetAbsVelocity() + gpGlobals->v_forward * 100 );

	m_iRockets--;

	side = - side;
}



bool CApache::FireGun()
{
	UTIL_MakeAimVectors( GetAbsAngles() );
		
	Vector posGun, angGun;
	GetAttachment( 1, posGun, angGun );

	Vector vecTarget = (m_posTarget - posGun).Normalize( );

	Vector vecOut;

	vecOut.x = DotProduct( gpGlobals->v_forward, vecTarget );
	vecOut.y = -DotProduct( gpGlobals->v_right, vecTarget );
	vecOut.z = DotProduct( gpGlobals->v_up, vecTarget );

	Vector angles = UTIL_VecToAngles (vecOut);

	angles.x = -angles.x;
	if (angles.y > 180)
		angles.y = angles.y - 360;
	if (angles.y < -180)
		angles.y = angles.y + 360;
	if (angles.x > 180)
		angles.x = angles.x - 360;
	if (angles.x < -180)
		angles.x = angles.x + 360;

	if (angles.x > m_angGun.x)
		m_angGun.x = min( angles.x, m_angGun.x + 12 );
	if (angles.x < m_angGun.x)
		m_angGun.x = max( angles.x, m_angGun.x - 12 );
	if (angles.y > m_angGun.y)
		m_angGun.y = min( angles.y, m_angGun.y + 12 );
	if (angles.y < m_angGun.y)
		m_angGun.y = max( angles.y, m_angGun.y - 12 );

	m_angGun.y = SetBoneController( 0, m_angGun.y );
	m_angGun.x = SetBoneController( 1, m_angGun.x );

	Vector posBarrel, angBarrel;
	GetAttachment( 0, posBarrel, angBarrel );
	Vector vecGun = (posBarrel - posGun).Normalize( );

	if (DotProduct( vecGun, vecTarget ) > 0.98)
	{
#if 1
		FireBullets( 1, posGun, vecGun, VECTOR_CONE_4DEGREES, 8192, BULLET_MONSTER_12MM, 1 );
		EMIT_SOUND( this, CHAN_WEAPON, "turret/tu_fire1.wav", 1, 0.3);
#else
		static float flNext;
		TraceResult tr;
		UTIL_TraceLine( posGun, posGun + vecGun * 8192, dont_ignore_monsters, ENT( pev ), &tr );

		if (!m_pBeam)
		{
			m_pBeam = CBeam::BeamCreate( "sprites/lgtning.spr", 80 );
			m_pBeam->PointEntInit( GetAbsOrigin(), entindex( ) );
			m_pBeam->SetEndAttachment( 1 );
			m_pBeam->SetColor( 255, 180, 96 );
			m_pBeam->SetBrightness( 192 );
		}

		if (flNext < gpGlobals->time)
		{
			flNext = gpGlobals->time + 0.5;
			m_pBeam->SetStartPos( tr.vecEndPos );
		}
#endif
		return true;
	}
	else
	{
		if (m_pBeam)
		{
			UTIL_Remove( m_pBeam );
			m_pBeam = NULL;
		}
	}
	return false;
}



void CApache :: ShowDamage( void )
{
	if (m_iDoSmokePuff > 0 || RANDOM_LONG(0,99) > GetHealth() )
	{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, GetAbsOrigin() );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( GetAbsOrigin().x );
			WRITE_COORD( GetAbsOrigin().y );
			WRITE_COORD( GetAbsOrigin().z - 32 );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( RANDOM_LONG(0,9) + 20 ); // scale * 10
			WRITE_BYTE( 12 ); // framerate
		MESSAGE_END();
	}
	if (m_iDoSmokePuff > 0)
		m_iDoSmokePuff--;
}


void CApache::OnTakeDamage( const CTakeDamageInfo& info )
{
	CTakeDamageInfo newInfo = info;

	if ( newInfo.GetInflictor()->GetOwner() == this )
		return;

	if ( newInfo.GetDamageTypes() & DMG_BLAST)
	{
		newInfo.GetMutableDamage() *= 2;
	}

	/*
	if ( (bitsDamageType & DMG_BULLET) && flDamage > 50)
	{
		// clip bullet damage at 50
		flDamage = 50;
	}
	*/

	// ALERT( at_console, "%.0f\n", flDamage );
	CBaseEntity::OnTakeDamage( newInfo );
}



void CApache::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	// ALERT( at_console, "%d %.0f\n", ptr->iHitgroup, flDamage );

	// ignore blades
	if ( tr.iHitgroup == 6 && (info.GetDamageTypes() & (DMG_ENERGYBEAM|DMG_BULLET|DMG_CLUB)))
		return;

	// hit hard, hits cockpit, hits engines
	if (info.GetDamage() > 50 || tr.iHitgroup == 1 || tr.iHitgroup == 2)
	{
		// ALERT( at_console, "%.0f\n", flDamage );
		g_MultiDamage.AddMultiDamage( info, this );
		m_iDoSmokePuff = 3 + (info.GetDamage() / 5.0);
	}
	else
	{
		// do half damage in the body
		// g_MultiDamage.AddMultiDamage( info.GetAttacker(), this, info.GetDamage() / 2.0, info.GetDamageTypes() );
		UTIL_Ricochet( tr.vecEndPos, 2.0 );
	}
}