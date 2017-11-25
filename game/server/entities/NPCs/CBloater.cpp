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
//=========================================================
// Bloater
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"

#include "CBloater.h"

LINK_ENTITY_TO_CLASS( monster_bloater, CBloater );

EntityClassification_t CBloater::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::ALIEN_MONSTER );
}

void CBloater::UpdateYawSpeed()
{
	int ys;

	ys = 120;

#if 0
	switch ( m_Activity )
	{
	}
#endif

	SetYawSpeed( ys );
}

void CBloater::OnTakeDamage( const CTakeDamageInfo& info )
{
	PainSound();
	CBaseMonster::OnTakeDamage( info );
}

void CBloater :: PainSound( void )
{
#if 0	
	int pitch = 95 + RANDOM_LONG(0,9);

	switch (RANDOM_LONG(0,5))
	{
	case 0: 
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_pain1.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	case 1:
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_pain2.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	default:
		break;
	}
#endif
}

void CBloater :: AlertSound( void )
{
#if 0
	int pitch = 95 + RANDOM_LONG(0,9);

	switch (RANDOM_LONG(0,2))
	{
	case 0: 
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_alert10.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	case 1:
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_alert20.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	case 2:
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_alert30.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	}
#endif
}

void CBloater :: IdleSound( void )
{
#if 0
	int pitch = 95 + RANDOM_LONG(0,9);

	switch (RANDOM_LONG(0,2))
	{
	case 0: 
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_idle1.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	case 1:
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_idle2.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	case 2:
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_idle3.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	}
#endif
}

void CBloater :: AttackSnd( void )
{
#if 0
	int pitch = 95 + RANDOM_LONG(0,9);

	switch (RANDOM_LONG(0,1))
	{
	case 0: 
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_attack1.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	case 1:
		EMIT_SOUND_DYN( this, CHAN_VOICE, "zombie/zo_attack2.wav", 1.0, ATTN_NORM, 0, pitch);
		break;
	}
#endif
}

void CBloater :: HandleAnimEvent( AnimEvent_t& event )
{
	switch( event.event )
	{
		case BLOATER_AE_ATTACK_MELEE1:
		{
			// do stuff for this event.
			AttackSnd();
		}
		break;

		default:
			CBaseMonster::HandleAnimEvent( event );
			break;
	}
}

void CBloater :: Spawn()
{
	Precache( );

	SetModel( "models/floater.mdl");
	SetSize( VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_FLY );
	GetSpawnFlags() |= FL_FLY;
	m_bloodColor		= BLOOD_COLOR_GREEN;
	SetHealth( 40 );
	SetViewOffset( VEC_VIEW );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();
}

void CBloater :: Precache()
{
	PRECACHE_MODEL("models/floater.mdl");
}	

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

