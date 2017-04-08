//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "hud.h"
#include "cl_util.h"

#include "vgui_InputSignalHandlers.h"
#include "vgui_TeamFortressViewport.h"

using namespace vgui;

void CHandler_MenuButtonOver::cursorEntered( Panel *panel )
{
	if( gViewPort && m_pMenuPanel )
	{
		m_pMenuPanel->SetActiveInfo( m_iButton );
	}
}
