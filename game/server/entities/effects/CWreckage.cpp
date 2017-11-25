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
#include "Weapons.h"

#include "CWreckage.h"

BEGIN_DATADESC( CWreckage )
	DEFINE_FIELD( m_flStartTime, FIELD_TIME ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( cycler_wreckage, CWreckage );

void CWreckage::Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	SetTakeDamageMode( DAMAGE_NO );
	GetEffects().ClearAll();

	SetFrame( 0 );
	SetNextThink( gpGlobals->time + 0.1 );

	if( HasModel() )
	{
		PRECACHE_MODEL( GetModelName() );
		SetModel( GetModelName() );
	}
	// SetScale( 5.0 );

	m_flStartTime = gpGlobals->time;
}

void CWreckage::Precache()
{
	if( HasModel() )
		PRECACHE_MODEL( GetModelName() );
}

void CWreckage::Think( void )
{
	StudioFrameAdvance();
	SetNextThink( gpGlobals->time + 0.2 );

	if( GetDamageTime() )
	{
		if( GetDamageTime() < gpGlobals->time )
		{
			UTIL_Remove( this );
			return;
		}
		else if( RANDOM_FLOAT( 0, GetDamageTime() - m_flStartTime ) > GetDamageTime() - gpGlobals->time )
		{
			return;
		}
	}

	Vector VecSrc;

	VecSrc.x = RANDOM_FLOAT( GetAbsMin().x, GetAbsMax().x );
	VecSrc.y = RANDOM_FLOAT( GetAbsMin().y, GetAbsMax().y );
	VecSrc.z = RANDOM_FLOAT( GetAbsMin().z, GetAbsMax().z );

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, VecSrc );
	WRITE_BYTE( TE_SMOKE );
	WRITE_COORD( VecSrc.x );
	WRITE_COORD( VecSrc.y );
	WRITE_COORD( VecSrc.z );
	WRITE_SHORT( g_sModelIndexSmoke );
	WRITE_BYTE( RANDOM_LONG( 0, 49 ) + 50 ); // scale * 10
	WRITE_BYTE( RANDOM_LONG( 0, 3 ) + 8 ); // framerate
	MESSAGE_END();
}