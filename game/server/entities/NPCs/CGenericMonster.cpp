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
// Generic Monster - purely for scripted sequence work.
//=========================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"

#include "CGenericMonster.h"

LINK_ENTITY_TO_CLASS( monster_generic, CGenericMonster );

EntityClassification_t CGenericMonster::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::PLAYER_ALLY );
}

void CGenericMonster::UpdateYawSpeed()
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

void CGenericMonster :: HandleAnimEvent( AnimEvent_t& event )
{
	switch( event.event )
	{
	case 0:
	default:
		CBaseMonster::HandleAnimEvent( event );
		break;
	}
}

int CGenericMonster :: ISoundMask ( void )
{
	//generic monster can't hear.
	return 0;
}

void CGenericMonster :: Spawn()
{
	Precache();

	SetModel( GetModelName() );

/*
	if ( FStrEq( GetModelName(), "models/player.mdl" ) )
		SetSize( VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	else
		SetSize( VEC_HULL_MIN, VEC_HULL_MAX);
*/

	if ( FStrEq( GetModelName(), "models/player.mdl" ) || FStrEq( GetModelName(), "models/holo.mdl" ) )
		SetSize( VEC_HULL_MIN, VEC_HULL_MAX );
	else
		SetSize( VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= BLOOD_COLOR_RED;
	SetHealth( 8 );
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();

	if ( GetSpawnFlags().Any( SF_GENERICMONSTER_NOTSOLID ) )
	{
		SetSolidType( SOLID_NOT );
		SetTakeDamageMode( DAMAGE_NO );
	}
}

void CGenericMonster :: Precache()
{
	PRECACHE_MODEL( GetModelName() );
}	

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
