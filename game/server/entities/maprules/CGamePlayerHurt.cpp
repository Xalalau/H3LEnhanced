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

#include "CGamePlayerHurt.h"

LINK_ENTITY_TO_CLASS( game_player_hurt, CGamePlayerHurt );

void CGamePlayerHurt::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( !CanFireForActivator( pActivator ) )
		return;

	if( pActivator->IsPlayer() )
	{
		if( GetDamage() < 0 )
			pActivator->GiveHealth( -GetDamage(), DMG_GENERIC );
		else
			pActivator->TakeDamage( this, this, GetDamage(), DMG_GENERIC );
	}

	SUB_UseTargets( pActivator, useType, value );

	if( RemoveOnFire() )
	{
		UTIL_Remove( this );
	}
}