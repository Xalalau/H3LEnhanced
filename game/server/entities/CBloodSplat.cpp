#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CBloodSplat.h"

BEGIN_DATADESC( CBloodSplat )
	DEFINE_THINKFUNC( Spray ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( blood_splat, CBloodSplat );

void CBloodSplat::Spawn( CBaseEntity* pOwner )
{
	pev->origin = pOwner->GetAbsOrigin() + Vector( 0, 0, 32 );
	pev->angles = pOwner->pev->v_angle;
	pev->owner = pOwner->edict();

	SetThink( &CBloodSplat::Spray );
	pev->nextthink = gpGlobals->time + 0.1;
}

void CBloodSplat::Spray()
{
	TraceResult	tr;

	if( g_Language != LANGUAGE_GERMAN )
	{
		UTIL_MakeVectors( pev->angles );
		UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, &tr );

		UTIL_BloodDecalTrace( &tr, BLOOD_COLOR_RED );
	}
	SetThink( &CBloodSplat::SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1;
}