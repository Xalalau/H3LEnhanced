#include "hud.h"
#include "cl_util.h"

#include "CHudList.h"

#include "vgui_TeamFortressViewport.h"

#include "particleman.h"
extern IParticleMan *g_pParticleMan;

#include "effects/CEnvironment.h"

#if USE_VGUI2
#include <vgui_controls/Panel.h>
#endif

#include "CBaseHud.h"

CBaseHud::CBaseHud()
{
}

CBaseHud::~CBaseHud()
{
	// cleans up memory allocated for sprite arrays
	delete[] m_pSprites;
}

void CBaseHud::Init()
{
	PreInit();

	CreateHudElements();
	HudList().InitHudElements();

	PostInit();

	ResetHud();
}

void CBaseHud::PreInit()
{
}

void CBaseHud::PostInit()
{
}

void CBaseHud::ActiveHudStateChanged( bool bIsActive )
{
	if( bIsActive )
		::MessageHandlers().SetFallbackHandlers( &m_MessageHandlers );
	else
		::MessageHandlers().SetFallbackHandlers( nullptr );
}

void CBaseHud::VidInit()
{
	// Only load this once
	//TODO: reload it so we can load the Hud from different locations in the future. - Solokiller
	if( !m_pSpriteList )
	{
		// we need to load the hud.txt, and all sprites within
		m_pSpriteList = SPR_GetList( "sprites/hud.txt", &m_iSpriteCountAllRes );

		if( m_pSpriteList )
		{
			// count the number of sprites of the appropriate res
			m_iSpriteCount = 0;
			client_sprite_t *p = m_pSpriteList;
			int j;
			for( j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if( p->iRes == Hud().GetResolution() )
					m_iSpriteCount++;
				p++;
			}

			// allocated memory for sprite arrays
			m_pSprites = new HudSprite_t[ m_iSpriteCount ];

			p = m_pSpriteList;
			int index = 0;
			for( j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if( p->iRes == Hud().GetResolution() )
				{
					char sz[ 256 ];
					V_sprintf_safe( sz, "sprites/%s.spr", p->szSprite );
					m_pSprites[ index ].hSprite = SPR_Load( sz );
					m_pSprites[ index ].rect = p->rc;
					strncpy( m_pSprites[ index ].szName, p->szName, sizeof( m_pSprites[ index ].szName ) );

					index++;
				}

				p++;
			}
		}
	}
	else
	{
		// we have already have loaded the sprite reference from hud.txt, but
		// we need to make sure all the sprites have been loaded (we've gone through a transition, or loaded a save game)
		client_sprite_t *p = m_pSpriteList;
		int index = 0;
		for( int j = 0; j < m_iSpriteCountAllRes; j++ )
		{
			if( p->iRes == Hud().GetResolution() )
			{
				char sz[ 256 ];
				V_sprintf_safe( sz, "sprites/%s.spr", p->szSprite );
				m_pSprites[ index ].hSprite = SPR_Load( sz );
				index++;
			}

			p++;
		}
	}

	LoadSprites();

	HudList().ForEachHudElem( &CHudElement::VidInit );
}

void CBaseHud::LoadSprites()
{
	// assumption: number_1, number_2, etc, are all listed and loaded sequentially
	m_HUD_number_0 = GetSpriteIndex( "number_0" );

	ASSERT( m_HUD_number_0 != INVALID_SPRITE_INDEX );

	const auto& rect = GetSpriteRect( m_HUD_number_0 );

	m_iFontHeight = rect.bottom - rect.top;
}

void CBaseHud::InitHud()
{
	// prepare all hud data
	HudList().ForEachHudElem( &CHudElement::InitHUDData );

	g_Environment.Initialize();

	if( g_pParticleMan )
		g_pParticleMan->ResetParticles();
}

void CBaseHud::ResetHud()
{
	// clear all hud data
	HudList().ForEachHudElem( &CHudElement::Reset );

	// reset sensitivity
	Hud().SetSensitivity( 0 );
}

bool CBaseHud::Redraw( float flTime, bool intermission )
{
	// Bring up the scoreboard during intermission
	if( gViewPort )
	{
		if( Hud().IsInIntermission() && !intermission )
		{
			// Have to do this here so the scoreboard goes away
			Hud().SetInIntermission( intermission );
			gViewPort->HideCommandMenu();
			gViewPort->HideScoreBoard();
			gViewPort->UpdateSpectatorPanel();
		}
		else if( !Hud().IsInIntermission() && intermission )
		{
			Hud().SetInIntermission( intermission );
			gViewPort->HideCommandMenu();
			gViewPort->HideVGUIMenu();
			gViewPort->ShowScoreBoard();
			gViewPort->UpdateSpectatorPanel();

			// Take a screenshot if the client's got the cvar set
			if( CVAR_GET_FLOAT( "hud_takesshots" ) != 0 )
				Hud().SetSnapshotTime( flTime + 1.0 );	// Take a screenshot in a second
		}
	}

	Hud().SetInIntermission( intermission );

	return DoDraw( flTime, intermission );
}

void CBaseHud::DrawHudElements( float flTime, HudElementEvaluatorFn evaluatorFn, void* pUserData )
{
	ASSERT( evaluatorFn );

	auto count = HudList().GetElementCount();

	for( decltype( count ) index = 0; index < count; ++index )
	{
		auto pElem = HudList().GetElementByIndex( index );

#if USE_VGUI2
		// Visible?
		bool visible = pElem->ShouldDraw();

		pElem->SetActive( visible );

		// If it's a vgui panel, hide/show as appropriate
		vgui2::Panel *pPanel = dynamic_cast<vgui2::Panel*>( pElem );
		if( pPanel && pPanel->IsVisible() != visible )
		{
			pPanel->SetVisible( visible );
		}
		else if( !pPanel )
		{
			// All HUD elements should now derive from vgui!!!
			//TODO - Solokiller
			//Assert( false );
		}

		if( visible )
		{
			pElem->ProcessInput();
		}
#endif

		if( evaluatorFn( pElem, pUserData ) )
			pElem->Draw( flTime );
	}
}

bool CBaseHud::PreThinkUpdateClient( client_data_t* cdata )
{
	return false;
}

bool CBaseHud::PostThinkUpdateClient( client_data_t* cdata )
{
	return false;
}

void CBaseHud::Think()
{
	auto count = HudList().GetElementCount();

	for( decltype( count ) index = 0; index < count; ++index )
	{
		auto pElem = HudList().GetElementByIndex( index );

		if( pElem->GetFlags() & HUD_ACTIVE )
			pElem->Think();
	}
}

void CBaseHud::UpdateFOV( int iNewFOV, bool bForce )
{
	if( gEngfuncs.IsSpectateOnly() )
	{
		if( auto pSpectator = GETHUDCLASS( CHudSpectator ) )
			Hud().SetFOV( pSpectator->GetFOV() );	// default_fov->value;
		else
			Hud().SetFOV( Hud().GetDefaultFOVCVar()->value );
	}
}

int CBaseHud::DrawHudString( int xpos, int ypos, int iMaxX, char *szIt, int r, int g, int b )
{
	return xpos + gEngfuncs.pfnDrawString( xpos, ypos, szIt, r, g, b );
}

int CBaseHud::DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b )
{
	char szString[ 32 ];
	V_sprintf_safe( szString, "%d", iNumber );
	return DrawHudStringReverse( xpos, ypos, iMinX, szString, r, g, b );
}

// draws a string from right to left (right-aligned)
int CBaseHud::DrawHudStringReverse( int xpos, int ypos, int iMinX, char *szString, int r, int g, int b )
{
	return xpos - gEngfuncs.pfnDrawStringReverse( xpos, ypos, szString, r, g, b );
}

int CBaseHud::DrawHudNumber( int x, int y, int iFlags, int iNumber, int r, int g, int b )
{
	int iWidth = GetSpriteRect( GetHudNumber0Index() ).right - GetSpriteRect( GetHudNumber0Index() ).left;
	int k;

	if( iNumber > 0 )
	{
		// SPR_Draw 100's
		if( iNumber >= 100 )
		{
			k = iNumber / 100;
			SPR_Set( GetSprite( GetHudNumber0Index() + k ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect( GetHudNumber0Index() + k ) );
			x += iWidth;
		}
		else if( iFlags & ( DHN_3DIGITS ) )
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw 10's
		if( iNumber >= 10 )
		{
			k = ( iNumber % 100 ) / 10;
			SPR_Set( GetSprite( GetHudNumber0Index() + k ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect( GetHudNumber0Index() + k ) );
			x += iWidth;
		}
		else if( iFlags & ( DHN_3DIGITS | DHN_2DIGITS ) )
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		k = iNumber % 10;
		SPR_Set( GetSprite( GetHudNumber0Index() + k ), r, g, b );
		SPR_DrawAdditive( 0, x, y, &GetSpriteRect( GetHudNumber0Index() + k ) );
		x += iWidth;
	}
	else if( iFlags & DHN_DRAWZERO )
	{
		SPR_Set( GetSprite( GetHudNumber0Index() ), r, g, b );

		// SPR_Draw 100's
		if( iFlags & ( DHN_3DIGITS ) )
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		if( iFlags & ( DHN_3DIGITS | DHN_2DIGITS ) )
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones

		SPR_DrawAdditive( 0, x, y, &GetSpriteRect( GetHudNumber0Index() ) );
		x += iWidth;
	}

	return x;
}


int CBaseHud::GetNumWidth( int iNumber, int iFlags ) const
{
	if( iFlags & ( DHN_3DIGITS ) )
		return 3;

	if( iFlags & ( DHN_2DIGITS ) )
		return 2;

	if( iNumber <= 0 )
	{
		if( iFlags & ( DHN_DRAWZERO ) )
			return 1;
		else
			return 0;
	}

	if( iNumber < 10 )
		return 1;

	if( iNumber < 100 )
		return 2;

	return 3;

}

void CBaseHud::GameShutdown()
{
	HudList().GameShutdown();

	//Free the list so we don't leak memory.
	if( m_pSpriteList )
	{
		gEngfuncs.COM_FreeFile( m_pSpriteList );
		m_pSpriteList = nullptr;
	}
}

HSPRITE CBaseHud::GetSprite( int index ) const
{
	ASSERT( index >= 0 && index < m_iSpriteCount );

	return ( index < 0 ) ? INVALID_HSPRITE : m_pSprites[ index ].hSprite;
}

const wrect_t& CBaseHud::GetSpriteRect( int index ) const
{
	ASSERT( index >= 0 && index < m_iSpriteCount );

	return m_pSprites[ index ].rect;
}

int CBaseHud::GetSpriteIndex( const char* SpriteName ) const
{
	ASSERT( SpriteName );

	// look through the loaded sprite name list for SpriteName
	for( int i = 0; i < m_iSpriteCount; i++ )
	{
		if( strncmp( SpriteName, m_pSprites[ i ].szName, MAX_SPRITE_NAME_LENGTH ) == 0 )
			return i;
	}

	return INVALID_SPRITE_INDEX; // invalid sprite
}