#ifndef COMMON_GAME_H
#define COMMON_GAME_H

/**
*	@file
*	Global header file for game code libraries.
*/

// Allow "DEBUG" in addition to default "_DEBUG"
#ifdef _DEBUG
#define DEBUG 1
#endif

#ifdef WIN32
// Silence certain warnings
#pragma warning(disable : 4244)		// int or float down-conversion
#pragma warning(disable : 4305)		// int or float data truncation
#pragma warning(disable : 4201)		// nameless struct/union
#pragma warning(disable : 4514)		// unreferenced inline function removed
#pragma warning(disable : 4100)		// unreferenced formal parameter
#endif

#include "archtypes.h"     // DAL

#include "Platform.h"

#include "MinMax.h"

// Vector class
#include "mathlib.h"

// Shared engine/DLL constants
#include "const.h"

#include "cdll_dll.h"

#include "GameConstants.h"

#endif //COMMON_GAME_H
