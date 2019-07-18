#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CItemSoda.h"

BEGIN_DATADESC( CItemSoda )
	DEFINE_THINKFUNC( CanThink ),
	DEFINE_TOUCHFUNC( CanTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_sodacan, CItemSoda );

void CItemSoda::Spawn( void )
{
	Precache();
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_TOSS );

	SetModel( "models/can.mdl" );
	SetSize( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );

	SetThink( &CItemSoda::CanThink );
	SetNextThink( gpGlobals->time + 0.5 );
}

void CItemSoda::Precache( void )
{
}

void CItemSoda::CanThink( void )
{
	EMIT_SOUND( this, CHAN_WEAPON, "weapons/g_bounce3.wav", 1, ATTN_NORM );

	SetSolidType( SOLID_TRIGGER );
	SetSize( Vector( -8, -8, 0 ), Vector( 8, 8, 8 ) );
	SetThink( NULL );
	SetTouch( &CItemSoda::CanTouch );
}

void CItemSoda::CanTouch( CBaseEntity *pOther )
{
	if( !pOther->IsPlayer() )
	{
		return;
	}

	// spoit sound here

	// ############ hu3lifezado ############ //
	// No Hu3-Life refrigerante faz bem! Te da 50 de vida ao inves de 1
	pOther->GiveHealth( 50, DMG_GENERIC );// a bit of health.
	// ############ //

	if( !FNullEnt( GetOwner() ) )
	{
		// tell the machine the can was taken
		GetOwner()->SetFrags( 0 );
	}

	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	GetEffects() = EF_NODRAW;
	SetTouch( NULL );
	SetThink( &CItemSoda::SUB_Remove );
	SetNextThink( gpGlobals->time );
}