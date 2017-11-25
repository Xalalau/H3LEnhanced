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

#include "CCorpse.h"

LINK_ENTITY_TO_CLASS( bodyque, CCorpse );

//Now inits to null in case of problems - Solokiller
DLL_GLOBAL CBaseEntity* g_pBodyQueueHead = nullptr;

void InitBodyQue()
{
	const char* const pszClassname = "bodyque";

	g_pBodyQueueHead = UTIL_CreateNamedEntity( pszClassname );

	ASSERT( g_pBodyQueueHead );

	CBaseEntity* pEnt = g_pBodyQueueHead;

	// Reserve 3 more slots for dead bodies
	for( int i = 0; i < 3; i++ )
	{
		pEnt->SetOwner( UTIL_CreateNamedEntity( pszClassname ) );
		pEnt = pEnt->GetOwner();

		ASSERT( pEnt );
	}

	pEnt->SetOwner( g_pBodyQueueHead );
}

//
// make a body que entry for the given ent so the ent can be respawned elsewhere
//
// GLOBALS ASSUMED SET:  g_eoBodyQueueHead
//
void CopyToBodyQue( CBaseEntity* pEntity )
{
	if( pEntity->GetEffects().Any( EF_NODRAW ) )
		return;

	g_pBodyQueueHead->SetAbsAngles( pEntity->GetAbsAngles() );
	g_pBodyQueueHead->SetModelName( pEntity->GetModelName() );
	g_pBodyQueueHead->SetModelIndex( pEntity->GetModelIndex() );
	g_pBodyQueueHead->SetFrame( pEntity->GetFrame() );
	g_pBodyQueueHead->SetColorMap( pEntity->GetColorMap() );
	g_pBodyQueueHead->SetMoveType( MOVETYPE_TOSS );
	g_pBodyQueueHead->SetAbsVelocity( pEntity->GetAbsVelocity() );
	g_pBodyQueueHead->GetFlags().ClearAll();
	g_pBodyQueueHead->SetDeadFlag( pEntity->GetDeadFlag() );
	g_pBodyQueueHead->SetRenderFX( kRenderFxDeadPlayer );
	g_pBodyQueueHead->SetRenderAmount( pEntity->entindex() );
	g_pBodyQueueHead->GetEffects() |= EF_NOINTERP;

	g_pBodyQueueHead->SetSequence( pEntity->GetSequence() );
	g_pBodyQueueHead->SetAnimTime( pEntity->GetAnimTime() );

	g_pBodyQueueHead->SetAbsOrigin( pEntity->GetAbsOrigin() );
	g_pBodyQueueHead->SetSize( pEntity->GetRelMin(), pEntity->GetRelMax() );
	g_pBodyQueueHead = g_pBodyQueueHead->GetOwner();
}