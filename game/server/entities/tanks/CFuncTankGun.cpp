#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"

#include "CFuncTankGun.h"

LINK_ENTITY_TO_CLASS( func_tank, CFuncTankGun );

void CFuncTankGun::Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity* pAttacker )
{
	int i;

	if( m_fireLast != 0 )
	{
		// FireBullets needs gpGlobals->v_up, etc.
		UTIL_MakeAimVectors( GetAbsAngles() );

		int bulletCount = ( gpGlobals->time - m_fireLast ) * m_fireRate;
		if( bulletCount > 0 )
		{
			for( i = 0; i < bulletCount; i++ )
			{
				switch( m_bulletType )
				{
				case TANK_BULLET_9MM:
					FireBullets( 1, barrelEnd, forward, gTankSpread[ m_spread ], 4096, BULLET_MONSTER_9MM, 1, m_iBulletDamage, pAttacker );
					break;

				case TANK_BULLET_MP5:
					FireBullets( 1, barrelEnd, forward, gTankSpread[ m_spread ], 4096, BULLET_MONSTER_MP5, 1, m_iBulletDamage, pAttacker );
					break;

				case TANK_BULLET_12MM:
					FireBullets( 1, barrelEnd, forward, gTankSpread[ m_spread ], 4096, BULLET_MONSTER_12MM, 1, m_iBulletDamage, pAttacker );
					break;

				default:
				case TANK_BULLET_NONE:
					break;
				}
			}
			CFuncTank::Fire( barrelEnd, forward, pAttacker );
		}
	}
	else
		CFuncTank::Fire( barrelEnd, forward, pAttacker );
}