#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CFlockingFlyerFlock.h"

#include "CFlockingFlyer.h"

BEGIN_DATADESC( CFlockingFlyer )
	DEFINE_FIELD( m_pSquadLeader, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_pSquadNext, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_fTurning, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fCourseAdjust, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fPathBlocked, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_vecReferencePoint, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecAdjustedVelocity, FIELD_VECTOR ),
	DEFINE_FIELD( m_flGoalSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_flLastBlockedTime, FIELD_TIME ),
	DEFINE_FIELD( m_flFakeBlockedTime, FIELD_TIME ),
	DEFINE_FIELD( m_flAlertTime, FIELD_TIME ),
	//DEFINE_FIELD( m_flFlockNextSoundTime, FIELD_TIME ),	// don't need to save
	DEFINE_THINKFUNC( IdleThink ),
	DEFINE_THINKFUNC( FormFlock ),
	DEFINE_THINKFUNC( Start ),
	DEFINE_THINKFUNC( FlockLeaderThink ),
	DEFINE_THINKFUNC( FlockFollowerThink ),
	DEFINE_THINKFUNC( FallHack ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_flyer, CFlockingFlyer );

void CFlockingFlyer::Spawn()
{
	Precache();
	SpawnCommonCode();

	SetFrame( 0 );
	SetNextThink( gpGlobals->time + 0.1 );
	SetThink( &CFlockingFlyer::IdleThink );
}

void CFlockingFlyer::Precache()
{
	//PRECACHE_MODEL("models/aflock.mdl");
	PRECACHE_MODEL( "models/boid.mdl" );
	CFlockingFlyerFlock::PrecacheFlockSounds();
}

void CFlockingFlyer::SpawnCommonCode()
{
	SetDeadFlag( DEAD_NO );
	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_FLY );
	SetTakeDamageMode( DAMAGE_NO );
	SetHealth( 1 );

	m_fPathBlocked = false;// obstacles will be detected
	m_flFieldOfView = 0.2;

	//SetModel( "models/aflock.mdl");
	SetModel( "models/boid.mdl" );

	//	SetSize( Vector(0,0,0), Vector(0,0,0));
	SetSize( Vector( -5, -5, 0 ), Vector( 5, 5, 2 ) );
}

void CFlockingFlyer::IdleThink( void )
{
	SetNextThink( gpGlobals->time + 0.2 );

	// see if there's a client in the same pvs as the monster
	if( UTIL_FindClientInPVS( this ) )
	{
		SetThink( &CFlockingFlyer::Start );
		SetNextThink( gpGlobals->time + 0.1 );
	}
}

void CFlockingFlyer::BoidAdvanceFrame()
{
	float flapspeed = ( GetSpeed() - GetArmorAmount() ) / AFLOCK_ACCELERATE;
	SetArmorAmount( GetArmorAmount() * .8 + GetSpeed() * .2 );

	if( flapspeed < 0 ) flapspeed = -flapspeed;
	if( flapspeed < 0.25 ) flapspeed = 0.25;
	if( flapspeed > 1.9 ) flapspeed = 1.9;

	SetFrameRate( flapspeed );

	Vector vecAVelocity = GetAngularVelocity();
	// lean
	vecAVelocity.x = -( GetAbsAngles().x + flapspeed * 5 );

	// bank
	vecAVelocity.z = -( GetAbsAngles().z + vecAVelocity.y );
	SetAngularVelocity( vecAVelocity );

	// SetFrameRate( flapspeed );
	StudioFrameAdvance( 0.1 );
}

//=========================================================
// Leader boid calls this to form a flock from surrounding boids
//=========================================================
void CFlockingFlyer::FormFlock( void )
{
	if( !InSquad() )
	{
		// I am my own leader
		m_pSquadLeader = this;
		m_pSquadNext = NULL;
		int squadCount = 1;

		CBaseEntity *pEntity = NULL;

		while( ( pEntity = UTIL_FindEntityInSphere( pEntity, GetAbsOrigin(), AFLOCK_MAX_RECRUIT_RADIUS ) ) != NULL )
		{
			CBaseMonster *pRecruit = pEntity->MyMonsterPointer();

			if( pRecruit && pRecruit != this && pRecruit->IsAlive() && !pRecruit->m_pCine )
			{
				// Can we recruit this guy?
				if( pRecruit->ClassnameIs( "monster_flyer" ) )
				{
					squadCount++;
					SquadAdd( ( CFlockingFlyer * ) pRecruit );
				}
			}
		}
	}

	SetThink( &CFlockingFlyer::IdleThink );// now that flock is formed, go to idle and wait for a player to come along.
	SetNextThink( gpGlobals->time );
}

//=========================================================
// Start - player enters the pvs, so get things going.
//=========================================================
void CFlockingFlyer::Start( void )
{
	SetNextThink( gpGlobals->time + 0.1 );

	if( IsLeader() )
	{
		SetThink( &CFlockingFlyer::FlockLeaderThink );
	}
	else
	{
		SetThink( &CFlockingFlyer::FlockFollowerThink );
	}

	/*
	Vector	vecTakeOff;
	vecTakeOff = Vector ( 0 , 0 , 0 );

	vecTakeOff.z = 50 + RANDOM_FLOAT ( 0, 100 );
	vecTakeOff.x = 20 - RANDOM_FLOAT ( 0, 40);
	vecTakeOff.y = 20 - RANDOM_FLOAT ( 0, 40);

	SetAbsVelocity( vecTakeOff );


	SetSpeed( GetAbsVelocity().Length() );
	SetSequence( 0 );
	*/
	SetActivity( ACT_FLY );
	ResetSequenceInfo();
	BoidAdvanceFrame();

	SetSpeed( AFLOCK_FLY_SPEED );// no delay!
}

//=========================================================
// Leader boids use this think every tenth
//=========================================================
void CFlockingFlyer::FlockLeaderThink( void )
{
	TraceResult		tr;
	Vector			vecDist;// used for general measurements
	Vector			vecDir;// used for general measurements
	/*int				cProcessed = 0;*/// keep track of how many other boids we've processed 
	float			flLeftSide;
	float			flRightSide;


	SetNextThink( gpGlobals->time + 0.1 );

	UTIL_MakeVectors( GetAbsAngles() );

	// is the way ahead clear?
	if( !FPathBlocked() )
	{
		// if the boid is turning, stop the trend.
		if( m_fTurning )
		{
			m_fTurning = false;
			Vector vecAVelocity = GetAngularVelocity();
			vecAVelocity.y = 0;
			SetAngularVelocity( vecAVelocity );
		}

		m_fPathBlocked = false;

		if( GetSpeed() <= AFLOCK_FLY_SPEED )
			SetSpeed( GetSpeed() + 5 );

		SetAbsVelocity( gpGlobals->v_forward * GetSpeed() );

		BoidAdvanceFrame();

		return;
	}

	// IF we get this far in the function, the leader's path is blocked!
	m_fPathBlocked = true;

	if( !m_fTurning )// something in the way and boid is not already turning to avoid
	{
		// measure clearance on left and right to pick the best dir to turn
		UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() + gpGlobals->v_right * AFLOCK_CHECK_DIST, ignore_monsters, ENT( pev ), &tr );
		vecDist = ( tr.vecEndPos - GetAbsOrigin() );
		flRightSide = vecDist.Length();

		UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() - gpGlobals->v_right * AFLOCK_CHECK_DIST, ignore_monsters, ENT( pev ), &tr );
		vecDist = ( tr.vecEndPos - GetAbsOrigin() );
		flLeftSide = vecDist.Length();

		Vector vecAVelocity = GetAngularVelocity();
		// turn right if more clearance on right side
		if( flRightSide > flLeftSide )
		{
			vecAVelocity.y = -AFLOCK_TURN_RATE;
			m_fTurning = true;
		}
		// default to left turn :)
		else if( flLeftSide > flRightSide )
		{
			vecAVelocity.y = AFLOCK_TURN_RATE;
			m_fTurning = true;
		}
		else
		{
			// equidistant. Pick randomly between left and right.
			m_fTurning = true;

			if( RANDOM_LONG( 0, 1 ) == 0 )
			{
				vecAVelocity.y = AFLOCK_TURN_RATE;
			}
			else
			{
				vecAVelocity.y = -AFLOCK_TURN_RATE;
			}
		}
		SetAngularVelocity( vecAVelocity );
	}
	SpreadFlock();

	SetAbsVelocity( gpGlobals->v_forward * GetSpeed() );

	// check and make sure we aren't about to plow into the ground, don't let it happen
	UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() - gpGlobals->v_up * 16, ignore_monsters, ENT( pev ), &tr );
	if( tr.flFraction != 1.0 && GetAbsVelocity().z < 0 )
	{
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.z = 0;
		SetAbsVelocity( vecVelocity );
	}

	// maybe it did, though.
	if( GetFlags().Any( FL_ONGROUND ) )
	{
		SetAbsOrigin( GetAbsOrigin() + Vector( 0, 0, 1 ) );
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.z = 0;
		SetAbsVelocity( vecVelocity );
	}

	if( m_flFlockNextSoundTime < gpGlobals->time )
	{
		MakeSound();
		m_flFlockNextSoundTime = gpGlobals->time + RANDOM_FLOAT( 1, 3 );
	}

	BoidAdvanceFrame();

	return;
}

//=========================================================
// follower boids execute this code when flocking
//=========================================================
void CFlockingFlyer::FlockFollowerThink( void )
{
	TraceResult		tr;
	Vector			vecDist;
	Vector			vecDir;
	Vector			vecDirToLeader;
	float			flDistToLeader;

	SetNextThink( gpGlobals->time + 0.1 );

	if( IsLeader() || !InSquad() )
	{
		// the leader has been killed and this flyer suddenly finds himself the leader. 
		SetThink( &CFlockingFlyer::FlockLeaderThink );
		return;
	}

	vecDirToLeader = ( m_pSquadLeader->GetAbsOrigin() - GetAbsOrigin() );
	flDistToLeader = vecDirToLeader.Length();

	// match heading with leader
	SetAbsAngles( m_pSquadLeader->GetAbsAngles() );

	//
	// We can see the leader, so try to catch up to it
	//
	if( FInViewCone( m_pSquadLeader ) )
	{
		// if we're too far away, speed up
		if( flDistToLeader > AFLOCK_TOO_FAR )
		{
			m_flGoalSpeed = m_pSquadLeader->GetAbsVelocity().Length() * 1.5;
		}

		// if we're too close, slow down
		else if( flDistToLeader < AFLOCK_TOO_CLOSE )
		{
			m_flGoalSpeed = m_pSquadLeader->GetAbsVelocity().Length() * 0.5;
		}
	}
	else
	{
		// wait up! the leader isn't out in front, so we slow down to let him pass
		m_flGoalSpeed = m_pSquadLeader->GetAbsVelocity().Length() * 0.5;
	}

	SpreadFlock2();

	SetSpeed( GetAbsVelocity().Length() );
	SetAbsVelocity( GetAbsVelocity().Normalize() );

	// if we are too far from leader, average a vector towards it into our current velocity
	if( flDistToLeader > AFLOCK_TOO_FAR )
	{
		vecDirToLeader = vecDirToLeader.Normalize();
		SetAbsVelocity( ( GetAbsVelocity() + vecDirToLeader ) * 0.5 );
	}

	// clamp speeds and handle acceleration
	if( m_flGoalSpeed > AFLOCK_FLY_SPEED * 2 )
	{
		m_flGoalSpeed = AFLOCK_FLY_SPEED * 2;
	}

	if( GetSpeed() < m_flGoalSpeed )
	{
		SetSpeed( GetSpeed() + AFLOCK_ACCELERATE );
	}
	else if( GetSpeed() > m_flGoalSpeed )
	{
		SetSpeed( GetSpeed() - AFLOCK_ACCELERATE );
	}

	SetAbsVelocity( GetAbsVelocity() * GetSpeed() );

	BoidAdvanceFrame();
}

/*
// Is this boid's course blocked?
if ( FBoidPathBlocked (pev) )
{
// course is still blocked from last time. Just keep flying along adjusted
// velocity
if ( m_fCourseAdjust )
{
SetAbsVelocity( m_vecAdjustedVelocity * GetSpeed() );
return;
}
else // set course adjust flag and calculate adjusted velocity
{
m_fCourseAdjust = true;

// use VELOCITY, not angles, not all boids point the direction they are flying
//vecDir = UTIL_VecToAngles( GetAbsVelocity() );
//UTIL_MakeVectors ( vecDir );

UTIL_MakeVectors ( GetAbsAngles() );

// measure clearance on left and right to pick the best dir to turn
UTIL_TraceLine(GetAbsOrigin(), GetAbsOrigin() + gpGlobals->v_right * AFLOCK_CHECK_DIST, ignore_monsters, ENT(pev), &tr);
vecDist = (tr.vecEndPos - GetAbsOrigin());
flRightSide = vecDist.Length();

UTIL_TraceLine(GetAbsOrigin(), GetAbsOrigin() - gpGlobals->v_right * AFLOCK_CHECK_DIST, ignore_monsters, ENT(pev), &tr);
vecDist = (tr.vecEndPos - GetAbsOrigin());
flLeftSide = vecDist.Length();

// slide right if more clearance on right side
if ( flRightSide > flLeftSide )
{
m_vecAdjustedVelocity = gpGlobals->v_right;
}
// else slide left
else
{
m_vecAdjustedVelocity = gpGlobals->v_right * -1;
}
}
return;
}

// if we make it this far, boids path is CLEAR!
m_fCourseAdjust = false;
*/

void CFlockingFlyer::FallHack( void )
{
	if( GetFlags().Any( FL_ONGROUND ) )
	{
		if( !GetGroundEntity()->ClassnameIs( "worldspawn" ) )
		{
			GetFlags().ClearFlags( FL_ONGROUND );
			SetNextThink( gpGlobals->time + 0.1 );
		}
		else
		{
			SetAbsVelocity( g_vecZero );
			SetThink( NULL );
		}
	}
}

void CFlockingFlyer::MakeSound( void )
{
	if( m_flAlertTime > gpGlobals->time )
	{
		// make agitated sounds
		switch( RANDOM_LONG( 0, 1 ) )
		{
		case 0:	EMIT_SOUND( this, CHAN_WEAPON, "boid/boid_alert1.wav", 1, ATTN_NORM );	break;
		case 1:	EMIT_SOUND( this, CHAN_WEAPON, "boid/boid_alert2.wav", 1, ATTN_NORM );	break;
		}

		return;
	}

	// make normal sound
	switch( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	EMIT_SOUND( this, CHAN_WEAPON, "boid/boid_idle1.wav", 1, ATTN_NORM );	break;
	case 1:	EMIT_SOUND( this, CHAN_WEAPON, "boid/boid_idle2.wav", 1, ATTN_NORM );	break;
	}
}

//=========================================================
// Searches for boids that are too close and pushes them away
//=========================================================
void CFlockingFlyer::SpreadFlock()
{
	Vector		vecDir;
	float		flSpeed;// holds vector magnitude while we fiddle with the direction

	CFlockingFlyer *pList = m_pSquadLeader;
	while( pList )
	{
		if( pList != this && ( GetAbsOrigin() - pList->GetAbsOrigin() ).Length() <= AFLOCK_TOO_CLOSE )
		{
			// push the other away
			vecDir = ( pList->GetAbsOrigin() - GetAbsOrigin() );
			vecDir = vecDir.Normalize();

			// store the magnitude of the other boid's velocity, and normalize it so we
			// can average in a course that points away from the leader.
			Vector vecVelocity = pList->GetAbsVelocity();
			flSpeed = vecVelocity.Length();
			vecVelocity = vecVelocity.Normalize();
			vecVelocity = ( vecVelocity + vecDir ) * 0.5;
			vecVelocity = vecVelocity * flSpeed;
			pList->SetAbsVelocity( vecVelocity );
		}

		pList = pList->m_pSquadNext;
	}
}

//=========================================================
// Alters the caller's course if he's too close to others 
//
// This function should **ONLY** be called when Caller's velocity is normalized!!
//=========================================================
void CFlockingFlyer::SpreadFlock2()
{
	Vector		vecDir;

	CFlockingFlyer *pList = m_pSquadLeader;
	while( pList )
	{
		if( pList != this && ( GetAbsOrigin() - pList->GetAbsOrigin() ).Length() <= AFLOCK_TOO_CLOSE )
		{
			vecDir = ( GetAbsOrigin() - pList->GetAbsOrigin() );
			vecDir = vecDir.Normalize();

			SetAbsVelocity( GetAbsVelocity() + vecDir );
		}

		pList = pList->m_pSquadNext;
	}
}

void CFlockingFlyer::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	CFlockingFlyer *pSquad;

	pSquad = ( CFlockingFlyer * ) m_pSquadLeader;

	while( pSquad )
	{
		pSquad->m_flAlertTime = gpGlobals->time + 15;
		pSquad = ( CFlockingFlyer * ) pSquad->m_pSquadNext;
	}

	if( m_pSquadLeader )
	{
		m_pSquadLeader->SquadRemove( this );
	}

	SetDeadFlag( DEAD_DEAD );

	SetFrameRate( 0 );
	GetEffects() = EF_NOINTERP;

	SetSize( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );
	SetMoveType( MOVETYPE_TOSS );

	SetThink( &CFlockingFlyer::FallHack );
	SetNextThink( gpGlobals->time + 0.1 );
}

//=========================================================
// FBoidPathBlocked - returns true if there is an obstacle ahead
//=========================================================
bool CFlockingFlyer::FPathBlocked()
{
	TraceResult		tr;
	Vector			vecDist;// used for general measurements
	Vector			vecDir;// used for general measurements
	bool			fBlocked;

	if( m_flFakeBlockedTime > gpGlobals->time )
	{
		m_flLastBlockedTime = gpGlobals->time;
		return true;
	}

	// use VELOCITY, not angles, not all boids point the direction they are flying
	//vecDir = UTIL_VecToAngles( pevBoid->velocity );
	UTIL_MakeVectors( GetAbsAngles() );

	fBlocked = false;// assume the way ahead is clear

					 // check for obstacle ahead
	UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() + gpGlobals->v_forward * AFLOCK_CHECK_DIST, ignore_monsters, ENT( pev ), &tr );
	if( tr.flFraction != 1.0 )
	{
		m_flLastBlockedTime = gpGlobals->time;
		fBlocked = true;
	}

	// extra wide checks
	UTIL_TraceLine( GetAbsOrigin() + gpGlobals->v_right * 12, GetAbsOrigin() + gpGlobals->v_right * 12 + gpGlobals->v_forward * AFLOCK_CHECK_DIST, ignore_monsters, ENT( pev ), &tr );
	if( tr.flFraction != 1.0 )
	{
		m_flLastBlockedTime = gpGlobals->time;
		fBlocked = true;
	}

	UTIL_TraceLine( GetAbsOrigin() - gpGlobals->v_right * 12, GetAbsOrigin() - gpGlobals->v_right * 12 + gpGlobals->v_forward * AFLOCK_CHECK_DIST, ignore_monsters, ENT( pev ), &tr );
	if( tr.flFraction != 1.0 )
	{
		m_flLastBlockedTime = gpGlobals->time;
		fBlocked = true;
	}

	if( !fBlocked && gpGlobals->time - m_flLastBlockedTime > 6 )
	{
		// not blocked, and it's been a few seconds since we've actually been blocked.
		m_flFakeBlockedTime = gpGlobals->time + RANDOM_LONG( 1, 3 );
	}

	return fBlocked;
}

//=========================================================
//
// SquadCount(), return the number of members of this squad
// callable from leaders & followers
//
//=========================================================
int CFlockingFlyer::SquadCount( void )
{
	CFlockingFlyer *pList = m_pSquadLeader;
	int squadCount = 0;
	while( pList )
	{
		squadCount++;
		pList = pList->m_pSquadNext;
	}

	return squadCount;
}

//=========================================================
//
// SquadRemove(), remove pRemove from my squad.
// If I am pRemove, promote m_pSquadNext to leader
//
//=========================================================
void CFlockingFlyer::SquadRemove( CFlockingFlyer *pRemove )
{
	ASSERT( pRemove != NULL );
	ASSERT( this->IsLeader() );
	ASSERT( pRemove->m_pSquadLeader == this );

	if( SquadCount() > 2 )
	{
		// Removing the leader, promote m_pSquadNext to leader
		if( pRemove == this )
		{
			CFlockingFlyer *pLeader = m_pSquadNext;

			if( pLeader )
			{
				// copy the enemy LKP to the new leader
				pLeader->m_vecEnemyLKP = m_vecEnemyLKP;

				CFlockingFlyer *pList = pLeader;

				while( pList )
				{
					pList->m_pSquadLeader = pLeader;
					pList = pList->m_pSquadNext;
				}

			}
			SquadUnlink();
		}
		else	// removing a node
		{
			CFlockingFlyer *pList = this;

			// Find the node before pRemove
			while( pList->m_pSquadNext != pRemove )
			{
				// assert to test valid list construction
				ASSERT( pList->m_pSquadNext != NULL );
				pList = pList->m_pSquadNext;
			}
			// List validity
			ASSERT( pList->m_pSquadNext == pRemove );

			// Relink without pRemove
			pList->m_pSquadNext = pRemove->m_pSquadNext;

			// Unlink pRemove
			pRemove->SquadUnlink();
		}
	}
	else
		SquadDisband();
}

//=========================================================
//
// SquadUnlink(), Unlink the squad pointers.
//
//=========================================================
void CFlockingFlyer::SquadUnlink( void )
{
	m_pSquadLeader = NULL;
	m_pSquadNext = NULL;
}

//=========================================================
//
// SquadAdd(), add pAdd to my squad
//
//=========================================================
void CFlockingFlyer::SquadAdd( CFlockingFlyer *pAdd )
{
	ASSERT( pAdd != NULL );
	ASSERT( !pAdd->InSquad() );
	ASSERT( this->IsLeader() );

	pAdd->m_pSquadNext = m_pSquadNext;
	m_pSquadNext = pAdd;
	pAdd->m_pSquadLeader = this;
}

//=========================================================
//
// SquadDisband(), Unlink all squad members
//
//=========================================================
void CFlockingFlyer::SquadDisband( void )
{
	CFlockingFlyer *pList = m_pSquadLeader;
	CFlockingFlyer *pNext;

	while( pList )
	{
		pNext = pList->m_pSquadNext;
		pList->SquadUnlink();
		pList = pNext;
	}
}