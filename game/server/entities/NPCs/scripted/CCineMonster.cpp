#include "extdll.h"
#include "util.h"
#include "animation.h"
#include "cbase.h"
#include "entities/NPCs/CBaseMonster.h"

#include "CCineMonster.h"

#define CLASSNAME "scripted_sequence"

void ScriptEntityCancel( CBaseEntity* pCine )
{
	// make sure they are a scripted_sequence
	if( pCine->ClassnameIs( CLASSNAME ) )
	{
		CCineMonster *pCineTarget = static_cast<CCineMonster*>( pCine );
		// make sure they have a monster in mind for the script
		CBaseEntity		*pEntity = pCineTarget->m_hTargetEnt;
		CBaseMonster	*pTarget = NULL;
		if( pEntity )
			pTarget = pEntity->MyMonsterPointer();

		if( pTarget )
		{
			// make sure their monster is actually playing a script
			if( pTarget->m_MonsterState == MONSTERSTATE_SCRIPT )
			{
				// tell them do die
				pTarget->m_scriptState = CCineMonster::SCRIPT_CLEANUP;
				// do it now
				pTarget->CineCleanup();
			}
		}
	}
}

BEGIN_DATADESC( CCineMonster )
	DEFINE_FIELD( m_iszIdle, FIELD_STRING ),
	DEFINE_FIELD( m_iszPlay, FIELD_STRING ),
	DEFINE_FIELD( m_iszEntity, FIELD_STRING ),
	DEFINE_FIELD( m_fMoveTo, FIELD_INTEGER ),
	DEFINE_FIELD( m_flRepeat, FIELD_FLOAT ),
	DEFINE_FIELD( m_flRadius, FIELD_FLOAT ),
	
	DEFINE_FIELD( m_iDelay, FIELD_INTEGER ),
	DEFINE_FIELD( m_startTime, FIELD_TIME ),
	
	DEFINE_FIELD( m_saved_movetype, FIELD_INTEGER ),
	DEFINE_FIELD( m_saved_solid, FIELD_INTEGER ),
	DEFINE_FIELD( m_saved_effects, FIELD_INTEGER ),
	DEFINE_FIELD( m_iFinishSchedule, FIELD_INTEGER ),
	DEFINE_FIELD( m_interruptable, FIELD_BOOLEAN ),
	DEFINE_THINKFUNC( CineThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( scripted_sequence, CCineMonster );

void CCineMonster::Spawn( void )
{
	// pev->solid = SOLID_TRIGGER;
	// SetSize( Vector(-8, -8, -8), Vector(8, 8, 8));
	pev->solid = SOLID_NOT;


	// REMOVE: The old side-effect
#if 0
	if( m_iszIdle )
		m_fMoveTo = 4;
#endif

	// if no targetname, start now
	if( !HasTargetname() || !FStringNull( m_iszIdle ) )
	{
		SetThink( &CCineMonster::CineThink );
		pev->nextthink = gpGlobals->time + 1.0;
		// Wait to be used?
		if( HasTargetname() )
			m_startTime = gpGlobals->time + 1E6;
	}
	if( pev->spawnflags & SF_SCRIPT_NOINTERRUPT )
		m_interruptable = false;
	else
		m_interruptable = true;
}

//
// Cache user-entity-field values until spawn is called.
//
void CCineMonster::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "m_iszIdle" ) )
	{
		m_iszIdle = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "m_iszPlay" ) )
	{
		m_iszPlay = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "m_iszEntity" ) )
	{
		m_iszEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "m_fMoveTo" ) )
	{
		m_fMoveTo = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "m_flRepeat" ) )
	{
		m_flRepeat = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "m_flRadius" ) )
	{
		m_flRadius = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "m_iFinishSchedule" ) )
	{
		m_iFinishSchedule = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
	{
		CBaseMonster::KeyValue( pkvd );
	}
}

//
// CineStart
//
void CCineMonster::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// do I already know who I should use
	CBaseEntity		*pEntity = m_hTargetEnt;
	CBaseMonster	*pTarget = NULL;

	if( pEntity )
		pTarget = pEntity->MyMonsterPointer();

	if( pTarget )
	{
		// am I already playing the script?
		if( pTarget->m_scriptState == SCRIPT_PLAYING )
			return;

		m_startTime = gpGlobals->time + 0.05;
	}
	else
	{
		// if not, try finding them
		SetThink( &CCineMonster::CineThink );
		pev->nextthink = gpGlobals->time;
	}
}

// This doesn't really make sense since only MOVETYPE_PUSH get 'Blocked' events
void CCineMonster::Blocked( CBaseEntity *pOther )
{
}

void CCineMonster::Touch( CBaseEntity *pOther )
{
	/*
	ALERT( at_aiconsole, "Cine Touch\n" );
	if( m_hTargetEnt && pOther == m_hTargetEnt )
	{
		CBaseMonster *pTarget = EHANDLE_cast<CBaseMonster*>( m_hTargetEnt );
		pTarget->m_monsterState == MONSTERSTATE_SCRIPT;
	}
	*/
}

/*
if( !FBitSet( pOther->pev->flags, FL_MONSTER ) )
{
	// touched by a non-monster.
	return;
}

pOther->GetAbsOrigin().z += 1;

if( FBitSet ( pOther->pev->flags, FL_ONGROUND ) )
{
	// clear the onground so physics don't bitch
	pOther->pev->flags -= FL_ONGROUND;
}

// toss the monster!
pOther->pev->velocity = pev->movedir * pev->speed;
pOther->pev->velocity.z += m_flHeight;


pev->solid = SOLID_NOT;// kill the trigger for now !!!UNDONE
}
*/

// Find an entity that I'm interested in and precache the sounds he'll need in the sequence.
void CCineMonster::Activate( void )
{
	CBaseMonster* pTarget = nullptr;

	CBaseEntity* pNextTarget = nullptr;

	// The entity name could be a target name or a classname
	// Check the targetname
	while( !pTarget && ( pNextTarget = UTIL_FindEntityByTargetname( pNextTarget, STRING( m_iszEntity ) ) ) )
	{
		if( pNextTarget->GetFlags().Any( FL_MONSTER ) )
		{
			pTarget = pNextTarget->MyMonsterPointer();
		}
	}

	// If no entity with that targetname, check the classname
	if( !pTarget )
	{
		pNextTarget = nullptr;
		while( !pTarget && ( pNextTarget = UTIL_FindEntityByClassname( pNextTarget, STRING( m_iszEntity ) ) ) )
		{
			pTarget = pNextTarget->MyMonsterPointer();
		}
	}

	// Found a compatible entity
	if( pTarget )
	{
		void *pmodel;
		pmodel = GET_MODEL_PTR( pTarget->edict() );
		if( pmodel )
		{
			// Look through the event list for stuff to precache
			SequencePrecache( pmodel, STRING( m_iszIdle ) );
			SequencePrecache( pmodel, STRING( m_iszPlay ) );
		}
	}
}

//
// ********** Cinematic Think **********
//
void CCineMonster::CineThink( void )
{
	if( FindEntity() )
	{
		PossessEntity();
		ALERT( at_aiconsole, "script \"%s\" using monster \"%s\"\n", GetTargetname(), STRING( m_iszEntity ) );
	}
	else
	{
		CancelScript();
		ALERT( at_aiconsole, "script \"%s\" can't find monster \"%s\"\n", GetTargetname(), STRING( m_iszEntity ) );
		pev->nextthink = gpGlobals->time + 1.0;
	}
}

//
// ********** Cinematic PAIN **********
//
void CCineMonster::Pain( void )
{
}

//
// ********** Cinematic DIE **********
//
void CCineMonster::Die( void )
{
	SetThink( &CCineMonster::SUB_Remove );
}

// find all the cinematic entities with my targetname and tell them to wait before starting
void CCineMonster::DelayStart( const bool bState )
{
	CBaseEntity* pCine = nullptr;

	while( (pCine = UTIL_FindEntityByTargetname( pCine, GetTargetname() )) )
	{
		if( pCine->ClassnameIs( "scripted_sequence" ) )
		{
			CCineMonster *pTarget = static_cast<CCineMonster*>( pCine );
			if( bState )
			{
				pTarget->m_iDelay++;
			}
			else
			{
				pTarget->m_iDelay--;
				if( pTarget->m_iDelay <= 0 )
					pTarget->m_startTime = gpGlobals->time + 0.05;
			}
		}
	}
}

// find a viable entity
bool CCineMonster::FindEntity()
{
	m_hTargetEnt = nullptr;

	CBaseEntity* pTargetEnt = nullptr;
	CBaseMonster* pTarget = nullptr;

	while( (pTargetEnt = UTIL_FindEntityByTargetname( pTargetEnt, STRING( m_iszEntity ) )) )
	{
		if( pTargetEnt->GetFlags().Any( FL_MONSTER ) )
		{
			pTarget = pTargetEnt->MyMonsterPointer();

			if( pTarget && pTarget->CanPlaySequence( FCanOverrideState(), SS_INTERRUPT_BY_NAME ) )
			{
				m_hTargetEnt = pTarget;
				return true;
			}

			ALERT( at_console, "Found %s, but can't play!\n", STRING( m_iszEntity ) );
		}

		pTarget = nullptr;
	}

	if( !pTarget )
	{
		pTargetEnt = nullptr;
		while( (pTargetEnt = UTIL_FindEntityInSphere( pTargetEnt, GetAbsOrigin(), m_flRadius )) )
		{
			if( pTargetEnt->ClassnameIs( STRING( m_iszEntity ) ) )
			{
				if( pTargetEnt->GetFlags().Any( FL_MONSTER ) )
				{
					pTarget = pTargetEnt->MyMonsterPointer();
					if( pTarget && pTarget->CanPlaySequence( FCanOverrideState(), SS_INTERRUPT_IDLE ) )
					{
						m_hTargetEnt = pTarget;
						return true;
					}
				}
			}
		}
	}
	m_hTargetEnt = nullptr;
	return false;
}

// make the entity enter a scripted sequence
void CCineMonster::PossessEntity( void )
{
	CBaseEntity		*pEntity = m_hTargetEnt;
	CBaseMonster	*pTarget = NULL;
	if( pEntity )
		pTarget = pEntity->MyMonsterPointer();

	if( pTarget )
	{

		// FindEntity() just checked this!
#if 0
		if( !pTarget->CanPlaySequence( FCanOverrideState() ) )
		{
			ALERT( at_aiconsole, "Can't possess entity %s\n", pTarget->GetClassname() );
			return;
		}
#endif

		pTarget->m_hGoalEnt = this;
		pTarget->m_pCine = this;
		pTarget->m_hTargetEnt = this;

		m_saved_movetype = pTarget->pev->movetype;
		m_saved_solid = pTarget->pev->solid;
		m_saved_effects = pTarget->pev->effects;
		pTarget->pev->effects |= pev->effects;

		switch( m_fMoveTo )
		{
		case 0:
			pTarget->m_scriptState = SCRIPT_WAIT;
			break;

		case 1:
			pTarget->m_scriptState = SCRIPT_WALK_TO_MARK;
			DelayStart( true );
			break;

		case 2:
			pTarget->m_scriptState = SCRIPT_RUN_TO_MARK;
			DelayStart( true );
			break;

		case 4:
			pTarget->SetAbsOrigin( GetAbsOrigin() );
			pTarget->pev->ideal_yaw = pev->angles.y;
			pTarget->pev->avelocity = Vector( 0, 0, 0 );
			pTarget->pev->velocity = Vector( 0, 0, 0 );
			pTarget->pev->effects |= EF_NOINTERP;
			pTarget->pev->angles.y = pev->angles.y;
			pTarget->m_scriptState = SCRIPT_WAIT;
			m_startTime = gpGlobals->time + 1E6;
			// UNDONE: Add a flag to do this so people can fixup physics after teleporting monsters
			//			pTarget->pev->flags &= ~FL_ONGROUND;
			break;
		}
		//		ALERT( at_aiconsole, "\"%s\" found and used (INT: %s)\n", pTarget->GetTargetname(), FBitSet(pev->spawnflags, SF_SCRIPT_NOINTERRUPT)?"No":"Yes" );

		pTarget->m_IdealMonsterState = MONSTERSTATE_SCRIPT;
		if( m_iszIdle )
		{
			StartSequence( pTarget, m_iszIdle, false );
			if( FStrEq( STRING( m_iszIdle ), STRING( m_iszPlay ) ) )
			{
				pTarget->pev->framerate = 0;
			}
		}
	}
}

// find all the cinematic entities with my targetname and stop them from playing
void CCineMonster::CancelScript( void )
{
	ALERT( at_aiconsole, "Cancelling script: %s\n", STRING( m_iszPlay ) );

	if( !HasTargetname() )
	{
		ScriptEntityCancel( this );
		return;
	}

	CBaseEntity* pCineTarget = nullptr;

	while( (pCineTarget = UTIL_FindEntityByTargetname( pCineTarget, GetTargetname() )) )
	{
		ScriptEntityCancel( pCineTarget );
	}
}

// lookup a sequence name and setup the target monster to play it
bool CCineMonster::StartSequence( CBaseMonster *pTarget, int iszSeq, const bool completeOnEmpty )
{
	if( !iszSeq && completeOnEmpty )
	{
		SequenceDone( pTarget );
		return false;
	}

	pTarget->pev->sequence = pTarget->LookupSequence( STRING( iszSeq ) );
	if( pTarget->pev->sequence == -1 )
	{
		ALERT( at_error, "%s: unknown scripted sequence \"%s\"\n", pTarget->GetTargetname(), STRING( iszSeq ) );
		pTarget->pev->sequence = 0;
		// return false;
	}

#if 0
	char *s;
	if( pev->spawnflags & SF_SCRIPT_NOINTERRUPT )
		s = "No";
	else
		s = "Yes";

	ALERT( at_console, "%s (%s): started \"%s\":INT:%s\n", pTarget->GetTargetname(), pTarget->GetClassname(), STRING( iszSeq ), s );
#endif

	pTarget->pev->frame = 0;
	pTarget->ResetSequenceInfo();
	return true;
}

//=========================================================
// FCanOverrideState - returns false, scripted sequences 
// cannot possess entities regardless of state.
//=========================================================
bool CCineMonster::FCanOverrideState() const
{
	if( pev->spawnflags & SF_SCRIPT_OVERRIDESTATE )
		return true;
	return false;
}

//=========================================================
// SequenceDone - called when a scripted sequence animation
// sequence is done playing ( or when an AI Scripted Sequence
// doesn't supply an animation sequence to play ). Expects
// the CBaseMonster pointer to the monster that the sequence
// possesses. 
//=========================================================
void CCineMonster::SequenceDone( CBaseMonster *pMonster )
{
	//ALERT( at_aiconsole, "Sequence %s finished\n", STRING( m_pCine->m_iszPlay ) );

	if( !( pev->spawnflags & SF_SCRIPT_REPEATABLE ) )
	{
		SetThink( &CCineMonster::SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	// This is done so that another sequence can take over the monster when triggered by the first

	pMonster->CineCleanup();

	FixScriptMonsterSchedule( pMonster );

	// This may cause a sequence to attempt to grab this guy NOW, so we have to clear him out
	// of the existing sequence
	SUB_UseTargets( NULL, USE_TOGGLE, 0 );
}

//=========================================================
// When a monster finishes a scripted sequence, we have to 
// fix up its state and schedule for it to return to a 
// normal AI monster. 
//
// Scripted sequences just dirty the Schedule and drop the
// monster in Idle State.
//=========================================================
void CCineMonster::FixScriptMonsterSchedule( CBaseMonster *pMonster )
{
	if( pMonster->m_IdealMonsterState != MONSTERSTATE_DEAD )
		pMonster->m_IdealMonsterState = MONSTERSTATE_IDLE;
	pMonster->ClearSchedule();
}

bool CCineMonster::CanInterrupt() const
{
	if( !m_interruptable )
		return false;

	const CBaseEntity *pTarget = m_hTargetEnt;

	if( pTarget != NULL && pTarget->pev->deadflag == DEAD_NO )
		return true;

	return false;
}

void CCineMonster::AllowInterrupt( const bool fAllow )
{
	if( pev->spawnflags & SF_SCRIPT_NOINTERRUPT )
		return;
	m_interruptable = fAllow;
}

int	CCineMonster::IgnoreConditions( void )
{
	if( CanInterrupt() )
		return 0;
	return SCRIPT_BREAK_CONDITIONS;
}