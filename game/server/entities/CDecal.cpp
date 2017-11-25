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

#include "CDecal.h"

BEGIN_DATADESC( CDecal )
	DEFINE_THINKFUNC( StaticDecal ),
	DEFINE_USEFUNC( TriggerDecal ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( infodecal, CDecal );

// UNDONE:  These won't get sent to joining players in multi-player
void CDecal::Spawn( void )
{
	if( GetSkin() < 0 || ( gpGlobals->deathmatch && GetSpawnFlags().Any( SF_DECAL_NOTINDEATHMATCH ) ) )
	{
		UTIL_RemoveNow( this );
		return;
	}

	if( !HasTargetname() )
	{
		SetThink( &CDecal::StaticDecal );
		// if there's no targetname, the decal will spray itself on as soon as the world is done spawning.
		SetNextThink( gpGlobals->time );
	}
	else
	{
		// if there IS a targetname, the decal sprays itself on when it is triggered.
		SetThink( &CDecal::SUB_DoNothing );
		SetUse( &CDecal::TriggerDecal );
	}
}

void CDecal::TriggerDecal( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// this is set up as a USE function for infodecals that have targetnames, so that the
	// decal doesn't get applied until it is fired. (usually by a scripted sequence)
	TraceResult trace;

	UTIL_TraceLine( GetAbsOrigin() - Vector( 5, 5, 5 ), GetAbsOrigin() + Vector( 5, 5, 5 ), ignore_monsters, ENT( pev ), &trace );

	CBaseEntity* pEntity = CBaseEntity::Instance( trace.pHit );

	//Should never happen since the engine sets it to the world if nothing was hit. - Solokiller
	if( !pEntity )
		return;

	const int entityIndex = ( short ) pEntity->entindex();

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_BSPDECAL );
	WRITE_COORD( GetAbsOrigin().x );
	WRITE_COORD( GetAbsOrigin().y );
	WRITE_COORD( GetAbsOrigin().z );
	WRITE_SHORT( ( int ) GetSkin() );
	WRITE_SHORT( entityIndex );
	if( entityIndex )
		WRITE_SHORT( ( int ) pEntity->GetModelIndex() );
	MESSAGE_END();

	SetThink( &CDecal::SUB_Remove );
	SetNextThink( gpGlobals->time + 0.1 );
}


void CDecal::StaticDecal( void )
{
	TraceResult trace;

	UTIL_TraceLine( GetAbsOrigin() - Vector( 5, 5, 5 ), GetAbsOrigin() + Vector( 5, 5, 5 ), ignore_monsters, ENT( pev ), &trace );

	CBaseEntity* pEntity = CBaseEntity::Instance( trace.pHit );

	//Should never happen since the engine sets it to the world if nothing was hit. - Solokiller
	if( !pEntity )
		return;

	const int entityIndex = ( short ) pEntity->entindex();

	const int modelIndex = entityIndex ? ( int ) pEntity->GetModelIndex() : 0;

	g_engfuncs.pfnStaticDecal( GetAbsOrigin(), ( int ) GetSkin(), entityIndex, modelIndex );

	SUB_Remove();
}


void CDecal::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "texture" ) )
	{
		SetSkin( DECAL_INDEX( pkvd->szValue ) );

		// Found
		if( GetSkin() >= 0 )
			return;
		ALERT( at_console, "Can't find decal %s\n", pkvd->szValue );
	}
	else
		CBaseEntity::KeyValue( pkvd );
}