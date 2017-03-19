// ##############################
// HU3-LIFE GENÉRICO INDISTRUTÍVEL
// ##############################

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"
#include "CGenericMonster.h"

#include "CGenericMonsterGod.h"

LINK_ENTITY_TO_CLASS(monster_generic_god, CGenericMonsterGod);

void CGenericMonsterGod::Spawn()
{
	Precache();

	SET_MODEL(ENT(pev), STRING(pev->model));

	/*
	if ( FStrEq( STRING(pev->model), "models/player.mdl" ) )
	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	else
	UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
	*/

	if (FStrEq(STRING(pev->model), "models/player.mdl") || FStrEq(STRING(pev->model), "models/holo.mdl"))
		UTIL_SetSize(this, VEC_HULL_MIN, VEC_HULL_MAX);
	else
		UTIL_SetSize(this, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_RED;
	pev->health = 999999;
	m_flFieldOfView = 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState = MONSTERSTATE_NONE;

	MonsterInit();

	if (pev->spawnflags & SF_GENERICMONSTER_NOTSOLID)
	{
		pev->solid = SOLID_NOT;
		pev->takedamage = DAMAGE_NO;
	}
}