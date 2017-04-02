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
#ifndef GAME_SERVER_ENTITIES_WEAPONS_CHU3XSPOT_H
#define GAME_SERVER_ENTITIES_WEAPONS_CHU3XSPOT_H

#include "entities/CLaserSpot.h"

class CHu3XSpot : public CLaserSpot
{
public:
	static CHu3XSpot* CreateSpot();
	void Spawn() override;
	void Precache() override;
};


#endif //GAME_SERVER_ENTITIES_WEAPONS_CHU3XSPOT_H