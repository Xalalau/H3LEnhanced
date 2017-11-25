#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"
#include "WeaponsConst.h"

#include "CItem.h"

#include "CItemLongJump.h"

LINK_ENTITY_TO_CLASS( item_longjump, CItemLongJump );

void CItemLongJump::Spawn( void )
{
	Precache();
	SetModel( "models/w_longjump.mdl" );
	CItem::Spawn();
}

void CItemLongJump::Precache( void )
{
	PRECACHE_MODEL( "models/w_longjump.mdl" );
}

bool CItemLongJump::MyTouch( CBasePlayer *pPlayer )
{
	if( pPlayer->m_fLongJump )
	{
		return false;
	}

	if( ( pPlayer->GetWeapons().Any( 1 << WEAPON_SUIT ) ) )
	{
		pPlayer->m_fLongJump = true;// player now has longjump module

		g_engfuncs.pfnSetPhysicsKeyValue( pPlayer->edict(), "slj", "1" );

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer );
		WRITE_STRING( GetClassname() );
		MESSAGE_END();

		EMIT_SOUND_SUIT( pPlayer, "!HEV_A1" );	// Play the longjump sound UNDONE: Kelly? correct sound?
		return true;
	}
	return false;
}