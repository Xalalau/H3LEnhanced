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
// CONTROLLER
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Effects.h"
#include "Schedule.h"
#include "Weapons.h"
#include "CSquadMonster.h"

#include "CController.h"

BEGIN_DATADESC(	CController )
	DEFINE_ARRAY( m_pBall, FIELD_CLASSPTR, 2 ),
	DEFINE_ARRAY( m_iBall, FIELD_INTEGER, 2 ),
	DEFINE_ARRAY( m_iBallTime, FIELD_TIME, 2 ),
	DEFINE_ARRAY( m_iBallCurrent, FIELD_INTEGER, 2 ),
	DEFINE_FIELD( m_vecEstVelocity, FIELD_VECTOR ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_alien_controller, CController );

const char *CController::pAttackSounds[] = 
{
	"controller/con_attack1.wav",
	"controller/con_attack2.wav",
	"controller/con_attack3.wav",
};

const char *CController::pIdleSounds[] = 
{
	"controller/con_idle1.wav",
	"controller/con_idle2.wav",
	"controller/con_idle3.wav",
	"controller/con_idle4.wav",
	"controller/con_idle5.wav",
};

const char *CController::pAlertSounds[] = 
{
	"controller/con_alert1.wav",
	"controller/con_alert2.wav",
	"controller/con_alert3.wav",
};

const char *CController::pPainSounds[] = 
{
	"controller/con_pain1.wav",
	"controller/con_pain2.wav",
	"controller/con_pain3.wav",
};

const char *CController::pDeathSounds[] = 
{
	"controller/con_die1.wav",
	"controller/con_die2.wav",
};

EntityClassification_t CController::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::ALIEN_MILITARY );
}

void CController::UpdateYawSpeed()
{
	int ys;

	ys = 120;

#if 0
	switch ( m_Activity )
	{
	}
#endif

	SetYawSpeed( ys );
}

void CController::OnTakeDamage( const CTakeDamageInfo& info )
{
	// HACK HACK -- until we fix this.
	if ( IsAlive() )
		PainSound();
	CBaseMonster::OnTakeDamage( info );
}


void CController::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	// shut off balls
	/*
	m_iBall[0] = 0;
	m_iBallTime[0] = gpGlobals->time + 4.0;
	m_iBall[1] = 0;
	m_iBallTime[1] = gpGlobals->time + 4.0;
	*/

	// fade balls
	if (m_pBall[0])
	{
		m_pBall[0]->SUB_StartFadeOut();
		m_pBall[0] = NULL;
	}
	if (m_pBall[1])
	{
		m_pBall[1]->SUB_StartFadeOut();
		m_pBall[1] = NULL;
	}

	CSquadMonster::Killed( info, gibAction );
}


void CController::GibMonster( void )
{
	// delete balls
	if (m_pBall[0])
	{
		UTIL_Remove( m_pBall[0] );
		m_pBall[0] = NULL;
	}
	if (m_pBall[1])
	{
		UTIL_Remove( m_pBall[1] );
		m_pBall[1] = NULL;
	}
	CSquadMonster::GibMonster( );
}




void CController :: PainSound( void )
{
	if (RANDOM_LONG(0,5) < 2)
		EMIT_SOUND_ARRAY_DYN( CHAN_VOICE, pPainSounds ); 
}	

void CController :: AlertSound( void )
{
	EMIT_SOUND_ARRAY_DYN( CHAN_VOICE, pAlertSounds ); 
}

void CController :: IdleSound( void )
{
	EMIT_SOUND_ARRAY_DYN( CHAN_VOICE, pIdleSounds ); 
}

void CController :: AttackSound( void )
{
	EMIT_SOUND_ARRAY_DYN( CHAN_VOICE, pAttackSounds ); 
}

void CController :: DeathSound( void )
{
	EMIT_SOUND_ARRAY_DYN( CHAN_VOICE, pDeathSounds ); 
}

void CController :: HandleAnimEvent( AnimEvent_t& event )
{
	switch( event.event )
	{
		case CONTROLLER_AE_HEAD_OPEN:
		{
			Vector vecStart, angleGun;
			
			GetAttachment( 0, vecStart, angleGun );
			
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_ELIGHT );
				WRITE_SHORT( entindex( ) + 0x1000 );		// entity, attachment
				WRITE_COORD( vecStart.x );		// origin
				WRITE_COORD( vecStart.y );
				WRITE_COORD( vecStart.z );
				WRITE_COORD( 1 );	// radius
				WRITE_BYTE( 255 );	// R
				WRITE_BYTE( 192 );	// G
				WRITE_BYTE( 64 );	// B
				WRITE_BYTE( 20 );	// life * 10
				WRITE_COORD( -32 ); // decay
			MESSAGE_END();

			m_iBall[0] = 192;
			m_iBallTime[0] = gpGlobals->time + atoi( event.options ) / 15.0;
			m_iBall[1] = 255;
			m_iBallTime[1] = gpGlobals->time + atoi( event.options ) / 15.0;

		}
		break;

		case CONTROLLER_AE_BALL_SHOOT:
		{
			Vector vecStart, angleGun;
			
			GetAttachment( 0, vecStart, angleGun );

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_ELIGHT );
				WRITE_SHORT( entindex( ) + 0x1000 );		// entity, attachment
				WRITE_COORD( 0 );		// origin
				WRITE_COORD( 0 );
				WRITE_COORD( 0 );
				WRITE_COORD( 32 );	// radius
				WRITE_BYTE( 255 );	// R
				WRITE_BYTE( 192 );	// G
				WRITE_BYTE( 64 );	// B
				WRITE_BYTE( 10 );	// life * 10
				WRITE_COORD( 32 ); // decay
			MESSAGE_END();

			CBaseMonster *pBall = (CBaseMonster*)Create( "controller_head_ball", vecStart, GetAbsAngles(), edict() );

			pBall->SetAbsVelocity( Vector( 0, 0, 32 ) );
			pBall->m_hEnemy = m_hEnemy;

			m_iBall[0] = 0;
			m_iBall[1] = 0;
		}
		break;

		case CONTROLLER_AE_SMALL_SHOOT:
		{
			AttackSound( );
			m_flShootTime = gpGlobals->time;
			m_flShootEnd = m_flShootTime + atoi( event.options ) / 15.0;
		}
		break;
		case CONTROLLER_AE_POWERUP_FULL:
		{
			m_iBall[0] = 255;
			m_iBallTime[0] = gpGlobals->time + atoi( event.options ) / 15.0;
			m_iBall[1] = 255;
			m_iBallTime[1] = gpGlobals->time + atoi( event.options ) / 15.0;
		}
		break;
		case CONTROLLER_AE_POWERUP_HALF:
		{
			m_iBall[0] = 192;
			m_iBallTime[0] = gpGlobals->time + atoi( event.options ) / 15.0;
			m_iBall[1] = 192;
			m_iBallTime[1] = gpGlobals->time + atoi( event.options ) / 15.0;
		}
		break;
		default:
			CBaseMonster::HandleAnimEvent( event );
			break;
	}
}

void CController :: Spawn()
{
	Precache( );

	SetModel( "models/controller.mdl");
	SetSize( Vector( -32, -32, 0 ), Vector( 32, 32, 64 ));

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_FLY );
	GetFlags() |= FL_FLY;
	m_bloodColor		= BLOOD_COLOR_GREEN;
	SetHealth( gSkillData.GetControllerHealth() );
	SetViewOffset( Vector( 0, 0, -2 ) );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= VIEW_FIELD_FULL;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();
}

void CController :: Precache()
{
	PRECACHE_MODEL("models/controller.mdl");

	PRECACHE_SOUND_ARRAY( pAttackSounds );
	PRECACHE_SOUND_ARRAY( pIdleSounds );
	PRECACHE_SOUND_ARRAY( pAlertSounds );
	PRECACHE_SOUND_ARRAY( pPainSounds );
	PRECACHE_SOUND_ARRAY( pDeathSounds );

	PRECACHE_MODEL( "sprites/xspark4.spr");

	UTIL_PrecacheOther( "controller_energy_ball" );
	UTIL_PrecacheOther( "controller_head_ball" );
}	

void CController::UpdateOnRemove()
{
	BaseClass::UpdateOnRemove();

	if( m_pBall[ 0 ] )
	{
		UTIL_Remove( m_pBall[ 0 ] );
		m_pBall[ 0 ] = nullptr;
	}

	if( m_pBall[ 1 ] )
	{
		UTIL_Remove( m_pBall[ 1 ] );
		m_pBall[ 1 ] = nullptr;
	}
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================


// Chase enemy schedule
Task_t tlControllerChaseEnemy[] = 
{
	{ TASK_GET_PATH_TO_ENEMY,	(float)128		},
	{ TASK_WAIT_FOR_MOVEMENT,	(float)0		},

};

Schedule_t slControllerChaseEnemy[] =
{
	{ 
		tlControllerChaseEnemy,
		ARRAYSIZE ( tlControllerChaseEnemy ),
		bits_COND_NEW_ENEMY			|
		bits_COND_TASK_FAILED,
		0,
		"ControllerChaseEnemy"
	},
};



Task_t	tlControllerStrafe[] =
{
	{ TASK_WAIT,					(float)0.2					},
	{ TASK_GET_PATH_TO_ENEMY,		(float)128					},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0					},
	{ TASK_WAIT,					(float)1					},
};

Schedule_t	slControllerStrafe[] =
{
	{ 
		tlControllerStrafe,
		ARRAYSIZE ( tlControllerStrafe ), 
		bits_COND_NEW_ENEMY,
		0,
		"ControllerStrafe"
	},
};


Task_t	tlControllerTakeCover[] =
{
	{ TASK_WAIT,					(float)0.2					},
	{ TASK_FIND_COVER_FROM_ENEMY,	(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0					},
	{ TASK_WAIT,					(float)1					},
};

Schedule_t	slControllerTakeCover[] =
{
	{ 
		tlControllerTakeCover,
		ARRAYSIZE ( tlControllerTakeCover ), 
		bits_COND_NEW_ENEMY,
		0,
		"ControllerTakeCover"
	},
};


Task_t	tlControllerFail[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT,				(float)2		},
	{ TASK_WAIT_PVS,			(float)0		},
};

Schedule_t	slControllerFail[] =
{
	{
		tlControllerFail,
		ARRAYSIZE ( tlControllerFail ),
		0,
		0,
		"ControllerFail"
	},
};



BEGIN_SCHEDULES( CController )
	slControllerChaseEnemy,
	slControllerStrafe,
	slControllerTakeCover,
	slControllerFail,
END_SCHEDULES()

void CController :: StartTask ( const Task_t& task )
{
	switch ( task.iTask )
	{
	case TASK_RANGE_ATTACK1:
		CSquadMonster :: StartTask ( task );
		break;
	case TASK_GET_PATH_TO_ENEMY_LKP:
		{
			if (BuildNearestRoute( m_vecEnemyLKP, GetViewOffset(), task.flData, (m_vecEnemyLKP - GetAbsOrigin()).Length() + 1024 ))
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToEnemyLKP failed!!\n" );
				TaskFail();
			}
			break;
		}
	case TASK_GET_PATH_TO_ENEMY:
		{
			CBaseEntity *pEnemy = m_hEnemy;

			if ( pEnemy == NULL )
			{
				TaskFail();
				return;
			}

			if (BuildNearestRoute( pEnemy->GetAbsOrigin(), pEnemy->GetViewOffset(), task.flData, (pEnemy->GetAbsOrigin() - GetAbsOrigin()).Length() + 1024 ))
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToEnemy failed!!\n" );
				TaskFail();
			}
			break;
		}
	default:
		CSquadMonster :: StartTask ( task );
		break;
	}
}


Vector Intersect( Vector vecSrc, Vector vecDst, Vector vecMove, float flSpeed )
{
	Vector vecTo = vecDst - vecSrc;

	float a = DotProduct( vecMove, vecMove ) - flSpeed * flSpeed;
	float b = 0 * DotProduct(vecTo, vecMove); // why does this work?
	float c = DotProduct( vecTo, vecTo );

	float t;
	if (a == 0)
	{
		t = c / (flSpeed * flSpeed);
	}
	else
	{
		t = b * b - 4 * a * c;
		t = sqrt( t ) / (2.0 * a);
		float t1 = -b +t;
		float t2 = -b -t;

		if (t1 < 0 || t2 < t1)
			t = t2;
		else
			t = t1;
	}

	// ALERT( at_console, "Intersect %f\n", t );

	if (t < 0.1)
		t = 0.1;
	if (t > 10.0)
		t = 10.0;

	Vector vecHit = vecTo + vecMove * t;
	return vecHit.Normalize( ) * flSpeed;
}


int CController::LookupFloat( )
{
	if (m_velocity.Length( ) < 32.0)
	{
		return LookupSequence( "up" );
	}

	UTIL_MakeAimVectors( GetAbsAngles() );
	float x = DotProduct( gpGlobals->v_forward, m_velocity );
	float y = DotProduct( gpGlobals->v_right, m_velocity );
	float z = DotProduct( gpGlobals->v_up, m_velocity );

	if (fabs(x) > fabs(y) && fabs(x) > fabs(z))
	{
		if (x > 0)
			return LookupSequence( "forward");
		else
			return LookupSequence( "backward");
	}
	else if (fabs(y) > fabs(z))
	{
		if (y > 0)
			return LookupSequence( "right");
		else
			return LookupSequence( "left");
	}
	else
	{
		if (z > 0)
			return LookupSequence( "up");
		else
			return LookupSequence( "down");
	}
}

void CController :: RunTask ( const Task_t& task )
{

	if (m_flShootEnd > gpGlobals->time)
	{
		Vector vecHand, vecAngle;
		
		GetAttachment( 2, vecHand, vecAngle );
	
		while (m_flShootTime < m_flShootEnd && m_flShootTime < gpGlobals->time)
		{
			Vector vecSrc = vecHand + GetAbsVelocity() * (m_flShootTime - gpGlobals->time);
			Vector vecDir;
			
			if (m_hEnemy != NULL)
			{
				if (HasConditions( bits_COND_SEE_ENEMY ))
				{
					m_vecEstVelocity = m_vecEstVelocity * 0.5 + m_hEnemy->GetAbsVelocity() * 0.5;
				}
				else
				{
					m_vecEstVelocity = m_vecEstVelocity * 0.8;
				}
				vecDir = Intersect( vecSrc, m_hEnemy->BodyTarget( GetAbsOrigin() ), m_vecEstVelocity, gSkillData.GetControllerSpeedBall() );
				float delta = 0.03490; // +-2 degree
				vecDir = vecDir + Vector( RANDOM_FLOAT( -delta, delta ), RANDOM_FLOAT( -delta, delta ), RANDOM_FLOAT( -delta, delta ) ) * gSkillData.GetControllerSpeedBall();

				vecSrc = vecSrc + vecDir * (gpGlobals->time - m_flShootTime);
				CBaseMonster *pBall = (CBaseMonster*)Create( "controller_energy_ball", vecSrc, GetAbsAngles(), edict() );
				pBall->SetAbsVelocity( vecDir );
			}
			m_flShootTime += 0.2;
		}

		if (m_flShootTime > m_flShootEnd)
		{
			m_iBall[0] = 64;
			m_iBallTime[0] = m_flShootEnd;
			m_iBall[1] = 64;
			m_iBallTime[1] = m_flShootEnd;
			m_fInCombat = false;
		}
	}

	switch ( task.iTask )
	{
	case TASK_WAIT_FOR_MOVEMENT:
	case TASK_WAIT:
	case TASK_WAIT_FACE_ENEMY:
	case TASK_WAIT_PVS:
		MakeIdealYaw( m_vecEnemyLKP );
		ChangeYaw( GetYawSpeed() );

		if (m_fSequenceFinished)
		{
			m_fInCombat = false;
		}

		CSquadMonster :: RunTask ( task );

		if (!m_fInCombat)
		{
			if (HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ))
			{
				SetSequence( LookupActivity( ACT_RANGE_ATTACK1 ) );
				SetFrame( 0 );
				ResetSequenceInfo( );
				m_fInCombat = true;
			}
			else if (HasConditions ( bits_COND_CAN_RANGE_ATTACK2 ))
			{
				SetSequence( LookupActivity( ACT_RANGE_ATTACK2 ) );
				SetFrame( 0 );
				ResetSequenceInfo( );
				m_fInCombat = true;
			}
			else
			{
				int iFloat = LookupFloat( );
				if (m_fSequenceFinished || iFloat != GetSequence() )
				{
					SetSequence( iFloat );
					SetFrame( 0 );
					ResetSequenceInfo( );
				}
			}
		}
		break;
	default: 
		CSquadMonster :: RunTask ( task );
		break;
	}
}

Schedule_t *CController :: GetSchedule ( void )
{
	switch	( m_MonsterState )
	{
	case MONSTERSTATE_COMBAT:
		{
			//Vector vecTmp = Intersect( Vector( 0, 0, 0 ), Vector( 100, 4, 7 ), Vector( 2, 10, -3 ), 20.0 );

			// dead enemy
			if ( HasConditions ( bits_COND_LIGHT_DAMAGE ) )
			{
				// m_iFrustration++;
			}
			if ( HasConditions ( bits_COND_HEAVY_DAMAGE ) )
			{
				// m_iFrustration++;
			}
		}
		break;

	default: break;
	}

	return CSquadMonster :: GetSchedule();
}



//=========================================================
//=========================================================
Schedule_t* CController :: GetScheduleOfType ( int Type ) 
{
	// ALERT( at_console, "%d\n", m_iFrustration );
	switch	( Type )
	{
	case SCHED_CHASE_ENEMY:
		return slControllerChaseEnemy;
	case SCHED_RANGE_ATTACK1:
		return slControllerStrafe;
	case SCHED_RANGE_ATTACK2:
	case SCHED_MELEE_ATTACK1:
	case SCHED_MELEE_ATTACK2:
	case SCHED_TAKE_COVER_FROM_ENEMY:
		return slControllerTakeCover;
	case SCHED_FAIL:
		return slControllerFail;
	}

	return CBaseMonster :: GetScheduleOfType( Type );
}

bool CController :: CheckRangeAttack1 ( float flDot, float flDist )
{
	if ( flDot > 0.5 && flDist > 256 && flDist <= 2048 )
	{
		return true;
	}
	return false;
}


bool CController :: CheckRangeAttack2 ( float flDot, float flDist )
{
	if ( flDot > 0.5 && flDist > 64 && flDist <= 2048 )
	{
		return true;
	}
	return false;
}


bool CController :: CheckMeleeAttack1 ( float flDot, float flDist )
{
	return false;
}


void CController :: SetActivity ( Activity NewActivity )
{
	CBaseMonster::SetActivity( NewActivity );

	switch ( m_Activity)
	{
	case ACT_WALK:
		m_flGroundSpeed = 100;
		break;
	default:
		m_flGroundSpeed = 100;
		break;
	}
}



//=========================================================
// RunAI
//=========================================================
void CController :: RunAI( void )
{
	CBaseMonster :: RunAI();
	Vector vecStart, angleGun;

	if ( HasMemory( bits_MEMORY_KILLED ) )
		return;

	for (int i = 0; i < 2; i++)
	{
		if (m_pBall[i] == NULL)
		{
			m_pBall[i] = CSprite::SpriteCreate( "sprites/xspark4.spr", GetAbsOrigin(), true );
			m_pBall[i]->SetTransparency( kRenderGlow, 255, 255, 255, 255, kRenderFxNoDissipation );
			m_pBall[i]->SetAttachment( this, (i + 3) );
			m_pBall[i]->SetScale( 1.0 );
		}

		float t = m_iBallTime[i] - gpGlobals->time;
		if (t > 0.1)
			t = 0.1 / t;
		else
			t = 1.0;

		m_iBallCurrent[i] += (m_iBall[i] - m_iBallCurrent[i]) * t;

		m_pBall[i]->SetBrightness( m_iBallCurrent[i] );

		GetAttachment( i + 2, vecStart, angleGun );
		m_pBall[i]->SetAbsOrigin( vecStart );
		
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_ELIGHT );
			WRITE_SHORT( entindex( ) + 0x1000 * (i + 3) );		// entity, attachment
			WRITE_COORD( vecStart.x );		// origin
			WRITE_COORD( vecStart.y );
			WRITE_COORD( vecStart.z );
			WRITE_COORD( m_iBallCurrent[i] / 8 );	// radius
			WRITE_BYTE( 255 );	// R
			WRITE_BYTE( 192 );	// G
			WRITE_BYTE( 64 );	// B
			WRITE_BYTE( 5 );	// life * 10
			WRITE_COORD( 0 ); // decay
		MESSAGE_END();
	}
}


extern void DrawRoute( CBaseEntity* pEntity, WayPoint_t *m_Route, int m_iRouteIndex, int r, int g, int b );

void CController::Stop( void ) 
{ 
	m_IdealActivity = GetStoppedActivity(); 
}


#define DIST_TO_CHECK	200
void CController :: Move ( float flInterval ) 
{
	float		flWaypointDist;
	float		flCheckDist;
	float		flDist;// how far the lookahead check got before hitting an object.
	float		flMoveDist;
	Vector		vecDir;
	Vector		vecApex;
	CBaseEntity	*pTargetEnt;

	// Don't move if no valid route
	if ( FRouteClear() )
	{
		ALERT( at_aiconsole, "Tried to move with no route!\n" );
		TaskFail();
		return;
	}
	
	if ( m_flMoveWaitFinished > gpGlobals->time )
		return;

// Debug, test movement code
#if 0
//	if ( CVAR_GET_FLOAT("stopmove" ) != 0 )
	{
		if ( m_movementGoal == MOVEGOAL_ENEMY )
			RouteSimplify( m_hEnemy );
		else
			RouteSimplify( m_hTargetEnt );
		FRefreshRoute();
		return;
	}
#else
// Debug, draw the route
//	DrawRoute( pev, m_Route, m_iRouteIndex, 0, 0, 255 );
#endif

	// if the monster is moving directly towards an entity (enemy for instance), we'll set this pointer
	// to that entity for the CheckLocalMove and Triangulate functions.
	pTargetEnt = NULL;

	if (m_flGroundSpeed == 0)
	{
		m_flGroundSpeed = 100;
		// TaskFail( );
		// return;
	}

	flMoveDist = m_flGroundSpeed * flInterval;

	do 
	{
		// local move to waypoint.
		vecDir = ( m_Route[ m_iRouteIndex ].vecLocation - GetAbsOrigin() ).Normalize();
		flWaypointDist = ( m_Route[ m_iRouteIndex ].vecLocation - GetAbsOrigin() ).Length();
		
		// MakeIdealYaw ( m_Route[ m_iRouteIndex ].vecLocation );
		// ChangeYaw( GetYawSpeed() );

		// if the waypoint is closer than CheckDist, CheckDist is the dist to waypoint
		if ( flWaypointDist < DIST_TO_CHECK )
		{
			flCheckDist = flWaypointDist;
		}
		else
		{
			flCheckDist = DIST_TO_CHECK;
		}
		
		if ( (m_Route[ m_iRouteIndex ].iType & (~bits_MF_NOT_TO_MASK)) == bits_MF_TO_ENEMY )
		{
			// only on a PURE move to enemy ( i.e., ONLY MF_TO_ENEMY set, not MF_TO_ENEMY and DETOUR )
			pTargetEnt = m_hEnemy;
		}
		else if ( (m_Route[ m_iRouteIndex ].iType & ~bits_MF_NOT_TO_MASK) == bits_MF_TO_TARGETENT )
		{
			pTargetEnt = m_hTargetEnt;
		}

		// !!!BUGBUG - CheckDist should be derived from ground speed.
		// If this fails, it should be because of some dynamic entity blocking this guy.
		// We've already checked this path, so we should wait and time out if the entity doesn't move
		flDist = 0;
		if ( CheckLocalMove ( GetAbsOrigin(), GetAbsOrigin() + vecDir * flCheckDist, pTargetEnt, &flDist ) != LOCALMOVE_VALID )
		{
			CBaseEntity *pBlocker;

			// Can't move, stop
			Stop();
			// Blocking entity is in global trace_ent
			pBlocker = CBaseEntity::Instance( gpGlobals->trace_ent );
			if (pBlocker)
			{
				this->Blocked( pBlocker );
			}
			if ( pBlocker && m_moveWaitTime > 0 && pBlocker->IsMoving() && !pBlocker->IsPlayer() && (gpGlobals->time-m_flMoveWaitFinished) > 3.0 )
			{
				// Can we still move toward our target?
				if ( flDist < m_flGroundSpeed )
				{
					// Wait for a second
					m_flMoveWaitFinished = gpGlobals->time + m_moveWaitTime;
	//				ALERT( at_aiconsole, "Move %s!!!\n", pBlocker->GetClassname() );
					return;
				}
			}
			else 
			{
				// try to triangulate around whatever is in the way.
				if ( FTriangulate( GetAbsOrigin(), m_Route[ m_iRouteIndex ].vecLocation, flDist, pTargetEnt, &vecApex ) )
				{
					InsertWaypoint( vecApex, bits_MF_TO_DETOUR );
					RouteSimplify( pTargetEnt );
				}
				else
				{
	 			    ALERT ( at_aiconsole, "Couldn't Triangulate\n" );
					Stop();
					if ( m_moveWaitTime > 0 )
					{
						FRefreshRoute();
						m_flMoveWaitFinished = gpGlobals->time + m_moveWaitTime * 0.5;
					}
					else
					{
						TaskFail();
						ALERT( at_aiconsole, "Failed to move!\n" );
						//ALERT( at_aiconsole, "%f, %f, %f\n", GetAbsOrigin().z, (GetAbsOrigin() + (vecDir * flCheckDist)).z, m_Route[m_iRouteIndex].vecLocation.z );
					}
					return;
				}
			}
		}

		// UNDONE: this is a hack to quit moving farther than it has looked ahead.
		if (flCheckDist < flMoveDist)
		{
			MoveExecute( pTargetEnt, vecDir, flCheckDist / m_flGroundSpeed );

			// ALERT( at_console, "%.02f\n", flInterval );
			AdvanceRoute( flWaypointDist );
			flMoveDist -= flCheckDist;
		}
		else
		{
			MoveExecute( pTargetEnt, vecDir, flMoveDist / m_flGroundSpeed );

			if ( ShouldAdvanceRoute( flWaypointDist - flMoveDist ) )
			{
				AdvanceRoute( flWaypointDist );
			}
			flMoveDist = 0;
		}

		if ( MovementIsComplete() )
		{
			Stop();
			RouteClear();
		}
	} while (flMoveDist > 0 && flCheckDist > 0);

	// cut corner?
	if (flWaypointDist < 128)
	{
		if ( m_movementGoal == MOVEGOAL_ENEMY )
			RouteSimplify( m_hEnemy );
		else
			RouteSimplify( m_hTargetEnt );
		FRefreshRoute();

		if (m_flGroundSpeed > 100)
			m_flGroundSpeed -= 40;
	}
	else
	{
		if (m_flGroundSpeed < 400)
			m_flGroundSpeed += 10;
	}
}



bool CController::ShouldAdvanceRoute( float flWaypointDist )
{
	if ( flWaypointDist <= 32  )
	{
		return true;
	}

	return false;
}


int CController :: CheckLocalMove ( const Vector &vecStart, const Vector &vecEnd, const CBaseEntity* const pTarget, float *pflDist )
{
	TraceResult tr;

	UTIL_TraceHull( vecStart + Vector( 0, 0, 32), vecEnd + Vector( 0, 0, 32), dont_ignore_monsters, Hull::LARGE, edict(), &tr );

	// ALERT( at_console, "%.0f %.0f %.0f : ", vecStart.x, vecStart.y, vecStart.z );
	// ALERT( at_console, "%.0f %.0f %.0f\n", vecEnd.x, vecEnd.y, vecEnd.z );

	if (pflDist)
	{
		*pflDist = ( (tr.vecEndPos - Vector( 0, 0, 32 )) - vecStart ).Length();// get the distance.
	}

	// ALERT( at_console, "check %d %d %f\n", tr.fStartSolid, tr.fAllSolid, tr.flFraction );
	if (tr.fStartSolid || tr.flFraction < 1.0)
	{
		if ( pTarget && pTarget->edict() == gpGlobals->trace_ent )
			return LOCALMOVE_VALID;
		return LOCALMOVE_INVALID;
	}

	return LOCALMOVE_VALID;
}


void CController::MoveExecute( CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval )
{
	if ( m_IdealActivity != m_movementActivity )
		m_IdealActivity = m_movementActivity;

	// ALERT( at_console, "move %.4f %.4f %.4f : %f\n", vecDir.x, vecDir.y, vecDir.z, flInterval );

	// float flTotal = m_flGroundSpeed * GetFrameRate() * flInterval;
	// UTIL_MoveToOrigin ( this, m_Route[ m_iRouteIndex ].vecLocation, flTotal, MOVE_STRAFE );

	m_velocity = m_velocity * 0.8 + m_flGroundSpeed * vecDir * 0.2;

	UTIL_MoveToOrigin( this, GetAbsOrigin() + m_velocity, m_velocity.Length() * flInterval, MOVE_STRAFE );
}