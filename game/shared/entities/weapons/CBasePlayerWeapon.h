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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CBASEPLAYERWEAPON_H
#define GAME_SHARED_ENTITIES_WEAPONS_CBASEPLAYERWEAPON_H

#include "WeaponsConst.h"

#include "CWeaponInfo.h"

/**
*	Items that the player has in their inventory that they can use
*/
class CBasePlayerWeapon : public CBaseAnimating
{
public:
	DECLARE_CLASS( CBasePlayerWeapon, CBaseAnimating );
	DECLARE_DATADESC();

	/**
	*	Constructor.
	*	@param iID Weapon ID.
	*/
	CBasePlayerWeapon( const int iID )
		: m_iId( iID )
	{
	}

	/**
	*	Must be called by subclasses to set up weapon info.
	*/
	void Precache() override;

	virtual void SetObjectCollisionBox() override;

	/**
	*	@return true if the item you want the item added to the player inventory
	*/
	virtual bool AddToPlayer( CBasePlayer *pPlayer );

	/**
	*	In this method call, "this" is the duplicate, pOriginal is the player's weapon.
	*	@return true if you want your duplicate removed from world
	*/
	virtual bool AddDuplicate( CBasePlayerWeapon *pOriginal );

	void DestroyItem();

	/**
	*	Default weapon touch
	*/
	void DefaultTouch( CBaseEntity *pOther );

	/**
	*	When an item is first spawned, this think is run to determine when the object has hit the ground.
	*/
	void FallThink();

	/**
	*	Make a weapon visible and tangible
	*/
	void Materialize();

	/**
	*	The weapon desires to become visible and tangible, if the game rules allow for it
	*/
	void AttemptToMaterialize();

	/**
	*	Copy a weapon
	*/
	CBaseEntity* Respawn() override;
	void FallInit();
	void CheckRespawn();

	/**
	*	@return true if you can add ammo to yourself when picked up
	*/
	virtual bool ExtractAmmo( CBasePlayerWeapon *pWeapon );

	/**
	*	@return true if you can add ammo to yourself when picked up
	*/
	virtual bool ExtractClipAmmo( CBasePlayerWeapon *pWeapon );

	/**
	*	@return true if you want to add yourself to the player
	*/
	virtual bool AddWeapon() { ExtractAmmo( this ); return true; }

	// generic "shared" ammo handlers
	bool AddPrimaryAmmo( int iCount, const char *szName, int iMaxClip );
	bool AddSecondaryAmmo( int iCount, const char *szName );

	/**
	*	Updates HUD state
	*	TODO: never implemented, and UpdateClientData seems to be doing the same thing. Remove? - Solokiller
	*/
	virtual void UpdateItemInfo() {}

	bool m_bPlayEmptySound;

	/**
	*	True when the gun is empty and the player is still holding down the attack key(s)
	*/
	bool m_bFireOnEmpty;

	virtual bool PlayEmptySound();
	virtual void ResetEmptySound();

	/**
	*	Sends the weapon animation to the owning client.
	*/
	virtual void SendWeaponAnim( int iAnim, int body = 0 );

	virtual bool CanDeploy() const;
	/**
	*	@return if deploy was successful
	*/
	virtual bool Deploy() { return true; }

	/**
	*	Can this weapon be put away right now?
	*/
	virtual bool CanHolster() { return true; } 

	virtual void Drop();
	virtual void Kill();
	virtual void AttachToPlayer( CBasePlayer *pPlayer );

	virtual bool IsUseable();
	bool DefaultDeploy( const char* const pszViewModel, const char* const pszWeaponModel, int iAnim, const char* const pszAnimExt, int body = 0 );
	bool DefaultReload( int iAnim, float fDelay, int body = 0 );

	/**
	*	Called each frame by the player PreThink
	*/
	virtual void ItemPreFrame() {}

	/**
	*	Called each frame by the player PostThink
	*/
	virtual void ItemPostFrame();

	//These methods are based on Source SDK methods, they are named differently to be more accurate in meaning. - Solokiller
	
	/**
	*	Called each frame by the player PreThink for inactive weapons.
	*/
	virtual void WeaponHolsterPreFrame() {}

	/**
	*	Called each frame by the player PostThink for the active weapon, if currently busy (awaiting timeout of attack delay).
	*/
	virtual void WeaponBusyPostFrame() {}

	//End Source SDK methods. - Solokiller

	// called by CBasePlayerWeapons ItemPostFrame()

	/**
	*	Do "+ATTACK"
	*/
	virtual void PrimaryAttack() {}

	/**
	*	Do "+ATTACK2"
	*/
	virtual void SecondaryAttack() {}

	/**
	*	Do "+RELOAD"
	*/
	virtual void Reload() {}

	/**
	*	Always called at beginning of ItemPostFrame. - Solokiller
	*/
	virtual void WeaponTick() {}

	/**
	*	Called when no buttons pressed
	*/
	virtual void WeaponIdle() {}

	/**
	*	Sends hud info to client dll, if things have changed
	*/
	virtual bool UpdateClientData( CBasePlayer *pPlayer );

	virtual void RetireWeapon();
	virtual bool ShouldWeaponIdle() { return false; }
	virtual void Holster();

	/**
	*	@return Whether this weapon is predicted or not.
	*/
	virtual bool IsPredicted() const
	{
#ifdef CLIENT_WEAPONS
		return true;
#else
		return false;
#endif
	}

	/**
	*	Decrement timers on predictable time variables.
	*	@param flTime Time to decrement by.
	*/
	virtual void DecrementTimers( float flTime ) {}

	void PrintState();

	float GetNextAttackDelay( float delay );

	/**
	*	Copies weapon data to the weapon_data_t instance.
	*	@param data Weapon data.
	*/
	virtual void GetWeaponData( weapon_data_t& data );

	/**
	*	Copies weapon data from the weapon_data_t instance.
	*	@param data Weapon data.
	*/
	virtual void SetWeaponData( const weapon_data_t& data );

	int PrimaryAmmoIndex() const;
	int SecondaryAmmoIndex() const;

	/**
	*	@return 0 to MAX_ITEMS_SLOTS, used in hud.
	*/
	int			iItemSlot() const { return m_pWeaponInfo->GetBucket(); }
	int			iItemPosition() const { return m_pWeaponInfo->GetPosition(); }
	const char	*pszAmmo1() const { return m_pWeaponInfo->GetPrimaryAmmoName(); }
	const char	*pszAmmo2() const { return m_pWeaponInfo->GetSecondaryAmmoName(); }
	const char	*pszName() const { return m_pWeaponInfo->GetWeaponName(); }
	int			iMaxClip() const { return m_pWeaponInfo->GetMaxMagazine(); }
	int			iWeight() const { return m_pWeaponInfo->GetWeight(); }
	int			iFlags() const { return m_pWeaponInfo->GetFlags(); }

	const CWeaponInfo* GetWeaponInfo() const { return m_pWeaponInfo; }

	/**
	*	The player whose inventory we are currently in, if any.
	*/
	CBasePlayer* m_pPlayer;

	/**
	*	Next weapon in our bucket of the owning player's inventory, if any.
	*/
	CBasePlayerWeapon* m_pNext;

	const int m_iId; // WEAPON_???

	/**
	*	Soonest time ItemPostFrame will call PrimaryAttack
	*/
	float m_flNextPrimaryAttack;

	/**
	*	Soonest time ItemPostFrame will call SecondaryAttack
	*/
	float m_flNextSecondaryAttack;

	/**
	*	Soonest time ItemPostFrame will call WeaponIdle
	*/
	float m_flTimeWeaponIdle;

	/**
	*	Number of shots left in the primary weapon clip, -1 it not used
	*/
	int m_iClip;

	/**
	*	The last version of m_iClip sent to hud dll
	*	Used on the client to store the predicted clip amount.
	*/
	int m_iClientClip;

	/**
	*	The last version of the weapon state sent to hud dll (is current weapon, is on target)
	*/
	WpnOnTargetState m_iClientWeaponState;

	/**
	*	Are we in the middle of a reload?
	*/
	bool m_fInReload;

	/**
	*	How much ammo you get when you pick up this weapon as placed by a level designer.
	*/
	int m_iDefaultAmmo;

	// hle time creep vars
	float m_flPrevPrimaryAttack;
	float m_flLastFireTime;

private:
	const CWeaponInfo* m_pWeaponInfo = nullptr;
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CBASEPLAYERWEAPON_H