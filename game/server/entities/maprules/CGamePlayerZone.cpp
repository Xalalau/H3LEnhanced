/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"

#include "CGamePlayerZone.h"

BEGIN_DATADESC( CGamePlayerZone )
	DEFINE_FIELD( m_iszInTarget, FIELD_STRING ),
	DEFINE_FIELD( m_iszOutTarget, FIELD_STRING ),
	DEFINE_FIELD( m_iszInCount, FIELD_STRING ),
	DEFINE_FIELD( m_iszOutCount, FIELD_STRING ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( game_zone_player, CGamePlayerZone );

void CGamePlayerZone::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "intarget" ) )
	{
		m_iszInTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "outtarget" ) )
	{
		m_iszOutTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "incount" ) )
	{
		m_iszInCount = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "outcount" ) )
	{
		m_iszOutCount = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CRuleBrushEntity::KeyValue( pkvd );
}

void CGamePlayerZone::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int playersInCount = 0;
	int playersOutCount = 0;

	if( !CanFireForActivator( pActivator ) )
		return;

	CBaseEntity *pPlayer = NULL;

	for( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pPlayer = UTIL_PlayerByIndex( i );
		if( pPlayer )
		{
			TraceResult trace;
			Hull::Hull hullNumber = Hull::HUMAN;

			if( pPlayer->GetFlags().Any( FL_DUCKING ) )
			{
				hullNumber = Hull::HEAD;
			}

			UTIL_TraceModel( pPlayer->GetAbsOrigin(), pPlayer->GetAbsOrigin(), hullNumber, edict(), &trace );

			if( trace.fStartSolid )
			{
				playersInCount++;
				if( m_iszInTarget )
				{
					FireTargets( STRING( m_iszInTarget ), pPlayer, pActivator, useType, value );
				}
			}
			else
			{
				playersOutCount++;
				if( m_iszOutTarget )
				{
					FireTargets( STRING( m_iszOutTarget ), pPlayer, pActivator, useType, value );
				}
			}
		}
	}

	if( m_iszInCount )
	{
		FireTargets( STRING( m_iszInCount ), pActivator, this, USE_SET, playersInCount );
	}

	if( m_iszOutCount )
	{
		FireTargets( STRING( m_iszOutCount ), pActivator, this, USE_SET, playersOutCount );
	}
}