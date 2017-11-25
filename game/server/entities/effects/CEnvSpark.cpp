#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CEnvSpark.h"

BEGIN_DATADESC( CEnvSpark )
	DEFINE_FIELD( m_flDelay, FIELD_FLOAT ),
	DEFINE_THINKFUNC( SparkThink ),
	DEFINE_USEFUNC( SparkStart ),
	DEFINE_USEFUNC( SparkStop ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( env_spark, CEnvSpark );
LINK_ENTITY_TO_CLASS( env_debris, CEnvSpark );

void CEnvSpark::Spawn( void )
{
	SetThink( NULL );
	SetUse( NULL );

	if( GetSpawnFlags().Any( SF_SPARK_TOGGLE ) ) // Use for on/off
	{
		if( GetSpawnFlags().Any( SF_SPARK_START_ON ) ) // Start on
		{
			SetThink( &CEnvSpark::SparkThink );	// start sparking
			SetUse( &CEnvSpark::SparkStop );		// set up +USE to stop sparking
		}
		else
			SetUse( &CEnvSpark::SparkStart );
	}
	else
		SetThink( &CEnvSpark::SparkThink );

	SetNextThink( gpGlobals->time + ( 0.1 + RANDOM_FLOAT( 0, 1.5 ) ) );

	if( m_flDelay <= 0 )
		m_flDelay = 1.5;

	Precache();
}

void CEnvSpark::Precache( void )
{
	PRECACHE_SOUND( "buttons/spark1.wav" );
	PRECACHE_SOUND( "buttons/spark2.wav" );
	PRECACHE_SOUND( "buttons/spark3.wav" );
	PRECACHE_SOUND( "buttons/spark4.wav" );
	PRECACHE_SOUND( "buttons/spark5.wav" );
	PRECACHE_SOUND( "buttons/spark6.wav" );
}

void CEnvSpark::SparkThink( void )
{
	SetNextThink( gpGlobals->time + 0.1 + RANDOM_FLOAT( 0, m_flDelay ) );
	DoSpark( this, GetAbsOrigin() );
}

void CEnvSpark::SparkStart( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetUse( &CEnvSpark::SparkStop );
	SetThink( &CEnvSpark::SparkThink );
	SetNextThink( gpGlobals->time + ( 0.1 + RANDOM_FLOAT( 0, m_flDelay ) ) );
}

void CEnvSpark::SparkStop( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetUse( &CEnvSpark::SparkStart );
	SetThink( NULL );
}

void CEnvSpark::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "MaxDelay" ) )
	{
		m_flDelay = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "style" ) ||
			 FStrEq( pkvd->szKeyName, "height" ) ||
			 FStrEq( pkvd->szKeyName, "killtarget" ) ||
			 FStrEq( pkvd->szKeyName, "value1" ) ||
			 FStrEq( pkvd->szKeyName, "value2" ) ||
			 FStrEq( pkvd->szKeyName, "value3" ) )
		pkvd->fHandled = true;
	else
		CBaseEntity::KeyValue( pkvd );
}