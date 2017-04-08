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
#include "CWeaponInfoCache.h"

#include "CMap.h"

#include "pm_shared.h"

#include "Weather.h"

#include "gamerules/GameRules.h"

extern DLL_GLOBAL bool gDisplayTitle;

/*
=========================================================
UpdateClientData

resends any changed player HUD info to the client.
Called every frame by PlayerPreThink
Also called at start of demo recording and playback by
ForceClientDllUpdate to ensure the demo gets messages
reflecting all of the HUD state info.
=========================================================
*/
void CBasePlayer::UpdateClientData()
{
	if( m_bNeedsNewConnectTime )
	{
		m_bNeedsNewConnectTime = false;

		m_flConnectTime = gpGlobals->time;
	}

	if( !m_bWeaponValidationReceived && m_flConnectTime + WEAPON_VALIDATION_GRACE_TIME < gpGlobals->time )
	{
		//If the client didn't send the message in time, drop the client. - Solokiller

		//Set it to true to avoid running this multiple times. - Solokiller
		m_bWeaponValidationReceived = true;

		UTIL_LogPrintf( "Player \"%s\" didn't send weapon validation in time, disconnecting\n", GetNetName() );

		if( !IS_DEDICATED_SERVER() )
		{
			//Listen server hosts usually don't have logging enabled, so echo to console unconditionally for them. - Solokiller
			UTIL_ServerPrintf( "Player \"%s\" didn't send weapon validation in time, disconnecting\n", GetNetName() );
		}

		if( IS_DEDICATED_SERVER() || entindex() != 1 )
		{
			SERVER_COMMAND( UTIL_VarArgs( "kick \"%s\" \"No weapon validation received\"\n", GetNetName() ) );
		}
		else
		{
			//The local player can't be kicked, so terminate the session instead - Solokiller
			CLIENT_COMMAND( edict(), "disconnect\n" );
		}
	}

	//The engine will not call ClientPutInServer after transitions, so we'll have to catch this event every map change. - Solokiller
	if( !m_bSentInitData )
	{
		m_bSentInitData = true;

		//Update Hud colors. - Solokiller
		CMap::GetInstance()->SendHudColors( this, true );
	}

	if( m_fInitHUD )
	{
		m_fInitHUD = false;
		gInitHUD = false;

		MESSAGE_BEGIN( MSG_ONE, gmsgResetHUD, NULL, this );
			WRITE_BYTE( 0 );
		MESSAGE_END();

		if( !m_fGameHUDInitialized )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgInitHUD, NULL, this );
			MESSAGE_END();

			g_pGameRules->InitHUD( this );

			UMSG_SendGameState( *this );

			m_fGameHUDInitialized = true;

			m_iObserverLastMode = OBS_ROAMING;

			if( g_pGameRules->IsMultiplayer() )
			{
				FireTargets( "game_playerjoin", this, this, USE_TOGGLE, 0 );
			}
		}

		FireTargets( "game_playerspawn", this, this, USE_TOGGLE, 0 );

		InitStatusBar();

		SendWeatherUpdate();
	}

	if( m_iHideHUD != m_iClientHideHUD )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHideWeapon, NULL, this );
			WRITE_BYTE( m_iHideHUD );
		MESSAGE_END();

		m_iClientHideHUD = m_iHideHUD;
	}

	if( m_iFOV != m_iClientFOV )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgSetFOV, NULL, this );
			WRITE_BYTE( m_iFOV );
		MESSAGE_END();

		// cache FOV change at end of function, so weapon updates can see that FOV has changed
	}

	// HACKHACK -- send the message to display the game title
	if( gDisplayTitle )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgShowGameTitle, NULL, this );
			WRITE_BYTE( 0 );
		MESSAGE_END();
		gDisplayTitle = false;
	}

	if( pev->health != m_iClientHealth )
	{
		int iHealth = clamp( static_cast<int>( pev->health ), 0, 255 );  // make sure that no negative health values are sent
		if( pev->health > 0.0f && pev->health <= 1.0f )
			iHealth = 1;

		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgHealth, NULL, this );
			WRITE_BYTE( iHealth );
		MESSAGE_END();

		m_iClientHealth = pev->health;
	}


	if( pev->armorvalue != m_iClientBattery )
	{
		m_iClientBattery = pev->armorvalue;

		ASSERT( gmsgBattery > 0 );
		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgBattery, NULL, this );
			WRITE_SHORT( ( int ) pev->armorvalue );
		MESSAGE_END();
	}

	if( pev->dmg_take || pev->dmg_save || m_bitsHUDDamage != m_bitsDamageType )
	{
		// Comes from inside me if not set
		Vector damageOrigin = GetAbsOrigin();
		// send "damage" message
		// causes screen to flash, and pain compass to show direction of damage
		edict_t *other = pev->dmg_inflictor;
		if( other )
		{
			CBaseEntity *pEntity = CBaseEntity::Instance( other );
			if( pEntity )
				damageOrigin = pEntity->Center();
		}

		// only send down damage type that have hud art
		int visibleDamageBits = m_bitsDamageType & DMG_SHOWNHUD;

		MESSAGE_BEGIN( MSG_ONE, gmsgDamage, NULL, this );
			WRITE_BYTE( pev->dmg_save );
			WRITE_BYTE( pev->dmg_take );
			WRITE_LONG( visibleDamageBits );
			WRITE_COORD( damageOrigin.x );
			WRITE_COORD( damageOrigin.y );
			WRITE_COORD( damageOrigin.z );
		MESSAGE_END();

		//TODO: both of these can probably be removed. - Solokiller
		pev->dmg_take = 0;
		pev->dmg_save = 0;
		m_bitsHUDDamage = m_bitsDamageType;

		// Clear off non-time-based damage indicators
		m_bitsDamageType &= DMG_TIMEBASED;
	}

	// Update Flashlight
	if( ( m_flFlashLightTime ) && ( m_flFlashLightTime <= gpGlobals->time ) )
	{
		if( FlashlightIsOn() )
		{
			if( m_iFlashBattery )
			{
				m_flFlashLightTime = FLASH_DRAIN_TIME + gpGlobals->time;
				m_iFlashBattery--;

				if( !m_iFlashBattery )
					FlashlightTurnOff();
			}
		}
		else
		{
			if( m_iFlashBattery < 100 )
			{
				m_flFlashLightTime = FLASH_CHARGE_TIME + gpGlobals->time;
				m_iFlashBattery++;
			}
			else
				m_flFlashLightTime = 0;
		}

		MESSAGE_BEGIN( MSG_ONE, gmsgFlashBattery, NULL, this );
			WRITE_BYTE( m_iFlashBattery );
		MESSAGE_END();
	}

	if( m_iTrain & TRAIN_NEW )
	{
		ASSERT( gmsgTrain > 0 );
		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgTrain, NULL, this );
			WRITE_BYTE( m_iTrain & 0xF );
		MESSAGE_END();

		m_iTrain &= ~TRAIN_NEW;
	}

	SendAmmoUpdate();

	// Update all the items
	for( int i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		if( m_rgpPlayerItems[ i ] )  // each item updates it's successors
			m_rgpPlayerItems[ i ]->UpdateClientData( this );
	}

	// Cache and client weapon change
	m_pClientActiveItem = m_pActiveItem;
	m_iClientFOV = m_iFOV;

	// Update Status Bar
	if( m_flNextSBarUpdateTime < gpGlobals->time )
	{
		UpdateStatusBar();
		m_flNextSBarUpdateTime = gpGlobals->time + 0.2;
	}
}

/*
===============
ForceClientDllUpdate

When recording a demo, we need to have the server tell us the entire client state
so that the client side .dll can behave correctly.
Reset stuff so that the state is transmitted.
===============
*/
void CBasePlayer::ForceClientDllUpdate()
{
	m_iClientHealth = -1;
	m_iClientBattery = -1;
	m_iTrain |= TRAIN_NEW;	// Force new train message.
	m_fWeapon = false;		// Force weapon send
	m_fInitHUD = true;		// Force HUD gmsgResetHUD message

	// Now force all the necessary messages to be sent.
	UpdateClientData();
}

void CBasePlayer::SendWeatherUpdate()
{
	WeatherType::WeatherType type = WeatherType::NONE;

	if( UTIL_FindEntityByClassname( nullptr, "env_rain" ) )
	{
		type = WeatherType::RAIN;
	}
	else if( UTIL_FindEntityByClassname( nullptr, "env_snow" ) )
	{
		type = WeatherType::SNOW;
	}

	if( type != WeatherType::NONE )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgReceiveW, nullptr, this );
			WRITE_BYTE( type );
		MESSAGE_END();
	}
}