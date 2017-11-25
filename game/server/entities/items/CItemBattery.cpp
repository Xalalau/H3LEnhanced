#include "extdll.h"
#include "util.h"
#include "Skill.h"
#include "cbase.h"
#include "CBasePlayer.h"
#include "Weapons.h"

#include "CItem.h"

#include "CItemBattery.h"

LINK_ENTITY_TO_CLASS( item_battery, CItemBattery );

void CItemBattery::Spawn( void )
{
	Precache();
	SetModel( "models/w_battery.mdl" );
	CItem::Spawn();
}

void CItemBattery::Precache( void )
{
	PRECACHE_MODEL( "models/w_battery.mdl" );
	PRECACHE_SOUND( "items/gunpickup2.wav" );
}

bool CItemBattery::MyTouch( CBasePlayer *pPlayer )
{
	if( pPlayer->GetDeadFlag() != DEAD_NO )
	{
		return false;
	}

	if( ( pPlayer->GetArmorAmount() < MAX_NORMAL_BATTERY ) &&
		( pPlayer->GetWeapons().Any( 1 << WEAPON_SUIT ) ) )
	{
		int pct;
		char szcharge[ 64 ];

		//TODO: make configurable - Solokiller
		pPlayer->SetArmorAmount( pPlayer->GetArmorAmount() + gSkillData.GetBatteryCapacity() );
		pPlayer->SetArmorAmount( min( pPlayer->GetArmorAmount(), static_cast<float>( MAX_NORMAL_BATTERY ) ) );

		EMIT_SOUND( pPlayer, CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer );
		WRITE_STRING( GetClassname() );
		MESSAGE_END();


		// Suit reports new power level
		// For some reason this wasn't working in release build -- round it.
		pct = ( int ) ( ( float ) ( pPlayer->GetArmorAmount() * 100.0 ) * ( 1.0 / MAX_NORMAL_BATTERY ) + 0.5 );
		pct = ( pct / 5 );
		if( pct > 0 )
			pct--;

		sprintf( szcharge, "!HEV_%1dP", pct );

		//EMIT_SOUND_SUIT( this, szcharge );
		pPlayer->SetSuitUpdate( szcharge, SUIT_SENTENCE, SUIT_NEXT_IN_30SEC );
		return true;
	}
	return false;
}