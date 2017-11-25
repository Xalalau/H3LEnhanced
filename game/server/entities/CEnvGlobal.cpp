#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CEnvGlobal.h"

BEGIN_DATADESC( CEnvGlobal )
	DEFINE_FIELD( m_globalstate, FIELD_STRING ),
	DEFINE_FIELD( m_triggermode, FIELD_INTEGER ),
	DEFINE_FIELD( m_initialstate, FIELD_INTEGER ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( env_global, CEnvGlobal );

void CEnvGlobal::Spawn( void )
{
	if( !m_globalstate )
	{
		UTIL_RemoveNow( this );
		return;
	}
	if( GetSpawnFlags().Any( SF_GLOBAL_SET ) )
	{
		if( !gGlobalState.EntityInTable( m_globalstate ) )
			gGlobalState.EntityAdd( m_globalstate, gpGlobals->mapname, ( GLOBALESTATE ) m_initialstate );
	}
}

void CEnvGlobal::KeyValue( KeyValueData *pkvd )
{
	//TODO: what if the keyvalue isn't handled? - Solokiller
	pkvd->fHandled = true;

	if( FStrEq( pkvd->szKeyName, "globalstate" ) )		// State name
		m_globalstate = ALLOC_STRING( pkvd->szValue );
	else if( FStrEq( pkvd->szKeyName, "triggermode" ) )
		m_triggermode = static_cast<TriggerMode>( atoi( pkvd->szValue ) );
	else if( FStrEq( pkvd->szKeyName, "initialstate" ) )
		m_initialstate = atoi( pkvd->szValue );
	else
		CPointEntity::KeyValue( pkvd );
}

void CEnvGlobal::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	GLOBALESTATE oldState = gGlobalState.EntityGetState( m_globalstate );
	GLOBALESTATE newState;

	switch( m_triggermode )
	{
	case TriggerMode::OFF:
		newState = GLOBAL_OFF;
		break;

	case TriggerMode::ON:
		newState = GLOBAL_ON;
		break;

	case TriggerMode::DEAD:
		newState = GLOBAL_DEAD;
		break;

	default:
	case TriggerMode::TOGGLE:
		if( oldState == GLOBAL_ON )
			newState = GLOBAL_OFF;
		else if( oldState == GLOBAL_OFF )
			newState = GLOBAL_ON;
		else
			newState = oldState;
	}

	if( gGlobalState.EntityInTable( m_globalstate ) )
		gGlobalState.EntitySetState( m_globalstate, newState );
	else
		gGlobalState.EntityAdd( m_globalstate, gpGlobals->mapname, newState );
}