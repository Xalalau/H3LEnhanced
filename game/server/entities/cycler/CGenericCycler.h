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
#ifndef GAME_SERVER_ENTITIES_CYCLER_CGENERICCYCLER_H
#define GAME_SERVER_ENTITIES_CYCLER_CGENERICCYCLER_H

#include "CCycler.h"

//
// we should get rid of all the other cyclers and replace them with this.
//
class CGenericCycler : public CCycler
{
public:
	void Spawn( void ) override { GenericCyclerSpawn( GetModelName(), Vector( -16, -16, 0 ), Vector( 16, 16, 72 ) ); }
};

#endif //GAME_SERVER_ENTITIES_CYCLER_CGENERICCYCLER_H