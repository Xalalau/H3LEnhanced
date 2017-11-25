#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CFuncTank.h"

#include "CFuncTankControls.h"

BEGIN_DATADESC( CFuncTankControls )
	DEFINE_FIELD( m_pTank, FIELD_CLASSPTR ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_tankcontrols, CFuncTankControls );

int	CFuncTankControls::ObjectCaps() const
{
	return ( CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ) | FCAP_IMPULSE_USE;
}

void CFuncTankControls::Spawn( void )
{
	SetSolidType( SOLID_TRIGGER );
	SetMoveType( MOVETYPE_NONE );
	GetEffects() |= EF_NODRAW;
	SetModel( GetModelName() );

	SetSize( GetRelMin(), GetRelMax() );
	SetAbsOrigin( GetAbsOrigin() );

	SetNextThink( gpGlobals->time + 0.3 );	// After all the func_tank's have spawned

	CBaseEntity::Spawn();
}

void CFuncTankControls::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ // pass the Use command onto the controls
	if( m_pTank )
		m_pTank->Use( pActivator, pCaller, useType, value );

	ASSERT( m_pTank != NULL );	// if this fails,  most likely means save/restore hasn't worked properly
}

void CFuncTankControls::Think()
{
	CBaseEntity* pTarget = nullptr;

	do
	{
		pTarget = UTIL_FindEntityByTargetname( pTarget, GetTarget() );
	}
	while( !FNullEnt( pTarget ) && strncmp( pTarget->GetClassname(), "func_tank", 9 ) );

	if( FNullEnt( pTarget ) )
	{
		ALERT( at_console, "No tank %s\n", GetTarget() );
		return;
	}

	m_pTank = static_cast<CFuncTank*>( pTarget );
}