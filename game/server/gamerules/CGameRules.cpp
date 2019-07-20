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
//=========================================================
// GameRules.cpp
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"
#include "Weapons.h"
#include "entities/CCorpse.h"
#include "gamerules/GameRules.h"
#include "Skill.h"
#include "Server.h"

#include "entities/spawnpoints/CBaseSpawnPoint.h"

extern DLL_GLOBAL bool	g_fGameOver;

void CGameRules::OnCreate()
{
	RefreshSkillData();
}

//=========================================================
//=========================================================
bool CGameRules::CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName )
{
	if ( pszAmmoName )
	{
		auto pType = g_AmmoTypes.GetAmmoTypeByName( pszAmmoName );

		if ( pType )
		{
			if ( pPlayer->GetAmmoCountByID( pType->GetID() ) < pType->GetMaxCarry() )
			{
				// player has room for more of this type of ammo
				return true;
			}
		}
	}

	return false;
}

//=========================================================
//=========================================================
CBaseEntity* CGameRules::GetPlayerSpawnSpot( CBasePlayer* pPlayer )
{
	CBaseEntity* pSpawnSpot = EntSelectSpawnPoint( pPlayer );

	//This should never be null.
	ASSERT( pSpawnSpot );

	pPlayer->SetAbsOrigin( pSpawnSpot->GetAbsOrigin() + Vector( 0, 0, 1 ) );
	pPlayer->SetViewAngle( g_vecZero );
	pPlayer->SetAbsVelocity( g_vecZero );
	pPlayer->SetAbsAngles( pSpawnSpot->GetAbsAngles() );
	pPlayer->SetPunchAngle( g_vecZero );
	pPlayer->SetFixAngleMode( FIXANGLE_SET );
	
	return pSpawnSpot;
}

//=========================================================
//=========================================================
bool CGameRules::CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerWeapon *pWeapon )
{
	// only living players can have items
	if ( pPlayer->GetDeadFlag() != DEAD_NO )
		return false;

	if ( pWeapon->pszAmmo1() )
	{
		if ( !CanHaveAmmo( pPlayer, pWeapon->pszAmmo1() ) )
		{
			// we can't carry anymore ammo for this gun. We can only 
			// have the gun if we aren't already carrying one of this type
			if ( pPlayer->HasPlayerItem( pWeapon ) )
			{
				return false;
			}
		}
	}
	else
	{
		// weapon doesn't use ammo, don't take another if you already have it.
		if ( pPlayer->HasPlayerItem( pWeapon ) )
		{
			return false;
		}
	}

	// note: will fall through to here if GetItemInfo doesn't fill the struct!
	return true;
}

//=========================================================
// load the SkillData struct with the proper values based on the skill level.
//=========================================================
void CGameRules::RefreshSkillData ( void )
{
	gSkillData.RefreshSkillData();
}

cvar_t* CGameRules::GetSkillCvar( const skilldata_t& skillData, const char* pszSkillCvarName )
{
	return skilldata_t::GetSkillCvar( pszSkillCvarName, skillData.GetSkillLevel() );
}

void CGameRules::PlayerRespawn( CBasePlayer* pPlayer, const bool bCopyCorpse )
{
	if( gpGlobals->coop || gpGlobals->deathmatch )
	{
		if( bCopyCorpse )
		{
			// make a copy of the dead body for appearances sake
			CopyToBodyQue( pPlayer );
		}

		// respawn player
		pPlayer->Spawn();
	}
	else
	{       // restart the entire server
		SERVER_COMMAND( "reload\n" );
	}
}

// ############ hu3lifezado ############ //
// [MODO COOP]
// Uso vazio e generalizado do changelevel do modo coop
// Preciso disso para acessar essa funcao externamente em qualquer arquivo do HL1
void CGameRules::ChangeLevelCoopToogle()
{
	return;
}
// [MODO COOP]
// Desativar a fisica dos jogadores em qualquer lugar
void CGameRules::DisablePhysics(CBaseEntity * pEntity)
{
	return;
}
// ############ //