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
#ifndef GAME_SHARED_ENTITIES_CBASEFORWARD_H
#define GAME_SHARED_ENTITIES_CBASEFORWARD_H

#include "Platform.h"

#include "DefaultClassifications.h"

/**
*	@file
*	Forward declarations for entity classes and related types.
*/

class SINGLE_INHERITANCE CBaseEntity;

class CBaseMonster;
class CSquadMonster;

/**
*	Set this bit on guns and stuff that should never respawn.
*/
#define	SF_NORESPAWN	( 1 << 30 )

/**
*	People gib if their health is <= this at the time of death.
*/
#define	GIB_HEALTH_VALUE	-30

/**
*	Tracers fire every 4 bullets
*/
#define TRACER_FREQ 4

/**
*	Max number of nodes available for a path.
*/
#define MAX_PATH_SIZE 10

/**
*	These are caps bits to indicate what an object's capabilities (currently used for save/restore and level transitions).
*/
enum FCapability
{
	/**
	*	Not used.
	*/
	FCAP_CUSTOMSAVE			= 0x00000001,

	/**
	*	Should transfer between transitions.
	*/
	FCAP_ACROSS_TRANSITION	= 0x00000002,

	/**
	*	Spawn after restore.
	*/
	FCAP_MUST_SPAWN			= 0x00000004,

	/**
	*	Don't save this.
	*/
	FCAP_DONT_SAVE			= 0x80000000,

	/**
	*	Can be used by the player.
	*/
	FCAP_IMPULSE_USE		= 0x00000008,

	/**
	*	Can be used by the player.
	*/
	FCAP_CONTINUOUS_USE		= 0x00000010,

	/**
	*	Can be used by the player.
	*/
	FCAP_ONOFF_USE			= 0x00000020,

	/**
	*	Player sends +/- 1 when using (currently only tracktrains).
	*/
	FCAP_DIRECTIONAL_USE	= 0x00000040,

	/**
	*	Can be used to "master" other entities (like multisource).
	*/
	FCAP_MASTER				= 0x00000080,

	/**
	*	UNDONE: This will ignore transition volumes (trigger_transition), but not the PVS!!!
	*	ALWAYS goes across transitions.
	*/
	FCAP_FORCE_TRANSITION	= 0x00000080,
};

enum USE_TYPE
{
	USE_OFF		= 0,
	USE_ON		= 1,
	USE_SET		= 2,
	USE_TOGGLE	= 3
};

/**
*	Possible values for entvars_t::fixangle
*
*	Solokiller
*/
enum FixAngleMode
{
	/**
	*	Do nothing
	*/
	FIXANGLE_NO			= 0,

	/**
	*	Set view angles to pev->angles
	*/
	FIXANGLE_SET		= 1,

	/**
	*	Add avelocity yaw value to view angles
	*	avelocity yaw value is set to 0 after this
	*/
	FIXANGLE_ADD_AVEL	= 2
};

/**
*	When calling Killed(), a value that governs gib behavior is expected to be one of these three values.
*/
enum GibAction
{
	/**
	*	Gib if entity was overkilled
	*/
	GIB_NORMAL = 0,

	/**
	*	Never gib, no matter how much death damage is done ( freezing, etc )
	*/
	GIB_NEVER = 1,

	/**
	*	Always gib ( Houndeye Shock, Barnacle Bite )
	*/
	GIB_ALWAYS = 2,
};

// All monsters need this data
enum BloodColor
{
	DONT_BLEED			= -1,
	BLOOD_COLOR_RED		= 247,
	BLOOD_COLOR_YELLOW	= 195,
	BLOOD_COLOR_GREEN	= BLOOD_COLOR_YELLOW,
};

typedef void ( CBaseEntity::*BASEPTR )();
typedef void ( CBaseEntity::*ENTITYFUNCPTR )( CBaseEntity* pOther );
typedef void ( CBaseEntity::*USEPTR )( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value );

extern void FireTargets( const char *targetName, CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value );

#endif //GAME_SHARED_ENTITIES_CBASEFORWARD_H
