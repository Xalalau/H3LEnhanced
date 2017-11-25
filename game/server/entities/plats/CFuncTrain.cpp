#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CPathCorner.h"

#include "CFuncTrain.h"

BEGIN_DATADESC( CFuncTrain )
	DEFINE_FIELD( m_sounds, FIELD_INTEGER ),
	DEFINE_FIELD( m_pevCurrentTarget, FIELD_EVARS ),
	DEFINE_FIELD( m_activated, FIELD_BOOLEAN ),

	DEFINE_THINKFUNC( Wait ),
	DEFINE_THINKFUNC( Next ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_train, CFuncTrain );

void CFuncTrain::Spawn( void )
{
	Precache();
	if( GetSpeed() == 0 )
		SetSpeed( 100 );

	if( !HasTarget() )
		ALERT( at_console, "FuncTrain with no target" );

	if( GetDamage() == 0 )
		SetDamage( 2 );

	SetMoveType( MOVETYPE_PUSH );

	//Remove tracktrain include when changed
	if( GetSpawnFlags().Any( SF_TRAIN_PASSABLE ) )
		SetSolidType( SOLID_NOT );
	else
		SetSolidType( SOLID_BSP );

	SetModel( GetModelName() );
	SetSize( GetRelMin(), GetRelMax() );
	SetAbsOrigin( GetAbsOrigin() );

	m_activated = false;

	if( m_volume == 0 )
		m_volume = 0.85;
}

void CFuncTrain::Precache( void )
{
	CBasePlatTrain::Precache();

#if 0  // obsolete
	// otherwise use preset sound
	switch( m_sounds )
	{
	case 0:
		pev->noise = 0;
		pev->noise1 = 0;
		break;

	case 1:
		PRECACHE_SOUND( "plats/train2.wav" );
		PRECACHE_SOUND( "plats/train1.wav" );
		pev->noise = MAKE_STRING( "plats/train2.wav" );
		pev->noise1 = MAKE_STRING( "plats/train1.wav" );
		break;

	case 2:
		PRECACHE_SOUND( "plats/platmove1.wav" );
		PRECACHE_SOUND( "plats/platstop1.wav" );
		pev->noise = MAKE_STRING( "plats/platstop1.wav" );
		pev->noise1 = MAKE_STRING( "plats/platmove1.wav" );
		break;
	}
#endif
}

void CFuncTrain::Activate( void )
{
	// Not yet active, so teleport to first target
	if( !m_activated )
	{
		m_activated = true;

		CBaseEntity* pTarg = UTIL_FindEntityByTargetname( nullptr, GetTarget() );

		//Use the world if no target was found. Replicates SDK behavior. - Solokiller
		if( !pTarg )
		{
			pTarg = CWorld::GetInstance();
		}

		SetTarget( pTarg->GetTarget() );
		//TODO change to EHANDLE - Solokiller
		m_pevCurrentTarget = pTarg->pev;// keep track of this since path corners change our target for us.

		SetAbsOrigin( pTarg->GetAbsOrigin() - ( GetRelMin() + GetRelMax() ) * 0.5 );

		if( !HasTargetname() )
		{	// not triggered, so start immediately
			SetNextThink( GetLastThink() + 0.1 );
			SetThink( &CFuncTrain::Next );
		}
		else
			GetSpawnFlags() |= SF_TRAIN_WAIT_RETRIGGER;
	}
}

void CFuncTrain::OverrideReset( void )
{
	// Are we moving?
	if( GetAbsVelocity() != g_vecZero && GetNextThink() != 0 )
	{
		SetTarget( GetMessage() );
		// now find our next target
		CBaseEntity* pTarg = GetNextTarget();
		if( !pTarg )
		{
			SetNextThink( 0 );
			SetAbsVelocity( g_vecZero );
		}
		else	// Keep moving for 0.1 secs, then find path_corner again and restart
		{
			SetThink( &CFuncTrain::Next );
			SetNextThink( GetLastThink() + 0.1 );
		}
	}
}

void CFuncTrain::Blocked( CBaseEntity *pOther )
{
	if( gpGlobals->time < m_flActivateFinished )
		return;

	m_flActivateFinished = gpGlobals->time + 0.5;

	pOther->TakeDamage( this, this, GetDamage(), DMG_CRUSH );
}

void CFuncTrain::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( GetSpawnFlags().Any( SF_TRAIN_WAIT_RETRIGGER ) )
	{
		// Move toward my target
		GetSpawnFlags().ClearFlags( SF_TRAIN_WAIT_RETRIGGER );
		Next();
	}
	else
	{
		GetSpawnFlags() |= SF_TRAIN_WAIT_RETRIGGER;
		// Pop back to last target if it's available
		if( pev->enemy )
			SetTarget( pev->enemy->v.targetname );
		SetNextThink( 0 );
		SetAbsVelocity( g_vecZero );
		if( pev->noiseStopMoving )
			EMIT_SOUND( this, CHAN_VOICE, ( char* ) STRING( pev->noiseStopMoving ), m_volume, ATTN_NORM );
	}
}

void CFuncTrain::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "sounds" ) )
	{
		m_sounds = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBasePlatTrain::KeyValue( pkvd );
}

void CFuncTrain::Wait( void )
{
	// Fire the pass target if there is one
	if( m_pevCurrentTarget->message )
	{
		FireTargets( STRING( m_pevCurrentTarget->message ), this, this, USE_TOGGLE, 0 );
		if( FBitSet( m_pevCurrentTarget->spawnflags, SF_CORNER_FIREONCE ) )
			m_pevCurrentTarget->message = 0;
	}

	// need pointer to LAST target.
	if( FBitSet( m_pevCurrentTarget->spawnflags, SF_TRAIN_WAIT_RETRIGGER ) || GetSpawnFlags().Any( SF_TRAIN_WAIT_RETRIGGER ) )
	{
		GetSpawnFlags() |= SF_TRAIN_WAIT_RETRIGGER;
		// clear the sound channel.
		if( pev->noiseMovement )
			STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ) );
		if( pev->noiseStopMoving )
			EMIT_SOUND( this, CHAN_VOICE, ( char* ) STRING( pev->noiseStopMoving ), m_volume, ATTN_NORM );
		SetNextThink( 0 );
		return;
	}

	// ALERT ( at_console, "%f\n", m_flWait );

	if( m_flWait != 0 )
	{// -1 wait will wait forever!		
		SetNextThink( GetLastThink() + m_flWait );
		if( pev->noiseMovement )
			STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ) );
		if( pev->noiseStopMoving )
			EMIT_SOUND( this, CHAN_VOICE, ( char* ) STRING( pev->noiseStopMoving ), m_volume, ATTN_NORM );
		SetThink( &CFuncTrain::Next );
	}
	else
	{
		Next();// do it RIGHT now!
	}
}

//
// Train next - path corner needs to change to next target 
//
void CFuncTrain::Next( void )
{
	CBaseEntity	*pTarg;


	// now find our next target
	//TODO: this entity is supposed to work with path_corner only. Other entities will work, but will probably misbehave. - Solokiller
	//Check for classname and ignore others?
	pTarg = GetNextTarget();

	if( !pTarg )
	{
		if( pev->noiseMovement )
			STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ) );
		// Play stop sound
		if( pev->noiseStopMoving )
			EMIT_SOUND( this, CHAN_VOICE, ( char* ) STRING( pev->noiseStopMoving ), m_volume, ATTN_NORM );
		return;
	}

	// Save last target in case we need to find it again
	SetMessage( GetTarget() );

	SetTarget( pTarg->GetTarget() );
	m_flWait = pTarg->GetDelay();

	if( m_pevCurrentTarget && m_pevCurrentTarget->speed != 0 )
	{// don't copy speed from target if it is 0 (uninitialized)
		SetSpeed( m_pevCurrentTarget->speed );
		ALERT( at_aiconsole, "Train %s speed to %4.2f\n", GetTargetname(), GetSpeed() );
	}
	m_pevCurrentTarget = pTarg->pev;// keep track of this since path corners change our target for us.

	pev->enemy = pTarg->edict();//hack

	if( FBitSet( m_pevCurrentTarget->spawnflags, SF_CORNER_TELEPORT ) )
	{
		// Path corner has indicated a teleport to the next corner.
		GetEffects() |= EF_NOINTERP;
		SetAbsOrigin( pTarg->GetAbsOrigin() - ( GetRelMin() + GetRelMax() )* 0.5 );
		Wait(); // Get on with doing the next path corner.
	}
	else
	{
		// Normal linear move.

		// CHANGED this from CHAN_VOICE to CHAN_STATIC around OEM beta time because trains should
		// use CHAN_STATIC for their movement sounds to prevent sound field problems.
		// this is not a hack or temporary fix, this is how things should be. (sjb).
		if( pev->noiseMovement )
		{
			STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ) );
			EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMovement ), m_volume, ATTN_NORM );
		}
		GetEffects().ClearFlags( EF_NOINTERP );
		SetMoveDone( &CFuncTrain::Wait );
		LinearMove( pTarg->GetAbsOrigin() - ( GetRelMin() + GetRelMax() )* 0.5, GetSpeed() );
	}
}