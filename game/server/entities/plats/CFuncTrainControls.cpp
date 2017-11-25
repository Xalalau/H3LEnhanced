#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CFuncTrackTrain.h"

#include "CFuncTrainControls.h"

BEGIN_DATADESC( CFuncTrainControls )
	DEFINE_THINKFUNC( Find ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_traincontrols, CFuncTrainControls );

void CFuncTrainControls::Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	SetModel( GetModelName() );

	SetSize( GetRelMin(), GetRelMax() );
	SetAbsOrigin( GetAbsOrigin() );

	SetThink( &CFuncTrainControls::Find );
	SetNextThink( gpGlobals->time );
}

void CFuncTrainControls::Find( void )
{
	CBaseEntity* pTarget = nullptr;

	do
	{
		pTarget = UTIL_FindEntityByTargetname( pTarget, GetTarget() );
	}
	while( !FNullEnt( pTarget ) && !pTarget->ClassnameIs( "func_tracktrain" ) );

	if( FNullEnt( pTarget ) )
	{
		ALERT( at_console, "No train %s\n", GetTarget() );
		return;
	}

	CFuncTrackTrain *ptrain = CFuncTrackTrain::Instance( pTarget );
	ptrain->SetControls( this );
	UTIL_Remove( this );
}