#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CEnvFunnel.h"

LINK_ENTITY_TO_CLASS( env_funnel, CEnvFunnel );

void CEnvFunnel::Spawn( void )
{
	Precache();
	SetSolidType( SOLID_NOT );
	GetEffects() = EF_NODRAW;
}

void CEnvFunnel::Precache( void )
{
	m_iSprite = PRECACHE_MODEL( "sprites/flare6.spr" );
}

void CEnvFunnel::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_LARGEFUNNEL );
	WRITE_COORD( GetAbsOrigin().x );
	WRITE_COORD( GetAbsOrigin().y );
	WRITE_COORD( GetAbsOrigin().z );
	WRITE_SHORT( m_iSprite );

	if( GetSpawnFlags().Any( SF_FUNNEL_REVERSE ) )// funnel flows in reverse?
	{
		WRITE_SHORT( 1 );
	}
	else
	{
		WRITE_SHORT( 0 );
	}


	MESSAGE_END();

	SetThink( &CEnvFunnel::SUB_Remove );
	SetNextThink( gpGlobals->time );
}