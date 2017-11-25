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

//TODO: overhaul the delayed triggering system to use a dedicated list. Saves edicts for actual entities - Solokiller
LINK_ENTITY_TO_CLASS( DelayedUse, CBaseDelay );

void CBaseDelay::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "delay" ) )
	{
		m_flDelay = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "killtarget" ) )
	{
		m_iszKillTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
	{
		CBaseEntity::KeyValue( pkvd );
	}
}

/*
==============================
SUB_UseTargets

If self.delay is set, a DelayedUse entity will be created that will actually
do the SUB_UseTargets after that many seconds have passed.

Removes all entities with a targetname that match self.killtarget,
and removes them, so some events can remove other triggers.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function (if they have one)

==============================
*/
void CBaseDelay::SUB_UseTargets( CBaseEntity *pActivator, USE_TYPE useType, float value )
{
	//
	// exit immediatly if we don't have a target or kill target
	//
	if( !HasTarget() && !m_iszKillTarget )
		return;

	//
	// check for a delay
	//
	if( m_flDelay != 0 )
	{
		// create a temp object to fire at a later time
		auto pTemp = static_cast<CBaseDelay*>( UTIL_CreateNamedEntity( "DelayedUse" ) );

		pTemp->SetNextThink( gpGlobals->time + m_flDelay );

		pTemp->SetThink( &CBaseDelay::DelayThink );

		// Save the useType
		pTemp->GetButtons().Set( ( int ) useType );
		pTemp->m_iszKillTarget = m_iszKillTarget;
		pTemp->m_flDelay = 0; // prevent "recursion"
		pTemp->SetTarget( GetTarget() );

		// HACKHACK
		// This wasn't in the release build of Half-Life.  We should have moved m_hActivator into this class
		// but changing member variable hierarchy would break save/restore without some ugly code.
		// This code is not as ugly as that code
		if( pActivator && pActivator->IsPlayer() )		// If a player activates, then save it
		{
			pTemp->SetOwner( pActivator );
		}
		else
		{
			pTemp->SetOwner( NULL );
		}

		return;
	}

	//
	// kill the killtargets
	//

	if( m_iszKillTarget )
	{
		CBaseEntity* pKillTarget = nullptr;

		ALERT( at_aiconsole, "KillTarget: %s\n", STRING( m_iszKillTarget ) );
		while( ( pKillTarget = UTIL_FindEntityByTargetname( pKillTarget, STRING( m_iszKillTarget ) ) ) != nullptr )
		{
			UTIL_Remove( pKillTarget );

			ALERT( at_aiconsole, "killing %s\n", pKillTarget->GetClassname() );
		}
	}

	//
	// fire targets
	//
	if( HasTarget() )
	{
		FireTargets( GetTarget(), pActivator, this, useType, value );
	}
}

void CBaseDelay::DelayThink( void )
{
	CBaseEntity *pActivator = NULL;

	if( GetOwner() )		// A player activated this on delay
	{
		pActivator = GetOwner();
	}
	// The use type is cached (and stashed) in GetButtons()
	SUB_UseTargets( pActivator, ( USE_TYPE ) GetButtons().Get(), 0 );
	UTIL_RemoveNow( this );
}