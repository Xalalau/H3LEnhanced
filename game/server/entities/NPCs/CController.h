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
#ifndef GAME_SERVER_ENTITIES_NPCS_CCONTROLLER_H
#define GAME_SERVER_ENTITIES_NPCS_CCONTROLLER_H

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	CONTROLLER_AE_HEAD_OPEN		1
#define	CONTROLLER_AE_BALL_SHOOT	2
#define	CONTROLLER_AE_SMALL_SHOOT	3
#define CONTROLLER_AE_POWERUP_FULL	4
#define CONTROLLER_AE_POWERUP_HALF	5

#define CONTROLLER_FLINCH_DELAY			2		// at most one flinch every n secs

class CController : public CSquadMonster
{
public:
	DECLARE_CLASS( CController, CSquadMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;

	void UpdateOnRemove() override;

	void SetYawSpeed( void ) override;
	EntityClassification_t GetClassification() override;
	void HandleAnimEvent( AnimEvent_t& event ) override;

	void RunAI( void ) override;
	bool CheckRangeAttack1( float flDot, float flDist ) override;	// balls
	bool CheckRangeAttack2( float flDot, float flDist ) override;	// head
	bool CheckMeleeAttack1( float flDot, float flDist ) override;	// block, throw
	Schedule_t* GetSchedule( void ) override;
	Schedule_t* GetScheduleOfType( int Type ) override;
	void StartTask( const Task_t* pTask ) override;
	void RunTask( const Task_t* pTask ) override;
	DECLARE_SCHEDULES() override;

	void Stop( void ) override;
	void Move( float flInterval ) override;
	int  CheckLocalMove( const Vector &vecStart, const Vector &vecEnd, const CBaseEntity* const pTarget, float *pflDist ) override;
	void MoveExecute( CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval ) override;
	void SetActivity( Activity NewActivity ) override;
	bool ShouldAdvanceRoute( float flWaypointDist ) override;
	int LookupFloat();

	float m_flNextFlinch;

	float m_flShootTime;
	float m_flShootEnd;

	void PainSound( void ) override;
	void AlertSound( void ) override;
	void IdleSound( void ) override;
	void AttackSound( void );
	void DeathSound( void ) override;

	static const char *pAttackSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pDeathSounds[];

	void OnTakeDamage( const CTakeDamageInfo& info ) override;
	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;
	void GibMonster( void ) override;

	CSprite *m_pBall[ 2 ];	// hand balls
	int m_iBall[ 2 ];			// how bright it should be
	float m_iBallTime[ 2 ];	// when it should be that color
	int m_iBallCurrent[ 2 ];	// current brightness

	Vector m_vecEstVelocity;

	Vector m_velocity;
	bool m_fInCombat;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CCONTROLLER_H