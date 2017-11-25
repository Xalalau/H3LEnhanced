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
#include "gamerules/GameRules.h"

#include "CBaseSpawnPoint.h"

DLL_GLOBAL CBaseEntity* g_pLastSpawn = nullptr;

BEGIN_DATADESC( CBaseSpawnPoint )
	DEFINE_FIELD( m_bEnabled, FIELD_BOOLEAN ),
END_DATADESC()

void CBaseSpawnPoint::KeyValue( KeyValueData* pkvd )
{
	if( FStrEq( pkvd->szKeyName, "enabled" ) )
	{
		m_bEnabled = atoi( pkvd->szValue ) != 0;
		pkvd->fHandled = true;
	}
	else
		BaseClass::KeyValue( pkvd );
}

void CBaseSpawnPoint::PlayerSpawned( CBasePlayer* const pPlayer )
{
	//In multiplayer, trigger targets on spawn.
	if( g_pGameRules->IsMultiplayer() && HasTarget() )
	{
		FireTargets( GetTarget(), pPlayer, pPlayer, USE_TOGGLE, 0 );
	}
}

bool IsSpawnPointValid( CBasePlayer* pPlayer, CBaseSpawnPoint* pSpawnPoint )
{
	CBaseEntity *ent = NULL;

	if( !pSpawnPoint->CanUseSpawnPoint( pPlayer ) )
	{
		return false;
	}

	while( ( (ent = UTIL_FindEntityInSphere( ent, pSpawnPoint->GetAbsOrigin(), 128 ) ) != NULL) )
	{
		// if ent is a client, don't spawn on 'em
		if( ent->IsPlayer() && ent != pPlayer )
			return false;
	}

	return true;
}

CBaseSpawnPoint* FindSpawnPoint( CBasePlayer* pPlayer, const char* const pszClassName, const bool bRandomPoint, const bool bFallbackToFirst, const bool bUseLastSpawn )
{
	//g_pLastSpawn could be the world. Shouldn't matter after the first random call.
	CBaseEntity* pSpot = bUseLastSpawn ? g_pLastSpawn : nullptr;

	if( bRandomPoint )
	{
		// Randomize the start spot
		for( int i = RANDOM_LONG( 1, 5 ); i > 0; i-- )
			pSpot = UTIL_FindEntityByClassname( pSpot, pszClassName );

		if( FNullEnt( pSpot ) )  // skip over the null point
			pSpot = UTIL_FindEntityByClassname( pSpot, pszClassName );
	}
	else
	{
		pSpot = UTIL_FindEntityByClassname( pSpot, pszClassName );
	}

	CBaseEntity* const pFirstSpot = pSpot;

	bool bValidPoint = false;

	do
	{
		if( pSpot )
		{
			// check if pSpot is valid
			if( IsSpawnPointValid( pPlayer, static_cast<CBaseSpawnPoint*>( pSpot ) ) )
			{
				if( pSpot->GetAbsOrigin() == g_vecZero )
				{
					pSpot = UTIL_FindEntityByClassname( pSpot, pszClassName );
					continue;
				}

				// if so, go to pSpot
				bValidPoint = true;
				break;
			}
		}
		// increment pSpot
		pSpot = UTIL_FindEntityByClassname( pSpot, pszClassName );
	}
	while( pSpot != pFirstSpot ); // loop if we're not back to the start

	// we haven't found a place to spawn yet, so kill any guy at the first spawn point and spawn there
	if( bFallbackToFirst && !bValidPoint && !FNullEnt( pSpot ) )
	{
		CBaseEntity* ent = nullptr;
		while( ( ent = UTIL_FindEntityInSphere( ent, pSpot->GetAbsOrigin(), 128 ) ) != nullptr )
		{
			// if ent is a client, kill em (unless they are ourselves)
			if( ent->IsPlayer() && ent != pPlayer )
				ent->TakeDamage( CWorld::GetInstance(), CWorld::GetInstance(), 300, DMG_GENERIC );
		}
	}

	return static_cast<CBaseSpawnPoint*>( pSpot );
}

CBaseSpawnPoint* FindSpawnPoint( CBasePlayer* pPlayer, const char* const* ppszClassNames, const size_t uiNumClassNames, 
								 const bool bRandomPoint, const bool bFallbackToFirst, const bool bUseLastSpawn )
{
	for( size_t uiIndex = 0; uiIndex < uiNumClassNames; ++uiIndex )
	{
		if( auto pSpawnpoint = FindSpawnPoint( pPlayer, ppszClassNames[ uiIndex ], bRandomPoint, bFallbackToFirst, bUseLastSpawn ) )
			return pSpawnpoint;
	}

	return nullptr;
}

static const char* const g_pszCoOpSpawnPoints[] = 
{
	"info_player_coop",
	"info_player_start"
};

CBaseEntity* EntSelectSpawnPoint( CBasePlayer* pPlayer )
{
	CBaseEntity* pSpot = nullptr;

	// choose a info_player_deathmatch point
	if( g_pGameRules->IsCoOp() )
	{
		pSpot = FindSpawnPoint( pPlayer, g_pszCoOpSpawnPoints, ARRAYSIZE( g_pszCoOpSpawnPoints ), false, false, true );
	}
	else if( g_pGameRules->IsDeathmatch() )
	{
		pSpot = FindSpawnPoint( pPlayer, "info_player_deathmatch", true, true, true );
	}

	if( FNullEnt( pSpot ) )
	{
		// If startspot is set, (re)spawn there.
		if( FStringNull( gpGlobals->startspot ) || !strlen( STRING( gpGlobals->startspot ) ) )
		{
			pSpot = FindSpawnPoint( pPlayer, "info_player_start", false, true, false );
		}
		else
		{
			pSpot = UTIL_FindEntityByTargetname( nullptr, STRING( gpGlobals->startspot ) );
		}
	}

	if( FNullEnt( pSpot ) )
	{
		ALERT( at_error, "PutClientInServer: no info_player_start on level" );
		//Use the world.
		return CWorld::GetInstance();
	}

	g_pLastSpawn = pSpot;

	return pSpot;
}