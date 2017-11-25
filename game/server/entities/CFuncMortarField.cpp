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
#include "SaveRestore.h"
#include "Weapons.h"
#include "Decals.h"
#include "entities/CSoundEnt.h"

#include "CFuncMortarField.h"

BEGIN_DATADESC(	CFuncMortarField )
	DEFINE_FIELD( m_iszXController, FIELD_STRING ),
	DEFINE_FIELD( m_iszYController, FIELD_STRING ),
	DEFINE_FIELD( m_flSpread, FIELD_FLOAT ),
	DEFINE_FIELD( m_flDelay, FIELD_FLOAT ),
	DEFINE_FIELD( m_iCount, FIELD_INTEGER ),
	DEFINE_FIELD( m_fControl, FIELD_INTEGER ),

	DEFINE_USEFUNC( FieldUse ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_mortar_field, CFuncMortarField );

void CFuncMortarField :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "m_iszXController"))
	{
		m_iszXController = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iszYController"))
	{
		m_iszYController = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flSpread"))
	{
		m_flSpread = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "m_fControl"))
	{
		m_fControl = atoi(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iCount"))
	{
		m_iCount = atoi(pkvd->szValue);
		pkvd->fHandled = true;
	}
}


// Drop bombs from above
void CFuncMortarField :: Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetModel( GetModelName() );    // set size and link into world
	SetMoveType( MOVETYPE_NONE );
	GetEffects() |= EF_NODRAW;
	SetUse( & CFuncMortarField::FieldUse );
	Precache();
}


void CFuncMortarField :: Precache( void )
{
	PRECACHE_SOUND ("weapons/mortar.wav");
	PRECACHE_SOUND ("weapons/mortarhit.wav");
	PRECACHE_MODEL( "sprites/lgtning.spr" );
}


// If connected to a table, then use the table controllers, else hit where the trigger is.
void CFuncMortarField :: FieldUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	Vector vecStart;

	vecStart.x = RANDOM_FLOAT( GetRelMin().x, GetRelMax().x );
	vecStart.y = RANDOM_FLOAT( GetRelMin().y, GetRelMax().y );
	vecStart.z = GetRelMax().z;

	switch( m_fControl )
	{
	case 0:	// random
		break;
	case 1: // Trigger Activator
		if (pActivator != NULL)
		{
			vecStart.x = pActivator->GetAbsOrigin().x;
			vecStart.y = pActivator->GetAbsOrigin().y;
		}
		break;
	case 2: // table
		{
			CBaseEntity *pController;

			if (!FStringNull(m_iszXController))
			{
				pController = UTIL_FindEntityByTargetname( NULL, STRING(m_iszXController));
				if (pController != NULL)
				{
					vecStart.x = GetRelMin().x + pController->GetIdealYaw() * ( GetBounds().x);
				}
			}
			if (!FStringNull(m_iszYController))
			{
				pController = UTIL_FindEntityByTargetname( NULL, STRING(m_iszYController));
				if (pController != NULL)
				{
					vecStart.y = GetRelMin().y + pController->GetIdealYaw() * ( GetBounds().y);
				}
			}
		}
		break;
	}

	int pitch = RANDOM_LONG(95,124);

	EMIT_SOUND_DYN( this, CHAN_VOICE, "weapons/mortar.wav", 1.0, ATTN_NONE, 0, pitch);

	float t = 2.5;
	for (int i = 0; i < m_iCount; i++)
	{
		Vector vecSpot = vecStart;
		vecSpot.x += RANDOM_FLOAT( -m_flSpread, m_flSpread );
		vecSpot.y += RANDOM_FLOAT( -m_flSpread, m_flSpread );

		TraceResult tr;
		UTIL_TraceLine( vecSpot, vecSpot + Vector( 0, 0, -1 ) * 4096, ignore_monsters, ENT(pev), &tr );

		edict_t *pentOwner = NULL;
		if (pActivator)	pentOwner = pActivator->edict();

		CBaseEntity *pMortar = Create("monster_mortar", tr.vecEndPos, Vector( 0, 0, 0 ), pentOwner );
		pMortar->SetNextThink( gpGlobals->time + t );
		t += RANDOM_FLOAT( 0.2, 0.5 );

		if (i == 0)
			CSoundEnt::InsertSound ( bits_SOUND_DANGER, tr.vecEndPos, 400, 0.3 );
	}
}