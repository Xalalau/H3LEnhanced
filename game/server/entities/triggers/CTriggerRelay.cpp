#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CTriggerRelay.h"

BEGIN_DATADESC( CTriggerRelay )
	DEFINE_FIELD( triggerType, FIELD_INTEGER ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( trigger_relay, CTriggerRelay );

void CTriggerRelay::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "triggerstate" ) )
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

void CTriggerRelay::Spawn( void )
{
}

void CTriggerRelay::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SUB_UseTargets( this, triggerType, 0 );
	if( GetSpawnFlags().Any( SF_RELAY_FIREONCE ) )
		UTIL_Remove( this );
}