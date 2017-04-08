#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CFuncPlat.h"

#include "CPlatTrigger.h"

LINK_ENTITY_TO_CLASS( plat_trigger, CPlatTrigger );

//
// Create a trigger entity for a platform.
//
void CPlatTrigger::SpawnInsideTrigger( CFuncPlat *pPlatform )
{
	m_hPlatform = pPlatform;
	// Create trigger entity, "point" it at the owning platform, give it a touch method
	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;
	pev->origin = pPlatform->GetAbsOrigin();

	// Establish the trigger field's size
	Vector vecTMin = pPlatform->pev->mins + Vector( 25, 25, 0 );
	Vector vecTMax = pPlatform->pev->maxs + Vector( 25, 25, 8 );
	vecTMin.z = vecTMax.z - ( pPlatform->m_vecPosition1.z - pPlatform->m_vecPosition2.z + 8 );
	if( pPlatform->pev->size.x <= 50 )
	{
		vecTMin.x = ( pPlatform->pev->mins.x + pPlatform->pev->maxs.x ) / 2;
		vecTMax.x = vecTMin.x + 1;
	}
	if( pPlatform->pev->size.y <= 50 )
	{
		vecTMin.y = ( pPlatform->pev->mins.y + pPlatform->pev->maxs.y ) / 2;
		vecTMax.y = vecTMin.y + 1;
	}
	SetSize( vecTMin, vecTMax );
}

//
// When the platform's trigger field is touched, the platform ???
//
void CPlatTrigger::Touch( CBaseEntity *pOther )
{
	// Ignore touches by non-players
	if( !pOther->IsPlayer() )
		return;

	auto pPlatform = EHANDLE_cast<CFuncPlat*>( m_hPlatform );

	if( FNullEnt( pPlatform ) )
	{
		//The target platform has been removed, remove myself as well. - Solokiller
		UTIL_Remove( this );
		return;
	}

	// Ignore touches by corpses
	if( !pOther->IsAlive() )
		return;

	// Make linked platform go up/down.
	if( pPlatform->m_toggle_state == TS_AT_BOTTOM )
		pPlatform->GoUp();
	else if( pPlatform->m_toggle_state == TS_AT_TOP )
		pPlatform->pev->nextthink = pPlatform->pev->ltime + 1;// delay going down
}

void CPlatTrigger::PlatSpawnInsideTrigger( CFuncPlat* pPlatform )
{
	auto pTrigger = static_cast<CPlatTrigger*>( UTIL_CreateNamedEntity( "plat_trigger" ) );
	pTrigger->SpawnInsideTrigger( pPlatform );
}
