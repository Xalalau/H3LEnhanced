#include "extdll.h"
#include "util.h"
#include "Skill.h"
#include "gamerules/GameRules.h"
#include "cbase.h"
#include "CBasePlayer.h"

#include "CItem.h"

#include "CHealthKit.h"

LINK_ENTITY_TO_CLASS( item_healthkit, CHealthKit );

void CHealthKit::Spawn( void )
{
	Precache();
	SetModel( "models/w_medkit.mdl" );

	CItem::Spawn();
}

void CHealthKit::Precache( void )
{
	PRECACHE_MODEL( "models/w_medkit.mdl" );
	PRECACHE_SOUND( "items/smallmedkit1.wav" );
}

bool CHealthKit::MyTouch( CBasePlayer *pPlayer )
{
	if( pPlayer->GetDeadFlag() != DEAD_NO )
	{
		return false;
	}

	if( pPlayer->GiveHealth( gSkillData.GetHealthKitCapacity(), DMG_GENERIC ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer );
		WRITE_STRING( GetClassname() );
		MESSAGE_END();

		EMIT_SOUND( pPlayer, CHAN_ITEM, "items/smallmedkit1.wav", 1, ATTN_NORM );

		if( g_pGameRules->ItemShouldRespawn( this ) )
		{
			Respawn();
		}
		else
		{
			UTIL_Remove( this );
		}

		return true;
	}

	return false;
}