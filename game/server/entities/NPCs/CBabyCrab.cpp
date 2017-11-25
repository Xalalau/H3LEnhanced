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

#include "CBabyCrab.h"

LINK_ENTITY_TO_CLASS( monster_babycrab, CBabyCrab );

void CBabyCrab::Spawn( void )
{
	CHeadCrab::Spawn();
	SetModel( "models/baby_headcrab.mdl" );
	SetRenderMode( kRenderTransTexture );
	SetRenderAmount( 192 );
	SetSize( Vector( -12, -12, 0 ), Vector( 12, 12, 24 ) );

	SetHealth( gSkillData.GetHeadcrabHealth() * 0.25 );	// less health than full grown
}

void CBabyCrab::Precache( void )
{
	PRECACHE_MODEL( "models/baby_headcrab.mdl" );
	CHeadCrab::Precache();
}

void CBabyCrab::UpdateYawSpeed()
{
	SetYawSpeed( 120 );
}

bool CBabyCrab::CheckRangeAttack1( float flDot, float flDist )
{
	if( GetFlags().Any( FL_ONGROUND ) )
	{
		auto pGroundEntity = GetGroundEntity();

		if( pGroundEntity && pGroundEntity->GetFlags().Any( FL_CLIENT | FL_MONSTER ) )
			return true;

		// A little less accurate, but jump from closer
		if( flDist <= 180 && flDot >= 0.55 )
			return true;
	}

	return false;
}

Schedule_t* CBabyCrab::GetScheduleOfType( int Type )
{
	switch( Type )
	{
	case SCHED_FAIL:	// If you fail, try to jump!
		if( m_hEnemy != NULL )
			return slHCRangeAttack1Fast;
		break;

	case SCHED_RANGE_ATTACK1:
		{
			return slHCRangeAttack1Fast;
		}
		break;
	}

	return CHeadCrab::GetScheduleOfType( Type );
}