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

#include "CPedestrian1.h"

BEGIN_DATADESC(	CPedestrian1 )
	DEFINE_FIELD( m_painTime, FIELD_TIME ),
	DEFINE_FIELD( m_healTime, FIELD_TIME ),
	DEFINE_FIELD( m_fearTime, FIELD_TIME ),
END_DATADESC()

LINK_ENTITY_TO_CLASS(monster_pedestre_1, CPedestrian1);

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

void CPedestrian1::DeclineFollowing(void)
{
	if (pev->spawnflags & SF_MONSTER_GAG)
		return;
	Talk(10);
	m_hTalkTarget = m_hEnemy;
	PlaySentence("FVP_POK", 2, VOL_NORM, ATTN_NORM);
}

void CPedestrian1 :: Scream(void)
{
	if (FOkToSpeak())
	{
		Talk(10);
		m_hTalkTarget = m_hEnemy;
		PlaySentence("FVP_SCREAM", 3, VOL_NORM, ATTN_NORM);
	}
}

void CPedestrian1::StartTask(const Task_t* pTask)
{
	switch (pTask->iTask)
	{
	case TASK_SAY_HEAL:
		//		if ( FOkToSpeak() )
		Talk(2);
		m_hTalkTarget = m_hTargetEnt;
		PlaySentence("FVP_HEAL", 2, VOL_NORM, ATTN_IDLE);

		TaskComplete();
		break;

	case TASK_SCREAM:
		Scream();
		TaskComplete();
		break;

	case TASK_RANDOM_SCREAM:
		if (RANDOM_FLOAT(0, 1) < pTask->flData)
			Scream();
		TaskComplete();
		break;

	case TASK_SAY_FEAR:
		if (FOkToSpeak())
		{
			Talk(2);
			m_hTalkTarget = m_hEnemy;
			if (m_hEnemy->IsPlayer())
				PlaySentence("FVP_PLFEAR", 5, VOL_NORM, ATTN_NORM);
			else
				PlaySentence("FVP_FEAR", 5, VOL_NORM, ATTN_NORM);
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
void CPedestrian1::Spawn(void)
{
	PRECACHE_MODEL("models/pedestre_1.mdl");
	Precache();

	SetModel("models/pedestre_1.mdl");
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
void CPedestrian1::Precache(void)
{
	PRECACHE_SOUND("pedestres/peido1.wav");
	PRECACHE_SOUND("pedestres/peido2.wav");
	PRECACHE_SOUND("pedestres/peido3.wav");
	PRECACHE_SOUND("pedestres/peido4.wav");
	PRECACHE_SOUND("pedestres/dor1.wav");
	PRECACHE_SOUND("pedestres/dor2.wav");
	PRECACHE_SOUND("pedestres/dor3.wav");
	PRECACHE_SOUND("pedestres/dor4.wav");
	PRECACHE_SOUND("pedestres/batidapolicia.wav");
	PRECACHE_SOUND("pedestres/foiatingido.wav");
	PRECACHE_SOUND("pedestres/jamateimto.wav");
	PRECACHE_SOUND("pedestres/socorro.wav");
	PRECACHE_SOUND("pedestres/trocatiro.wav");
	PRECACHE_SOUND("pedestres/vaiproutrolado.wav");

	// every new scientist must call this, otherwise
	// when a level is loaded, nobody will talk (time is reset to 0)
	TalkInit();

	CTalkMonster::Precache();
}

// Init talk data
void CPedestrian1::TalkInit()
{

	CTalkMonster::TalkInit();

	// scientist will try to talk to friends in this order:

	m_szFriends[0] = "monster_pedestre_1";
	m_szFriends[1] = "monster_pedestre_2";
	m_szFriends[2] = "monster_pedestre_3";

	// scientists speach group names (group names are in sentences.txt)

	m_szGrp[TLK_ANSWER] = "FVP_ANSWER";
	m_szGrp[TLK_QUESTION] = "FVP_QUESTION";
	m_szGrp[TLK_IDLE] = "FVP_IDLE";
	m_szGrp[TLK_STARE] = "FVP_STARE";
	m_szGrp[TLK_USE] = "FVP_OK";
	m_szGrp[TLK_UNUSE] = "FVP_WAIT";
	m_szGrp[TLK_STOP] = "FVP_STOP";
	m_szGrp[TLK_NOSHOOT] = "FVP_SCARED";
	m_szGrp[TLK_HELLO] = "FVP_HELLO";

	m_szGrp[TLK_PLHURT1] = "!FVP_CUREA";
	m_szGrp[TLK_PLHURT2] = "!FVP_CUREB";
	m_szGrp[TLK_PLHURT3] = "!FVP_CUREC";

	m_szGrp[TLK_PHELLO] = "FVP_PHELLO";
	m_szGrp[TLK_PIDLE] = "FVP_PIDLE";
	m_szGrp[TLK_PQUESTION] = "FVP_PQUEST";
	m_szGrp[TLK_SMELL] = "FVP_SMELL";

	m_szGrp[TLK_WOUND] = "FVP_WOUND";
	m_szGrp[TLK_MORTAL] = "FVP_MORTAL";
}

//=========================================================
// PainSound
//=========================================================
void CPedestrian1::PainSound(void)
{
	if (gpGlobals->time < m_painTime)
		return;

	m_painTime = gpGlobals->time + RANDOM_FLOAT(0.5, 0.75);

	switch (RANDOM_LONG(0, 3))
	{
	case 0: EMIT_SOUND_DYN(this, CHAN_VOICE, "pedestres/dor1.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 1: EMIT_SOUND_DYN(this, CHAN_VOICE, "pedestres/dor2.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 2: EMIT_SOUND_DYN(this, CHAN_VOICE, "pedestres/dor3.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 3: EMIT_SOUND_DYN(this, CHAN_VOICE, "pedestres/dor4.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	}
}