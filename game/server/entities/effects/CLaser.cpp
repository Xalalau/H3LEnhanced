#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CBeam.h"
#include "CSprite.h"

#include "CLaser.h"

BEGIN_DATADESC( CLaser )
	DEFINE_FIELD( m_pSprite, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_iszSpriteName, FIELD_STRING ),
	DEFINE_FIELD( m_firePosition, FIELD_POSITION_VECTOR ),
	DEFINE_THINKFUNC( StrikeThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( env_laser, CLaser );

void CLaser::Spawn( void )
{
	if( !HasModel() )
	{
		SetThink( &CLaser::SUB_Remove );
		return;
	}
	SetSolidType( SOLID_NOT );							// Remove model & collisions
	Precache();

	SetThink( &CLaser::StrikeThink );
	GetFlags() |= FL_CUSTOMENTITY;

	PointsInit( GetAbsOrigin(), GetAbsOrigin() );

	if( !m_pSprite && m_iszSpriteName )
		m_pSprite = CSprite::SpriteCreate( STRING( m_iszSpriteName ), GetAbsOrigin(), true );
	else
		m_pSprite = NULL;

	if( m_pSprite )
		m_pSprite->SetTransparency( kRenderGlow, GetRenderColor().x, GetRenderColor().y, GetRenderColor().z, GetRenderAmount(), GetRenderFX() );

	if( HasTargetname() && !GetSpawnFlags().Any( SF_BEAM_STARTON ) )
		TurnOff();
	else
		TurnOn();
}

void CLaser::Precache( void )
{
	SetModelIndex( PRECACHE_MODEL( GetModelName() ) );
	if( m_iszSpriteName )
		PRECACHE_MODEL( ( char * ) STRING( m_iszSpriteName ) );
}

void CLaser::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "LaserTarget" ) )
	{
		SetMessage( ALLOC_STRING( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "width" ) )
	{
		SetWidth( ( int ) atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "NoiseAmplitude" ) )
	{
		SetNoise( atoi( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "TextureScroll" ) )
	{
		SetScrollRate( atoi( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "texture" ) )
	{
		SetModelName( ALLOC_STRING( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "EndSprite" ) )
	{
		m_iszSpriteName = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "framestart" ) )
	{
		SetFrame( atoi( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "damage" ) )
	{
		SetDamage( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CBeam::KeyValue( pkvd );
}

void CLaser::TurnOn( void )
{
	GetEffects().ClearFlags( EF_NODRAW );
	if( m_pSprite )
		m_pSprite->TurnOn();
	SetDamageTime( gpGlobals->time );
	SetNextThink( gpGlobals->time );
}

void CLaser::TurnOff( void )
{
	GetEffects() |= EF_NODRAW;
	SetNextThink( 0 );
	if( m_pSprite )
		m_pSprite->TurnOff();
}

bool CLaser::IsOn() const
{
	if( GetEffects().Any( EF_NODRAW ) )
		return false;
	return true;
}

void CLaser::FireAtPoint( TraceResult &tr )
{
	SetEndPos( tr.vecEndPos );
	if( m_pSprite )
		m_pSprite->SetAbsOrigin( tr.vecEndPos );

	BeamDamage( &tr );
	DoSparks( GetStartPos(), tr.vecEndPos );
}

void CLaser::StrikeThink( void )
{
	CBaseEntity *pEnd = UTIL_RandomTargetname( GetMessage() );

	if( pEnd )
		m_firePosition = pEnd->GetAbsOrigin();

	TraceResult tr;

	UTIL_TraceLine( GetAbsOrigin(), m_firePosition, dont_ignore_monsters, NULL, &tr );
	FireAtPoint( tr );
	SetNextThink( gpGlobals->time + 0.1 );
}

void CLaser::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	const bool active = IsOn();

	if( !ShouldToggle( useType, active ) )
		return;
	if( active )
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}