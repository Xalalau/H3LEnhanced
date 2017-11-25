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
#include "Skill.h"
#include "cbase.h"
#include "Weapons.h"

#include "CControllerHeadBall.h"

BEGIN_DATADESC( CControllerHeadBall )
	DEFINE_THINKFUNC( HuntThink ),
	DEFINE_THINKFUNC( DieThink ),
	DEFINE_TOUCHFUNC( BounceTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( controller_head_ball, CControllerHeadBall );

void CControllerHeadBall::Spawn( void )
{
	Precache();
	// motor
	SetMoveType( MOVETYPE_FLY );
	SetSolidType( SOLID_BBOX );

	SetModel( "sprites/xspark4.spr" );
	SetRenderMode( kRenderTransAdd );
	SetRenderColor( Vector( 255, 255, 255 ) );
	SetRenderAmount( 255 );
	SetScale( 2.0 );

	SetSize( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );
	SetAbsOrigin( GetAbsOrigin() );

	SetThink( &CControllerHeadBall::HuntThink );
	SetTouch( &CControllerHeadBall::BounceTouch );

	m_vecIdeal = Vector( 0, 0, 0 );

	SetNextThink( gpGlobals->time + 0.1 );

	m_hOwner = GetOwner();
	SetDamageTime( gpGlobals->time );
}

void CControllerHeadBall::Precache( void )
{
	PRECACHE_MODEL( "sprites/xspark1.spr" );
	PRECACHE_SOUND( "debris/zap4.wav" );
	PRECACHE_SOUND( "weapons/electro4.wav" );
}

void CControllerHeadBall::HuntThink( void )
{
	SetNextThink( gpGlobals->time + 0.1 );

	SetRenderAmount( GetRenderAmount() - 5 );

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_ELIGHT );
	WRITE_SHORT( entindex() );		// entity, attachment
	WRITE_COORD( GetAbsOrigin().x );		// origin
	WRITE_COORD( GetAbsOrigin().y );
	WRITE_COORD( GetAbsOrigin().z );
	WRITE_COORD( GetRenderAmount() / 16 );	// radius
	WRITE_BYTE( 255 );	// R
	WRITE_BYTE( 255 );	// G
	WRITE_BYTE( 255 );	// B
	WRITE_BYTE( 2 );	// life * 10
	WRITE_COORD( 0 ); // decay
	MESSAGE_END();

	// check world boundaries
	//TODO: use constants - Solokiller
	if( gpGlobals->time - GetDamageTime() > 5 || GetRenderAmount() < 64 || m_hEnemy == NULL || m_hOwner == NULL || GetAbsOrigin().x < -4096 || GetAbsOrigin().x > 4096 || GetAbsOrigin().y < -4096 || GetAbsOrigin().y > 4096 || GetAbsOrigin().z < -4096 || GetAbsOrigin().z > 4096 )
	{
		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}

	MovetoTarget( m_hEnemy->Center() );

	if( ( m_hEnemy->Center() - GetAbsOrigin() ).Length() < 64 )
	{
		TraceResult tr;

		UTIL_TraceLine( GetAbsOrigin(), m_hEnemy->Center(), dont_ignore_monsters, ENT( pev ), &tr );

		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if( pEntity != NULL && pEntity->GetTakeDamageMode() != DAMAGE_NO )
		{
			g_MultiDamage.Clear();
			pEntity->TraceAttack( CTakeDamageInfo( m_hOwner, gSkillData.GetControllerDmgZap(), DMG_SHOCK ), GetAbsVelocity(), tr );
			g_MultiDamage.ApplyMultiDamage( this, m_hOwner );
		}

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMENTPOINT );
		WRITE_SHORT( entindex() );
		WRITE_COORD( tr.vecEndPos.x );
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z );
		WRITE_SHORT( g_sModelIndexLaser );
		WRITE_BYTE( 0 ); // frame start
		WRITE_BYTE( 10 ); // framerate
		WRITE_BYTE( 3 ); // life
		WRITE_BYTE( 20 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );	// brightness
		WRITE_BYTE( 10 );		// speed
		MESSAGE_END();

		UTIL_EmitAmbientSound( this, tr.vecEndPos, "weapons/electro4.wav", 0.5, ATTN_NORM, 0, RANDOM_LONG( 140, 160 ) );

		SetThink( &CControllerHeadBall::DieThink );
		SetNextThink( gpGlobals->time + 0.3 );
	}

	// Crawl( );
}

void CControllerHeadBall::DieThink( void )
{
	UTIL_Remove( this );
}

void CControllerHeadBall::BounceTouch( CBaseEntity *pOther )
{
	Vector vecDir = m_vecIdeal.Normalize();

	TraceResult tr = UTIL_GetGlobalTrace();

	float n = -DotProduct( tr.vecPlaneNormal, vecDir );

	vecDir = 2.0 * tr.vecPlaneNormal * n + vecDir;

	m_vecIdeal = vecDir * m_vecIdeal.Length();
}

void CControllerHeadBall::MovetoTarget( Vector vecTarget )
{
	// accelerate
	float flSpeed = m_vecIdeal.Length();
	if( flSpeed == 0 )
	{
		m_vecIdeal = GetAbsVelocity();
		flSpeed = m_vecIdeal.Length();
	}

	if( flSpeed > 400 )
	{
		m_vecIdeal = m_vecIdeal.Normalize() * 400;
	}
	m_vecIdeal = m_vecIdeal + ( vecTarget - GetAbsOrigin() ).Normalize() * 100;
	SetAbsVelocity( m_vecIdeal );
}

void CControllerHeadBall::Crawl( void )
{

	Vector vecAim = Vector( RANDOM_FLOAT( -1, 1 ), RANDOM_FLOAT( -1, 1 ), RANDOM_FLOAT( -1, 1 ) ).Normalize();
	Vector vecPnt = GetAbsOrigin() + GetAbsVelocity() * 0.3 + vecAim * 64;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_BEAMENTPOINT );
	WRITE_SHORT( entindex() );
	WRITE_COORD( vecPnt.x );
	WRITE_COORD( vecPnt.y );
	WRITE_COORD( vecPnt.z );
	WRITE_SHORT( g_sModelIndexLaser );
	WRITE_BYTE( 0 ); // frame start
	WRITE_BYTE( 10 ); // framerate
	WRITE_BYTE( 3 ); // life
	WRITE_BYTE( 20 );  // width
	WRITE_BYTE( 0 );   // noise
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 255 );	// brightness
	WRITE_BYTE( 10 );		// speed
	MESSAGE_END();
}