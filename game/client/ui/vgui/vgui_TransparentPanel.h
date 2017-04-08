//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef GAME_CLIENT_UI_VGUI_VGUI_TRANSPARENTPANEL_H
#define GAME_CLIENT_UI_VGUI_VGUI_TRANSPARENTPANEL_H

#include <VGUI_Panel.h>

//================================================================
// Panel that draws itself with a transparent black background
class CTransparentPanel : public vgui::Panel
{
private:
	int	m_iTransparency;
public:
	CTransparentPanel( int iTrans, int x, int y, int wide, int tall )
		: vgui::Panel( x, y, wide, tall )
	{
		m_iTransparency = iTrans;
	}

	virtual void paintBackground()
	{
		if( m_iTransparency )
		{
			// Transparent black background
			drawSetColor( 0, 0, 0, m_iTransparency );
			drawFilledRect( 0, 0, _size[ 0 ], _size[ 1 ] );
		}
	}
};

#endif //GAME_CLIENT_UI_VGUI_VGUI_TRANSPARENTPANEL_H
