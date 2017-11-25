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
// rat - environmental monster
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"

#include "CRat.h"

LINK_ENTITY_TO_CLASS( monster_rat, CRat );

EntityClassification_t CRat::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::INSECT );
}

void CRat::UpdateYawSpeed()
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:
	default:
		ys = 45;
		break;
	}

	SetYawSpeed( ys );
}

void CRat :: Spawn()
{
	Precache( );

	SetModel( "models/bigrat.mdl");
	SetSize( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= BLOOD_COLOR_RED;
	SetHealth( 8 );
	SetViewOffset( Vector ( 0, 0, 6 ) );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();
}

void CRat :: Precache()
{
	PRECACHE_MODEL("models/bigrat.mdl");
}	

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
