#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CPathCorner.h"

// Global Savedata for Delay
BEGIN_DATADESC( CPathCorner )
	DEFINE_FIELD( m_flWait, FIELD_FLOAT ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( path_corner, CPathCorner );

void CPathCorner::Spawn()
{
	ASSERTSZ( HasTargetname(), "path_corner without a targetname" );
}

//
// Cache user-entity-field values until spawn is called.
//
void CPathCorner::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		m_flWait = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

#if 0
void CPathCorner::Touch( CBaseEntity *pOther )
{
	if( FBitSet( pOther->pev->flags, FL_MONSTER ) )
	{// monsters don't navigate path corners based on touch anymore
		return;
	}

	// If OTHER isn't explicitly looking for this path_corner, bail out
	if( pOther->m_hGoalEnt != this )
	{
		return;
	}

	// If OTHER has an enemy, this touch is incidental, ignore
	if( !FNullEnt( pOther->pev->enemy ) )
	{
		return;		// fighting, not following a path
	}

	// UNDONE: support non-zero flWait
	/*
	if (m_flWait != 0)
	ALERT(at_warning, "Non-zero path-cornder waits NYI");
	*/

	// Find the next "stop" on the path, make it the goal of the "toucher".
	if( !HasTarget() )
	{
		ALERT( at_warning, "PathCornerTouch: no next stop specified" );
	}

	pOther->m_hGoalEnt = UTIL_FindEntityByTargetname( nullptr, GetTarget() );

	// If "next spot" was not found (does not exist - level design error)
	if( !pOther->m_hGoalEnt )
	{
		ALERT( at_console, "PathCornerTouch--%s couldn't find next stop in path: %s", GetClassname(), GetTarget() );
		return;
	}

	// Turn towards the next stop in the path.
	pOther->pev->ideal_yaw = UTIL_VecToYaw( pOther->m_hGoalEnt->GetAbsOrigin() - pOther->GetAbsOrigin() );
}
#endif