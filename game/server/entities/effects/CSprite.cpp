#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CSprite.h"

BEGIN_DATADESC( CSprite )
	DEFINE_FIELD( m_lastTime, FIELD_TIME ),
	DEFINE_FIELD( m_maxFrame, FIELD_FLOAT ),
	DEFINE_THINKFUNC( AnimateThink ),
	DEFINE_THINKFUNC( ExpandThink ),
	DEFINE_THINKFUNC( AnimateUntilDead ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( env_sprite, CSprite );

void CSprite::Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	GetEffects().ClearAll();
	SetFrame( 0 );

	Precache();
	SetModel( GetModelName() );

	m_maxFrame = ( float ) MODEL_FRAMES( GetModelIndex() ) - 1;
	if( HasTargetname() && !GetSpawnFlags().Any( SF_SPRITE_STARTON ) )
		TurnOff();
	else
		TurnOn();

	// Worldcraft only sets y rotation, copy to Z
	if( GetAbsAngles().y != 0 && GetAbsAngles().z == 0 )
	{
		Vector vecAngles = GetAbsAngles();
		vecAngles.z = vecAngles.y;
		vecAngles.y = 0;
		SetAbsAngles( vecAngles );
	}
}

void CSprite::Precache( void )
{
	PRECACHE_MODEL( GetModelName() );

	// Reset attachment after save/restore
	if( auto pAimEnt = GetAimEntity() )
		SetAttachment( pAimEnt, GetBody() );
	else
	{
		// Clear attachment
		SetSkin( 0 );
		SetBody( 0 );
	}
}

void CSprite::AnimateThink( void )
{
	Animate( GetFrameRate() * ( gpGlobals->time - m_lastTime ) );

	SetNextThink( gpGlobals->time + 0.1 );
	m_lastTime = gpGlobals->time;
}

void CSprite::ExpandThink( void )
{
	float frametime = gpGlobals->time - m_lastTime;
	SetScale( GetScale() + ( GetSpeed() * frametime ) );
	SetRenderAmount( GetRenderAmount() - ( GetHealth() * frametime ) );
	if( GetRenderAmount() <= 0 )
	{
		SetRenderAmount( 0 );
		UTIL_Remove( this );
	}
	else
	{
		SetNextThink( gpGlobals->time + 0.1 );
		m_lastTime = gpGlobals->time;
	}
}

void CSprite::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	//TODO: not going to work properly if any other flags are set - Solokiller
	const bool on = GetEffects().Get() != EF_NODRAW;
	if( ShouldToggle( useType, on ) )
	{
		if( on )
		{
			TurnOff();
		}
		else
		{
			TurnOn();
		}
	}
}

void CSprite::Animate( float frames )
{
	SetFrame( GetFrame() + frames );
	if( GetFrame() > m_maxFrame )
	{
		if( GetSpawnFlags().Any( SF_SPRITE_ONCE ) )
		{
			TurnOff();
		}
		else
		{
			if( m_maxFrame > 0 )
				SetFrame( fmod( GetFrame(), m_maxFrame ) );
		}
	}
}

void CSprite::Expand( float scaleSpeed, float fadeSpeed )
{
	SetSpeed( scaleSpeed );
	SetHealth( fadeSpeed );
	SetThink( &CSprite::ExpandThink );

	SetNextThink( gpGlobals->time );
	m_lastTime = gpGlobals->time;
}

void CSprite::SpriteInit( const char *pSpriteName, const Vector &origin )
{
	SetModelName( pSpriteName );
	SetAbsOrigin( origin );
	Spawn();
}

void CSprite::TurnOff( void )
{
	GetEffects() = EF_NODRAW;
	SetNextThink( 0 );
}

void CSprite::TurnOn( void )
{
	GetEffects().ClearAll();
	if( ( GetFrameRate() && m_maxFrame > 1.0 ) || GetSpawnFlags().Any( SF_SPRITE_ONCE ) )
	{
		SetThink( &CSprite::AnimateThink );
		SetNextThink( gpGlobals->time );
		m_lastTime = gpGlobals->time;
	}
	SetFrame( 0 );
}

void CSprite::AnimateUntilDead( void )
{
	if( gpGlobals->time > GetDamageTime() )
		UTIL_Remove( this );
	else
	{
		AnimateThink();
		SetNextThink( gpGlobals->time );
	}
}

CSprite *CSprite::SpriteCreate( const char *pSpriteName, const Vector &origin, const bool animate )
{
	auto pSprite = static_cast<CSprite*>( UTIL_CreateNamedEntity( "env_sprite" ) );
	pSprite->SpriteInit( pSpriteName, origin );
	pSprite->SetSolidType( SOLID_NOT );
	pSprite->SetMoveType( MOVETYPE_NOCLIP );
	if( animate )
		pSprite->TurnOn();

	return pSprite;
}