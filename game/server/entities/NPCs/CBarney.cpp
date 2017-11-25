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
// UNDONE: Holster weapon?

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "CTalkMonster.h"
#include "Schedule.h"
#include "DefaultAI.h"
#include "Weapons.h"
#include "entities/CSoundEnt.h"

#include "CBarney.h"

BEGIN_DATADESC(	CBarney )
	DEFINE_FIELD( m_fGunDrawn, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_painTime, FIELD_TIME ),
	DEFINE_FIELD( m_checkAttackTime, FIELD_TIME ),
	DEFINE_FIELD( m_lastAttackCheck, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flPlayerDamage, FIELD_FLOAT ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_barney, CBarney );

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Task_t	tlBaFollow[] =
{
	{ TASK_MOVE_TO_TARGET_RANGE,(float)128		},	// Move within 128 of target ent (client)
	{ TASK_SET_SCHEDULE,		(float)SCHED_TARGET_FACE },
};

Schedule_t	slBaFollow[] =
{
	{
		tlBaFollow,
		ARRAYSIZE ( tlBaFollow ),
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND |
		bits_COND_PROVOKED,
		bits_SOUND_DANGER,
		"Follow"
	},
};

//=========================================================
// BarneyDraw- much better looking draw schedule for when
// barney knows who he's gonna attack.
//=========================================================
Task_t	tlBarneyEnemyDraw[] =
{
	{ TASK_STOP_MOVING,					0				},
	{ TASK_FACE_ENEMY,					0				},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,	(float) ACT_ARM },
};

Schedule_t slBarneyEnemyDraw[] = 
{
	{
		tlBarneyEnemyDraw,
		ARRAYSIZE ( tlBarneyEnemyDraw ),
		0,
		0,
		"Barney Enemy Draw"
	}
};

Task_t	tlBaFaceTarget[] =
{
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_FACE_TARGET,			(float)0		},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_SET_SCHEDULE,		(float)SCHED_TARGET_CHASE },
};

Schedule_t	slBaFaceTarget[] =
{
	{
		tlBaFaceTarget,
		ARRAYSIZE ( tlBaFaceTarget ),
		bits_COND_CLIENT_PUSH	|
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND |
		bits_COND_PROVOKED,
		bits_SOUND_DANGER,
		"FaceTarget"
	},
};


Task_t	tlIdleBaStand[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT,				(float)2		}, // repick IDLESTAND every two seconds.
	{ TASK_TLK_HEADRESET,		(float)0		}, // reset head position
};

Schedule_t	slIdleBaStand[] =
{
	{ 
		tlIdleBaStand,
		ARRAYSIZE ( tlIdleBaStand ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND	|
		bits_COND_SMELL			|
		bits_COND_PROVOKED,

		bits_SOUND_COMBAT		|// sound flags - change these, and you'll break the talking code.
		//bits_SOUND_PLAYER		|
		//bits_SOUND_WORLD		|
		
		bits_SOUND_DANGER		|
		bits_SOUND_MEAT			|// scents
		bits_SOUND_CARCASS		|
		bits_SOUND_GARBAGE,
		"IdleStand"
	},
};

BEGIN_SCHEDULES( CBarney )
	slBaFollow,
	slBarneyEnemyDraw,
	slBaFaceTarget,
	slIdleBaStand,
END_SCHEDULES()

void CBarney :: StartTask( const Task_t& task )
{
	CTalkMonster::StartTask( task );	
}

void CBarney :: RunTask( const Task_t& task )
{
	switch ( task.iTask )
	{
	case TASK_RANGE_ATTACK1:
		if (m_hEnemy != NULL && (m_hEnemy->IsPlayer()))
		{
			SetFrameRate( 1.5 );
		}
		CTalkMonster::RunTask( task );
		break;
	default:
		CTalkMonster::RunTask( task );
		break;
	}
}

int CBarney :: ISoundMask ( void) 
{
	return	bits_SOUND_WORLD	|
			bits_SOUND_COMBAT	|
			bits_SOUND_CARCASS	|
			bits_SOUND_MEAT		|
			bits_SOUND_GARBAGE	|
			bits_SOUND_DANGER	|
			bits_SOUND_PLAYER;
}

EntityClassification_t CBarney::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::PLAYER_ALLY );
}

void CBarney :: AlertSound( void )
{
	if ( m_hEnemy != NULL )
	{
		if ( FOkToSpeak() )
		{
			PlaySentence( "BA_ATTACK", RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE );
		}
	}

}

void CBarney::UpdateYawSpeed()
{
	int ys;

	ys = 0;

	switch ( m_Activity )
	{
	case ACT_IDLE:		
		ys = 70;
		break;
	case ACT_WALK:
		ys = 70;
		break;
	case ACT_RUN:
		ys = 90;
		break;
	default:
		ys = 70;
		break;
	}

	SetYawSpeed( ys );
}

bool CBarney::CheckRangeAttack1( float flDot, float flDist )
{
	if ( flDist <= 1024 && flDot >= 0.5 )
	{
		if ( gpGlobals->time > m_checkAttackTime )
		{
			TraceResult tr;
			
			Vector shootOrigin = GetAbsOrigin() + Vector( 0, 0, 55 );
			CBaseEntity *pEnemy = m_hEnemy;
			Vector shootTarget = ( (pEnemy->BodyTarget( shootOrigin ) - pEnemy->GetAbsOrigin()) + m_vecEnemyLKP );
			UTIL_TraceLine( shootOrigin, shootTarget, dont_ignore_monsters, ENT(pev), &tr );
			m_checkAttackTime = gpGlobals->time + 1;
			if ( tr.flFraction == 1.0 || (tr.pHit != NULL && CBaseEntity::Instance(tr.pHit) == pEnemy) )
				m_lastAttackCheck = true;
			else
				m_lastAttackCheck = false;
			m_checkAttackTime = gpGlobals->time + 1.5;
		}
		return m_lastAttackCheck;
	}
	return false;
}


//=========================================================
// BarneyFirePistol - shoots one round from the pistol at
// the enemy barney is facing.
//=========================================================
void CBarney :: BarneyFirePistol ( void )
{
	Vector vecShootOrigin;

	UTIL_MakeVectors( GetAbsAngles() );
	vecShootOrigin = GetAbsOrigin() + Vector( 0, 0, 55 );
	Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

	Vector angDir = UTIL_VecToAngles( vecShootDir );
	SetBlending( 0, angDir.x );
	GetEffects() = EF_MUZZLEFLASH;

	FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_2DEGREES, 1024, BULLET_MONSTER_9MM );
	
	int pitchShift = RANDOM_LONG( 0, 20 );
	
	// Only shift about half the time
	if ( pitchShift > 10 )
		pitchShift = 0;
	else
		pitchShift -= 5;
	EMIT_SOUND_DYN( this, CHAN_WEAPON, "barney/ba_attack2.wav", 1, ATTN_NORM, 0, 100 + pitchShift );

	CSoundEnt::InsertSound ( bits_SOUND_COMBAT, GetAbsOrigin(), 384, 0.3 );

	// UNDONE: Reload?
	m_cAmmoLoaded--;// take away a bullet!
}

void CBarney :: HandleAnimEvent( AnimEvent_t& event )
{
	switch( event.event )
	{
	case BARNEY_AE_SHOOT:
		BarneyFirePistol();
		break;

	case BARNEY_AE_DRAW:
		// barney's bodygroup switches here so he can pull gun from holster
		SetBody( BARNEY_BODY_GUNDRAWN );
		m_fGunDrawn = true;
		break;

	case BARNEY_AE_HOLSTER:
		// change bodygroup to replace gun in holster
		SetBody( BARNEY_BODY_GUNHOLSTERED );
		m_fGunDrawn = false;
		break;

	default:
		CTalkMonster::HandleAnimEvent( event );
	}
}

void CBarney :: Spawn()
{
	Precache( );

	SetModel( "models/barney.mdl");
	SetSize( VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= BLOOD_COLOR_RED;
	SetHealth( gSkillData.GetBarneyHealth() );
	SetViewOffset( Vector ( 0, 0, 50 ) );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_MonsterState		= MONSTERSTATE_NONE;

	SetBody( 0 ); // gun in holster
	m_fGunDrawn			= false;

	m_afCapability		= bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;

	MonsterInit();
	SetUse( &CBarney::FollowerUse );
}

void CBarney :: Precache()
{
	PRECACHE_MODEL("models/barney.mdl");

	PRECACHE_SOUND("barney/ba_attack1.wav" );
	PRECACHE_SOUND("barney/ba_attack2.wav" );

	PRECACHE_SOUND("barney/ba_pain1.wav");
	PRECACHE_SOUND("barney/ba_pain2.wav");
	PRECACHE_SOUND("barney/ba_pain3.wav");

	PRECACHE_SOUND("barney/ba_die1.wav");
	PRECACHE_SOUND("barney/ba_die2.wav");
	PRECACHE_SOUND("barney/ba_die3.wav");
	
	// every new barney must call this, otherwise
	// when a level is loaded, nobody will talk (time is reset to 0)
	TalkInit();
	CTalkMonster::Precache();
}	

// Init talk data
void CBarney :: TalkInit()
{
	
	CTalkMonster::TalkInit();

	// scientists speach group names (group names are in sentences.txt)

	m_szGrp[TLK_ANSWER]  =	"BA_ANSWER";
	m_szGrp[TLK_QUESTION] =	"BA_QUESTION";
	m_szGrp[TLK_IDLE] =		"BA_IDLE";
	m_szGrp[TLK_STARE] =		"BA_STARE";
	m_szGrp[TLK_USE] =		"BA_OK";
	m_szGrp[TLK_UNUSE] =	"BA_WAIT";
	m_szGrp[TLK_STOP] =		"BA_STOP";

	m_szGrp[TLK_NOSHOOT] =	"BA_SCARED";
	m_szGrp[TLK_HELLO] =	"BA_HELLO";

	m_szGrp[TLK_PLHURT1] =	"!BA_CUREA";
	m_szGrp[TLK_PLHURT2] =	"!BA_CUREB"; 
	m_szGrp[TLK_PLHURT3] =	"!BA_CUREC";

	m_szGrp[TLK_PHELLO] =	NULL;	//"BA_PHELLO";		// UNDONE
	m_szGrp[TLK_PIDLE] =	NULL;	//"BA_PIDLE";			// UNDONE
	m_szGrp[TLK_PQUESTION] = "BA_PQUEST";		// UNDONE

	m_szGrp[TLK_SMELL] =	"BA_SMELL";
	
	m_szGrp[TLK_WOUND] =	"BA_WOUND";
	m_szGrp[TLK_MORTAL] =	"BA_MORTAL";

	// get voice for head - just one barney voice for now
	m_voicePitch = 100;
}


static bool IsFacing( CBaseEntity* pEntity, const Vector &reference )
{
	Vector vecDir = (reference - pEntity->GetAbsOrigin());
	vecDir.z = 0;
	vecDir = vecDir.Normalize();
	Vector forward, angle;
	angle = pEntity->GetViewAngle();
	angle.x = 0;
	UTIL_MakeVectorsPrivate( angle, &forward, nullptr, nullptr );
	// He's facing me, he meant it
	if ( DotProduct( forward, vecDir ) > 0.96 )	// +/- 15 degrees or so
	{
		return true;
	}
	return false;
}


void CBarney::OnTakeDamage( const CTakeDamageInfo& info )
{
	// make sure friends talk about it if player hurts talkmonsters...
	CTalkMonster::OnTakeDamage( info );
	if ( !IsAlive() || GetDeadFlag() == DEAD_DYING )
		return;

	if ( m_MonsterState != MONSTERSTATE_PRONE && (info.GetAttacker()->GetFlags().Any( FL_CLIENT ) ) )
	{
		m_flPlayerDamage += info.GetDamage();

		// This is a heurstic to determine if the player intended to harm me
		// If I have an enemy, we can't establish intent (may just be crossfire)
		if ( m_hEnemy == NULL )
		{
			// If the player was facing directly at me, or I'm already suspicious, get mad
			if ( (m_afMemory & bits_MEMORY_SUSPICIOUS) || IsFacing( info.GetAttacker(), GetAbsOrigin() ) )
			{
				// Alright, now I'm pissed!
				PlaySentence( "BA_MAD", 4, VOL_NORM, ATTN_NORM );

				Remember( bits_MEMORY_PROVOKED );
				StopFollowing( true );
			}
			else
			{
				// Hey, be careful with that
				PlaySentence( "BA_SHOT", 4, VOL_NORM, ATTN_NORM );
				Remember( bits_MEMORY_SUSPICIOUS );
			}
		}
		else if ( !(m_hEnemy->IsPlayer()) && GetDeadFlag() == DEAD_NO )
		{
			PlaySentence( "BA_SHOT", 4, VOL_NORM, ATTN_NORM );
		}
	}
}

void CBarney :: PainSound ( void )
{
	if (gpGlobals->time < m_painTime)
		return;
	
	m_painTime = gpGlobals->time + RANDOM_FLOAT(0.5, 0.75);

	switch (RANDOM_LONG(0,2))
	{
	case 0: EMIT_SOUND_DYN( this, CHAN_VOICE, "barney/ba_pain1.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 1: EMIT_SOUND_DYN( this, CHAN_VOICE, "barney/ba_pain2.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 2: EMIT_SOUND_DYN( this, CHAN_VOICE, "barney/ba_pain3.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	}
}

void CBarney :: DeathSound ( void )
{
	switch (RANDOM_LONG(0,2))
	{
	case 0: EMIT_SOUND_DYN( this, CHAN_VOICE, "barney/ba_die1.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 1: EMIT_SOUND_DYN( this, CHAN_VOICE, "barney/ba_die2.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 2: EMIT_SOUND_DYN( this, CHAN_VOICE, "barney/ba_die3.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	}
}


void CBarney::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	CTakeDamageInfo newInfo = info;

	switch( tr.iHitgroup)
	{
	case HITGROUP_CHEST:
	case HITGROUP_STOMACH:
		if (newInfo.GetDamageTypes() & (DMG_BULLET | DMG_SLASH | DMG_BLAST))
		{
			newInfo.GetMutableDamage() = newInfo.GetDamage() / 2;
		}
		break;
	case 10:
		if (newInfo.GetDamageTypes() & (DMG_BULLET | DMG_SLASH | DMG_CLUB))
		{
			newInfo.GetMutableDamage() -= 20;
			if (newInfo.GetDamage() <= 0)
			{
				UTIL_Ricochet( tr.vecEndPos, 1.0 );
				newInfo.GetMutableDamage() = 0.01;
			}
		}
		// always a head shot
		tr.iHitgroup = HITGROUP_HEAD;
		break;
	}

	CTalkMonster::TraceAttack( newInfo, vecDir, tr );
}


void CBarney::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	if ( GetBody() < BARNEY_BODY_GUNGONE )
	{// drop the gun!
		Vector vecGunPos;
		Vector vecGunAngles;

		SetBody( BARNEY_BODY_GUNGONE );

		GetAttachment( 0, vecGunPos, vecGunAngles );
		
		/*CBaseEntity *pGun = */DropItem( "weapon_9mmhandgun", vecGunPos, vecGunAngles );
	}

	SetUse( NULL );	
	CTalkMonster::Killed( info, gibAction );
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

Schedule_t* CBarney :: GetScheduleOfType ( int Type )
{
	Schedule_t *psched;

	switch( Type )
	{
	case SCHED_ARM_WEAPON:
		if ( m_hEnemy != NULL )
		{
			// face enemy, then draw.
			return slBarneyEnemyDraw;
		}
		break;

	// Hook these to make a looping schedule
	case SCHED_TARGET_FACE:
		// call base class default so that barney will talk
		// when 'used' 
		psched = CTalkMonster::GetScheduleOfType(Type);

		if (psched == slIdleStand)
			return slBaFaceTarget;	// override this for different target face behavior
		else
			return psched;

	case SCHED_TARGET_CHASE:
		return slBaFollow;

	case SCHED_IDLE_STAND:
		// call base class default so that scientist will talk
		// when standing during idle
		psched = CTalkMonster::GetScheduleOfType(Type);

		if (psched == slIdleStand)
		{
			// just look straight ahead.
			return slIdleBaStand;
		}
		else
			return psched;	
	}

	return CTalkMonster::GetScheduleOfType( Type );
}

Schedule_t *CBarney :: GetSchedule ( void )
{
	if ( HasConditions( bits_COND_HEAR_SOUND ) )
	{
		CSound *pSound;
		pSound = PBestSound();

		ASSERT( pSound != NULL );
		if ( pSound && (pSound->m_iType & bits_SOUND_DANGER) )
			return GetScheduleOfType( SCHED_TAKE_COVER_FROM_BEST_SOUND );
	}
	if ( HasConditions( bits_COND_ENEMY_DEAD ) && FOkToSpeak() )
	{
		PlaySentence( "BA_KILL", 4, VOL_NORM, ATTN_NORM );
	}

	switch( m_MonsterState )
	{
	case MONSTERSTATE_COMBAT:
		{
// dead enemy
			if ( HasConditions( bits_COND_ENEMY_DEAD ) )
			{
				// call base class, all code to handle dead enemies is centralized there.
				return CBaseMonster :: GetSchedule();
			}

			// always act surprized with a new enemy
			if ( HasConditions( bits_COND_NEW_ENEMY ) && HasConditions( bits_COND_LIGHT_DAMAGE) )
				return GetScheduleOfType( SCHED_SMALL_FLINCH );
				
			// wait for one schedule to draw gun
			if (!m_fGunDrawn )
				return GetScheduleOfType( SCHED_ARM_WEAPON );

			if ( HasConditions( bits_COND_HEAVY_DAMAGE ) )
				return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
		}
		break;

	case MONSTERSTATE_ALERT:	
	case MONSTERSTATE_IDLE:
		if ( HasConditions(bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE))
		{
			// flinch if hurt
			return GetScheduleOfType( SCHED_SMALL_FLINCH );
		}

		if ( m_hEnemy == NULL && IsFollowing() )
		{
			if ( !m_hTargetEnt->IsAlive() )
			{
				// UNDONE: Comment about the recently dead player here?
				StopFollowing( false );
				break;
			}
			else
			{
				if ( HasConditions( bits_COND_CLIENT_PUSH ) )
				{
					return GetScheduleOfType( SCHED_MOVE_AWAY_FOLLOW );
				}
				return GetScheduleOfType( SCHED_TARGET_FACE );
			}
		}

		if ( HasConditions( bits_COND_CLIENT_PUSH ) )
		{
			return GetScheduleOfType( SCHED_MOVE_AWAY );
		}

		// try to say something about smells
		TrySmellTalk();
		break;

	default: break;
	}
	
	return CTalkMonster::GetSchedule();
}

MONSTERSTATE CBarney :: GetIdealState ( void )
{
	return CTalkMonster::GetIdealState();
}



void CBarney::DeclineFollowing( void )
{
	PlaySentence( "BA_POK", 2, VOL_NORM, ATTN_NORM );
}