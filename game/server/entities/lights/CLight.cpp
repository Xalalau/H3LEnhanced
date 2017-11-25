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

===== lights.cpp ========================================================

  spawn and think functions for editor-placed lights

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CLight.h"

BEGIN_DATADESC( CLight )
	DEFINE_FIELD( m_iStyle, FIELD_INTEGER ),
	DEFINE_FIELD( m_iszPattern, FIELD_STRING ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( light, CLight );

//
// shut up spawn functions for new spotlights
//
LINK_ENTITY_TO_CLASS( light_spot, CLight );

//
// Cache user-entity-field values until spawn is called.
//
void CLight :: KeyValue( KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "style"))
	{
		m_iStyle = atoi(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "pitch"))
	{
		Vector vecAngles = GetAbsAngles();
		vecAngles.x = atof(pkvd->szValue);
		SetAbsAngles( vecAngles );
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "pattern"))
	{
		m_iszPattern = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
	{
		CPointEntity::KeyValue( pkvd );
	}
}

void CLight :: Spawn( void )
{
	if ( !HasTargetname() )
	{       // inert light
		UTIL_RemoveNow( this );
		return;
	}
	
	if (m_iStyle >= 32)
	{
		if ( GetSpawnFlags().Any( SF_LIGHT_START_OFF ) )
			LIGHT_STYLE(m_iStyle, "a");
		else if (m_iszPattern)
			LIGHT_STYLE(m_iStyle, (char *)STRING( m_iszPattern ));
		else
			LIGHT_STYLE(m_iStyle, "m");
	}
}


void CLight :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (m_iStyle >= 32)
	{
		if ( !ShouldToggle( useType, !GetSpawnFlags().Any( SF_LIGHT_START_OFF ) ) )
			return;

		if( GetSpawnFlags().Any( SF_LIGHT_START_OFF ) )
		{
			if (m_iszPattern)
				LIGHT_STYLE(m_iStyle, (char *)STRING( m_iszPattern ));
			else
				LIGHT_STYLE(m_iStyle, "m");
			GetSpawnFlags().ClearFlags( SF_LIGHT_START_OFF );
		}
		else
		{
			LIGHT_STYLE(m_iStyle, "a");
			GetSpawnFlags().AddFlags( SF_LIGHT_START_OFF );
		}
	}
}