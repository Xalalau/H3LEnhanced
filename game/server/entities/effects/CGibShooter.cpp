#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CGib.h"

#include "CGibShooter.h"

BEGIN_DATADESC( CGibShooter )
	DEFINE_FIELD( m_iGibs, FIELD_INTEGER ),
	DEFINE_FIELD( m_iGibCapacity, FIELD_INTEGER ),
	DEFINE_FIELD( m_iGibMaterial, FIELD_INTEGER ),
	DEFINE_FIELD( m_iGibModelIndex, FIELD_INTEGER ),
	DEFINE_FIELD( m_flGibVelocity, FIELD_FLOAT ),
	DEFINE_FIELD( m_flVariance, FIELD_FLOAT ),
	DEFINE_FIELD( m_flGibLife, FIELD_FLOAT ),
	DEFINE_THINKFUNC( ShootThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( gibshooter, CGibShooter );

void CGibShooter::Spawn( void )
{
	Precache();

	SetSolidType( SOLID_NOT );
	GetEffects() = EF_NODRAW;

	if( m_flDelay == 0 )
	{
		m_flDelay = 0.1;
	}

	if( m_flGibLife == 0 )
	{
		m_flGibLife = 25;
	}

	SetMovedir( this );
	SetBody( MODEL_FRAMES( m_iGibModelIndex ) );
}

void CGibShooter::Precache( void )
{
	if( g_Language == LANGUAGE_GERMAN )
	{
		m_iGibModelIndex = PRECACHE_MODEL( "models/germanygibs.mdl" );
	}
	else
	{
		m_iGibModelIndex = PRECACHE_MODEL( "models/hgibs.mdl" );
	}
}

void CGibShooter::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "m_iGibs" ) )
	{
		m_iGibs = m_iGibCapacity = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "m_flVelocity" ) )
	{
		m_flGibVelocity = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "m_flVariance" ) )
	{
		m_flVariance = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "m_flGibLife" ) )
	{
		m_flGibLife = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
	{
		CBaseDelay::KeyValue( pkvd );
	}
}

void CGibShooter::ShootThink( void )
{
	SetNextThink( gpGlobals->time + m_flDelay );

	Vector vecShootDir = GetMoveDir();

	vecShootDir = vecShootDir + gpGlobals->v_right * RANDOM_FLOAT( -1, 1 ) * m_flVariance;
	vecShootDir = vecShootDir + gpGlobals->v_forward * RANDOM_FLOAT( -1, 1 ) * m_flVariance;
	vecShootDir = vecShootDir + gpGlobals->v_up * RANDOM_FLOAT( -1, 1 ) * m_flVariance;

	vecShootDir = vecShootDir.Normalize();
	CGib *pGib = CreateGib();

	if( pGib )
	{
		pGib->SetAbsOrigin( GetAbsOrigin() );
		pGib->SetAbsVelocity( vecShootDir * m_flGibVelocity );

		Vector vecAVelocity = pGib->GetAngularVelocity();

		vecAVelocity.x = RANDOM_FLOAT( 100, 200 );
		vecAVelocity.y = RANDOM_FLOAT( 100, 300 );

		pGib->SetAngularVelocity( vecAVelocity );

		float thinkTime = pGib->GetNextThink() - gpGlobals->time;

		pGib->m_lifeTime = ( m_flGibLife * RANDOM_FLOAT( 0.95, 1.05 ) );	// +/- 5%
		if( pGib->m_lifeTime < thinkTime )
		{
			pGib->SetNextThink( gpGlobals->time + pGib->m_lifeTime );
			pGib->m_lifeTime = 0;
		}

	}

	if( --m_iGibs <= 0 )
	{
		if( GetSpawnFlags().Any( SF_GIBSHOOTER_REPEATABLE ) )
		{
			m_iGibs = m_iGibCapacity;
			SetThink( NULL );
			SetNextThink( gpGlobals->time );
		}
		else
		{
			SetThink( &CGibShooter::SUB_Remove );
			SetNextThink( gpGlobals->time );
		}
	}
}

void CGibShooter::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink( &CGibShooter::ShootThink );
	SetNextThink( gpGlobals->time );
}

CGib *CGibShooter::CreateGib( void )
{
	if( CVAR_GET_FLOAT( "violence_hgibs" ) == 0 )
		return NULL;

	auto pGib = CGib::GibCreate( "models/hgibs.mdl" );
	pGib->m_bloodColor = BLOOD_COLOR_RED;

	if( GetBody() <= 1 )
	{
		ALERT( at_aiconsole, "GibShooter Body is <= 1!\n" );
	}

	pGib->SetBody( RANDOM_LONG( 1, GetBody() - 1 ) );// avoid throwing random amounts of the 0th gib. (skull).

	return pGib;
}