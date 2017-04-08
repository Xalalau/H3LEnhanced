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
#ifndef GAME_SERVER_ENTITIES_NPCS_CHEADCRAB_H
#define GAME_SERVER_ENTITIES_NPCS_CHEADCRAB_H

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		HC_AE_JUMPATTACK	( 2 )

extern Schedule_t slHCRangeAttack1Fast[];
extern Schedule_t slHCRangeAttack1[];

class CHeadCrab : public CBaseMonster
{
public:
	DECLARE_CLASS( CHeadCrab, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void RunTask( const Task_t* pTask ) override;
	void StartTask( const Task_t* pTask ) override;
	void SetYawSpeed( void ) override;
	void LeapTouch( CBaseEntity *pOther );
	Vector Center() const override;
	Vector BodyTarget( const Vector &posSrc ) const override;
	void PainSound( void ) override;
	void DeathSound( void ) override;
	void IdleSound( void ) override;
	void AlertSound( void ) override;
	void PrescheduleThink( void ) override;
	EntityClassification_t GetClassification() override;
	void HandleAnimEvent( AnimEvent_t& event ) override;
	bool CheckRangeAttack1( float flDot, float flDist ) override;
	bool CheckRangeAttack2( float flDot, float flDist ) override;
	void OnTakeDamage( const CTakeDamageInfo& info ) override;

	virtual float GetDamageAmount( void ) { return gSkillData.GetHeadcrabDmgBite(); }
	virtual int GetVoicePitch( void ) { return 100; }
	virtual float GetSoundVolume() const { return 1.0; }
	Schedule_t* GetScheduleOfType( int Type ) override;

	DECLARE_SCHEDULES() override;

	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pAttackSounds[];
	static const char *pDeathSounds[];
	static const char *pBiteSounds[];
};

#endif //GAME_SERVER_ENTITIES_NPCS_CHEADCRAB_H