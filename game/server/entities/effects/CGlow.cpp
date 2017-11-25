#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CGlow.h"

BEGIN_DATADESC( CGlow )
	DEFINE_FIELD( m_lastTime, FIELD_TIME ),
	DEFINE_FIELD( m_maxFrame, FIELD_FLOAT ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( env_glow, CGlow );

//TODO: is this entity any different from CSprite? - Solokiller

void CGlow::Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	GetEffects().ClearAll();
	SetFrame( 0 );

	PRECACHE_MODEL( GetModelName() );
	SetModel( GetModelName() );

	m_maxFrame = ( float ) MODEL_FRAMES( GetModelIndex() ) - 1;
	if( m_maxFrame > 1.0 && GetFrameRate() != 0 )
		SetNextThink( gpGlobals->time + 0.1 );

	m_lastTime = gpGlobals->time;
}


void CGlow::Think( void )
{
	Animate( GetFrameRate() * ( gpGlobals->time - m_lastTime ) );

	SetNextThink( gpGlobals->time + 0.1 );
	m_lastTime = gpGlobals->time;
}


void CGlow::Animate( float frames )
{
	if( m_maxFrame > 0 )
		SetFrame( fmod( GetFrame() + frames, m_maxFrame ) );
}