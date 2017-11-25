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
// hgrunt
//=========================================================
#include "extdll.h"
#include "CPlane.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"
#include "animation.h"
#include "CSquadMonster.h"
#include "Weapons.h"
#include "CTalkMonster.h"
#include "entities/CSoundEnt.h"

#include "CHGrunt.h"

int g_fGruntQuestion;				// true if an idle grunt asked a question. Cleared when someone answers.

BEGIN_DATADESC(	CHGrunt )
	DEFINE_FIELD( m_flNextGrenadeCheck, FIELD_TIME ),
	DEFINE_FIELD( m_flNextPainTime, FIELD_TIME ),
//	DEFINE_FIELD( m_flLastEnemySightTime, FIELD_TIME ), // don't save, go to zero
	DEFINE_FIELD( m_vecTossVelocity, FIELD_VECTOR ),
	DEFINE_FIELD( m_fThrowGrenade, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fStanding, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fFirstEncounter, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_cClipSize, FIELD_INTEGER ),
	DEFINE_FIELD( m_voicePitch, FIELD_INTEGER ),
//  DEFINE_FIELD( m_iBrassShell, FIELD_INTEGER ),
//  DEFINE_FIELD( m_iShotgunShell, FIELD_INTEGER ),
	DEFINE_FIELD( m_iSentence, FIELD_INTEGER ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_human_grunt, CHGrunt );

const char *CHGrunt::pGruntSentences[] = 
{
	"HG_GREN", // grenade scared grunt
	"HG_ALERT", // sees player
	"HG_MONSTER", // sees monster
	"HG_COVER", // running to cover
	"HG_THROW", // about to throw grenade
	"HG_CHARGE",  // running out to get the enemy
	"HG_TAUNT", // say rude things
};

//=========================================================
// Speak Sentence - say your cued up sentence.
//
// Some grunt sentences (take cover and charge) rely on actually
// being able to execute the intended action. It's really lame
// when a grunt says 'COVER ME' and then doesn't move. The problem
// is that the sentences were played when the decision to TRY
// to move to cover was made. Now the sentence is played after 
// we know for sure that there is a valid path. The schedule
// may still fail but in most cases, well after the grunt has 
// started moving.
//=========================================================
void CHGrunt :: SpeakSentence( void )
{
	if ( m_iSentence == HGRUNT_SENT_NONE )
	{
		// no sentence cued up.
		return; 
	}

	if (FOkToSpeak())
	{
		g_Sentences.PlayRndSz( this, pGruntSentences[ m_iSentence ], HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
		JustSpoke();
	}
}

Relationship CHGrunt::IRelationship ( CBaseEntity *pTarget )
{
	if ( pTarget->ClassnameIs( "monster_alien_grunt" ) || ( pTarget->ClassnameIs( "monster_gargantua" ) ) )
	{
		return R_NM;
	}

	return CSquadMonster::IRelationship( pTarget );
}

//=========================================================
// GibMonster - make gun fly through the air.
//=========================================================
void CHGrunt :: GibMonster ( void )
{
	Vector	vecGunPos;
	Vector	vecGunAngles;

	if ( GetBodygroup( 2 ) != 2 )
	{// throw a gun if the grunt has one
		GetAttachment( 0, vecGunPos, vecGunAngles );
		
		CBaseEntity *pGun;
		if ( GetWeapons().Any( HGRUNT_SHOTGUN ) )
		{
			pGun = DropItem( "weapon_shotgun", vecGunPos, vecGunAngles );
		}
		else
		{
			pGun = DropItem( "weapon_9mmAR", vecGunPos, vecGunAngles );
		}
		if ( pGun )
		{
			pGun->SetAbsVelocity( Vector (RANDOM_FLOAT(-100,100), RANDOM_FLOAT(-100,100), RANDOM_FLOAT(200,300)) );
			pGun->SetAngularVelocity( Vector ( 0, RANDOM_FLOAT( 200, 400 ), 0 ) );
		}
	
		if( GetWeapons().Any( HGRUNT_GRENADELAUNCHER ) )
		{
			pGun = DropItem( "ammo_ARgrenades", vecGunPos, vecGunAngles );
			if ( pGun )
			{
				pGun->SetAbsVelocity( Vector (RANDOM_FLOAT(-100,100), RANDOM_FLOAT(-100,100), RANDOM_FLOAT(200,300)) );
				pGun->SetAngularVelocity( Vector ( 0, RANDOM_FLOAT( 200, 400 ), 0 ) );
			}
		}
	}

	CBaseMonster :: GibMonster();
}

int CHGrunt :: ISoundMask ( void )
{
	return	bits_SOUND_WORLD	|
			bits_SOUND_COMBAT	|
			bits_SOUND_PLAYER	|
			bits_SOUND_DANGER;
}

//=========================================================
// someone else is talking - don't speak
//=========================================================
bool CHGrunt::FOkToSpeak() const
{
// if someone else is talking, don't speak
	if (gpGlobals->time <= CTalkMonster::g_talkWaitTime)
		return false;

	if ( GetSpawnFlags().Any( SF_MONSTER_GAG ) )
	{
		if ( m_MonsterState != MONSTERSTATE_COMBAT )
		{
			// no talking outside of combat if gagged.
			return false;
		}
	}

	// if player is not in pvs, don't speak
//	if( !UTIL_FindClientInPVS( this ) )
//		return false;
	
	return true;
}

//=========================================================
//=========================================================
void CHGrunt :: JustSpoke( void )
{
	CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT(1.5, 2.0);
	m_iSentence = HGRUNT_SENT_NONE;
}

void CHGrunt :: PrescheduleThink ( void )
{
	if ( InSquad() && m_hEnemy != NULL )
	{
		if ( HasConditions ( bits_COND_SEE_ENEMY ) )
		{
			// update the squad's last enemy sighting time.
			MySquadLeader()->m_flLastEnemySightTime = gpGlobals->time;
		}
		else
		{
			if ( gpGlobals->time - MySquadLeader()->m_flLastEnemySightTime > 5 )
			{
				// been a while since we've seen the enemy
				MySquadLeader()->m_fEnemyEluded = true;
			}
		}
	}
}

bool CHGrunt::FCanCheckAttacks() const
{
	if ( !HasConditions( bits_COND_ENEMY_TOOFAR ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CHGrunt :: CheckMeleeAttack1 ( float flDot, float flDist )
{
	if( !m_hEnemy )
		return false;

	CBaseMonster* pEnemy = m_hEnemy->MyMonsterPointer();

	if ( !pEnemy )
	{
		return false;
	}

	if ( flDist <= 64 && flDot >= 0.7	&& 
		 pEnemy->Classify() != EntityClassifications().GetClassificationId( classify::ALIEN_BIOWEAPON ) &&
		 pEnemy->Classify() != EntityClassifications().GetClassificationId( classify::PLAYER_BIOWEAPON ) )
	{
		return true;
	}
	return false;
}

bool CHGrunt :: CheckRangeAttack1 ( float flDot, float flDist )
{
	if ( !HasConditions( bits_COND_ENEMY_OCCLUDED ) && flDist <= 2048 && flDot >= 0.5 && NoFriendlyFire() )
	{
		TraceResult	tr;

		if ( !m_hEnemy->IsPlayer() && flDist <= 64 )
		{
			// kick nonclients, but don't shoot at them.
			return false;
		}

		Vector vecSrc = GetGunPosition();

		// verify that a bullet fired from the gun will hit the enemy before the world.
		UTIL_TraceLine( vecSrc, m_hEnemy->BodyTarget(vecSrc), ignore_monsters, ignore_glass, ENT(pev), &tr);

		if ( tr.flFraction == 1.0 )
		{
			return true;
		}
	}

	return false;
}

bool CHGrunt :: CheckRangeAttack2 ( float flDot, float flDist )
{
	if( !GetWeapons().Any( HGRUNT_HANDGRENADE | HGRUNT_GRENADELAUNCHER ) )
	{
		return false;
	}
	
	// if the grunt isn't moving, it's ok to check.
	if ( m_flGroundSpeed != 0 )
	{
		m_fThrowGrenade = false;
		return m_fThrowGrenade;
	}

	// assume things haven't changed too much since last time
	if (gpGlobals->time < m_flNextGrenadeCheck )
	{
		return m_fThrowGrenade;
	}

	if ( !m_hEnemy->GetFlags().Any( FL_ONGROUND ) && m_hEnemy->GetWaterLevel() == WATERLEVEL_DRY && m_vecEnemyLKP.z > GetAbsMax().z  )
	{
		//!!!BUGBUG - we should make this check movetype and make sure it isn't FLY? Players who jump a lot are unlikely to 
		// be grenaded.
		// don't throw grenades at anything that isn't on the ground!
		m_fThrowGrenade = false;
		return m_fThrowGrenade;
	}
	
	Vector vecTarget;

	if( GetWeapons().Any( HGRUNT_HANDGRENADE ) )
	{
		// find feet
		if (RANDOM_LONG(0,1))
		{
			// magically know where they are
			vecTarget = Vector( m_hEnemy->GetAbsOrigin().x, m_hEnemy->GetAbsOrigin().y, m_hEnemy->GetAbsMin().z );
		}
		else
		{
			// toss it to where you last saw them
			vecTarget = m_vecEnemyLKP;
		}
		// vecTarget = m_vecEnemyLKP + (m_hEnemy->BodyTarget( GetAbsOrigin() ) - m_hEnemy->GetAbsOrigin());
		// estimate position
		// vecTarget = vecTarget + m_hEnemy->GetAbsVelocity() * 2;
	}
	else
	{
		// find target
		// vecTarget = m_hEnemy->BodyTarget( GetAbsOrigin() );
		vecTarget = m_vecEnemyLKP + (m_hEnemy->BodyTarget( GetAbsOrigin() ) - m_hEnemy->GetAbsOrigin());
		// estimate position
		if (HasConditions( bits_COND_SEE_ENEMY))
			vecTarget = vecTarget + ((vecTarget - GetAbsOrigin()).Length() / gSkillData.GetHGruntGrenadeSpeed() ) * m_hEnemy->GetAbsVelocity();
	}

	// are any of my squad members near the intended grenade impact area?
	if ( InSquad() )
	{
		if (SquadMemberInRange( vecTarget, 256 ))
		{
			// crap, I might blow my own guy up. Don't throw a grenade and don't check again for a while.
			m_flNextGrenadeCheck = gpGlobals->time + 1; // one full second.
			m_fThrowGrenade = false;
		}
	}
	
	if ( ( vecTarget - GetAbsOrigin() ).Length2D() <= 256 )
	{
		// crap, I don't want to blow myself up
		m_flNextGrenadeCheck = gpGlobals->time + 1; // one full second.
		m_fThrowGrenade = false;
		return m_fThrowGrenade;
	}

		
	if( GetWeapons().Any( HGRUNT_HANDGRENADE ) )
	{
		Vector vecToss = VecCheckToss( this, GetGunPosition(), vecTarget, 0.5 );

		if ( vecToss != g_vecZero )
		{
			m_vecTossVelocity = vecToss;

			// throw a hand grenade
			m_fThrowGrenade = true;
			// don't check again for a while.
			m_flNextGrenadeCheck = gpGlobals->time; // 1/3 second.
		}
		else
		{
			// don't throw
			m_fThrowGrenade = false;
			// don't check again for a while.
			m_flNextGrenadeCheck = gpGlobals->time + 1; // one full second.
		}
	}
	else
	{
		Vector vecToss = VecCheckThrow( this, GetGunPosition(), vecTarget, gSkillData.GetHGruntGrenadeSpeed(), 0.5 );

		if ( vecToss != g_vecZero )
		{
			m_vecTossVelocity = vecToss;

			// throw a hand grenade
			m_fThrowGrenade = true;
			// don't check again for a while.
			m_flNextGrenadeCheck = gpGlobals->time + 0.3; // 1/3 second.
		}
		else
		{
			// don't throw
			m_fThrowGrenade = false;
			// don't check again for a while.
			m_flNextGrenadeCheck = gpGlobals->time + 1; // one full second.
		}
	}

	

	return m_fThrowGrenade;
}

void CHGrunt::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	CTakeDamageInfo newInfo = info;

	// check for helmet shot
	if ( tr.iHitgroup == 11)
	{
		// make sure we're wearing one
		if (GetBodygroup( 1 ) == HEAD_GRUNT && (newInfo.GetDamageTypes() & (DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_CLUB)))
		{
			// absorb damage
			newInfo.GetMutableDamage() -= 20;
			if (newInfo.GetDamage() <= 0)
			{
				UTIL_Ricochet( tr.vecEndPos, 1.0 );
				newInfo.GetMutableDamage() = 0.01;
			}
		}
		// it's head shot anyways
		tr.iHitgroup = HITGROUP_HEAD;
	}
	CSquadMonster::TraceAttack( newInfo, vecDir, tr );
}


//=========================================================
// OnTakeDamage - overridden for the grunt because the grunt
// needs to forget that he is in cover if he's hurt. (Obviously
// not in a safe place anymore).
//=========================================================
void CHGrunt::OnTakeDamage( const CTakeDamageInfo& info )
{
	Forget( bits_MEMORY_INCOVER );

	CSquadMonster::OnTakeDamage( info );
}

void CHGrunt::UpdateYawSpeed()
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:	
		ys = 150;		
		break;
	case ACT_RUN:	
		ys = 150;	
		break;
	case ACT_WALK:	
		ys = 180;		
		break;
	case ACT_RANGE_ATTACK1:	
		ys = 120;	
		break;
	case ACT_RANGE_ATTACK2:	
		ys = 120;	
		break;
	case ACT_MELEE_ATTACK1:	
		ys = 120;	
		break;
	case ACT_MELEE_ATTACK2:	
		ys = 120;	
		break;
	case ACT_TURN_LEFT:
	case ACT_TURN_RIGHT:	
		ys = 180;
		break;
	case ACT_GLIDE:
	case ACT_FLY:
		ys = 30;
		break;
	default:
		ys = 90;
		break;
	}

	SetYawSpeed( ys );
}

void CHGrunt :: IdleSound( void )
{
	if (FOkToSpeak() && (g_fGruntQuestion || RANDOM_LONG(0,1)))
	{
		if (!g_fGruntQuestion)
		{
			// ask question or make statement
			switch (RANDOM_LONG(0,2))
			{
			case 0: // check in
				g_Sentences.PlayRndSz( this, "HG_CHECK", HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
				g_fGruntQuestion = 1;
				break;
			case 1: // question
				g_Sentences.PlayRndSz( this, "HG_QUEST", HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
				g_fGruntQuestion = 2;
				break;
			case 2: // statement
				g_Sentences.PlayRndSz( this, "HG_IDLE", HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
				break;
			}
		}
		else
		{
			switch (g_fGruntQuestion)
			{
			case 1: // check in
				g_Sentences.PlayRndSz( this, "HG_CLEAR", HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
				break;
			case 2: // question 
				g_Sentences.PlayRndSz( this, "HG_ANSWER", HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
				break;
			}
			g_fGruntQuestion = 0;
		}
		JustSpoke();
	}
}

//=========================================================
// CheckAmmo - overridden for the grunt because he actually
// uses ammo! (base class doesn't)
//=========================================================
void CHGrunt :: CheckAmmo ( void )
{
	if ( m_cAmmoLoaded <= 0 )
	{
		SetConditions(bits_COND_NO_AMMO_LOADED);
	}
}

EntityClassification_t CHGrunt::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::HUMAN_MILITARY );
}

//=========================================================
//=========================================================
CBaseEntity *CHGrunt :: Kick( void )
{
	TraceResult tr;

	UTIL_MakeVectors( GetAbsAngles() );
	Vector vecStart = GetAbsOrigin();
	vecStart.z += GetBounds().z * 0.5;
	Vector vecEnd = vecStart + (gpGlobals->v_forward * 70);

	UTIL_TraceHull( vecStart, vecEnd, dont_ignore_monsters, Hull::HEAD, ENT(pev), &tr );
	
	if ( tr.pHit )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		return pEntity;
	}

	return NULL;
}

//=========================================================
// GetGunPosition	return the end of the barrel
//=========================================================

Vector CHGrunt :: GetGunPosition( )
{
	if (m_fStanding )
	{
		return GetAbsOrigin() + Vector( 0, 0, 60 );
	}
	else
	{
		return GetAbsOrigin() + Vector( 0, 0, 48 );
	}
}

//=========================================================
// Shoot
//=========================================================
void CHGrunt :: Shoot ( void )
{
	if (m_hEnemy == NULL)
	{
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

	UTIL_MakeVectors ( GetAbsAngles() );

	Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40,90) + gpGlobals->v_up * RANDOM_FLOAT(75,200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
	EjectBrass ( vecShootOrigin - vecShootDir * 24, vecShellVelocity, GetAbsAngles().y, m_iBrassShell, TE_BOUNCE_SHELL);
	FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_10DEGREES, 2048, BULLET_MONSTER_MP5 ); // shoot +-5 degrees

	GetEffects() |= EF_MUZZLEFLASH;
	
	m_cAmmoLoaded--;// take away a bullet!

	Vector angDir = UTIL_VecToAngles( vecShootDir );
	SetBlending( 0, angDir.x );
}

//=========================================================
// Shoot
//=========================================================
void CHGrunt :: Shotgun ( void )
{
	if (m_hEnemy == NULL)
	{
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

	UTIL_MakeVectors ( GetAbsAngles() );

	Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40,90) + gpGlobals->v_up * RANDOM_FLOAT(75,200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
	EjectBrass ( vecShootOrigin - vecShootDir * 24, vecShellVelocity, GetAbsAngles().y, m_iShotgunShell, TE_BOUNCE_SHOTSHELL);
	FireBullets(gSkillData.GetHGruntShotgunPellets(), vecShootOrigin, vecShootDir, VECTOR_CONE_15DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0 ); // shoot +-7.5 degrees

	GetEffects() |= EF_MUZZLEFLASH;
	
	m_cAmmoLoaded--;// take away a bullet!

	Vector angDir = UTIL_VecToAngles( vecShootDir );
	SetBlending( 0, angDir.x );
}

void CHGrunt :: HandleAnimEvent( AnimEvent_t& event )
{
	Vector	vecShootDir;
	Vector	vecShootOrigin;

	switch( event.event )
	{
		case HGRUNT_AE_DROP_GUN:
			{
			Vector	vecGunPos;
			Vector	vecGunAngles;

			GetAttachment( 0, vecGunPos, vecGunAngles );

			// switch to body group with no gun.
			SetBodygroup( GUN_GROUP, GUN_NONE );

			// now spawn a gun.
			if( GetWeapons().Any( HGRUNT_SHOTGUN ) )
			{
				 DropItem( "weapon_shotgun", vecGunPos, vecGunAngles );
			}
			else
			{
				 DropItem( "weapon_9mmAR", vecGunPos, vecGunAngles );
			}
			if( GetWeapons().Any( HGRUNT_GRENADELAUNCHER ) )
			{
				DropItem( "ammo_ARgrenades", BodyTarget( GetAbsOrigin() ), vecGunAngles );
			}

			}
			break;

		case HGRUNT_AE_RELOAD:
			EMIT_SOUND( this, CHAN_WEAPON, "hgrunt/gr_reload1.wav", 1, ATTN_NORM );
			m_cAmmoLoaded = m_cClipSize;
			ClearConditions(bits_COND_NO_AMMO_LOADED);
			break;

		case HGRUNT_AE_GREN_TOSS:
		{
			UTIL_MakeVectors( GetAbsAngles() );
			// CGrenade::ShootTimed( this, GetAbsOrigin() + gpGlobals->v_forward * 34 + Vector (0, 0, 32), m_vecTossVelocity, 3.5 );
			CGrenade::ShootTimed( this, GetGunPosition(), m_vecTossVelocity, 3.5 );

			m_fThrowGrenade = false;
			m_flNextGrenadeCheck = gpGlobals->time + 6;// wait six seconds before even looking again to see if a grenade can be thrown.
			// !!!LATER - when in a group, only try to throw grenade if ordered.
		}
		break;

		case HGRUNT_AE_GREN_LAUNCH:
		{
			EMIT_SOUND( this, CHAN_WEAPON, "weapons/glauncher.wav", 0.8, ATTN_NORM);
			CGrenade::ShootContact( this, GetGunPosition(), m_vecTossVelocity );
			m_fThrowGrenade = false;
			if (gSkillData.GetSkillLevel() == SKILL_HARD)
				m_flNextGrenadeCheck = gpGlobals->time + RANDOM_FLOAT( 2, 5 );// wait a random amount of time before shooting again
			else
				m_flNextGrenadeCheck = gpGlobals->time + 6;// wait six seconds before even looking again to see if a grenade can be thrown.
		}
		break;

		case HGRUNT_AE_GREN_DROP:
		{
			UTIL_MakeVectors( GetAbsAngles() );
			CGrenade::ShootTimed( this, GetAbsOrigin() + gpGlobals->v_forward * 17 - gpGlobals->v_right * 27 + gpGlobals->v_up * 6, g_vecZero, 3 );
		}
		break;

		case HGRUNT_AE_BURST1:
		{
			if( GetWeapons().Any( HGRUNT_9MMAR ) )
			{
				Shoot();

				// the first round of the three round burst plays the sound and puts a sound in the world sound list.
				if ( RANDOM_LONG(0,1) )
				{
					EMIT_SOUND( this, CHAN_WEAPON, "hgrunt/gr_mgun1.wav", 1, ATTN_NORM );
				}
				else
				{
					EMIT_SOUND( this, CHAN_WEAPON, "hgrunt/gr_mgun2.wav", 1, ATTN_NORM );
				}
			}
			else
			{
				Shotgun( );

				EMIT_SOUND( this, CHAN_WEAPON, "weapons/sbarrel1.wav", 1, ATTN_NORM );
			}
		
			CSoundEnt::InsertSound ( bits_SOUND_COMBAT, GetAbsOrigin(), 384, 0.3 );
		}
		break;

		case HGRUNT_AE_BURST2:
		case HGRUNT_AE_BURST3:
			Shoot();
			break;

		case HGRUNT_AE_KICK:
		{
			CBaseEntity *pHurt = Kick();

			if ( pHurt )
			{
				// SOUND HERE!
				UTIL_MakeVectors( GetAbsAngles() );

				Vector vecPunchAngle = pHurt->GetPunchAngle();
				vecPunchAngle.x = 15;
				pHurt->SetPunchAngle( vecPunchAngle );
				pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() + gpGlobals->v_forward * 100 + gpGlobals->v_up * 50 );
				pHurt->TakeDamage( this, this, gSkillData.GetHGruntDmgKick(), DMG_CLUB );
			}
		}
		break;

		case HGRUNT_AE_CAUGHT_ENEMY:
		{
			if ( FOkToSpeak() )
			{
				g_Sentences.PlayRndSz( this, "HG_ALERT", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
				 JustSpoke();
			}

		}

		default:
			CSquadMonster::HandleAnimEvent( event );
			break;
	}
}

void CHGrunt :: Spawn()
{
	Precache( );

	SetModel( "models/hgrunt.mdl");
	SetSize( VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= BLOOD_COLOR_RED;
	GetEffects().ClearAll();
	SetHealth( gSkillData.GetHGruntHealth() );
	m_flFieldOfView		= 0.2;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;
	m_flNextGrenadeCheck = gpGlobals->time + 1;
	m_flNextPainTime	= gpGlobals->time;
	m_iSentence			= HGRUNT_SENT_NONE;

	m_afCapability		= bits_CAP_SQUAD | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;

	m_fEnemyEluded		= false;
	m_fFirstEncounter	= true;// this is true when the grunt spawns, because he hasn't encountered an enemy yet.

	m_HackedGunPos = Vector ( 0, 0, 55 );

	if( GetWeapons().None() )
	{
		// initialize to original values
		GetWeapons().Set( HGRUNT_9MMAR | HGRUNT_HANDGRENADE );
		// GetWeapons().Set( HGRUNT_SHOTGUN );
		// GetWeapons().Set( HGRUNT_9MMAR | HGRUNT_GRENADELAUNCHER );
	}

	if( GetWeapons().Any( HGRUNT_SHOTGUN ) )
	{
		SetBodygroup( GUN_GROUP, GUN_SHOTGUN );
		m_cClipSize		= 8;
	}
	else
	{
		m_cClipSize		= GRUNT_CLIP_SIZE;
	}
	m_cAmmoLoaded		= m_cClipSize;

	if (RANDOM_LONG( 0, 99 ) < 80)
		SetSkin( 0 );	// light skin
	else
		SetSkin( 1 );	// dark skin

	if( GetWeapons().Any( HGRUNT_SHOTGUN ) )
	{
		SetBodygroup( HEAD_GROUP, HEAD_SHOTGUN);
	}
	else if ( GetWeapons().Any( HGRUNT_GRENADELAUNCHER ) )
	{
		SetBodygroup( HEAD_GROUP, HEAD_M203 );
		SetSkin( 1 ); // alway dark skin
	}

	CTalkMonster::g_talkWaitTime = 0;

	MonsterInit();
}

void CHGrunt :: Precache()
{
	PRECACHE_MODEL("models/hgrunt.mdl");

	PRECACHE_SOUND( "hgrunt/gr_mgun1.wav" );
	PRECACHE_SOUND( "hgrunt/gr_mgun2.wav" );
	
	PRECACHE_SOUND( "hgrunt/gr_die1.wav" );
	PRECACHE_SOUND( "hgrunt/gr_die2.wav" );
	PRECACHE_SOUND( "hgrunt/gr_die3.wav" );

	PRECACHE_SOUND( "hgrunt/gr_pain1.wav" );
	PRECACHE_SOUND( "hgrunt/gr_pain2.wav" );
	PRECACHE_SOUND( "hgrunt/gr_pain3.wav" );
	PRECACHE_SOUND( "hgrunt/gr_pain4.wav" );
	PRECACHE_SOUND( "hgrunt/gr_pain5.wav" );

	PRECACHE_SOUND( "hgrunt/gr_reload1.wav" );

	PRECACHE_SOUND( "weapons/glauncher.wav" );

	PRECACHE_SOUND( "weapons/sbarrel1.wav" );

	PRECACHE_SOUND("zombie/claw_miss2.wav");// because we use the basemonster SWIPE animation event

	// get voice pitch
	if (RANDOM_LONG(0,1))
		m_voicePitch = 109 + RANDOM_LONG(0,7);
	else
		m_voicePitch = 100;

	m_iBrassShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell
	m_iShotgunShell = PRECACHE_MODEL ("models/shotgunshell.mdl");
}	

void CHGrunt :: StartTask ( const Task_t& task )
{
	m_iTaskStatus = TASKSTATUS_RUNNING;

	switch ( task.iTask )
	{
	case TASK_GRUNT_CHECK_FIRE:
		if ( !NoFriendlyFire() )
		{
			SetConditions( bits_COND_GRUNT_NOFIRE );
		}
		TaskComplete();
		break;

	case TASK_GRUNT_SPEAK_SENTENCE:
		SpeakSentence();
		TaskComplete();
		break;
	
	case TASK_WALK_PATH:
	case TASK_RUN_PATH:
		// grunt no longer assumes he is covered if he moves
		Forget( bits_MEMORY_INCOVER );
		CSquadMonster ::StartTask( task );
		break;

	case TASK_RELOAD:
		m_IdealActivity = ACT_RELOAD;
		break;

	case TASK_GRUNT_FACE_TOSS_DIR:
		break;

	case TASK_FACE_IDEAL:
	case TASK_FACE_ENEMY:
		CSquadMonster :: StartTask( task );
		if ( GetMoveType() == MOVETYPE_FLY)
		{
			m_IdealActivity = ACT_GLIDE;
		}
		break;

	default: 
		CSquadMonster :: StartTask( task );
		break;
	}
}

void CHGrunt :: RunTask ( const Task_t& task )
{
	switch ( task.iTask )
	{
	case TASK_GRUNT_FACE_TOSS_DIR:
		{
			// project a point along the toss vector and turn to face that point.
			MakeIdealYaw( GetAbsOrigin() + m_vecTossVelocity * 64 );
			ChangeYaw( GetYawSpeed() );

			if ( FacingIdeal() )
			{
				m_iTaskStatus = TASKSTATUS_COMPLETE;
			}
			break;
		}
	default:
		{
			CSquadMonster :: RunTask( task );
			break;
		}
	}
}

void CHGrunt :: PainSound ( void )
{
	if ( gpGlobals->time > m_flNextPainTime )
	{
#if 0
		if ( RANDOM_LONG(0,99) < 5 )
		{
			// pain sentences are rare
			if (FOkToSpeak())
			{
				SENTENCEG_PlayRndSz( this, "HG_PAIN", HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, PITCH_NORM);
				JustSpoke();
				return;
			}
		}
#endif 
		switch ( RANDOM_LONG(0,6) )
		{
		case 0:	
			EMIT_SOUND( this, CHAN_VOICE, "hgrunt/gr_pain3.wav", 1, ATTN_NORM );
			break;
		case 1:
			EMIT_SOUND( this, CHAN_VOICE, "hgrunt/gr_pain4.wav", 1, ATTN_NORM );
			break;
		case 2:
			EMIT_SOUND( this, CHAN_VOICE, "hgrunt/gr_pain5.wav", 1, ATTN_NORM );
			break;
		case 3:
			EMIT_SOUND( this, CHAN_VOICE, "hgrunt/gr_pain1.wav", 1, ATTN_NORM );
			break;
		case 4:
			EMIT_SOUND( this, CHAN_VOICE, "hgrunt/gr_pain2.wav", 1, ATTN_NORM );
			break;
		}

		m_flNextPainTime = gpGlobals->time + 1;
	}
}

void CHGrunt :: DeathSound ( void )
{
	switch ( RANDOM_LONG(0,2) )
	{
	case 0:	
		EMIT_SOUND( this, CHAN_VOICE, "hgrunt/gr_die1.wav", 1, ATTN_IDLE );
		break;
	case 1:
		EMIT_SOUND( this, CHAN_VOICE, "hgrunt/gr_die2.wav", 1, ATTN_IDLE );
		break;
	case 2:
		EMIT_SOUND( this, CHAN_VOICE, "hgrunt/gr_die3.wav", 1, ATTN_IDLE );
		break;
	}
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

//=========================================================
// GruntFail
//=========================================================
Task_t	tlGruntFail[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT,				(float)2		},
	{ TASK_WAIT_PVS,			(float)0		},
};

Schedule_t	slGruntFail[] =
{
	{
		tlGruntFail,
		ARRAYSIZE ( tlGruntFail ),
		bits_COND_CAN_RANGE_ATTACK1 |
		bits_COND_CAN_RANGE_ATTACK2 |
		bits_COND_CAN_MELEE_ATTACK1 |
		bits_COND_CAN_MELEE_ATTACK2,
		0,
		"Grunt Fail"
	},
};

//=========================================================
// Grunt Combat Fail
//=========================================================
Task_t	tlGruntCombatFail[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT_FACE_ENEMY,		(float)2		},
	{ TASK_WAIT_PVS,			(float)0		},
};

Schedule_t	slGruntCombatFail[] =
{
	{
		tlGruntCombatFail,
		ARRAYSIZE ( tlGruntCombatFail ),
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2,
		0,
		"Grunt Combat Fail"
	},
};

//=========================================================
// Victory dance!
//=========================================================
Task_t	tlGruntVictoryDance[] =
{
	{ TASK_STOP_MOVING,						(float)0					},
	{ TASK_FACE_ENEMY,						(float)0					},
	{ TASK_WAIT,							(float)1.5					},
	{ TASK_GET_PATH_TO_ENEMY_CORPSE,		(float)0					},
	{ TASK_WALK_PATH,						(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,				(float)0					},
	{ TASK_FACE_ENEMY,						(float)0					},
	{ TASK_PLAY_SEQUENCE,					(float)ACT_VICTORY_DANCE	},
};

Schedule_t	slGruntVictoryDance[] =
{
	{ 
		tlGruntVictoryDance,
		ARRAYSIZE ( tlGruntVictoryDance ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE,
		0,
		"GruntVictoryDance"
	},
};

//=========================================================
// Establish line of fire - move to a position that allows
// the grunt to attack.
//=========================================================
Task_t tlGruntEstablishLineOfFire[] = 
{
	{ TASK_SET_FAIL_SCHEDULE,	(float)SCHED_GRUNT_ELOF_FAIL	},
	{ TASK_GET_PATH_TO_ENEMY,	(float)0						},
	{ TASK_GRUNT_SPEAK_SENTENCE,(float)0						},
	{ TASK_RUN_PATH,			(float)0						},
	{ TASK_WAIT_FOR_MOVEMENT,	(float)0						},
};

Schedule_t slGruntEstablishLineOfFire[] =
{
	{ 
		tlGruntEstablishLineOfFire,
		ARRAYSIZE ( tlGruntEstablishLineOfFire ),
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_MELEE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2	|
		bits_COND_CAN_MELEE_ATTACK2	|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER,
		"GruntEstablishLineOfFire"
	},
};

//=========================================================
// GruntFoundEnemy - grunt established sight with an enemy
// that was hiding from the squad.
//=========================================================
Task_t	tlGruntFoundEnemy[] =
{
	{ TASK_STOP_MOVING,				0							},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,(float)ACT_SIGNAL1			},
};

Schedule_t	slGruntFoundEnemy[] =
{
	{ 
		tlGruntFoundEnemy,
		ARRAYSIZE ( tlGruntFoundEnemy ), 
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER,
		"GruntFoundEnemy"
	},
};

//=========================================================
// GruntCombatFace Schedule
//=========================================================
Task_t	tlGruntCombatFace1[] =
{
	{ TASK_STOP_MOVING,				0							},
	{ TASK_SET_ACTIVITY,			(float)ACT_IDLE				},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_WAIT,					(float)1.5					},
	{ TASK_SET_SCHEDULE,			(float)SCHED_GRUNT_SWEEP	},
};

Schedule_t	slGruntCombatFace[] =
{
	{ 
		tlGruntCombatFace1,
		ARRAYSIZE ( tlGruntCombatFace1 ), 
		bits_COND_NEW_ENEMY				|
		bits_COND_ENEMY_DEAD			|
		bits_COND_CAN_RANGE_ATTACK1		|
		bits_COND_CAN_RANGE_ATTACK2,
		0,
		"Combat Face"
	},
};

//=========================================================
// Suppressing fire - don't stop shooting until the clip is
// empty or grunt gets hurt.
//=========================================================
Task_t	tlGruntSignalSuppress[] =
{
	{ TASK_STOP_MOVING,					0						},
	{ TASK_FACE_IDEAL,					(float)0				},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,	(float)ACT_SIGNAL2		},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_GRUNT_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_GRUNT_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_GRUNT_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_GRUNT_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_GRUNT_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
};

Schedule_t	slGruntSignalSuppress[] =
{
	{ 
		tlGruntSignalSuppress,
		ARRAYSIZE ( tlGruntSignalSuppress ), 
		bits_COND_ENEMY_DEAD		|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND		|
		bits_COND_GRUNT_NOFIRE		|
		bits_COND_NO_AMMO_LOADED,

		bits_SOUND_DANGER,
		"SignalSuppress"
	},
};

Task_t	tlGruntSuppress[] =
{
	{ TASK_STOP_MOVING,			0							},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
};

Schedule_t	slGruntSuppress[] =
{
	{ 
		tlGruntSuppress,
		ARRAYSIZE ( tlGruntSuppress ), 
		bits_COND_ENEMY_DEAD		|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND		|
		bits_COND_GRUNT_NOFIRE		|
		bits_COND_NO_AMMO_LOADED,

		bits_SOUND_DANGER,
		"Suppress"
	},
};


//=========================================================
// grunt wait in cover - we don't allow danger or the ability
// to attack to break a grunt's run to cover schedule, but
// when a grunt is in cover, we do want them to attack if they can.
//=========================================================
Task_t	tlGruntWaitInCover[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_SET_ACTIVITY,			(float)ACT_IDLE				},
	{ TASK_WAIT_FACE_ENEMY,			(float)1					},
};

Schedule_t	slGruntWaitInCover[] =
{
	{ 
		tlGruntWaitInCover,
		ARRAYSIZE ( tlGruntWaitInCover ), 
		bits_COND_NEW_ENEMY			|
		bits_COND_HEAR_SOUND		|
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2	|
		bits_COND_CAN_MELEE_ATTACK1	|
		bits_COND_CAN_MELEE_ATTACK2,

		bits_SOUND_DANGER,
		"GruntWaitInCover"
	},
};

//=========================================================
// run to cover.
// !!!BUGBUG - set a decent fail schedule here.
//=========================================================
Task_t	tlGruntTakeCover1[] =
{
	{ TASK_STOP_MOVING,				(float)0							},
	{ TASK_SET_FAIL_SCHEDULE,		(float)SCHED_GRUNT_TAKECOVER_FAILED	},
	{ TASK_WAIT,					(float)0.2							},
	{ TASK_FIND_COVER_FROM_ENEMY,	(float)0							},
	{ TASK_GRUNT_SPEAK_SENTENCE,	(float)0							},
	{ TASK_RUN_PATH,				(float)0							},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0							},
	{ TASK_REMEMBER,				(float)bits_MEMORY_INCOVER			},
	{ TASK_SET_SCHEDULE,			(float)SCHED_GRUNT_WAIT_FACE_ENEMY	},
};

Schedule_t	slGruntTakeCover[] =
{
	{ 
		tlGruntTakeCover1,
		ARRAYSIZE ( tlGruntTakeCover1 ), 
		0,
		0,
		"TakeCover"
	},
};

//=========================================================
// drop grenade then run to cover.
//=========================================================
Task_t	tlGruntGrenadeCover1[] =
{
	{ TASK_STOP_MOVING,						(float)0							},
	{ TASK_FIND_COVER_FROM_ENEMY,			(float)99							},
	{ TASK_FIND_FAR_NODE_COVER_FROM_ENEMY,	(float)384							},
	{ TASK_PLAY_SEQUENCE,					(float)ACT_SPECIAL_ATTACK1			},
	{ TASK_CLEAR_MOVE_WAIT,					(float)0							},
	{ TASK_RUN_PATH,						(float)0							},
	{ TASK_WAIT_FOR_MOVEMENT,				(float)0							},
	{ TASK_SET_SCHEDULE,					(float)SCHED_GRUNT_WAIT_FACE_ENEMY	},
};

Schedule_t	slGruntGrenadeCover[] =
{
	{ 
		tlGruntGrenadeCover1,
		ARRAYSIZE ( tlGruntGrenadeCover1 ), 
		0,
		0,
		"GrenadeCover"
	},
};


//=========================================================
// drop grenade then run to cover.
//=========================================================
Task_t	tlGruntTossGrenadeCover1[] =
{
	{ TASK_FACE_ENEMY,						(float)0							},
	{ TASK_RANGE_ATTACK2, 					(float)0							},
	{ TASK_SET_SCHEDULE,					(float)SCHED_TAKE_COVER_FROM_ENEMY	},
};

Schedule_t	slGruntTossGrenadeCover[] =
{
	{ 
		tlGruntTossGrenadeCover1,
		ARRAYSIZE ( tlGruntTossGrenadeCover1 ), 
		0,
		0,
		"TossGrenadeCover"
	},
};

//=========================================================
// hide from the loudest sound source (to run from grenade)
//=========================================================
Task_t	tlGruntTakeCoverFromBestSound[] =
{
	{ TASK_SET_FAIL_SCHEDULE,			(float)SCHED_COWER			},// duck and cover if cannot move from explosion
	{ TASK_STOP_MOVING,					(float)0					},
	{ TASK_FIND_COVER_FROM_BEST_SOUND,	(float)0					},
	{ TASK_RUN_PATH,					(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,			(float)0					},
	{ TASK_REMEMBER,					(float)bits_MEMORY_INCOVER	},
	{ TASK_TURN_LEFT,					(float)179					},
};

Schedule_t	slGruntTakeCoverFromBestSound[] =
{
	{ 
		tlGruntTakeCoverFromBestSound,
		ARRAYSIZE ( tlGruntTakeCoverFromBestSound ), 
		0,
		0,
		"GruntTakeCoverFromBestSound"
	},
};

//=========================================================
// Grunt reload schedule
//=========================================================
Task_t	tlGruntHideReload[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_SET_FAIL_SCHEDULE,		(float)SCHED_RELOAD			},
	{ TASK_FIND_COVER_FROM_ENEMY,	(float)0					},
	{ TASK_RUN_PATH,				(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0					},
	{ TASK_REMEMBER,				(float)bits_MEMORY_INCOVER	},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_RELOAD			},
};

Schedule_t slGruntHideReload[] = 
{
	{
		tlGruntHideReload,
		ARRAYSIZE ( tlGruntHideReload ),
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND,

		bits_SOUND_DANGER,
		"GruntHideReload"
	}
};

//=========================================================
// Do a turning sweep of the area
//=========================================================
Task_t	tlGruntSweep[] =
{
	{ TASK_TURN_LEFT,			(float)179	},
	{ TASK_WAIT,				(float)1	},
	{ TASK_TURN_LEFT,			(float)179	},
	{ TASK_WAIT,				(float)1	},
};

Schedule_t	slGruntSweep[] =
{
	{ 
		tlGruntSweep,
		ARRAYSIZE ( tlGruntSweep ), 
		
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2	|
		bits_COND_HEAR_SOUND,

		bits_SOUND_WORLD		|// sound flags
		bits_SOUND_DANGER		|
		bits_SOUND_PLAYER,

		"Grunt Sweep"
	},
};

//=========================================================
// primary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlGruntRangeAttack1A[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,		(float)ACT_CROUCH },
	{ TASK_GRUNT_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
};

Schedule_t	slGruntRangeAttack1A[] =
{
	{ 
		tlGruntRangeAttack1A,
		ARRAYSIZE ( tlGruntRangeAttack1A ), 
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_ENEMY_OCCLUDED	|
		bits_COND_HEAR_SOUND		|
		bits_COND_GRUNT_NOFIRE		|
		bits_COND_NO_AMMO_LOADED,
		
		bits_SOUND_DANGER,
		"Range Attack1A"
	},
};


//=========================================================
// primary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlGruntRangeAttack1B[] =
{
	{ TASK_STOP_MOVING,				(float)0		},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,(float)ACT_IDLE_ANGRY  },
	{ TASK_GRUNT_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_GRUNT_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
};

Schedule_t	slGruntRangeAttack1B[] =
{
	{ 
		tlGruntRangeAttack1B,
		ARRAYSIZE ( tlGruntRangeAttack1B ), 
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_ENEMY_OCCLUDED	|
		bits_COND_NO_AMMO_LOADED	|
		bits_COND_GRUNT_NOFIRE		|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER,
		"Range Attack1B"
	},
};

//=========================================================
// secondary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlGruntRangeAttack2[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_GRUNT_FACE_TOSS_DIR,		(float)0					},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_RANGE_ATTACK2	},
	{ TASK_SET_SCHEDULE,			(float)SCHED_GRUNT_WAIT_FACE_ENEMY	},// don't run immediately after throwing grenade.
};

Schedule_t	slGruntRangeAttack2[] =
{
	{ 
		tlGruntRangeAttack2,
		ARRAYSIZE ( tlGruntRangeAttack2 ), 
		0,
		0,
		"RangeAttack2"
	},
};


//=========================================================
// repel 
//=========================================================
Task_t	tlGruntRepel[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_GLIDE 	},
};

Schedule_t	slGruntRepel[] =
{
	{ 
		tlGruntRepel,
		ARRAYSIZE ( tlGruntRepel ), 
		bits_COND_SEE_ENEMY			|
		bits_COND_NEW_ENEMY			|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER			|
		bits_SOUND_COMBAT			|
		bits_SOUND_PLAYER, 
		"Repel"
	},
};


//=========================================================
// repel 
//=========================================================
Task_t	tlGruntRepelAttack[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_FLY 	},
};

Schedule_t	slGruntRepelAttack[] =
{
	{ 
		tlGruntRepelAttack,
		ARRAYSIZE ( tlGruntRepelAttack ), 
		bits_COND_ENEMY_OCCLUDED,
		0,
		"Repel Attack"
	},
};

//=========================================================
// repel land
//=========================================================
Task_t	tlGruntRepelLand[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_LAND	},
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0				},
	{ TASK_RUN_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_CLEAR_LASTPOSITION,		(float)0				},
};

Schedule_t	slGruntRepelLand[] =
{
	{ 
		tlGruntRepelLand,
		ARRAYSIZE ( tlGruntRepelLand ), 
		bits_COND_SEE_ENEMY			|
		bits_COND_NEW_ENEMY			|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER			|
		bits_SOUND_COMBAT			|
		bits_SOUND_PLAYER, 
		"Repel Land"
	},
};


BEGIN_SCHEDULES( CHGrunt )
	slGruntFail,
	slGruntCombatFail,
	slGruntVictoryDance,
	slGruntEstablishLineOfFire,
	slGruntFoundEnemy,
	slGruntCombatFace,
	slGruntSignalSuppress,
	slGruntSuppress,
	slGruntWaitInCover,
	slGruntTakeCover,
	slGruntGrenadeCover,
	slGruntTossGrenadeCover,
	slGruntTakeCoverFromBestSound,
	slGruntHideReload,
	slGruntSweep,
	slGruntRangeAttack1A,
	slGruntRangeAttack1B,
	slGruntRangeAttack2,
	slGruntRepel,
	slGruntRepelAttack,
	slGruntRepelLand,
END_SCHEDULES()

//=========================================================
// SetActivity 
//=========================================================
void CHGrunt :: SetActivity ( Activity NewActivity )
{
	int	iSequence = ACTIVITY_NOT_AVAILABLE;

	switch ( NewActivity)
	{
	case ACT_RANGE_ATTACK1:
		// grunt is either shooting standing or shooting crouched
		if( GetWeapons().Any( HGRUNT_9MMAR ) )
		{
			if ( m_fStanding )
			{
				// get aimable sequence
				iSequence = LookupSequence( "standing_mp5" );
			}
			else
			{
				// get crouching shoot
				iSequence = LookupSequence( "crouching_mp5" );
			}
		}
		else
		{
			if ( m_fStanding )
			{
				// get aimable sequence
				iSequence = LookupSequence( "standing_shotgun" );
			}
			else
			{
				// get crouching shoot
				iSequence = LookupSequence( "crouching_shotgun" );
			}
		}
		break;
	case ACT_RANGE_ATTACK2:
		// grunt is going to a secondary long range attack. This may be a thrown 
		// grenade or fired grenade, we must determine which and pick proper sequence
		if( GetWeapons().Any( HGRUNT_HANDGRENADE ) )
		{
			// get toss anim
			iSequence = LookupSequence( "throwgrenade" );
		}
		else
		{
			// get launch anim
			iSequence = LookupSequence( "launchgrenade" );
		}
		break;
	case ACT_RUN:
		if ( GetHealth() <= HGRUNT_LIMP_HEALTH )
		{
			// limp!
			iSequence = LookupActivity ( ACT_RUN_HURT );
		}
		else
		{
			iSequence = LookupActivity ( NewActivity );
		}
		break;
	case ACT_WALK:
		if ( GetHealth() <= HGRUNT_LIMP_HEALTH )
		{
			// limp!
			iSequence = LookupActivity ( ACT_WALK_HURT );
		}
		else
		{
			iSequence = LookupActivity ( NewActivity );
		}
		break;
	case ACT_IDLE:
		if ( m_MonsterState == MONSTERSTATE_COMBAT )
		{
			NewActivity = ACT_IDLE_ANGRY;
		}
		iSequence = LookupActivity ( NewActivity );
		break;
	default:
		iSequence = LookupActivity ( NewActivity );
		break;
	}
	
	m_Activity = NewActivity; // Go ahead and set this so it doesn't keep trying when the anim is not present

	// Set to the desired anim, or default anim if the desired is not present
	if ( iSequence > ACTIVITY_NOT_AVAILABLE )
	{
		if ( GetSequence() != iSequence || !m_fSequenceLoops )
		{
			SetFrame( 0 );
		}

		SetSequence( iSequence );	// Set to the reset anim (if it's there)
		ResetSequenceInfo( );
		UpdateYawSpeed();
	}
	else
	{
		// Not available try to get default anim
		ALERT ( at_console, "%s has no sequence for act:%d\n", GetClassname(), NewActivity );
		SetSequence( 0 );	// Set to the reset anim (if it's there)
	}
}

//=========================================================
// Get Schedule!
//=========================================================
Schedule_t *CHGrunt :: GetSchedule( void )
{

	// clear old sentence
	m_iSentence = HGRUNT_SENT_NONE;

	// flying? If PRONE, barnacle has me. IF not, it's assumed I am rapelling. 
	if ( GetMoveType() == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE )
	{
		if( GetFlags().Any( FL_ONGROUND ) )
		{
			// just landed
			SetMoveType( MOVETYPE_STEP );
			return GetScheduleOfType ( SCHED_GRUNT_REPEL_LAND );
		}
		else
		{
			// repel down a rope, 
			if ( m_MonsterState == MONSTERSTATE_COMBAT )
				return GetScheduleOfType ( SCHED_GRUNT_REPEL_ATTACK );
			else
				return GetScheduleOfType ( SCHED_GRUNT_REPEL );
		}
	}

	// grunts place HIGH priority on running away from danger sounds.
	if ( HasConditions(bits_COND_HEAR_SOUND) )
	{
		CSound *pSound;
		pSound = PBestSound();

		ASSERT( pSound != NULL );
		if ( pSound)
		{
			if (pSound->m_iType & bits_SOUND_DANGER)
			{
				// dangerous sound nearby!
				
				//!!!KELLY - currently, this is the grunt's signal that a grenade has landed nearby,
				// and the grunt should find cover from the blast
				// good place for "SHIT!" or some other colorful verbal indicator of dismay.
				// It's not safe to play a verbal order here "Scatter", etc cause 
				// this may only affect a single individual in a squad. 
				
				if (FOkToSpeak())
				{
					g_Sentences.PlayRndSz( this, "HG_GREN", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
					JustSpoke();
				}
				return GetScheduleOfType( SCHED_TAKE_COVER_FROM_BEST_SOUND );
			}
			/*
			if (!HasConditions( bits_COND_SEE_ENEMY ) && ( pSound->m_iType & (bits_SOUND_PLAYER | bits_SOUND_COMBAT) ))
			{
				MakeIdealYaw( pSound->m_vecOrigin );
			}
			*/
		}
	}
	switch	( m_MonsterState )
	{
	case MONSTERSTATE_COMBAT:
		{
// dead enemy
			if ( HasConditions( bits_COND_ENEMY_DEAD ) )
			{
				// call base class, all code to handle dead enemies is centralized there.
				return CBaseMonster :: GetSchedule();
			}

// new enemy
			if ( HasConditions(bits_COND_NEW_ENEMY) )
			{
				if ( InSquad() )
				{
					MySquadLeader()->m_fEnemyEluded = false;

					if ( !IsLeader() )
					{
						return GetScheduleOfType ( SCHED_TAKE_COVER_FROM_ENEMY );
					}
					else 
					{
						//!!!KELLY - the leader of a squad of grunts has just seen the player or a 
						// monster and has made it the squad's enemy. You
						// can check GetFlags() for FL_CLIENT to determine whether this is the player
						// or a monster. He's going to immediately start
						// firing, though. If you'd like, we can make an alternate "first sight" 
						// schedule where the leader plays a handsign anim
						// that gives us enough time to hear a short sentence or spoken command
						// before he starts pluggin away.
						if (FOkToSpeak())// && RANDOM_LONG(0,1))
						{
							if ((m_hEnemy != NULL) && m_hEnemy->IsPlayer())
								// player
								g_Sentences.PlayRndSz( this, "HG_ALERT", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
							else if ((m_hEnemy != NULL) &&
									(m_hEnemy->Classify() != EntityClassifications().GetClassificationId( classify::PLAYER_ALLY ) ) && 
									(m_hEnemy->Classify() != EntityClassifications().GetClassificationId( classify::HUMAN_PASSIVE ) ) && 
									(m_hEnemy->Classify() != EntityClassifications().GetClassificationId( classify::MACHINE ) ))
								// monster
								g_Sentences.PlayRndSz( this, "HG_MONST", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);

							JustSpoke();
						}
						
						if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) )
						{
							return GetScheduleOfType ( SCHED_GRUNT_SUPPRESS );
						}
						else
						{
							return GetScheduleOfType ( SCHED_GRUNT_ESTABLISH_LINE_OF_FIRE );
						}
					}
				}
			}
// no ammo
			else if ( HasConditions ( bits_COND_NO_AMMO_LOADED ) )
			{
				//!!!KELLY - this individual just realized he's out of bullet ammo. 
				// He's going to try to find cover to run to and reload, but rarely, if 
				// none is available, he'll drop and reload in the open here. 
				return GetScheduleOfType ( SCHED_GRUNT_COVER_AND_RELOAD );
			}
			
// damaged just a little
			else if ( HasConditions( bits_COND_LIGHT_DAMAGE ) )
			{
				// if hurt:
				// 90% chance of taking cover
				// 10% chance of flinch.
				int iPercent = RANDOM_LONG(0,99);

				if ( iPercent <= 90 && m_hEnemy != NULL )
				{
					// only try to take cover if we actually have an enemy!

					//!!!KELLY - this grunt was hit and is going to run to cover.
					if (FOkToSpeak()) // && RANDOM_LONG(0,1))
					{
						//SENTENCEG_PlayRndSz( this, "HG_COVER", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
						m_iSentence = HGRUNT_SENT_COVER;
						//JustSpoke();
					}
					return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
				}
				else
				{
					return GetScheduleOfType( SCHED_SMALL_FLINCH );
				}
			}
// can kick
			else if ( HasConditions ( bits_COND_CAN_MELEE_ATTACK1 ) )
			{
				return GetScheduleOfType ( SCHED_MELEE_ATTACK1 );
			}
// can grenade launch

			else if ( GetWeapons().Any( HGRUNT_GRENADELAUNCHER ) && HasConditions ( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_HGRUNT_GRENADE ) )
			{
				// shoot a grenade if you can
				return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
			}
// can shoot
			else if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) )
			{
				if ( InSquad() )
				{
					// if the enemy has eluded the squad and a squad member has just located the enemy
					// and the enemy does not see the squad member, issue a call to the squad to waste a 
					// little time and give the player a chance to turn.
					if ( MySquadLeader()->m_fEnemyEluded && !HasConditions ( bits_COND_ENEMY_FACING_ME ) )
					{
						MySquadLeader()->m_fEnemyEluded = false;
						return GetScheduleOfType ( SCHED_GRUNT_FOUND_ENEMY );
					}
				}

				if ( OccupySlot ( bits_SLOTS_HGRUNT_ENGAGE ) )
				{
					// try to take an available ENGAGE slot
					return GetScheduleOfType( SCHED_RANGE_ATTACK1 );
				}
				else if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_HGRUNT_GRENADE ) )
				{
					// throw a grenade if can and no engage slots are available
					return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
				}
				else
				{
					// hide!
					return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
				}
			}
// can't see enemy
			else if ( HasConditions( bits_COND_ENEMY_OCCLUDED ) )
			{
				if ( HasConditions( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_HGRUNT_GRENADE ) )
				{
					//!!!KELLY - this grunt is about to throw or fire a grenade at the player. Great place for "fire in the hole"  "frag out" etc
					if (FOkToSpeak())
					{
						g_Sentences.PlayRndSz( this, "HG_THROW", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
						JustSpoke();
					}
					return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
				}
				else if ( OccupySlot( bits_SLOTS_HGRUNT_ENGAGE ) )
				{
					//!!!KELLY - grunt cannot see the enemy and has just decided to 
					// charge the enemy's position. 
					if (FOkToSpeak())// && RANDOM_LONG(0,1))
					{
						//SENTENCEG_PlayRndSz( this, "HG_CHARGE", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
						m_iSentence = HGRUNT_SENT_CHARGE;
						//JustSpoke();
					}

					return GetScheduleOfType( SCHED_GRUNT_ESTABLISH_LINE_OF_FIRE );
				}
				else
				{
					//!!!KELLY - grunt is going to stay put for a couple seconds to see if
					// the enemy wanders back out into the open, or approaches the
					// grunt's covered position. Good place for a taunt, I guess?
					if (FOkToSpeak() && RANDOM_LONG(0,1))
					{
						g_Sentences.PlayRndSz( this, "HG_TAUNT", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
						JustSpoke();
					}
					return GetScheduleOfType( SCHED_STANDOFF );
				}
			}
			
			if ( HasConditions( bits_COND_SEE_ENEMY ) && !HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) )
			{
				return GetScheduleOfType ( SCHED_GRUNT_ESTABLISH_LINE_OF_FIRE );
			}

			break;
		}

	default: break;
	}
	
	// no special cases here, call the base class
	return CSquadMonster :: GetSchedule();
}

//=========================================================
//=========================================================
Schedule_t* CHGrunt :: GetScheduleOfType ( int Type ) 
{
	switch	( Type )
	{
	case SCHED_TAKE_COVER_FROM_ENEMY:
		{
			if ( InSquad() )
			{
				if ( gSkillData.GetSkillLevel() == SKILL_HARD && HasConditions( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_HGRUNT_GRENADE ) )
				{
					if (FOkToSpeak())
					{
						g_Sentences.PlayRndSz( this, "HG_THROW", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
						JustSpoke();
					}
					return slGruntTossGrenadeCover;
				}
				else
				{
					return &slGruntTakeCover[ 0 ];
				}
			}
			else
			{
				if ( RANDOM_LONG(0,1) )
				{
					return &slGruntTakeCover[ 0 ];
				}
				else
				{
					return &slGruntGrenadeCover[ 0 ];
				}
			}
		}
	case SCHED_TAKE_COVER_FROM_BEST_SOUND:
		{
			return &slGruntTakeCoverFromBestSound[ 0 ];
		}
	case SCHED_GRUNT_TAKECOVER_FAILED:
		{
			if ( HasConditions( bits_COND_CAN_RANGE_ATTACK1 ) && OccupySlot( bits_SLOTS_HGRUNT_ENGAGE ) )
			{
				return GetScheduleOfType( SCHED_RANGE_ATTACK1 );
			}

			return GetScheduleOfType ( SCHED_FAIL );
		}
		break;
	case SCHED_GRUNT_ELOF_FAIL:
		{
			// human grunt is unable to move to a position that allows him to attack the enemy.
			return GetScheduleOfType ( SCHED_TAKE_COVER_FROM_ENEMY );
		}
		break;
	case SCHED_GRUNT_ESTABLISH_LINE_OF_FIRE:
		{
			return &slGruntEstablishLineOfFire[ 0 ];
		}
		break;
	case SCHED_RANGE_ATTACK1:
		{
			// randomly stand or crouch
			if (RANDOM_LONG(0,9) == 0)
				m_fStanding = RANDOM_LONG(0,1) != 0;
		 
			if (m_fStanding)
				return &slGruntRangeAttack1B[ 0 ];
			else
				return &slGruntRangeAttack1A[ 0 ];
		}
	case SCHED_RANGE_ATTACK2:
		{
			return &slGruntRangeAttack2[ 0 ];
		}
	case SCHED_COMBAT_FACE:
		{
			return &slGruntCombatFace[ 0 ];
		}
	case SCHED_GRUNT_WAIT_FACE_ENEMY:
		{
			return &slGruntWaitInCover[ 0 ];
		}
	case SCHED_GRUNT_SWEEP:
		{
			return &slGruntSweep[ 0 ];
		}
	case SCHED_GRUNT_COVER_AND_RELOAD:
		{
			return &slGruntHideReload[ 0 ];
		}
	case SCHED_GRUNT_FOUND_ENEMY:
		{
			return &slGruntFoundEnemy[ 0 ];
		}
	case SCHED_VICTORY_DANCE:
		{
			if ( InSquad() )
			{
				if ( !IsLeader() )
				{
					return &slGruntFail[ 0 ];
				}
			}

			return &slGruntVictoryDance[ 0 ];
		}
	case SCHED_GRUNT_SUPPRESS:
		{
			if ( m_hEnemy->IsPlayer() && m_fFirstEncounter )
			{
				m_fFirstEncounter = false;// after first encounter, leader won't issue handsigns anymore when he has a new enemy
				return &slGruntSignalSuppress[ 0 ];
			}
			else
			{
				return &slGruntSuppress[ 0 ];
			}
		}
	case SCHED_FAIL:
		{
			if ( m_hEnemy != NULL )
			{
				// grunt has an enemy, so pick a different default fail schedule most likely to help recover.
				return &slGruntCombatFail[ 0 ];
			}

			return &slGruntFail[ 0 ];
		}
	case SCHED_GRUNT_REPEL:
		{
			if ( GetAbsVelocity().z > -128)
			{
				Vector vecVelocity = GetAbsVelocity();
				vecVelocity.z -= 32;
				SetAbsVelocity( vecVelocity );
			}
			return &slGruntRepel[ 0 ];
		}
	case SCHED_GRUNT_REPEL_ATTACK:
		{
			if ( GetAbsVelocity().z > -128)
			{
				Vector vecVelocity = GetAbsVelocity();
				vecVelocity.z -= 32;
				SetAbsVelocity( vecVelocity );
			}
			return &slGruntRepelAttack[ 0 ];
		}
	case SCHED_GRUNT_REPEL_LAND:
		{
			return &slGruntRepelLand[ 0 ];
		}
	default:
		{
			return CSquadMonster :: GetScheduleOfType ( Type );
		}
	}
}