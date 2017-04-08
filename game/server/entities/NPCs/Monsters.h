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
#ifndef GAME_SERVER_ENTITIES_NPCS_MONSTERS_H
#define GAME_SERVER_ENTITIES_NPCS_MONSTERS_H

#include "Skill.h"
#include "Relationship.h"

/*

===== monsters.h ========================================================

  Header file for monster-related utility code

*/

struct Schedule_t;

// CHECKLOCALMOVE result types 
enum LocalMove
{
	/**
	*	Move is not possible.
	*/
	LOCALMOVE_INVALID					= 0,

	/**
	*	Move is not possible, don't try to triangulate.
	*/
	LOCALMOVE_INVALID_DONT_TRIANGULATE	= 1,

	/**
	*	Move is possible.
	*/
	LOCALMOVE_VALID						= 2,
};

/**
*	Hit Group standards
*/
enum HitGroup
{
	HITGROUP_GENERIC	= 0,
	HITGROUP_HEAD		= 1,
	HITGROUP_CHEST		= 2,
	HITGROUP_STOMACH	= 3,
	HITGROUP_LEFTARM	= 4,	
	HITGROUP_RIGHTARM	= 5,
	HITGROUP_LEFTLEG	= 6,
	HITGROUP_RIGHTLEG	= 7,
};


/**
*	Monster Spawnflags
*/
enum MonsterSpawnFlag
{
	/**
	*	Spawnflag that makes monsters wait until player can see them before attacking.
	*/
	SF_MONSTER_WAIT_TILL_SEEN		= 1,

	/**
	*	No idle noises from this monster.
	*/
	SF_MONSTER_GAG					= 2,

	/**
	*	Monster is blocked by monsterclip.
	*/
	SF_MONSTER_HITMONSTERCLIP		= 4,

//										8

/**
*	Monster won't attack anyone, no one will attack him.
*/
	SF_MONSTER_PRISONER				= 16,

//										32

//										64

	/**
	*	Spawnflag that makes monsters wait to check for attacking until the script is done or they've been attacked.
	*/
	SF_MONSTER_WAIT_FOR_SCRIPT		= 128,

	/**
	*	This is a predisaster scientist or barney. Influences how they speak.
	*/
	SF_MONSTER_PREDISASTER			= 256,

	//TODO: move & rename - Solokiller
	/**
	*	Fade out corpse after death.
	*/
	SF_MONSTER_FADECORPSE			= 512,

	/**
	*	If set, the monster naturally falls to the ground instead of being dropped onto the ground immediately.
	*/
	SF_MONSTER_FALL_TO_GROUND		= 0x80000000,
};

// specialty spawnflags
enum
{
	/**
	*	Don't attack the player unless provoked.
	*/
	SF_MONSTER_WAIT_UNTIL_PROVOKED	= 64,
};



// MoveToOrigin stuff
enum
{
	/**
	*	When this far away from moveGoal, start turning to face next goal.
	*/
	MOVE_START_TURN_DIST	= 64,

	/**
	*	If a monster can't step this far, it is stuck.
	*/
	MOVE_STUCK_DIST			= 32,
};


// MoveToOrigin stuff
enum MoveToOrigin
{
	/**
	*	Normal move in the direction monster is facing.
	*/
	MOVE_NORMAL		= 0,

	/**
	*	Moves in direction specified, no matter which way monster is facing.
	*/
	MOVE_STRAFE		= 1
};

// spawn flags 256 and above are already taken by the engine
/**
*	Moves the given entity towards the destination, covering the given amount of distance.
*	@param pEntity Entity to move.
*	@param vecGoal Destination to move towards.
*	@param flDist Distance to cover.
*	@param moveType Movement type.
*/
void UTIL_MoveToOrigin( CBaseEntity* pEntity, const Vector& vecGoal, float flDist, const MoveToOrigin moveType );

/**
*	Checks if an object can be tossed from one position to another. (underhand throw)
*	@param pEntity The entity that is tossing the object.
*	@param vecSpot1 Starting position.
*	@param vecSpot2 Destination.
*	@param flGravityAdj Gravity to subject the object to.
*	@return The velocity at which an object should be lobbed from vecspot1 to land near vecspot2.
*	Returns g_vecZero if toss is not feasible.
*/
Vector VecCheckToss( CBaseEntity* pEntity, const Vector& vecSpot1, Vector vecSpot2, float flGravityAdj = 1.0 );

/**
*	Checks if an object can be thrown from one position to another. (overhand throw)
*	@param pEntity The entity that is throwing the object.
*	@param vecSpot1 Starting position.
*	@param vecSpot2 Destination.
*	@param flSpeed Throw speed, in units/sec.
*	@param flGravityAdj Gravity to subject the object to.
*	@return The velocity vector at which an object should be thrown from vecspot1 to hit vecspot2.
*	Returns g_vecZero if throw is not feasible.
*/
Vector VecCheckThrow( CBaseEntity* pEntity, const Vector& vecSpot1, Vector vecSpot2, float flSpeed, float flGravityAdj = 1.0 );

extern DLL_GLOBAL Vector g_vecAttackDir;

/**
*	Tosses a brass shell from passed origin at passed velocity.
*	@param vecOrigin Shell origin.
*	@param vecVelocity Shell velocity.
*	@param rotation Shell yaw rotation.
*	@param model Index of the model to use. Value returned by PRECACHE_MODEL.
*	@param soundtype Type of sound to make when the shell hits something.
*/
void EjectBrass( const Vector& vecOrigin, const Vector& vecVelocity, float rotation, int model, TE_Bounce soundtype );

/**
*	Creates models that explode outward.
*	@param vecOrigin Origin around which to spawn the models.
*	@param speed Movement speed.
*	@param model Index of the model to use. Value returned by PRECACHE_MODEL.
*	@param count Number of models to create.
*	TODO: implementation is disabled. - Solokiller
*/
void ExplodeModel( const Vector& vecOrigin, float speed, int model, int count );

/**
*	These bits represent the monster's memory.
*/
enum MonsterMemory
{
	MEMORY_CLEAR					= 0,

	/**
	*	Right now only used for houndeyes.
	*/
	bits_MEMORY_PROVOKED			= 1 << 0,

	/**
	*	Monster knows it is in a covered position.
	*/
	bits_MEMORY_INCOVER				= 1 << 1,

	/**
	*	Ally is suspicious of the player, and will move to provoked more easily.
	*/
	bits_MEMORY_SUSPICIOUS			= 1 << 2,

	/**
	*	Finished monster path (just used by big momma for now).
	*/
	bits_MEMORY_PATH_FINISHED		= 1 << 3,

	/**
	*	Moving on a path.
	*/
	bits_MEMORY_ON_PATH				= 1 << 4,

	/**
	*	Movement has already failed.
	*/
	bits_MEMORY_MOVE_FAILED			= 1 << 5,

	/**
	*	Has already flinched.
	*/
	bits_MEMORY_FLINCHED			= 1 << 6,

	/**
	*	HACKHACK -- remember that I've already called my Killed().
	*/
	bits_MEMORY_KILLED				= 1 << 7,

	/**
	*	Monster-specific memory.
	*/
	bits_MEMORY_CUSTOM4				= 1 << 28,

	/**
	*	Monster-specific memory
	*/
	bits_MEMORY_CUSTOM3				= 1 << 29,

	/**
	*	Monster-specific memory
	*/
	bits_MEMORY_CUSTOM2				= 1 << 30,

	/**
	*	Monster-specific memory
	*/
	bits_MEMORY_CUSTOM1				= 1 << 31,
};

/**
*	Trigger conditions for scripted AI
*	These MUST match the CHOICES interface in halflife.fgd for the base monster
*/
enum 
{
	/**
	*	"No Trigger"
	*/
	AITRIGGER_NONE = 0,

	/**
	*	"See Player"
	*/
	AITRIGGER_SEEPLAYER_ANGRY_AT_PLAYER,

	/**
	*	"Take Damage"
	*/
	AITRIGGER_TAKEDAMAGE,

	/**
	*	"50% Health Remaining"
	*/
	AITRIGGER_HALFHEALTH,

	/**
	*	"Death"
	*/
	AITRIGGER_DEATH,

	/**
	*	"Squad Member Dead"
	*/
	AITRIGGER_SQUADMEMBERDIE,

	/**
	*	"Squad Leader Dead"
	*/
	AITRIGGER_SQUADLEADERDIE,

	/**
	*	"Hear World"
	*/
	AITRIGGER_HEARWORLD,

	/**
	*	"Hear Player"
	*/
	AITRIGGER_HEARPLAYER,

	/**
	*	"Hear Combat"
	*/
	AITRIGGER_HEARCOMBAT,

	/**
	*	"See Player Unconditional"
	*/
	AITRIGGER_SEEPLAYER_UNCONDITIONAL,

	/**
	*	"See Player, Not In Combat"
	*/
	AITRIGGER_SEEPLAYER_NOT_IN_COMBAT,
};

/**
*	Monster schedules meta data. - Solokiller
*/
struct Schedules_t
{
	const Schedules_t* pBaseList;

	const Schedule_t* const* ppSchedules;

	size_t uiNumSchedules;
};

/**
*	Specialized for every monster that defines custom schedules. - Solokiller
*	@tparam T Monster class.
*	@return true if the schedules were initialized, false otherwise.
*/
template<typename T>
bool InitSchedules()
{
	return false;
}

#define DECLARE_SCHEDULES()								\
private:												\
	static Schedules_t m_Schedules;						\
														\
	template<typename T>								\
	friend bool InitSchedules();						\
														\
public:													\
	static const Schedules_t* GetBaseSchedulesList();	\
	static const Schedules_t* GetThisSchedulesList();	\
	virtual const Schedules_t* GetSchedulesList() const

#define __BEGIN_SCHEDULES( thisClass )							\
																\
Schedules_t thisClass::m_Schedules;								\
																\
const Schedules_t* thisClass::GetThisSchedulesList()			\
{																\
	return &m_Schedules;										\
}																\
																\
const Schedules_t* thisClass::GetSchedulesList() const			\
{																\
	return &m_Schedules;										\
}																\
																\
template<>														\
bool InitSchedules<thisClass>();								\
																\
namespace __##thisClass##__Init									\
{																\
	const bool bInitSchedules = InitSchedules<thisClass>();		\
}																\
																\
template<>														\
bool InitSchedules<thisClass>()									\
{																\
	typedef thisClass ThisClass;								\
																\
	static Schedule_t* schedules[] =							\
	{

/**
*	Begins the schedules list for the base class.
*/
#define BEGIN_SCHEDULES_NOBASE( thisClass )				\
const Schedules_t* thisClass::GetBaseSchedulesList()	\
{														\
	return nullptr;										\
}														\
														\
__BEGIN_SCHEDULES( thisClass )

/**
*	Begins the schedules list for subclasses.
*/
#define BEGIN_SCHEDULES( thisClass )						\
const Schedules_t* thisClass::GetBaseSchedulesList()		\
{															\
	return thisClass::BaseClass::GetThisSchedulesList();	\
}															\
															\
__BEGIN_SCHEDULES( thisClass )

/**
*	Ends the schedules list.
*/
#define END_SCHEDULES()											\
	};															\
																\
	Schedules_t* pSchedules = &ThisClass::m_Schedules;			\
	pSchedules->pBaseList = ThisClass::GetBaseSchedulesList();	\
	pSchedules->ppSchedules = schedules;						\
	pSchedules->uiNumSchedules = ARRAYSIZE( schedules );		\
																\
	return true;												\
}

#endif //GAME_SERVER_ENTITIES_NPCS_MONSTERS_H
