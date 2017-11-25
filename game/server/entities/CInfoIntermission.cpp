#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CInfoIntermission.h"

LINK_ENTITY_TO_CLASS( info_intermission, CInfoIntermission );

void CInfoIntermission::Spawn( void )
{
	SetAbsOrigin( GetAbsOrigin() );
	SetSolidType( SOLID_NOT );
	GetEffects() = EF_NODRAW;
	SetViewAngle( g_vecZero );

	SetNextThink( gpGlobals->time + 2 );// let targets spawn!

}

void CInfoIntermission::Think( void )
{
	// find my target
	CBaseEntity* pTarget = UTIL_FindEntityByTargetname( nullptr, GetTarget() );

	if( pTarget )
	{
		Vector vecViewAngle = UTIL_VecToAngles( ( pTarget->GetAbsOrigin() - GetAbsOrigin() ).Normalize() );
		vecViewAngle.x = -vecViewAngle.x;
		SetViewAngle( vecViewAngle );
	}
}