// ##############################
// HU3-LIFE PUTA 2
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

#include "CBitch2.h"

BEGIN_DATADESC(	CBitch2 )
	DEFINE_FIELD( m_painTime, FIELD_TIME ),
	DEFINE_FIELD( m_healTime, FIELD_TIME ),
	DEFINE_FIELD( m_fearTime, FIELD_TIME ),
END_DATADESC()

LINK_ENTITY_TO_CLASS(monster_puta_2, CBitch2);

//=========================================================
// Spawn
//=========================================================
void CBitch2::Spawn(void)
{
	PRECACHE_MODEL("models/puta2.mdl");
	Precache();

	SetModel("models/puta2.mdl");
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