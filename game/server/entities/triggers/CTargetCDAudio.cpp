#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CTriggerCDAudio.h"

#include "CTargetCDAudio.h"

LINK_ENTITY_TO_CLASS( target_cdaudio, CTargetCDAudio );

void CTargetCDAudio::Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );

	if( GetScale() > 0 )
		SetNextThink( gpGlobals->time + 1.0 );
}

void CTargetCDAudio::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "radius" ) )
	{
		SetScale( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CTargetCDAudio::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	Play();
}

// only plays for ONE client, so only use in single play!
void CTargetCDAudio::Think( void )
{
	edict_t *pClient;

	// manually find the single player. 
	pClient = g_engfuncs.pfnPEntityOfEntIndex( 1 );

	// Can't play if the client is not connected!
	if( !pClient )
		return;

	SetNextThink( gpGlobals->time + 0.5 );

	if( ( pClient->v.origin - GetAbsOrigin() ).Length() <= GetScale() )
		Play();

}

void CTargetCDAudio::Play( void )
{
	PlayCDTrack( ( int ) GetHealth() );
	UTIL_Remove( this );
}