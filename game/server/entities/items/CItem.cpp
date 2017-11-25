#include "extdll.h"
#include "util.h"
#include "gamerules/GameRules.h"
#include "cbase.h"

#include "CItem.h"

extern bool gEvilImpulse101;

BEGIN_DATADESC( CItem )
	DEFINE_TOUCHFUNC( ItemTouch ),
	DEFINE_THINKFUNC( Materialize ),
END_DATADESC()

void CItem::Spawn( void )
{
	SetMoveType( MOVETYPE_TOSS );
	SetSolidType( SOLID_TRIGGER );
	SetAbsOrigin( GetAbsOrigin() );
	SetSize( Vector( -16, -16, 0 ), Vector( 16, 16, 16 ) );
	SetTouch( &CItem::ItemTouch );

	if( UTIL_DropToFloor( this ) == DropToFloor::TOOFAR )
	{
		ALERT( at_error, "Item %s fell out of level at %f,%f,%f", GetClassname(), GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z );
		UTIL_Remove( this );
		return;
	}
}

CBaseEntity* CItem::Respawn( void )
{
	SetTouch( NULL );
	GetEffects() |= EF_NODRAW;

	SetAbsOrigin( g_pGameRules->VecItemRespawnSpot( this ) );// blip to whereever you should respawn.

	SetThink( &CItem::Materialize );
	SetNextThink( g_pGameRules->FlItemRespawnTime( this ) );
	return this;
}

void CItem::ItemTouch( CBaseEntity *pOther )
{
	// if it's not a player, ignore
	if( !pOther->IsPlayer() )
	{
		return;
	}

	CBasePlayer *pPlayer = ( CBasePlayer * ) pOther;

	// ok, a player is touching this item, but can he have it?
	if( !g_pGameRules->CanHaveItem( pPlayer, this ) )
	{
		// no? Ignore the touch.
		return;
	}

	if( MyTouch( pPlayer ) )
	{
		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
		SetTouch( NULL );

		// player grabbed the item. 
		g_pGameRules->PlayerGotItem( pPlayer, this );
		if( g_pGameRules->ItemShouldRespawn( this ) == GR_ITEM_RESPAWN_YES )
		{
			Respawn();
		}
		else
		{
			UTIL_Remove( this );
		}
	}
	else if( gEvilImpulse101 )
	{
		UTIL_Remove( this );
	}
}

void CItem::Materialize( void )
{
	if( GetEffects().Any( EF_NODRAW ) )
	{
		// changing from invisible state to visible.
		EMIT_SOUND_DYN( this, CHAN_WEAPON, "items/suitchargeok1.wav", 1, ATTN_NORM, 0, 150 );
		GetEffects().ClearFlags( EF_NODRAW );
		GetEffects() |= EF_MUZZLEFLASH;
	}

	SetTouch( &CItem::ItemTouch );
}