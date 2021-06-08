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
// bullsquid - big, spotty tentacle-mouthed meanie.
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/NPCs/Monsters.h"
#include "entities/NPCs/Schedule.h"
#include "nodes/Nodes.h"
#include "Effects.h"
#include "Decals.h"
#include "entities/CSoundEnt.h"
#include "Server.h"

#include "CSquidSpit.h"

#include "CBullsquid.h"

BEGIN_DATADESC(	CBullsquid )
	DEFINE_FIELD( m_fCanThreatDisplay, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flLastHurtTime, FIELD_TIME ),
	DEFINE_FIELD( m_flNextSpitTime, FIELD_TIME ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_bullchicken, CBullsquid );

//=========================================================
// IgnoreConditions 
//=========================================================
int CBullsquid::IgnoreConditions ( void )
{
	int iIgnore = CBaseMonster::IgnoreConditions();

	if ( gpGlobals->time - m_flLastHurtTime <= 20 )
	{
		// haven't been hurt in 20 seconds, so let the squid care about stink. 
		iIgnore = bits_COND_SMELL | bits_COND_SMELL_FOOD;
	}

	if ( m_hEnemy != NULL )
	{
		if ( m_hEnemy->ClassnameIs( "monster_headcrab" ) )
		{
			// (Unless after a tasty headcrab)
			iIgnore = bits_COND_SMELL | bits_COND_SMELL_FOOD;
		}
	}


	return iIgnore;
}

Relationship CBullsquid::IRelationship ( CBaseEntity *pTarget )
{
	if ( gpGlobals->time - m_flLastHurtTime < 5 && pTarget->ClassnameIs( "monster_headcrab" ) )
	{
		// if squid has been hurt in the last 5 seconds, and is getting relationship for a headcrab, 
		// tell squid to disregard crab. 
		return R_NO;
	}

	return CBaseMonster :: IRelationship ( pTarget );
}

//=========================================================
// OnTakeDamage - overridden for bullsquid so we can keep track
// of how much time has passed since it was last injured
//=========================================================
void CBullsquid::OnTakeDamage( const CTakeDamageInfo& info )
{
	float flDist;
	Vector vecApex;

	// if the squid is running, has an enemy, was hurt by the enemy, hasn't been hurt in the last 3 seconds, and isn't too close to the enemy,
	// it will swerve. (whew).
	if ( m_hEnemy != NULL && IsMoving() && info.GetAttacker() == m_hEnemy && gpGlobals->time - m_flLastHurtTime > 3 )
	{
		flDist = ( GetAbsOrigin() - m_hEnemy->GetAbsOrigin() ).Length2D();
		
		if ( flDist > SQUID_SPRINT_DIST )
		{
			flDist = ( GetAbsOrigin() - m_Route[ m_iRouteIndex ].vecLocation ).Length2D();// reusing flDist. 

			if ( FTriangulate( GetAbsOrigin(), m_Route[ m_iRouteIndex ].vecLocation, flDist * 0.5, m_hEnemy, &vecApex ) )
			{
				InsertWaypoint( vecApex, bits_MF_TO_DETOUR | bits_MF_DONT_SIMPLIFY );
			}
		}
	}

	if ( !info.GetAttacker()->ClassnameIs( "monster_headcrab" ) )
	{
		// don't forget about headcrabs if it was a headcrab that hurt the squid.
		m_flLastHurtTime = gpGlobals->time;
	}

	CBaseMonster::OnTakeDamage( info );
}

bool CBullsquid :: CheckRangeAttack1 ( float flDot, float flDist )
{
	if ( IsMoving() && flDist >= 512 )
	{
		// squid will far too far behind if he stops running to spit at this distance from the enemy.
		return false;
	}

	if ( flDist > 64 && flDist <= 784 && flDot >= 0.5 && gpGlobals->time >= m_flNextSpitTime )
	{
		if ( m_hEnemy != NULL )
		{
			if ( fabs( GetAbsOrigin().z - m_hEnemy->GetAbsOrigin().z ) > 256 )
			{
				// don't try to spit at someone up really high or down really low.
				return false;
			}
		}

		if ( IsMoving() )
		{
			// don't spit again for a long time, resume chasing enemy.
			m_flNextSpitTime = gpGlobals->time + 5;
		}
		else
		{
			// not moving, so spit again pretty soon.
			m_flNextSpitTime = gpGlobals->time + 0.5;
		}

		return true;
	}

	return false;
}

bool CBullsquid :: CheckMeleeAttack1 ( float flDot, float flDist )
{
	if ( m_hEnemy->GetHealth() <= gSkillData.GetBullsquidDmgWhip() && flDist <= 85 && flDot >= 0.7 )
	{
		return true;
	}
	return false;
}

bool CBullsquid :: CheckMeleeAttack2 ( float flDot, float flDist )
{
	//TODO: compute distance based on target size - Solokiller
	if ( flDist <= 85 && flDot >= 0.7 && !HasConditions( bits_COND_CAN_MELEE_ATTACK1 ) )		// The player & bullsquid can be as much as their bboxes 
	{										// apart (48 * sqrt(3)) and he can still attack (85 is a little more than 48*sqrt(3))
		return true;
	}
	return false;
}  

//=========================================================
//  FValidateHintType 
//=========================================================
bool CBullsquid::FValidateHintType( short sHint ) const
{
	static short sSquidHints[] =
	{
		HINT_WORLD_HUMAN_BLOOD,
	};

	for ( size_t i = 0 ; i < ARRAYSIZE ( sSquidHints ) ; i++ )
	{
		if ( sSquidHints[ i ] == sHint )
		{
			return true;
		}
	}

	ALERT ( at_aiconsole, "Couldn't validate hint type" );
	return false;
}

int CBullsquid :: ISoundMask ( void )
{
	return	bits_SOUND_WORLD	|
			bits_SOUND_COMBAT	|
			bits_SOUND_CARCASS	|
			bits_SOUND_MEAT		|
			bits_SOUND_GARBAGE	|
			bits_SOUND_PLAYER;
}

EntityClassification_t CBullsquid::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::ALIEN_PREDATOR );
}

//=========================================================
// IdleSound 
//=========================================================
#define SQUID_ATTN_IDLE	(float)1.5
void CBullsquid :: IdleSound ( void )
{
	switch ( RANDOM_LONG(0,4) )
	{
	case 0:	
		EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_idle1.wav", 1, SQUID_ATTN_IDLE );
		break;
	case 1:	
		EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_idle2.wav", 1, SQUID_ATTN_IDLE );
		break;
	case 2:	
		EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_idle3.wav", 1, SQUID_ATTN_IDLE );
		break;
	case 3:	
		EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_idle4.wav", 1, SQUID_ATTN_IDLE );
		break;
	case 4:	
		EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_idle5.wav", 1, SQUID_ATTN_IDLE );
		break;
	}
}

void CBullsquid :: PainSound ( void )
{
	int iPitch = RANDOM_LONG( 85, 120 );

	switch ( RANDOM_LONG(0,3) )
	{
	case 0:	
		EMIT_SOUND_DYN( this, CHAN_VOICE, "bullchicken/bc_pain1.wav", 1, ATTN_NORM, 0, iPitch );
		break;
	case 1:	
		EMIT_SOUND_DYN( this, CHAN_VOICE, "bullchicken/bc_pain2.wav", 1, ATTN_NORM, 0, iPitch );
		break;
	case 2:	
		EMIT_SOUND_DYN( this, CHAN_VOICE, "bullchicken/bc_pain3.wav", 1, ATTN_NORM, 0, iPitch );
		break;
	case 3:	
		EMIT_SOUND_DYN( this, CHAN_VOICE, "bullchicken/bc_pain4.wav", 1, ATTN_NORM, 0, iPitch );
		break;
	}
}

void CBullsquid :: AlertSound ( void )
{
	int iPitch = RANDOM_LONG( 140, 160 );

	switch ( RANDOM_LONG ( 0, 1  ) )
	{
	case 0:
		EMIT_SOUND_DYN( this, CHAN_VOICE, "bullchicken/bc_idle1.wav", 1, ATTN_NORM, 0, iPitch );
		break;
	case 1:
		EMIT_SOUND_DYN( this, CHAN_VOICE, "bullchicken/bc_idle2.wav", 1, ATTN_NORM, 0, iPitch );
		break;
	}
}

void CBullsquid::UpdateYawSpeed()
{
	int ys;

	ys = 0;

	switch ( m_Activity )
	{
	case	ACT_WALK:			ys = 90;	break;
	case	ACT_RUN:			ys = 90;	break;
	case	ACT_IDLE:			ys = 90;	break;
	case	ACT_RANGE_ATTACK1:	ys = 90;	break;
	default:
		ys = 90;
		break;
	}

	SetYawSpeed( ys );
}

void CBullsquid :: HandleAnimEvent( AnimEvent_t& event )
{
	switch( event.event )
	{
		case BSQUID_AE_SPIT:
		{
			Vector	vecSpitOffset;
			Vector	vecSpitDir;

			UTIL_MakeVectors ( GetAbsAngles() );

			// !!!HACKHACK - the spot at which the spit originates (in front of the mouth) was measured in 3ds and hardcoded here.
			// we should be able to read the position of bones at runtime for this info.
			vecSpitOffset = ( gpGlobals->v_right * 8 + gpGlobals->v_forward * 37 + gpGlobals->v_up * 23 );		
			vecSpitOffset = ( GetAbsOrigin() + vecSpitOffset );
			vecSpitDir = ( ( m_hEnemy->GetAbsOrigin() + m_hEnemy->GetViewOffset() ) - vecSpitOffset ).Normalize();

			vecSpitDir.x += RANDOM_FLOAT( -0.05, 0.05 );
			vecSpitDir.y += RANDOM_FLOAT( -0.05, 0.05 );
			vecSpitDir.z += RANDOM_FLOAT( -0.05, 0 );


			// do stuff for this event.
			AttackSound();

			// spew the spittle temporary ents.
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpitOffset );
				WRITE_BYTE( TE_SPRITE_SPRAY );
				WRITE_COORD( vecSpitOffset.x);	// pos
				WRITE_COORD( vecSpitOffset.y);	
				WRITE_COORD( vecSpitOffset.z);	
				WRITE_COORD( vecSpitDir.x);	// dir
				WRITE_COORD( vecSpitDir.y);	
				WRITE_COORD( vecSpitDir.z);	
				WRITE_SHORT( iSquidSpitSprite );	// model
				// ############ hu3lifezado ############ //
				// Vomito volumoso (15)
				WRITE_BYTE ( 100 );			// count
				// ############ hu3lifezado ############ //
				WRITE_BYTE ( 210 );			// speed
				WRITE_BYTE ( 25 );			// noise ( client will divide by 100 )
			MESSAGE_END();

			CSquidSpit::Shoot( this, vecSpitOffset, vecSpitDir * 900 );
		}
		break;

		case BSQUID_AE_BITE:
		{
			// SOUND HERE!
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, gSkillData.GetBullsquidDmgBite(), DMG_SLASH );
			
			if ( pHurt )
			{
				//Vector vecPunchAngle = pHurt->GetPunchAngle();
				//vecPunchAngle.z = -15;
				//vecPunchAngle.x = -45;
				//pHurt->SetPunchAngle( vecPunchAngle );
				Vector vecVelocity = pHurt->GetAbsVelocity();
				vecVelocity = vecVelocity - gpGlobals->v_forward * 100;
				vecVelocity = vecVelocity + gpGlobals->v_up * 100;
				pHurt->SetAbsVelocity( vecVelocity );
			}
		}
		break;

		case BSQUID_AE_TAILWHIP:
		{
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, gSkillData.GetBullsquidDmgWhip(), DMG_CLUB | DMG_ALWAYSGIB );
			if ( pHurt ) 
			{
				Vector vecPunchAngle = pHurt->GetPunchAngle();
				vecPunchAngle.z = -20;
				vecPunchAngle.x = 20;
				pHurt->SetPunchAngle( vecPunchAngle );
				Vector vecVelocity = pHurt->GetAbsVelocity();
				vecVelocity = vecVelocity + gpGlobals->v_right * 200;
				vecVelocity = vecVelocity + gpGlobals->v_up * 100;
				pHurt->SetAbsVelocity( vecVelocity );
			}
		}
		break;

		case BSQUID_AE_BLINK:
		{
			// close eye. 
				SetSkin( 1 );
		}
		break;

		case BSQUID_AE_HOP:
		{
			float flGravity = g_psv_gravity->value;

			// throw the squid up into the air on this frame.
			if ( GetFlags().Any( FL_ONGROUND ) )
			{
				GetFlags().ClearFlags( FL_ONGROUND );
			}

			// jump into air for 0.8 (24/30) seconds
			Vector vecVelocity = GetAbsVelocity();
//			vecVelocity.z += (0.875 * flGravity) * 0.5;
			vecVelocity.z += (0.625 * flGravity) * 0.5;
			SetAbsVelocity( vecVelocity );
		}
		break;

		case BSQUID_AE_THROW:
			{
				int iPitch;

				// squid throws its prey IF the prey is a client. 
				CBaseEntity *pHurt = CheckTraceHullAttack( 70, 0, 0 );


				if ( pHurt )
				{
					// croonchy bite sound
					iPitch = RANDOM_FLOAT( 90, 110 );
					switch ( RANDOM_LONG( 0, 1 ) )
					{
					case 0:
						EMIT_SOUND_DYN( this, CHAN_WEAPON, "bullchicken/bc_bite2.wav", 1, ATTN_NORM, 0, iPitch );
						break;
					case 1:
						EMIT_SOUND_DYN( this, CHAN_WEAPON, "bullchicken/bc_bite3.wav", 1, ATTN_NORM, 0, iPitch );
						break;
					}

					//pHurt->SetPunchAngle( Vector(
					//	RANDOM_LONG( 0, 34 ) - 5,
					//	RANDOM_LONG( 0, 89 ) - 45,
					//	RANDOM_LONG( 0, 49 ) - 25
					//) );
		
					// screeshake transforms the viewmodel as well as the viewangle. No problems with seeing the ends of the viewmodels.
					UTIL_ScreenShake( pHurt->GetAbsOrigin(), 25.0, 1.5, 0.7, 2 );

					if ( pHurt->IsPlayer() )
					{
						UTIL_MakeVectors( GetAbsAngles() );
						// ############ hu3lifezado ############ //
						// Pancada do dinossauro arremessa player (* 300)
						pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() + gpGlobals->v_forward * 1000 + gpGlobals->v_up * 1000 );
						// ############ //
					}
				}
			}
		break;

		default:
			CBaseMonster::HandleAnimEvent( event );
	}
}

void CBullsquid :: Spawn()
{
	Precache( );

	SetModel( "models/bullsquid.mdl");
	SetSize( Vector( -32, -32, 0 ), Vector( 32, 32, 64 ) );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= BLOOD_COLOR_GREEN;
	GetEffects().ClearAll();
	SetHealth( gSkillData.GetBullsquidHealth() );
	m_flFieldOfView		= 0.2;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	m_fCanThreatDisplay	= true;
	m_flNextSpitTime = gpGlobals->time;

	MonsterInit();
}

void CBullsquid :: Precache()
{
	PRECACHE_MODEL("models/bullsquid.mdl");
	
	PRECACHE_MODEL("sprites/bigspit.spr");// spit projectile.
	
	iSquidSpitSprite = PRECACHE_MODEL("sprites/tinyspit.spr");// client side spittle.

	PRECACHE_SOUND("zombie/claw_miss2.wav");// because we use the basemonster SWIPE animation event

	PRECACHE_SOUND("bullchicken/bc_attack2.wav");
	PRECACHE_SOUND("bullchicken/bc_attack3.wav");
	
	PRECACHE_SOUND("bullchicken/bc_die1.wav");
	PRECACHE_SOUND("bullchicken/bc_die2.wav");
	PRECACHE_SOUND("bullchicken/bc_die3.wav");
	
	PRECACHE_SOUND("bullchicken/bc_idle1.wav");
	PRECACHE_SOUND("bullchicken/bc_idle2.wav");
	PRECACHE_SOUND("bullchicken/bc_idle3.wav");
	PRECACHE_SOUND("bullchicken/bc_idle4.wav");
	PRECACHE_SOUND("bullchicken/bc_idle5.wav");
	
	PRECACHE_SOUND("bullchicken/bc_pain1.wav");
	PRECACHE_SOUND("bullchicken/bc_pain2.wav");
	PRECACHE_SOUND("bullchicken/bc_pain3.wav");
	PRECACHE_SOUND("bullchicken/bc_pain4.wav");
	
	PRECACHE_SOUND("bullchicken/bc_attackgrowl.wav");
	PRECACHE_SOUND("bullchicken/bc_attackgrowl2.wav");
	PRECACHE_SOUND("bullchicken/bc_attackgrowl3.wav");

	PRECACHE_SOUND("bullchicken/bc_acid1.wav");

	PRECACHE_SOUND("bullchicken/bc_bite2.wav");
	PRECACHE_SOUND("bullchicken/bc_bite3.wav");

	PRECACHE_SOUND("bullchicken/bc_spithit1.wav");
	PRECACHE_SOUND("bullchicken/bc_spithit2.wav");

}	

void CBullsquid :: DeathSound ( void )
{
	switch ( RANDOM_LONG(0,2) )
	{
	case 0:	
		EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_die1.wav", 1, ATTN_NORM );
		break;
	case 1:
		EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_die2.wav", 1, ATTN_NORM );
		break;
	case 2:
		EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_die3.wav", 1, ATTN_NORM );
		break;
	}
}

void CBullsquid :: AttackSound ( void )
{
	switch ( RANDOM_LONG(0,1) )
	{
	case 0:
		EMIT_SOUND( this, CHAN_WEAPON, "bullchicken/bc_attack2.wav", 1, ATTN_NORM );
		break;
	case 1:
		EMIT_SOUND( this, CHAN_WEAPON, "bullchicken/bc_attack3.wav", 1, ATTN_NORM );
		break;
	}
}


//========================================================
// RunAI - overridden for bullsquid because there are things
// that need to be checked every think.
//========================================================
void CBullsquid :: RunAI ( void )
{
	// first, do base class stuff
	CBaseMonster :: RunAI();

	if ( GetSkin() != 0 )
	{
		// close eye if it was open.
		SetSkin( 0 );
	}

	if ( RANDOM_LONG(0,39) == 0 )
	{
		SetSkin( 1 );
	}

	if ( m_hEnemy != NULL && m_Activity == ACT_RUN )
	{
		// chasing enemy. Sprint for last bit
		if ( (GetAbsOrigin() - m_hEnemy->GetAbsOrigin()).Length2D() < SQUID_SPRINT_DIST )
		{
			SetFrameRate( 1.25 );
		}
	}

}

//========================================================
// AI Schedules Specific to this monster
//=========================================================

// primary range attack
Task_t	tlSquidRangeAttack1[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE	},
};

Schedule_t	slSquidRangeAttack1[] =
{
	{ 
		tlSquidRangeAttack1,
		ARRAYSIZE ( tlSquidRangeAttack1 ), 
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_ENEMY_OCCLUDED	|
		bits_COND_NO_AMMO_LOADED,
		0,
		"Squid Range Attack1"
	},
};

// Chase enemy schedule
Task_t tlSquidChaseEnemy1[] = 
{
	{ TASK_SET_FAIL_SCHEDULE,	(float)SCHED_RANGE_ATTACK1	},// !!!OEM - this will stop nasty squid oscillation.
	{ TASK_GET_PATH_TO_ENEMY,	(float)0					},
	{ TASK_RUN_PATH,			(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,	(float)0					},
};

Schedule_t slSquidChaseEnemy[] =
{
	{ 
		tlSquidChaseEnemy1,
		ARRAYSIZE ( tlSquidChaseEnemy1 ),
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_SMELL_FOOD		|
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_MELEE_ATTACK1	|
		bits_COND_CAN_MELEE_ATTACK2	|
		bits_COND_TASK_FAILED		|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER			|
		bits_SOUND_MEAT,
		"Squid Chase Enemy"
	},
};

Task_t tlSquidHurtHop[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_SOUND_WAKE,			(float)0		},
	{ TASK_SQUID_HOPTURN,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},// in case squid didn't turn all the way in the air.
};

Schedule_t slSquidHurtHop[] =
{
	{
		tlSquidHurtHop,
		ARRAYSIZE ( tlSquidHurtHop ),
		0,
		0,
		"SquidHurtHop"
	}
};

Task_t tlSquidSeeCrab[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_SOUND_WAKE,			(float)0			},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_EXCITED	},
	{ TASK_FACE_ENEMY,			(float)0			},
};

Schedule_t slSquidSeeCrab[] =
{
	{
		tlSquidSeeCrab,
		ARRAYSIZE ( tlSquidSeeCrab ),
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE,
		0,
		"SquidSeeCrab"
	}
};

// squid walks to something tasty and eats it.
Task_t tlSquidEat[] =
{
	{ TASK_STOP_MOVING,				(float)0				},
	{ TASK_EAT,						(float)10				},// this is in case the squid can't get to the food
	{ TASK_STORE_LASTPOSITION,		(float)0				},
	{ TASK_GET_PATH_TO_BESTSCENT,	(float)0				},
	{ TASK_WALK_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT			},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT			},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT			},
	{ TASK_EAT,						(float)50				},
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0				},
	{ TASK_WALK_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_CLEAR_LASTPOSITION,		(float)0				},
};

Schedule_t slSquidEat[] =
{
	{
		tlSquidEat,
		ARRAYSIZE( tlSquidEat ),
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_NEW_ENEMY	,
		
		// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
		// here or the monster won't detect these sounds at ALL while running this schedule.
		bits_SOUND_MEAT			|
		bits_SOUND_CARCASS,
		"SquidEat"
	}
};

// this is a bit different than just Eat. We use this schedule when the food is far away, occluded, or behind
// the squid. This schedule plays a sniff animation before going to the source of food.
Task_t tlSquidSniffAndEat[] =
{
	{ TASK_STOP_MOVING,				(float)0				},
	{ TASK_EAT,						(float)10				},// this is in case the squid can't get to the food
	{ TASK_PLAY_SEQUENCE,			(float)ACT_DETECT_SCENT },
	{ TASK_STORE_LASTPOSITION,		(float)0				},
	{ TASK_GET_PATH_TO_BESTSCENT,	(float)0				},
	{ TASK_WALK_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT			},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT			},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT			},
	{ TASK_EAT,						(float)50				},
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0				},
	{ TASK_WALK_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_CLEAR_LASTPOSITION,		(float)0				},
};

Schedule_t slSquidSniffAndEat[] =
{
	{
		tlSquidSniffAndEat,
		ARRAYSIZE( tlSquidSniffAndEat ),
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_NEW_ENEMY	,
		
		// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
		// here or the monster won't detect these sounds at ALL while running this schedule.
		bits_SOUND_MEAT			|
		bits_SOUND_CARCASS,
		"SquidSniffAndEat"
	}
};

// squid does this to stinky things. 
Task_t tlSquidWallow[] =
{
	{ TASK_STOP_MOVING,				(float)0				},
	{ TASK_EAT,						(float)10				},// this is in case the squid can't get to the stinkiness
	{ TASK_STORE_LASTPOSITION,		(float)0				},
	{ TASK_GET_PATH_TO_BESTSCENT,	(float)0				},
	{ TASK_WALK_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_INSPECT_FLOOR},
	{ TASK_EAT,						(float)50				},// keeps squid from eating or sniffing anything else for a while.
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0				},
	{ TASK_WALK_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_CLEAR_LASTPOSITION,		(float)0				},
};

Schedule_t slSquidWallow[] =
{
	{
		tlSquidWallow,
		ARRAYSIZE( tlSquidWallow ),
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_NEW_ENEMY	,
		
		// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
		// here or the monster won't detect these sounds at ALL while running this schedule.
		bits_SOUND_GARBAGE,

		"SquidWallow"
	}
};

BEGIN_SCHEDULES( CBullsquid ) 
	slSquidRangeAttack1,
	slSquidChaseEnemy,
	slSquidHurtHop,
	slSquidSeeCrab,
	slSquidEat,
	slSquidSniffAndEat,
	slSquidWallow
END_SCHEDULES()

Schedule_t *CBullsquid :: GetSchedule( void )
{
	switch	( m_MonsterState )
	{
	case MONSTERSTATE_ALERT:
		{
			if ( HasConditions(bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE) )
			{
				return GetScheduleOfType ( SCHED_SQUID_HURTHOP );
			}

			if ( HasConditions(bits_COND_SMELL_FOOD) )
			{
				CSound		*pSound;

				pSound = PBestScent();
				
				if ( pSound && (!FInViewCone ( pSound->m_vecOrigin ) || !FVisible ( pSound->m_vecOrigin )) )
				{
					// scent is behind or occluded
					return GetScheduleOfType( SCHED_SQUID_SNIFF_AND_EAT );
				}

				// food is right out in the open. Just go get it.
				return GetScheduleOfType( SCHED_SQUID_EAT );
			}

			if ( HasConditions(bits_COND_SMELL) )
			{
				// there's something stinky. 
				CSound		*pSound;

				pSound = PBestScent();
				if ( pSound )
					return GetScheduleOfType( SCHED_SQUID_WALLOW);
			}

			break;
		}
	case MONSTERSTATE_COMBAT:
		{
// dead enemy
			if ( HasConditions( bits_COND_ENEMY_DEAD ) )
			{
				// call base class, all code to handle dead enemies is centralized there.
				return CBaseMonster :: GetSchedule();
			}

			if ( HasConditions(bits_COND_NEW_ENEMY) )
			{
				if ( m_fCanThreatDisplay && IRelationship( m_hEnemy ) == R_HT )
				{
					// this means squid sees a headcrab!
					m_fCanThreatDisplay = false;// only do the headcrab dance once per lifetime.
					return GetScheduleOfType ( SCHED_SQUID_SEECRAB );
				}
				else
				{
					return GetScheduleOfType ( SCHED_WAKE_ANGRY );
				}
			}

			if ( HasConditions(bits_COND_SMELL_FOOD) )
			{
				CSound		*pSound;

				pSound = PBestScent();
				
				if ( pSound && (!FInViewCone ( pSound->m_vecOrigin ) || !FVisible ( pSound->m_vecOrigin )) )
				{
					// scent is behind or occluded
					return GetScheduleOfType( SCHED_SQUID_SNIFF_AND_EAT );
				}

				// food is right out in the open. Just go get it.
				return GetScheduleOfType( SCHED_SQUID_EAT );
			}

			if ( HasConditions( bits_COND_CAN_RANGE_ATTACK1 ) )
			{
				return GetScheduleOfType ( SCHED_RANGE_ATTACK1 );
			}

			if ( HasConditions( bits_COND_CAN_MELEE_ATTACK1 ) )
			{
				return GetScheduleOfType ( SCHED_MELEE_ATTACK1 );
			}

			if ( HasConditions( bits_COND_CAN_MELEE_ATTACK2 ) )
			{
				return GetScheduleOfType ( SCHED_MELEE_ATTACK2 );
			}
			
			return GetScheduleOfType ( SCHED_CHASE_ENEMY );

			break;
		}

	default: break;
	}

	return CBaseMonster :: GetSchedule();
}

//=========================================================
// GetScheduleOfType
//=========================================================
Schedule_t* CBullsquid :: GetScheduleOfType ( int Type ) 
{
	switch	( Type )
	{
	case SCHED_RANGE_ATTACK1:
		return &slSquidRangeAttack1[ 0 ];
		break;
	case SCHED_SQUID_HURTHOP:
		return &slSquidHurtHop[ 0 ];
		break;
	case SCHED_SQUID_SEECRAB:
		return &slSquidSeeCrab[ 0 ];
		break;
	case SCHED_SQUID_EAT:
		return &slSquidEat[ 0 ];
		break;
	case SCHED_SQUID_SNIFF_AND_EAT:
		return &slSquidSniffAndEat[ 0 ];
		break;
	case SCHED_SQUID_WALLOW:
		return &slSquidWallow[ 0 ];
		break;
	case SCHED_CHASE_ENEMY:
		return &slSquidChaseEnemy[ 0 ];
		break;
	}

	return CBaseMonster :: GetScheduleOfType ( Type );
}

void CBullsquid :: StartTask ( const Task_t& task )
{
	m_iTaskStatus = TASKSTATUS_RUNNING;

	switch ( task.iTask )
	{
	case TASK_MELEE_ATTACK2:
		{
			switch ( RANDOM_LONG ( 0, 2 ) )
			{
			case 0:	
				EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_attackgrowl.wav", 1, ATTN_NORM );
				break;
			case 1:	
				EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_attackgrowl2.wav", 1, ATTN_NORM );
				break;
			case 2:	
				EMIT_SOUND( this, CHAN_VOICE, "bullchicken/bc_attackgrowl3.wav", 1, ATTN_NORM );
				break;
			}

			CBaseMonster :: StartTask ( task );
			break;
		}
	case TASK_SQUID_HOPTURN:
		{
			SetActivity ( ACT_HOP );
			MakeIdealYaw ( m_vecEnemyLKP );
			break;
		}
	case TASK_GET_PATH_TO_ENEMY:
		{
			if ( BuildRoute ( m_hEnemy->GetAbsOrigin(), bits_MF_TO_ENEMY, m_hEnemy ) )
			{
				m_iTaskStatus = TASKSTATUS_COMPLETE;
			}
			else
			{
				ALERT ( at_aiconsole, "GetPathToEnemy failed!!\n" );
				TaskFail();
			}
			break;
		}
	default:
		{
			CBaseMonster :: StartTask ( task );
			break;
		}
	}
}

void CBullsquid :: RunTask ( const Task_t& task )
{
	switch ( task.iTask )
	{
	case TASK_SQUID_HOPTURN:
		{
			MakeIdealYaw( m_vecEnemyLKP );
			ChangeYaw( GetYawSpeed() );

			if ( m_fSequenceFinished )
			{
				m_iTaskStatus = TASKSTATUS_COMPLETE;
			}
			break;
		}
	default:
		{
			CBaseMonster :: RunTask( task );
			break;
		}
	}
}


//=========================================================
// GetIdealState - Overridden for Bullsquid to deal with
// the feature that makes it lose interest in headcrabs for 
// a while if something injures it. 
//=========================================================
MONSTERSTATE CBullsquid :: GetIdealState ( void )
{
	int	iConditions;

	iConditions = IScheduleFlags();
	
	// If no schedule conditions, the new ideal state is probably the reason we're in here.
	switch ( m_MonsterState )
	{
	case MONSTERSTATE_COMBAT:
		/*
		COMBAT goes to ALERT upon death of enemy
		*/
		{
			if ( m_hEnemy != NULL && ( iConditions & bits_COND_LIGHT_DAMAGE || iConditions & bits_COND_HEAVY_DAMAGE ) && m_hEnemy->ClassnameIs( "monster_headcrab" ) )
			{
				// if the squid has a headcrab enemy and something hurts it, it's going to forget about the crab for a while.
				m_hEnemy = NULL;
				m_IdealMonsterState = MONSTERSTATE_ALERT;
			}
			break;
		}

	default: break;
	}

	m_IdealMonsterState = CBaseMonster :: GetIdealState();

	return m_IdealMonsterState;
}

