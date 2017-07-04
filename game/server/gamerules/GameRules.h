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
#ifndef GAME_SERVER_GAMERULES_GAMERULES_H
#define GAME_SERVER_GAMERULES_GAMERULES_H

#include "CGameRules.h"

// ############ hu3lifezado ############ //
// [MODO COOP]
// Valores do Hu3-Life
namespace Coop
{
/**
*	Co-op setting options.
*/
enum Coop
{
	/**
	*	Sem coop.
	*/
	NO = 0,

	/**
	*	Com coop.
	*/
	YES = 1
};
}
// ############ //

namespace GameRules
{
/**
*	Determines which co-op mode is in use.
*/
Coop::Coop DetermineCoopMode();
}

/**
*	Instantiate the proper game rules object
*/
CGameRules* InstallGameRules();

extern DLL_GLOBAL CGameRules* g_pGameRules;

#endif //GAME_SERVER_GAMERULES_GAMERULES_H
