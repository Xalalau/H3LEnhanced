/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// hud_redraw.cpp
//
#include <math.h>
#include "hud.h"
#include "cl_util.h"
#include "bench.h"
#include "strtools.h"

#include "vgui_TeamFortressViewport.h"

#define MAX_LOGO_FRAMES 56

int grgLogoFrame[MAX_LOGO_FRAMES] = 
{
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 13, 13, 13, 13, 12, 11, 10, 9, 8, 14, 15,
	16, 17, 18, 19, 20, 20, 20, 20, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
	29, 29, 29, 29, 29, 28, 27, 26, 25, 24, 30, 31 
};


extern int g_iVisibleMouse;

// Think
void CHLHud::Think()
{
	BaseClass::Think();
}

// Redraw
// step through the local data,  placing the appropriate graphics & text as appropriate
// returns true if they've changed, false otherwise
bool CHLHud::DoDraw( float flTime, bool intermission )
{
	// if no redrawing is necessary
	// return false;
	
	// draw all registered HUD elements
	if ( m_pCvarDraw->value )
	{
		struct CDrawInfo final
		{
			const bool bIntermission;
			CHLHud& hud;

			CDrawInfo( bool bIntermission, CHLHud& hud )
				: bIntermission( bIntermission )
				, hud( hud )
			{
			}
		};

		CDrawInfo info( intermission, *this );

		DrawHudElements(
			flTime,
			[]( CHudElement* pElem, void* pUserData )
			{
				const auto& info = *reinterpret_cast<const CDrawInfo*>( pUserData );

				if( !Bench_Active() )
				{
					if( !info.bIntermission )
					{
						if( ( pElem->GetFlags() & HUD_ACTIVE ) && !( info.hud.GetHideHudBits().Any( HIDEHUD_ALL ) ) )
							return true;
					}
					else
					{  // it's an intermission,  so only draw hud elements that are set to draw during intermissions
						if( pElem->GetFlags() & HUD_INTERMISSION )
							return true;
					}
				}
				else
				{
					if( ( Q_strcmp( pElem->GetName(), "CHudBenckmark" ) == 0 ) &&
						( pElem->GetFlags() & HUD_ACTIVE ) &&
						!( info.hud.GetHideHudBits().Any( HIDEHUD_ALL ) ) )
					{
						return true;
					}
				}

				return false;
			},
			&info
		);
	}

	// are we in demo mode? do we need to draw the logo in the top corner?
	if( m_bLogo )
	{
		int x, y, i;

		if( m_hsprLogo == INVALID_HSPRITE )
			m_hsprLogo = LoadSprite("sprites/%d_logo.spr");

		SPR_Set(m_hsprLogo, 250, 250, 250 );
		
		x = SPR_Width(m_hsprLogo, 0);
		x = ScreenWidth - x;
		y = SPR_Height(m_hsprLogo, 0)/2;

		// Draw the logo at 20 fps
		int iFrame = (int)(flTime * 20) % MAX_LOGO_FRAMES;
		i = grgLogoFrame[iFrame] - 1;

		SPR_DrawAdditive(i, x, y, NULL);
	}

	/*
	if ( g_iVisibleMouse )
	{
		void IN_GetMousePos( int *mx, int *my );
		int mx, my;

		IN_GetMousePos( &mx, &my );
		
		if( m_hsprCursor == INVALID_HSPRITE )
		{
			char sz[256];
			V_sprintf_safe( sz, "sprites/cursor.spr" );
			m_hsprCursor = SPR_Load( sz );
		}

		SPR_Set(m_hsprCursor, 250, 250, 250 );
		
		// Draw the logo at 20 fps
		SPR_DrawAdditive( 0, mx, my, NULL );
	}
	*/

	return true;
}
