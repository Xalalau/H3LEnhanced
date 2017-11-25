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
#include "CSquadMonster.h"
#include "Monsters.h"

#include "CHGrunt.h"

#include "CDeadHGrunt.h"

const char* const CDeadHGrunt::m_szPoses[] = { "deadstomach", "deadside", "deadsitting" };

LINK_ENTITY_TO_CLASS( monster_hgrunt_dead, CDeadHGrunt );

//=========================================================
// ********** DeadHGrunt SPAWN **********
//=========================================================
void CDeadHGrunt::Spawn( void )
{
	PRECACHE_MODEL( "models/hgrunt.mdl" );
	SetModel( "models/hgrunt.mdl" );

	GetEffects().ClearAll();
	SetYawSpeed( 8 );
	SetSequence( 0 );
	m_bloodColor = BLOOD_COLOR_RED;

	SetSequence( LookupSequence( m_szPoses[ m_iPose ] ) );

	if( GetSequence() == -1 )
	{
		ALERT( at_console, "Dead hgrunt with bad pose\n" );
	}

	// Corpses have less health
	SetHealth( 8 );

	// map old bodies onto new bodies
	switch( GetBody() )
	{
	case 0: // Grunt with Gun
		SetBody( 0 );
		SetSkin( 0 );
		SetBodygroup( HEAD_GROUP, HEAD_GRUNT );
		SetBodygroup( GUN_GROUP, GUN_MP5 );
		break;
	case 1: // Commander with Gun
		SetBody( 0 );
		SetSkin( 0 );
		SetBodygroup( HEAD_GROUP, HEAD_COMMANDER );
		SetBodygroup( GUN_GROUP, GUN_MP5 );
		break;
	case 2: // Grunt no Gun
		SetBody( 0 );
		SetSkin( 0 );
		SetBodygroup( HEAD_GROUP, HEAD_GRUNT );
		SetBodygroup( GUN_GROUP, GUN_NONE );
		break;
	case 3: // Commander no Gun
		SetBody( 0 );
		SetSkin( 0 );
		SetBodygroup( HEAD_GROUP, HEAD_COMMANDER );
		SetBodygroup( GUN_GROUP, GUN_NONE );
		break;
	}

	MonsterInitDead();
}

void CDeadHGrunt::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "pose" ) )
	{
		m_iPose = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}