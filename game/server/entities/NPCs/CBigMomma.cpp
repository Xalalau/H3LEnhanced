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
//=========================================================
// monster template
//=========================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"
#include "Decals.h"
#include "Weapons.h"
#include "Server.h"

#include "CInfoBM.h"
#include "CBMortar.h"

#include "CBigMomma.h"

int gSpitSprite, gSpitDebrisSprite;
Vector VecCheckSplatToss( CBaseEntity* pEntity, const Vector &vecSpot1, Vector vecSpot2, float maxHeight );
void SpriteSpray( const Vector &position, const Vector &direction, int spriteModel, int count );

// UNDONE:	
//
#define BIG_CHILDCLASS		"monster_babycrab"

const char *CBigMomma::pChildDieSounds[] =
{
	"gonarch/gon_childdie1.wav",
	"gonarch/gon_childdie2.wav",
	"gonarch/gon_childdie3.wav",
};

const char *CBigMomma::pSackSounds[] =
{
	"gonarch/gon_sack1.wav",
	"gonarch/gon_sack2.wav",
	"gonarch/gon_sack3.wav",
};

const char *CBigMomma::pDeathSounds[] =
{
	"gonarch/gon_die1.wav",
};

const char *CBigMomma::pAttackSounds[] =
{
	"gonarch/gon_attack1.wav",
	"gonarch/gon_attack2.wav",
	"gonarch/gon_attack3.wav",
};
const char *CBigMomma::pAttackHitSounds[] =
{
	"zombie/claw_strike1.wav",
	"zombie/claw_strike2.wav",
	"zombie/claw_strike3.wav",
};

const char *CBigMomma::pBirthSounds[] =
{
	"gonarch/gon_birth1.wav",
	"gonarch/gon_birth2.wav",
	"gonarch/gon_birth3.wav",
};

const char *CBigMomma::pAlertSounds[] =
{
	"gonarch/gon_alert1.wav",
	"gonarch/gon_alert2.wav",
	"gonarch/gon_alert3.wav",
};

const char *CBigMomma::pPainSounds[] =
{
	"gonarch/gon_pain2.wav",
	"gonarch/gon_pain4.wav",
	"gonarch/gon_pain5.wav",
};

const char *CBigMomma::pFootSounds[] =
{
	"gonarch/gon_step1.wav",
	"gonarch/gon_step2.wav",
	"gonarch/gon_step3.wav",
};

BEGIN_DATADESC(	CBigMomma )
	DEFINE_FIELD( m_nodeTime, FIELD_TIME ),
	DEFINE_FIELD( m_crabTime, FIELD_TIME ),
	DEFINE_FIELD( m_mortarTime, FIELD_TIME ),
	DEFINE_FIELD( m_painSoundTime, FIELD_TIME ),
	DEFINE_FIELD( m_crabCount, FIELD_INTEGER ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_bigmomma, CBigMomma );

void CBigMomma :: KeyValue( KeyValueData *pkvd )
{
#if 0
	if (FStrEq(pkvd->szKeyName, "volume"))
	{
		m_volume = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
#endif
		CBaseMonster::KeyValue( pkvd );
}

EntityClassification_t CBigMomma::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::ALIEN_MONSTER );
}

void CBigMomma::UpdateYawSpeed()
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:
		ys = 100;
		break;
	default:
		ys = 90;
	}
	SetYawSpeed( ys );
}

void CBigMomma :: HandleAnimEvent( AnimEvent_t& event )
{
	switch( event.event )
	{
		case BIG_AE_MELEE_ATTACKBR:
		case BIG_AE_MELEE_ATTACKBL:
		case BIG_AE_MELEE_ATTACK1:
		{
			Vector forward, right;

			UTIL_MakeVectorsPrivate( GetAbsAngles(), &forward, &right, nullptr );

			Vector center = GetAbsOrigin() + forward * 128;
			Vector mins = center - Vector( 64, 64, 0 );
			Vector maxs = center + Vector( 64, 64, 64 );

			CBaseEntity *pList[8];
			int count = UTIL_EntitiesInBox( pList, 8, mins, maxs, FL_MONSTER|FL_CLIENT );
			CBaseEntity *pHurt = NULL;

			for ( int i = 0; i < count && !pHurt; i++ )
			{
				if ( pList[i] != this )
				{
					if ( pList[i]->GetOwner() != this )
						pHurt = pList[i];
				}
			}
					
			if ( pHurt )
			{
				pHurt->TakeDamage( this, this, gSkillData.GetBigMommaDmgSlash(), DMG_CRUSH | DMG_SLASH );
				Vector vecPunchAngle = pHurt->GetPunchAngle();
				vecPunchAngle.x = 15;
				pHurt->SetPunchAngle( vecPunchAngle );
				switch( event.event )
				{
					case BIG_AE_MELEE_ATTACKBR:
						pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() + (forward * 150) + Vector(0,0,250) - (right * 200) );
					break;

					case BIG_AE_MELEE_ATTACKBL:
						pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() + (forward * 150) + Vector(0,0,250) + (right * 200) );
					break;

					case BIG_AE_MELEE_ATTACK1:
						pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() + (forward * 220) + Vector(0,0,200) );
					break;
				}

				pHurt->GetFlags().ClearFlags( FL_ONGROUND );
				EMIT_SOUND_DYN( this, CHAN_WEAPON, RANDOM_SOUND_ARRAY(pAttackHitSounds), 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5,5) );
			}
		}
		break;
		
		case BIG_AE_SCREAM:
			EMIT_SOUND_ARRAY_DYN( CHAN_VOICE, pAlertSounds );
			break;
		
		case BIG_AE_PAIN_SOUND:
			EMIT_SOUND_ARRAY_DYN( CHAN_VOICE, pPainSounds );
			break;
		
		case BIG_AE_ATTACK_SOUND:
			EMIT_SOUND_ARRAY_DYN( CHAN_WEAPON, pAttackSounds );
			break;

		case BIG_AE_BIRTH_SOUND:
			EMIT_SOUND_ARRAY_DYN( CHAN_BODY, pBirthSounds );
			break;

		case BIG_AE_SACK:
			if ( RANDOM_LONG(0,100) < 30 )
				EMIT_SOUND_ARRAY_DYN( CHAN_BODY, pSackSounds );
			break;

		case BIG_AE_DEATHSOUND:
			EMIT_SOUND_ARRAY_DYN( CHAN_VOICE, pDeathSounds );
			break;

		case BIG_AE_STEP1:		// Footstep left
		case BIG_AE_STEP3:		// Footstep back left
			EMIT_SOUND_ARRAY_DYN( CHAN_ITEM, pFootSounds );
			break;

		case BIG_AE_STEP4:		// Footstep back right
		case BIG_AE_STEP2:		// Footstep right
			EMIT_SOUND_ARRAY_DYN( CHAN_BODY, pFootSounds );
			break;

		case BIG_AE_MORTAR_ATTACK1:
			LaunchMortar();
			break;

		case BIG_AE_LAY_CRAB:
			LayHeadcrab();
			break;

		case BIG_AE_JUMP_FORWARD:
			GetFlags().ClearFlags( FL_ONGROUND );

			SetAbsOrigin( GetAbsOrigin() + Vector ( 0 , 0 , 1) );// take him off ground so engine doesn't instantly reset onground 
			UTIL_MakeVectors( GetAbsAngles() );

			SetAbsVelocity( (gpGlobals->v_forward * 200) + gpGlobals->v_up * 500 );
			break;

		case BIG_AE_EARLY_TARGET:
			{
				CBaseEntity *pTarget = m_hTargetEnt;
				if ( pTarget && pTarget->HasMessage() )
					FireTargets( pTarget->GetMessage(), this, this, USE_TOGGLE, 0 );
				Remember( bits_MEMORY_FIRED_NODE );
			}
			break;

		default:
			CBaseMonster::HandleAnimEvent( event );
			break;
	}
}

void CBigMomma::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	CTakeDamageInfo newInfo = info;

	if ( tr.iHitgroup != 1 )
	{
		// didn't hit the sack?
		
		if ( GetDamageTime() != gpGlobals->time || (RANDOM_LONG(0,10) < 1) )
		{
			UTIL_Ricochet( tr.vecEndPos, RANDOM_FLOAT( 1, 2) );
			SetDamageTime( gpGlobals->time );
		}

		newInfo.GetMutableDamage() = 0.1;// don't hurt the monster much, but allow bits_COND_LIGHT_DAMAGE to be generated
	}
	else if ( gpGlobals->time > m_painSoundTime )
	{
		m_painSoundTime = gpGlobals->time + RANDOM_LONG(1, 3);
		EMIT_SOUND_ARRAY_DYN( CHAN_VOICE, pPainSounds );
	}


	CBaseMonster::TraceAttack( newInfo, vecDir, tr );
}


void CBigMomma::OnTakeDamage( const CTakeDamageInfo& info )
{
	CTakeDamageInfo newInfo = info;

	// Don't take any acid damage -- BigMomma's mortar is acid
	if ( newInfo.GetDamageTypes() & DMG_ACID )
		newInfo.GetMutableDamage() = 0;

	if ( !HasMemory(bits_MEMORY_PATH_FINISHED) )
	{
		if ( GetHealth() <= newInfo.GetDamage() )
		{
			SetHealth( newInfo.GetDamage() + 1 );
			Remember( bits_MEMORY_ADVANCE_NODE | bits_MEMORY_COMPLETED_NODE );
			ALERT( at_aiconsole, "BM: Finished node health!!!\n" );
		}
	}

	CBaseMonster::OnTakeDamage( newInfo );
}

void CBigMomma :: LayHeadcrab( void )
{
	CBaseEntity *pChild = CBaseEntity::Create( BIG_CHILDCLASS, GetAbsOrigin(), GetAbsAngles(), edict() );

	pChild->GetSpawnFlags() |= SF_MONSTER_FALL_TO_GROUND;

	// Is this the second crab in a pair?
	if ( HasMemory( bits_MEMORY_CHILDPAIR ) )
	{
		m_crabTime = gpGlobals->time + RANDOM_FLOAT( 5, 10 );
		Forget( bits_MEMORY_CHILDPAIR );
	}
	else
	{
		m_crabTime = gpGlobals->time + RANDOM_FLOAT( 0.5, 2.5 );
		Remember( bits_MEMORY_CHILDPAIR );
	}

	TraceResult tr;
	UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() - Vector(0,0,100), ignore_monsters, edict(), &tr);
	UTIL_DecalTrace( &tr, DECAL_MOMMABIRTH );

	EMIT_SOUND_DYN( this, CHAN_WEAPON, RANDOM_SOUND_ARRAY(pBirthSounds), 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5,5) );
	m_crabCount++;
}



void CBigMomma::DeathNotice( CBaseEntity* pChild )
{
	if ( m_crabCount > 0 )		// Some babies may cross a transition, but we reset the count then
		m_crabCount--;
	if ( IsAlive() )
	{
		// Make the "my baby's dead" noise!
		EMIT_SOUND_ARRAY_DYN( CHAN_WEAPON, pChildDieSounds );
	}
}


void CBigMomma::LaunchMortar( void )
{
	m_mortarTime = gpGlobals->time + RANDOM_FLOAT( 2, 15 );
	
	Vector startPos = GetAbsOrigin();
	startPos.z += 180;

	EMIT_SOUND_DYN( this, CHAN_WEAPON, RANDOM_SOUND_ARRAY(pSackSounds), 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5,5) );
	CBMortar *pBomb = CBMortar::Shoot( this, startPos, GetMoveDir() );
	pBomb->SetGravity( 1.0 );
	SpriteSpray( startPos, Vector(0,0,1), gSpitSprite, 24 );
}

void CBigMomma :: Spawn()
{
	Precache( );

	SetModel( "models/big_mom.mdl");
	SetSize( Vector( -32, -32, 0 ), Vector( 32, 32, 64 ) );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= BLOOD_COLOR_GREEN;
	SetHealth( 150 * gSkillData.GetBigMommaHealthFactor() );
	SetViewOffset( Vector ( 0, 0, 128 ) );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= 0.3;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();
}

void CBigMomma :: Precache()
{
	PRECACHE_MODEL("models/big_mom.mdl");

	PRECACHE_SOUND_ARRAY( pChildDieSounds );
	PRECACHE_SOUND_ARRAY( pSackSounds );
	PRECACHE_SOUND_ARRAY( pDeathSounds );
	PRECACHE_SOUND_ARRAY( pAttackSounds );
	PRECACHE_SOUND_ARRAY( pAttackHitSounds );
	PRECACHE_SOUND_ARRAY( pBirthSounds );
	PRECACHE_SOUND_ARRAY( pAlertSounds );
	PRECACHE_SOUND_ARRAY( pPainSounds );
	PRECACHE_SOUND_ARRAY( pFootSounds );

	UTIL_PrecacheOther( BIG_CHILDCLASS );

	// TEMP: Squid
	PRECACHE_MODEL("sprites/mommaspit.spr");// spit projectile.
	gSpitSprite = PRECACHE_MODEL("sprites/mommaspout.spr");// client side spittle.
	gSpitDebrisSprite = PRECACHE_MODEL("sprites/mommablob.spr" );

	PRECACHE_SOUND( "bullchicken/bc_acid1.wav" );
	PRECACHE_SOUND( "bullchicken/bc_spithit1.wav" );
	PRECACHE_SOUND( "bullchicken/bc_spithit2.wav" );
}	


void CBigMomma::Activate( void )
{
	if ( m_hTargetEnt == NULL )
		Remember( bits_MEMORY_ADVANCE_NODE );	// Start 'er up
}


void CBigMomma::NodeStart( string_t iszNextNode )
{
	SetNetName( iszNextNode );

	CBaseEntity* pTarget = nullptr;

	if ( HasNetName() )
	{
		pTarget = UTIL_FindEntityByTargetname( nullptr, GetNetName() );
	}


	if ( !pTarget )
	{
		ALERT( at_aiconsole, "BM: Finished the path!!\n" );
		Remember( bits_MEMORY_PATH_FINISHED );
		return;
	}
	Remember( bits_MEMORY_ON_PATH );
	m_hTargetEnt = pTarget;
}


void CBigMomma::NodeReach( void )
{
	CBaseEntity *pTarget = m_hTargetEnt;

	Forget( bits_MEMORY_ADVANCE_NODE );

	if ( !pTarget )
		return;

	if ( pTarget->GetHealth() )
	{
		SetHealth( pTarget->GetHealth() * gSkillData.GetBigMommaHealthFactor() );
		SetMaxHealth( GetHealth() );
	}

	if ( !HasMemory( bits_MEMORY_FIRED_NODE ) )
	{
		if ( pTarget->HasMessage() )
			FireTargets( pTarget->GetMessage(), this, this, USE_TOGGLE, 0 );
	}
	Forget( bits_MEMORY_FIRED_NODE );

	SetNetName( pTarget->GetTarget() );
	if ( pTarget->GetHealth() == 0 )
		Remember( bits_MEMORY_ADVANCE_NODE );	// Move on if no health at this node
}


	// Slash
bool CBigMomma::CheckMeleeAttack1( float flDot, float flDist )
{
	if (flDot >= 0.7)
	{
		if ( flDist <= BIG_ATTACKDIST )
			return true;
	}
	return false;
}


// Lay a crab
bool CBigMomma::CheckMeleeAttack2( float flDot, float flDist )
{
	return CanLayCrab();
}


// Mortar launch
bool CBigMomma::CheckRangeAttack1( float flDot, float flDist )
{
	if ( flDist <= BIG_MORTARDIST && m_mortarTime < gpGlobals->time )
	{
		CBaseEntity *pEnemy = m_hEnemy;

		if ( pEnemy )
		{
			Vector startPos = GetAbsOrigin();
			startPos.z += 180;
			SetMoveDir( VecCheckSplatToss( this, startPos, pEnemy->BodyTarget( GetAbsOrigin() ), RANDOM_FLOAT( 150, 500 ) ) );
			if ( GetMoveDir() != g_vecZero )
				return true;
		}
	}
	return false;
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

enum
{
	SCHED_BIG_NODE = LAST_COMMON_SCHEDULE + 1,
	SCHED_NODE_FAIL,
};

enum
{
	TASK_MOVE_TO_NODE_RANGE = LAST_COMMON_TASK + 1,	// Move within node range
	TASK_FIND_NODE,									// Find my next node
	TASK_PLAY_NODE_PRESEQUENCE,						// Play node pre-script
	TASK_PLAY_NODE_SEQUENCE,						// Play node script
	TASK_PROCESS_NODE,								// Fire targets, etc.
	TASK_WAIT_NODE,									// Wait at the node
	TASK_NODE_DELAY,								// Delay walking toward node for a bit. You've failed to get there
	TASK_NODE_YAW,									// Get the best facing direction for this node
};


Task_t	tlBigNode[] =
{
	{ TASK_SET_FAIL_SCHEDULE,	(float)SCHED_NODE_FAIL },
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FIND_NODE,			(float)0		},	// Find my next node
	{ TASK_PLAY_NODE_PRESEQUENCE,(float)0		},	// Play the pre-approach sequence if any
	{ TASK_MOVE_TO_NODE_RANGE,	(float)0		},	// Move within node range
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_NODE_YAW,			(float)0		},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_WAIT_NODE,			(float)0		},	// Wait for node delay
	{ TASK_PLAY_NODE_SEQUENCE,	(float)0		},	// Play the sequence if one exists
	{ TASK_PROCESS_NODE,		(float)0		},	// Fire targets, etc.
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE	},
};

Schedule_t	slBigNode[] =
{
	{ 
		tlBigNode,
		ARRAYSIZE ( tlBigNode ), 
		0,
		0,
		"Big Node"
	},
};


Task_t	tlNodeFail[] =
{
	{ TASK_NODE_DELAY,			(float)10		},	// Try to do something else for 10 seconds
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE	},
};

Schedule_t	slNodeFail[] =
{
	{ 
		tlNodeFail,
		ARRAYSIZE ( tlNodeFail ), 
		0,
		0,
		"NodeFail"
	},
};

BEGIN_SCHEDULES( CBigMomma )
	slBigNode,
	slNodeFail,
END_SCHEDULES()

Schedule_t *CBigMomma::GetScheduleOfType( int Type )
{
	switch( Type )
	{
		case SCHED_BIG_NODE:
			return slBigNode;
		break;

		case SCHED_NODE_FAIL:
			return slNodeFail;
		break;
	}

	return CBaseMonster::GetScheduleOfType( Type );
}


bool CBigMomma::ShouldGoToNode() const
{
	if ( HasMemory( bits_MEMORY_ADVANCE_NODE ) )
	{
		if ( m_nodeTime < gpGlobals->time )
			return true;
	}
	return false;
}



Schedule_t *CBigMomma::GetSchedule( void )
{
	if ( ShouldGoToNode() )
	{
		return GetScheduleOfType( SCHED_BIG_NODE );
	}

	return CBaseMonster::GetSchedule();
}


void CBigMomma::StartTask( const Task_t& task )
{
	switch ( task.iTask )
	{
	case TASK_FIND_NODE:
		{
			CBaseEntity *pTarget = m_hTargetEnt;
			if ( !HasMemory( bits_MEMORY_ADVANCE_NODE ) )
			{
				if ( pTarget )
					SetNetName( m_hTargetEnt->GetTarget() );
			}
			NodeStart( MAKE_STRING( GetNetName() ) );
			TaskComplete();
			ALERT( at_aiconsole, "BM: Found node %s\n", GetNetName() );
		}
		break;

	case TASK_NODE_DELAY:
		m_nodeTime = gpGlobals->time + task.flData;
		TaskComplete();
		ALERT( at_aiconsole, "BM: FAIL! Delay %.2f\n", task.flData );
		break;

	case TASK_PROCESS_NODE:
		ALERT( at_aiconsole, "BM: Reached node %s\n", GetNetName() );
		NodeReach();
		TaskComplete();
		break;

	case TASK_PLAY_NODE_PRESEQUENCE:
	case TASK_PLAY_NODE_SEQUENCE:
		{
			string_t sequence;
			if ( task.iTask == TASK_PLAY_NODE_SEQUENCE )
				sequence = GetNodeSequence();
			else
				sequence = GetNodePresequence();

			ALERT( at_aiconsole, "BM: Playing node sequence %s\n", STRING(sequence) );
			if ( !FStringNull( sequence ) )
			{
				auto sequenceIndex = LookupSequence( STRING( sequence ) );
				if ( sequenceIndex != -1 )
				{
					SetSequence( sequenceIndex );
					SetFrame( 0 );
					ResetSequenceInfo( );
					//TODO: shouldn't this be sequence? - Solokiller
					ALERT( at_aiconsole, "BM: Sequence %s\n", STRING(GetNodeSequence()) );
					return;
				}
			}
			TaskComplete();
		}
		break;

	case TASK_NODE_YAW:
		SetIdealYaw( GetNodeYaw() );
		TaskComplete();
		break;

	case TASK_WAIT_NODE:
		m_flWait = gpGlobals->time + GetNodeDelay();
		if ( m_hTargetEnt->GetSpawnFlags().Any( SF_INFOBM_WAIT ) )
			ALERT( at_aiconsole, "BM: Wait at node %s forever\n", GetNetName() );
		else
			ALERT( at_aiconsole, "BM: Wait at node %s for %.2f\n", GetNetName(), GetNodeDelay() );
		break;


	case TASK_MOVE_TO_NODE_RANGE:
		{
			CBaseEntity *pTarget = m_hTargetEnt;
			if ( !pTarget )
				TaskFail();
			else
			{
				if ( (pTarget->GetAbsOrigin() - GetAbsOrigin()).Length() < GetNodeRange() )
					TaskComplete();
				else
				{
					Activity act = ACT_WALK;
					if ( pTarget->GetSpawnFlags().Any( SF_INFOBM_RUN ) )
						act = ACT_RUN;

					m_vecMoveGoal = pTarget->GetAbsOrigin();
					if ( !MoveToTarget( act, 2 ) )
					{
						TaskFail();
					}
				}
			}
		}
		ALERT( at_aiconsole, "BM: Moving to node %s\n", GetNetName() );

		break;

	case TASK_MELEE_ATTACK1:
		// Play an attack sound here
		EMIT_SOUND_DYN( this, CHAN_VOICE, RANDOM_SOUND_ARRAY(pAttackSounds), 1.0, ATTN_NORM, 0, PITCH_NORM );
		CBaseMonster::StartTask( task );
		break;

	default: 
		CBaseMonster::StartTask( task );
		break;
	}
}

void CBigMomma::RunTask( const Task_t& task )
{
	switch ( task.iTask )
	{
	case TASK_MOVE_TO_NODE_RANGE:
		{
			float distance;

			if ( m_hTargetEnt == NULL )
				TaskFail();
			else
			{
				distance = ( m_vecMoveGoal - GetAbsOrigin() ).Length2D();
				// Set the appropriate activity based on an overlapping range
				// overlap the range to prevent oscillation
				if ( (distance < GetNodeRange()) || MovementIsComplete() )
				{
					ALERT( at_aiconsole, "BM: Reached node!\n" );
					TaskComplete();
					RouteClear();		// Stop moving
				}
			}
		}

		break;

	case TASK_WAIT_NODE:
		if ( m_hTargetEnt != NULL && m_hTargetEnt->GetSpawnFlags().Any( SF_INFOBM_WAIT ) )
			return;

		if ( gpGlobals->time > m_flWaitFinished )
			TaskComplete();
		ALERT( at_aiconsole, "BM: The WAIT is over!\n" );
		break;

	case TASK_PLAY_NODE_PRESEQUENCE:
	case TASK_PLAY_NODE_SEQUENCE:
		if ( m_fSequenceFinished )
		{
			m_Activity = ACT_RESET;
			TaskComplete();
		}
		break;

	default:
		CBaseMonster::RunTask( task );
		break;
	}
}



Vector VecCheckSplatToss( CBaseEntity* pEntity, const Vector &vecSpot1, Vector vecSpot2, float maxHeight )
{
	TraceResult		tr;
	Vector			vecMidPoint;// halfway point between Spot1 and Spot2
	Vector			vecApex;// highest point 
	Vector			vecScale;
	Vector			vecGrenadeVel;
	Vector			vecTemp;
	float			flGravity = g_psv_gravity->value;

	// calculate the midpoint and apex of the 'triangle'
	vecMidPoint = vecSpot1 + (vecSpot2 - vecSpot1) * 0.5;
	UTIL_TraceLine(vecMidPoint, vecMidPoint + Vector(0,0,maxHeight), ignore_monsters, pEntity->edict(), &tr);
	vecApex = tr.vecEndPos;

	UTIL_TraceLine(vecSpot1, vecApex, dont_ignore_monsters, pEntity->edict(), &tr);
	if (tr.flFraction != 1.0)
	{
		// fail!
		return g_vecZero;
	}

	// Don't worry about actually hitting the target, this won't hurt us!

	// How high should the grenade travel (subtract 15 so the grenade doesn't hit the ceiling)?
	float height = (vecApex.z - vecSpot1.z) - 15;
	// How fast does the grenade need to travel to reach that height given gravity?
	float speed = sqrt( 2 * flGravity * height );
	
	// How much time does it take to get there?
	float time = speed / flGravity;
	vecGrenadeVel = (vecSpot2 - vecSpot1);
	vecGrenadeVel.z = 0;
	
	// Travel half the distance to the target in that time (apex is at the midpoint)
	vecGrenadeVel = vecGrenadeVel * ( 0.5 / time );
	// Speed to offset gravity at the desired height
	vecGrenadeVel.z = speed;

	return vecGrenadeVel;
}
