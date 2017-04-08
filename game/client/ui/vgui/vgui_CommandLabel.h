//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef GAME_CLIENT_UI_VGUI_VGUI_COMMANDLABEL_H
#define GAME_CLIENT_UI_VGUI_VGUI_COMMANDLABEL_H

#include <VGUI_Label.h>

// Command Label
// Overridden label so we can darken it when submenus open
class CommandLabel : public vgui::Label
{
private:
	int		m_iState;

public:
	CommandLabel( const char* text, int x, int y, int wide, int tall )
		: vgui::Label( text, x, y, wide, tall )
	{
		m_iState = false;
	}

	void PushUp()
	{
		m_iState = false;
		repaint();
	}

	void PushDown()
	{
		m_iState = true;
		repaint();
	}
};

#endif //GAME_CLIENT_UI_VGUI_VGUI_COMMANDLABEL_H
