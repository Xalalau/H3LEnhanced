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

#include "CCyclerSprite.h"

BEGIN_DATADESC( CCyclerSprite )
	DEFINE_FIELD( m_animate, FIELD_INTEGER ),
	DEFINE_FIELD( m_lastTime, FIELD_TIME ),
	DEFINE_FIELD( m_maxFrame, FIELD_FLOAT ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( cycler_sprite, CCyclerSprite );

void CCyclerSprite::Spawn( void )
{
	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_NONE );
	SetTakeDamageMode( DAMAGE_YES );
	GetEffects().ClearAll();

	SetFrame( 0 );
	SetNextThink( gpGlobals->time + 0.1 );
	m_animate = 1;
	m_lastTime = gpGlobals->time;

	PRECACHE_MODEL( GetModelName() );
	SetModel( GetModelName() );

	m_maxFrame = ( float ) MODEL_FRAMES( GetModelIndex() ) - 1;
}

void CCyclerSprite::Think( void )
{
	if( ShouldAnimate() )
		Animate( GetFrameRate() * ( gpGlobals->time - m_lastTime ) );

	SetNextThink( gpGlobals->time + 0.1 );
	m_lastTime = gpGlobals->time;
}

void CCyclerSprite::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_animate = !m_animate;
	ALERT( at_console, "Sprite: %s\n", GetModelName() );
}

void CCyclerSprite::OnTakeDamage( const CTakeDamageInfo& info )
{
	if( m_maxFrame > 1.0 )
	{
		Animate( 1.0 );
	}
}

void CCyclerSprite::Animate( float frames )
{
	SetFrame( GetFrame() + frames );
	if( m_maxFrame > 0 )
		SetFrame( fmod( GetFrame(), m_maxFrame ) );
}