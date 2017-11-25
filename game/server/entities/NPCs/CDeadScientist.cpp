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
#include "CTalkMonster.h"

#include "CScientist.h"

#include "CDeadScientist.h"

const char* const CDeadScientist::m_szPoses[] = { "lying_on_back", "lying_on_stomach", "dead_sitting", "dead_hang", "dead_table1", "dead_table2", "dead_table3" };

LINK_ENTITY_TO_CLASS( monster_scientist_dead, CDeadScientist );

//
// ********** DeadScientist SPAWN **********
//
void CDeadScientist::Spawn()
{
	PRECACHE_MODEL( "models/scientist.mdl" );
	SetModel( "models/scientist.mdl" );

	GetEffects().ClearAll();
	SetSequence( 0 );
	// Corpses have less health
	SetHealth( 8 );//gSkillData.GetScientistHealth();

	m_bloodColor = BLOOD_COLOR_RED;

	if( GetBody() == -1 )
	{// -1 chooses a random head
		SetBody( RANDOM_LONG( 0, NUM_SCIENTIST_HEADS - 1 ) );// pick a head, any head
	}
	// Luther is black, make his hands black
	if( GetBody() == HEAD_LUTHER )
		SetSkin( 1 );
	else
		SetSkin( 0 );

	SetSequence( LookupSequence( m_szPoses[ m_iPose ] ) );
	if( GetSequence() == -1 )
	{
		ALERT( at_console, "Dead scientist with bad pose\n" );
	}

	//	SetSkin( GetSkin() + 2 ); // use bloody skin -- UNDONE: Turn this back on when we have a bloody skin again!
	MonsterInitDead();
}

void CDeadScientist::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "pose" ) )
	{
		m_iPose = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}