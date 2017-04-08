#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "com_weapons.h"

#include "CHud.h"

float HUD_GetFOV();
int CL_ButtonBits( int );
void CL_ResetButtonBits( int bits );

extern cvar_t* sensitivity;
cvar_t* cl_lw = nullptr;

namespace
{
static CHud g_Hud;
}

CHud& Hud()
{
	return g_Hud;
}

void CHud::Init()
{
	// In case we get messages before the first update -- time will be valid
	m_flTime = 1.0;

	m_iFOV = 0;

	default_fov = CVAR_CREATE( "default_fov", "90", 0 );
	hud_takesshots = CVAR_CREATE( "hud_takesshots", "0", FCVAR_ARCHIVE );	// controls whether or not to automatically take screenshots at the end of a round
	cl_lw = gEngfuncs.pfnGetCvarPointer( "cl_lw" );
	CVAR_CREATE( "zoom_sensitivity_ratio", "1.2", 0 );

	HOOK_GLOBAL_MESSAGE( *this, SetFOV );

	//TODO: for each Hud - Solokiller
	GetHud().Init();
}

void CHud::VidInit()
{
	m_scrinfo.iSize = sizeof( m_scrinfo );
	GetScreenInfo( &m_scrinfo );

	// ----------
	// Load Sprites
	// ---------
	//	m_hsprFont = LoadSprite("sprites/%d_font.spr");

	if( ScreenWidth < 640 )
		m_iResolution = 320;
	else
		m_iResolution = 640;

	//TODO: for each hud - Solokiller
	GetHud().VidInit();
}

void CHud::GameShutdown()
{
	//TODO: for each hud - Solokiller
	GetHud().GameShutdown();
}

bool CHud::Redraw( float flTime, bool intermission )
{
	m_flOldTime = m_flTime;	// save time of previous redraw
	m_flTime = flTime;
	m_flTimeDelta = ( double ) m_flTime - m_flOldTime;

	// Clock was reset, reset delta
	if( m_flTimeDelta < 0 )
		m_flTimeDelta = 0;

	if( m_flSnapshotTime && m_flSnapshotTime < flTime )
	{
		gEngfuncs.pfnClientCmd( "snapshot\n" );
		m_flSnapshotTime = 0;
	}

	return GetHud().Redraw( flTime, intermission );
}

bool CHud::UpdateClientData( client_data_t* cdata )
{
	auto bChanged = PreThinkUpdateClient( cdata );

	m_scrinfo.iSize = sizeof( m_scrinfo );
	GetScreenInfo( &m_scrinfo );

	GetHud().Think();

	UpdateFOV( HUD_GetFOV(), true );

	bChanged = PostThinkUpdateClient( cdata ) || bChanged;

	return bChanged;
}

bool CHud::PreThinkUpdateClient( client_data_t* cdata )
{
	m_vecOrigin = cdata->origin;
	m_vecAngles = cdata->viewangles;

	m_iKeyBits = CL_ButtonBits( 0 );
	m_iWeaponBits = cdata->iWeaponBits;

	return GetHud().PreThinkUpdateClient( cdata );
}

bool CHud::PostThinkUpdateClient( client_data_t* cdata )
{
	cdata->fov = GetFOV();

	CL_ResetButtonBits( GetKeyBits() );

	return GetHud().PostThinkUpdateClient( cdata ) || true;
}

void CHud::UpdateFOV( int iNewFOV, bool bForce )
{
	if( iNewFOV == 0 )
	{
		SetFOV( default_fov->value );
	}
	else
	{
		SetFOV( iNewFOV );
	}

	// the clients fov is actually set in the client data update section of the hud

	// Set a new sensitivity
	if( m_iFOV == default_fov->value )
	{
		// reset to saved sensitivity
		SetSensitivity( 0 );
	}
	else
	{
		// set a new sensitivity that is proportional to the change from the FOV default
		SetSensitivity( sensitivity->value * ( ( float ) iNewFOV / ( float ) default_fov->value ) * CVAR_GET_FLOAT( "zoom_sensitivity_ratio" ) );
	}

	if( !bForce )
	{
		// think about default fov
		if( m_iFOV == 0 )
		{  // only let players adjust up in fov,  and only if they are not overriden by something else
			SetFOV( max( default_fov->value, 90.0f ) );
		}
	}

	GetHud().UpdateFOV( iNewFOV, bForce );
}

void CHud::SetHud( CBaseHud* pHud )
{
	if( m_pCurrentHud )
	{
		m_pCurrentHud->ActiveHudStateChanged( false );
	}

	m_pCurrentHud = pHud;

	if( m_pCurrentHud )
	{
		m_pCurrentHud->ActiveHudStateChanged( true );
	}
}

void CHud::MsgFunc_SetFOV( const char *pszName, int iSize, void *pbuf )
{
	CBufferReader reader( pbuf, iSize );

	const int newfov = reader.ReadByte();

	//Weapon prediction already takes care of changing the fog. ( g_lastFOV ).
	if( cl_lw && cl_lw->value )
		return;

	g_lastFOV = newfov;

	UpdateFOV( newfov, false );
}