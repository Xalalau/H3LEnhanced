#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CFlockingFlyer.h"

#include "CFlockingFlyerFlock.h"

BEGIN_DATADESC( CFlockingFlyerFlock )
	DEFINE_FIELD( m_cFlockSize, FIELD_INTEGER ),
	DEFINE_FIELD( m_flFlockRadius, FIELD_FLOAT ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_flyer_flock, CFlockingFlyerFlock );

void CFlockingFlyerFlock::Spawn()
{
	Precache();
	SpawnFlock();

	UTIL_RemoveNow( this );		// dump the spawn ent
}

void CFlockingFlyerFlock::Precache()
{
	//PRECACHE_MODEL("models/aflock.mdl");		
	PRECACHE_MODEL( "models/boid.mdl" );

	PrecacheFlockSounds();
}

void CFlockingFlyerFlock::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "iFlockSize" ) )
	{
		m_cFlockSize = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "flFlockRadius" ) )
	{
		m_flFlockRadius = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
}

void CFlockingFlyerFlock::SpawnFlock( void )
{
	float R = m_flFlockRadius;
	int iCount;
	Vector vecSpot;
	CFlockingFlyer *pBoid, *pLeader;

	pLeader = pBoid = NULL;

	for( iCount = 0; iCount < m_cFlockSize; iCount++ )
	{
		pBoid = static_cast<CFlockingFlyer*>( UTIL_CreateNamedEntity( "monster_flyer" ) );

		if( !pLeader )
		{
			// make this guy the leader.
			pLeader = pBoid;

			pLeader->m_pSquadLeader = pLeader;
			pLeader->m_pSquadNext = NULL;
		}

		vecSpot.x = RANDOM_FLOAT( -R, R );
		vecSpot.y = RANDOM_FLOAT( -R, R );
		vecSpot.z = RANDOM_FLOAT( 0, 16 );
		vecSpot = GetAbsOrigin() + vecSpot;

		pBoid->SetAbsOrigin( vecSpot );
		pBoid->pev->movetype = MOVETYPE_FLY;
		pBoid->SpawnCommonCode();
		pBoid->pev->flags &= ~FL_ONGROUND;
		pBoid->pev->velocity = g_vecZero;
		pBoid->pev->angles = pev->angles;

		pBoid->pev->frame = 0;
		pBoid->pev->nextthink = gpGlobals->time + 0.2;
		pBoid->SetThink( &CFlockingFlyer::IdleThink );

		if( pBoid != pLeader )
		{
			pLeader->SquadAdd( pBoid );
		}
	}
}

void CFlockingFlyerFlock::PrecacheFlockSounds( void )
{
	PRECACHE_SOUND( "boid/boid_alert1.wav" );
	PRECACHE_SOUND( "boid/boid_alert2.wav" );

	PRECACHE_SOUND( "boid/boid_idle1.wav" );
	PRECACHE_SOUND( "boid/boid_idle2.wav" );
}