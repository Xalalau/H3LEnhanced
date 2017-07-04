// ##############################
// HU3-LIFE REGRAS DO COOP
// ##############################

#ifndef GAME_SERVER_GAMERULES_CHALFLIFECOOP_H
#define GAME_SERVER_GAMERULES_CHALFLIFECOOP_H

#include "CGameRules.h"
#include "CHu3LifeCoop.h"

class CBaseHalfLifeCoop : public CGameRules
{
public:
	CBaseHalfLifeCoop();

	void OnCreate() override;

	virtual void Think() override;
	cvar_t* GetSkillCvar( const skilldata_t& skillData, const char* pszSkillCvarName ) override;
	virtual bool IsAllowedToSpawn( CBaseEntity *pEntity ) override;
	virtual bool FAllowFlashlight() const override;

	virtual bool FShouldSwitchWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pWeapon ) override;
	virtual bool GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pCurrentWeapon ) override;

// Functions to verify the single/multiplayer status of a game
	virtual bool IsMultiplayer() const override;
	virtual bool IsDeathmatch() const override;
	virtual bool IsCoOp() const override;

// Client connection/disconnection
	virtual bool ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ CCONNECT_REJECT_REASON_SIZE ] ) override;
	virtual void InitHUD( CBasePlayer *pl ) override;
	virtual void ClientDisconnected( edict_t *pClient ) override;
	virtual void UpdateGameMode( CBasePlayer *pPlayer ) override; 

// Client damage rules
	virtual float FlPlayerFallDamage( CBasePlayer *pPlayer ) override;
	virtual bool  FPlayerCanTakeDamage( CBasePlayer *pPlayer, const CTakeDamageInfo& info ) override;

// Client spawn/respawn control
	void FixPlayerCrouchStuck(edict_t *pPlayer);
	virtual void PlayerSpawn( CBasePlayer *pPlayer ) override;
	int SetPlayerName(CBaseEntity *pPlayer);
	void LoadPlayerItems(CBasePlayer *pPlayer, playerCoopSaveRestore* CoopPlyData);
	virtual void ChangeLevelCoop(CBaseEntity* pLandmark, char* m_szLandmarkName, char* st_szNextMap);
	void SavePlayerItems(CBasePlayer *pPlayer, playerCoopSaveRestore* CoopPlyData);
	void SavePlayerItemsAux(CBasePlayer *pPlayer, playerCoopSaveRestore* CoopPlyData, CBasePlayerWeapon *pWeapon, int j);
	virtual void PlayerThink( CBasePlayer *pPlayer ) override;

	virtual bool FPlayerCanRespawn( CBasePlayer *pPlayer ) override;
	virtual float FlPlayerSpawnTime( CBasePlayer *pPlayer ) override;
	virtual CBaseEntity* GetPlayerSpawnSpot( CBasePlayer* pPlayer ) override;

	virtual bool AllowAutoTargetCrosshair() override;
	virtual bool ClientCommand( CBasePlayer *pPlayer, const char *pcmd ) override;

// Client kills/scoring
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled ) override;
	virtual void PlayerKilled( CBasePlayer* pVictim, const CTakeDamageInfo& info ) override;
	virtual void DeathNotice( CBasePlayer* pVictim, const CTakeDamageInfo& info ) override;

// Weapon retrieval
	virtual void PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pWeapon ) override;
	virtual bool CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerWeapon *pWeapon ) override;

// Weapon spawn/respawn control
	virtual int WeaponShouldRespawn( CBasePlayerWeapon *pWeapon ) override;
	virtual float FlWeaponRespawnTime( CBasePlayerWeapon *pWeapon ) override;
	virtual float FlWeaponTryRespawn( CBasePlayerWeapon *pWeapon ) override;
	virtual Vector VecWeaponRespawnSpot( CBasePlayerWeapon *pWeapon ) override;

// Item retrieval
	virtual bool CanHaveItem( CBasePlayer *pPlayer, CItem *pItem ) override;
	virtual void PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem ) override;

// Item spawn/respawn control
	virtual int ItemShouldRespawn( CItem *pItem ) override;
	virtual float FlItemRespawnTime( CItem *pItem ) override;
	virtual Vector VecItemRespawnSpot( CItem *pItem ) override;

// Ammo retrieval
	virtual void PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount ) override;

// Ammo spawn/respawn control
	virtual int AmmoShouldRespawn( CBasePlayerAmmo *pAmmo ) override;
	virtual float FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo ) override;
	virtual Vector VecAmmoRespawnSpot( CBasePlayerAmmo *pAmmo ) override;

// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime() override;
	virtual float FlHEVChargerRechargeTime() override;

// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons( CBasePlayer *pPlayer ) override;

// What happens to a dead player's ammo	
	virtual int DeadPlayerAmmo( CBasePlayer *pPlayer ) override;

// Teamplay stuff	
	virtual const char *GetTeamID( CBaseEntity *pEntity ) override { return ""; }
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget ) override;

	virtual bool PlayTextureSounds() override { return false; }
	virtual bool PlayFootstepSounds( CBasePlayer *pl, float fvol ) override;

// Monsters
	virtual bool FAllowMonsters() const override;

	virtual void EndMultiplayerGame() override { GoToIntermission(); }

protected:
	virtual void ChangeLevel();
	virtual void GoToIntermission();
	float m_flIntermissionEndTime2;
	bool m_bEndIntermissionButtonHit2 = false;
	void SendMOTDToClient(CBasePlayer* pPlayer);
};

#endif //GAME_SERVER_GAMERULES_CHALFLIFECOOP_H
