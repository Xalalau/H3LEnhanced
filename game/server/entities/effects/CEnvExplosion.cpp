/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== explode.cpp ========================================================

  Explosion-related code

*/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Decals.h"

#include "CEnvExplosion.h"

BEGIN_DATADESC(	CEnvExplosion )
	DEFINE_FIELD( m_iMagnitude, FIELD_INTEGER ),
	DEFINE_FIELD( m_spriteScale, FIELD_INTEGER ),
	DEFINE_THINKFUNC( Smoke ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( env_explosion, CEnvExplosion );

void CEnvExplosion::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "iMagnitude"))
	{
		SetMagnitude( atoi( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CEnvExplosion::Spawn( void )
{ 
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;

	pev->movetype = MOVETYPE_NONE;
	/*
	if ( m_iMagnitude > 250 )
	{
		m_iMagnitude = 250;
	}
	*/

	float flSpriteScale;
	flSpriteScale = ( m_iMagnitude - 50) * 0.6;
	
	/*
	if ( flSpriteScale > 50 )
	{
		flSpriteScale = 50;
	}
	*/
	if ( flSpriteScale < 10 )
	{
		flSpriteScale = 10;
	}

	m_spriteScale = (int)flSpriteScale;
}

void CEnvExplosion::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ 
	TraceResult tr;

	pev->model = iStringNull;//invisible
	pev->solid = SOLID_NOT;// intangible

	Vector		vecSpot;// trace starts here!

	vecSpot = GetAbsOrigin() + Vector ( 0 , 0 , 8 );
	
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -40 ),  ignore_monsters, ENT(pev), & tr);
	
	// Pull out of the wall a bit
	if ( tr.flFraction != 1.0 )
	{
		pev->origin = tr.vecEndPos + (tr.vecPlaneNormal * (m_iMagnitude - 24) * 0.6);
	}
	else
	{
		pev->origin = GetAbsOrigin();
	}

	// draw decal
	if (! ( pev->spawnflags & SF_ENVEXPLOSION_NODECAL))
	{
		if ( RANDOM_FLOAT( 0 , 1 ) < 0.5 )
		{
			UTIL_DecalTrace( &tr, DECAL_SCORCH1 );
		}
		else
		{
			UTIL_DecalTrace( &tr, DECAL_SCORCH2 );
		}
	}

	// draw fireball
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NOFIREBALL ) )
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, GetAbsOrigin() );
			WRITE_BYTE( TE_EXPLOSION);
			WRITE_COORD( GetAbsOrigin().x );
			WRITE_COORD( GetAbsOrigin().y );
			WRITE_COORD( GetAbsOrigin().z );
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( ( byte )m_spriteScale ); // scale * 10
			WRITE_BYTE( 15  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, GetAbsOrigin() );
			WRITE_BYTE( TE_EXPLOSION);
			WRITE_COORD( GetAbsOrigin().x );
			WRITE_COORD( GetAbsOrigin().y );
			WRITE_COORD( GetAbsOrigin().z );
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 0 ); // no sprite
			WRITE_BYTE( 15  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	}

	// do damage
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NODAMAGE ) )
	{
		RadiusDamage( this, this, m_iMagnitude, EntityClassifications().GetNoneId(), DMG_BLAST );
	}

	SetThink( &CEnvExplosion::Smoke );
	pev->nextthink = gpGlobals->time + 0.3;

	// draw sparks
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NOSPARKS ) )
	{
		int sparkCount = RANDOM_LONG(0,3);

		for ( int i = 0; i < sparkCount; i++ )
		{
			Create( "spark_shower", GetAbsOrigin(), tr.vecPlaneNormal, NULL );
		}
	}
}

void CEnvExplosion::Smoke( void )
{
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NOSMOKE ) )
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, GetAbsOrigin() );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( GetAbsOrigin().x );
			WRITE_COORD( GetAbsOrigin().y );
			WRITE_COORD( GetAbsOrigin().z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( ( byte )m_spriteScale ); // scale * 10
			WRITE_BYTE( 12  ); // framerate
		MESSAGE_END();
	}
	
	if ( !(pev->spawnflags & SF_ENVEXPLOSION_REPEATABLE) )
	{
		UTIL_Remove( this );
	}
}

void UTIL_CreateExplosion( Vector vecCenter, const Vector& vecAngles, CBaseEntity* pOwner, int iMagnitude, const bool bDoDamage, const float flDelay, const float flRandomRange )
{
	if( flRandomRange != 0 )
	{
		vecCenter.x += UTIL_RandomFloat( -flRandomRange, flRandomRange );
		vecCenter.y += UTIL_RandomFloat( -flRandomRange, flRandomRange );
	}

	CEnvExplosion* pExplosion = static_cast<CEnvExplosion*>( CBaseEntity::Create( "env_explosion", vecCenter, vecAngles, pOwner ? pOwner->edict() : nullptr, false ) );

	pExplosion->SetMagnitude( iMagnitude );

	if( !bDoDamage )
		pExplosion->pev->spawnflags |= SF_ENVEXPLOSION_NODAMAGE;

	pExplosion->Spawn();

	if( flDelay <= 0 )
	{
		pExplosion->Use( nullptr, nullptr, USE_TOGGLE, 0 );
	}
	else
	{
		pExplosion->SetThink( &CBaseEntity::SUB_CallUseToggle );
		pExplosion->pev->nextthink = gpGlobals->time + flDelay;
	}
}
