//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "hud.h"
#include "cl_util.h"

#include "vgui_ActionSignalHandlers.h"
#include "vgui_TeamFortressViewport.h"

void CMenuHandler_TextWindow::actionPerformed( vgui::Panel* panel )
{
	if( m_iState == HIDE_TEXTWINDOW )
	{
		gViewPort->HideTopMenu();
	}
	else
	{
		gViewPort->HideCommandMenu();
		gViewPort->ShowVGUIMenu( m_iState );
	}
}

CMenuHandler_ToggleCvar::CMenuHandler_ToggleCvar( const char * cvarname )
{
	m_cvar = gEngfuncs.pfnGetCvarPointer( cvarname );
}

void CMenuHandler_ToggleCvar::actionPerformed( vgui::Panel* panel )
{
	//TODO: this doesn't properly update the string value - Solokiller
	if( m_cvar->value )
		m_cvar->value = 0.0f;
	else
		m_cvar->value = 1.0f;

	// hide the menu 
	gViewPort->HideCommandMenu();

	gViewPort->UpdateSpectatorPanel();
}
