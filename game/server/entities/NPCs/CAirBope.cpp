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

// ##############################
// HU3-LIFE BOPE AEREO
// ##############################

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Weapons.h"
#include "nodes/Nodes.h"
#include "entities/CSoundEnt.h"
#include "Effects.h"
#include "customentity.h"
#include "COsprey.h"

#include "CAirBope.h"

LINK_ENTITY_TO_CLASS(bope_aereo, CAirBope);

void CAirBope::Spawn(void)
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SetModel("models/bope_aereo.mdl");
	SetSize(Vector(-400, -400, -100), Vector(400, 400, 32));
	SetAbsOrigin(GetAbsOrigin());

	pev->flags |= FL_MONSTER;
	pev->takedamage = DAMAGE_YES;
	m_flRightHealth = 200;
	m_flLeftHealth = 200;
	pev->health = 400;

	m_flFieldOfView = 0; // 180 degrees

	pev->sequence = 0;
	ResetSequenceInfo();
	pev->frame = RANDOM_LONG(0, 0xFF);

	InitBoneControllers();

	SetThink(&COsprey::FindAllThink);
	SetUse(&COsprey::CommandUse);

	if (!(pev->spawnflags & SF_OSPREY_WAITFORTRIGGER))
	{
		pev->nextthink = gpGlobals->time + 1.0;
	}

	m_pos2 = GetAbsOrigin();
	m_ang2 = pev->angles;
	m_vel2 = pev->velocity;
}

void CAirBope::Precache(void)
{
	UTIL_PrecacheOther("monster_human_grunt");

	PRECACHE_MODEL("models/bope_aereo.mdl");
	PRECACHE_MODEL("models/HVR.mdl");

	PRECACHE_SOUND("apache/ap_rotor4.wav");
	PRECACHE_SOUND("weapons/mortarhit.wav");

	m_iSpriteTexture = PRECACHE_MODEL("sprites/rope.spr");

	m_iExplode = PRECACHE_MODEL("sprites/fexplo.spr");
	m_iTailGibs = PRECACHE_MODEL("models/osprey_tailgibs.mdl");
	m_iBodyGibs = PRECACHE_MODEL("models/osprey_bodygibs.mdl");
	m_iEngineGibs = PRECACHE_MODEL("models/osprey_enginegibs.mdl");
}
