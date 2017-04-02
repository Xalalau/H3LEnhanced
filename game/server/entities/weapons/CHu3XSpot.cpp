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

#include "CHu3XSpot.h"

LINK_ENTITY_TO_CLASS(laser_hu3, CHu3XSpot);

//=========================================================
// Inicializacao geral
//=========================================================
void CHu3XSpot::Spawn()
{
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	pev->rendermode = kRenderGlow;
	pev->renderfx = kRenderFxNoDissipation;
	pev->renderamt = 255;

	SetModel("sprites/laserdot_hu3.spr");
	SetAbsOrigin(GetAbsOrigin());
};

void CHu3XSpot::Precache()
{
	PRECACHE_MODEL("sprites/laserdot_hu3.spr");
}

//=========================================================
// Create- Instancia um ponto de mira. 
//=========================================================
CHu3XSpot *CHu3XSpot::CreateSpot()
{
	CHu3XSpot *pSpot = GetClassPtr((CHu3XSpot *)NULL);
	pSpot->Spawn();

	pSpot->pev->classname = MAKE_STRING("laser_hu3");

	return pSpot;

}