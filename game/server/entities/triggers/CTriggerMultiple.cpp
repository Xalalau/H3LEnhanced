#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CTriggerMultiple.h"

LINK_ENTITY_TO_CLASS( trigger_multiple, CTriggerMultiple );

void CTriggerMultiple::Spawn( void )
{
	if( m_flWait == 0 )
		m_flWait = 0.2;

	InitTrigger();

	ASSERTSZ( GetHealth() == 0, "trigger_multiple with health" );
	//	SetAbsOrigin( GetAbsOrigin());
	//	SetModel( GetModelName() );
	//	if (GetHealth() > 0)
	//		{
	//		if ( GetSpawnFlags().Any( SPAWNFLAG_NOTOUCH ) )
	//			ALERT(at_error, "trigger_multiple spawn: health and notouch don't make sense");
	//		SetMaxHealth( GetHealth() );
	//UNDONE: where to get pfnDie from?
	//		pev->pfnDie = multi_killed;
	//		SetTakeDamageMode( DAMAGE_YES );
	//		SetSolidType( SOLID_BBOX );
	//		SetAbsOrigin( GetAbsOrigin());  // make sure it links into the world
	//		}
	//	else
	{
		SetTouch( &CTriggerMultiple::MultiTouch );
	}
}