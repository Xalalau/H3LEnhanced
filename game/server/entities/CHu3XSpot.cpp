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
#include "CBasePlayer.h"

#include "CHu3XSpot.h"

BEGIN_DATADESC(CHu3XSpot)
END_DATADESC()

LINK_ENTITY_TO_CLASS(laser_hu3, CHu3XSpot);

//=========================================================
// Inicializacao
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
}

void CHu3XSpot::Precache()
{
	PRECACHE_MODEL("sprites/laserdot_hu3.spr");
}

CHu3XSpot *CHu3XSpot::CreateSpot()
{
	CHu3XSpot *pSpot = GetClassPtr((CHu3XSpot *)NULL);
	pSpot->Spawn();

	pSpot->pev->classname = MAKE_STRING("laser_hu3");

	return pSpot;
}

//=========================================================
// Renderizar
//=========================================================
void CHu3XSpot::UpdateSpot(CBasePlayer* m_pPlayer, CHu3XSpot* m_pLaser)
{
	if (!m_pLaser)
		return;

	if (m_pPlayer->hu3_cam_crosshair)
	{
		UTIL_MakeVectors(m_pPlayer->GetViewAngle());

		Vector vecSrc = m_pPlayer->GetGunPosition();

		Vector vecEnd = vecSrc + gpGlobals->v_forward * 8192.0;

		TraceResult tr;

		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, m_pPlayer->edict(), &tr);

		m_pLaser->SetAbsOrigin(tr.vecEndPos);
	}
}

//=========================================================
// Remover
//=========================================================
void CHu3XSpot::RemoveSpot(CHu3XSpot* m_pLaser)
{
	if (m_pLaser)
	{
		m_pLaser->Killed(CTakeDamageInfo(nullptr, 0, 0), GIB_NEVER);
		m_pLaser = nullptr;
	}
}