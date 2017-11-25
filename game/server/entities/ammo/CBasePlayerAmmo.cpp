#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"

#include "gamerules/GameRules.h"

#include "CBasePlayerAmmo.h"

extern bool gEvilImpulse101;

BEGIN_DATADESC( CBasePlayerAmmo )
	DEFINE_TOUCHFUNC( DefaultTouch ),
	DEFINE_THINKFUNC( Materialize ),
END_DATADESC()

void CBasePlayerAmmo::Spawn()
{
	SetMoveType( MOVETYPE_TOSS );
	SetSolidType( SOLID_TRIGGER );
	SetSize( Vector( -16, -16, 0 ), Vector( 16, 16, 16 ) );
	SetAbsOrigin( GetAbsOrigin() );

	SetTouch( &CBasePlayerAmmo::DefaultTouch );
}

void CBasePlayerAmmo::DefaultTouch( CBaseEntity* pOther )
{
	if( !pOther->IsPlayer() )
	{
		return;
	}

	if( AddAmmo( pOther ) )
	{
		if( g_pGameRules->AmmoShouldRespawn( this ) == GR_AMMO_RESPAWN_YES )
		{
			Respawn();
		}
		else
		{
			SetTouch( NULL );
			SetThink( &CBasePlayerAmmo::SUB_Remove );
			SetNextThink( gpGlobals->time + .1 );
		}
	}
	else if( gEvilImpulse101 )
	{
		// evil impulse 101 hack, kill always
		SetTouch( NULL );
		SetThink( &CBasePlayerAmmo::SUB_Remove );
		SetNextThink( gpGlobals->time + .1 );
	}
}

CBaseEntity* CBasePlayerAmmo::Respawn()
{
	GetEffects() |= EF_NODRAW;
	SetTouch( NULL );

	SetAbsOrigin( g_pGameRules->VecAmmoRespawnSpot( this ) );// move to wherever I'm supposed to repawn.

	SetThink( &CBasePlayerAmmo::Materialize );
	SetNextThink( g_pGameRules->FlAmmoRespawnTime( this ) );

	return this;
}

void CBasePlayerAmmo::Materialize()
{
	if( GetEffects() & EF_NODRAW )
	{
		// changing from invisible state to visible.
		EMIT_SOUND_DYN( this, CHAN_WEAPON, "items/suitchargeok1.wav", 1, ATTN_NORM, 0, 150 );
		GetEffects().ClearFlags( EF_NODRAW );
		GetEffects() |= EF_MUZZLEFLASH;
	}

	SetTouch( &CBasePlayerAmmo::DefaultTouch );
}

bool UTIL_GiveAmmoToPlayer( CBaseEntity* pGiver, CBaseEntity* pPlayer,
							const int iAmount, const char* const pszAmmoName,
							const char* const pszPickupSound )
{
	ASSERT( pGiver );
	ASSERT( pPlayer );

	if( !pGiver || !pPlayer )
		return false;

	if( !pPlayer->IsPlayer() )
		return false;

	CBasePlayer* pPlayerEnt = static_cast<CBasePlayer*>( pPlayer );

	const bool bResult = ( pPlayerEnt->GiveAmmo( iAmount, pszAmmoName ) != -1 );

	if( bResult && pszPickupSound )
	{
		EMIT_SOUND( pGiver, CHAN_ITEM, pszPickupSound, 1, ATTN_NORM );
	}

	return bResult;
}