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
//
// CHalfLifeRules.cpp
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"CBasePlayer.h"
#include	"Weapons.h"
#include	"CHalfLifeRules.h"
#include	"Skill.h"
#include	"entities/items/CItem.h"

extern DLL_GLOBAL CGameRules	*g_pGameRules;
extern DLL_GLOBAL bool	g_fGameOver;

//=========================================================
//=========================================================
CHalfLifeRules::CHalfLifeRules( void )
{
}

//=========================================================
//=========================================================
void CHalfLifeRules::Think ( void )
{
}

//=========================================================
//=========================================================
bool CHalfLifeRules::IsMultiplayer() const
{
	return false;
}

//=========================================================
//=========================================================
bool CHalfLifeRules::IsDeathmatch() const
{
	return false;
}

//=========================================================
//=========================================================
bool CHalfLifeRules::IsCoOp() const
{
	return false;
}


//=========================================================
//=========================================================
bool CHalfLifeRules::FShouldSwitchWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pWeapon )
{
	if ( !pPlayer->m_pActiveItem )
	{
		// player doesn't have an active item!
		return true;
	}

	if ( !pPlayer->m_pActiveItem->CanHolster() )
	{
		return false;
	}

	return true;
}

//=========================================================
//=========================================================
bool CHalfLifeRules::GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pCurrentWeapon )
{
	return false;
}

//=========================================================
//=========================================================
bool CHalfLifeRules::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ CCONNECT_REJECT_REASON_SIZE ] )
{
	return true;
}

void CHalfLifeRules :: InitHUD( CBasePlayer *pl )
{
}

//=========================================================
//=========================================================
void CHalfLifeRules :: ClientDisconnected( edict_t *pClient )
{
}

//=========================================================
//=========================================================
float CHalfLifeRules::FlPlayerFallDamage( CBasePlayer *pPlayer )
{
	// subtract off the speed at which a player is allowed to fall without being hurt,
	// so damage will be based on speed beyond that, not the entire fall
	pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
	return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
}

//=========================================================
//=========================================================
void CHalfLifeRules :: PlayerSpawn( CBasePlayer *pPlayer )
{
	// ############ hu3lifezado ############ //
	// Delay para executar checagem de comandos preconfigurados
	checkCommandsDelay = gpGlobals->time + 0.1;
	// ############ //
}

//=========================================================
//=========================================================
bool CHalfLifeRules::AllowAutoTargetCrosshair()
{
	return ( gSkillData.GetSkillLevel() == SKILL_EASY );
}

//=========================================================
//=========================================================
void CHalfLifeRules :: PlayerThink( CBasePlayer *pPlayer )
{
	// ############ hu3lifezado ############ //
	// Executo estados especiais nas entidades
	// NOTA: Isso era feito em PlayerSpawn() e funcionava quando usavamos o comando
	//       "map algum_mapa" para testarmos os niveis, mas em loads entre fases
	//       a funcao infelizmente rodava em um momento anterior ao que deveria e
	//       portanto nao tinha efeito. Fui obrigado a vir para o Think().
	if (gpGlobals->time > checkCommandsDelay && checkCommandsDelay != -1)
	{
		edict_t		*pEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
		CBaseEntity *pEntity;
		char * sp_remove = (char*)CVAR_GET_STRING("sp_remove");
		int j, count_sp_remove = 0;

		// Contar a quantidade de entidades a remover
		char* tok = strtok(sp_remove, ";");
		while (tok != NULL) {
			count_sp_remove++;
			tok = strtok(NULL, ";");
		}

		if (count_sp_remove > 0)
		{
			for (int i = 1; i < gpGlobals->maxEntities; i++, pEdict++)
			{
				if (!pEdict)
					break;

				pEntity = CBaseEntity::Instance(pEdict);
				if (!pEntity)
					continue; // Essa verificacao em Util.cpp dentro de UTIL_MonstersInSphere() usa continue ao inves de break

				// Remover a entidade se ela estiver marcada como nao apropriada para o sp
				tok = sp_remove;
				for (j = 0; j < count_sp_remove; ++j) {
					if (strcmp(pEntity->GetTargetname(), tok) == 0)
					{
						pEntity->SUB_Remove();

						break;
					}
					tok += strlen(tok) + 1;
					tok += strspn(tok, ";");
				}
			}
		}

		checkCommandsDelay = -1;
	}
	// ############ //
}


//=========================================================
//=========================================================
bool CHalfLifeRules::FPlayerCanRespawn( CBasePlayer *pPlayer )
{
	return true;
}

//=========================================================
//=========================================================
float CHalfLifeRules :: FlPlayerSpawnTime( CBasePlayer *pPlayer )
{
	return gpGlobals->time;//now!
}

//=========================================================
// IPointsForKill - how many points awarded to anyone
// that kills this player?
//=========================================================
int CHalfLifeRules :: IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	return 1;
}

//=========================================================
// PlayerKilled - someone/something killed this player
//=========================================================
void CHalfLifeRules::PlayerKilled( CBasePlayer* pVictim, const CTakeDamageInfo& info )
{
}

//=========================================================
// Deathnotice
//=========================================================
void CHalfLifeRules::DeathNotice( CBasePlayer* pVictim, const CTakeDamageInfo& info )
{
}

//=========================================================
// PlayerGotWeapon - player has grabbed a weapon that was
// sitting in the world
//=========================================================
void CHalfLifeRules :: PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pWeapon )
{
}

//=========================================================
// FlWeaponRespawnTime - what is the time in the future
// at which this weapon may spawn?
//=========================================================
float CHalfLifeRules :: FlWeaponRespawnTime( CBasePlayerWeapon *pWeapon )
{
	return -1;
}

//=========================================================
// FlWeaponRespawnTime - Returns 0 if the weapon can respawn 
// now,  otherwise it returns the time at which it can try
// to spawn again.
//=========================================================
float CHalfLifeRules :: FlWeaponTryRespawn( CBasePlayerWeapon *pWeapon )
{
	return 0;
}

//=========================================================
// VecWeaponRespawnSpot - where should this weapon spawn?
// Some game variations may choose to randomize spawn locations
//=========================================================
Vector CHalfLifeRules :: VecWeaponRespawnSpot( CBasePlayerWeapon *pWeapon )
{
	return pWeapon->GetAbsOrigin();
}

//=========================================================
// WeaponShouldRespawn - any conditions inhibiting the
// respawning of this weapon?
//=========================================================
int CHalfLifeRules :: WeaponShouldRespawn( CBasePlayerWeapon *pWeapon )
{
	return GR_WEAPON_RESPAWN_NO;
}

//=========================================================
//=========================================================
bool CHalfLifeRules::CanHaveItem( CBasePlayer *pPlayer, CItem *pItem )
{
	return true;
}

//=========================================================
//=========================================================
void CHalfLifeRules::PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem )
{
}

//=========================================================
//=========================================================
int CHalfLifeRules::ItemShouldRespawn( CItem *pItem )
{
	return GR_ITEM_RESPAWN_NO;
}


//=========================================================
// At what time in the future may this Item respawn?
//=========================================================
float CHalfLifeRules::FlItemRespawnTime( CItem *pItem )
{
	return -1;
}

//=========================================================
// Where should this item respawn?
// Some game variations may choose to randomize spawn locations
//=========================================================
Vector CHalfLifeRules::VecItemRespawnSpot( CItem *pItem )
{
	return pItem->GetAbsOrigin();
}

//=========================================================
//=========================================================
bool CHalfLifeRules::IsAllowedToSpawn( CBaseEntity *pEntity )
{
	return true;
}

//=========================================================
//=========================================================
void CHalfLifeRules::PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount )
{
}

//=========================================================
//=========================================================
int CHalfLifeRules::AmmoShouldRespawn( CBasePlayerAmmo *pAmmo )
{
	return GR_AMMO_RESPAWN_NO;
}

//=========================================================
//=========================================================
float CHalfLifeRules::FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo )
{
	return -1;
}

//=========================================================
//=========================================================
Vector CHalfLifeRules::VecAmmoRespawnSpot( CBasePlayerAmmo *pAmmo )
{
	return pAmmo->GetAbsOrigin();
}

//=========================================================
//=========================================================
float CHalfLifeRules::FlHealthChargerRechargeTime( void )
{
	return 0;// don't recharge
}

//=========================================================
//=========================================================
int CHalfLifeRules::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

//=========================================================
//=========================================================
int CHalfLifeRules::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}

//=========================================================
//=========================================================
int CHalfLifeRules::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	// why would a single player in half life need this? 
	return GR_NOTTEAMMATE;
}

//=========================================================
//=========================================================
bool CHalfLifeRules::FAllowMonsters() const
{
	return true;
}
