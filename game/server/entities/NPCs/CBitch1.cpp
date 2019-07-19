// ##############################
// HU3-LIFE PUTA 1
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

BEGIN_DATADESC(	CBitch1 )
	DEFINE_FIELD( m_painTime, FIELD_TIME ),
	DEFINE_FIELD( m_healTime, FIELD_TIME ),
	DEFINE_FIELD( m_fearTime, FIELD_TIME ),
END_DATADESC()

LINK_ENTITY_TO_CLASS(monster_puta_1, CBitch1);

enum
{
	TASK_SAY_HEAL = LAST_TALKMONSTER_TASK + 1,
	TASK_HEAL,
	TASK_SAY_FEAR,
	TASK_RUN_PATH_SCARED,
	TASK_SCREAM,
	TASK_RANDOM_SCREAM,
	TASK_MOVE_TO_TARGET_RANGE_SCARED,
};


void CBitch1::DeclineFollowing(void)
{
	if (pev->spawnflags & SF_MONSTER_GAG)
		return;
	Talk(10);
	m_hTalkTarget = m_hEnemy;
	PlaySentence("PT_POK", 2, VOL_NORM, ATTN_NORM);
}


void CBitch1::Scream(void)
{
	if (FOkToSpeak())
	{
		Talk(10);
		m_hTalkTarget = m_hEnemy;
		PlaySentence("PT_SCREAM", RANDOM_FLOAT(3, 6), VOL_NORM, ATTN_NORM);
	}
}

void CBitch1::StartTask(const Task_t& pTask)
{
	switch (pTask.iTask)
	{
	case TASK_SAY_HEAL:
		//		if ( FOkToSpeak() )
		Talk(2);
		m_hTalkTarget = m_hTargetEnt;
		PlaySentence("PT_HEAL", 2, VOL_NORM, ATTN_IDLE);

		TaskComplete();
		break;

	case TASK_SCREAM:
		Scream();
		TaskComplete();
		break;

	case TASK_RANDOM_SCREAM:
		if (RANDOM_FLOAT(0, 1) < pTask.flData)
			Scream();
		TaskComplete();
		break;

	case TASK_SAY_FEAR:
		if (FOkToSpeak())
		{
			Talk(2);
			m_hTalkTarget = m_hEnemy;
			if (m_hEnemy->IsPlayer())
				PlaySentence("PT_PLFEAR", 5, VOL_NORM, ATTN_NORM);
			else
				PlaySentence("PT_FEAR", 5, VOL_NORM, ATTN_NORM);
		}
		TaskComplete();
		break;

	case TASK_HEAL:
		m_IdealActivity = ACT_MELEE_ATTACK1;
		break;

	case TASK_RUN_PATH_SCARED:
		m_movementActivity = ACT_RUN_SCARED;
		break;

	case TASK_MOVE_TO_TARGET_RANGE_SCARED:
	{
		if ((m_hTargetEnt->pev->origin - pev->origin).Length() < 1)
			TaskComplete();
		else
		{
			m_vecMoveGoal = m_hTargetEnt->pev->origin;
			if (!MoveToTarget(ACT_WALK_SCARED, 0.5))
				TaskFail();
		}
	}
	break;

	default:
		CTalkMonster::StartTask(pTask);
		break;
	}
}

//=========================================================
// Spawn
//=========================================================
void CBitch1::Spawn(void)
{
	PRECACHE_MODEL("models/puta1.mdl");
	Precache();

	SetModel("models/puta1.mdl");
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

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CBitch1::Precache(void)
{
	PRECACHE_SOUND("putas/geme1.wav");
	PRECACHE_SOUND("putas/geme2.wav");
	PRECACHE_SOUND("putas/geme3.wav");
	PRECACHE_SOUND("putas/geme4.wav");
	PRECACHE_SOUND("putas/geme5.wav");
	PRECACHE_SOUND("putas/gemendo1.wav");
	PRECACHE_SOUND("putas/gemendo2.wav");
	PRECACHE_SOUND("putas/gemendo3.wav");
	PRECACHE_SOUND("putas/gemendo4.wav");

	// every new scientist must call this, otherwise
	// when a level is loaded, nobody will talk (time is reset to 0)
	TalkInit();

	CTalkMonster::Precache();
}

// Init talk data
void CBitch1::TalkInit()
{

	CTalkMonster::TalkInit();

	// scientist will try to talk to friends in this order:

	m_szFriends[0] = RANDOM_LONG(0, 1) ? "monster_puta_1" : "monster_puta_2";
	m_szFriends[1] = RANDOM_LONG(0, 1) ? "monster_puta_3" : "monster_puta_4";
	m_szFriends[2] = RANDOM_LONG(0, 1) ? "monster_peitos_gloriosos" : "monster_puta_1";

	// scientists speach group names (group names are in sentences.txt)

	m_szGrp[TLK_ANSWER] = "PT_ANSWER";
	m_szGrp[TLK_QUESTION] = "PT_QUESTION";
	m_szGrp[TLK_IDLE] = "PT_IDLE";
	m_szGrp[TLK_STARE] = "PT_STARE";
	m_szGrp[TLK_USE] = "PT_OK";
	m_szGrp[TLK_UNUSE] = "PT_WAIT";
	m_szGrp[TLK_STOP] = "PT_STOP";
	m_szGrp[TLK_NOSHOOT] = "PT_SCARED";
	m_szGrp[TLK_HELLO] = "PT_HELLO";

	m_szGrp[TLK_PLHURT1] = "!PT_CUREA";
	m_szGrp[TLK_PLHURT2] = "!PT_CUREB";
	m_szGrp[TLK_PLHURT3] = "!PT_CUREC";

	m_szGrp[TLK_PHELLO] = "PT_PHELLO";
	m_szGrp[TLK_PIDLE] = "PT_PIDLE";
	m_szGrp[TLK_PQUESTION] = "PT_PQUEST";
	m_szGrp[TLK_SMELL] = "PT_SMELL";

	m_szGrp[TLK_WOUND] = "PT_WOUND";
	m_szGrp[TLK_MORTAL] = "PT_MORTAL";
}

//=========================================================
// PainSound
//=========================================================
void CBitch1::PainSound(void)
{
	if (gpGlobals->time < m_painTime)
		return;

	m_painTime = gpGlobals->time + RANDOM_FLOAT(0.5, 0.75);

	switch (RANDOM_LONG(0, 3))
	{
	case 0: EMIT_SOUND_DYN(this, CHAN_VOICE, "putas/gemendo1.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 1: EMIT_SOUND_DYN(this, CHAN_VOICE, "putas/gemendo2.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 2: EMIT_SOUND_DYN(this, CHAN_VOICE, "putas/gemendo3.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 3: EMIT_SOUND_DYN(this, CHAN_VOICE, "putas/gemendo4.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	}
}