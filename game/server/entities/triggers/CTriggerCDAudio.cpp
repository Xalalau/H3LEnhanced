#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CTriggerCDAudio.h"

void PlayCDTrack( int iTrack )
{
	edict_t *pClient;

	// manually find the single player. 
	pClient = g_engfuncs.pfnPEntityOfEntIndex( 1 );

	// Can't play if the client is not connected!
	if( !pClient )
		return;

	if( iTrack < -1 || iTrack > 30 )
	{
		ALERT( at_console, "TriggerCDAudio - Track %d out of range\n" );
		return;
	}

	if( iTrack == -1 )
	{
		CLIENT_COMMAND( pClient, "cd stop\n" );
	}
	else
	{
		char string[ 64 ];

		sprintf( string, "cd play %3d\n", iTrack );
		CLIENT_COMMAND( pClient, string );
	}
}

LINK_ENTITY_TO_CLASS( trigger_cdaudio, CTriggerCDAudio );

void CTriggerCDAudio::Spawn( void )
{
	InitTrigger();
}

void CTriggerCDAudio::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	PlayTrack();
}

//
// Changes tracks or stops CD when player touches
//
// !!!HACK - overloaded HEALTH to avoid adding new field
void CTriggerCDAudio::Touch( CBaseEntity *pOther )
{
	if( !pOther->IsPlayer() )
	{// only clients may trigger these events
		return;
	}

	PlayTrack();
}

// only plays for ONE client, so only use in single play!
void CTriggerCDAudio::PlayTrack( void )
{
	PlayCDTrack( ( int ) GetHealth() );

	SetTouch( NULL );
	UTIL_Remove( this );
}