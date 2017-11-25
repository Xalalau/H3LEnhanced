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

#include "CInfoBM.h"

BEGIN_DATADESC( CInfoBM )
	DEFINE_FIELD( m_preSequence, FIELD_STRING ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( info_bigmomma, CInfoBM );

void CInfoBM::Spawn( void )
{
}

void CInfoBM::KeyValue( KeyValueData* pkvd )
{
	if( FStrEq( pkvd->szKeyName, "radius" ) )
	{
		SetScale( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "reachdelay" ) )
	{
		SetSpeed( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "reachtarget" ) )
	{
		SetMessage( ALLOC_STRING( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "reachsequence" ) )
	{
		SetNetName( ALLOC_STRING( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "presequence" ) )
	{
		m_preSequence = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue( pkvd );
}