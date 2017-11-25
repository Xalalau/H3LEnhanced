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

#include "NPCs/Monsters.h"

#include "entities/CBaseCombatCharacter.h"

bool CBaseCombatCharacter::ShouldFadeOnDeath() const
{
	// if flagged to fade out or I have an owner (I came from a monster spawner)
	if( GetSpawnFlags().Any( SF_MONSTER_FADECORPSE ) || !FNullEnt( GetOwner() ) )
		return true;

	return false;
}
