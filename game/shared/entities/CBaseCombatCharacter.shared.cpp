/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CBaseCombatCharacter.h"

BEGIN_DATADESC( CBaseCombatCharacter )
	DEFINE_FIELD( m_bitsDamageType, FIELD_INTEGER ),
	DEFINE_ARRAY( m_rgbTimeBasedDamage, FIELD_CHARACTER, CDMG_TIMEBASED ),
END_DATADESC()

// take health
float CBaseCombatCharacter::GiveHealth( float flHealth, int bitsDamageType )
{
	if( GetTakeDamageMode() == DAMAGE_NO )
		return 0;

	// clear out any damage types we healed.
	// UNDONE: generic health should not heal any
	// UNDONE: time-based damage

	//TODO: if this method is being used to hurt, shouldn't it retain time-based damage? - Solokiller
	m_bitsDamageType &= ~( bitsDamageType & ~DMG_TIMEBASED );

	return CBaseEntity::GiveHealth( flHealth, bitsDamageType );
}