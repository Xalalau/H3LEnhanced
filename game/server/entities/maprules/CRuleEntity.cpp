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
#include "eiface.h"
#include "util.h"
#include "gamerules/GameRules.h"
#include "cbase.h"
#include "CBasePlayer.h"

#include "CRuleEntity.h"

BEGIN_DATADESC(	CRuleEntity )
	DEFINE_FIELD( m_iszMaster, FIELD_STRING),
END_DATADESC()

void CRuleEntity::Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	GetEffects() = EF_NODRAW;
}

void CRuleEntity::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "master"))
	{
		SetMaster( ALLOC_STRING(pkvd->szValue) );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

bool CRuleEntity::CanFireForActivator( CBaseEntity *pActivator ) const
{
	if ( m_iszMaster )
	{
		return UTIL_IsMasterTriggered( m_iszMaster, pActivator );
	}
	
	return true;
}