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
#include "Skill.h"
#include "Decals.h"
#include "cbase.h"
#include "Weapons.h"

#include "TempEntity.h"

#include "CBMortar.h"

extern int gSpitSprite;

BEGIN_DATADESC( CBMortar )
	DEFINE_FIELD( m_maxFrame, FIELD_INTEGER ),
	DEFINE_THINKFUNC( Animate ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( bmortar, CBMortar );

// UNDONE: right now this is pretty much a copy of the squid spit with minor changes to the way it does damage
void CBMortar::Spawn( void )
{
	SetMoveType( MOVETYPE_TOSS );

	SetSolidType( SOLID_BBOX );
	SetRenderMode( kRenderTransAlpha );
	SetRenderAmount( 255 );

	SetModel( "sprites/mommaspit.spr" );
	SetFrame( 0 );
	SetScale( 0.5 );

	SetSize( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );

	m_maxFrame = ( float ) MODEL_FRAMES( GetModelIndex() ) - 1;
	SetDamageTime( gpGlobals->time + 0.4 );
}

CBMortar *CBMortar::Shoot( CBaseEntity* pOwner, Vector vecStart, Vector vecVelocity )
{
	auto pSpit = static_cast<CBMortar*>( UTIL_CreateNamedEntity( "bmortar" ) );
	pSpit->Spawn();

	pSpit->SetAbsOrigin( vecStart );
	pSpit->SetAbsVelocity( vecVelocity );
	pSpit->SetOwner( pOwner );
	pSpit->SetScale( 2.5 );
	pSpit->SetThink( &CBMortar::Animate );
	pSpit->SetNextThink( gpGlobals->time + 0.1 );

	return pSpit;
}

void CBMortar::Touch( CBaseEntity *pOther )
{
	TraceResult tr;
	int		iPitch;

	// splat sound
	iPitch = RANDOM_FLOAT( 90, 110 );

	EMIT_SOUND_DYN( this, CHAN_VOICE, "bullchicken/bc_acid1.wav", 1, ATTN_NORM, 0, iPitch );

	switch( RANDOM_LONG( 0, 1 ) )
	{
	case 0:
		EMIT_SOUND_DYN( this, CHAN_WEAPON, "bullchicken/bc_spithit1.wav", 1, ATTN_NORM, 0, iPitch );
		break;
	case 1:
		EMIT_SOUND_DYN( this, CHAN_WEAPON, "bullchicken/bc_spithit2.wav", 1, ATTN_NORM, 0, iPitch );
		break;
	}

	if( pOther->IsBSPModel() )
	{

		// make a splat on the wall
		UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() + GetAbsVelocity() * 10, dont_ignore_monsters, ENT( pev ), &tr );
		UTIL_DecalTrace( &tr, DECAL_MOMMASPLAT );
	}
	else
	{
		tr.vecEndPos = GetAbsOrigin();
		tr.vecPlaneNormal = -1 * GetAbsVelocity().Normalize();
	}
	// make some flecks
	SpriteSpray( tr.vecEndPos, tr.vecPlaneNormal, gSpitSprite, 24 );

	CBaseEntity* pOwner = GetOwner();

	RadiusDamage( GetAbsOrigin(), CTakeDamageInfo( this, pOwner, gSkillData.GetBigMommaDmgBlast(), DMG_ACID ), gSkillData.GetBigMommaRadiusBlast(), EntityClassifications().GetNoneId() );
	UTIL_Remove( this );
}

void CBMortar::Animate( void )
{
	SetNextThink( gpGlobals->time + 0.1 );

	if( gpGlobals->time > GetDamageTime() )
	{
		SetDamageTime( gpGlobals->time + 0.2 );
		SpriteSpray( GetAbsOrigin(), -GetAbsVelocity().Normalize(), gSpitSprite, 3 );
	}

	const bool bWasAnimated = GetFrame() != 0;
	SetFrame( GetFrame() + 1 );
	if( bWasAnimated )
	{
		if( GetFrame() > m_maxFrame )
		{
			SetFrame( 0 );
		}
	}
}