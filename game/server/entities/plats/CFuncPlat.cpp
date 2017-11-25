#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CPlatTrigger.h"

#include "CFuncPlat.h"

BEGIN_DATADESC( CFuncPlat )
	DEFINE_USEFUNC( PlatUse ),
	DEFINE_THINKFUNC( CallGoDown ),
	DEFINE_THINKFUNC( CallHitTop ),
	DEFINE_THINKFUNC( CallHitBottom ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_plat, CFuncPlat );

void CFuncPlat::Spawn()
{
	Setup();

	Precache();

	// If this platform is the target of some button, it starts at the TOP position,
	// and is brought down by that button.  Otherwise, it starts at BOTTOM.
	if( HasTargetname() )
	{
		SetAbsOrigin( m_vecPosition1 );
		m_toggle_state = TS_AT_TOP;
		SetUse( &CFuncPlat::PlatUse );
	}
	else
	{
		SetAbsOrigin( m_vecPosition2 );
		m_toggle_state = TS_AT_BOTTOM;
	}
}

void CFuncPlat::Precache()
{
	CBasePlatTrain::Precache();
	//PRECACHE_SOUND("plats/platmove1.wav");
	//PRECACHE_SOUND("plats/platstop1.wav");
	//TODO: why is this in precache? PrecacheOther could create useless entities because of this... - Solokiller
	if( !IsTogglePlat() )
		CPlatTrigger::PlatSpawnInsideTrigger( this );		// the "start moving" trigger
}

void CFuncPlat::Setup( void )
{
	//pev->noiseMovement = MAKE_STRING("plats/platmove1.wav");
	//pev->noiseStopMoving = MAKE_STRING("plats/platstop1.wav");

	if( m_flTLength == 0 )
		m_flTLength = 80;
	if( m_flTWidth == 0 )
		m_flTWidth = 10;

	SetAbsAngles( g_vecZero );

	SetSolidType( SOLID_BSP );
	SetMoveType( MOVETYPE_PUSH );

	SetAbsOrigin( GetAbsOrigin() );		// set size and link into world
	SetSize( GetRelMin(), GetRelMax() );
	SetModel( GetModelName() );

	// vecPosition1 is the top position, vecPosition2 is the bottom
	m_vecPosition1 = GetAbsOrigin();
	m_vecPosition2 = GetAbsOrigin();
	if( m_flHeight != 0 )
		m_vecPosition2.z = GetAbsOrigin().z - m_flHeight;
	else
		m_vecPosition2.z = GetAbsOrigin().z - GetBounds().z + 8;
	if( GetSpeed() == 0 )
		SetSpeed( 150 );

	if( m_volume == 0 )
		m_volume = 0.85;
}

void CFuncPlat::Blocked( CBaseEntity *pOther )
{
	ALERT( at_aiconsole, "%s Blocked by %s\n", GetClassname(), pOther->GetClassname() );
	// Hurt the blocker a little
	pOther->TakeDamage( this, this, 1, DMG_CRUSH );

	if( pev->noiseMovement )
		STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ) );

	// Send the platform back where it came from
	ASSERT( m_toggle_state == TS_GOING_UP || m_toggle_state == TS_GOING_DOWN );
	if( m_toggle_state == TS_GOING_UP )
		GoDown();
	else if( m_toggle_state == TS_GOING_DOWN )
		GoUp();
}

//
// Used by SUB_UseTargets, when a platform is the target of a button.
// Start bringing platform down.
//
void CFuncPlat::PlatUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( IsTogglePlat() )
	{
		// Top is off, bottom is on
		const bool on = m_toggle_state == TS_AT_BOTTOM;

		if( !ShouldToggle( useType, on ) )
			return;

		if( m_toggle_state == TS_AT_TOP )
			GoDown();
		else if( m_toggle_state == TS_AT_BOTTOM )
			GoUp();
	}
	else
	{
		SetUse( NULL );

		if( m_toggle_state == TS_AT_TOP )
			GoDown();
	}
}

//
// Platform is at bottom, now starts moving up
//
void CFuncPlat::GoUp( void )
{
	if( pev->noiseMovement )
		EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ), m_volume, ATTN_NORM );

	ASSERT( m_toggle_state == TS_AT_BOTTOM || m_toggle_state == TS_GOING_DOWN );
	m_toggle_state = TS_GOING_UP;
	SetMoveDone( &CFuncPlat::CallHitTop );
	LinearMove( m_vecPosition1, GetSpeed() );
}

//
// Platform is at top, now starts moving down.
//
void CFuncPlat::GoDown( void )
{
	if( pev->noiseMovement )
		EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ), m_volume, ATTN_NORM );

	ASSERT( m_toggle_state == TS_AT_TOP || m_toggle_state == TS_GOING_UP );
	m_toggle_state = TS_GOING_DOWN;
	SetMoveDone( &CFuncPlat::CallHitBottom );
	LinearMove( m_vecPosition2, GetSpeed() );
}

//
// Platform has hit top.  Pauses, then starts back down again.
//
void CFuncPlat::HitTop( void )
{
	if( pev->noiseMovement )
		STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ) );

	if( pev->noiseStopMoving )
		EMIT_SOUND( this, CHAN_WEAPON, ( char* ) STRING( pev->noiseStopMoving ), m_volume, ATTN_NORM );

	ASSERT( m_toggle_state == TS_GOING_UP );
	m_toggle_state = TS_AT_TOP;

	if( !IsTogglePlat() )
	{
		// After a delay, the platform will automatically start going down again.
		SetThink( &CFuncPlat::CallGoDown );
		SetNextThink( GetLastThink() + 3 );
	}
}

//
// Platform has hit bottom.  Stops and waits forever.
//
void CFuncPlat::HitBottom( void )
{
	if( pev->noiseMovement )
		STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ) );

	if( pev->noiseStopMoving )
		EMIT_SOUND( this, CHAN_WEAPON, ( char* ) STRING( pev->noiseStopMoving ), m_volume, ATTN_NORM );

	ASSERT( m_toggle_state == TS_GOING_DOWN );
	m_toggle_state = TS_AT_BOTTOM;
}