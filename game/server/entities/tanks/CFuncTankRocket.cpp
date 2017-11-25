#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CFuncTankRocket.h"

LINK_ENTITY_TO_CLASS( func_tankrocket, CFuncTankRocket );

void CFuncTankRocket::Precache( void )
{
	UTIL_PrecacheOther( "rpg_rocket" );
	CFuncTank::Precache();
}

void CFuncTankRocket::Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity* pAttacker )
{
	int i;

	if( m_fireLast != 0 )
	{
		int bulletCount = ( gpGlobals->time - m_fireLast ) * m_fireRate;
		if( bulletCount > 0 )
		{
			for( i = 0; i < bulletCount; i++ )
			{
				/*CBaseEntity *pRocket = */CBaseEntity::Create( "rpg_rocket", barrelEnd, GetAbsAngles(), edict() );
			}
			CFuncTank::Fire( barrelEnd, forward, pAttacker );
		}
	}
	else
		CFuncTank::Fire( barrelEnd, forward, pAttacker );
}