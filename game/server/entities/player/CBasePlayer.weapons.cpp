/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"
#include "Weapons.h"
#include "entities/weapons/CWeaponBox.h"

#include "Skill.h"
#include "gamerules/GameRules.h"
#include "Server.h"
#include "ServerInterface.h"

#include "CBasePlayerUtils.h"

//Wasn't initialized - Solokiller
bool DLL_GLOBAL gEvilImpulse101 = false;

//
// Marks everything as new so the player will resend this to the hud.
//
void CBasePlayer::RenewItems()
{
	//TODO: needed? - Solokiller
}

//=========================================================
// PackDeadPlayerItems - call this when a player dies to
// pack up the appropriate weapons and ammo items, and to
// destroy anything that shouldn't be packed.
//
// This is pretty brute force :(
//=========================================================
void CBasePlayer::PackDeadPlayerItems()
{
	int iWeaponRules;
	int iAmmoRules;
	int i;
	CBasePlayerWeapon *rgpPackWeapons[ 20 ];// 20 hardcoded for now. How to determine exactly how many weapons we have? TODO

	//+ 1 so it can be iterated until == -1. See below. - Solokiller
	int iPackAmmo[ CAmmoTypes::MAX_AMMO_TYPES + 1 ];
	int iPW = 0;// index into packweapons array
	int iPA = 0;// index into packammo array

	memset( rgpPackWeapons, 0, sizeof( rgpPackWeapons ) );
	memset( iPackAmmo, -1, sizeof( iPackAmmo ) );

	// get the game rules 
	iWeaponRules = g_pGameRules->DeadPlayerWeapons( this );
	iAmmoRules = g_pGameRules->DeadPlayerAmmo( this );

	if( iWeaponRules == GR_PLR_DROP_GUN_NO && iAmmoRules == GR_PLR_DROP_AMMO_NO )
	{
		// nothing to pack. Remove the weapons and return. Don't call create on the box!
		RemoveAllItems( true );
		return;
	}

	// go through all of the weapons and make a list of the ones to pack
	for( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		if( m_rgpPlayerItems[ i ] )
		{
			// there's a weapon here. Should I pack it?
			CBasePlayerWeapon *pPlayerItem = m_rgpPlayerItems[ i ];

			while( pPlayerItem )
			{
				switch( iWeaponRules )
				{
				case GR_PLR_DROP_GUN_ACTIVE:
					if( m_pActiveItem && pPlayerItem == m_pActiveItem )
					{
						// this is the active item. Pack it.
						rgpPackWeapons[ iPW++ ] = ( CBasePlayerWeapon * ) pPlayerItem;
					}
					break;

				case GR_PLR_DROP_GUN_ALL:
					rgpPackWeapons[ iPW++ ] = ( CBasePlayerWeapon * ) pPlayerItem;
					break;

				default:
					break;
				}

				pPlayerItem = pPlayerItem->m_pNext;
			}
		}
	}

	// now go through ammo and make a list of which types to pack.
	if( iAmmoRules != GR_PLR_DROP_AMMO_NO )
	{
		for( i = 0; i < CAmmoTypes::MAX_AMMO_TYPES; i++ )
		{
			if( m_rgAmmo[ i ] > 0 )
			{
				// player has some ammo of this type.
				switch( iAmmoRules )
				{
				case GR_PLR_DROP_AMMO_ALL:
					iPackAmmo[ iPA++ ] = i;
					break;

				case GR_PLR_DROP_AMMO_ACTIVE:
					if( m_pActiveItem && i == m_pActiveItem->PrimaryAmmoIndex() )
					{
						// this is the primary ammo type for the active weapon
						iPackAmmo[ iPA++ ] = i;
					}
					else if( m_pActiveItem && i == m_pActiveItem->SecondaryAmmoIndex() )
					{
						// this is the secondary ammo type for the active weapon
						iPackAmmo[ iPA++ ] = i;
					}
					break;

				default:
					break;
				}
			}
		}
	}

	// create a box to pack the stuff into.
	CWeaponBox *pWeaponBox = ( CWeaponBox * ) CBaseEntity::Create( "weaponbox", GetAbsOrigin(), GetAbsAngles(), edict() );

	Vector vecAngles = pWeaponBox->GetAbsAngles();
	vecAngles.x = 0;// don't let weaponbox tilt.
	vecAngles.z = 0;
	pWeaponBox->SetAbsAngles( vecAngles );

	pWeaponBox->SetThink( &CWeaponBox::Kill );
	pWeaponBox->SetNextThink( gpGlobals->time + 120 );

	// back these two lists up to their first elements
	iPA = 0;
	iPW = 0;

	// pack the ammo
	while( iPackAmmo[ iPA ] != -1 )
	{
		pWeaponBox->PackAmmo( MAKE_STRING( g_AmmoTypes.GetAmmoTypeByID( iPackAmmo[ iPA ] )->GetName() ), m_rgAmmo[ iPackAmmo[ iPA ] ] );
		iPA++;
	}

	// now pack all of the items in the lists
	while( rgpPackWeapons[ iPW ] )
	{
		// weapon unhooked from the player. Pack it into der box.
		pWeaponBox->PackWeapon( rgpPackWeapons[ iPW ] );

		iPW++;
	}

	pWeaponBox->SetAbsVelocity( GetAbsVelocity() * 1.2 );// weaponbox has player's velocity, then some.

	RemoveAllItems( true );// now strip off everything that wasn't handled by the code above.
}

void CBasePlayer::RemoveAllItems( const bool removeSuit )
{
	if( m_pActiveItem )
	{
		ResetAutoaim();
		m_pActiveItem->Holster();
		m_pActiveItem = NULL;
	}

	m_pLastItem = NULL;

	if( m_pTank != NULL )
	{
		m_pTank->Use( this, this, USE_OFF, 0 );
		m_pTank = NULL;
	}

	int i;
	CBasePlayerWeapon *pPendingItem;
	for( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		m_pActiveItem = m_rgpPlayerItems[ i ];
		while( m_pActiveItem )
		{
			pPendingItem = m_pActiveItem->m_pNext;
			m_pActiveItem->Drop();
			m_pActiveItem = pPendingItem;
		}
		m_rgpPlayerItems[ i ] = NULL;
	}
	m_pActiveItem = NULL;

	ClearViewModelName();
	ClearWeaponModelName();

	if( removeSuit )
		GetWeapons().ClearAll();
	else
		GetWeapons().ClearFlags( WEAPON_ALLWEAPONS );

	for( i = 0; i < CAmmoTypes::MAX_AMMO_TYPES; i++ )
		m_rgAmmo[ i ] = 0;

	UpdateClientData();
	// send Selected Weapon Message to our client
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, this );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
	MESSAGE_END();
}

//=========================================================
// 
//=========================================================
bool CBasePlayer::SwitchWeapon( CBasePlayerWeapon *pWeapon )
{
	if( !pWeapon->CanDeploy() )
	{
		return false;
	}

	ResetAutoaim();

	if( m_pActiveItem )
	{
		m_pActiveItem->Holster();
	}

	m_pActiveItem = pWeapon;
	pWeapon->Deploy();

	return true;
}

//
// Add a weapon to the player (Item == Weapon == Selectable Object)
//
bool CBasePlayer::AddPlayerItem( CBasePlayerWeapon *pItem )
{
	CBasePlayerWeapon *pInsert;

	pInsert = m_rgpPlayerItems[ pItem->iItemSlot() ];

	while( pInsert )
	{
		if( pInsert->ClassnameIs( pItem->GetClassname() ) )
		{
			if( pItem->AddDuplicate( pInsert ) )
			{
				g_pGameRules->PlayerGotWeapon( this, pItem );
				pItem->CheckRespawn();

				// ugly hack to update clip w/o an update clip message
				pInsert->UpdateItemInfo();
				if( m_pActiveItem )
					m_pActiveItem->UpdateItemInfo();

				pItem->Kill();
			}
			else if( gEvilImpulse101 )
			{
				// FIXME: remove anyway for deathmatch testing
				pItem->Kill();
			}
			return false;
		}
		pInsert = pInsert->m_pNext;
	}


	if( pItem->AddToPlayer( this ) )
	{
		g_pGameRules->PlayerGotWeapon( this, pItem );
		pItem->CheckRespawn();

		pItem->m_pNext = m_rgpPlayerItems[ pItem->iItemSlot() ];
		m_rgpPlayerItems[ pItem->iItemSlot() ] = pItem;

		// should we switch to this item?
		if( g_pGameRules->FShouldSwitchWeapon( this, pItem ) )
		{
			SwitchWeapon( pItem );
		}

		return true;
	}
	else if( gEvilImpulse101 )
	{
		// FIXME: remove anyway for deathmatch testing
		pItem->Kill();
	}
	return false;
}

bool CBasePlayer::RemovePlayerItem( CBasePlayerWeapon *pItem )
{
	if( m_pActiveItem == pItem )
	{
		ResetAutoaim();
		pItem->Holster();
		pItem->SetNextThink( 0 );// crowbar may be trying to swing again, etc.
		pItem->SetThink( NULL );
		m_pActiveItem = NULL;
		ClearViewModelName();
		ClearWeaponModelName();
	}

	//In some cases an item can be both the active and last item, like for instance dropping all weapons and only having an exhaustible weapon left. - Solokiller
	if( m_pLastItem == pItem )
		m_pLastItem = NULL;

	CBasePlayerWeapon *pPrev = m_rgpPlayerItems[ pItem->iItemSlot() ];

	if( pPrev == pItem )
	{
		m_rgpPlayerItems[ pItem->iItemSlot() ] = pItem->m_pNext;
		return true;
	}
	else
	{
		while( pPrev && pPrev->m_pNext != pItem )
		{
			pPrev = pPrev->m_pNext;
		}
		if( pPrev )
		{
			pPrev->m_pNext = pItem->m_pNext;
			return true;
		}
	}
	return false;
}

//=========================================================
// DropPlayerItem - drop the named item, or if no name,
// the active item. 
//=========================================================
void CBasePlayer::DropPlayerItem( char *pszItemName )
{
	if( !g_pGameRules->IsMultiplayer() || ( weaponstay.value > 0 ) )
	{
		// no dropping in single player.
		return;
	}

	if( !strlen( pszItemName ) )
	{
		// if this string has no length, the client didn't type a name!
		// assume player wants to drop the active item.
		// make the string null to make future operations in this function easier
		pszItemName = NULL;
	}

	CBasePlayerWeapon *pWeapon;
	int i;

	for( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		pWeapon = m_rgpPlayerItems[ i ];

		while( pWeapon )
		{
			if( pszItemName )
			{
				// try to match by name. 
				if( !strcmp( pszItemName, pWeapon->GetClassname() ) )
				{
					// match! 
					break;
				}
			}
			else
			{
				// trying to drop active item
				if( pWeapon == m_pActiveItem )
				{
					// active item!
					break;
				}
			}

			pWeapon = pWeapon->m_pNext;
		}


		// if we land here with a valid pWeapon pointer, that's because we found the 
		// item we want to drop and hit a BREAK;  pWeapon is the item.
		if( pWeapon )
		{
			if( !g_pGameRules->GetNextBestWeapon( this, pWeapon ) )
				return; // can't drop the item they asked for, may be our last item or something we can't holster

			UTIL_MakeVectors( GetAbsAngles() );

			GetWeapons().ClearFlags( 1 << pWeapon->m_iId );// take item off hud

			CWeaponBox *pWeaponBox = ( CWeaponBox * ) CBaseEntity::Create( "weaponbox", GetAbsOrigin() + gpGlobals->v_forward * 10, GetAbsAngles(), edict() );
			Vector vecAngles = pWeaponBox->GetAbsAngles();
			vecAngles.x = 0;
			vecAngles.z = 0;
			pWeaponBox->SetAbsAngles( vecAngles );
			pWeaponBox->PackWeapon( pWeapon );
			pWeaponBox->SetAbsVelocity( gpGlobals->v_forward * 300 + gpGlobals->v_forward * 100 );

			// drop half of the ammo for this weapon.
			int	iAmmoIndex;

			iAmmoIndex = GetAmmoIndex( pWeapon->pszAmmo1() ); // ???

			if( iAmmoIndex != -1 )
			{
				// this weapon weapon uses ammo, so pack an appropriate amount.
				if( pWeapon->iFlags() & ITEM_FLAG_EXHAUSTIBLE )
				{
					// pack up all the ammo, this weapon is its own ammo type
					pWeaponBox->PackAmmo( MAKE_STRING( pWeapon->pszAmmo1() ), m_rgAmmo[ iAmmoIndex ] );
					m_rgAmmo[ iAmmoIndex ] = 0;

				}
				else
				{
					// pack half of the ammo
					pWeaponBox->PackAmmo( MAKE_STRING( pWeapon->pszAmmo1() ), m_rgAmmo[ iAmmoIndex ] / 2 );
					m_rgAmmo[ iAmmoIndex ] /= 2;
				}

			}

			return;// we're done, so stop searching with the FOR loop.
		}
	}
}

//=========================================================
// HasPlayerItem Does the player already have this item?
//=========================================================
bool CBasePlayer::HasPlayerItem( CBasePlayerWeapon *pCheckItem ) const
{
	CBasePlayerWeapon *pItem = m_rgpPlayerItems[ pCheckItem->iItemSlot() ];

	while( pItem )
	{
		if( pItem->ClassnameIs( pCheckItem->GetClassname() ) )
		{
			return true;
		}
		pItem = pItem->m_pNext;
	}

	return false;
}

//=========================================================
// HasNamedPlayerItem Does the player already have this item?
//=========================================================
bool CBasePlayer::HasNamedPlayerItem( const char *pszItemName ) const
{
	CBasePlayerWeapon *pItem;
	int i;

	for( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		pItem = m_rgpPlayerItems[ i ];

		while( pItem )
		{
			if( !strcmp( pszItemName, pItem->GetClassname() ) )
			{
				return true;
			}
			pItem = pItem->m_pNext;
		}
	}

	return false;
}

//==============================================
// HasWeapons - do I have any weapons at all?
//==============================================
bool CBasePlayer::HasWeapons() const
{
	int i;

	for( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		if( m_rgpPlayerItems[ i ] )
		{
			return true;
		}
	}

	return false;
}

void CBasePlayer::SelectPrevItem( int iItem )
{
	//TODO
}

void CBasePlayer::SelectNextItem( int iItem )
{
	CBasePlayerWeapon *pItem;

	pItem = m_rgpPlayerItems[ iItem ];

	if( !pItem )
		return;

	if( pItem == m_pActiveItem )
	{
		// select the next one in the chain
		pItem = m_pActiveItem->m_pNext;
		if( !pItem )
		{
			return;
		}

		CBasePlayerWeapon *pLast;
		pLast = pItem;
		while( pLast->m_pNext )
			pLast = pLast->m_pNext;

		// relink chain
		pLast->m_pNext = m_pActiveItem;
		m_pActiveItem->m_pNext = NULL;
		m_rgpPlayerItems[ iItem ] = pItem;
	}

	ResetAutoaim();

	// FIX, this needs to queue them up and delay
	if( m_pActiveItem )
	{
		m_pActiveItem->Holster();
	}

	m_pActiveItem = pItem;

	if( m_pActiveItem )
	{
		m_pActiveItem->Deploy();
		m_pActiveItem->UpdateItemInfo();
	}
}

void CBasePlayer::SelectLastItem()
{
	if( !m_pLastItem )
	{
		return;
	}

	if( m_pActiveItem && !m_pActiveItem->CanHolster() )
	{
		return;
	}

	ResetAutoaim();

	// FIX, this needs to queue them up and delay
	if( m_pActiveItem )
		m_pActiveItem->Holster();

	CBasePlayerWeapon *pTemp = m_pActiveItem;
	m_pActiveItem = m_pLastItem;
	m_pLastItem = pTemp;
	m_pActiveItem->Deploy();
	m_pActiveItem->UpdateItemInfo();
}

void CBasePlayer::SelectItem( const char *pstr )
{
	if( !pstr )
		return;

	CBasePlayerWeapon *pItem = NULL;

	for( int i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		if( m_rgpPlayerItems[ i ] )
		{
			pItem = m_rgpPlayerItems[ i ];

			while( pItem )
			{
				if( pItem->ClassnameIs( pstr ) )
					break;
				pItem = pItem->m_pNext;
			}
		}

		if( pItem )
			break;
	}

	if( !pItem )
		return;


	if( pItem == m_pActiveItem )
		return;

	ResetAutoaim();

	// FIX, this needs to queue them up and delay
	if( m_pActiveItem )
		m_pActiveItem->Holster();

	m_pLastItem = m_pActiveItem;
	m_pActiveItem = pItem;

	if( m_pActiveItem )
	{
		m_pActiveItem->Deploy();
		m_pActiveItem->UpdateItemInfo();
	}
}

/*
============
ItemPreFrame

Called every frame by the player PreThink
============
*/
void CBasePlayer::ItemPreFrame()
{
	if( m_flNextAttack > UTIL_WeaponTimeBase() )
	{
		return;
	}

	//Update all holstered weapons. - Solokiller
	ForEachPlayerWeapon( *this, []( CBasePlayer& player, CBasePlayerWeapon& weapon )
	{
		if( &weapon == player.m_pActiveItem )
			return;

		weapon.WeaponHolsterPreFrame();
	});

	if( !m_pActiveItem )
		return;

	m_pActiveItem->ItemPreFrame();
}

/*
============
ItemPostFrame

Called every frame by the player PostThink
============
*/
void CBasePlayer::ItemPostFrame()
{
	// check if the player is using a tank
	if( m_pTank != NULL )
		return;

	const bool bCanAttack = m_flNextAttack <= UTIL_WeaponTimeBase();

	//The original code would only check for impulse commands if the weapon could fire.
	//If this cvar is non-zero we'll let players perform impulse commands whenever they want (Source engine behavior) - Solokiller
	if( sv_new_impulse_check.value == 1 || bCanAttack )
	{
		ImpulseCommands();
	}

	if( m_pActiveItem )
	{
		if( !bCanAttack )
		{
			m_pActiveItem->WeaponBusyPostFrame();
		}
		else
		{
			m_pActiveItem->ItemPostFrame();
		}
	}
}

// ############ hu3lifezado ############ //
// [MODO COOP]
// Retorno a entidade para poder lidar com ela
CBaseEntity* CBasePlayer::GiveNamedItem(const char *pszName)
{
	CBaseEntity* pEntity = UTIL_CreateNamedEntity(pszName);

	if (!pEntity)
	{
		ALERT(at_console, "NULL Ent in GiveNamedItem \"%s\"!\n", pszName);
		return NULL;
	}
	pEntity->SetAbsOrigin(GetAbsOrigin());
	pEntity->GetSpawnFlags() |= SF_NORESPAWN;

	DispatchSpawn(pEntity->edict());
	pEntity->Touch(this);

	return pEntity;

}
// ############ //

//
// Returns the unique ID for the ammo, or -1 if error
//
int CBasePlayer::GiveAmmo( int iCount, const char *szName )
{
	if( !szName )
	{
		// no ammo.
		return -1;
	}

	auto pType = g_AmmoTypes.GetAmmoTypeByName( szName );

	//Invalid name. - Solokiller
	if( !pType )
		return -1;

	if( !g_pGameRules->CanHaveAmmo( this, szName ) )
	{
		// game rules say I can't have any more of this ammo type.
		return -1;
	}

	int i = 0;

	i = GetAmmoIndex( szName );

	if( i < 0 || i >= CAmmoTypes::MAX_AMMO_TYPES )
		return -1;

	int iAdd = min( iCount, pType->GetMaxCarry() - m_rgAmmo[ i ] );

	// ############ hu3lifezado ############ //
	//	if( iAdd < 1 )
	//	return i;
	// [MODO COOP]
	// Sou obrigado a processar municao negativa devido a um tipo de bug! Hack em LoadPlayerItems do CHalfLifeCoop.cpp
	if (g_pGameRules->IsCoOp())
	{
		m_rgAmmo[i] += iCount;
	}
	else // Codigo original
	{
		if (iAdd < 1)
			return i;

		m_rgAmmo[i] += iAdd;
	}
	// ############ //

	if( gmsgAmmoPickup )  // make sure the ammo messages have been linked first
	{
		// ############ hu3lifezado ############ //
		// [MODO COOP]
		// Sou obrigado a processar municao negativa devido a um tipo de bug! Hack em LoadPlayerItems do CHalfLifeCoop.cpp
		// Send the message that ammo has been picked up
		MESSAGE_BEGIN(MSG_ONE, gmsgAmmoPickup, NULL, this);
		WRITE_BYTE(GetAmmoIndex(szName));		// ammo ID
		if (g_pGameRules->IsCoOp())
			WRITE_BYTE(iCount);		// amount
		else
			WRITE_BYTE(iAdd);		// amount
		MESSAGE_END();
		// ############ //
	}

	return i;
}

// Called from UpdateClientData
// makes sure the client has all the necessary ammo info,  if values have changed
void CBasePlayer::SendAmmoUpdate()
{
	for( int i = 0; i < CAmmoTypes::MAX_AMMO_TYPES; i++ )
	{
		if( m_rgAmmo[ i ] != m_rgAmmoLast[ i ] )
		{
			m_rgAmmoLast[ i ] = m_rgAmmo[ i ];

			ASSERT( m_rgAmmo[ i ] >= 0 );
			ASSERT( m_rgAmmo[ i ] < 255 );

			// send "Ammo" update message
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, this );
			WRITE_BYTE( i );
			WRITE_BYTE( max( min( m_rgAmmo[ i ], 254 ), 0 ) );  // clamp the value to one byte
			MESSAGE_END();
		}
	}
}

void CBasePlayer::ResetAutoaim()
{
	if( m_vecAutoAim.x != 0 || m_vecAutoAim.y != 0 )
	{
		m_vecAutoAim = Vector( 0, 0, 0 );
		SET_CROSSHAIRANGLE( edict(), 0, 0 );
	}
	m_fOnTarget = false;
}

//=========================================================
// Autoaim
// set crosshair position to point to enemey
//=========================================================
Vector CBasePlayer::GetAutoaimVector( float flDelta )
{
	return GetAutoaimVectorFromPoint( GetGunPosition(), flDelta );
}

Vector CBasePlayer::GetAutoaimVectorFromPoint( const Vector& vecSrc, float flDelta )
{
	if( gSkillData.GetSkillLevel() == SKILL_HARD )
	{
		UTIL_MakeVectors( GetViewAngle() + GetPunchAngle() );
		return gpGlobals->v_forward;
	}

	float flDist = 8192;

	// always use non-sticky autoaim
	// UNDONE: use sever variable to chose!
	if( 1 || gSkillData.GetSkillLevel() == SKILL_MEDIUM )
	{
		m_vecAutoAim = Vector( 0, 0, 0 );
		// flDelta *= 0.5;
	}

	const bool bOldTargeting = m_fOnTarget;
	Vector angles = AutoaimDeflection( vecSrc, flDist, flDelta );

	// update ontarget if changed
	if( !g_pGameRules->AllowAutoTargetCrosshair() )
		m_fOnTarget = false;
	else if( bOldTargeting != m_fOnTarget )
	{
		m_pActiveItem->UpdateItemInfo();
	}

	if( angles.x > 180 )
		angles.x -= 360;
	if( angles.x < -180 )
		angles.x += 360;
	if( angles.y > 180 )
		angles.y -= 360;
	if( angles.y < -180 )
		angles.y += 360;

	if( angles.x > 25 )
		angles.x = 25;
	if( angles.x < -25 )
		angles.x = -25;
	if( angles.y > 12 )
		angles.y = 12;
	if( angles.y < -12 )
		angles.y = -12;


	// always use non-sticky autoaim
	// UNDONE: use sever variable to chose!
	if( 0 || gSkillData.GetSkillLevel() == SKILL_EASY )
	{
		m_vecAutoAim = m_vecAutoAim * 0.67 + angles * 0.33;
	}
	else
	{
		m_vecAutoAim = angles * 0.9;
	}

	// m_vecAutoAim = m_vecAutoAim * 0.99;

	// Don't send across network if sv_aim is 0
	if( g_psv_aim->value != 0 )
	{
		if( m_vecAutoAim.x != m_lastx ||
			m_vecAutoAim.y != m_lasty )
		{
			SET_CROSSHAIRANGLE( edict(), -m_vecAutoAim.x, m_vecAutoAim.y );

			m_lastx = m_vecAutoAim.x;
			m_lasty = m_vecAutoAim.y;
		}
	}

	// ALERT( at_console, "%f %f\n", angles.x, angles.y );

	UTIL_MakeVectors( GetViewAngle() + GetPunchAngle() + m_vecAutoAim );
	return gpGlobals->v_forward;
}

Vector CBasePlayer::AutoaimDeflection( const Vector &vecSrc, float flDist, float flDelta )
{
	edict_t		*pEdict = g_engfuncs.pfnPEntityOfEntIndex( 1 );
	CBaseEntity	*pEntity;
	float		bestdot;
	Vector		bestdir;
	edict_t		*bestent;
	TraceResult tr;

	if( g_psv_aim->value == 0 )
	{
		m_fOnTarget = false;
		return g_vecZero;
	}

	UTIL_MakeVectors( GetViewAngle() + GetPunchAngle() + m_vecAutoAim );

	// try all possible entities
	bestdir = gpGlobals->v_forward;
	bestdot = flDelta; // +- 10 degrees
	bestent = NULL;

	m_fOnTarget = false;

	UTIL_TraceLine( vecSrc, vecSrc + bestdir * flDist, dont_ignore_monsters, edict(), &tr );


	if( tr.pHit && tr.pHit->v.takedamage != DAMAGE_NO )
	{
		// don't look through water
		if( !( ( GetWaterLevel() != WATERLEVEL_HEAD && tr.pHit->v.waterlevel == WATERLEVEL_HEAD )
			   || ( GetWaterLevel() == WATERLEVEL_HEAD && tr.pHit->v.waterlevel == WATERLEVEL_DRY ) ) )
		{
			if( tr.pHit->v.takedamage == DAMAGE_AIM )
				m_fOnTarget = true;

			return m_vecAutoAim;
		}
	}

	for( int i = 1; i < gpGlobals->maxEntities; i++, pEdict++ )
	{
		Vector center;
		Vector dir;
		float dot;

		if( pEdict->free )	// Not in use
			continue;

		if( pEdict->v.takedamage != DAMAGE_AIM )
			continue;
		if( pEdict == edict() )
			continue;
		//		if (pev->team > 0 && pEdict->v.team == pev->team)
		//			continue;	// don't aim at teammate

		pEntity = Instance( pEdict );
		if( pEntity == NULL )
			continue;

		if( !g_pGameRules->ShouldAutoAim( this, pEntity ) )
			continue;


		if( !pEntity->IsAlive() )
			continue;

		// don't look through water
		if( ( GetWaterLevel() != WATERLEVEL_HEAD && pEntity->GetWaterLevel() == WATERLEVEL_HEAD )
			|| ( GetWaterLevel() == WATERLEVEL_HEAD && pEntity->GetWaterLevel() == WATERLEVEL_DRY ) )
			continue;

		center = pEntity->BodyTarget( vecSrc );

		dir = ( center - vecSrc ).Normalize();

		// make sure it's in front of the player
		if( DotProduct( dir, gpGlobals->v_forward ) < 0 )
			continue;

		dot = fabs( DotProduct( dir, gpGlobals->v_right ) )
			+ fabs( DotProduct( dir, gpGlobals->v_up ) ) * 0.5;

		// tweek for distance
		dot *= 1.0 + 0.2 * ( ( center - vecSrc ).Length() / flDist );

		if( dot > bestdot )
			continue;	// to far to turn

		UTIL_TraceLine( vecSrc, center, dont_ignore_monsters, edict(), &tr );
		if( tr.flFraction != 1.0 && tr.pHit != pEdict )
		{
			// ALERT( at_console, "hit %s, can't see %s\n", STRING( tr.pHit->v.classname ), STRING( pEdict->v.classname ) );
			continue;
		}

		// don't shoot at friends
		if( IRelationship( pEntity ) < 0 )
		{
			if( !pEntity->IsPlayer() && !g_pGameRules->IsDeathmatch() )
				// ALERT( at_console, "friend\n");
				continue;
		}

		// can shoot at this one
		bestdot = dot;
		bestent = pEdict;
		bestdir = dir;
	}

	if( bestent )
	{
		bestdir = UTIL_VecToAngles( bestdir );
		bestdir.x = -bestdir.x;
		bestdir = bestdir - GetViewAngle() - GetPunchAngle();

		if( bestent->v.takedamage == DAMAGE_AIM )
			m_fOnTarget = true;

		return bestdir;
	}

	return Vector( 0, 0, 0 );
}
