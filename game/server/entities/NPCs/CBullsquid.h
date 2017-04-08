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
#ifndef GAME_ENTITIES_NPCS_CBULLSQUID_H
#define GAME_ENTITIES_NPCS_CBULLSQUID_H

#define		SQUID_SPRINT_DIST	256 // how close the squid has to get before starting to sprint and refusing to swerve

//=========================================================
// monster-specific schedule types
//=========================================================
enum
{
	SCHED_SQUID_HURTHOP = LAST_COMMON_SCHEDULE + 1,
	SCHED_SQUID_SMELLFOOD,
	SCHED_SQUID_SEECRAB,
	SCHED_SQUID_EAT,
	SCHED_SQUID_SNIFF_AND_EAT,
	SCHED_SQUID_WALLOW,
};

//=========================================================
// monster-specific tasks
//=========================================================
enum
{
	TASK_SQUID_HOPTURN = LAST_COMMON_TASK + 1,
};

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		BSQUID_AE_SPIT		( 1 )
#define		BSQUID_AE_BITE		( 2 )
#define		BSQUID_AE_BLINK		( 3 )
#define		BSQUID_AE_TAILWHIP	( 4 )
#define		BSQUID_AE_HOP		( 5 )
#define		BSQUID_AE_THROW		( 6 )

class CBullsquid : public CBaseMonster
{
public:
	DECLARE_CLASS( CBullsquid, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetYawSpeed( void ) override;
	int  ISoundMask( void ) override;
	EntityClassification_t GetClassification() override;
	void HandleAnimEvent( AnimEvent_t& event ) override;
	void IdleSound( void ) override;
	void PainSound( void ) override;
	void DeathSound( void ) override;
	void AlertSound( void ) override;
	void AttackSound( void );
	void StartTask( const Task_t* pTask ) override;
	void RunTask( const Task_t* pTask ) override;
	bool CheckMeleeAttack1( float flDot, float flDist ) override;
	bool CheckMeleeAttack2( float flDot, float flDist ) override;
	bool CheckRangeAttack1( float flDot, float flDist ) override;
	void RunAI( void ) override;
	bool FValidateHintType( short sHint ) const override;
	Schedule_t *GetSchedule( void ) override;
	Schedule_t *GetScheduleOfType( int Type ) override;
	void OnTakeDamage( const CTakeDamageInfo& info ) override;
	Relationship IRelationship( CBaseEntity *pTarget ) override;
	int IgnoreConditions( void ) override;
	MONSTERSTATE GetIdealState( void ) override;

	DECLARE_SCHEDULES() override;

	bool m_fCanThreatDisplay;// this is so the squid only does the "I see a headcrab!" dance one time. 

	float m_flLastHurtTime;// we keep track of this, because if something hurts a squid, it will forget about its love of headcrabs for a while.
	float m_flNextSpitTime;// last time the bullsquid used the spit attack.
};

#endif //GAME_ENTITIES_NPCS_CBULLSQUID_H