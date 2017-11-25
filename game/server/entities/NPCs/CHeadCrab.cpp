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
// headcrab.cpp - tiny, jumpy alien parasite
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"
#include "Server.h"

#include "CHeadCrab.h"

Task_t	tlHCRangeAttack1[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE	},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_WAIT_RANDOM,			(float)0.5		},
};

Schedule_t	slHCRangeAttack1[] =
{
	{ 
		tlHCRangeAttack1,
		ARRAYSIZE ( tlHCRangeAttack1 ), 
		bits_COND_ENEMY_OCCLUDED	|
		bits_COND_NO_AMMO_LOADED,
		0,
		"HCRangeAttack1"
	},
};

Task_t	tlHCRangeAttack1Fast[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE	},
};

Schedule_t	slHCRangeAttack1Fast[] =
{
	{ 
		tlHCRangeAttack1Fast,
		ARRAYSIZE ( tlHCRangeAttack1Fast ), 
		bits_COND_ENEMY_OCCLUDED	|
		bits_COND_NO_AMMO_LOADED,
		0,
		"HCRAFast"
	},
};

BEGIN_DATADESC( CHeadCrab )
	DEFINE_TOUCHFUNC( LeapTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_headcrab, CHeadCrab );

BEGIN_SCHEDULES( CHeadCrab )
	slHCRangeAttack1,
	slHCRangeAttack1Fast,
END_SCHEDULES()

const char *CHeadCrab::pIdleSounds[] = 
{
	"headcrab/hc_idle1.wav",
	"headcrab/hc_idle2.wav",
	"headcrab/hc_idle3.wav",
};
const char *CHeadCrab::pAlertSounds[] = 
{
	"headcrab/hc_alert1.wav",
};
const char *CHeadCrab::pPainSounds[] = 
{
	"headcrab/hc_pain1.wav",
	"headcrab/hc_pain2.wav",
	"headcrab/hc_pain3.wav",
};
const char *CHeadCrab::pAttackSounds[] = 
{
	"headcrab/hc_attack1.wav",
	"headcrab/hc_attack2.wav",
	"headcrab/hc_attack3.wav",
};

const char *CHeadCrab::pDeathSounds[] = 
{
	"headcrab/hc_die1.wav",
	"headcrab/hc_die2.wav",
};

const char *CHeadCrab::pBiteSounds[] = 
{
	"headcrab/hc_headbite.wav",
};

EntityClassification_t CHeadCrab::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::ALIEN_PREY );
}

//=========================================================
// Center - returns the real center of the headcrab.  The 
// bounding box is much larger than the actual creature so 
// this is needed for targeting
//=========================================================
Vector CHeadCrab::Center() const
{
	return Vector( GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z + 6 );
}


Vector CHeadCrab::BodyTarget( const Vector &posSrc ) const
{ 
	return Center();
}

void CHeadCrab::UpdateYawSpeed()
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:			
		ys = 30;
		break;
	case ACT_RUN:			
	case ACT_WALK:			
		ys = 20;
		break;
	case ACT_TURN_LEFT:
	case ACT_TURN_RIGHT:
		ys = 60;
		break;
	case ACT_RANGE_ATTACK1:	
		ys = 30;
		break;
	default:
		ys = 30;
		break;
	}

	SetYawSpeed( ys );
}

void CHeadCrab :: HandleAnimEvent( AnimEvent_t& event )
{
	switch( event.event )
	{
		case HC_AE_JUMPATTACK:
		{
			GetFlags().ClearFlags( FL_ONGROUND );

			SetAbsOrigin( GetAbsOrigin() + Vector ( 0 , 0 , 1) );// take him off ground so engine doesn't instantly reset onground 
			UTIL_MakeVectors ( GetAbsAngles() );

			Vector vecJumpDir;
			if (m_hEnemy != NULL)
			{
				float gravity = g_psv_gravity->value;
				if (gravity <= 1)
					gravity = 1;

				// How fast does the headcrab need to travel to reach that height given gravity?
				float height = (m_hEnemy->GetAbsOrigin().z + m_hEnemy->GetViewOffset().z - GetAbsOrigin().z);
				if (height < 16)
					height = 16;
				float speed = sqrt( 2 * gravity * height );
				float time = speed / gravity;

				// Scale the sideways velocity to get there at the right time
				vecJumpDir = (m_hEnemy->GetAbsOrigin() + m_hEnemy->GetViewOffset() - GetAbsOrigin());
				vecJumpDir = vecJumpDir * ( 1.0 / time );

				// Speed to offset gravity at the desired height
				vecJumpDir.z = speed;

				// Don't jump too far/fast
				float distance = vecJumpDir.Length();
				
				if (distance > 650)
				{
					vecJumpDir = vecJumpDir * ( 650.0 / distance );
				}
			}
			else
			{
				// jump hop, don't care where
				vecJumpDir = Vector( gpGlobals->v_forward.x, gpGlobals->v_forward.y, gpGlobals->v_up.z ) * 350;
			}

			int iSound = RANDOM_LONG(0,2);
			if ( iSound != 0 )
				EMIT_SOUND_DYN( this, CHAN_VOICE, pAttackSounds[iSound], GetSoundVolume(), ATTN_IDLE, 0, GetVoicePitch() );

			SetAbsVelocity( vecJumpDir );
			m_flNextAttack = gpGlobals->time + 2;
		}
		break;

		default:
			CBaseMonster::HandleAnimEvent( event );
			break;
	}
}

void CHeadCrab :: Spawn()
{
	Precache( );

	SetModel( "models/headcrab.mdl");
	SetSize( Vector(-12, -12, 0), Vector(12, 12, 24) );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= BLOOD_COLOR_GREEN;
	GetEffects().ClearAll();
	SetHealth( gSkillData.GetHeadcrabHealth() );
	SetViewOffset( Vector ( 0, 0, 20 ) );// position of the eyes relative to monster's origin.
	SetYawSpeed( 5 );//!!! should we put this in the monster's changeanim function since turn rates may vary with state/anim?
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();
}

void CHeadCrab :: Precache()
{
	PRECACHE_SOUND_ARRAY(pIdleSounds);
	PRECACHE_SOUND_ARRAY(pAlertSounds);
	PRECACHE_SOUND_ARRAY(pPainSounds);
	PRECACHE_SOUND_ARRAY(pAttackSounds);
	PRECACHE_SOUND_ARRAY(pDeathSounds);
	PRECACHE_SOUND_ARRAY(pBiteSounds);

	PRECACHE_MODEL("models/headcrab.mdl");
}	

void CHeadCrab :: RunTask ( const Task_t& task )
{
	switch ( task.iTask )
	{
	case TASK_RANGE_ATTACK1:
	case TASK_RANGE_ATTACK2:
		{
			if ( m_fSequenceFinished )
			{
				TaskComplete();
				SetTouch( NULL );
				m_IdealActivity = ACT_IDLE;
			}
			break;
		}
	default:
		{
			CBaseMonster :: RunTask( task );
		}
	}
}

//=========================================================
// LeapTouch - this is the headcrab's touch function when it
// is in the air
//=========================================================
void CHeadCrab :: LeapTouch ( CBaseEntity *pOther )
{
	if ( pOther->GetTakeDamageMode() == DAMAGE_NO )
	{
		return;
	}

	if ( pOther->Classify() == Classify() )
	{
		return;
	}

	// Don't hit if back on ground
	if ( !GetFlags().Any( FL_ONGROUND ) )
	{
		EMIT_SOUND_DYN( this, CHAN_WEAPON, RANDOM_SOUND_ARRAY(pBiteSounds), GetSoundVolume(), ATTN_IDLE, 0, GetVoicePitch() );
		
		pOther->TakeDamage( this, this, GetDamageAmount(), DMG_SLASH );
	}

	SetTouch( NULL );
}

void CHeadCrab :: PrescheduleThink ( void )
{
	// make the crab coo a little bit in combat state
	if ( m_MonsterState == MONSTERSTATE_COMBAT && RANDOM_FLOAT( 0, 5 ) < 0.1 )
	{
		IdleSound();
	}
}

void CHeadCrab :: StartTask ( const Task_t& task )
{
	m_iTaskStatus = TASKSTATUS_RUNNING;

	switch ( task.iTask )
	{
	case TASK_RANGE_ATTACK1:
		{
			EMIT_SOUND_DYN( this, CHAN_WEAPON, pAttackSounds[0], GetSoundVolume(), ATTN_IDLE, 0, GetVoicePitch() );
			m_IdealActivity = ACT_RANGE_ATTACK1;
			SetTouch ( &CHeadCrab::LeapTouch );
			break;
		}
	default:
		{
			CBaseMonster :: StartTask( task );
		}
	}
}

bool CHeadCrab :: CheckRangeAttack1 ( float flDot, float flDist )
{
	if ( GetFlags().Any( FL_ONGROUND ) && flDist <= 256 && flDot >= 0.65 )
	{
		return true;
	}
	return false;
}

bool CHeadCrab :: CheckRangeAttack2 ( float flDot, float flDist )
{
	return false;
	// BUGBUG: Why is this code here?  There is no ACT_RANGE_ATTACK2 animation.  I've disabled it for now.
#if 0
	if ( GetFlags().Any( FL_ONGROUND ) && flDist > 64 && flDist <= 256 && flDot >= 0.5 )
	{
		return true;
	}
	return false;
#endif
}

void CHeadCrab::OnTakeDamage( const CTakeDamageInfo& info )
{
	CTakeDamageInfo newInfo = info;

	// Don't take any acid damage -- BigMomma's mortar is acid
	if ( newInfo.GetDamageTypes() & DMG_ACID )
		newInfo.GetMutableDamage() = 0;

	CBaseMonster::OnTakeDamage( newInfo );
}

//=========================================================
// IdleSound
//=========================================================
#define CRAB_ATTN_IDLE (float)1.5
void CHeadCrab :: IdleSound ( void )
{
	EMIT_SOUND_DYN( this, CHAN_VOICE, RANDOM_SOUND_ARRAY(pIdleSounds), GetSoundVolume(), ATTN_IDLE, 0, GetVoicePitch() );
}

void CHeadCrab :: AlertSound ( void )
{
	EMIT_SOUND_DYN( this, CHAN_VOICE, RANDOM_SOUND_ARRAY(pAlertSounds), GetSoundVolume(), ATTN_IDLE, 0, GetVoicePitch() );
}

void CHeadCrab :: PainSound ( void )
{
	EMIT_SOUND_DYN( this, CHAN_VOICE, RANDOM_SOUND_ARRAY(pPainSounds), GetSoundVolume(), ATTN_IDLE, 0, GetVoicePitch() );
}

void CHeadCrab :: DeathSound ( void )
{
	EMIT_SOUND_DYN( this, CHAN_VOICE, RANDOM_SOUND_ARRAY(pDeathSounds), GetSoundVolume(), ATTN_IDLE, 0, GetVoicePitch() );
}

Schedule_t* CHeadCrab :: GetScheduleOfType ( int Type )
{
	switch	( Type )
	{
		case SCHED_RANGE_ATTACK1:
		{
			return &slHCRangeAttack1[ 0 ];
		}
		break;
	}

	return CBaseMonster::GetScheduleOfType( Type );
}
