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
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"

// ############ hu3lifezado ############ //
// Entidade generica da mira em terceira pessoa
#ifndef CLIENT_DLL
#include "entities/CHu3XSpot.h"
#endif
// ############ //

#include "CMP5.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"
#include "entities/CSoundEnt.h"
#include "gamerules/GameRules.h"

LINK_ENTITY_TO_CLASS( weapon_mp5, CMP5 );
LINK_ENTITY_TO_CLASS( weapon_9mmAR, CMP5 );

CMP5::CMP5()
	: BaseClass( WEAPON_MP5 )
{
}

//=========================================================
//=========================================================
void CMP5::Spawn( )
{
	SetClassname("weapon_9mmAR"); // hack to allow for old names
	Precache( );
	SetModel( "models/w_9mmAR.mdl");

	FallInit();// get ready to fall down.
}


void CMP5::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/v_9mmAR.mdl");
	PRECACHE_MODEL("models/w_9mmAR.mdl");
	PRECACHE_MODEL("models/p_9mmAR.mdl");

	m_iShell = PRECACHE_MODEL ("models/shell.mdl");// brass shellTE_MODEL

	PRECACHE_MODEL("models/grenade.mdl");	// grenade

	PRECACHE_MODEL("models/w_9mmARclip.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND("items/clipinsert1.wav");
	PRECACHE_SOUND("items/cliprelease1.wav");

	PRECACHE_SOUND ("weapons/hks1.wav");// H to the K
	PRECACHE_SOUND ("weapons/hks2.wav");// H to the K
	PRECACHE_SOUND ("weapons/hks3.wav");// H to the K

	PRECACHE_SOUND( "weapons/glauncher.wav" );
	PRECACHE_SOUND( "weapons/glauncher2.wav" );

	PRECACHE_SOUND ("weapons/357_cock1.wav");

	m_usMP5 = PRECACHE_EVENT( 1, "events/mp5.sc" );
	m_usMP52 = PRECACHE_EVENT( 1, "events/mp52.sc" );
}

// ############ hu3lifezado ############ //
// Criacao e modificacao do ponto de mira de terceira pessoa
void CMP5::ItemPreFrame(void)
{
#ifndef CLIENT_DLL
	if (m_pPlayer->hu3_cam_crosshair == 0)
	{
		if (m_pLaser)
		{
			m_pLaser->RemoveSpot(m_pLaser);
			m_pLaser = nullptr;
		}
	}
	else
	{
		if (!m_pLaser)
			m_pLaser = CHu3XSpot::CreateSpot();	

		m_pLaser->UpdateSpot(m_pPlayer, m_pLaser);
	}
#endif	
}

// Recolha da arma
void CMP5::Holster()
{
#ifndef CLIENT_DLL
	if (m_pLaser)
	{
		m_pLaser->RemoveSpot(m_pLaser);
		m_pLaser = nullptr;
	}
#endif
}
// ############ //

bool CMP5::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return true;
	}
	return false;
}

bool CMP5::Deploy()
{
	return DefaultDeploy( "models/v_9mmAR.mdl", "models/p_9mmAR.mdl", MP5_DEPLOY, "mp5" );
}


void CMP5::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->GetWaterLevel() == WATERLEVEL_HEAD )
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	m_pPlayer->GetEffects().AddFlags( EF_MUZZLEFLASH );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecDir;

	if ( !bIsMultiplayer() )
	{
		// optimized multiplayer. Widened to make it easier to hit a moving player
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, 8192, BULLET_PLAYER_MP5, 2, 0, m_pPlayer, m_pPlayer->random_seed );
	}
	else
	{
		// single player spread
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5, 2, 0, m_pPlayer, m_pPlayer->random_seed );
	}

  int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usMP5, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, 0);

	m_flNextPrimaryAttack = GetNextAttackDelay(0.1);

	if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}



void CMP5::SecondaryAttack( void )
{
	// don't fire underwater
	if (m_pPlayer->GetWaterLevel() == WATERLEVEL_HEAD )
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_pPlayer->m_rgAmmo[ SecondaryAmmoIndex() ] == 0)
	{
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_pPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;
	m_pPlayer->m_flStopExtraSoundTime = UTIL_WeaponTimeBase() + 0.2;
			
	m_pPlayer->m_rgAmmo[ SecondaryAmmoIndex() ]--;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

 	UTIL_MakeVectors( m_pPlayer->GetViewAngle() + m_pPlayer->GetPunchAngle() );

	// we don't add in player velocity anymore.
	CGrenade::ShootContact( m_pPlayer, 
							m_pPlayer->GetAbsOrigin() + m_pPlayer->GetViewOffset() + gpGlobals->v_forward * 16,
							gpGlobals->v_forward * 800 );

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT( flags, m_pPlayer->edict(), m_usMP52 );
	
	m_flNextPrimaryAttack = GetNextAttackDelay(1);
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5;// idle pretty soon after shooting.

	if (!m_pPlayer->m_rgAmmo[ SecondaryAmmoIndex() ])
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, 0);
}

void CMP5::Reload( void )
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] <= 0 )
		return;

	// ############ hu3lifezado ############ //
	// Remocao da mira em terceira pessoa
#ifndef CLIENT_DLL
	m_pLaser->RemoveSpot(m_pLaser);
	m_pLaser = nullptr;
#endif
	// ############ //

	DefaultReload( MP5_RELOAD, 1.5 );
}


void CMP5::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = MP5_LONGIDLE;	
		break;
	
	default:
	case 1:
		iAnim = MP5_IDLE1;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.
}

bool CMP5::IsUseable()
{
	//Can be used if the player has AR grenades. - Solokiller
	return BaseClass::IsUseable() || m_pPlayer->m_rgAmmo[ SecondaryAmmoIndex() ] > 0;
}

class CMP5AmmoClip : public CBasePlayerAmmo
{
public:
	DECLARE_CLASS( CMP5AmmoClip, CBasePlayerAmmo );

	void Spawn( void ) override
	{ 
		Precache( );
		SetModel( "models/w_9mmARclip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void ) override
	{
		PRECACHE_MODEL ("models/w_9mmARclip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo( CBaseEntity *pOther ) override
	{ 
		return UTIL_GiveAmmoToPlayer( this, pOther, AMMO_MP5CLIP_GIVE, "9mm" );
	}
};
LINK_ENTITY_TO_CLASS( ammo_mp5clip, CMP5AmmoClip );
LINK_ENTITY_TO_CLASS( ammo_9mmAR, CMP5AmmoClip );



class CMP5Chainammo : public CBasePlayerAmmo
{
public:
	DECLARE_CLASS( CMP5Chainammo, CBasePlayerAmmo );

	void Spawn( void ) override
	{ 
		Precache( );
		SetModel( "models/w_chainammo.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void ) override
	{
		PRECACHE_MODEL ("models/w_chainammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo( CBaseEntity *pOther ) override
	{ 
		return UTIL_GiveAmmoToPlayer( this, pOther, AMMO_CHAINBOX_GIVE, "9mm" );
	}
};
LINK_ENTITY_TO_CLASS( ammo_9mmbox, CMP5Chainammo );


class CMP5AmmoGrenade : public CBasePlayerAmmo
{
public:
	DECLARE_CLASS( CMP5AmmoGrenade, CBasePlayerAmmo );

	void Spawn( void ) override
	{ 
		Precache( );
		SetModel( "models/w_ARgrenade.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void ) override
	{
		PRECACHE_MODEL ("models/w_ARgrenade.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo( CBaseEntity *pOther ) override
	{ 
		return UTIL_GiveAmmoToPlayer( this, pOther, AMMO_M203BOX_GIVE, "ARgrenades" );
	}
};
LINK_ENTITY_TO_CLASS( ammo_mp5grenades, CMP5AmmoGrenade );
LINK_ENTITY_TO_CLASS( ammo_ARgrenades, CMP5AmmoGrenade );


















