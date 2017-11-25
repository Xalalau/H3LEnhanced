#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"
#include "WeaponsConst.h"

#include "CItem.h"

#include "CItemSuit.h"

LINK_ENTITY_TO_CLASS( item_suit, CItemSuit );

void CItemSuit::Spawn( void )
{
	Precache();
	SetModel( "models/w_suit.mdl" );
	CItem::Spawn();
}

void CItemSuit::Precache( void )
{
	PRECACHE_MODEL( "models/w_suit.mdl" );
}

bool CItemSuit::MyTouch( CBasePlayer *pPlayer )
{
	if( pPlayer->GetWeapons().Any( 1 << WEAPON_SUIT ) )
		return false;

	if( GetSpawnFlags().Any( SF_SUIT_SHORTLOGON ) )
		EMIT_SOUND_SUIT( pPlayer, "!HEV_A0" );	// short version of suit logon,
	else
		EMIT_SOUND_SUIT( pPlayer, "!HEV_AAx" );	// long version of suit logon

	pPlayer->GetWeapons().AddFlags( 1 << WEAPON_SUIT );
	return true;
}