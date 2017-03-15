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
#ifndef GAME_SERVER_ENTITIES_NPCS_CBIGMOMMA_H
#define GAME_SERVER_ENTITIES_NPCS_CBIGMOMMA_H

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	BIG_AE_STEP1				1		// Footstep left
#define	BIG_AE_STEP2				2		// Footstep right
#define	BIG_AE_STEP3				3		// Footstep back left
#define	BIG_AE_STEP4				4		// Footstep back right
#define BIG_AE_SACK					5		// Sack slosh
#define BIG_AE_DEATHSOUND			6		// Death sound

#define	BIG_AE_MELEE_ATTACKBR		8		// Leg attack
#define	BIG_AE_MELEE_ATTACKBL		9		// Leg attack
#define	BIG_AE_MELEE_ATTACK1		10		// Leg attack
#define BIG_AE_MORTAR_ATTACK1		11		// Launch a mortar
#define BIG_AE_LAY_CRAB				12		// Lay a headcrab
#define BIG_AE_JUMP_FORWARD			13		// Jump up and forward
#define BIG_AE_SCREAM				14		// alert sound
#define BIG_AE_PAIN_SOUND			15		// pain sound
#define BIG_AE_ATTACK_SOUND			16		// attack sound
#define BIG_AE_BIRTH_SOUND			17		// birth sound
#define BIG_AE_EARLY_TARGET			50		// Fire target early

// User defined conditions
#define bits_COND_NODE_SEQUENCE			( bits_COND_SPECIAL1 )		// pev->netname contains the name of a sequence to play

// Attack distance constants
#define	BIG_ATTACKDIST		170
#define BIG_MORTARDIST		800
// ############ hu3lifezado ############ //
// Mais aliens (20)
#define BIG_MAXCHILDREN		100			// Max # of live headcrab children
// ############ //

#define bits_MEMORY_CHILDPAIR		(bits_MEMORY_CUSTOM1)
#define bits_MEMORY_ADVANCE_NODE	(bits_MEMORY_CUSTOM2)
#define bits_MEMORY_COMPLETED_NODE	(bits_MEMORY_CUSTOM3)
#define bits_MEMORY_FIRED_NODE		(bits_MEMORY_CUSTOM4)

class CBigMomma : public CBaseMonster
{
public:
	DECLARE_CLASS( CBigMomma, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void Activate( void ) override;
	void OnTakeDamage( const CTakeDamageInfo& info ) override;

	void		RunTask( const Task_t* pTask ) override;
	void		StartTask( const Task_t* pTask ) override;
	Schedule_t	*GetSchedule( void ) override;
	Schedule_t	*GetScheduleOfType( int Type ) override;
	void		TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr ) override;

	void NodeStart( int iszNextNode );
	void NodeReach( void );
	bool ShouldGoToNode() const;

	void SetYawSpeed( void ) override;
	int  Classify( void ) override;
	void HandleAnimEvent( AnimEvent_t& event ) override;
	void LayHeadcrab( void );

	int GetNodeSequence( void )
	{
		CBaseEntity *pTarget = m_hTargetEnt;
		if( pTarget )
		{
			return pTarget->pev->netname;	// netname holds node sequence
		}
		return 0;
	}


	int GetNodePresequence( void )
	{
		CInfoBM *pTarget = ( CInfoBM * ) ( CBaseEntity * ) m_hTargetEnt;
		if( pTarget )
		{
			return pTarget->m_preSequence;
		}
		return 0;
	}

	float GetNodeDelay( void )
	{
		CBaseEntity *pTarget = m_hTargetEnt;
		if( pTarget )
		{
			return pTarget->pev->speed;	// Speed holds node delay
		}
		return 0;
	}

	float GetNodeRange( void )
	{
		CBaseEntity *pTarget = m_hTargetEnt;
		if( pTarget )
		{
			return pTarget->pev->scale;	// Scale holds node delay
		}
		return 1e6;
	}

	float GetNodeYaw( void )
	{
		CBaseEntity *pTarget = m_hTargetEnt;
		if( pTarget )
		{
			if( pTarget->pev->angles.y != 0 )
				return pTarget->pev->angles.y;
		}
		return pev->angles.y;
	}

	// Restart the crab count on each new level
	void OverrideReset( void ) override
	{
		m_crabCount = 0;
	}

	void DeathNotice( CBaseEntity* pChild ) override;

	bool CanLayCrab() const
	{
		if( m_crabTime < gpGlobals->time && m_crabCount < BIG_MAXCHILDREN )
		{
			// Don't spawn crabs inside each other
			Vector mins = GetAbsOrigin() - Vector( 32, 32, 0 );
			Vector maxs = GetAbsOrigin() + Vector( 32, 32, 0 );

			CBaseEntity *pList[ 2 ];
			int count = UTIL_EntitiesInBox( pList, 2, mins, maxs, FL_MONSTER );
			for( int i = 0; i < count; i++ )
			{
				if( pList[ i ] != this )	// Don't hurt yourself!
					return false;
			}
			return true;
		}

		return false;
	}

	void LaunchMortar( void );

	void SetObjectCollisionBox( void ) override
	{
		pev->absmin = GetAbsOrigin() + Vector( -95, -95, 0 );
		pev->absmax = GetAbsOrigin() + Vector( 95, 95, 190 );
	}

	bool CheckMeleeAttack1( float flDot, float flDist ) override;	// Slash
	bool CheckMeleeAttack2( float flDot, float flDist ) override;	// Lay a crab
	bool CheckRangeAttack1( float flDot, float flDist ) override;	// Mortar launch

	static const char *pChildDieSounds[];
	static const char *pSackSounds[];
	static const char *pDeathSounds[];
	static const char *pAttackSounds[];
	static const char *pAttackHitSounds[];
	static const char *pBirthSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pFootSounds[];

	DECLARE_SCHEDULES() override;

private:
	float	m_nodeTime;
	float	m_crabTime;
	float	m_mortarTime;
	float	m_painSoundTime;
	int		m_crabCount;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CBIGMOMMA_H