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
#ifndef GAME_SERVER_GAMERULES_CGAMERULES_H
#define GAME_SERVER_GAMERULES_CGAMERULES_H

#include "CTakeDamageInfo.h"

//#include "Weapons.h"
//#include "items.h"
class CBaseEntity;
class CBasePlayerWeapon;
class CBasePlayer;
class CItem;
class CBasePlayerAmmo;
struct skilldata_t;

struct edict_t;

/**
*	Weapon respawning return codes
*/
enum
{	
	GR_NONE = 0,
	
	GR_WEAPON_RESPAWN_YES,
	GR_WEAPON_RESPAWN_NO,
	
	GR_AMMO_RESPAWN_YES,
	GR_AMMO_RESPAWN_NO,
	
	GR_ITEM_RESPAWN_YES,
	GR_ITEM_RESPAWN_NO,

	GR_PLR_DROP_GUN_ALL,
	GR_PLR_DROP_GUN_ACTIVE,
	GR_PLR_DROP_GUN_NO,

	GR_PLR_DROP_AMMO_ALL,
	GR_PLR_DROP_AMMO_ACTIVE,
	GR_PLR_DROP_AMMO_NO,
};

/**
*	Player relationship return codes
*/
enum
{
	GR_NOTTEAMMATE = 0,
	GR_TEAMMATE,
	GR_ENEMY,
	GR_ALLY,
	GR_NEUTRAL,
};

/**
*	This class is used to decide how to handle certain actions, and performs certain actions based on the active rules.
*	An instance is created when a map starts.
*	Gamerules exist for singleplayer, deathmatch, teamplay, and (in the future) co-op and CTF.
*	TODO: co-op and CTF gamerules.
*/
class CGameRules
{
public:
	virtual ~CGameRules() = 0;

	virtual void OnCreate();

	/**
	*	Fill skill data struct with proper values
	*/
	virtual void RefreshSkillData();

	/**
	*	Gets a skill cvar pointer by name. Allows gamerules to override the cvar that is returned.
	*	@param skillData Skill data instance whose skill level will be used.
	*	@param pszSkillCvarName Name of the cvar to get.
	*	TODO: replace skill data with skill level.
	*/
	virtual cvar_t* GetSkillCvar( const skilldata_t& skillData, const char* pszSkillCvarName );

	/**
	*	Runs every server frame, should handle any timer tasks, periodic events, etc.
	*/
	virtual void Think() = 0;

	/**
	*	Can this item spawn (eg monsters don't spawn in deathmatch).
	*/
	virtual bool IsAllowedToSpawn( CBaseEntity *pEntity ) = 0;

	/**
	*	Are players allowed to switch on their flashlight?
	*/
	virtual bool FAllowFlashlight() const = 0;

	/**
	*	Should the player switch to this weapon?
	*/
	virtual bool FShouldSwitchWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pWeapon ) = 0;

	/**
	*	I can't use this weapon anymore, get me the next best one.
	*/
	virtual bool GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pCurrentWeapon ) = 0;

// Functions to verify the single/multiplayer status of a game

	/**
	*	Is this a multiplayer game? (either coop or deathmatch)
	*/
	virtual bool IsMultiplayer() const = 0;

	/**
	*	Is this a deathmatch game?
	*/
	virtual bool IsDeathmatch() const = 0;

	/**
	*	Is this deathmatch game being played with team rules?
	*/
	virtual bool IsTeamplay() const { return false; }

	/**
	*	Is this a coop game?
	*/
	virtual bool IsCoOp() const = 0;

	/**
	*	This is the game name that gets seen in the server browser
	*/
	virtual const char *GetGameDescription() const { return "Spirit of Half-Life"; }
	
// Client connection/disconnection

	/**
	*	A client just connected to the server (player hasn't spawned yet)
	*	If ClientConnected returns false, the connection is rejected and the user is provided the reason specified in
	*	szRejectReason
	*	Only the client's name and remote address are provided to the dll for verification.
	*/
	virtual bool ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ CCONNECT_REJECT_REASON_SIZE ] ) = 0;

	/**
	*	The client dll is ready for updating
	*/
	virtual void InitHUD( CBasePlayer *pl ) = 0;

	/**
	*	A client just disconnected from the server
	*/
	virtual void ClientDisconnected( edict_t *pClient ) = 0;

	/**
	*	The client needs to be informed of the current game mode
	*/
	virtual void UpdateGameMode( CBasePlayer *pPlayer ) {}

// Client damage rules

	/**
	*	This client just hit the ground after a fall. How much damage?
	*/
	virtual float FlPlayerFallDamage( CBasePlayer *pPlayer ) = 0;

	/**
	*	Can this player take damage from this attacker?
	*/
	virtual bool FPlayerCanTakeDamage( CBasePlayer *pPlayer, const CTakeDamageInfo& info ) { return true; }

	/**
	*	Should this player autoaim at the given target?
	*/
	virtual bool ShouldAutoAim( CBasePlayer *pPlayer, CBaseEntity* pTarget ) { return true; }

// Client spawn/respawn control

	/**
	*	Called by CBasePlayer::Spawn just before releasing player into the game
	*/
	virtual void PlayerSpawn( CBasePlayer *pPlayer ) = 0;

	/**
	*	Handles player respawn. Called by PlayerDeathThink.
	*	@param pPlayer Player to respawn.
	*	@param bCopyCorpse Whether to copy the player's corpse in multiplayer games.
	*/
	virtual void PlayerRespawn( CBasePlayer* pPlayer, const bool bCopyCorpse );

	/**
	*	Called by CBasePlayer::PreThink every frame, before physics are run and after keys are accepted
	*/
	virtual void PlayerThink( CBasePlayer *pPlayer ) = 0;

	/**
	*	Is this player allowed to respawn now?
	*/
	virtual bool FPlayerCanRespawn( CBasePlayer *pPlayer ) = 0;

	/**
	*	When in the future will this player be able to spawn?
	*/
	virtual float FlPlayerSpawnTime( CBasePlayer *pPlayer ) = 0;

	/**
	*	Place this player on their spawnspot and face them the proper direction.
	*/
	virtual CBaseEntity* GetPlayerSpawnSpot( CBasePlayer* pPlayer );

	/**
	*	Can players use auto aim?
	*/
	virtual bool AllowAutoTargetCrosshair() { return true; }

	/**
	*	Handles the user commands;  returns true if command handled properly
	*/
	virtual bool ClientCommand( CBasePlayer *pPlayer, const char *pcmd ) { return false; }

	/**
	*	The player has changed userinfo;  can change it now
	*/
	virtual void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer ) {}

// Client kills/scoring

	/**
	*	How many points do I award whoever kills this player?
	*/
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled ) = 0;

	/**
	*	Called each time a player dies
	*/
	virtual void PlayerKilled( CBasePlayer* pVictim, const CTakeDamageInfo& info ) = 0;

	/**
	*	Call this from within a GameRules class to report an obituary.
	*/
	virtual void DeathNotice( CBasePlayer* pVictim, const CTakeDamageInfo& info ) = 0;

// Weapon retrieval

	/**
	*	The player is touching a CBasePlayerWeapon, do I give it to him?
	*/
	virtual bool CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerWeapon *pWeapon );

	/**
	*	Called each time a player picks up a weapon from the ground
	*/
	virtual void PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pWeapon ) = 0;

// Weapon spawn/respawn control

	/**
	*	Should this weapon respawn?
	*/
	virtual int WeaponShouldRespawn( CBasePlayerWeapon *pWeapon ) = 0;

	/**
	*	When may this weapon respawn?
	*/
	virtual float FlWeaponRespawnTime( CBasePlayerWeapon *pWeapon ) = 0;

	/**
	*	Can i respawn now, and if not, when should i try again?
	*/
	virtual float FlWeaponTryRespawn( CBasePlayerWeapon *pWeapon ) = 0;

	/**
	*	Where in the world should this weapon respawn?
	*/
	virtual Vector VecWeaponRespawnSpot( CBasePlayerWeapon *pWeapon ) = 0;

// Item retrieval

	/**
	*	Is this player allowed to take this item?
	*/
	virtual bool CanHaveItem( CBasePlayer *pPlayer, CItem *pItem ) = 0;

	/**
	*	Call each time a player picks up an item (battery, healthkit, longjump)
	*/
	virtual void PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem ) = 0;

// Item spawn/respawn control
	/**
	*	Should this item respawn?
	*/
	virtual int ItemShouldRespawn( CItem *pItem ) = 0;

	/**
	*	When may this item respawn?
	*/
	virtual float FlItemRespawnTime( CItem *pItem ) = 0;

	/**
	*	Where in the world should this item respawn?
	*/
	virtual Vector VecItemRespawnSpot( CItem *pItem ) = 0;

// Ammo retrieval

	/**
	*	Can this player take more of this ammo?
	*/
	virtual bool CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName );

	/**
	*	Called each time a player picks up some ammo in the world
	*/
	virtual void PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount ) = 0;

// Ammo spawn/respawn control

	/**
	*	Should this ammo item respawn?
	*/
	virtual int AmmoShouldRespawn( CBasePlayerAmmo *pAmmo ) = 0;

	/**
	*	When should this ammo item respawn?
	*/
	virtual float FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo ) = 0;

	/**
	*	Where in the world should this ammo item respawn?
	*	By default, everything spawns at the ammo origin.
	*/
	virtual Vector VecAmmoRespawnSpot( CBasePlayerAmmo *pAmmo ) = 0;

// Healthcharger respawn control

	/**
	*	How long until a depleted HealthCharger recharges itself?
	*/
	virtual float FlHealthChargerRechargeTime() = 0;

	/**
	*	How long until a depleted HealthCharger recharges itself?
	*/
	virtual float FlHEVChargerRechargeTime() { return 0; }

// What happens to a dead player's weapons

	/**
	*	What do I do with a player's weapons when he's killed?
	*/
	virtual int DeadPlayerWeapons( CBasePlayer *pPlayer ) = 0;

// What happens to a dead player's ammo	

	/**
	*	Do I drop ammo when the player dies? How much?
	*/
	virtual int DeadPlayerAmmo( CBasePlayer *pPlayer ) = 0;

// Teamplay stuff

	/**
	*	What team is this entity on?
	*/
	virtual const char *GetTeamID( CBaseEntity *pEntity ) = 0;

	/**
	*	What is the player's relationship with this entity?
	*/
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget ) = 0;
	virtual int GetTeamIndex( const char *pTeamName ) { return -1; }
	virtual const char *GetIndexedTeamName( int teamIndex ) { return ""; }
	virtual bool IsValidTeam( const char *pTeamName ) { return true; }
	virtual void ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, const bool bKill, const bool bGib ) {}
	virtual const char *SetDefaultPlayerTeam( CBasePlayer *pPlayer ) { return ""; }

// Sounds
	virtual bool PlayTextureSounds() { return true; }
	virtual bool PlayFootstepSounds( CBasePlayer *pl, float fvol ) { return true; }

// Monsters
	/**
	*	Are monsters allowed?
	*/
	virtual bool FAllowMonsters() const = 0;

	/**
	*	Immediately end a multiplayer game
	*/
	virtual void EndMultiplayerGame() {}
};

inline CGameRules::~CGameRules()
{
}

#endif //GAME_SERVER_GAMERULES_CGAMERULES_H
