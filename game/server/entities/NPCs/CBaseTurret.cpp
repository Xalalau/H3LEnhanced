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
/*

===== turret.cpp ========================================================

*/

//
// TODO: 
//		Revisit enemy validation stuff, maybe it's not necessary with the newest monster code
//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"
#include "Effects.h"

#include "CBaseTurret.h"

BEGIN_DATADESC(	CBaseTurret )
	DEFINE_FIELD( m_flMaxSpin, FIELD_FLOAT ),
	DEFINE_FIELD( m_iSpin, FIELD_INTEGER ),

	DEFINE_FIELD( m_pEyeGlow, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_eyeBrightness, FIELD_INTEGER ),
	DEFINE_FIELD( m_iDeployHeight, FIELD_INTEGER ),
	DEFINE_FIELD( m_iRetractHeight, FIELD_INTEGER ),
	DEFINE_FIELD( m_iMinPitch, FIELD_INTEGER ),

	DEFINE_FIELD( m_iBaseTurnRate, FIELD_INTEGER ),
	DEFINE_FIELD( m_fTurnRate, FIELD_FLOAT ),
	DEFINE_FIELD( m_iOrientation, FIELD_INTEGER ),
	DEFINE_FIELD( m_bOn, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fBeserk, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bAutoStart, FIELD_BOOLEAN ),


	DEFINE_FIELD( m_vecLastSight, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_flLastSight, FIELD_TIME ),
	DEFINE_FIELD( m_flMaxWait, FIELD_FLOAT ),
	DEFINE_FIELD( m_iSearchSpeed, FIELD_INTEGER ),

	DEFINE_FIELD( m_flStartYaw, FIELD_FLOAT ),
	DEFINE_FIELD( m_vecCurAngles, FIELD_VECTOR ),
	DEFINE_FIELD( m_vecGoalAngles, FIELD_VECTOR ),

	DEFINE_FIELD( m_flPingTime, FIELD_TIME ),
	DEFINE_FIELD( m_flSpinUpTime, FIELD_TIME ),

	DEFINE_USEFUNC( TurretUse ),
	DEFINE_THINKFUNC( ActiveThink ),
	DEFINE_THINKFUNC( SearchThink ),
	DEFINE_THINKFUNC( AutoSearchThink ),
	DEFINE_THINKFUNC( TurretDeath ),

	DEFINE_THINKFUNC( SpinDownCall ),
	DEFINE_THINKFUNC( SpinUpCall ),

	DEFINE_THINKFUNC( Deploy ),
	DEFINE_THINKFUNC( Retire ),
	DEFINE_THINKFUNC( Initialize ),
END_DATADESC()

void CBaseTurret::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "maxsleep"))
	{
		m_flMaxWait = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "orientation"))
	{
		m_iOrientation = atoi(pkvd->szValue);
		pkvd->fHandled = true;

	}
	else if (FStrEq(pkvd->szKeyName, "searchspeed"))
	{
		m_iSearchSpeed = atoi(pkvd->szValue);
		pkvd->fHandled = true;

	}
	else if (FStrEq(pkvd->szKeyName, "turnrate"))
	{
		m_iBaseTurnRate = atoi(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "style") ||
			 FStrEq(pkvd->szKeyName, "height") ||
			 FStrEq(pkvd->szKeyName, "value1") ||
			 FStrEq(pkvd->szKeyName, "value2") ||
			 FStrEq(pkvd->szKeyName, "value3"))
		pkvd->fHandled = true;
	else
		CBaseMonster::KeyValue( pkvd );
}


void CBaseTurret::Spawn()
{ 
	Precache( );
	SetNextThink( gpGlobals->time + 1 );
	SetMoveType( MOVETYPE_FLY );
	SetSequence( 0 );
	SetFrame( 0 );
	SetSolidType( SOLID_SLIDEBOX );
	SetTakeDamageMode( DAMAGE_AIM );

	GetFlags() |= FL_MONSTER;
	SetUse( &CBaseTurret::TurretUse );

	if ( GetSpawnFlags().Any( SF_MONSTER_TURRET_AUTOACTIVATE )
		 && !GetSpawnFlags().Any( SF_MONSTER_TURRET_STARTINACTIVE ) )
	{
		m_bAutoStart = true;
	}

	ResetSequenceInfo( );
	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );
	m_flFieldOfView = VIEW_FIELD_FULL;
	// m_flSightRange = TURRET_RANGE;
}


void CBaseTurret::Precache( )
{
	PRECACHE_SOUND ("turret/tu_fire1.wav");
	PRECACHE_SOUND ("turret/tu_ping.wav");
	PRECACHE_SOUND ("turret/tu_active2.wav");
	PRECACHE_SOUND ("turret/tu_die.wav");
	PRECACHE_SOUND ("turret/tu_die2.wav");
	PRECACHE_SOUND ("turret/tu_die3.wav");
	// PRECACHE_SOUND ("turret/tu_retract.wav"); // just use deploy sound to save memory
	PRECACHE_SOUND ("turret/tu_deploy.wav");
	PRECACHE_SOUND ("turret/tu_spinup.wav");
	PRECACHE_SOUND ("turret/tu_spindown.wav");
	PRECACHE_SOUND ("turret/tu_search.wav");
	PRECACHE_SOUND ("turret/tu_alert.wav");
}

void CBaseTurret::UpdateOnRemove()
{
	BaseClass::UpdateOnRemove();

	if( m_pEyeGlow )
	{
		UTIL_Remove( m_pEyeGlow );
		m_pEyeGlow = nullptr;
	}
}

void CBaseTurret::Initialize(void)
{
	m_bOn = false;
	m_fBeserk = false;
	m_iSpin = 0;

	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );

	if (m_iBaseTurnRate == 0) m_iBaseTurnRate = TURRET_TURNRATE;
	if (m_flMaxWait == 0) m_flMaxWait = TURRET_MAXWAIT;
	m_flStartYaw = GetAbsAngles().y;
	if (m_iOrientation == 1)
	{
		SetIdealPitch( 180 );
		Vector vecAngles = GetAbsAngles();
		vecAngles.x = 180;
		vecAngles.y = vecAngles.y + 180;
		if( vecAngles.y > 360 )
			vecAngles.y = vecAngles.y - 360;
		SetAbsAngles( vecAngles );
		Vector vecView = GetViewOffset();
		vecView.z = -vecView.z;
		SetViewOffset( vecView );
		GetEffects() |= EF_INVLIGHT;
	}

	m_vecGoalAngles.x = 0;

	if ( m_bAutoStart )
	{
		m_flLastSight = gpGlobals->time + m_flMaxWait;
		SetThink(&CBaseTurret::AutoSearchThink);		
		SetNextThink( gpGlobals->time + .1 );
	}
	else
		SetThink(&CBaseTurret::SUB_DoNothing);
}

void CBaseTurret::TurretUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, m_bOn ) )
		return;

	if ( m_bOn )
	{
		m_hEnemy = NULL;
		SetNextThink( gpGlobals->time + 0.1 );
		m_bAutoStart = false;// switching off a turret disables autostart
		//!!!! this should spin down first!!BUGBUG
		SetThink(&CBaseTurret::Retire);
	}
	else 
	{
		SetNextThink( gpGlobals->time + 0.1 ); // turn on delay

		// if the turret is flagged as an autoactivate turret, re-enable it's ability open self.
		if ( GetSpawnFlags().Any( SF_MONSTER_TURRET_AUTOACTIVATE ) )
		{
			m_bAutoStart = true;
		}
		
		SetThink(&CBaseTurret::Deploy);
	}
}


void CBaseTurret::Ping( void )
{
	// make the pinging noise every second while searching
	if (m_flPingTime == 0)
		m_flPingTime = gpGlobals->time + 1;
	else if (m_flPingTime <= gpGlobals->time)
	{
		m_flPingTime = gpGlobals->time + 1;
		EMIT_SOUND( this, CHAN_ITEM, "turret/tu_ping.wav", 1, ATTN_NORM);
		EyeOn( );
	}
	else if (m_eyeBrightness > 0)
	{
		EyeOff( );
	}
}


void CBaseTurret::EyeOn( )
{
	if (m_pEyeGlow)
	{
		if (m_eyeBrightness != 255)
		{
			m_eyeBrightness = 255;
		}
		m_pEyeGlow->SetBrightness( m_eyeBrightness );
	}
}


void CBaseTurret::EyeOff( )
{
	if (m_pEyeGlow)
	{
		if (m_eyeBrightness > 0)
		{
			m_eyeBrightness = max( 0, m_eyeBrightness - 30 );
			m_pEyeGlow->SetBrightness( m_eyeBrightness );
		}
	}
}


void CBaseTurret::ActiveThink(void)
{
	bool fAttack = false;
	Vector vecDirToEnemy;

	SetNextThink( gpGlobals->time + 0.1 );
	StudioFrameAdvance( );

	if ((!m_bOn ) || (m_hEnemy == NULL))
	{
		m_hEnemy = NULL;
		m_flLastSight = gpGlobals->time + m_flMaxWait;
		SetThink(&CBaseTurret::SearchThink);
		return;
	}
	
	// if it's dead, look for something new
	if ( !m_hEnemy->IsAlive() )
	{
		if (!m_flLastSight)
		{
			m_flLastSight = gpGlobals->time + 0.5; // continue-shooting timeout
		}
		else
		{
			if (gpGlobals->time > m_flLastSight)
			{	
				m_hEnemy = NULL;
				m_flLastSight = gpGlobals->time + m_flMaxWait;
				SetThink(&CBaseTurret::SearchThink);
				return;
			}
		}
	}

	Vector vecMid = GetAbsOrigin() + GetViewOffset();
	Vector vecMidEnemy = m_hEnemy->BodyTarget( vecMid );

	// Look for our current enemy
	bool fEnemyVisible = FBoxVisible( m_hEnemy, vecMidEnemy );	

	vecDirToEnemy = vecMidEnemy - vecMid;	// calculate dir and dist to enemy
	float flDistToEnemy = vecDirToEnemy.Length();

	Vector vec = UTIL_VecToAngles(vecMidEnemy - vecMid);	

	// Current enmey is not visible.
	if (!fEnemyVisible || (flDistToEnemy > TURRET_RANGE))
	{
		if (!m_flLastSight)
			m_flLastSight = gpGlobals->time + 0.5;
		else
		{
			// Should we look for a new target?
			if (gpGlobals->time > m_flLastSight)
			{
				m_hEnemy = NULL;
				m_flLastSight = gpGlobals->time + m_flMaxWait;
				SetThink(&CBaseTurret::SearchThink);
				return;
			}
		}
		fEnemyVisible = false;
	}
	else
	{
		m_vecLastSight = vecMidEnemy;
	}

	UTIL_MakeAimVectors(m_vecCurAngles);	

	/*
	ALERT( at_console, "%.0f %.0f : %.2f %.2f %.2f\n", 
		m_vecCurAngles.x, m_vecCurAngles.y,
		gpGlobals->v_forward.x, gpGlobals->v_forward.y, gpGlobals->v_forward.z );
	*/
	
	Vector vecLOS = vecDirToEnemy; //vecMid - m_vecLastSight;
	vecLOS = vecLOS.Normalize();

	// Is the Gun looking at the target
	if (DotProduct(vecLOS, gpGlobals->v_forward) <= 0.866) // 30 degree slop
		fAttack = false;
	else
		fAttack = true;

	// fire the gun
	if (m_iSpin && ((fAttack) || (m_fBeserk)))
	{
		Vector vecSrc, vecAng;
		GetAttachment( 0, vecSrc, vecAng );
		SetTurretAnim(TURRET_ANIM_FIRE);
		Shoot(vecSrc, gpGlobals->v_forward );
	} 
	else
	{
		SetTurretAnim(TURRET_ANIM_SPIN);
	}

	//move the gun
	if (m_fBeserk)
	{
		if (RANDOM_LONG(0,9) == 0)
		{
			m_vecGoalAngles.y = RANDOM_FLOAT(0,360);
			m_vecGoalAngles.x = RANDOM_FLOAT(0,90) - 90 * m_iOrientation;
			TakeDamage( this, this, 1, DMG_GENERIC ); // don't beserk forever
			return;
		}
	} 
	else if (fEnemyVisible)
	{
		if (vec.y > 360)
			vec.y -= 360;

		if (vec.y < 0)
			vec.y += 360;

		//ALERT(at_console, "[%.2f]", vec.x);
		
		if (vec.x < -180)
			vec.x += 360;

		if (vec.x > 180)
			vec.x -= 360;

		// now all numbers should be in [1...360]
		// pin to turret limitations to [-90...15]

		if (m_iOrientation == 0)
		{
			if (vec.x > 90)
				vec.x = 90;
			else if (vec.x < m_iMinPitch)
				vec.x = m_iMinPitch;
		}
		else
		{
			if (vec.x < -90)
				vec.x = -90;
			else if (vec.x > -m_iMinPitch)
				vec.x = -m_iMinPitch;
		}

		// ALERT(at_console, "->[%.2f]\n", vec.x);

		m_vecGoalAngles.y = vec.y;
		m_vecGoalAngles.x = vec.x;

	}

	SpinUpCall();
	MoveTurret();
}

void CBaseTurret::Deploy(void)
{
	SetNextThink( gpGlobals->time + 0.1 );
	StudioFrameAdvance( );

	if ( GetSequence() != TURRET_ANIM_DEPLOY)
	{
		m_bOn = true;
		SetTurretAnim(TURRET_ANIM_DEPLOY);
		EMIT_SOUND( this, CHAN_BODY, "turret/tu_deploy.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
		SUB_UseTargets( this, USE_ON, 0 );
	}

	if (m_fSequenceFinished)
	{
		Vector vecMaxs = GetRelMax();
		vecMaxs.z = m_iDeployHeight;
		Vector vecMins = GetRelMin();
		vecMins.z = -m_iDeployHeight;
		SetSize( vecMins, vecMaxs );

		m_vecCurAngles.x = 0;

		if (m_iOrientation == 1)
		{
			m_vecCurAngles.y = UTIL_AngleMod( GetAbsAngles().y + 180 );
		}
		else
		{
			m_vecCurAngles.y = UTIL_AngleMod( GetAbsAngles().y );
		}

		SetTurretAnim(TURRET_ANIM_SPIN);
		SetFrameRate( 0 );
		SetThink(&CBaseTurret::SearchThink);
	}

	m_flLastSight = gpGlobals->time + m_flMaxWait;
}

void CBaseTurret::Retire(void)
{
	// make the turret level
	m_vecGoalAngles.x = 0;
	m_vecGoalAngles.y = m_flStartYaw;

	SetNextThink( gpGlobals->time + 0.1 );

	StudioFrameAdvance( );

	EyeOff( );

	if (!MoveTurret())
	{
		if (m_iSpin)
		{
			SpinDownCall();
		}
		else if ( GetSequence() != TURRET_ANIM_RETIRE)
		{
			SetTurretAnim(TURRET_ANIM_RETIRE);
			EMIT_SOUND_DYN( this, CHAN_BODY, "turret/tu_deploy.wav", TURRET_MACHINE_VOLUME, ATTN_NORM, 0, 120);
			SUB_UseTargets( this, USE_OFF, 0 );
		}
		else if (m_fSequenceFinished) 
		{	
			m_bOn = false;
			m_flLastSight = 0;
			SetTurretAnim(TURRET_ANIM_NONE);
			Vector vecMaxs = GetRelMax();
			vecMaxs.z = m_iRetractHeight;
			Vector vecMins = GetRelMin();
			vecMins.z = -m_iRetractHeight;
			SetSize( vecMins, vecMaxs );
			if ( m_bAutoStart )
			{
				SetThink(&CBaseTurret::AutoSearchThink);		
				SetNextThink( gpGlobals->time + .1 );
			}
			else
				SetThink(&CBaseTurret::SUB_DoNothing);
		}
	}
	else
	{
		SetTurretAnim(TURRET_ANIM_SPIN);
	}
}

void CBaseTurret::SetTurretAnim(TURRET_ANIM anim)
{
	if ( GetSequence() != anim)
	{
		switch(anim)
		{
		case TURRET_ANIM_FIRE:
		case TURRET_ANIM_SPIN:
			if ( GetSequence() != TURRET_ANIM_FIRE && GetSequence() != TURRET_ANIM_SPIN)
			{
				SetFrame( 0 );
			}
			break;
		default:
			SetFrame( 0 );
			break;
		}

		SetSequence( anim );
		ResetSequenceInfo( );

		switch(anim)
		{
			//Explicitly handle these in case more are added. - Solokiller
		case TURRET_ANIM_NONE:
		case TURRET_ANIM_FIRE:
		case TURRET_ANIM_SPIN:
		case TURRET_ANIM_DEPLOY: break;

		case TURRET_ANIM_RETIRE:
			SetFrame( 255 );
			SetFrameRate( -1.0 );
			break;
		case TURRET_ANIM_DIE:
			SetFrameRate( 1.0 );
			break;
		}
		//ALERT(at_console, "Turret anim #%d\n", anim);
	}
}


//
// This search function will sit with the turret deployed and look for a new target. 
// After a set amount of time, the barrel will spin down. After m_flMaxWait, the turret will
// retact.
//
void CBaseTurret::SearchThink(void)
{
	// ensure rethink
	SetTurretAnim(TURRET_ANIM_SPIN);
	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.1 );

	if (m_flSpinUpTime == 0 && m_flMaxSpin)
		m_flSpinUpTime = gpGlobals->time + m_flMaxSpin;

	Ping( );

	// If we have a target and we're still healthy
	if (m_hEnemy != NULL)
	{
		if (!m_hEnemy->IsAlive() )
			m_hEnemy = NULL;// Dead enemy forces a search for new one
	}


	// Acquire Target
	if (m_hEnemy == NULL)
	{
		Look(TURRET_RANGE);
		m_hEnemy = BestVisibleEnemy();
	}

	// If we've found a target, spin up the barrel and start to attack
	if (m_hEnemy != NULL)
	{
		m_flLastSight = 0;
		m_flSpinUpTime = 0;
		SetThink(&CBaseTurret::ActiveThink);
	}
	else
	{
		// Are we out of time, do we need to retract?
 		if (gpGlobals->time > m_flLastSight)
		{
			//Before we retrace, make sure that we are spun down.
			m_flLastSight = 0;
			m_flSpinUpTime = 0;
			SetThink(&CBaseTurret::Retire);
		}
		// should we stop the spin?
		else if ((m_flSpinUpTime) && (gpGlobals->time > m_flSpinUpTime))
		{
			SpinDownCall();
		}
		
		// generic hunt for new victims
		m_vecGoalAngles.y = (m_vecGoalAngles.y + 0.1 * m_fTurnRate);
		if (m_vecGoalAngles.y >= 360)
			m_vecGoalAngles.y -= 360;
		MoveTurret();
	}
}


// 
// This think function will deploy the turret when something comes into range. This is for
// automatically activated turrets.
//
void CBaseTurret::AutoSearchThink(void)
{
	// ensure rethink
	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.3 );

	// If we have a target and we're still healthy

	if (m_hEnemy != NULL)
	{
		if (!m_hEnemy->IsAlive() )
			m_hEnemy = NULL;// Dead enemy forces a search for new one
	}

	// Acquire Target

	if (m_hEnemy == NULL)
	{
		Look( TURRET_RANGE );
		m_hEnemy = BestVisibleEnemy();
	}

	if (m_hEnemy != NULL)
	{
		SetThink(&CBaseTurret::Deploy);
		EMIT_SOUND( this, CHAN_BODY, "turret/tu_alert.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
	}
}


void CBaseTurret ::	TurretDeath( void )
{
	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.1 );

	if ( GetDeadFlag() != DEAD_DEAD)
	{
		SetDeadFlag( DEAD_DEAD );

		float flRndSound = RANDOM_FLOAT ( 0 , 1 );

		if ( flRndSound <= 0.33 )
			EMIT_SOUND( this, CHAN_BODY, "turret/tu_die.wav", 1.0, ATTN_NORM);
		else if ( flRndSound <= 0.66 )
			EMIT_SOUND( this, CHAN_BODY, "turret/tu_die2.wav", 1.0, ATTN_NORM);
		else 
			EMIT_SOUND( this, CHAN_BODY, "turret/tu_die3.wav", 1.0, ATTN_NORM);

		EMIT_SOUND_DYN( this, CHAN_STATIC, "turret/tu_active2.wav", 0, 0, SND_STOP, 100);

		if (m_iOrientation == 0)
			m_vecGoalAngles.x = -15;
		else
			m_vecGoalAngles.x = -90;

		SetTurretAnim(TURRET_ANIM_DIE); 

		EyeOn( );	
	}

	EyeOff( );

	if ( GetDamageTime() + RANDOM_FLOAT( 0, 2 ) > gpGlobals->time)
	{
		// lots of smoke
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( RANDOM_FLOAT( GetAbsMin().x, GetAbsMax().x ) );
			WRITE_COORD( RANDOM_FLOAT( GetAbsMin().y, GetAbsMax().y ) );
			WRITE_COORD( GetAbsOrigin().z - m_iOrientation * 64 );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 25 ); // scale * 10
			WRITE_BYTE( 10 - m_iOrientation * 5); // framerate
		MESSAGE_END();
	}
	
	if( GetDamageTime() + RANDOM_FLOAT( 0, 5 ) > gpGlobals->time)
	{
		Vector vecSrc = Vector( RANDOM_FLOAT( GetAbsMin().x, GetAbsMax().x ), RANDOM_FLOAT( GetAbsMin().y, GetAbsMax().y ), 0 );
		if (m_iOrientation == 0)
			vecSrc = vecSrc + Vector( 0, 0, RANDOM_FLOAT( GetAbsOrigin().z, GetAbsMax().z ) );
		else
			vecSrc = vecSrc + Vector( 0, 0, RANDOM_FLOAT( GetAbsMin().z, GetAbsOrigin().z ) );

		UTIL_Sparks( vecSrc );
	}

	if (m_fSequenceFinished && !MoveTurret( ) && GetDamageTime() + 5 < gpGlobals->time)
	{
		SetFrameRate( 0 );
		SetThink( NULL );
	}
}



void CBaseTurret::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	CTakeDamageInfo newInfo = info;

	if ( tr.iHitgroup == 10 )
	{
		// hit armor
		if ( GetDamageTime() != gpGlobals->time || (RANDOM_LONG(0,10) < 1) )
		{
			UTIL_Ricochet( tr.vecEndPos, RANDOM_FLOAT( 1, 2) );
			SetDamageTime( gpGlobals->time );
		}

		newInfo.GetMutableDamage() = 0.1;// don't hurt the monster much, but allow bits_COND_LIGHT_DAMAGE to be generated
	}

	if ( GetTakeDamageMode() == DAMAGE_NO )
		return;

	g_MultiDamage.AddMultiDamage( newInfo, this );
}

// take damage. bitsDamageType indicates type of damage sustained, ie: DMG_BULLET

void CBaseTurret::OnTakeDamage( const CTakeDamageInfo& info )
{
	if ( GetTakeDamageMode() == DAMAGE_NO )
		return;

	CTakeDamageInfo newInfo = info;

	if (!m_bOn )
		newInfo.GetMutableDamage() /= 10.0;

	SetHealth( GetHealth() - newInfo.GetDamage() );
	if ( GetHealth() <= 0)
	{
		SetHealth( 0 );
		SetTakeDamageMode( DAMAGE_NO );
		SetDamageTime( gpGlobals->time );

		GetFlags().ClearFlags( FL_MONSTER ); // why are they set in the first place???

		SetUse(NULL);
		SetThink(&CBaseTurret::TurretDeath);
		SUB_UseTargets( this, USE_ON, 0 ); // wake up others
		SetNextThink( gpGlobals->time + 0.1 );

		return;
	}

	if ( GetHealth() <= 10)
	{
		if ( m_bOn && (1 || RANDOM_LONG(0, 0x7FFF) > 800))
		{
			m_fBeserk = true;
			SetThink(&CBaseTurret::SearchThink);
		}
	}
}

int CBaseTurret::MoveTurret(void)
{
	int state = 0;
	// any x movement?
	
	if (m_vecCurAngles.x != m_vecGoalAngles.x)
	{
		float flDir = m_vecGoalAngles.x > m_vecCurAngles.x ? 1 : -1 ;

		m_vecCurAngles.x += 0.1 * m_fTurnRate * flDir;

		// if we started below the goal, and now we're past, peg to goal
		if (flDir == 1)
		{
			if (m_vecCurAngles.x > m_vecGoalAngles.x)
				m_vecCurAngles.x = m_vecGoalAngles.x;
		} 
		else
		{
			if (m_vecCurAngles.x < m_vecGoalAngles.x)
				m_vecCurAngles.x = m_vecGoalAngles.x;
		}

		if (m_iOrientation == 0)
			SetBoneController(1, -m_vecCurAngles.x);
		else
			SetBoneController(1, m_vecCurAngles.x);
		state = 1;
	}

	if (m_vecCurAngles.y != m_vecGoalAngles.y)
	{
		float flDir = m_vecGoalAngles.y > m_vecCurAngles.y ? 1 : -1 ;
		float flDist = fabs(m_vecGoalAngles.y - m_vecCurAngles.y);
		
		if (flDist > 180)
		{
			flDist = 360 - flDist;
			flDir = -flDir;
		}
		if (flDist > 30)
		{
			if (m_fTurnRate < m_iBaseTurnRate * 10)
			{
				m_fTurnRate += m_iBaseTurnRate;
			}
		}
		else if (m_fTurnRate > 45)
		{
			m_fTurnRate -= m_iBaseTurnRate;
		}
		else
		{
			m_fTurnRate += m_iBaseTurnRate;
		}

		m_vecCurAngles.y += 0.1 * m_fTurnRate * flDir;

		if (m_vecCurAngles.y < 0)
			m_vecCurAngles.y += 360;
		else if (m_vecCurAngles.y >= 360)
			m_vecCurAngles.y -= 360;

		if (flDist < (0.05 * m_iBaseTurnRate))
			m_vecCurAngles.y = m_vecGoalAngles.y;

		//ALERT(at_console, "%.2f -> %.2f\n", m_vecCurAngles.y, y);
		if (m_iOrientation == 0)
			SetBoneController(0, m_vecCurAngles.y - GetAbsAngles().y );
		else 
			SetBoneController(0, GetAbsAngles().y - 180 - m_vecCurAngles.y );
		state = 1;
	}

	if (!state)
		m_fTurnRate = m_iBaseTurnRate;

	//ALERT(at_console, "(%.2f, %.2f)->(%.2f, %.2f)\n", m_vecCurAngles.x, 
	//	m_vecCurAngles.y, m_vecGoalAngles.x, m_vecGoalAngles.y);
	return state;
}

//
// ID as a machine
//
EntityClassification_t CBaseTurret::GetClassification()
{
	if ( m_bOn || m_bAutoStart )
		return EntityClassifications().GetClassificationId( classify::MACHINE );
	return EntityClassifications().GetNoneId();
}