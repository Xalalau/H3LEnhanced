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
#include "CRpg.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"
#include "gamerules/GameRules.h"

#ifdef SERVER_DLL
#include "entities/CLaserSpot.h"
#include "entities/CRpgRocket.h"
#else
#include "hud.h"
#include "cl_util.h"
#include "pm_defs.h"
#include "renderer/view.h"
#include "com_weapons.h"
#endif

#ifdef SERVER_DLL
BEGIN_DATADESC( CRpg )
	DEFINE_FIELD( m_bSpotActive, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_cActiveRockets, FIELD_INTEGER ),
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS( weapon_rpg, CRpg );

CRpg::CRpg()
	: BaseClass( WEAPON_RPG )
{
}

void CRpg::Reload( void )
{
	if ( m_iClip == 1 )
	{
		// don't bother with any of this if don't need to reload.
		return;
	}

	if ( m_pPlayer->GetAmmoCountByID( PrimaryAmmoIndex() ) <= 0 )
		return;

	// because the RPG waits to autoreload when no missiles are active while  the LTD is on, the
	// weapons code is constantly calling into this function, but is often denied because 
	// a) missiles are in flight, but the LTD is on
	// or
	// b) player is totally out of ammo and has nothing to switch to, and should be allowed to
	//    shine the designator around
	//
	// Set the next attack time into the future so that WeaponIdle will get called more often
	// than reload, allowing the RPG LTD to be updated
	
	m_flNextPrimaryAttack = GetNextAttackDelay(0.5);

	if ( m_cActiveRockets && m_bSpotActive )
	{
		// no reloading when there are active missiles tracking the designator.
		// ward off future autoreload attempts by setting next attack time into the future for a bit. 
		return;
	}

#ifdef CLIENT_DLL
	if( m_pSpot && m_bSpotActive )
	{
		m_pSpot->callback = []( TEMPENTITY* pEntity, float frametime, float currenttime )
		{
			if( currenttime >= pEntity->entity.curstate.fuser1 )
			{
				pEntity->entity.curstate.effects &= EF_NODRAW;

				pEntity->callback = nullptr;

				pEntity->flags &= ~FTENT_NOMODEL;
			}
		};

		m_pSpot->entity.curstate.fuser1 = gpGlobals->time + 2.1;
		//Prevent it from being added. - Solokiller
		m_pSpot->flags |= FTENT_NOMODEL;
	}
#else
	if ( m_pSpot && m_bSpotActive )
	{
		m_pSpot->Suspend( 2.1 );
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.1;
	}
#endif

	//Used to be uninitialized. Shouldn't have caused any problems, but still. - Solokiller
	bool bResult = false;

	if ( m_iClip == 0 )
		bResult = DefaultReload( RPG_RELOAD, 2 );
	
	if ( bResult )
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	
}

void CRpg::Spawn( )
{
	Precache( );

	SetModel( "models/w_rpg.mdl");
	m_bSpotActive = true;

	if ( bIsMultiplayer() )
	{
		// more default ammo in multiplay. 
		m_iDefaultAmmo *= 2;
	}

	FallInit();// get ready to fall down.
}


void CRpg::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/w_rpg.mdl");
	PRECACHE_MODEL("models/v_rpg.mdl");
	PRECACHE_MODEL("models/p_rpg.mdl");

	PRECACHE_SOUND("items/9mmclip1.wav");

	UTIL_PrecacheOther( "laser_spot" );
	UTIL_PrecacheOther( "rpg_rocket" );

	PRECACHE_SOUND("weapons/rocketfire1.wav");
	PRECACHE_SOUND("weapons/glauncher.wav"); // alternative fire sound

	m_usRpg = PRECACHE_EVENT ( 1, "events/rpg.sc" );
}

bool CRpg::AddToPlayer( CBasePlayer *pPlayer )
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

bool CRpg::Deploy()
{
	if ( m_iClip == 0 )
	{
		return DefaultDeploy( "models/v_rpg.mdl", "models/p_rpg.mdl", RPG_DRAW_UL, "rpg" );
	}

	return DefaultDeploy( "models/v_rpg.mdl", "models/p_rpg.mdl", RPG_DRAW1, "rpg" );
}


bool CRpg::CanHolster()
{
	if ( m_bSpotActive && m_cActiveRockets )
	{
		// can't put away while guiding a missile.
		return false;
	}

	return true;
}

void CRpg::Holster()
{
	m_fInReload = false;// cancel any reload in progress.

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	
	SendWeaponAnim( RPG_HOLSTER1 );

#ifdef CLIENT_DLL
	if( m_pSpot )
	{
		//Die as soon as possible. - Solokiller
		m_pSpot->die = 0;
		//Make invisible. - Solokiller
		m_pSpot->entity.curstate.effects |= EF_NODRAW;
		m_pSpot = nullptr;
	}
#else
	if (m_pSpot)
	{
		UTIL_Remove( m_pSpot );
		m_pSpot = nullptr;
	}
#endif

}



void CRpg::PrimaryAttack()
{
	if ( m_iClip )
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

#ifndef CLIENT_DLL
		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		UTIL_MakeVectors( m_pPlayer->GetViewAngle() );
		Vector vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -8;
		
		CRpgRocket *pRocket = CRpgRocket::CreateRpgRocket( vecSrc, m_pPlayer->GetViewAngle(), m_pPlayer, this );

		UTIL_MakeVectors( m_pPlayer->GetViewAngle() );// RpgRocket::Create stomps on globals, so remake.
		pRocket->SetAbsVelocity( pRocket->GetAbsVelocity() + gpGlobals->v_forward * DotProduct( m_pPlayer->GetAbsVelocity(), gpGlobals->v_forward ) );
#endif

		// firing RPG no longer turns on the designator. ALT fire is a toggle switch for the LTD.
		// Ken signed up for this as a global change (sjb)

		int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

		PLAYBACK_EVENT( flags, m_pPlayer->edict(), m_usRpg );

		m_iClip--; 
				
		m_flNextPrimaryAttack = GetNextAttackDelay(1.5);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;

		ResetEmptySound();
	}
	else
	{
		PlayEmptySound( );
	}
	UpdateSpot( );
}


void CRpg::SecondaryAttack()
{
	m_bSpotActive = !m_bSpotActive;

#ifdef CLIENT_DLL
	if( !m_bSpotActive && m_pSpot )
	{
		//Die as soon as possible. - Solokiller
		m_pSpot->die = 0;
		//Make invisible. - Solokiller
		m_pSpot->entity.curstate.effects |= EF_NODRAW;
		m_pSpot = nullptr;
}
#else
	if (!m_bSpotActive && m_pSpot)
	{
		UTIL_Remove( m_pSpot );
		m_pSpot = nullptr;
	}
#endif

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.2;
}


void CRpg::WeaponIdle( void )
{
	UpdateSpot( );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ])
	{
		ResetEmptySound();

		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
		if (flRand <= 0.75 || m_bSpotActive )
		{
			if ( m_iClip == 0 )
				iAnim = RPG_IDLE_UL;
			else
				iAnim = RPG_IDLE;

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 90.0 / 15.0;
		}
		else
		{
			if ( m_iClip == 0 )
				iAnim = RPG_FIDGET_UL;
			else
				iAnim = RPG_FIDGET;

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;
		}

		SendWeaponAnim( iAnim );
	}
	else
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1;
	}
}

void CRpg::UpdateSpot( void )
{
#ifdef CLIENT_DLL
	if( m_bSpotActive )
	{
		if( !m_pSpot )
		{
			//Don't locally predict if weapon prediction is disabled. - Solokiller
			if( !cl_lw->value )
				return;

			m_pSpot = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh( g_vecZero, gEngfuncs.CL_LoadModel( "sprites/laserdot.spr", nullptr ) );

			if( !m_pSpot )
				return;

			m_pSpot->entity.curstate.rendermode		= kRenderGlow;
			m_pSpot->entity.curstate.renderfx		= kRenderFxNoDissipation;
			m_pSpot->entity.curstate.renderamt		= 255;

			//Never die on its own. - Solokiller
			m_pSpot->die = 1e10;

			//Prevent it from being culled. - Solokiller
			//Enable callbacks.
			m_pSpot->flags |= FTENT_PERSIST | FTENT_CLIENTCUSTOM;

			m_pSpot->entity.curstate.scale = 1.0;
		}

		if( !cl_lw->value )
		{
			m_pSpot->die = 0;
			m_pSpot->entity.curstate.effects = EF_NODRAW;
			m_pSpot = nullptr;
			return;
		}
	}
#else
	if ( m_bSpotActive )
	{
		if (!m_pSpot)
		{
			m_pSpot = CLaserSpot::CreateSpot();

			m_pSpot->GetFlags() |= FL_SKIPLOCALHOST;

			m_pSpot->SetOwner( m_pPlayer );
		}

		UTIL_MakeVectors( m_pPlayer->GetViewAngle() );
		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = gpGlobals->v_forward;

		TraceResult tr;
		UTIL_TraceLine ( vecSrc, vecSrc + vecAiming * 8192, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr );
		
		m_pSpot->SetAbsOrigin( tr.vecEndPos );
	}
#endif
}


class CRpgAmmo : public CBasePlayerAmmo
{
public:
	DECLARE_CLASS( CRpgAmmo, CBasePlayerAmmo );

	void Spawn( void ) override
	{ 
		Precache( );
		SetModel( "models/w_rpgammo.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void ) override
	{
		PRECACHE_MODEL ("models/w_rpgammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo( CBaseEntity *pOther ) override
	{ 
		int iGive;

		if ( bIsMultiplayer() )
		{
			// hand out more ammo per rocket in multiplayer.
			iGive = AMMO_RPGCLIP_GIVE * 2;
		}
		else
		{
			iGive = AMMO_RPGCLIP_GIVE;
		}

		return UTIL_GiveAmmoToPlayer( this, pOther, iGive, "rockets" );
	}
};
LINK_ENTITY_TO_CLASS( ammo_rpgclip, CRpgAmmo );