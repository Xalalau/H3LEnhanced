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

#include "CFuncIllusionary.h"

LINK_ENTITY_TO_CLASS( func_illusionary, CFuncIllusionary );

void CFuncIllusionary::Spawn( void )
{
	SetAbsAngles( g_vecZero );
	SetMoveType( MOVETYPE_NONE );
	SetSolidType( SOLID_NOT );// always solid_not 
	SetModel( GetModelName() );

	// I'd rather eat the network bandwidth of this than figure out how to save/restore
	// these entities after they have been moved to the client, or respawn them ala Quake
	// Perhaps we can do this in deathmatch only.
	//	MAKE_STATIC(ENT(pev));
}

void CFuncIllusionary::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "skin" ) )//skin is used for content type
	{
		SetSkin( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}