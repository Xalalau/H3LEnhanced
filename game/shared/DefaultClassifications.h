#ifndef GAME_SHARED_DEFAULTCLASSIFICATIONS_H
#define GAME_SHARED_DEFAULTCLASSIFICATIONS_H

#include <cstdlib>

/**
*	@addtogroup Classifications
*
*	Classifications used by entities
*
*	@{
*/

namespace classify
{
extern const char NONE[];
extern const char MACHINE[];
extern const char PLAYER[];
extern const char HUMAN_PASSIVE[];
extern const char HUMAN_MILITARY[];
extern const char ALIEN_MILITARY[];
extern const char ALIEN_PASSIVE[];
extern const char ALIEN_MONSTER[];
extern const char ALIEN_PREY[];
extern const char ALIEN_PREDATOR[];
extern const char INSECT[];
extern const char PLAYER_ALLY[];
/*
*	Hornets and snarks. Launched by players.
*/
extern const char PLAYER_BIOWEAPON[];

/*
*	Hornets and snarks. Launched by the alien menace.
*/
extern const char ALIEN_BIOWEAPON[];

/*
*	Entities that want to be ignored use this.
*	Used to be called CLASS_BARNACLE.
*	Special because no one pays attention to it, and it eats a wide cross-section of creatures.
*/
extern const char IGNORE[];

/**
*	List of all default classifications.
*	@see MAX_DEFAULT_CLASSIFICATIONS
*/
extern const char* const DEFAULT_CLASSIFICATIONS[];

/**
*	Number of default classifications.
*	@see DEFAULT_CLASSIFICATIONS
*/
extern const size_t MAX_DEFAULT_CLASSIFICATIONS;
}

/** @} */

#endif //GAME_SHARED_DEFAULTCLASSIFICATIONS_H
