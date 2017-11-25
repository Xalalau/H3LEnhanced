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

EntityClassification_t CGMan::GetClassification()
{
	return EntityClassifications().GetNoneId();
}

void CGMan::UpdateYawSpeed()
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:
	default:
		ys = 90;
	}

	SetYawSpeed( ys );
}

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

int CGMan :: ISoundMask ( void )
{
	//generic monster can't hear.
	return 0;
}

void CGMan :: Spawn()
{
	Precache();

	SetModel( "models/gman.mdl" );
	SetSize( VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= DONT_BLEED;
	SetHealth( 100 );
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();
}

void CGMan :: Precache()
{
	PRECACHE_MODEL( "models/gman.mdl" );
}	


//=========================================================
// AI Schedules Specific to this monster
//=========================================================


void CGMan :: StartTask( const Task_t& task )
{
	switch( task.iTask )
	{
	case TASK_WAIT:
		if (m_hPlayer == NULL)
		{
			m_hPlayer = UTIL_FindEntityByClassname( NULL, "player" );
		}
		break;
	}
	CBaseMonster::StartTask( task );
}

void CGMan :: RunTask( const Task_t& task )
{
	switch( task.iTask )
	{
	case TASK_WAIT:
		// look at who I'm talking to
		if (m_flTalkTime > gpGlobals->time && m_hTalkTarget != NULL)
		{
			float yaw = VecToYaw(m_hTalkTarget->GetAbsOrigin() - GetAbsOrigin()) - GetAbsAngles().y;

			if (yaw > 180) yaw -= 360;
			if (yaw < -180) yaw += 360;

			// turn towards vector
			SetBoneController( 0, yaw );
		}
		// look at player, but only if playing a "safe" idle animation
		else if (m_hPlayer != NULL && GetSequence() == 0)
		{
			float yaw = VecToYaw(m_hPlayer->GetAbsOrigin() - GetAbsOrigin()) - GetAbsAngles().y;

			if (yaw > 180) yaw -= 360;
			if (yaw < -180) yaw += 360;

			// turn towards vector
			SetBoneController( 0, yaw );
		}
		else 
		{
			SetBoneController( 0, 0 );
		}
		CBaseMonster::RunTask( task );
		break;
	default:
		SetBoneController( 0, 0 );
		CBaseMonster::RunTask( task );
		break;
	}
}


//=========================================================
// Override all damage
//=========================================================
void CGMan::OnTakeDamage( const CTakeDamageInfo& info )
{
	SetHealth( GetMaxHealth() / 2 ); // always trigger the 50% damage aitrigger

	if ( info.GetDamage() > 0 )
	{
		SetConditions(bits_COND_LIGHT_DAMAGE);
	}

	if ( info.GetDamage() >= 20 )
	{
		SetConditions(bits_COND_HEAVY_DAMAGE);
	}
}


void CGMan::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	UTIL_Ricochet( tr.vecEndPos, 1.0 );
	g_MultiDamage.AddMultiDamage( info, this );
}


void CGMan::PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, const bool bConcurrent, CBaseEntity *pListener )
{
	CBaseMonster::PlayScriptedSentence( pszSentence, duration, volume, attenuation, bConcurrent, pListener );

	m_flTalkTime = gpGlobals->time + duration;
	m_hTalkTarget = pListener;
}
