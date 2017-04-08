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
// GMan - misunderstood servant of the people
//=========================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"
#include "Weapons.h"

#include "CGMan.h"

BEGIN_DATADESC(	CGMan )
	DEFINE_FIELD( m_hTalkTarget, FIELD_EHANDLE ),
	DEFINE_FIELD( m_flTalkTime, FIELD_TIME ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_gman, CGMan );

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
EntityClassification_t CGMan::GetClassification()
{
	return EntityClassifications().GetNoneId();
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CGMan :: SetYawSpeed ( void )
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:
	default:
		ys = 90;
	}

	pev->yaw_speed = ys;
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CGMan :: HandleAnimEvent( AnimEvent_t& event )
{
	switch( event.event )
	{
	case 0:
	default:
		CBaseMonster::HandleAnimEvent( event );
		break;
	}
}

//=========================================================
// ISoundMask - generic monster can't hear.
//=========================================================
int CGMan :: ISoundMask ( void )
{
	return 0;
}

//=========================================================
// Spawn
//=========================================================
void CGMan :: Spawn()
{
	Precache();

	SetModel( "models/gman.mdl" );
	SetSize( VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= DONT_BLEED;
	pev->health			= 100;
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CGMan :: Precache()
{
	PRECACHE_MODEL( "models/gman.mdl" );
}	


//=========================================================
// AI Schedules Specific to this monster
//=========================================================


void CGMan :: StartTask( const Task_t* pTask )
{
	switch( pTask->iTask )
	{
	case TASK_WAIT:
		if (m_hPlayer == NULL)
		{
			m_hPlayer = UTIL_FindEntityByClassname( NULL, "player" );
		}
		break;
	}
	CBaseMonster::StartTask( pTask );
}

void CGMan :: RunTask( const Task_t* pTask )
{
	switch( pTask->iTask )
	{
	case TASK_WAIT:
		// look at who I'm talking to
		if (m_flTalkTime > gpGlobals->time && m_hTalkTarget != NULL)
		{
			float yaw = VecToYaw(m_hTalkTarget->GetAbsOrigin() - GetAbsOrigin()) - pev->angles.y;

			if (yaw > 180) yaw -= 360;
			if (yaw < -180) yaw += 360;

			// turn towards vector
			SetBoneController( 0, yaw );
		}
		// look at player, but only if playing a "safe" idle animation
		else if (m_hPlayer != NULL && pev->sequence == 0)
		{
			float yaw = VecToYaw(m_hPlayer->GetAbsOrigin() - GetAbsOrigin()) - pev->angles.y;

			if (yaw > 180) yaw -= 360;
			if (yaw < -180) yaw += 360;

			// turn towards vector
			SetBoneController( 0, yaw );
		}
		else 
		{
			SetBoneController( 0, 0 );
		}
		CBaseMonster::RunTask( pTask );
		break;
	default:
		SetBoneController( 0, 0 );
		CBaseMonster::RunTask( pTask );
		break;
	}
}


//=========================================================
// Override all damage
//=========================================================
void CGMan::OnTakeDamage( const CTakeDamageInfo& info )
{
	pev->health = pev->max_health / 2; // always trigger the 50% damage aitrigger

	if ( info.GetDamage() > 0 )
	{
		SetConditions(bits_COND_LIGHT_DAMAGE);
	}

	if ( info.GetDamage() >= 20 )
	{
		SetConditions(bits_COND_HEAVY_DAMAGE);
	}
}


void CGMan::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr )
{
	UTIL_Ricochet( ptr->vecEndPos, 1.0 );
	g_MultiDamage.AddMultiDamage( info, this );
}


void CGMan::PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, const bool bConcurrent, CBaseEntity *pListener )
{
	CBaseMonster::PlayScriptedSentence( pszSentence, duration, volume, attenuation, bConcurrent, pListener );

	m_flTalkTime = gpGlobals->time + duration;
	m_hTalkTarget = pListener;
}
