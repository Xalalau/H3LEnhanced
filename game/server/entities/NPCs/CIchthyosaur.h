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
#ifndef GAME_SERVER_ENTITIES_NPCS_CICHTHYOSAUR_H
#define GAME_SERVER_ENTITIES_NPCS_CICHTHYOSAUR_H

#define ICHTHYOSAUR_SPEED 150

//=========================================================
// Monster's Anim Events Go Here
//=========================================================

// UNDONE: Save/restore here
class CIchthyosaur : public CFlyingMonster
{
public:
	DECLARE_CLASS( CIchthyosaur, CFlyingMonster );
	DECLARE_DATADESC();

	void  Spawn( void ) override;
	void  Precache( void ) override;
	void  SetYawSpeed( void ) override;
	EntityClassification_t GetClassification() override;
	void  HandleAnimEvent( AnimEvent_t& event ) override;
	DECLARE_SCHEDULES() override;

	Schedule_t *GetSchedule( void ) override;
	Schedule_t *GetScheduleOfType( int Type ) override;

	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;
	void BecomeDead( void ) override;

	void CombatUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void BiteTouch( CBaseEntity *pOther );

	void  StartTask( const Task_t* pTask ) override;
	void  RunTask( const Task_t* pTask ) override;

	bool  CheckMeleeAttack1( float flDot, float flDist ) override;
	bool  CheckRangeAttack1( float flDot, float flDist ) override;

	float ChangeYaw( int speed ) override;
	Activity GetStoppedActivity( void ) override;

	void  Move( float flInterval ) override;
	void  MoveExecute( CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval ) override;
	void  MonsterThink( void ) override;
	void  Stop( void ) override;
	void  Swim( void );
	Vector DoProbe( const Vector &Probe );

	float VectorToPitch( const Vector &vec );
	float FlPitchDiff( void );
	float ChangePitch( int speed );

	Vector m_SaveVelocity;
	float m_idealDist;

	float m_flBlink;

	float m_flEnemyTouched;
	bool  m_bOnAttack;

	float m_flMaxSpeed;
	float m_flMinSpeed;
	float m_flMaxDist;

	CBeam *m_pBeam;

	float m_flNextAlert;

	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pAttackSounds[];
	static const char *pBiteSounds[];
	static const char *pDieSounds[];
	static const char *pPainSounds[];

	void IdleSound( void ) override;
	void AlertSound( void ) override;
	void AttackSound( void );
	void BiteSound( void );
	void DeathSound( void ) override;
	void PainSound( void ) override;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CICHTHYOSAUR_H