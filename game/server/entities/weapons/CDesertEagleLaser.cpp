#if USE_OPFOR
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
#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CDesertEagleLaser.h"

LINK_ENTITY_TO_CLASS( eagle_laser, CDesertEagleLaser );

//=========================================================
//=========================================================
CDesertEagleLaser *CDesertEagleLaser::CreateSpot()
{
	auto pSpot = static_cast<CDesertEagleLaser*>( UTIL_CreateNamedEntity( "eagle_laser" ) );
	pSpot->Spawn();

	return pSpot;
}

#endif //USE_OPFOR
