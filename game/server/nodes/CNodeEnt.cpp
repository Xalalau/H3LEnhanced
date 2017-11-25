/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CTestHull.h"
#include "CGraph.h"

#include "CNodeEnt.h"

LINK_ENTITY_TO_CLASS( info_node, CNodeEnt );
LINK_ENTITY_TO_CLASS( info_node_air, CNodeEnt );

void CNodeEnt::Spawn( void )
{
	SetMoveType( MOVETYPE_NONE );
	SetSolidType( SOLID_NOT );// always solid_not 

	if( WorldGraph.m_fGraphPresent )
	{
		// graph loaded from disk, so discard all these node ents as soon as they spawn
		UTIL_RemoveNow( this );
		return;
	}

	if( WorldGraph.m_cNodes >= MAX_NODES )
	{
		ALERT( at_aiconsole, "cNodes > MAX_NODES\n" );
		return;
	}

	WorldGraph.m_pNodes[ WorldGraph.m_cNodes ].m_vecOriginPeek =
		WorldGraph.m_pNodes[ WorldGraph.m_cNodes ].m_vecOrigin = GetAbsOrigin();
	WorldGraph.m_pNodes[ WorldGraph.m_cNodes ].m_flHintYaw = GetAbsAngles().y;
	WorldGraph.m_pNodes[ WorldGraph.m_cNodes ].m_sHintType = m_sHintType;
	WorldGraph.m_pNodes[ WorldGraph.m_cNodes ].m_sHintActivity = m_sHintActivity;

	if( ClassnameIs( "info_node_air" ) )
		WorldGraph.m_pNodes[ WorldGraph.m_cNodes ].m_afNodeInfo = bits_NODE_AIR;
	else
		WorldGraph.m_pNodes[ WorldGraph.m_cNodes ].m_afNodeInfo = 0;

	WorldGraph.m_cNodes++;

	UTIL_RemoveNow( this );
}

//=========================================================
// nodes start out as ents in the world. As they are spawned,
// the node info is recorded then the ents are discarded.
//=========================================================
void CNodeEnt::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "hinttype" ) )
	{
		m_sHintType = ( short ) atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}

	if( FStrEq( pkvd->szKeyName, "activity" ) )
	{
		m_sHintActivity = ( short ) atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}