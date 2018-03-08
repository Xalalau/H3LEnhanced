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
#include "CGlobalState.h"
// ############ hu3lifezado ############ //
// [MODO COOP]
#include "gamerules/GameRules.h"
// ############ //

const char* GLOBALESTATEToString( const GLOBALESTATE state )
{
	switch( state )
	{
	case GLOBAL_OFF:	return "Off";
	case GLOBAL_ON:		return "On";
	case GLOBAL_DEAD:	return "Dead";
	default:			return "unknown";
	}
}

CGlobalState gGlobalState;

CGlobalState::CGlobalState( void )
{
	Reset();
}

void CGlobalState::Reset( void )
{
	m_pList = NULL;
	m_listCount = 0;
}

globalentity_t *CGlobalState::Find( string_t globalname )
{
	// ############ hu3lifezado ############ //
	// [MODO COOP]
	// No modo coop nos nao carregamos nenhuma entidade de um mapa para o outro desse jeito
	if (g_pGameRules->IsCoOp())
	{
		return NULL;
	}
	// ############ //

	if( !globalname )
		return NULL;

	globalentity_t *pTest;
	const char *pEntityName = STRING( globalname );


	pTest = m_pList;
	while( pTest )
	{
		if( FStrEq( pEntityName, pTest->name ) )
			break;

		pTest = pTest->pNext;
	}

	return pTest;
}


// This is available all the time now on impulse 104, remove later
//#ifdef _DEBUG
void CGlobalState::DumpGlobals( void )
{
	ALERT( at_console, "-- Globals --\n" );
	
	for( globalentity_t *pTest = m_pList; pTest; pTest = pTest->pNext )
	{
		ALERT( at_console, "%s: %s (%s)\n", pTest->name, pTest->levelName, GLOBALESTATEToString( pTest->state ) );
	}
}
//#endif


void CGlobalState::EntityAdd( string_t globalname, string_t mapName, GLOBALESTATE state )
{
	ASSERT( !Find( globalname ) );

	globalentity_t *pNewEntity = ( globalentity_t * ) calloc( sizeof( globalentity_t ), 1 );
	ASSERT( pNewEntity != NULL );
	pNewEntity->pNext = m_pList;
	m_pList = pNewEntity;
	strcpy( pNewEntity->name, STRING( globalname ) );
	strcpy( pNewEntity->levelName, STRING( mapName ) );
	pNewEntity->state = state;
	m_listCount++;
}


void CGlobalState::EntitySetState( string_t globalname, GLOBALESTATE state )
{
	globalentity_t *pEnt = Find( globalname );

	if( pEnt )
		pEnt->state = state;
}


const globalentity_t *CGlobalState::EntityFromTable( string_t globalname )
{
	globalentity_t *pEnt = Find( globalname );

	return pEnt;
}


GLOBALESTATE CGlobalState::EntityGetState( string_t globalname )
{
	globalentity_t *pEnt = Find( globalname );
	if( pEnt )
		return pEnt->state;

	return GLOBAL_OFF;
}


// Global Savedata for Delay
BEGIN_DATADESC_NOBASE( CGlobalState )
DEFINE_FIELD( m_listCount, FIELD_INTEGER ),
END_DATADESC()

// Global Savedata for Delay
BEGIN_DATADESC_NOBASE( globalentity_t )
DEFINE_ARRAY( name, FIELD_CHARACTER, 64 ),
DEFINE_ARRAY( levelName, FIELD_CHARACTER, 32 ),
DEFINE_FIELD( state, FIELD_INTEGER ),
END_DATADESC()

bool CGlobalState::Save( CSave &save )
{
	const DataMap_t* pDataMap = GetDataMap();

	if( !save.WriteFields( "GLOBAL", this, *pDataMap, pDataMap->pTypeDesc, pDataMap->uiNumDescriptors ) )
		return false;

	const DataMap_t* pGlobalDataMap = globalentity_t::GetThisDataMap();

	globalentity_t *pEntity = m_pList;
	for( int i = 0; i < m_listCount && pEntity; i++ )
	{
		if( !save.WriteFields( "GENT", pEntity, *pGlobalDataMap, pGlobalDataMap->pTypeDesc, pGlobalDataMap->uiNumDescriptors ) )
			return false;

		pEntity = pEntity->pNext;
	}

	return true;
}

bool CGlobalState::Restore( CRestore &restore )
{
	const DataMap_t* pDataMap = GetDataMap();

	globalentity_t tmpEntity;

	ClearStates();
	if( !restore.ReadFields( "GLOBAL", this, *pDataMap, pDataMap->pTypeDesc, pDataMap->uiNumDescriptors ) )
		return false;

	const DataMap_t* pGlobalDataMap = globalentity_t::GetThisDataMap();

	const int listCount = m_listCount;	// Get new list count
	m_listCount = 0;				// Clear loaded data

	for( int i = 0; i < listCount; i++ )
	{
		if( !restore.ReadFields( "GENT", &tmpEntity, *pGlobalDataMap, pGlobalDataMap->pTypeDesc, pGlobalDataMap->uiNumDescriptors ) )
			return false;
		EntityAdd( MAKE_STRING( tmpEntity.name ), MAKE_STRING( tmpEntity.levelName ), tmpEntity.state );
	}
	return true;
}

void CGlobalState::EntityUpdate( string_t globalname, string_t mapname )
{
	globalentity_t *pEnt = Find( globalname );

	if( pEnt )
		strcpy( pEnt->levelName, STRING( mapname ) );
}


void CGlobalState::ClearStates( void )
{
	globalentity_t *pFree = m_pList;
	while( pFree )
	{
		globalentity_t *pNext = pFree->pNext;
		free( pFree );
		pFree = pNext;
	}
	Reset();
}


void SaveGlobalState( SAVERESTOREDATA *pSaveData )
{
	CSave saveHelper( pSaveData );
	gGlobalState.Save( saveHelper );
}


void RestoreGlobalState( SAVERESTOREDATA *pSaveData )
{
	CRestore restoreHelper( pSaveData );
	gGlobalState.Restore( restoreHelper );
}


void ResetGlobalState( void )
{
	gGlobalState.ClearStates();
	gInitHUD = true;	// Init the HUD on a new game / load game
}