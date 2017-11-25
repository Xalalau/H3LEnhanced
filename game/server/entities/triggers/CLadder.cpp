#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CLadder.h"

LINK_ENTITY_TO_CLASS( func_ladder, CLadder );

void CLadder::KeyValue( KeyValueData *pkvd )
{
	CBaseTrigger::KeyValue( pkvd );
}

void CLadder::Spawn( void )
{
	Precache();

	SetModel( GetModelName() );    // set size and link into world
	SetMoveType( MOVETYPE_PUSH );
}

void CLadder::Precache( void )
{
	// Do all of this in here because we need to 'convert' old saved games
	SetSolidType( SOLID_NOT );
	SetSkin( CONTENTS_LADDER );
	if( CVAR_GET_FLOAT( "showtriggers" ) == 0 )
	{
		SetRenderMode( kRenderTransTexture );
		SetRenderAmount( 0 );
	}
	GetEffects().ClearFlags( EF_NODRAW );
}