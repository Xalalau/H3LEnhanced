#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CTriggerChangeTarget.h"

BEGIN_DATADESC( CTriggerChangeTarget )
	DEFINE_FIELD( m_iszNewTarget, FIELD_STRING ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( trigger_changetarget, CTriggerChangeTarget );

void CTriggerChangeTarget::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "m_iszNewTarget" ) )
	{
		m_iszNewTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerChangeTarget::Spawn( void )
{
}

void CTriggerChangeTarget::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pTarget = UTIL_FindEntityByString( NULL, "targetname", GetTarget() );

	if( pTarget )
	{
		pTarget->pev->target = m_iszNewTarget;
		CBaseMonster *pMonster = pTarget->MyMonsterPointer();
		if( pMonster )
		{
			pMonster->m_hGoalEnt = NULL;
		}
	}
}