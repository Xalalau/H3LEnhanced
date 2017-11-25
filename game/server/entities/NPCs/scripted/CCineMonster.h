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
#ifndef GAME_SERVER_ENTITIES_NPCS_SCRIPTED_CCINEMONSTER_H
#define GAME_SERVER_ENTITIES_NPCS_SCRIPTED_CCINEMONSTER_H

#define SF_SCRIPT_WAITTILLSEEN		1
#define SF_SCRIPT_EXITAGITATED		2
#define SF_SCRIPT_REPEATABLE		4
#define SF_SCRIPT_LEAVECORPSE		8
//#define SF_SCRIPT_INTERPOLATE		16 // don't use, old bug
#define SF_SCRIPT_NOINTERRUPT		32
#define SF_SCRIPT_OVERRIDESTATE		64
#define SF_SCRIPT_NOSCRIPTMOVEMENT	128

#define SCRIPT_BREAK_CONDITIONS		(bits_COND_LIGHT_DAMAGE|bits_COND_HEAVY_DAMAGE)

enum SS_INTERRUPT
{
	SS_INTERRUPT_IDLE = 0,
	SS_INTERRUPT_BY_NAME,
	SS_INTERRUPT_AI,
};

/*
classname "scripted_sequence"
targetname "me" - there can be more than one with the same name, and they act in concert
target "the_entity_I_want_to_start_playing" or "class entity_classname" will pick the closest inactive scientist
play "name_of_sequence"
idle "name of idle sequence to play before starting"
donetrigger "whatever" - can be any other triggerable entity such as another sequence, train, door, or a special case like "die" or "remove"
moveto - if set the monster first moves to this nodes position
range # - only search this far to find the target
spawnflags - (stop if blocked, stop if player seen)
*/
class CCineMonster : public CBaseMonster
{
public:
	DECLARE_CLASS( CCineMonster, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	virtual void KeyValue( KeyValueData *pkvd ) override;
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	virtual void Blocked( CBaseEntity *pOther ) override;
	virtual void Touch( CBaseEntity *pOther ) override;
	virtual int	 ObjectCaps() const override { return ( CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ); }
	virtual void Activate( void ) override;

	// void CineSpawnThink( void );
	void CineThink( void );
	void Pain( void );
	void Die( void );
	void DelayStart( const bool bState );
	bool FindEntity();
	virtual void PossessEntity( void );

	void ReleaseEntity( CBaseMonster *pEntity );
	void CancelScript( void );
	virtual bool StartSequence( CBaseMonster *pTarget, int iszSeq, const bool completeOnEmpty );
	virtual bool FCanOverrideState() const;
	void SequenceDone( CBaseMonster *pMonster );
	virtual void FixScriptMonsterSchedule( CBaseMonster *pMonster );
	bool	CanInterrupt() const;
	void	AllowInterrupt( const bool fAllow );
	int		IgnoreConditions( void ) override;

	int	m_iszIdle;		// string index for idle animation
	int	m_iszPlay;		// string index for scripted animation
	int m_iszEntity;	// entity that is wanted for this script
	int m_fMoveTo;
	int m_iFinishSchedule;
	float m_flRadius;		// range to search
	float m_flRepeat;	// repeat rate

	int m_iDelay;
	float m_startTime;

	MoveType	m_saved_movetype;
	Solid	m_saved_solid;
	int m_saved_effects;
	//	Vector m_vecOrigOrigin;
	bool m_interruptable;
};

#endif //GAME_SERVER_ENTITIES_NPCS_SCRIPTED_CCINEMONSTER_H