#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CTriggerVolume.h"

LINK_ENTITY_TO_CLASS( trigger_transition, CTriggerVolume );

void CTriggerVolume::Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	SetModel( GetModelName() );    // set size and link into world
	SetModelName( iStringNull );
	SetModelIndex( 0 );
}