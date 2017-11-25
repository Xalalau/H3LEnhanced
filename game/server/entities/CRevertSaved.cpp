#include "extdll.h"
#include "util.h"
#include "shake.h"
#include "cbase.h"

#include "CRevertSaved.h"

BEGIN_DATADESC( CRevertSaved )
	DEFINE_FIELD( m_messageTime, FIELD_FLOAT ),	// These are not actual times, but durations, so save as floats
	DEFINE_FIELD( m_loadTime, FIELD_FLOAT ),
	DEFINE_THINKFUNC( MessageThink ),
	DEFINE_THINKFUNC( LoadThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( player_loadsaved, CRevertSaved );

void CRevertSaved::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	UTIL_ScreenFadeAll( GetRenderColor(), Duration(), HoldTime(), GetRenderAmount(), FFADE_OUT );
	SetNextThink( gpGlobals->time + MessageTime() );
	SetThink( &CRevertSaved::MessageThink );
}

void CRevertSaved::MessageThink( void )
{
	UTIL_ShowMessageAll( GetMessage() );
	float nextThink = LoadTime() - MessageTime();
	if( nextThink > 0 )
	{
		SetNextThink( gpGlobals->time + nextThink );
		SetThink( &CRevertSaved::LoadThink );
	}
	else
		LoadThink();
}

void CRevertSaved::LoadThink( void )
{
	if( !gpGlobals->deathmatch )
	{
		SERVER_COMMAND( "reload\n" );
	}
}

void CRevertSaved::KeyValue( KeyValueData *pkvd )
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
	else if( FStrEq( pkvd->szKeyName, "messagetime" ) )
	{
		SetMessageTime( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "loadtime" ) )
	{
		SetLoadTime( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue( pkvd );
}