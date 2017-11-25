#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CAutoTrigger.h"

BEGIN_DATADESC( CAutoTrigger )
	DEFINE_FIELD( m_globalstate, FIELD_STRING ),
	DEFINE_FIELD( triggerType, FIELD_INTEGER ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( trigger_auto, CAutoTrigger );

void CAutoTrigger::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "globalstate" ) )
	{
		m_globalstate = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "triggerstate" ) )
	{
		int type = atoi( pkvd->szValue );
		switch( type )
		{
		case 0:
			triggerType = USE_OFF;
			break;
		case 2:
			triggerType = USE_TOGGLE;
			break;
		default:
			triggerType = USE_ON;
			break;
		}
		pkvd->fHandled = true;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CAutoTrigger::Spawn( void )
{
	Precache();
}

void CAutoTrigger::Precache( void )
{
	SetNextThink( gpGlobals->time + 0.1 );
}

void CAutoTrigger::Think( void )
{
	if( !m_globalstate || gGlobalState.EntityGetState( m_globalstate ) == GLOBAL_ON )
	{
		SUB_UseTargets( this, triggerType, 0 );
		if( GetSpawnFlags().Any( SF_AUTO_FIREONCE ) )
			UTIL_Remove( this );
	}
}