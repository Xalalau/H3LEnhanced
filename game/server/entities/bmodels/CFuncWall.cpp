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

#include "CFuncWall.h"

LINK_ENTITY_TO_CLASS( func_wall, CFuncWall );

void CFuncWall::Spawn( void )
{
	SetAbsAngles( g_vecZero );
	SetMoveType( MOVETYPE_PUSH );  // so it doesn't get pushed by anything
	SetSolidType( SOLID_BSP );
	SetModel( GetModelName() );

	// If it can't move/go away, it's really part of the world
	GetFlags() |= FL_WORLDBRUSH;
}

void CFuncWall::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( ShouldToggle( useType, static_cast<int>( GetFrame() ) != 0 ) )
		SetFrame( 1 - GetFrame() );
}