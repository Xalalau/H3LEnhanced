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
/*

	h_tentacle.cpp - silo of death tentacle monster (half life)

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"
#include "entities/CSoundEnt.h"

#include "CTentacle.h"

//TODO: initialize - Solokiller
bool CTentacle::g_fFlySound;
bool CTentacle::g_fSquirmSound;

const char *CTentacle::pHitSilo[] =
{
	"tentacle/te_strike1.wav",
	"tentacle/te_strike2.wav",
};

const char *CTentacle::pHitDirt[] =
{
	"player/pl_dirt1.wav",
	"player/pl_dirt2.wav",
	"player/pl_dirt3.wav",
	"player/pl_dirt4.wav",
};

const char *CTentacle::pHitWater[] =
{
	"player/pl_slosh1.wav",
	"player/pl_slosh2.wav",
	"player/pl_slosh3.wav",
	"player/pl_slosh4.wav",
};

// stike sounds
#define TE_NONE -1
#define TE_SILO 0
#define TE_DIRT 1
#define TE_WATER 2

BEGIN_DATADESC(	CTentacle )
	DEFINE_FIELD( m_flInitialYaw, FIELD_FLOAT ),
	DEFINE_FIELD( m_iGoalAnim, FIELD_INTEGER ),
	DEFINE_FIELD( m_iLevel, FIELD_INTEGER ),
	DEFINE_FIELD( m_iDir, FIELD_INTEGER ),
	DEFINE_FIELD( m_flFramerateAdj, FIELD_FLOAT ),
	DEFINE_FIELD( m_flSoundYaw, FIELD_FLOAT ),
	DEFINE_FIELD( m_iSoundLevel, FIELD_INTEGER ),
	DEFINE_FIELD( m_flSoundTime, FIELD_TIME ),
	DEFINE_FIELD( m_flSoundRadius, FIELD_FLOAT ),
	DEFINE_FIELD( m_iHitDmg, FIELD_INTEGER ),
	DEFINE_FIELD( m_flHitTime, FIELD_TIME ),
	DEFINE_FIELD( m_flTapRadius, FIELD_FLOAT ),
	DEFINE_FIELD( m_flNextSong, FIELD_TIME ),
	DEFINE_FIELD( m_iTapSound, FIELD_INTEGER ),
	DEFINE_FIELD( m_flMaxYaw, FIELD_FLOAT ),
	DEFINE_FIELD( m_vecPrevSound, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_flPrevSoundTime, FIELD_TIME ),

	DEFINE_THINKFUNC( Cycle ),
	DEFINE_USEFUNC( CommandUse ),
	DEFINE_THINKFUNC( Start ),
	DEFINE_THINKFUNC( DieThink ),
	DEFINE_THINKFUNC( Test ),
	DEFINE_TOUCHFUNC( HitTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_tentacle, CTentacle );

EntityClassification_t CTentacle::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::ALIEN_MONSTER );
}

//
// Tentacle Spawn
//
void CTentacle :: Spawn( )
{
	Precache( );

	SetSolidType( SOLID_BBOX );
	SetMoveType( MOVETYPE_FLY );
	GetEffects().ClearAll();
	SetHealth( 75 );
	SetSequence( 0 );

	SetModel( "models/tentacle2.mdl");
	SetSize( Vector( -32, -32, 0 ), Vector( 32, 32, 64 ) );

	SetTakeDamageMode( DAMAGE_AIM );
	GetFlags() |= FL_MONSTER;
	
	m_bloodColor		= BLOOD_COLOR_GREEN;

	SetThink( &CTentacle::Start );
	SetTouch( &CTentacle::HitTouch );
	SetUse( &CTentacle::CommandUse );

	SetNextThink( gpGlobals->time + 0.2 );

	ResetSequenceInfo( );
	m_iDir = 1;

	SetYawSpeed( 18 );
	m_flInitialYaw = GetAbsAngles().y;
	SetIdealYaw( m_flInitialYaw );

	g_fFlySound = false;
	g_fSquirmSound = false;

	m_iHitDmg = 20;

	if (m_flMaxYaw <= 0)
		m_flMaxYaw = 65;

	m_MonsterState = MONSTERSTATE_IDLE;

	// SetThink( Test );
	SetAbsOrigin( GetAbsOrigin() );
}

void CTentacle :: Precache( )
{
	PRECACHE_MODEL("models/tentacle2.mdl");

	PRECACHE_SOUND("ambience/flies.wav");
	PRECACHE_SOUND("ambience/squirm2.wav");

	PRECACHE_SOUND("tentacle/te_alert1.wav");
	PRECACHE_SOUND("tentacle/te_alert2.wav");
	PRECACHE_SOUND("tentacle/te_flies1.wav");
	PRECACHE_SOUND("tentacle/te_move1.wav");
	PRECACHE_SOUND("tentacle/te_move2.wav");
	PRECACHE_SOUND("tentacle/te_roar1.wav");
	PRECACHE_SOUND("tentacle/te_roar2.wav");
	PRECACHE_SOUND("tentacle/te_search1.wav");
	PRECACHE_SOUND("tentacle/te_search2.wav");
	PRECACHE_SOUND("tentacle/te_sing1.wav");
	PRECACHE_SOUND("tentacle/te_sing2.wav");
	PRECACHE_SOUND("tentacle/te_squirm2.wav");
	PRECACHE_SOUND("tentacle/te_strike1.wav");
	PRECACHE_SOUND("tentacle/te_strike2.wav");
	PRECACHE_SOUND("tentacle/te_swing1.wav");
	PRECACHE_SOUND("tentacle/te_swing2.wav");

	PRECACHE_SOUND_ARRAY( pHitSilo );
	PRECACHE_SOUND_ARRAY( pHitDirt );
	PRECACHE_SOUND_ARRAY( pHitWater );
}

CTentacle::CTentacle( )
{
	m_flMaxYaw = 65;
	m_iTapSound = 0;
}

void CTentacle::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "sweeparc"))
	{
		m_flMaxYaw = atof(pkvd->szValue) / 2.0;
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "sound"))
	{
		m_iTapSound = atoi(pkvd->szValue);
		pkvd->fHandled = true;

	}
	else
		CBaseMonster::KeyValue( pkvd );
}

int CTentacle :: Level( float dz )
{
	if (dz < 216)
		return 0;
	if (dz < 408)
		return 1;
	if (dz < 600)
		return 2;
	return 3;
}

float CTentacle :: MyHeight( )
{
	switch ( MyLevel( ) )
	{
	case 1:
		return 256;
	case 2:
		return 448;
	case 3:
		return 640;
	}
	return 0;
}

int CTentacle :: MyLevel( )
{
	switch( GetSequence() )
	{
	case TENTACLE_ANIM_Pit_Idle: 
		return -1;

	case TENTACLE_ANIM_rise_to_Temp1:
	case TENTACLE_ANIM_Temp1_to_Floor:
	case TENTACLE_ANIM_Floor_to_Lev1:
		return 0;

	case TENTACLE_ANIM_Floor_Idle:
	case TENTACLE_ANIM_Floor_Fidget_Pissed:
	case TENTACLE_ANIM_Floor_Fidget_SmallRise:
	case TENTACLE_ANIM_Floor_Fidget_Wave:
	case TENTACLE_ANIM_Floor_Strike:
	case TENTACLE_ANIM_Floor_Tap:
	case TENTACLE_ANIM_Floor_Rotate:
	case TENTACLE_ANIM_Floor_Rear:
	case TENTACLE_ANIM_Floor_Rear_Idle:
		return 0;

	case TENTACLE_ANIM_Lev1_Idle:
	case TENTACLE_ANIM_Lev1_Fidget_Claw:
	case TENTACLE_ANIM_Lev1_Fidget_Shake:
	case TENTACLE_ANIM_Lev1_Fidget_Snap:
	case TENTACLE_ANIM_Lev1_Strike:
	case TENTACLE_ANIM_Lev1_Tap:
	case TENTACLE_ANIM_Lev1_Rotate:
	case TENTACLE_ANIM_Lev1_Rear:
	case TENTACLE_ANIM_Lev1_Rear_Idle:
		return 1;

	case TENTACLE_ANIM_Lev1_to_Lev2:
		return 1;

	case TENTACLE_ANIM_Lev2_Idle:
	case TENTACLE_ANIM_Lev2_Fidget_Shake:
	case TENTACLE_ANIM_Lev2_Fidget_Swing:
	case TENTACLE_ANIM_Lev2_Fidget_Tut:
	case TENTACLE_ANIM_Lev2_Strike:
	case TENTACLE_ANIM_Lev2_Tap:
	case TENTACLE_ANIM_Lev2_Rotate:
	case TENTACLE_ANIM_Lev2_Rear:
	case TENTACLE_ANIM_Lev2_Rear_Idle:
		return 2;

	case TENTACLE_ANIM_Lev2_to_Lev3:
		return 2;

	case TENTACLE_ANIM_Lev3_Idle:
	case TENTACLE_ANIM_Lev3_Fidget_Shake:
	case TENTACLE_ANIM_Lev3_Fidget_Side:
	case TENTACLE_ANIM_Lev3_Fidget_Swipe:
	case TENTACLE_ANIM_Lev3_Strike:
	case TENTACLE_ANIM_Lev3_Tap:
	case TENTACLE_ANIM_Lev3_Rotate:
	case TENTACLE_ANIM_Lev3_Rear:
	case TENTACLE_ANIM_Lev3_Rear_Idle:
		return 3;

	case TENTACLE_ANIM_Lev1_Door_reach:
		return -1;
	}
	return -1;
}

void CTentacle :: Test( void )
{
	SetSequence( TENTACLE_ANIM_Floor_Strike );
	SetFrameRate( 0 );
	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.1 );
}

//
// TentacleThink
//
void CTentacle :: Cycle( void )
{
	// ALERT( at_console, "%s %.2f %d %d\n", GetTargetname(), GetAbsOrigin().z, m_MonsterState, m_IdealMonsterState );
	SetNextThink( gpGlobals-> time + 0.1 );

	// ALERT( at_console, "%s %d %d %d %f %f\n", GetTargetname(), GetSequence(), m_iGoalAnim, m_iDir, GetFrameRate(), GetHealth() );

	if (m_MonsterState == MONSTERSTATE_SCRIPT || m_IdealMonsterState == MONSTERSTATE_SCRIPT)
	{
		Vector vecAngles = GetAbsAngles();
		vecAngles.y = m_flInitialYaw;
		SetAbsAngles( vecAngles );
		SetIdealYaw( m_flInitialYaw );
		ClearConditions( IgnoreConditions() );
		MonsterThink( );
		m_iGoalAnim = TENTACLE_ANIM_Pit_Idle;
		return;
	}

	DispatchAnimEvents( );
	StudioFrameAdvance( );

	ChangeYaw( GetYawSpeed() );

	CSound *pSound;

	Listen( );

	// Listen will set this if there's something in my sound list
	if ( HasConditions( bits_COND_HEAR_SOUND ) )
		pSound = PBestSound();
	else
		pSound = NULL;

	if ( pSound )
	{
		Vector vecDir;
		if (gpGlobals->time - m_flPrevSoundTime < 0.5)
		{
			float dt = gpGlobals->time - m_flPrevSoundTime;
			vecDir = pSound->m_vecOrigin + (pSound->m_vecOrigin - m_vecPrevSound) / dt - GetAbsOrigin();
		}
		else
		{
			vecDir = pSound->m_vecOrigin - GetAbsOrigin();
		}
		m_flPrevSoundTime = gpGlobals->time;
		m_vecPrevSound = pSound->m_vecOrigin;

		m_flSoundYaw = UTIL_VecToYaw ( vecDir ) - m_flInitialYaw;
		m_iSoundLevel = Level( vecDir.z );

		if (m_flSoundYaw < -180)
			m_flSoundYaw += 360;
		if (m_flSoundYaw > 180)
			m_flSoundYaw -= 360;

		// ALERT( at_console, "sound %d %.0f\n", m_iSoundLevel, m_flSoundYaw );
		if (m_flSoundTime < gpGlobals->time)
		{
			// play "I hear new something" sound
			/*
			const char* sound;	

			switch( RANDOM_LONG(0,1) )
			{
			case 0: sound = "tentacle/te_alert1.wav"; break;
			case 1: sound = "tentacle/te_alert2.wav"; break;
			}
			*/

			// UTIL_EmitAmbientSound( this, GetAbsOrigin() + Vector( 0, 0, MyHeight()), sound, 1.0, ATTN_NORM, 0, 100);
		}
		m_flSoundTime = gpGlobals->time + RANDOM_FLOAT( 5.0, 10.0 );
	}

	// clip ideal_yaw
	float dy = m_flSoundYaw;
	switch( GetSequence() )
	{
	case TENTACLE_ANIM_Floor_Rear:
	case TENTACLE_ANIM_Floor_Rear_Idle:
	case TENTACLE_ANIM_Lev1_Rear:
	case TENTACLE_ANIM_Lev1_Rear_Idle:
	case TENTACLE_ANIM_Lev2_Rear:
	case TENTACLE_ANIM_Lev2_Rear_Idle:
	case TENTACLE_ANIM_Lev3_Rear:
	case TENTACLE_ANIM_Lev3_Rear_Idle:
		if (dy < 0 && dy > -m_flMaxYaw)
			dy = -m_flMaxYaw;
		if (dy > 0 && dy < m_flMaxYaw)
			dy = m_flMaxYaw;
		break;
	default:
		if (dy < -m_flMaxYaw)
			dy = -m_flMaxYaw;
		if (dy > m_flMaxYaw)
			dy = m_flMaxYaw;
	}
	SetIdealYaw( m_flInitialYaw + dy );

	if (m_fSequenceFinished)
	{
		// ALERT( at_console, "%s done %d %d\n", GetTargetname(), GetSequence(), m_iGoalAnim );
		if ( GetHealth() <= 1)
		{
			m_iGoalAnim = TENTACLE_ANIM_Pit_Idle;
			if ( GetSequence() == TENTACLE_ANIM_Pit_Idle)
			{
				SetHealth( 75 );
			}
		}
		else if ( m_flSoundTime > gpGlobals->time )
		{
			if (m_flSoundYaw >= -(m_flMaxYaw + 30) && m_flSoundYaw <= (m_flMaxYaw + 30))
			{
				// strike
				m_iGoalAnim = LookupActivity( ACT_T_STRIKE + m_iSoundLevel );
			}
			else if (m_flSoundYaw >= -m_flMaxYaw * 2 && m_flSoundYaw <= m_flMaxYaw * 2) 
			{
				// tap
				m_iGoalAnim = LookupActivity( ACT_T_TAP + m_iSoundLevel );
			}
			else
			{
				// go into rear idle
				m_iGoalAnim = LookupActivity( ACT_T_REARIDLE + m_iSoundLevel );
			}
		}
		else if ( GetSequence() == TENTACLE_ANIM_Pit_Idle)
		{
			// stay in pit until hear noise
			m_iGoalAnim = TENTACLE_ANIM_Pit_Idle;
		}
		else if ( GetSequence() == m_iGoalAnim)
		{
			if (MyLevel() >= 0 && gpGlobals->time < m_flSoundTime)
			{
				if (RANDOM_LONG(0,9) < m_flSoundTime - gpGlobals->time)
				{
					// continue stike
					m_iGoalAnim = LookupActivity( ACT_T_STRIKE + m_iSoundLevel );
				}
				else
				{
					// tap
					m_iGoalAnim = LookupActivity( ACT_T_TAP + m_iSoundLevel );
				}
			}
			else if (MyLevel( ) < 0)
			{
				m_iGoalAnim = LookupActivity( ACT_T_IDLE + 0 );
			}
			else
			{
				if (m_flNextSong < gpGlobals->time)
				{
					// play "I hear new something" sound
					const char* sound = "";	

					switch( RANDOM_LONG(0,1) )
					{
					case 0: sound = "tentacle/te_sing1.wav"; break;
					case 1: sound = "tentacle/te_sing2.wav"; break;
					}

					EMIT_SOUND( this, CHAN_VOICE, sound, 1.0, ATTN_NORM);

					m_flNextSong = gpGlobals->time + RANDOM_FLOAT( 10, 20 );
				}

				if (RANDOM_LONG(0,15) == 0)
				{
					// idle on new level
					m_iGoalAnim = LookupActivity( ACT_T_IDLE + RANDOM_LONG(0,3) );
				}
				else if (RANDOM_LONG(0,3)  == 0)
				{
					// tap
					m_iGoalAnim = LookupActivity( ACT_T_TAP + MyLevel( ) );
				}
				else
				{
					// idle
					m_iGoalAnim = LookupActivity( ACT_T_IDLE + MyLevel( ) );
				}
			}
			if (m_flSoundYaw < 0)
				m_flSoundYaw += RANDOM_FLOAT( 2, 8 );
			else
				m_flSoundYaw -= RANDOM_FLOAT( 2, 8 );
		}

		SetSequence( FindTransition( GetSequence(), m_iGoalAnim, &m_iDir ) );

		if (m_iDir > 0)
		{
			SetFrame( 0 );
		}
		else
		{
			m_iDir = -1; // just to safe
			SetFrame( 255 );
		}
		ResetSequenceInfo( );

		m_flFramerateAdj = RANDOM_FLOAT( -0.2, 0.2 );
		SetFrameRate( m_iDir * 1.0 + m_flFramerateAdj );

		switch( GetSequence() )
		{
		case TENTACLE_ANIM_Floor_Tap:
		case TENTACLE_ANIM_Lev1_Tap:
		case TENTACLE_ANIM_Lev2_Tap:
		case TENTACLE_ANIM_Lev3_Tap:
			{
				Vector vecSrc;
				UTIL_MakeVectors( GetAbsAngles() );

				TraceResult tr1, tr2;

				vecSrc = GetAbsOrigin() + Vector( 0, 0, MyHeight() - 4);
				UTIL_TraceLine( vecSrc, vecSrc + gpGlobals->v_forward * 512, ignore_monsters, ENT( pev ), &tr1 );

				vecSrc = GetAbsOrigin() + Vector( 0, 0, MyHeight() + 8);
				UTIL_TraceLine( vecSrc, vecSrc + gpGlobals->v_forward * 512, ignore_monsters, ENT( pev ), &tr2 );

				// ALERT( at_console, "%f %f\n", tr1.flFraction * 512, tr2.flFraction * 512 );

				m_flTapRadius = SetBlending( 0, RANDOM_FLOAT( tr1.flFraction * 512, tr2.flFraction * 512 ) );
			}
			break;
		default:
			m_flTapRadius = 336; // 400 - 64
			break;
		}
		Vector vecView = GetViewOffset();
		vecView.z = MyHeight();
		SetViewOffset( vecView );
		// ALERT( at_console, "seq %d\n", GetSequence() );
	}

	if (m_flPrevSoundTime + 2.0 > gpGlobals->time)
	{
		// 1.5 normal speed if hears sounds
		SetFrameRate( m_iDir * 1.5 + m_flFramerateAdj );
	}
	else if (m_flPrevSoundTime + 5.0 > gpGlobals->time)
	{
		// slowdown to normal
		SetFrameRate( m_iDir + m_iDir * (5 - (gpGlobals->time - m_flPrevSoundTime)) / 2 + m_flFramerateAdj );
	}
}

void CTentacle::CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// ALERT( at_console, "%s triggered %d\n", GetTargetname(), useType ); 
	switch( useType )
	{
	case USE_OFF:
		SetTakeDamageMode( DAMAGE_NO );
		SetThink( &CTentacle::DieThink );
		m_iGoalAnim = TENTACLE_ANIM_Engine_Death1;
		break;
	case USE_ON:
		if (pActivator)
		{
			// ALERT( at_console, "insert sound\n");
			CSoundEnt::InsertSound ( bits_SOUND_WORLD, pActivator->GetAbsOrigin(), 1024, 1.0 );
		}
		break;
	case USE_SET:
		break;
	case USE_TOGGLE:
		SetTakeDamageMode( DAMAGE_NO );
		SetThink( &CTentacle::DieThink );
		m_iGoalAnim = TENTACLE_ANIM_Engine_Idle;
		break;
	}

}

void CTentacle :: DieThink( void )
{
	SetNextThink( gpGlobals-> time + 0.1 );

	DispatchAnimEvents( );
	StudioFrameAdvance( );

	ChangeYaw( 24 );

	if (m_fSequenceFinished)
	{
		if ( GetSequence() == m_iGoalAnim)
		{
			switch( m_iGoalAnim )
			{
			case TENTACLE_ANIM_Engine_Idle:
			case TENTACLE_ANIM_Engine_Sway:
			case TENTACLE_ANIM_Engine_Swat:
			case TENTACLE_ANIM_Engine_Bob:
				m_iGoalAnim = TENTACLE_ANIM_Engine_Sway + RANDOM_LONG( 0, 2 );
				break;
			case TENTACLE_ANIM_Engine_Death1:
			case TENTACLE_ANIM_Engine_Death2:
			case TENTACLE_ANIM_Engine_Death3:
				UTIL_Remove( this );
				return;
			}
		}

		// ALERT( at_console, "%d : %d => ", GetSequence(), m_iGoalAnim );
		SetSequence( FindTransition( GetSequence(), m_iGoalAnim, &m_iDir ) );
		// ALERT( at_console, "%d\n", GetSequence() );

		if (m_iDir > 0)
		{
			SetFrame( 0 );
		}
		else
		{
			SetFrame( 255 );
		}
		ResetSequenceInfo( );

		float dy;
		switch( GetSequence() )
		{
		case TENTACLE_ANIM_Floor_Rear:
		case TENTACLE_ANIM_Floor_Rear_Idle:
		case TENTACLE_ANIM_Lev1_Rear:
		case TENTACLE_ANIM_Lev1_Rear_Idle:
		case TENTACLE_ANIM_Lev2_Rear:
		case TENTACLE_ANIM_Lev2_Rear_Idle:
		case TENTACLE_ANIM_Lev3_Rear:
		case TENTACLE_ANIM_Lev3_Rear_Idle:
		case TENTACLE_ANIM_Engine_Idle:
		case TENTACLE_ANIM_Engine_Sway:
		case TENTACLE_ANIM_Engine_Swat:
		case TENTACLE_ANIM_Engine_Bob:
		case TENTACLE_ANIM_Engine_Death1:
		case TENTACLE_ANIM_Engine_Death2:
		case TENTACLE_ANIM_Engine_Death3:
			SetFrameRate( RANDOM_FLOAT( m_iDir - 0.2, m_iDir + 0.2 ) );
			dy = 180;
			break;
		default:
			SetFrameRate( 1.5 );
			dy = 0;
			break;
		}
		SetIdealYaw( m_flInitialYaw + dy );
	}
}

void CTentacle :: HandleAnimEvent( AnimEvent_t& event )
{
	const char* sound = "";

	switch( event.event )
	{
	case 1:	// bang 
		{
			Vector vecSrc, vecAngles;
			GetAttachment( 0, vecSrc, vecAngles );

			// Vector vecSrc = GetAbsOrigin() + m_flTapRadius * Vector( cos( GetAbsAngles().y * (3.14192653 / 180.0) ), sin( GetAbsAngles().y * (M_PI / 180.0) ), 0.0 );

			// vecSrc.z += MyHeight( );

			switch( m_iTapSound )
			{
			case TE_SILO:
				UTIL_EmitAmbientSound( this, vecSrc, RANDOM_SOUND_ARRAY( pHitSilo ), 1.0, ATTN_NORM, 0, 100);
				break;
			case TE_NONE:
				break;
			case TE_DIRT:
				UTIL_EmitAmbientSound( this, vecSrc, RANDOM_SOUND_ARRAY( pHitDirt ), 1.0, ATTN_NORM, 0, 100);
				break;
			case TE_WATER:
				UTIL_EmitAmbientSound( this, vecSrc, RANDOM_SOUND_ARRAY( pHitWater ), 1.0, ATTN_NORM, 0, 100);
				break;
			}
          gpGlobals->force_retouch++;
		}
		break;

	case 3: // start killing swing
		m_iHitDmg = 200;
		// UTIL_EmitAmbientSound( this, GetAbsOrigin() + Vector( 0, 0, MyHeight()), "tentacle/te_swing1.wav", 1.0, ATTN_NORM, 0, 100);
		break;

	case 4: // end killing swing
		m_iHitDmg = 25;
		break;

	case 5: // just "whoosh" sound
		// UTIL_EmitAmbientSound( this, GetAbsOrigin() + Vector( 0, 0, MyHeight()), "tentacle/te_swing2.wav", 1.0, ATTN_NORM, 0, 100);
		break;

	case 2:	// tap scrape
	case 6: // light tap
		{
			Vector vecSrc = GetAbsOrigin() + m_flTapRadius * Vector( cos( GetAbsAngles().y * (M_PI / 180.0) ), sin( GetAbsAngles().y * (M_PI / 180.0) ), 0.0 );

			vecSrc.z += MyHeight( );

			float flVol = RANDOM_FLOAT( 0.3, 0.5 );

			switch( m_iTapSound )
			{
			case TE_SILO:
				UTIL_EmitAmbientSound( this, vecSrc, RANDOM_SOUND_ARRAY( pHitSilo ), flVol, ATTN_NORM, 0, 100);
				break;
			case TE_NONE:
				break;
			case TE_DIRT:
				UTIL_EmitAmbientSound( this, vecSrc, RANDOM_SOUND_ARRAY( pHitDirt ), flVol, ATTN_NORM, 0, 100);
				break;
			case TE_WATER:
				UTIL_EmitAmbientSound( this, vecSrc, RANDOM_SOUND_ARRAY( pHitWater ), flVol, ATTN_NORM, 0, 100);
				break;
			}
		}
		break;


	case 7: // roar
		switch( RANDOM_LONG(0,1) )
		{
		default:
		case 0: sound = "tentacle/te_roar1.wav"; break;
		case 1: sound = "tentacle/te_roar2.wav"; break;
		}

		UTIL_EmitAmbientSound( this, GetAbsOrigin() + Vector( 0, 0, MyHeight()), sound, 1.0, ATTN_NORM, 0, 100);
		break;

	case 8: // search
		switch( RANDOM_LONG(0,1) )
		{
		case 0: sound = "tentacle/te_search1.wav"; break;
		case 1: sound = "tentacle/te_search2.wav"; break;
		}

		UTIL_EmitAmbientSound( this, GetAbsOrigin() + Vector( 0, 0, MyHeight()), sound, 1.0, ATTN_NORM, 0, 100);
		break;

	case 9: // swing
		switch( RANDOM_LONG(0,1) )
		{
		case 0: sound = "tentacle/te_move1.wav"; break;
		case 1: sound = "tentacle/te_move2.wav"; break;
		}

		UTIL_EmitAmbientSound( this, GetAbsOrigin() + Vector( 0, 0, MyHeight()), sound, 1.0, ATTN_NORM, 0, 100);
		break;

	default:
		CBaseMonster::HandleAnimEvent( event );
	}
}

//
// TentacleStart
//
// void CTentacle :: Start( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
void CTentacle :: Start( void )
{
	SetThink( &CTentacle::Cycle );

	if ( !g_fFlySound )
	{
		EMIT_SOUND ( this, CHAN_BODY, "ambience/flies.wav", 1, ATTN_NORM );
		g_fFlySound = true;
//		SetNextThink( gpGlobals-> time + 0.1 );
	}
	else if ( !g_fSquirmSound )
	{
		EMIT_SOUND ( this, CHAN_BODY, "ambience/squirm2.wav", 1, ATTN_NORM );
		g_fSquirmSound = true;
	}
	
	SetNextThink( gpGlobals->time + 0.1 );
}

void CTentacle :: HitTouch( CBaseEntity *pOther )
{
	TraceResult tr = UTIL_GetGlobalTrace( );

	if (pOther->GetModelIndex() == GetModelIndex() )
		return;

	if (m_flHitTime > gpGlobals->time)
		return;

	// only look at the ones where the player hit me
	if (tr.pHit == NULL || tr.pHit->v.modelindex != GetModelIndex() )
		return;

	if (tr.iHitgroup >= 3)
	{
		pOther->TakeDamage( this, this, m_iHitDmg, DMG_CRUSH );
		// ALERT( at_console, "wack %3d : ", m_iHitDmg );
	}
	else if (tr.iHitgroup != 0)
	{
		pOther->TakeDamage( this, this, 20, DMG_CRUSH );
		// ALERT( at_console, "tap  %3d : ", 20 );
	}
	else
	{
		return; // Huh?
	}

	m_flHitTime = gpGlobals->time + 0.5;

	// ALERT( at_console, "%s : ", STRING( tr.pHit->v.classname ) );

	// ALERT( at_console, "%.0f : %s : %d\n", GetAbsAngles().y, pOther->GetClassname(), tr.iHitgroup );
}

void CTentacle::OnTakeDamage( const CTakeDamageInfo& info )
{
	if (info.GetDamage() > GetHealth() )
	{
		SetHealth( 1 );
	}
	else
	{
		SetHealth( GetHealth() - info.GetDamage() );
	}
}

void CTentacle::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	m_iGoalAnim = TENTACLE_ANIM_Pit_Idle;
}