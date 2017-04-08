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
#ifndef GAME_SERVER_ENTITIES_NPCS_CGARGANTUA_H
#define GAME_SERVER_ENTITIES_NPCS_CGARGANTUA_H

// Garg animation events
#define GARG_AE_SLASH_LEFT			1
//#define GARG_AE_BEAM_ATTACK_RIGHT	2		// No longer used
#define GARG_AE_LEFT_FOOT			3
#define GARG_AE_RIGHT_FOOT			4
#define GARG_AE_STOMP				5
#define GARG_AE_BREATHE				6

// Gargantua is immune to any damage but this
#define GARG_DAMAGE					(DMG_ENERGYBEAM|DMG_CRUSH|DMG_MORTAR|DMG_BLAST)
#define GARG_EYE_SPRITE_NAME		"sprites/gargeye1.spr"
#define GARG_BEAM_SPRITE_NAME		"sprites/xbeam3.spr"
#define GARG_BEAM_SPRITE2			"sprites/xbeam3.spr"

#define GARG_FLAME_LENGTH			330
#define GARG_GIB_MODEL				"models/metalplategibs.mdl"

#define ATTN_GARG					(ATTN_NORM)

//=========================================================
// Gargantua Monster
//=========================================================
class CGargantua : public CBaseMonster
{
public:
	DECLARE_CLASS( CGargantua, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;

	void UpdateOnRemove() override;

	void SetYawSpeed( void ) override;
	EntityClassification_t GetClassification() override;
	void OnTakeDamage( const CTakeDamageInfo& info ) override;
	void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr ) override;
	void HandleAnimEvent( AnimEvent_t& event ) override;

	bool CheckMeleeAttack1( float flDot, float flDist ) override;		// Swipe
	bool CheckMeleeAttack2( float flDot, float flDist ) override;		// Flames
	bool CheckRangeAttack1( float flDot, float flDist ) override;		// Stomp attack
	void SetObjectCollisionBox( void ) override
	{
		pev->absmin = GetAbsOrigin() + Vector( -80, -80, 0 );
		pev->absmax = GetAbsOrigin() + Vector( 80, 80, 214 );
	}

	Schedule_t *GetScheduleOfType( int Type ) override;
	void StartTask( const Task_t* pTask ) override;
	void RunTask( const Task_t* pTask ) override;

	void PrescheduleThink( void ) override;

	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;
	void DeathEffect( void );

	void EyeOff( void );
	void EyeOn( int level );
	void EyeUpdate( void );
	void Leap( void );
	void StompAttack( void );
	void FlameCreate( void );
	void FlameUpdate( void );
	void FlameControls( float angleX, float angleY );
	void FlameDestroy( void );
	inline bool FlameIsOn() const { return m_pFlame[ 0 ] != nullptr; }

	void FlameDamage( Vector vecStart, Vector vecEnd, CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, EntityClassification_t iClassIgnore, int bitsDamageType );

	DECLARE_SCHEDULES() override;

private:
	static const char *pAttackHitSounds[];
	static const char *pBeamAttackSounds[];
	static const char *pAttackMissSounds[];
	static const char *pRicSounds[];
	static const char *pFootSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pAttackSounds[];
	static const char *pStompSounds[];
	static const char *pBreatheSounds[];

	CBaseEntity* GargantuaCheckTraceHullAttack( float flDist, int iDamage, int iDmgType );

	CSprite		*m_pEyeGlow;		// Glow around the eyes
	CBeam		*m_pFlame[ 4 ];		// Flame beams

	int			m_eyeBrightness;	// Brightness target
	float		m_seeTime;			// Time to attack (when I see the enemy, I set this)
	float		m_flameTime;		// Time of next flame attack
	float		m_painSoundTime;	// Time of next pain sound
	float		m_streakTime;		// streak timer (don't send too many)
	float		m_flameX;			// Flame thrower aim
	float		m_flameY;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CGARGANTUA_H