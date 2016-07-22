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
	string_t istrClassname = MAKE_STRING( "bodyque" );

	g_pBodyQueueHead = CBaseEntity::Instance( CREATE_NAMED_ENTITY( istrClassname ) );
	entvars_t* pev = VARS( g_pBodyQueueHead );

	// Reserve 3 more slots for dead bodies
	for( int i = 0; i < 3; i++ )
	{
		pev->owner = CREATE_NAMED_ENTITY( istrClassname );
		pev = VARS( pev->owner );
	}

	pev->owner = g_pBodyQueueHead->edict();
}

//
// make a body que entry for the given ent so the ent can be respawned elsewhere
//
// GLOBALS ASSUMED SET:  g_eoBodyQueueHead
//
void CopyToBodyQue( CBaseEntity* pEntity )
{
	if( pEntity->pev->effects & EF_NODRAW )
		return;

	entvars_t *pevHead = VARS( g_pBodyQueueHead );

	pevHead->angles = pEntity->pev->angles;
	pevHead->model = pEntity->pev->model;
	pevHead->modelindex = pEntity->pev->modelindex;
	pevHead->frame = pEntity->pev->frame;
	pevHead->colormap = pEntity->pev->colormap;
	pevHead->movetype = MOVETYPE_TOSS;
	pevHead->velocity = pEntity->pev->velocity;
	pevHead->flags = 0;
	pevHead->deadflag = pEntity->pev->deadflag;
	pevHead->renderfx = kRenderFxDeadPlayer;
	pevHead->renderamt = pEntity->entindex();

	pevHead->effects = pEntity->pev->effects | EF_NOINTERP;
	//pevHead->goalstarttime = pEntity->pev->goalstarttime;
	//pevHead->goalframe	= pEntity->pev->goalframe;
	//pevHead->goalendtime = pEntity->pev->goalendtime ;

	pevHead->sequence = pEntity->pev->sequence;
	pevHead->animtime = pEntity->pev->animtime;

	g_pBodyQueueHead->SetAbsOrigin( pEntity->GetAbsOrigin() );
	g_pBodyQueueHead->SetSize( pEntity->pev->mins, pEntity->pev->maxs );
	g_pBodyQueueHead = CBaseEntity::Instance( pevHead->owner );
}