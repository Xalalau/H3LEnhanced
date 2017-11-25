#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CTriggerPush.h"

LINK_ENTITY_TO_CLASS( trigger_push, CTriggerPush );

void CTriggerPush::Spawn()
{
	Vector vecAngles = GetAbsAngles();

	if( vecAngles == g_vecZero )
	{
		vecAngles.y = 360;
		SetAbsAngles( vecAngles );
	}

	InitTrigger();

	if( GetSpeed() == 0 )
		SetSpeed( 100 );

	if( GetSpawnFlags().Any( SF_TRIGGER_PUSH_START_OFF ) )// if flagged to Start Turned Off, make trigger nonsolid.
		SetSolidType( SOLID_NOT );

	SetUse( &CTriggerPush::ToggleUse );

	SetAbsOrigin( GetAbsOrigin() );		// Link into the list
}

void CTriggerPush::KeyValue( KeyValueData *pkvd )
{
	CBaseTrigger::KeyValue( pkvd );
}

void CTriggerPush::Touch( CBaseEntity *pOther )
{
	// UNDONE: Is there a better way than health to detect things that have physics? (clients/monsters)
	switch( pOther->GetMoveType() )
	{
	case MOVETYPE_NONE:
	case MOVETYPE_PUSH:
	case MOVETYPE_NOCLIP:
	case MOVETYPE_FOLLOW:
		return;
	}

	if( pOther->GetSolidType() != SOLID_NOT && pOther->GetSolidType() != SOLID_BSP )
	{
		// Instant trigger, just transfer velocity and remove
		if( GetSpawnFlags().Any( SF_TRIG_PUSH_ONCE ) )
		{
			pOther->SetAbsVelocity( pOther->GetAbsVelocity() + ( GetSpeed() * GetMoveDir() ) );
			if( pOther->GetAbsVelocity().z > 0 )
				pOther->GetFlags().ClearFlags( FL_ONGROUND );
			UTIL_Remove( this );
		}
		else
		{	// Push field, transfer to base velocity
			Vector vecPush = ( GetSpeed() * GetMoveDir() );
			if( pOther->GetFlags().Any( FL_BASEVELOCITY ) )
				vecPush = vecPush + pOther->GetBaseVelocity();

			pOther->SetBaseVelocity( vecPush );

			pOther->GetFlags() |= FL_BASEVELOCITY;
			//			ALERT( at_console, "Vel %f, base %f\n", pevToucher->velocity.z, pevToucher->basevelocity.z );
		}
	}
}