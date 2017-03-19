// ##############################
// HU3-LIFE PEITOS GLORIOSOS
// ##############################

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "CTalkMonster.h"
#include "Schedule.h"
#include "DefaultAI.h"
#include "animation.h"
#include "entities/CSoundEnt.h"
#include "CScientist.h"
#include "CBitch1.h"

#include "CGloriousBreasts.h"

LINK_ENTITY_TO_CLASS(monster_peitos_gloriosos, CGloriousBreasts);

//=========================================================
// Spawn
//=========================================================
void CGloriousBreasts::Spawn(void)
{
	PRECACHE_MODEL("models/peitosgloriosos.mdl");
	Precache();

	SetModel("models/peitosgloriosos.mdl");
	SetSize(VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_RED;
	pev->health = gSkillData.GetScientistHealth();

	pev->view_ofs = Vector(0, 0, 50);// position of the eyes relative to monster's origin.
	m_flFieldOfView = VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so scientists will notice player and say hello
	m_MonsterState = MONSTERSTATE_NONE;

	//	m_flDistTooFar		= 256.0;

	m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_OPEN_DOORS | bits_CAP_AUTO_DOORS | bits_CAP_USE;

	MonsterInit();
	SetUse(&CScientist::FollowerUse);
}