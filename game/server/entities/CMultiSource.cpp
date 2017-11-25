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

#include "CMultiSource.h"

BEGIN_DATADESC( CMultiSource )
	//!!!BUGBUG FIX
	DEFINE_ARRAY( m_rgEntities, FIELD_EHANDLE, MS_MAX_TARGETS ),
	DEFINE_ARRAY( m_rgTriggered, FIELD_INTEGER, MS_MAX_TARGETS ),
	DEFINE_FIELD( m_iTotal, FIELD_INTEGER ),
	DEFINE_FIELD( m_globalstate, FIELD_STRING ),
	DEFINE_THINKFUNC( Register ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( multisource, CMultiSource );
//
// Cache user-entity-field values until spawn is called.
//

void CMultiSource::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "style" ) ||
		FStrEq( pkvd->szKeyName, "height" ) ||
		FStrEq( pkvd->szKeyName, "killtarget" ) ||
		FStrEq( pkvd->szKeyName, "value1" ) ||
		FStrEq( pkvd->szKeyName, "value2" ) ||
		FStrEq( pkvd->szKeyName, "value3" ) )
		pkvd->fHandled = true;
	else if( FStrEq( pkvd->szKeyName, "globalstate" ) )
	{
		m_globalstate = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

#define SF_MULTI_INIT		1

void CMultiSource::Spawn()
{
	// set up think for later registration

	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	SetNextThink( gpGlobals->time + 0.1 );
	GetSpawnFlags() |= SF_MULTI_INIT;	// Until it's initialized
	SetThink( &CMultiSource::Register );
}

void CMultiSource::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int i;

	bool bFound = false;

	// Find the entity in our list
	for( i = 0; i < m_iTotal; ++i )
	{
		if( m_rgEntities[ i ] == pCaller )
		{
			bFound = true;
			break;
		}
	}

	// if we didn't find it, report error and leave
	if( !bFound )
	{
		ALERT( at_console, "MultiSrc:Used by non member %s.\n", pCaller->GetClassname() );
		return;
	}

	// CONSIDER: a Use input to the multisource always toggles.  Could check useType for ON/OFF/TOGGLE

	m_rgTriggered[ i ] ^= 1;

	// 
	if( IsTriggered( pActivator ) )
	{
		ALERT( at_aiconsole, "Multisource %s enabled (%d inputs)\n", GetTargetname(), m_iTotal );
		USE_TYPE targetUseType = USE_TOGGLE;
		if( m_globalstate )
			targetUseType = USE_ON;
		SUB_UseTargets( NULL, targetUseType, 0 );
	}
}


bool CMultiSource::IsTriggered( const CBaseEntity* const ) const
{
	// Is everything triggered?
	int i = 0;

	// Still initializing?
	if( GetSpawnFlags().Any( SF_MULTI_INIT ) )
		return false;

	while( i < m_iTotal )
	{
		if( m_rgTriggered[ i ] == 0 )
			break;
		i++;
	}

	if( i == m_iTotal )
	{
		if( !m_globalstate || gGlobalState.EntityGetState( m_globalstate ) == GLOBAL_ON )
			return true;
	}

	return false;
}

void CMultiSource::Register( void )
{
	m_iTotal = 0;
	memset( m_rgEntities, 0, MS_MAX_TARGETS * sizeof( EHANDLE ) );

	SetThink( &CMultiSource::SUB_DoNothing );

	// search for all entities which target this multisource (GetTargetname())

	CBaseEntity* pTarget = nullptr;

	while( ( pTarget = UTIL_FindEntityByTarget( pTarget, GetTargetname() ) ) != nullptr && ( m_iTotal < MS_MAX_TARGETS ) )
	{
		m_rgEntities[ m_iTotal++ ] = pTarget;
	}

	pTarget = nullptr;

	while( ( pTarget = UTIL_FindEntityByClassname( pTarget, "multi_manager" ) ) != nullptr && ( m_iTotal < MS_MAX_TARGETS ) )
	{
		if( pTarget->HasTarget( GetTargetname() ) )
			m_rgEntities[ m_iTotal++ ] = pTarget;
	}

	GetSpawnFlags().ClearFlags( SF_MULTI_INIT );
}