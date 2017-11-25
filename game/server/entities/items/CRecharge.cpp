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
/*

===== h_battery.cpp ========================================================

  battery-related code

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "SaveRestore.h"
#include "Skill.h"
#include "gamerules/GameRules.h"
#include "WeaponsConst.h"

#include "CRecharge.h"

BEGIN_DATADESC( CRecharge )
	DEFINE_FIELD( m_flNextCharge, FIELD_TIME ),
	DEFINE_FIELD( m_iReactivate, FIELD_INTEGER),
	DEFINE_FIELD( m_iJuice, FIELD_INTEGER),
	DEFINE_FIELD( m_iOn, FIELD_INTEGER),
	DEFINE_FIELD( m_flSoundTime, FIELD_TIME ),
	DEFINE_THINKFUNC( Off ),
	DEFINE_THINKFUNC( Recharge ),
END_DATADESC()

LINK_ENTITY_TO_CLASS(func_recharge, CRecharge);

void CRecharge::KeyValue( KeyValueData *pkvd )
{
	if (	FStrEq(pkvd->szKeyName, "style") ||
				FStrEq(pkvd->szKeyName, "height") ||
				FStrEq(pkvd->szKeyName, "value1") ||
				FStrEq(pkvd->szKeyName, "value2") ||
				FStrEq(pkvd->szKeyName, "value3"))
	{
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "dmdelay"))
	{
		m_iReactivate = atoi(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CRecharge::Spawn()
{
	Precache( );

	SetSolidType( SOLID_BSP );
	SetMoveType( MOVETYPE_PUSH );

	SetAbsOrigin( GetAbsOrigin());		// set size and link into world
	SetSize( GetRelMin(), GetRelMax() );
	SetModel( GetModelName() );
	m_iJuice = gSkillData.GetSuitChargerCapacity();
	SetFrame( 0 );
}

void CRecharge::Precache()
{
	PRECACHE_SOUND("items/suitcharge1.wav");
	PRECACHE_SOUND("items/suitchargeno1.wav");
	PRECACHE_SOUND("items/suitchargeok1.wav");
}


void CRecharge::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ 
	// if it's not a player, ignore
	if( !pActivator->IsPlayer() )
		return;

	// if there is no juice left, turn it off
	if (m_iJuice <= 0)
	{
		SetFrame( 1 );
		Off();
	}

	// if the player doesn't have the suit, or there is no juice left, make the deny noise
	if( ( m_iJuice <= 0 ) || ( !( pActivator->GetWeapons().Any( 1 << WEAPON_SUIT ) ) ) )
	{
		if (m_flSoundTime <= gpGlobals->time)
		{
			m_flSoundTime = gpGlobals->time + 0.62;
			EMIT_SOUND( this, CHAN_ITEM, "items/suitchargeno1.wav", 0.85, ATTN_NORM );
		}
		return;
	}

	SetNextThink( GetLastThink() + 0.25 );
	SetThink(&CRecharge::Off);

	// Time to recharge yet?

	if (m_flNextCharge >= gpGlobals->time)
		return;

	// Make sure that we have a caller
	if (!pActivator)
		return;

	m_hActivator = pActivator;

	//only recharge the player

	if (!m_hActivator->IsPlayer() )
		return;
	
	// Play the on sound or the looping charging sound
	if (!m_iOn)
	{
		m_iOn++;
		EMIT_SOUND( this, CHAN_ITEM, "items/suitchargeok1.wav", 0.85, ATTN_NORM );
		m_flSoundTime = 0.56 + gpGlobals->time;
	}
	if ((m_iOn == 1) && (m_flSoundTime <= gpGlobals->time))
	{
		m_iOn++;
		EMIT_SOUND( this, CHAN_STATIC, "items/suitcharge1.wav", 0.85, ATTN_NORM );
	}

	// ############ hu3lifezado ############ //
	// Os numeros estavam hardcoded em "100" aqui, troquei para MAX_NORMAL_BATTERY
	// charge the player
	//TODO: constant for this is used by battery, make configurable - Solokiller
	if (m_hActivator->GetArmorAmount() < MAX_NORMAL_BATTERY)
	{
		m_iJuice--;
		m_hActivator->SetArmorAmount( m_hActivator->GetArmorAmount() + 1 );

		if (m_hActivator->GetArmorAmount() > MAX_NORMAL_BATTERY)
			m_hActivator->SetArmorAmount( MAX_NORMAL_BATTERY );
	}
	// ############ //

	// govern the rate of charge
	m_flNextCharge = gpGlobals->time + 0.1;
}

void CRecharge::Recharge(void)
{
	m_iJuice = gSkillData.GetSuitChargerCapacity();
	SetFrame( 0 );
	SetThink( &CRecharge::SUB_DoNothing );
}

void CRecharge::Off(void)
{
	// Stop looping sound.
	if (m_iOn > 1)
		STOP_SOUND( this, CHAN_STATIC, "items/suitcharge1.wav" );

	m_iOn = 0;

	if ((!m_iJuice) &&  ( ( m_iReactivate = g_pGameRules->FlHEVChargerRechargeTime() ) > 0) )
	{
		SetNextThink( GetLastThink() + m_iReactivate );
		SetThink(&CRecharge::Recharge);
	}
	else
		SetThink( &CRecharge::SUB_DoNothing );
}
