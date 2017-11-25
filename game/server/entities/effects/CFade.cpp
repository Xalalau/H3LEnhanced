#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "shake.h"

#include "CFade.h"

LINK_ENTITY_TO_CLASS( env_fade, CFade );

void CFade::Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	GetEffects().ClearAll();
	SetFrame( 0 );
}

void CFade::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int fadeFlags = 0;

	if( !GetSpawnFlags().Any( SF_FADE_IN ) )
		fadeFlags |= FFADE_OUT;

	if( GetSpawnFlags().Any( SF_FADE_MODULATE ) )
		fadeFlags |= FFADE_MODULATE;

	if( GetSpawnFlags().Any( SF_FADE_ONLYONE ) )
	{
		if( pActivator->IsNetClient() )
		{
			UTIL_ScreenFade( pActivator, GetRenderColor(), Duration(), HoldTime(), GetRenderAmount(), fadeFlags );
		}
	}
	else
	{
		UTIL_ScreenFadeAll( GetRenderColor(), Duration(), HoldTime(), GetRenderAmount(), fadeFlags );
	}
	SUB_UseTargets( this, USE_TOGGLE, 0 );
}

void CFade::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "duration" ) )
	{
		SetDuration( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "holdtime" ) )
	{
		SetHoldTime( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue( pkvd );
}