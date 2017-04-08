//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef GAME_CLIENT_UI_VGUI_VGUI_DRAGNDROPPANEL_H
#define GAME_CLIENT_UI_VGUI_VGUI_DRAGNDROPPANEL_H

#include <VGUI_InputSignal.h>
#include <VGUI_Panel.h>

#include "vgui_defaultinputsignal.h"

namespace vgui
{
class LineBorder;
}

class DragNDropPanel;

class CDragNDropHandler : public vgui::CDefaultInputSignal
{
private:
	DragNDropPanel*	m_pPanel;
	bool			m_bDragging;
	int				m_iaDragOrgPos[ 2 ];
	int				m_iaDragStart[ 2 ];

public:
	CDragNDropHandler( DragNDropPanel* pPanel )
	{
		m_pPanel = pPanel;
		m_bDragging = false;
	}

	void cursorMoved( int x, int y, vgui::Panel* panel );
	void mousePressed( vgui::MouseCode code, vgui::Panel* panel );
	void mouseReleased( vgui::MouseCode code, vgui::Panel* panel );
};

// Panel that can be dragged around
class DragNDropPanel : public vgui::Panel
{
private:
	bool				m_bBeingDragged;
	vgui::LineBorder	*m_pBorder;
public:
	DragNDropPanel( int x, int y, int wide, int tall );

	virtual void setDragged( bool bState )
	{
		m_bBeingDragged = bState;

		if( m_bBeingDragged )
			setBorder( m_pBorder );
		else
			setBorder( NULL );
	}
};

#endif //GAME_CLIENT_UI_VGUI_VGUI_DRAGNDROPPANEL_H
