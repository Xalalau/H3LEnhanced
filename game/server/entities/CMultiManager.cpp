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

#include "CMultiManager.h"

// Global Savedata for multi_manager
BEGIN_DATADESC( CMultiManager )
	DEFINE_FIELD( m_cTargets, FIELD_INTEGER ),
	DEFINE_FIELD( m_index, FIELD_INTEGER ),
	DEFINE_FIELD( m_startTime, FIELD_TIME ),
	DEFINE_ARRAY( m_iTargetName, FIELD_STRING, MAX_MULTI_TARGETS ),
	DEFINE_ARRAY( m_flTargetDelay, FIELD_FLOAT, MAX_MULTI_TARGETS ),

	DEFINE_THINKFUNC( ManagerThink ),
	DEFINE_USEFUNC( ManagerUse ),

#if _DEBUG
	DEFINE_THINKFUNC( ManagerReport ),
#endif
END_DATADESC()

LINK_ENTITY_TO_CLASS( multi_manager, CMultiManager );

void CMultiManager::KeyValue( KeyValueData *pkvd )
{
	// UNDONE: Maybe this should do something like this:
	//CBaseToggle::KeyValue( pkvd );
	// if ( !pkvd->fHandled )
	// ... etc.

	if( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		m_flWait = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else // add this field to the target list
	{
		// this assumes that additional fields are targetnames and their values are delay values.
		if( m_cTargets < MAX_MULTI_TARGETS )
		{
			char tmp[ 128 ];

			UTIL_StripToken( pkvd->szKeyName, tmp );
			m_iTargetName[ m_cTargets ] = ALLOC_STRING( tmp );
			m_flTargetDelay[ m_cTargets ] = atof( pkvd->szValue );
			m_cTargets++;
			pkvd->fHandled = true;
		}
	}
}


void CMultiManager::Spawn( void )
{
	pev->solid = SOLID_NOT;
	SetUse( &CMultiManager::ManagerUse );
	SetThink( &CMultiManager::ManagerThink );

	// Sort targets
	// Quick and dirty bubble sort
	int swapped = 1;

	while( swapped )
	{
		swapped = 0;
		for( int i = 1; i < m_cTargets; i++ )
		{
			if( m_flTargetDelay[ i ] < m_flTargetDelay[ i - 1 ] )
			{
				// Swap out of order elements
				int name = m_iTargetName[ i ];
				float delay = m_flTargetDelay[ i ];
				m_iTargetName[ i ] = m_iTargetName[ i - 1 ];
				m_flTargetDelay[ i ] = m_flTargetDelay[ i - 1 ];
				m_iTargetName[ i - 1 ] = name;
				m_flTargetDelay[ i - 1 ] = delay;
				swapped = 1;
			}
		}
	}
}


bool CMultiManager::HasTarget( string_t targetname ) const
{
	for( int i = 0; i < m_cTargets; i++ )
		if( FStrEq( STRING( targetname ), STRING( m_iTargetName[ i ] ) ) )
			return true;

	return false;
}


// Designers were using this to fire targets that may or may not exist -- 
// so I changed it to use the standard target fire code, made it a little simpler.
void CMultiManager::ManagerThink( void )
{
	float	time;

	time = gpGlobals->time - m_startTime;
	while( m_index < m_cTargets && m_flTargetDelay[ m_index ] <= time )
	{
		FireTargets( STRING( m_iTargetName[ m_index ] ), m_hActivator, this, USE_TOGGLE, 0 );
		m_index++;
	}

	if( m_index >= m_cTargets )// have we fired all targets?
	{
		SetThink( NULL );
		if( IsClone() )
		{
			UTIL_Remove( this );
			return;
		}
		SetUse( &CMultiManager::ManagerUse );// allow manager re-use 
	}
	else
		pev->nextthink = m_startTime + m_flTargetDelay[ m_index ];
}

CMultiManager *CMultiManager::Clone( void )
{
	auto pMulti = static_cast<CMultiManager*>( UTIL_CreateNamedEntity( "multi_manager" ) );

	edict_t *pEdict = pMulti->pev->pContainingEntity;
	memcpy( pMulti->pev, pev, sizeof( *pev ) );
	pMulti->pev->pContainingEntity = pEdict;

	pMulti->pev->spawnflags |= SF_MULTIMAN_CLONE;
	pMulti->m_cTargets = m_cTargets;
	memcpy( pMulti->m_iTargetName, m_iTargetName, sizeof( m_iTargetName ) );
	memcpy( pMulti->m_flTargetDelay, m_flTargetDelay, sizeof( m_flTargetDelay ) );

	return pMulti;
}


// The USE function builds the time table and starts the entity thinking.
void CMultiManager::ManagerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// In multiplayer games, clone the MM and execute in the clone (like a thread)
	// to allow multiple players to trigger the same multimanager
	if( ShouldClone() )
	{
		CMultiManager *pClone = Clone();
		pClone->ManagerUse( pActivator, pCaller, useType, value );
		return;
	}

	m_hActivator = pActivator;
	m_index = 0;
	m_startTime = gpGlobals->time;

	SetUse( NULL );// disable use until all targets have fired

	SetThink( &CMultiManager::ManagerThink );
	pev->nextthink = gpGlobals->time;
}

#if _DEBUG
void CMultiManager::ManagerReport( void )
{
	int	cIndex;

	for( cIndex = 0; cIndex < m_cTargets; cIndex++ )
	{
		ALERT( at_console, "%s %f\n", STRING( m_iTargetName[ cIndex ] ), m_flTargetDelay[ cIndex ] );
	}
}
#endif