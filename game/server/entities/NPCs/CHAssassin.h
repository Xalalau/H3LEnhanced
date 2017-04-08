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
#ifndef GAME_SERVER_ENTITIES_NPCS_CHASSASSIN_H
#define GAME_SERVER_ENTITIES_NPCS_CHASSASSIN_H

//=========================================================
// monster-specific schedule types
//=========================================================
enum
{
	SCHED_ASSASSIN_EXPOSED = LAST_COMMON_SCHEDULE + 1,// cover was blown.
	SCHED_ASSASSIN_JUMP,	// fly through the air
	SCHED_ASSASSIN_JUMP_ATTACK,	// fly through the air and shoot
	SCHED_ASSASSIN_JUMP_LAND, // hit and run away
};

//=========================================================
// monster-specific tasks
//=========================================================

enum
{
	TASK_ASSASSIN_FALL_TO_GROUND = LAST_COMMON_TASK + 1, // falling and waiting to hit ground
};


//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		ASSASSIN_AE_SHOOT1	1
#define		ASSASSIN_AE_TOSS1	2
#define		ASSASSIN_AE_JUMP	3


#define bits_MEMORY_BADJUMP		(bits_MEMORY_CUSTOM1)

class CHAssassin : public CBaseMonster
{
public:
	DECLARE_CLASS( CHAssassin, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetYawSpeed( void ) override;
	EntityClassification_t GetClassification() override;
	int  ISoundMask( void ) override;
	void Shoot( void );
	void HandleAnimEvent( AnimEvent_t& event ) override;
	Schedule_t* GetSchedule( void ) override;
	Schedule_t* GetScheduleOfType( int Type ) override;
	bool CheckMeleeAttack1( float flDot, float flDist ) override;	// jump
																	// bool CheckMeleeAttack2 ( float flDot, float flDist ) override;
	bool CheckRangeAttack1( float flDot, float flDist ) override;	// shoot
	bool CheckRangeAttack2( float flDot, float flDist ) override;	// throw grenade
	void StartTask( const Task_t* pTask ) override;
	void RunAI( void ) override;
	void RunTask( const Task_t* pTask ) override;
	void DeathSound( void ) override;
	void IdleSound( void ) override;
	DECLARE_SCHEDULES() override;

	float m_flLastShot;
	float m_flDiviation;

	float m_flNextJump;
	Vector m_vecJumpVelocity;

	float m_flNextGrenadeCheck;
	Vector	m_vecTossVelocity;
	bool	m_fThrowGrenade;

	int		m_iTargetRanderamt;

	int		m_iFrustration;

	int		m_iShell;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CHASSASSIN_H