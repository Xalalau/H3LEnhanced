#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CBubbling.h"

BEGIN_DATADESC( CBubbling )
	DEFINE_FIELD( m_density, FIELD_INTEGER ),
	DEFINE_FIELD( m_frequency, FIELD_INTEGER ),
	DEFINE_FIELD( m_state, FIELD_BOOLEAN ),
	// Let spawn restore this!
	//DEFINE_FIELD( m_bubbleModel, FIELD_INTEGER ),
	DEFINE_THINKFUNC( FizzThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( env_bubbles, CBubbling );

void CBubbling::Spawn( void )
{
	Precache();
	SetModel( GetModelName() );		// Set size

	SetSolidType( SOLID_NOT );							// Remove model & collisions
	SetRenderAmount( 0 );								// The engine won't draw this model if this is set to 0 and blending is on
	SetRenderMode( kRenderTransTexture );
	int speed = fabs( GetSpeed() );

	// HACKHACK!!! - Speed in rendercolor
	SetRenderColor( Vector( speed >> 8, speed & 255, ( GetSpeed() < 0 ) ? 1 : 0 ) );


	if( !GetSpawnFlags().Any( SF_BUBBLES_STARTOFF ) )
	{
		SetThink( &CBubbling::FizzThink );
		SetNextThink( gpGlobals->time + 2.0 );
		m_state = true;
	}
	else
		m_state = false;
}

void CBubbling::Precache( void )
{
	m_bubbleModel = PRECACHE_MODEL( "sprites/bubble.spr" );			// Precache bubble sprite
}

void CBubbling::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "density" ) )
	{
		m_density = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "frequency" ) )
	{
		m_frequency = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "current" ) )
	{
		SetSpeed( atoi( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CBubbling::FizzThink( void )
{
	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, VecBModelOrigin( this ) );
		WRITE_BYTE( TE_FIZZ );
		WRITE_SHORT( ( short ) ENTINDEX( edict() ) );
		WRITE_SHORT( ( short ) m_bubbleModel );
		WRITE_BYTE( m_density );
	MESSAGE_END();

	if( m_frequency > 19 )
		SetNextThink( gpGlobals->time + 0.5 );
	else
		SetNextThink( gpGlobals->time + 2.5 - ( 0.1 * m_frequency ) );
}

void CBubbling::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( ShouldToggle( useType, m_state ) )
		m_state = !m_state;

	if( m_state )
	{
		SetThink( &CBubbling::FizzThink );
		SetNextThink( gpGlobals->time + 0.1 );
	}
	else
	{
		SetThink( NULL );
		SetNextThink( 0 );
	}
}