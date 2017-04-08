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
#ifndef GAME_SERVER_ENTITIES_NPCS_BASEMONSTER_H
#define GAME_SERVER_ENTITIES_NPCS_BASEMONSTER_H

#include "Monsters.h"

#define	ROUTE_SIZE			8 // how many waypoints a monster can store at one time
#define MAX_OLD_ENEMIES		4 // how many old enemies to remember

#define	bits_CAP_DUCK			( 1 << 0 )// crouch
#define	bits_CAP_JUMP			( 1 << 1 )// jump/leap
#define bits_CAP_STRAFE			( 1 << 2 )// strafe ( walk/run sideways)
#define bits_CAP_SQUAD			( 1 << 3 )// can form squads
#define	bits_CAP_SWIM			( 1 << 4 )// proficiently navigate in water
#define bits_CAP_CLIMB			( 1 << 5 )// climb ladders/ropes
#define bits_CAP_USE			( 1 << 6 )// open doors/push buttons/pull levers
#define bits_CAP_HEAR			( 1 << 7 )// can hear forced sounds
#define bits_CAP_AUTO_DOORS		( 1 << 8 )// can trigger auto doors
#define bits_CAP_OPEN_DOORS		( 1 << 9 )// can open manual doors
#define bits_CAP_TURN_HEAD		( 1 << 10)// can turn head, always bone controller 0

#define bits_CAP_RANGE_ATTACK1	( 1 << 11)// can do a range attack 1
#define bits_CAP_RANGE_ATTACK2	( 1 << 12)// can do a range attack 2
#define bits_CAP_MELEE_ATTACK1	( 1 << 13)// can do a melee attack 1
#define bits_CAP_MELEE_ATTACK2	( 1 << 14)// can do a melee attack 2

#define bits_CAP_FLY			( 1 << 15)// can fly, move all around

#define bits_CAP_DOORS_GROUP    (bits_CAP_USE | bits_CAP_AUTO_DOORS | bits_CAP_OPEN_DOORS)

enum MONSTERSTATE
{
	MONSTERSTATE_NONE = 0,
	MONSTERSTATE_IDLE,
	MONSTERSTATE_COMBAT,
	MONSTERSTATE_ALERT,
	MONSTERSTATE_HUNT,
	MONSTERSTATE_PRONE,
	MONSTERSTATE_SCRIPT,
	MONSTERSTATE_PLAYDEAD,
	MONSTERSTATE_DEAD
};

/**
*	These MoveFlag values are assigned to a WayPoint's TYPE in order to demonstrate the
*	type of movement the monster should use to get there.
*/
enum MoveFlag
{
	/**
	*	Local move to targetent.
	*/
	bits_MF_TO_TARGETENT	= 1 << 0,

	/**
	*	Local move to enemy.
	*/
	bits_MF_TO_ENEMY		= 1 << 1,

	/**
	*	Local move to a hiding place.
	*/
	bits_MF_TO_COVER		= 1 << 2,

	/**
	*	Local move to detour point.
	*/
	bits_MF_TO_DETOUR		= 1 << 3,

	/**
	*	Local move to a path corner.
	*/
	bits_MF_TO_PATHCORNER	= 1 << 4,

	/**
	*	Local move to a node.
	*/
	bits_MF_TO_NODE			= 1 << 5,

	/**
	*	Local move to an arbitrary point.
	*/
	bits_MF_TO_LOCATION		= 1 << 6,

	/**
	*	This waypoint is the goal of the whole move.
	*/
	bits_MF_IS_GOAL			= 1 << 7,

	/**
	*	Don't let the route code simplify this waypoint.
	*/
	bits_MF_DONT_SIMPLIFY	= 1 << 8,

	/**
	*	If you define any flags that aren't _TO_ flags, add them here so we can mask
	*	them off when doing compares.
	*/
	bits_MF_NOT_TO_MASK = ( bits_MF_IS_GOAL | bits_MF_DONT_SIMPLIFY )
};

enum MoveGoal
{
	MOVEGOAL_NONE			= 0,
	MOVEGOAL_TARGETENT		= bits_MF_TO_TARGETENT,
	MOVEGOAL_ENEMY			= bits_MF_TO_ENEMY,
	MOVEGOAL_PATHCORNER		= bits_MF_TO_PATHCORNER,
	MOVEGOAL_LOCATION		= bits_MF_TO_LOCATION,
	MOVEGOAL_NODE			= bits_MF_TO_NODE,
};

/**
*	An array of waypoints makes up the monster's route. 
*/
struct WayPoint_t
{
	Vector	vecLocation;

	/**
	*	Waypoint type.
	*	@see MoveFlag
	*/
	int		iType;
};

/**
*	Generic Monster
*/
class CBaseMonster : public CBaseCombatCharacter
{
private:
	/**
	*	Monster conditions. Bit vector of MonsterCondition flags.
	*	@see MonsterCondition
	*	@see SetConditions
	*	@see ClearConditions
	*	@see HasConditions
	*	@see HasAllConditions
	*/
	int m_afConditions;

public:
	DECLARE_CLASS( CBaseMonster, CBaseToggle );
	DECLARE_DATADESC();

		enum SCRIPTSTATE
		{
			SCRIPT_PLAYING = 0,		// Playing the sequence
			SCRIPT_WAIT,				// Waiting on everyone in the script to be ready
			SCRIPT_CLEANUP,					// Cancelling the script / cleaning up
			SCRIPT_WALK_TO_MARK,
			SCRIPT_RUN_TO_MARK,
		};


	
		// these fields have been added in the process of reworking the state machine. (sjb)
		EHANDLE				m_hEnemy;		 // the entity that the monster is fighting.
		EHANDLE				m_hTargetEnt;	 // the entity that the monster is trying to reach
		EHANDLE				m_hOldEnemy[ MAX_OLD_ENEMIES ];
		Vector				m_vecOldEnemy[ MAX_OLD_ENEMIES ];

		float				m_flFieldOfView;// width of monster's field of view ( dot product )
		float				m_flWaitFinished;// if we're told to wait, this is the time that the wait will be over.
		float				m_flMoveWaitFinished;

		Activity			m_Activity;// what the monster is doing (animation)
		Activity			m_IdealActivity;// monster should switch to this activity
		
		int					m_LastHitGroup; // the last body region that took damage
		
		MONSTERSTATE		m_MonsterState;// monster's current state
		MONSTERSTATE		m_IdealMonsterState;// monster should change to this state
	
		int					m_iTaskStatus;
		Schedule_t			*m_pSchedule;
		size_t				m_iScheduleIndex;

		WayPoint_t			m_Route[ ROUTE_SIZE ];	// Positions of movement
		int					m_movementGoal;			// Goal that defines route
		int					m_iRouteIndex;			// index into m_Route[]
		float				m_moveWaitTime;			// How long I should wait for something to move

		Vector				m_vecMoveGoal; // kept around for node graph moves, so we know our ultimate goal
		Activity			m_movementActivity;	// When moving, set this activity

		int					m_iAudibleList; // first index of a linked list of sounds that the monster can hear.
		int					m_afSoundTypes;

		Vector				m_vecLastPosition;// monster sometimes wants to return to where it started after an operation.

		int					m_iHintNode; // this is the hint node that the monster is moving towards or performing active idle on.

		int					m_afMemory;

		int					m_iMaxHealth;// keeps track of monster's maximum health value (for re-healing, etc)

	Vector				m_vecEnemyLKP;// last known position of enemy. (enemy's origin)

	int					m_cAmmoLoaded;		// how much ammo is in the weapon (used to trigger reload anim sequences)

	int					m_afCapability;// tells us what a monster can/can't do.

	float				m_flNextAttack;		// cannot attack again until this time

	int					m_bloodColor;		// color of blood particless

	int					m_failSchedule;				// Schedule type to choose if current schedule fails

	float				m_flHungryTime;// set this is a future time to stop the monster from eating for a while. 

	float				m_flDistTooFar;	// if enemy farther away than this, bits_COND_ENEMY_TOOFAR set in CheckEnemy
	float				m_flDistLook;	// distance monster sees (Default 2048)

	int					m_iTriggerCondition;// for scripted AI, this is the condition that will cause the activation of the monster's TriggerTarget
	string_t			m_iszTriggerTarget;// name of target that should be fired. 

	Vector				m_HackedGunPos;	// HACK until we can query end of gun

// Scripted sequence Info
	SCRIPTSTATE			m_scriptState;		// internal cinematic state
	CCineMonster		*m_pCine;

	virtual bool Restore( CRestore& restore ) override;

	void KeyValue( KeyValueData *pkvd ) override;

// monster use function
	void MonsterUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

// overrideable Monster member functions
	
	virtual int BloodColor() const override { return m_bloodColor; }

	virtual CBaseMonster *MyMonsterPointer( void ) override { return this; }
	virtual void Look ( int iDistance );// basic sight function for monsters
	virtual void RunAI ( void );// core ai function!	
	void Listen ( void );

	virtual bool	IsAlive() const override { return (pev->deadflag != DEAD_DEAD); }

// Basic Monster AI functions
	virtual float ChangeYaw ( int speed );
	float VecToYaw( Vector vecDir );
	float FlYawDiff() const; 

	float DamageForce( float damage );

// stuff written for new state machine
		virtual void MonsterThink( void );
		void CallMonsterThink( void ) { this->MonsterThink(); }
		virtual Relationship IRelationship( CBaseEntity *pTarget );
		virtual void MonsterInit ( void );
		virtual void MonsterInitDead( void );	// Call after animation/pose is set up
		virtual void BecomeDead( void );
		void CorpseFallThink( void );

		void MonsterInitThink ( void );
		virtual void StartMonster ( void );
		virtual CBaseEntity* BestVisibleEnemy ( void );// finds best visible enemy for attack
		virtual bool FInViewCone( const CBaseEntity *pEntity ) const;// see if pEntity is in monster's view cone
		virtual bool FInViewCone( const Vector& vecOrigin ) const;// see if given location is in monster's view cone
		virtual void HandleAnimEvent( AnimEvent_t& event ) override;

		virtual int CheckLocalMove ( const Vector &vecStart, const Vector &vecEnd, const CBaseEntity* const pTarget, float *pflDist );// check validity of a straight move through space
		virtual void Move( float flInterval = 0.1 );
		virtual void MoveExecute( CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval );
		virtual bool ShouldAdvanceRoute( float flWaypointDist );

		virtual Activity GetStoppedActivity( void ) { return ACT_IDLE; }
		virtual void Stop( void ) { m_IdealActivity = GetStoppedActivity(); }

		// This will stop animation until you call ResetSequenceInfo() at some point in the future
		inline void StopAnimation( void ) { pev->framerate = 0; }

		// these functions will survey conditions and set appropriate conditions bits for attack types.
		virtual bool CheckRangeAttack1( float flDot, float flDist );
		virtual bool CheckRangeAttack2( float flDot, float flDist );
		virtual bool CheckMeleeAttack1( float flDot, float flDist );
		virtual bool CheckMeleeAttack2( float flDot, float flDist );

		bool FHaveSchedule() const;
		bool FScheduleValid() const;
		void ClearSchedule( void );
		bool FScheduleDone() const;
		void ChangeSchedule ( Schedule_t *pNewSchedule );
		void NextScheduledTask ( void );
		const Schedule_t* ScheduleInList( const char* const pszName, const Schedule_t* const* pList, size_t listCount ) const;

		/*
		*	Schedule list.
		*/
		//TODO: the ifdef is temporary until the client no longer references monster code. - Solokiller
#ifndef CLIENT_DLL
		DECLARE_SCHEDULES();
#endif

		/**
		*	Gets a schedule by name.
		*/
		const Schedule_t* ScheduleFromName( const char* const pszName ) const;
		
		void MaintainSchedule ( void );
		virtual void StartTask( const Task_t* pTask );
		virtual void RunTask( const Task_t* pTask );
		virtual Schedule_t *GetScheduleOfType( int Type );
		virtual Schedule_t *GetSchedule( void );
		virtual void ScheduleChange( void ) {}
		// virtual bool CanPlaySequence() const { return ((m_pCine == NULL) && (m_MonsterState == MONSTERSTATE_NONE || m_MonsterState == MONSTERSTATE_IDLE || m_IdealMonsterState == MONSTERSTATE_IDLE)); }
		virtual bool CanPlaySequence( const bool fDisregardState, int interruptLevel ) const;
		virtual bool CanPlaySentence( const bool fDisregardState ) const { return IsAlive(); }
		virtual void PlaySentence( const char *pszSentence, float duration, float volume, float attenuation );
		virtual void PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, const bool bConcurrent, CBaseEntity *pListener );

		virtual void SentenceStop( void );

		const Task_t* GetTask() const;
		virtual MONSTERSTATE GetIdealState ( void );
		virtual void SetActivity ( Activity NewActivity );
		void SetSequenceByName( const char* const pszSequence );
		void SetState ( MONSTERSTATE State );
		virtual void ReportAIState( void );

		void CheckAttacks ( CBaseEntity *pTarget, float flDist );
		virtual bool CheckEnemy ( CBaseEntity* pEnemy );
		void PushEnemy( CBaseEntity *pEnemy, Vector &vecLastKnownPos );
		bool PopEnemy();

		bool FGetNodeRoute( const Vector& vecDest );
		
		inline void TaskComplete( void ) { if ( !HasConditions(bits_COND_TASK_FAILED) ) m_iTaskStatus = TASKSTATUS_COMPLETE; }
		void MovementComplete( void );
		inline void TaskFail( void ) { SetConditions(bits_COND_TASK_FAILED); }
		inline void TaskBegin( void ) { m_iTaskStatus = TASKSTATUS_RUNNING; }
		bool TaskIsRunning() const;
		inline bool TaskIsComplete() const { return (m_iTaskStatus == TASKSTATUS_COMPLETE); }
		inline bool MovementIsComplete() const { return (m_movementGoal == MOVEGOAL_NONE); }

		int IScheduleFlags ( void );
		bool FRefreshRoute();
		bool FRouteClear() const;
		void RouteSimplify( const CBaseEntity* const pTargetEnt );
		void AdvanceRoute ( float distance );
		virtual bool FTriangulate( const Vector &vecStart , const Vector &vecEnd, float flDist, const CBaseEntity* const pTargetEnt, Vector *pApex );
		void MakeIdealYaw( Vector vecTarget );
		virtual void SetYawSpeed ( void ) { return; };// allows different yaw_speeds for each activity
		bool BuildRoute( const Vector &vecGoal, int iMoveFlag, const CBaseEntity* const pTarget );
		virtual bool BuildNearestRoute( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist );
		int RouteClassify( int iMoveFlag );
		void InsertWaypoint ( Vector vecLocation, int afMoveFlags );
		
		bool FindLateralCover( const Vector &vecThreat, const Vector &vecViewOffset );
		virtual bool FindCover( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist );
		virtual bool FValidateCover ( const Vector &vecCoverLocation ) { return true; }
		virtual float CoverRadius( void ) { return 784; } // Default cover radius

		virtual bool FCanCheckAttacks() const;
		virtual void CheckAmmo( void ) { return; };
		virtual int IgnoreConditions ( void );
		
		inline void	SetConditions( int iConditions ) { m_afConditions |= iConditions; }
		inline void	ClearConditions( int iConditions ) { m_afConditions &= ~iConditions; }
		inline bool HasConditions( int iConditions ) const { return ( m_afConditions & iConditions ) != 0; }
		inline bool HasAllConditions( int iConditions ) const { return ( ( m_afConditions & iConditions ) == iConditions ); }

		virtual bool FValidateHintType( short sHint ) const;
		int FindHintNode ( void );
		virtual bool FCanActiveIdle() const;
		void SetTurnActivity ( void );
		float FLSoundVolume ( CSound *pSound );

		bool MoveToNode( Activity movementAct, float waitTime, const Vector &goal );
		bool MoveToTarget( Activity movementAct, float waitTime );
		bool MoveToLocation( Activity movementAct, float waitTime, const Vector &goal );
		bool MoveToEnemy( Activity movementAct, float waitTime );

		// Returns the time when the door will be open
		float	OpenDoorAndWait( CBaseEntity* pDoor );

		virtual int ISoundMask( void );
		virtual CSound* PBestSound ( void );
		virtual CSound* PBestScent ( void );
		virtual float HearingSensitivity( void ) { return 1.0; };

		bool BarnacleVictimGrabbed( CBaseEntity* pBarnacle ) override;
		void BarnacleVictimBitten( CBaseEntity* pBarnacle ) override;
		void BarnacleVictimReleased() override;

		void SetEyePosition ( void );

		//TODO: should this be in a mixin? - Solokiller
		bool FShouldEat() const;// see if a monster is 'hungry'
		void Eat ( float flFullDuration );// make the monster 'full' for a while.

		CBaseEntity *CheckTraceHullAttack( float flDist, int iDamage, int iDmgType );
		bool FacingIdeal() const;

		bool FCheckAITrigger();// checks and, if necessary, fires the monster's trigger target. 
		
		/**
		*	@return Whether the monster would not hit a friendly if it were to attack a target in front of it.
		*/
		virtual bool NoFriendlyFire() { return true; }

		bool BBoxFlat() const;

		// PrescheduleThink 
		virtual void PrescheduleThink( void ) { return; };

		bool GetEnemy();
		void MakeDamageBloodDecal ( int cCount, float flNoise, TraceResult *ptr, const Vector &vecDir );
		void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr ) override;

	// combat functions
	virtual Activity GetDeathActivity ( void );
	Activity GetSmallFlinchActivity( void );
	virtual void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;
	virtual void GibMonster( void );
	bool ShouldGibMonster( GibAction gibAction ) const;
	void		 CallGibMonster( void );
	virtual bool	HasHumanGibs();
	virtual bool	HasAlienGibs();
	virtual void	FadeMonster( void );	// Called instead of GibMonster() when gibs are disabled

	Vector ShootAtEnemy( const Vector &shootOrigin );
	virtual Vector BodyTarget( const Vector &posSrc ) const override { return Center( ) * 0.75 + EyePosition() * 0.25; }		// position to shoot at

	virtual	Vector GetGunPosition( void ) override;

	virtual void OnTakeDamage( const CTakeDamageInfo& info ) override;
	void DeadTakeDamage( const CTakeDamageInfo& info );

	void RadiusDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, EntityClassification_t iClassIgnore, int bitsDamageType );
	void RadiusDamage( Vector vecSrc, CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, EntityClassification_t iClassIgnore, int bitsDamageType );
	virtual bool IsMoving() const override { return m_movementGoal != MOVEGOAL_NONE; }

	void RouteClear( void );
	void RouteNew( void );
	
	virtual void DeathSound ( void ) { return; };
	virtual void AlertSound ( void ) { return; };
	virtual void IdleSound ( void ) { return; };
	virtual void PainSound ( void ) { return; };
	
	virtual void StopFollowing( const bool clearSchedule ) {}

	inline void	Remember( int iMemory ) { m_afMemory |= iMemory; }
	inline void	Forget( int iMemory ) { m_afMemory &= ~iMemory; }
	inline bool HasMemory( int iMemory ) const { return ( m_afMemory & iMemory ) != 0; }
	inline bool HasAllMemories( int iMemory ) const { return ( (m_afMemory & iMemory) == iMemory ); }

	bool ExitScriptedSequence();
	bool CineCleanup();

	CBaseEntity* DropItem ( const char* const pszItemName, const Vector &vecPos, const Vector &vecAng );// drop an item.

	//Shock effect begin. - Solokiller
	float m_flShockDuration;
	float m_flShockTime;

	bool m_bShockEffect;

	RenderMode m_OldRenderMode;
	RenderFX m_OldRenderFX;
	Vector m_OldRenderColor;
	float m_flOldRenderAmt;

	/**
	*	Adds a shock effect to this monster.
	*/
	void AddShockEffect( float r, float g, float b, float size, float flShockDuration );

	void UpdateShockEffect();

	//Shock effect end. - Solokiller
};

#endif //GAME_SERVER_ENTITIES_NPCS_BASEMONSTER_H
