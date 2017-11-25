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
/*

===== subs.cpp ========================================================

  frequently used global functions

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "SaveRestore.h"
#include "nodes/Nodes.h"
#include "DoorConstants.h"

extern CGraph WorldGraph;

extern bool FEntIsVisible(entvars_t* pev, entvars_t* pevTarget);

// This updates global tables that need to know about entities being removed
void CBaseEntity::UpdateOnRemove( void )
{
	int	i;

	if ( GetFlags().Any( FL_GRAPHED ) )
	{
	// this entity was a LinkEnt in the world node graph, so we must remove it from
	// the graph since we are removing it from the world.
		for ( i = 0 ; i < WorldGraph.m_cLinks ; i++ )
		{
			if ( WorldGraph.m_pLinkPool [ i ].m_pLinkEnt == pev )
			{
				// if this link has a link ent which is the same ent that is removing itself, remove it!
				WorldGraph.m_pLinkPool [ i ].m_pLinkEnt = NULL;
			}
		}
	}
	if ( HasGlobalName() )
		gGlobalState.EntitySetState( MAKE_STRING( GetGlobalName() ), GLOBAL_DEAD );

	// tell owner ( if any ) that we're dead.This is mostly for MonsterMaker functionality.
	//Killtarget didn't do this before, so the counter broke. - Solokiller
	if( CBaseEntity* pOwner = GetOwner() )
	{
		pOwner->DeathNotice( this );
	}
}

// Convenient way to delay removing oneself
void CBaseEntity :: SUB_Remove( void )
{
	UpdateOnRemove();
	if ( GetHealth() > 0)
	{
		// this situation can screw up monsters who can't tell their entity pointers are invalid.
		SetHealth( 0 );
		ALERT( at_aiconsole, "SUB_Remove called on entity with health > 0\n");
	}

	UTIL_RemoveNow( this );
}

void CBaseEntity :: SUB_UseTargets( CBaseEntity *pActivator, USE_TYPE useType, float value )
{
	//
	// fire targets
	//
	if ( HasTarget() )
	{
		FireTargets( GetTarget(), pActivator, this, useType, value );
	}
}


void FireTargets( const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !targetName )
		return;

	ALERT( at_aiconsole, "Firing: (%s)\n", targetName );

	CBaseEntity* pTarget = nullptr;

	while( ( pTarget = UTIL_FindEntityByTargetname( pTarget, targetName ) ) != nullptr )
	{
		if( !pTarget->GetFlags().Any( FL_KILLME ) ) // Don't use dying ents
		{
			ALERT( at_aiconsole, "Found: %s, firing (%s)\n", pTarget->GetClassname(), targetName );
			pTarget->Use( pActivator, pCaller, useType, value );
		}
	}
}

/*
QuakeEd only writes a single float for angles (bad idea), so up and down are
just constant angles.
*/
void SetMovedir( CBaseEntity* pEntity )
{
	if ( pEntity->GetAbsAngles() == Vector(0, -1, 0))
	{
		pEntity->SetMoveDir( Vector(0, 0, 1) );
	}
	else if ( pEntity->GetAbsAngles() == Vector(0, -2, 0))
	{
		pEntity->SetMoveDir( Vector(0, 0, -1) );
	}
	else
	{
		UTIL_MakeVectors( pEntity->GetAbsAngles() );
		pEntity->SetMoveDir( gpGlobals->v_forward );
	}
	
	pEntity->SetAbsAngles( g_vecZero );
}

/*
=============
FEntIsVisible

returns true if the passed entity is visible to caller, even if not infront ()
=============
*/
bool
FEntIsVisible(
	entvars_t*		pev,
	entvars_t*		pevTarget)
{
	Vector vecSpot1 = pev->origin + pev->view_ofs;
	Vector vecSpot2 = pevTarget->origin + pevTarget->view_ofs;
	TraceResult tr;

	UTIL_TraceLine(vecSpot1, vecSpot2, ignore_monsters, ENT(pev), &tr);
	
	if (tr.fInOpen && tr.fInWater)
		return false;                   // sight line crossed contents

	if (tr.flFraction == 1)
		return true;

	return false;
}


