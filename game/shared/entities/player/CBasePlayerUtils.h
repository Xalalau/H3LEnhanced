#ifndef GAME_SHARED_ENTITIES_PLAYER_CBASEPLAYERUTILS_H
#define GAME_SHARED_ENTITIES_PLAYER_CBASEPLAYERUTILS_H

/**
*	@file
*
*	Provides utility functions for operating on CBasePlayer and subclasses, and related classes.
*/

/*
//TODO: fix missing header guards. - Solokiller
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"
#include "Weapons.h"
*/

/**
*	Iterates over all player weapons and invokes functor on each non-null weapon.
*	Arguments passed to operator() are CBasePlayer& and CBasePlayerWeapon&.
*	@param player Player whose weapons will be enumerated over
*	@param functor Functor to invoke for each weapon.
*/
template<typename FUNCTOR>
void ForEachPlayerWeapon( CBasePlayer& player, const FUNCTOR& functor )
{
	for( int iBucket = 0; iBucket < MAX_WEAPON_SLOTS; ++iBucket )
	{
		for( CBasePlayerWeapon* pWeapon = player.m_rgpPlayerItems[ iBucket ]; pWeapon; pWeapon = pWeapon->m_pNext )
		{
			functor( player, *pWeapon );
		}
	}
}

#endif //GAME_SHARED_ENTITIES_PLAYER_CBASEPLAYERUTILS_H
