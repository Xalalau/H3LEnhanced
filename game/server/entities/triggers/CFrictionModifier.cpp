#include "extdll.h"
#include "util.h"
#include "cbase.h"

// ############ hu3lifezado ############ //
// [MODO COOP]
#include "gamerules/GameRules.h"
// ############ //

#include "CFrictionModifier.h"

// Global Savedata for changelevel friction modifier
BEGIN_DATADESC( CFrictionModifier )
	DEFINE_FIELD( m_frictionFraction, FIELD_FLOAT ),
	DEFINE_TOUCHFUNC( ChangeFriction ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_friction, CFrictionModifier );

// Modify an entity's friction
void CFrictionModifier::Spawn( void )
{
	SetSolidType( SOLID_TRIGGER );
	SetModel( GetModelName() );    // set size and link into world
	SetMoveType( MOVETYPE_NONE );
	SetTouch( &CFrictionModifier::ChangeFriction );
}

// Sets toucher's friction to m_frictionFraction (1.0 = normal friction)
void CFrictionModifier::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "modifier" ) )
	{
		m_frictionFraction = atof( pkvd->szValue ) / 100.0;
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

// Sets toucher's friction to m_frictionFraction (1.0 = normal friction)
void CFrictionModifier::ChangeFriction( CBaseEntity *pOther )
{
	// ############ hu3lifezado ############ //
	// [MODO COOP]
	// Infelizmente essa entidade esta ruim no modo coop e nos nao estamos nem ai pra ela, vamos ignorar o problema
	if (g_pGameRules->IsCoOp())
		return;
	// ############ //

	if( pOther->GetMoveType() != MOVETYPE_BOUNCEMISSILE && pOther->GetMoveType() != MOVETYPE_BOUNCE )
		pOther->SetFriction( m_frictionFraction );
}