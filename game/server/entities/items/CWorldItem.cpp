#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CWorldItem.h"

LINK_ENTITY_TO_CLASS( world_items, CWorldItem );

void CWorldItem::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "type" ) )
	{
		m_iType = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CWorldItem::Spawn( void )
{
	CBaseEntity *pEntity = NULL;

	switch( m_iType )
	{
	case 44: // ITEM_BATTERY:
		pEntity = CBaseEntity::Create( "item_battery", GetAbsOrigin(), GetAbsAngles() );
		break;
	case 42: // ITEM_ANTIDOTE:
		pEntity = CBaseEntity::Create( "item_antidote", GetAbsOrigin(), GetAbsAngles() );
		break;
	case 43: // ITEM_SECURITY:
		pEntity = CBaseEntity::Create( "item_security", GetAbsOrigin(), GetAbsAngles() );
		break;
	case 45: // ITEM_SUIT:
		pEntity = CBaseEntity::Create( "item_suit", GetAbsOrigin(), GetAbsAngles() );
		break;
	}

	if( !pEntity )
	{
		ALERT( at_console, "unable to create world_item %d\n", m_iType );
	}
	else
	{
		pEntity->SetTarget( GetTarget() );
		pEntity->SetTargetname( GetTargetname() );
		pEntity->GetSpawnFlags() = GetSpawnFlags();
	}

	UTIL_RemoveNow( this );
}