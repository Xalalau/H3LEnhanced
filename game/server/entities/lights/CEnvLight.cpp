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

#include "CEnvLight.h"

LINK_ENTITY_TO_CLASS( light_environment, CEnvLight );

void CEnvLight::KeyValue( KeyValueData* pkvd )
{
	if( FStrEq( pkvd->szKeyName, "_light" ) )
	{
		int r, g, b, v, j;
		char szColor[ 64 ];
		j = sscanf( pkvd->szValue, "%d %d %d %d\n", &r, &g, &b, &v );
		if( j == 1 )
		{
			g = b = r;
		}
		else if( j == 4 )
		{
			r = r * ( v / 255.0 );
			g = g * ( v / 255.0 );
			b = b * ( v / 255.0 );
		}

		// simulate qrad direct, ambient,and gamma adjustments, as well as engine scaling
		r = pow( r / 114.0, 0.6 ) * 264;
		g = pow( g / 114.0, 0.6 ) * 264;
		b = pow( b / 114.0, 0.6 ) * 264;

		pkvd->fHandled = true;
		sprintf( szColor, "%d", r );
		CVAR_SET_STRING( "sv_skycolor_r", szColor );
		sprintf( szColor, "%d", g );
		CVAR_SET_STRING( "sv_skycolor_g", szColor );
		sprintf( szColor, "%d", b );
		CVAR_SET_STRING( "sv_skycolor_b", szColor );
	}
	else
	{
		CLight::KeyValue( pkvd );
	}
}

void CEnvLight::Spawn( void )
{
	char szVector[ 64 ];
	UTIL_MakeAimVectors( GetAbsAngles() );

	sprintf( szVector, "%f", gpGlobals->v_forward.x );
	CVAR_SET_STRING( "sv_skyvec_x", szVector );
	sprintf( szVector, "%f", gpGlobals->v_forward.y );
	CVAR_SET_STRING( "sv_skyvec_y", szVector );
	sprintf( szVector, "%f", gpGlobals->v_forward.z );
	CVAR_SET_STRING( "sv_skyvec_z", szVector );

	CLight::Spawn();
}