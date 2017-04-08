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

#include "CNihilanth.h"

#include "CNihilanthHVR.h"

BEGIN_DATADESC( CNihilanthHVR )
	DEFINE_FIELD( m_flIdealVel, FIELD_FLOAT ),
	DEFINE_FIELD( m_vecIdeal, FIELD_VECTOR ),
	DEFINE_FIELD( m_pNihilanth, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_hTouch, FIELD_EHANDLE ),
	DEFINE_FIELD( m_nFrames, FIELD_INTEGER ),
	DEFINE_THINKFUNC( HoverThink ),
	DEFINE_THINKFUNC( DissipateThink ),
	DEFINE_THINKFUNC( ZapThink ),
	DEFINE_THINKFUNC( TeleportThink ),
	DEFINE_TOUCHFUNC( TeleportTouch ),
	DEFINE_TOUCHFUNC( RemoveTouch ),
	DEFINE_TOUCHFUNC( BounceTouch ),
	DEFINE_TOUCHFUNC( ZapTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( nihilanth_energy_ball, CNihilanthHVR );


void CNihilanthHVR::Spawn( void )
{
	Precache();

	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	pev->scale = 3.0;
}

void CNihilanthHVR::Precache( void )
{
	PRECACHE_MODEL( "sprites/flare6.spr" );
	PRECACHE_MODEL( "sprites/nhth1.spr" );
	PRECACHE_MODEL( "sprites/exit1.spr" );
	PRECACHE_MODEL( "sprites/tele1.spr" );
	PRECACHE_MODEL( "sprites/animglow01.spr" );
	PRECACHE_MODEL( "sprites/xspark4.spr" );
	PRECACHE_MODEL( "sprites/muzzleflash3.spr" );
	PRECACHE_SOUND( "debris/zap4.wav" );
	PRECACHE_SOUND( "weapons/electro4.wav" );
	PRECACHE_SOUND( "x/x_teleattack1.wav" );
}

void CNihilanthHVR::CircleInit( CBaseEntity *pTarget )
{
	pev->movetype = MOVETYPE_NOCLIP;
	pev->solid = SOLID_NOT;

	// SetModel( "sprites/flare6.spr");
	// pev->scale = 3.0;
	// SetModel( "sprites/xspark4.spr");
	SetModel( "sprites/muzzleflash3.spr" );
	pev->rendercolor.x = 255;
	pev->rendercolor.y = 224;
	pev->rendercolor.z = 192;
	pev->scale = 2.0;
	m_nFrames = 1;
	pev->renderamt = 255;

	SetSize( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );
	SetAbsOrigin( GetAbsOrigin() );

	SetThink( &CNihilanthHVR::HoverThink );
	SetTouch( &CNihilanthHVR::BounceTouch );
	pev->nextthink = gpGlobals->time + 0.1;

	m_hTargetEnt = pTarget;
}

void CNihilanthHVR::AbsorbInit( void )
{
	SetThink( &CNihilanthHVR::DissipateThink );
	pev->renderamt = 255;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_BEAMENTS );
	WRITE_SHORT( this->entindex() );
	WRITE_SHORT( m_hTargetEnt->entindex() + 0x1000 );
	WRITE_SHORT( g_sModelIndexLaser );
	WRITE_BYTE( 0 ); // framestart
	WRITE_BYTE( 0 ); // framerate
	WRITE_BYTE( 50 ); // life
	WRITE_BYTE( 80 );  // width
	WRITE_BYTE( 80 );   // noise
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 128 );   // r, g, b
	WRITE_BYTE( 64 );   // r, g, b
	WRITE_BYTE( 255 );	// brightness
	WRITE_BYTE( 30 );		// speed
	MESSAGE_END();
}

void CNihilanthHVR::TeleportInit( CNihilanth *pOwner, CBaseEntity *pEnemy, CBaseEntity *pTarget, CBaseEntity *pTouch )
{
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	pev->rendercolor.x = 255;
	pev->rendercolor.y = 255;
	pev->rendercolor.z = 255;
	pev->velocity.z *= 0.2;

	SetModel( "sprites/exit1.spr" );

	m_pNihilanth = pOwner;
	m_hEnemy = pEnemy;
	m_hTargetEnt = pTarget;
	m_hTouch = pTouch;

	SetThink( &CNihilanthHVR::TeleportThink );
	SetTouch( &CNihilanthHVR::TeleportTouch );
	pev->nextthink = gpGlobals->time + 0.1;

	EMIT_SOUND_DYN( this, CHAN_WEAPON, "x/x_teleattack1.wav", 1, 0.2, 0, 100 );
}

void CNihilanthHVR::GreenBallInit()
{
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	pev->rendercolor.x = 255;
	pev->rendercolor.y = 255;
	pev->rendercolor.z = 255;
	pev->scale = 1.0;

	SetModel( "sprites/exit1.spr" );

	SetTouch( &CNihilanthHVR::RemoveTouch );
}

void CNihilanthHVR::ZapInit( CBaseEntity *pEnemy )
{
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SetModel( "sprites/nhth1.spr" );

	pev->rendercolor.x = 255;
	pev->rendercolor.y = 255;
	pev->rendercolor.z = 255;
	pev->scale = 2.0;

	pev->velocity = ( pEnemy->GetAbsOrigin() - GetAbsOrigin() ).Normalize() * 200;

	m_hEnemy = pEnemy;
	SetThink( &CNihilanthHVR::ZapThink );
	SetTouch( &CNihilanthHVR::ZapTouch );
	pev->nextthink = gpGlobals->time + 0.1;

	EMIT_SOUND_DYN( this, CHAN_WEAPON, "debris/zap4.wav", 1, ATTN_NORM, 0, 100 );
}

void CNihilanthHVR::HoverThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if( m_hTargetEnt != NULL )
	{
		CircleTarget( m_hTargetEnt->GetAbsOrigin() + Vector( 0, 0, 16 * N_SCALE ) );
	}
	else
	{
		UTIL_Remove( this );
	}


	if( RANDOM_LONG( 0, 99 ) < 5 )
	{
		/*
		CBaseEntity *pOther = UTIL_RandomClassname( GetClassname() );

		if (pOther && pOther != this)
		{
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMENTS );
		WRITE_SHORT( this->entindex() );
		WRITE_SHORT( pOther->entindex() );
		WRITE_SHORT( g_sModelIndexLaser );
		WRITE_BYTE( 0 ); // framestart
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 10 ); // life
		WRITE_BYTE( 80 );  // width
		WRITE_BYTE( 80 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 128 );   // r, g, b
		WRITE_BYTE( 64 );   // r, g, b
		WRITE_BYTE( 255 );	// brightness
		WRITE_BYTE( 30 );		// speed
		MESSAGE_END();
		}
		*/
		/*
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMENTS );
		WRITE_SHORT( this->entindex() );
		WRITE_SHORT( m_hTargetEnt->entindex() + 0x1000 );
		WRITE_SHORT( g_sModelIndexLaser );
		WRITE_BYTE( 0 ); // framestart
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 10 ); // life
		WRITE_BYTE( 80 );  // width
		WRITE_BYTE( 80 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 128 );   // r, g, b
		WRITE_BYTE( 64 );   // r, g, b
		WRITE_BYTE( 255 );	// brightness
		WRITE_BYTE( 30 );		// speed
		MESSAGE_END();
		*/
	}

	pev->frame = ( ( int ) pev->frame + 1 ) % m_nFrames;
}

bool CNihilanthHVR::CircleTarget( Vector vecTarget )
{
	bool fClose = false;

	Vector vecDest = vecTarget;
	Vector vecEst = GetAbsOrigin() + pev->velocity * 0.5;
	Vector vecSrc = GetAbsOrigin();
	vecDest.z = 0;
	vecEst.z = 0;
	vecSrc.z = 0;
	float d1 = ( vecDest - vecSrc ).Length() - 24 * N_SCALE;
	float d2 = ( vecDest - vecEst ).Length() - 24 * N_SCALE;

	if( m_vecIdeal == Vector( 0, 0, 0 ) )
	{
		m_vecIdeal = pev->velocity;
	}

	if( d1 < 0 && d2 <= d1 )
	{
		// ALERT( at_console, "too close\n");
		m_vecIdeal = m_vecIdeal - ( vecDest - vecSrc ).Normalize() * 50;
	}
	else if( d1 > 0 && d2 >= d1 )
	{
		// ALERT( at_console, "too far\n");
		m_vecIdeal = m_vecIdeal + ( vecDest - vecSrc ).Normalize() * 50;
	}
	pev->avelocity.z = d1 * 20;

	if( d1 < 32 )
	{
		fClose = true;
	}

	m_vecIdeal = m_vecIdeal + Vector( RANDOM_FLOAT( -2, 2 ), RANDOM_FLOAT( -2, 2 ), RANDOM_FLOAT( -2, 2 ) );
	m_vecIdeal = Vector( m_vecIdeal.x, m_vecIdeal.y, 0 ).Normalize() * 200
		/* + Vector( -m_vecIdeal.y, m_vecIdeal.x, 0 ).Normalize( ) * 32 */
		+ Vector( 0, 0, m_vecIdeal.z );
	// m_vecIdeal = m_vecIdeal + Vector( -m_vecIdeal.y, m_vecIdeal.x, 0 ).Normalize( ) * 2;

	// move up/down
	d1 = vecTarget.z - GetAbsOrigin().z;
	if( d1 > 0 && m_vecIdeal.z < 200 )
		m_vecIdeal.z += 20;
	else if( d1 < 0 && m_vecIdeal.z > -200 )
		m_vecIdeal.z -= 20;

	pev->velocity = m_vecIdeal;

	// ALERT( at_console, "%.0f %.0f %.0f\n", m_vecIdeal.x, m_vecIdeal.y, m_vecIdeal.z );
	return fClose;
}

void CNihilanthHVR::DissipateThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if( pev->scale > 5.0 )
		UTIL_Remove( this );

	pev->renderamt -= 2;
	pev->scale += 0.1;

	if( m_hTargetEnt != NULL )
	{
		CircleTarget( m_hTargetEnt->GetAbsOrigin() + Vector( 0, 0, 4096 ) );
	}
	else
	{
		UTIL_Remove( this );
	}

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_ELIGHT );
	WRITE_SHORT( entindex() );		// entity, attachment
	WRITE_COORD( GetAbsOrigin().x );		// origin
	WRITE_COORD( GetAbsOrigin().y );
	WRITE_COORD( GetAbsOrigin().z );
	WRITE_COORD( pev->renderamt );	// radius
	WRITE_BYTE( 255 );	// R
	WRITE_BYTE( 192 );	// G
	WRITE_BYTE( 64 );	// B
	WRITE_BYTE( 2 );	// life * 10
	WRITE_COORD( 0 ); // decay
	MESSAGE_END();
}

void CNihilanthHVR::ZapThink( void )
{
	pev->nextthink = gpGlobals->time + 0.05;

	// check world boundaries
	if( m_hEnemy == NULL || GetAbsOrigin().x < -4096 || GetAbsOrigin().x > 4096 || GetAbsOrigin().y < -4096 || GetAbsOrigin().y > 4096 || GetAbsOrigin().z < -4096 || GetAbsOrigin().z > 4096 )
	{
		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}

	if( pev->velocity.Length() < 2000 )
	{
		pev->velocity = pev->velocity * 1.2;
	}


	// MovetoTarget( m_hEnemy->Center( ) );

	if( ( m_hEnemy->Center() - GetAbsOrigin() ).Length() < 256 )
	{
		TraceResult tr;

		UTIL_TraceLine( GetAbsOrigin(), m_hEnemy->Center(), dont_ignore_monsters, edict(), &tr );

		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if( pEntity != NULL && pEntity->pev->takedamage )
		{
			g_MultiDamage.Clear();
			pEntity->TraceAttack( CTakeDamageInfo( this, gSkillData.GetNihilanthZap(), DMG_SHOCK ), pev->velocity, &tr );
			g_MultiDamage.ApplyMultiDamage( this, this );
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
		WRITE_BYTE( 20 );   // noise
		WRITE_BYTE( 64 );   // r, g, b
		WRITE_BYTE( 196 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );	// brightness
		WRITE_BYTE( 10 );		// speed
		MESSAGE_END();

		UTIL_EmitAmbientSound( this, tr.vecEndPos, "weapons/electro4.wav", 0.5, ATTN_NORM, 0, RANDOM_LONG( 140, 160 ) );

		SetTouch( NULL );
		UTIL_Remove( this );
		pev->nextthink = gpGlobals->time + 0.2;
		return;
	}

	pev->frame = ( int ) ( pev->frame + 1 ) % 11;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_ELIGHT );
	WRITE_SHORT( entindex() );		// entity, attachment
	WRITE_COORD( GetAbsOrigin().x );		// origin
	WRITE_COORD( GetAbsOrigin().y );
	WRITE_COORD( GetAbsOrigin().z );
	WRITE_COORD( 128 );	// radius
	WRITE_BYTE( 128 );	// R
	WRITE_BYTE( 128 );	// G
	WRITE_BYTE( 255 );	// B
	WRITE_BYTE( 10 );	// life * 10
	WRITE_COORD( 128 ); // decay
	MESSAGE_END();

	// Crawl( );
}

void CNihilanthHVR::TeleportThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	// check world boundaries
	if( m_hEnemy == NULL || !m_hEnemy->IsAlive() || !IsInWorld() )
	{
		STOP_SOUND( this, CHAN_WEAPON, "x/x_teleattack1.wav" );
		UTIL_Remove( this );
		return;
	}

	if( ( m_hEnemy->Center() - GetAbsOrigin() ).Length() < 128 )
	{
		STOP_SOUND( this, CHAN_WEAPON, "x/x_teleattack1.wav" );
		UTIL_Remove( this );

		if( m_hTargetEnt != NULL )
			m_hTargetEnt->Use( m_hEnemy, m_hEnemy, USE_ON, 1.0 );

		if( m_hTouch != NULL && m_hEnemy != NULL )
			m_hTouch->Touch( m_hEnemy );
	}
	else
	{
		MovetoTarget( m_hEnemy->Center() );
	}

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_ELIGHT );
	WRITE_SHORT( entindex() );		// entity, attachment
	WRITE_COORD( GetAbsOrigin().x );		// origin
	WRITE_COORD( GetAbsOrigin().y );
	WRITE_COORD( GetAbsOrigin().z );
	WRITE_COORD( 256 );	// radius
	WRITE_BYTE( 0 );	// R
	WRITE_BYTE( 255 );	// G
	WRITE_BYTE( 0 );	// B
	WRITE_BYTE( 10 );	// life * 10
	WRITE_COORD( 256 ); // decay
	MESSAGE_END();

	pev->frame = ( int ) ( pev->frame + 1 ) % 20;
}

void CNihilanthHVR::TeleportTouch( CBaseEntity *pOther )
{
	CBaseEntity *pEnemy = m_hEnemy;

	if( pOther == pEnemy )
	{
		if( m_hTargetEnt != NULL )
			m_hTargetEnt->Use( pEnemy, pEnemy, USE_ON, 1.0 );

		if( m_hTouch != NULL && pEnemy != NULL )
			m_hTouch->Touch( pEnemy );
	}
	else
	{
		m_pNihilanth->MakeFriend( GetAbsOrigin() );
	}

	SetTouch( NULL );
	STOP_SOUND( this, CHAN_WEAPON, "x/x_teleattack1.wav" );
	UTIL_Remove( this );
}

void CNihilanthHVR::RemoveTouch( CBaseEntity *pOther )
{
	STOP_SOUND( this, CHAN_WEAPON, "x/x_teleattack1.wav" );
	UTIL_Remove( this );
}

void CNihilanthHVR::BounceTouch( CBaseEntity *pOther )
{
	Vector vecDir = m_vecIdeal.Normalize();

	TraceResult tr = UTIL_GetGlobalTrace();

	float n = -DotProduct( tr.vecPlaneNormal, vecDir );

	vecDir = 2.0 * tr.vecPlaneNormal * n + vecDir;

	m_vecIdeal = vecDir * m_vecIdeal.Length();
}

void CNihilanthHVR::ZapTouch( CBaseEntity *pOther )
{
	UTIL_EmitAmbientSound( this, GetAbsOrigin(), "weapons/electro4.wav", 1.0, ATTN_NORM, 0, RANDOM_LONG( 90, 95 ) );

	RadiusDamage( this, this, 50, EntityClassifications().GetNoneId(), DMG_SHOCK );
	pev->velocity = pev->velocity * 0;

	/*
	for (int i = 0; i < 10; i++)
	{
	Crawl( );
	}
	*/

	SetTouch( NULL );
	UTIL_Remove( this );
	pev->nextthink = gpGlobals->time + 0.2;
}

void CNihilanthHVR::MovetoTarget( Vector vecTarget )
{
	if( m_vecIdeal == Vector( 0, 0, 0 ) )
	{
		m_vecIdeal = pev->velocity;
	}

	// accelerate
	float flSpeed = m_vecIdeal.Length();
	if( flSpeed > 300 )
	{
		m_vecIdeal = m_vecIdeal.Normalize() * 300;
	}
	m_vecIdeal = m_vecIdeal + ( vecTarget - GetAbsOrigin() ).Normalize() * 300;
	pev->velocity = m_vecIdeal;
}

void CNihilanthHVR::Crawl( void )
{

	Vector vecAim = Vector( RANDOM_FLOAT( -1, 1 ), RANDOM_FLOAT( -1, 1 ), RANDOM_FLOAT( -1, 1 ) ).Normalize();
	Vector vecPnt = GetAbsOrigin() + pev->velocity * 0.2 + vecAim * 128;

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
	WRITE_BYTE( 80 );   // noise
	WRITE_BYTE( 64 );   // r, g, b
	WRITE_BYTE( 128 );   // r, g, b
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 255 );	// brightness
	WRITE_BYTE( 10 );		// speed
	MESSAGE_END();
}