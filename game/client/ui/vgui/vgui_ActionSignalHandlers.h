//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef GAME_CLIENT_UI_VGUI_VGUI_ACTIONSIGNALHANDLERS_H
#define GAME_CLIENT_UI_VGUI_VGUI_ACTIONSIGNALHANDLERS_H

#include <VGUI_ActionSignal.h>

#include "vgui_Defs.h"

struct cvar_t;

/**
*	@file
*
*	Generic action signal handlers
*/

#define HIDE_TEXTWINDOW		0
#define SHOW_MAPBRIEFING	1
#define SHOW_CLASSDESC		2
#define SHOW_MOTD			3
#define SHOW_SPECHELP		4

class CMenuHandler_TextWindow : public vgui::ActionSignal
{
private:
	int	m_iState;
public:
	CMenuHandler_TextWindow( int iState )
	{
		m_iState = iState;
	}

	virtual void actionPerformed( vgui::Panel* panel );
};

class CMenuHandler_ToggleCvar : public vgui::ActionSignal
{
private:
	cvar_t * m_cvar;

public:
	CMenuHandler_ToggleCvar( const char * cvarname );

	virtual void actionPerformed( vgui::Panel* panel );
};

#endif //GAME_CLIENT_UI_VGUI_VGUI_ACTIONSIGNALHANDLERS_H
