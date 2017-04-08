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
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = 0;
	pev->frame = 0;

	Precache();
	SetModel( STRING( pev->model ) );

	m_maxFrame = ( float ) MODEL_FRAMES( pev->modelindex ) - 1;
	if( HasTargetname() && !( pev->spawnflags & SF_SPRITE_STARTON ) )
		TurnOff();
	else
		TurnOn();

	// Worldcraft only sets y rotation, copy to Z
	if( pev->angles.y != 0 && pev->angles.z == 0 )
	{
		pev->angles.z = pev->angles.y;
		pev->angles.y = 0;
	}
}

void CSprite::Precache( void )
{
	PRECACHE_MODEL( ( char * ) STRING( pev->model ) );

	// Reset attachment after save/restore
	if( pev->aiment )
		SetAttachment( pev->aiment, pev->body );
	else
	{
		// Clear attachment
		pev->skin = 0;
		pev->body = 0;
	}
}

void CSprite::AnimateThink( void )
{
	Animate( pev->framerate * ( gpGlobals->time - m_lastTime ) );

	pev->nextthink = gpGlobals->time + 0.1;
	m_lastTime = gpGlobals->time;
}

void CSprite::ExpandThink( void )
{
	float frametime = gpGlobals->time - m_lastTime;
	pev->scale += pev->speed * frametime;
	pev->renderamt -= pev->health * frametime;
	if( pev->renderamt <= 0 )
	{
		pev->renderamt = 0;
		UTIL_Remove( this );
	}
	else
	{
		pev->nextthink = gpGlobals->time + 0.1;
		m_lastTime = gpGlobals->time;
	}
}

void CSprite::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	const bool on = pev->effects != EF_NODRAW;
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
	pev->frame += frames;
	if( pev->frame > m_maxFrame )
	{
		if( pev->spawnflags & SF_SPRITE_ONCE )
		{
			TurnOff();
		}
		else
		{
			if( m_maxFrame > 0 )
				pev->frame = fmod( pev->frame, m_maxFrame );
		}
	}
}

void CSprite::Expand( float scaleSpeed, float fadeSpeed )
{
	pev->speed = scaleSpeed;
	pev->health = fadeSpeed;
	SetThink( &CSprite::ExpandThink );

	pev->nextthink = gpGlobals->time;
	m_lastTime = gpGlobals->time;
}

void CSprite::SpriteInit( const char *pSpriteName, const Vector &origin )
{
	pev->model = MAKE_STRING( pSpriteName );
	pev->origin = origin;
	Spawn();
}

void CSprite::TurnOff( void )
{
	pev->effects = EF_NODRAW;
	pev->nextthink = 0;
}

void CSprite::TurnOn( void )
{
	pev->effects = 0;
	if( ( pev->framerate && m_maxFrame > 1.0 ) || ( pev->spawnflags & SF_SPRITE_ONCE ) )
	{
		SetThink( &CSprite::AnimateThink );
		pev->nextthink = gpGlobals->time;
		m_lastTime = gpGlobals->time;
	}
	pev->frame = 0;
}

void CSprite::AnimateUntilDead( void )
{
	if( gpGlobals->time > pev->dmgtime )
		UTIL_Remove( this );
	else
	{
		AnimateThink();
		pev->nextthink = gpGlobals->time;
	}
}

CSprite *CSprite::SpriteCreate( const char *pSpriteName, const Vector &origin, const bool animate )
{
	auto pSprite = static_cast<CSprite*>( UTIL_CreateNamedEntity( "env_sprite" ) );
	pSprite->SpriteInit( pSpriteName, origin );
	pSprite->pev->solid = SOLID_NOT;
	pSprite->pev->movetype = MOVETYPE_NOCLIP;
	if( animate )
		pSprite->TurnOn();

	return pSprite;
}