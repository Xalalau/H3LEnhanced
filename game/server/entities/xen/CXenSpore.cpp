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

#include "CXenSpore.h"

const char *CXenSpore::pModelNames[] =
{
	"models/fungus(small).mdl",
	"models/fungus.mdl",
	"models/fungus(large).mdl",
};

// I just eyeballed these -- fill in hulls for the legs
const Vector CXenSporeLarge::m_hullSizes[] =
{
	Vector( 90, -25, 0 ),
	Vector( 25, 75, 0 ),
	Vector( -15, -100, 0 ),
	Vector( -90, -35, 0 ),
	Vector( -90, 60, 0 ),
};

LINK_ENTITY_TO_CLASS( xen_spore_small, CXenSporeSmall );
LINK_ENTITY_TO_CLASS( xen_spore_medium, CXenSporeMed );
LINK_ENTITY_TO_CLASS( xen_spore_large, CXenSporeLarge );
LINK_ENTITY_TO_CLASS( xen_hull, CXenHull );

void CXenSpore::Spawn( void )
{
	Precache();

	SetModel( pModelNames[ GetSkin() ] );
	SetMoveType( MOVETYPE_NONE );
	SetSolidType( SOLID_BBOX );
	SetTakeDamageMode( DAMAGE_YES );

	//	SetActivity( ACT_IDLE );
	SetSequence( 0 );
	SetFrame( RANDOM_FLOAT( 0, 255 ) );
	SetFrameRate( RANDOM_FLOAT( 0.7, 1.4 ) );
	ResetSequenceInfo();
	SetNextThink( gpGlobals->time + RANDOM_FLOAT( 0.1, 0.4 ) );	// Load balance these a bit
}

void CXenSpore::Precache( void )
{
	PRECACHE_MODEL( ( char * ) pModelNames[ GetSkin() ] );
}

void CXenSpore::Touch( CBaseEntity *pOther )
{
}

void CXenSpore::Think( void )
{
	/*float flInterval = */StudioFrameAdvance();
	SetNextThink( gpGlobals->time + 0.1 );

#if 0
	DispatchAnimEvents( flInterval );

	switch( GetActivity() )
	{
	default:
	case ACT_IDLE:
		break;

	}
#endif
}

void CXenSporeSmall::Spawn( void )
{
	SetSkin( 0 );
	CXenSpore::Spawn();
	SetSize( Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
}

void CXenSporeMed::Spawn( void )
{
	SetSkin( 1 );
	CXenSpore::Spawn();
	SetSize( Vector( -40, -40, 0 ), Vector( 40, 40, 120 ) );
}

void CXenSporeLarge::Spawn( void )
{
	SetSkin( 2 );
	CXenSpore::Spawn();
	SetSize( Vector( -48, -48, 110 ), Vector( 48, 48, 240 ) );

	Vector forward, right;

	UTIL_MakeVectorsPrivate( GetAbsAngles(), &forward, &right, nullptr );

	// Rotate the leg hulls into position
	for( size_t i = 0; i < ARRAYSIZE( m_hullSizes ); i++ )
		CXenHull::CreateHull( this, Vector( -12, -12, 0 ), Vector( 12, 12, 120 ), ( m_hullSizes[ i ].x * forward ) + ( m_hullSizes[ i ].y * right ) );
}

CXenHull *CXenHull::CreateHull( CBaseEntity *source, const Vector &mins, const Vector &maxs, const Vector &offset )
{
	auto pHull = static_cast<CXenHull*>( UTIL_CreateNamedEntity( "xen_hull" ) );

	pHull->SetAbsOrigin( source->GetAbsOrigin() + offset );
	pHull->SetModel( source->GetModelName() );
	pHull->SetSolidType( SOLID_BBOX );
	pHull->SetMoveType( MOVETYPE_NONE );
	pHull->SetOwner( source );
	pHull->SetSize( mins, maxs );
	pHull->SetRenderAmount( 0 );
	pHull->SetRenderMode( kRenderTransTexture );
	//	pHull->GetEffects() = EF_NODRAW;

	return pHull;
}