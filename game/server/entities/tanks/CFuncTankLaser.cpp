#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Effects.h"

#include "CFuncTankLaser.h"

BEGIN_DATADESC( CFuncTankLaser )
	DEFINE_FIELD( m_pLaser, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_laserTime, FIELD_TIME ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_tanklaser, CFuncTankLaser );

void CFuncTankLaser::Activate( void )
{
	if( !GetLaser() )
	{
		UTIL_Remove( this );
		ALERT( at_error, "Laser tank with no env_laser!\n" );
	}
	else
	{
		m_pLaser->TurnOff();
	}
}

void CFuncTankLaser::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "laserentity" ) )
	{
		SetMessage( ALLOC_STRING( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CFuncTank::KeyValue( pkvd );
}

void CFuncTankLaser::Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity* pAttacker )
{
	int i;
	TraceResult tr;

	if( m_fireLast != 0 && GetLaser() )
	{
		// TankTrace needs gpGlobals->v_up, etc.
		UTIL_MakeAimVectors( GetAbsAngles() );

		int bulletCount = ( gpGlobals->time - m_fireLast ) * m_fireRate;
		if( bulletCount )
		{
			for( i = 0; i < bulletCount; i++ )
			{
				m_pLaser->SetAbsOrigin( barrelEnd );
				TankTrace( barrelEnd, forward, gTankSpread[ m_spread ], tr );

				m_laserTime = gpGlobals->time;
				m_pLaser->TurnOn();
				m_pLaser->SetDamageTime( gpGlobals->time - 1.0 );
				m_pLaser->FireAtPoint( tr );
				m_pLaser->SetNextThink( 0 );
			}
			CFuncTank::Fire( barrelEnd, forward, pAttacker );
		}
	}
	else
	{
		CFuncTank::Fire( barrelEnd, forward, pAttacker );
	}
}

void CFuncTankLaser::Think( void )
{
	if( m_pLaser && ( gpGlobals->time > m_laserTime ) )
		m_pLaser->TurnOff();

	CFuncTank::Think();
}

CLaser *CFuncTankLaser::GetLaser( void )
{
	if( m_pLaser )
		return m_pLaser;

	CBaseEntity* pLaser = nullptr;

	while( ( pLaser = UTIL_FindEntityByTargetname( pLaser, GetMessage() ) ) != nullptr )
	{
		// Found the landmark
		if( pLaser->ClassnameIs( "env_laser" ) )
		{
			m_pLaser = static_cast<CLaser*>( pLaser );
			break;
		}
	}

	return m_pLaser;
}