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
#ifndef GAME_SERVER_ENTITIES_NPCS_CAGRUNT_H
#define GAME_SERVER_ENTITIES_NPCS_CAGRUNT_H

//=========================================================
// monster-specific schedule types
//=========================================================
enum
{
	SCHED_AGRUNT_SUPPRESS = LAST_COMMON_SCHEDULE + 1,
	SCHED_AGRUNT_THREAT_DISPLAY,
};

//=========================================================
// monster-specific tasks
//=========================================================
enum
{
	TASK_AGRUNT_SETUP_HIDE_ATTACK = LAST_COMMON_TASK + 1,
	TASK_AGRUNT_GET_PATH_TO_ENEMY_CORPSE,
};

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		AGRUNT_AE_HORNET1	( 1 )
#define		AGRUNT_AE_HORNET2	( 2 )
#define		AGRUNT_AE_HORNET3	( 3 )
#define		AGRUNT_AE_HORNET4	( 4 )
#define		AGRUNT_AE_HORNET5	( 5 )
// some events are set up in the QC file that aren't recognized by the code yet.
#define		AGRUNT_AE_PUNCH		( 6 )
#define		AGRUNT_AE_BITE		( 7 )

#define		AGRUNT_AE_LEFT_FOOT	 ( 10 )
#define		AGRUNT_AE_RIGHT_FOOT ( 11 )

#define		AGRUNT_AE_LEFT_PUNCH ( 12 )
#define		AGRUNT_AE_RIGHT_PUNCH ( 13 )

#define		AGRUNT_MELEE_DIST	100

class CAGrunt : public CSquadMonster
{
public:
	DECLARE_CLASS( CAGrunt, CSquadMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetYawSpeed( void ) override;
	EntityClassification_t GetClassification() override;
	int  ISoundMask( void ) override;
	void HandleAnimEvent( AnimEvent_t& event ) override;
	void SetObjectCollisionBox( void ) override
	{
		pev->absmin = GetAbsOrigin() + Vector( -32, -32, 0 );
		pev->absmax = GetAbsOrigin() + Vector( 32, 32, 85 );
	}

	Schedule_t* GetSchedule( void ) override;
	Schedule_t* GetScheduleOfType( int Type ) override;
	bool FCanCheckAttacks() const override;
	bool CheckMeleeAttack1( float flDot, float flDist ) override;
	bool CheckRangeAttack1( float flDot, float flDist ) override;
	void StartTask( const Task_t* pTask ) override;
	void AlertSound( void ) override;
	void DeathSound( void ) override;
	void PainSound( void ) override;
	void AttackSound( void );
	void PrescheduleThink( void ) override;
	void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr ) override;
	Relationship IRelationship( CBaseEntity *pTarget ) override;
	void StopTalking( void );
	bool ShouldSpeak();
	DECLARE_SCHEDULES() override;

	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];
	static const char *pAttackSounds[];
	static const char *pDieSounds[];
	static const char *pPainSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];

	bool	m_fCanHornetAttack;
	float	m_flNextHornetAttackCheck;

	float m_flNextPainTime;

	// three hacky fields for speech stuff. These don't really need to be saved.
	float	m_flNextSpeakTime;
	float	m_flNextWordTime;
	int		m_iLastWord;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CAGRUNT_H