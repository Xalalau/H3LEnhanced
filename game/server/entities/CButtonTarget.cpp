#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CButtonTarget.h"

LINK_ENTITY_TO_CLASS( button_target, CButtonTarget );

void CButtonTarget::Spawn( void )
{
	SetMoveType( MOVETYPE_PUSH );
	SetSolidType( SOLID_BSP );
	SetModel( GetModelName() );
	SetTakeDamageMode( DAMAGE_YES );

	if( GetSpawnFlags().Any( SF_BTARGET_ON ) )
		SetFrame( 1 );
}

void CButtonTarget::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( !ShouldToggle( useType, static_cast<int>( GetFrame() ) != 0 ) )
		return;
	SetFrame( 1 - GetFrame() );
	if( GetFrame() )
		SUB_UseTargets( pActivator, USE_ON, 0 );
	else
		SUB_UseTargets( pActivator, USE_OFF, 0 );
}

void CButtonTarget::OnTakeDamage( const CTakeDamageInfo& info )
{
	Use( info.GetAttacker(), this, USE_TOGGLE, 0 );
}

int	CButtonTarget::ObjectCaps() const
{
	int caps = CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION;

	if( GetSpawnFlags().Any( SF_BTARGET_USE ) )
		return caps | FCAP_IMPULSE_USE;
	else
		return caps;
}