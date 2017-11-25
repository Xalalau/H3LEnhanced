#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CBaseTrigger.h"

BEGIN_DATADESC( CBaseTrigger )
	DEFINE_TOUCHFUNC( TeleportTouch ),
	DEFINE_TOUCHFUNC( MultiTouch ),
	DEFINE_THINKFUNC( MultiWaitOver ),
	DEFINE_USEFUNC( CounterUse ),
	DEFINE_USEFUNC( ToggleUse ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( trigger, CBaseTrigger );

void CBaseTrigger::TeleportTouch( CBaseEntity *pOther )
{
	// Only teleport monsters or clients
	if( !pOther->GetFlags().Any( FL_CLIENT | FL_MONSTER ) )
		return;

	if( !UTIL_IsMasterTriggered( m_sMaster, pOther ) )
		return;

	if( !GetSpawnFlags().Any( SF_TRIGGER_ALLOWMONSTERS ) )
	{// no monsters allowed!
		if( pOther->GetFlags().Any( FL_MONSTER ) )
		{
			return;
		}
	}

	if( GetSpawnFlags().Any( SF_TRIGGER_NOCLIENTS ) )
	{// no clients allowed
		if( pOther->IsPlayer() )
		{
			return;
		}
	}

	CBaseEntity* pTarget = UTIL_FindEntityByTargetname( nullptr, GetTarget() );
	if( !pTarget )
		return;

	Vector tmp = pTarget->GetAbsOrigin();

	if( pOther->IsPlayer() )
	{
		tmp.z -= pOther->GetRelMin().z;// make origin adjustments in case the teleportee is a player. (origin in center, not at feet)
	}

	tmp.z++;

	pOther->GetFlags().ClearFlags( FL_ONGROUND );

	pOther->SetAbsOrigin( tmp );

	pOther->SetAbsAngles( pTarget->GetAbsAngles() );

	if( pOther->IsPlayer() )
	{
		pOther->SetViewAngle( pTarget->GetAbsAngles() );
	}

	pOther->SetFixAngleMode( FIXANGLE_SET );
	pOther->SetAbsVelocity( g_vecZero );
	pOther->SetBaseVelocity( g_vecZero );
}

void CBaseTrigger::MultiTouch( CBaseEntity *pOther )
{
	// Only touch clients, monsters, or pushables (depending on flags)
	if( ( pOther->GetFlags().Any( FL_CLIENT ) && !GetSpawnFlags().Any( SF_TRIGGER_NOCLIENTS ) ) ||
		( pOther->GetFlags().Any( FL_MONSTER ) && GetSpawnFlags().Any( SF_TRIGGER_ALLOWMONSTERS ) ) ||
		( GetSpawnFlags().Any( SF_TRIGGER_PUSHABLES ) && pOther->ClassnameIs( "func_pushable" ) ) )
	{

#if 0
		// if the trigger has an angles field, check player's facing direction
		if( GetMoveDir() != g_vecZero )
		{
			UTIL_MakeVectors( pOther->GetAbsAngles() );
			if( DotProduct( gpGlobals->v_forward, GetMoveDir() ) < 0 )
				return;         // not facing the right way
		}
#endif

		ActivateMultiTrigger( pOther );
	}
}

//
// the trigger was just touched/killed/used
// self.enemy should be set to the activator so it can be held through a delay
// so wait for the delay time before firing
//
void CBaseTrigger::ActivateMultiTrigger( CBaseEntity *pActivator )
{
	if( GetNextThink() > gpGlobals->time )
		return;         // still waiting for reset time

	if( !UTIL_IsMasterTriggered( m_sMaster, pActivator ) )
		return;

	if( ClassnameIs( "trigger_secret" ) )
	{
		CBaseEntity* pEnemy = GetPEVEnemy();
		if( !pEnemy || !pEnemy->IsPlayer() )
			return;
		gpGlobals->found_secrets++;
	}

	if( !FStringNull( pev->noise ) )
		EMIT_SOUND( this, CHAN_VOICE, ( char* ) STRING( pev->noise ), 1, ATTN_NORM );

	// don't trigger again until reset
	// SetTakeDamageMode( DAMAGE_NO );

	m_hActivator = pActivator;
	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );

	if( HasMessage() && pActivator->IsPlayer() )
	{
		UTIL_ShowMessage( GetMessage(), pActivator );
		//ClientPrint( pActivator, HUD_PRINTCENTER, GetMessage() );
	}

	if( m_flWait > 0 )
	{
		SetThink( &CBaseTrigger::MultiWaitOver );
		SetNextThink( gpGlobals->time + m_flWait );
	}
	else
	{
		// we can't just remove (self) here, because this is a touch function
		// called while C code is looping through area links...
		SetTouch( NULL );
		SetNextThink( gpGlobals->time + 0.1 );
		SetThink( &CBaseTrigger::SUB_Remove );
	}
}

// the wait time has passed, so set back up for another activation
void CBaseTrigger::MultiWaitOver( void )
{
	//	if ( GetMaxHealth() )
	//		{
	//		SetHealth( GetMaxHealth() );
	//		SetTakeDamageMode( DAMAGE_YES );
	//		SetSolidType( SOLID_BBOX );
	//		}
	SetThink( NULL );
}

void CBaseTrigger::CounterUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_cTriggersLeft--;
	m_hActivator = pActivator;

	if( m_cTriggersLeft < 0 )
		return;

	const bool fTellActivator =
		( m_hActivator != nullptr ) &&
		m_hActivator->IsPlayer() &&
		!GetSpawnFlags().Any( SPAWNFLAG_NOMESSAGE );

	if( m_cTriggersLeft != 0 )
	{
		if( fTellActivator )
		{
			// UNDONE: I don't think we want these Quakesque messages
			switch( m_cTriggersLeft )
			{
			case 1:		ALERT( at_console, "Only 1 more to go..." );		break;
			case 2:		ALERT( at_console, "Only 2 more to go..." );		break;
			case 3:		ALERT( at_console, "Only 3 more to go..." );		break;
			default:	ALERT( at_console, "There are more to go..." );	break;
			}
		}
		return;
	}

	// !!!UNDONE: I don't think we want these Quakesque messages
	if( fTellActivator )
		ALERT( at_console, "Sequence completed!" );

	ActivateMultiTrigger( m_hActivator );
}

//
// Cache user-entity-field values until spawn is called.
//
void CBaseTrigger::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "damage" ) )
	{
		SetDamage( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "count" ) )
	{
		m_cTriggersLeft = ( int ) atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "damagetype" ) )
	{
		m_bitsDamageInflict = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

//
// ToggleUse - If this is the USE function for a trigger, its state will toggle every time it's fired
//
void CBaseTrigger::ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( GetSolidType() == SOLID_NOT )
	{// if the trigger is off, turn it on
		SetSolidType( SOLID_TRIGGER );

		// Force retouch
		gpGlobals->force_retouch++;
	}
	else
	{// turn the trigger off
		SetSolidType( SOLID_NOT );
	}
	SetAbsOrigin( GetAbsOrigin() );
}

/*
================
InitTrigger
================
*/
void CBaseTrigger::InitTrigger()
{
	// trigger angles are used for one-way touches.  An angle of 0 is assumed
	// to mean no restrictions, so use a yaw of 360 instead.
	if( GetAbsAngles() != g_vecZero )
		SetMovedir( this );
	SetSolidType( SOLID_TRIGGER );
	SetMoveType( MOVETYPE_NONE );
	SetModel( GetModelName() );    // set size and link into world
	if( CVAR_GET_FLOAT( "showtriggers" ) == 0 )
		GetEffects() |= EF_NODRAW;
}