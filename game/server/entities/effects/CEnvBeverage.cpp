#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CEnvBeverage.h"

LINK_ENTITY_TO_CLASS( env_beverage, CEnvBeverage );

void CEnvBeverage::Spawn( void )
{
	Precache();
	SetSolidType( SOLID_NOT );
	GetEffects() = EF_NODRAW;
	SetFrags( 0 );

	if( GetHealth() == 0 )
	{
		SetHealth( 10 );
	}
}

void CEnvBeverage::Precache( void )
{
	PRECACHE_MODEL( "models/can.mdl" );
	PRECACHE_SOUND( "weapons/g_bounce3.wav" );
}

void CEnvBeverage::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( GetFrags() != 0 || GetHealth() <= 0 )
	{
		// no more cans while one is waiting in the dispenser, or if I'm out of cans.
		return;
	}

	CBaseEntity *pCan = CBaseEntity::Create( "item_sodacan", GetAbsOrigin(), GetAbsAngles(), edict() );

	if( GetSkin() == 6 )
	{
		// random
		pCan->SetSkin( RANDOM_LONG( 0, 5 ) );
	}
	else
	{
		pCan->SetSkin( GetSkin() );
	}

	SetFrags( 1 );
	SetHealth( GetHealth() - 1 );

	//SetThink (SUB_Remove);
	//SetNextThink( gpGlobals->time );
}