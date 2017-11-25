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
// Alien slave monster
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "CSquadMonster.h"
#include "Schedule.h"
#include "Effects.h"
#include "Weapons.h"
#include "entities/CSoundEnt.h"

#include "CISlave.h"

const char *CISlave::pAttackHitSounds[] =
{
	"zombie/claw_strike1.wav",
	"zombie/claw_strike2.wav",
	"zombie/claw_strike3.wav",
};

const char *CISlave::pAttackMissSounds[] =
{
	"zombie/claw_miss1.wav",
	"zombie/claw_miss2.wav",
};

const char *CISlave::pPainSounds[] =
{
	"aslave/slv_pain1.wav",
	"aslave/slv_pain2.wav",
};

const char *CISlave::pDeathSounds[] =
{
	"aslave/slv_die1.wav",
	"aslave/slv_die2.wav",
};

BEGIN_DATADESC(	CISlave )
	DEFINE_FIELD( m_iBravery, FIELD_INTEGER ),

	DEFINE_ARRAY( m_pBeam, FIELD_CLASSPTR, ISLAVE_MAX_BEAMS ),
	DEFINE_FIELD( m_iBeams, FIELD_INTEGER ),

	DEFINE_FIELD( m_voicePitch, FIELD_INTEGER ),

	DEFINE_FIELD( m_hDead, FIELD_EHANDLE ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_alien_slave, CISlave );
LINK_ENTITY_TO_CLASS( monster_vortigaunt, CISlave );

EntityClassification_t CISlave::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::ALIEN_MILITARY );
}


Relationship CISlave::IRelationship( CBaseEntity *pTarget )
{
	if ( (pTarget->IsPlayer()) )
		if ( GetSpawnFlags().Any( SF_MONSTER_WAIT_UNTIL_PROVOKED ) && ! (m_afMemory & bits_MEMORY_PROVOKED ))
			return R_NO;
	return CBaseMonster::IRelationship( pTarget );
}


void CISlave::CallForHelp( const char* const pszClassname, float flDist, EHANDLE hEnemy, Vector &vecLocation )
{
	// ALERT( at_aiconsole, "help " );

	//TODO: why does this take a classname? Shouldn't this be in CBaseMonster? - Solokiller

	// skip ones not on my netname
	if ( !HasNetName() )
		return;

	CBaseEntity *pEntity = NULL;

	while ((pEntity = UTIL_FindEntityByString( pEntity, "netname", GetNetName() )) != NULL)
	{
		float d = (GetAbsOrigin() - pEntity->GetAbsOrigin()).Length();
		if (d < flDist)
		{
			CBaseMonster *pMonster = pEntity->MyMonsterPointer( );
			if (pMonster)
			{
				pMonster->m_afMemory |= bits_MEMORY_PROVOKED;
				pMonster->PushEnemy( hEnemy, vecLocation );
			}
		}
	}
}

void CISlave :: AlertSound( void )
{
	if ( m_hEnemy != NULL )
	{
		g_Sentences.PlayRndSz( this, "SLV_ALERT", 0.85, ATTN_NORM, 0, m_voicePitch);

		CallForHelp( "monster_alien_slave", 512, m_hEnemy, m_vecEnemyLKP );
	}
}

//=========================================================
// IdleSound
//=========================================================
void CISlave :: IdleSound( void )
{
	if (RANDOM_LONG( 0, 2 ) == 0)
	{
		g_Sentences.PlayRndSz( this, "SLV_IDLE", 0.85, ATTN_NORM, 0, m_voicePitch);
	}

#if 0
	int side = RANDOM_LONG( 0, 1 ) * 2 - 1;

	ClearBeams( );
	ArmBeam( side );

	UTIL_MakeAimVectors( GetAbsAngles() );
	Vector vecSrc = GetAbsOrigin() + gpGlobals->v_right * 2 * side;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 8 );		// radius * 0.1
		WRITE_BYTE( 255 );		// r
		WRITE_BYTE( 180 );		// g
		WRITE_BYTE( 96 );		// b
		WRITE_BYTE( 10 );		// time * 10
		WRITE_BYTE( 0 );		// decay * 0.1
	MESSAGE_END( );

	EMIT_SOUND_DYN( this, CHAN_WEAPON, "debris/zap1.wav", 1, ATTN_NORM, 0, 100 );
#endif
}

void CISlave :: PainSound( void )
{
	if (RANDOM_LONG( 0, 2 ) == 0)
	{
		EMIT_SOUND_DYN ( this, CHAN_WEAPON, pPainSounds[ RANDOM_LONG(0,ARRAYSIZE(pPainSounds)-1) ], 1.0, ATTN_NORM, 0, m_voicePitch );
	}
}

void CISlave :: DeathSound( void )
{
	EMIT_SOUND_DYN ( this, CHAN_WEAPON, pDeathSounds[ RANDOM_LONG(0,ARRAYSIZE(pDeathSounds)-1) ], 1.0, ATTN_NORM, 0, m_voicePitch );
}

int CISlave :: ISoundMask ( void) 
{
	return	bits_SOUND_WORLD	|
			bits_SOUND_COMBAT	|
			bits_SOUND_DANGER	|
			bits_SOUND_PLAYER;
}


void CISlave::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	ClearBeams();
	CSquadMonster::Killed( info, gibAction );
}

void CISlave ::UpdateYawSpeed()
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_WALK:		
		ys = 50;	
		break;
	case ACT_RUN:		
		ys = 70;
		break;
	case ACT_IDLE:		
		ys = 50;
		break;
	default:
		ys = 90;
		break;
	}

	SetYawSpeed( ys );
}

void CISlave :: HandleAnimEvent( AnimEvent_t& event )
{
	// ALERT( at_console, "event %d : %f\n", pEvent->event, GetFrame() );
	switch( event.event )
	{
		case ISLAVE_AE_CLAW:
		{
			// SOUND HERE!
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, gSkillData.GetSlaveDmgClaw(), DMG_SLASH );
			if ( pHurt )
			{
				if( pHurt->GetFlags().Any( FL_MONSTER | FL_CLIENT ) )
				{
					Vector vecPunchAngle = pHurt->GetPunchAngle();
					vecPunchAngle.z = -18;
					vecPunchAngle.x = 5;
					pHurt->SetPunchAngle( vecPunchAngle );
				}
				// Play a random attack hit sound
				EMIT_SOUND_DYN ( this, CHAN_WEAPON, pAttackHitSounds[ RANDOM_LONG(0,ARRAYSIZE(pAttackHitSounds)-1) ], 1.0, ATTN_NORM, 0, m_voicePitch );
			}
			else
			{
				// Play a random attack miss sound
				EMIT_SOUND_DYN ( this, CHAN_WEAPON, pAttackMissSounds[ RANDOM_LONG(0,ARRAYSIZE(pAttackMissSounds)-1) ], 1.0, ATTN_NORM, 0, m_voicePitch );
			}
		}
		break;

		case ISLAVE_AE_CLAWRAKE:
		{
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, gSkillData.GetSlaveDmgClawrake(), DMG_SLASH );
			if ( pHurt )
			{
				if( pHurt->GetFlags().Any( FL_MONSTER | FL_CLIENT ) )
				{
					Vector vecPunchAngle = pHurt->GetPunchAngle();
					vecPunchAngle.z = -18;
					vecPunchAngle.x = 5;
					pHurt->SetPunchAngle( vecPunchAngle );
				}
				EMIT_SOUND_DYN ( this, CHAN_WEAPON, pAttackHitSounds[ RANDOM_LONG(0,ARRAYSIZE(pAttackHitSounds)-1) ], 1.0, ATTN_NORM, 0, m_voicePitch );
			}
			else
			{
				EMIT_SOUND_DYN ( this, CHAN_WEAPON, pAttackMissSounds[ RANDOM_LONG(0,ARRAYSIZE(pAttackMissSounds)-1) ], 1.0, ATTN_NORM, 0, m_voicePitch );
			}
		}
		break;

		case ISLAVE_AE_ZAP_POWERUP:
		{
			// speed up attack when on hard
			if (gSkillData.GetSkillLevel() == SKILL_HARD)
				SetFrameRate( 1.5 );

			UTIL_MakeAimVectors( GetAbsAngles() );

			if (m_iBeams == 0)
			{
				Vector vecSrc = GetAbsOrigin() + gpGlobals->v_forward * 2;
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
					WRITE_BYTE(TE_DLIGHT);
					WRITE_COORD(vecSrc.x);	// X
					WRITE_COORD(vecSrc.y);	// Y
					WRITE_COORD(vecSrc.z);	// Z
					WRITE_BYTE( 12 );		// radius * 0.1
					WRITE_BYTE( 255 );		// r
					WRITE_BYTE( 180 );		// g
					WRITE_BYTE( 96 );		// b
					WRITE_BYTE( 20 / GetFrameRate() );		// time * 10
					WRITE_BYTE( 0 );		// decay * 0.1
				MESSAGE_END( );

			}
			if (m_hDead != NULL)
			{
				WackBeam( -1, m_hDead );
				WackBeam( 1, m_hDead );
			}
			else
			{
				ArmBeam( -1 );
				ArmBeam( 1 );
				BeamGlow( );
			}

			EMIT_SOUND_DYN( this, CHAN_WEAPON, "debris/zap4.wav", 1, ATTN_NORM, 0, 100 + m_iBeams * 10 );
			SetSkin( m_iBeams / 2 );
		}
		break;

		case ISLAVE_AE_ZAP_SHOOT:
		{
			ClearBeams( );

			if (m_hDead != NULL)
			{
				Vector vecDest = m_hDead->GetAbsOrigin() + Vector( 0, 0, 38 );
				TraceResult trace;
				UTIL_TraceHull( vecDest, vecDest, dont_ignore_monsters, Hull::HUMAN, m_hDead->edict(), &trace );

				if ( !trace.fStartSolid )
				{
					CBaseEntity *pNew = Create( "monster_alien_slave", m_hDead->GetAbsOrigin(), m_hDead->GetAbsAngles() );
					pNew->GetSpawnFlags() |= SF_MONSTER_WAIT_TILL_SEEN;
					WackBeam( -1, pNew );
					WackBeam( 1, pNew );
					UTIL_Remove( m_hDead );
					EMIT_SOUND_DYN( this, CHAN_WEAPON, "hassault/hw_shoot1.wav", 1, ATTN_NORM, 0, RANDOM_LONG( 130, 160 ) );
					break;
				}
			}
			g_MultiDamage.Clear();

			UTIL_MakeAimVectors( GetAbsAngles() );

			ZapBeam( -1 );
			ZapBeam( 1 );

			EMIT_SOUND_DYN( this, CHAN_WEAPON, "hassault/hw_shoot1.wav", 1, ATTN_NORM, 0, RANDOM_LONG( 130, 160 ) );
			// STOP_SOUND( this, CHAN_WEAPON, "debris/zap4.wav" );
			g_MultiDamage.ApplyMultiDamage( this, this );

			m_flNextAttack = gpGlobals->time + RANDOM_FLOAT( 0.5, 4.0 );
		}
		break;

		case ISLAVE_AE_ZAP_DONE:
		{
			ClearBeams( );
		}
		break;

		default:
			CSquadMonster::HandleAnimEvent( event );
			break;
	}
}

bool CISlave :: CheckRangeAttack1 ( float flDot, float flDist )
{
	if (m_flNextAttack > gpGlobals->time)
	{
		return false;
	}

	return CSquadMonster::CheckRangeAttack1( flDot, flDist );
}

bool CISlave :: CheckRangeAttack2 ( float flDot, float flDist )
{
	return false;

	if (m_flNextAttack > gpGlobals->time)
	{
		return false;
	}

	m_hDead = NULL;
	m_iBravery = 0;

	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByClassname( pEntity, "monster_alien_slave" )) != NULL)
	{
		TraceResult tr;

		UTIL_TraceLine( EyePosition( ), pEntity->EyePosition( ), ignore_monsters, ENT(pev), &tr );
		if (tr.flFraction == 1.0 || tr.pHit == pEntity->edict())
		{
			if (pEntity->GetDeadFlag() == DEAD_DEAD)
			{
				float d = (GetAbsOrigin() - pEntity->GetAbsOrigin()).Length();
				if (d < flDist)
				{
					m_hDead = pEntity;
					flDist = d;
				}
				m_iBravery--;
			}
			else
			{
				m_iBravery++;
			}
		}
	}
	if (m_hDead != NULL)
		return true;
	else
		return false;
}

void CISlave :: StartTask ( const Task_t& task )
{
	ClearBeams( );

	CSquadMonster :: StartTask ( task );
}

void CISlave :: Spawn()
{
	Precache( );

	SetModel( "models/islave.mdl");
	SetSize( VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= BLOOD_COLOR_GREEN;
	GetEffects().ClearAll();
	SetHealth( gSkillData.GetSlaveHealth() );
	SetViewOffset( Vector ( 0, 0, 64 ) );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_MonsterState		= MONSTERSTATE_NONE;
	m_afCapability		= bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_RANGE_ATTACK2 | bits_CAP_DOORS_GROUP;

	m_voicePitch		= RANDOM_LONG( 85, 110 );

	MonsterInit();
}

void CISlave :: Precache()
{
	size_t i;

	PRECACHE_MODEL("models/islave.mdl");
	PRECACHE_MODEL("sprites/lgtning.spr");
	PRECACHE_SOUND("debris/zap1.wav");
	PRECACHE_SOUND("debris/zap4.wav");
	PRECACHE_SOUND("weapons/electro4.wav");
	PRECACHE_SOUND("hassault/hw_shoot1.wav");
	PRECACHE_SOUND("zombie/zo_pain2.wav");
	PRECACHE_SOUND("headcrab/hc_headbite.wav");
	PRECACHE_SOUND("weapons/cbar_miss1.wav");

	for ( i = 0; i < ARRAYSIZE( pAttackHitSounds ); i++ )
		PRECACHE_SOUND((char *)pAttackHitSounds[i]);

	for ( i = 0; i < ARRAYSIZE( pAttackMissSounds ); i++ )
		PRECACHE_SOUND((char *)pAttackMissSounds[i]);

	for ( i = 0; i < ARRAYSIZE( pPainSounds ); i++ )
		PRECACHE_SOUND((char *)pPainSounds[i]);

	for ( i = 0; i < ARRAYSIZE( pDeathSounds ); i++ )
		PRECACHE_SOUND((char *)pDeathSounds[i]);
}	

void CISlave::UpdateOnRemove()
{
	BaseClass::UpdateOnRemove();

	ClearBeams();
}


//=========================================================
// OnTakeDamage - get provoked when injured
//=========================================================

void CISlave::OnTakeDamage( const CTakeDamageInfo& info )
{
	// don't slash one of your own
	if (( info.GetDamageTypes() & DMG_SLASH) && info.GetAttacker() && IRelationship( info.GetAttacker() ) < R_DL)
		return;

	m_afMemory |= bits_MEMORY_PROVOKED;
	CSquadMonster::OnTakeDamage( info );
}


void CISlave::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	if (info.GetDamageTypes() & DMG_SHOCK)
		return;

	CSquadMonster::TraceAttack( info, vecDir, tr );
}


//=========================================================
// AI Schedules Specific to this monster
//=========================================================



// primary range attack
Task_t	tlSlaveAttack1[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
};

Schedule_t	slSlaveAttack1[] =
{
	{ 
		tlSlaveAttack1,
		ARRAYSIZE ( tlSlaveAttack1 ), 
		bits_COND_CAN_MELEE_ATTACK1 |
		bits_COND_HEAR_SOUND |
		bits_COND_HEAVY_DAMAGE, 

		bits_SOUND_DANGER,
		"Slave Range Attack1"
	},
};


BEGIN_SCHEDULES( CISlave )
	slSlaveAttack1,
END_SCHEDULES()


//=========================================================
//=========================================================
Schedule_t *CISlave :: GetSchedule( void )
{
	ClearBeams( );

/*
	if ( !GetSpawnFlags().None() )
	{
		GetSpawnFlags().ClearAll();
		return GetScheduleOfType( SCHED_RELOAD );
	}
*/

	if ( HasConditions( bits_COND_HEAR_SOUND ) )
	{
		CSound *pSound;
		pSound = PBestSound();

		ASSERT( pSound != NULL );

		if ( pSound && (pSound->m_iType & bits_SOUND_DANGER) )
			return GetScheduleOfType( SCHED_TAKE_COVER_FROM_BEST_SOUND );
		if ( pSound->m_iType & bits_SOUND_COMBAT )
			m_afMemory |= bits_MEMORY_PROVOKED;
	}

	switch (m_MonsterState)
	{
	case MONSTERSTATE_COMBAT:
// dead enemy
		if ( HasConditions( bits_COND_ENEMY_DEAD ) )
		{
			// call base class, all code to handle dead enemies is centralized there.
			return CBaseMonster :: GetSchedule();
		}

		if ( GetHealth() < 20 || m_iBravery < 0)
		{
			if (!HasConditions( bits_COND_CAN_MELEE_ATTACK1 ))
			{
				m_failSchedule = SCHED_CHASE_ENEMY;
				if (HasConditions( bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE))
				{
					return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
				}
				if ( HasConditions ( bits_COND_SEE_ENEMY ) && HasConditions ( bits_COND_ENEMY_FACING_ME ) )
				{
					// ALERT( at_console, "exposed\n");
					return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
				}
			}
		}
		break;

	default: break;
	}
	return CSquadMonster::GetSchedule( );
}


Schedule_t *CISlave :: GetScheduleOfType ( int Type ) 
{
	switch	( Type )
	{
	case SCHED_FAIL:
		if (HasConditions( bits_COND_CAN_MELEE_ATTACK1 ))
		{
			return CSquadMonster :: GetScheduleOfType( SCHED_MELEE_ATTACK1 ); ;
		}
		break;
	case SCHED_RANGE_ATTACK1:
		return slSlaveAttack1;
	case SCHED_RANGE_ATTACK2:
		return slSlaveAttack1;
	}
	return CSquadMonster :: GetScheduleOfType( Type );
}


//=========================================================
// ArmBeam - small beam from arm to nearby geometry
//=========================================================

void CISlave :: ArmBeam( int side )
{
	TraceResult tr;
	float flDist = 1.0;
	
	if (m_iBeams >= ISLAVE_MAX_BEAMS)
		return;

	UTIL_MakeAimVectors( GetAbsAngles() );
	Vector vecSrc = GetAbsOrigin() + gpGlobals->v_up * 36 + gpGlobals->v_right * side * 16 + gpGlobals->v_forward * 32;

	for (int i = 0; i < 3; i++)
	{
		Vector vecAim = gpGlobals->v_right * side * RANDOM_FLOAT( 0, 1 ) + gpGlobals->v_up * RANDOM_FLOAT( -1, 1 );
		TraceResult tr1;
		UTIL_TraceLine ( vecSrc, vecSrc + vecAim * 512, dont_ignore_monsters, ENT( pev ), &tr1);
		if (flDist > tr1.flFraction)
		{
			tr = tr1;
			flDist = tr.flFraction;
		}
	}

	// Couldn't find anything close enough
	if ( flDist == 1.0 )
		return;

	DecalGunshot( &tr, BULLET_PLAYER_CROWBAR );

	m_pBeam[m_iBeams] = CBeam::BeamCreate( "sprites/lgtning.spr", 30 );
	if (!m_pBeam[m_iBeams])
		return;

	m_pBeam[m_iBeams]->PointEntInit( tr.vecEndPos, entindex( ) );
	m_pBeam[m_iBeams]->SetEndAttachment( side < 0 ? 2 : 1 );
	// m_pBeam[m_iBeams]->SetColor( 180, 255, 96 );
	m_pBeam[m_iBeams]->SetColor( 96, 128, 16 );
	m_pBeam[m_iBeams]->SetBrightness( 64 );
	m_pBeam[m_iBeams]->SetNoise( 80 );
	m_iBeams++;
}


//=========================================================
// BeamGlow - brighten all beams
//=========================================================
void CISlave :: BeamGlow( )
{
	int b = m_iBeams * 32;
	if (b > 255)
		b = 255;

	for (int i = 0; i < m_iBeams; i++)
	{
		if (m_pBeam[i]->GetBrightness() != 255) 
		{
			m_pBeam[i]->SetBrightness( b );
		}
	}
}


//=========================================================
// WackBeam - regenerate dead colleagues
//=========================================================
void CISlave :: WackBeam( int side, CBaseEntity *pEntity )
{
	Vector vecDest;
	
	if (m_iBeams >= ISLAVE_MAX_BEAMS)
		return;

	if (pEntity == NULL)
		return;

	m_pBeam[m_iBeams] = CBeam::BeamCreate( "sprites/lgtning.spr", 30 );
	if (!m_pBeam[m_iBeams])
		return;

	m_pBeam[m_iBeams]->PointEntInit( pEntity->Center(), entindex( ) );
	m_pBeam[m_iBeams]->SetEndAttachment( side < 0 ? 2 : 1 );
	m_pBeam[m_iBeams]->SetColor( 180, 255, 96 );
	m_pBeam[m_iBeams]->SetBrightness( 255 );
	m_pBeam[m_iBeams]->SetNoise( 80 );
	m_iBeams++;
}

//=========================================================
// ZapBeam - heavy damage directly forward
//=========================================================
void CISlave :: ZapBeam( int side )
{
	Vector vecSrc, vecAim;
	TraceResult tr;
	CBaseEntity *pEntity;

	if (m_iBeams >= ISLAVE_MAX_BEAMS)
		return;

	vecSrc = GetAbsOrigin() + gpGlobals->v_up * 36;
	vecAim = ShootAtEnemy( vecSrc );
	float deflection = 0.01;
	vecAim = vecAim + side * gpGlobals->v_right * RANDOM_FLOAT( 0, deflection ) + gpGlobals->v_up * RANDOM_FLOAT( -deflection, deflection );
	UTIL_TraceLine ( vecSrc, vecSrc + vecAim * 1024, dont_ignore_monsters, ENT( pev ), &tr);

	m_pBeam[m_iBeams] = CBeam::BeamCreate( "sprites/lgtning.spr", 50 );
	if (!m_pBeam[m_iBeams])
		return;

	m_pBeam[m_iBeams]->PointEntInit( tr.vecEndPos, entindex( ) );
	m_pBeam[m_iBeams]->SetEndAttachment( side < 0 ? 2 : 1 );
	m_pBeam[m_iBeams]->SetColor( 180, 255, 96 );
	m_pBeam[m_iBeams]->SetBrightness( 255 );
	m_pBeam[m_iBeams]->SetNoise( 20 );
	m_iBeams++;

	pEntity = CBaseEntity::Instance(tr.pHit);
	if (pEntity != NULL && pEntity->GetTakeDamageMode() != DAMAGE_NO )
	{
		pEntity->TraceAttack( CTakeDamageInfo( this, gSkillData.GetSlaveDmgZap(), DMG_SHOCK ), vecAim, tr );
	}
	UTIL_EmitAmbientSound( this, tr.vecEndPos, "weapons/electro4.wav", 0.5, ATTN_NORM, 0, RANDOM_LONG( 140, 160 ) );
}


//=========================================================
// ClearBeams - remove all beams
//=========================================================
void CISlave :: ClearBeams( )
{
	for (int i = 0; i < ISLAVE_MAX_BEAMS; i++)
	{
		if (m_pBeam[i])
		{
			UTIL_Remove( m_pBeam[i] );
			m_pBeam[i] = NULL;
		}
	}
	m_iBeams = 0;
	SetSkin( 0 );

	STOP_SOUND( this, CHAN_WEAPON, "debris/zap4.wav" );
}
