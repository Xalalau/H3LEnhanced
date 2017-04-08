//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef GAME_CLIENT_UI_VGUI_VGUI_TEAMMENU_H
#define GAME_CLIENT_UI_VGUI_VGUI_TEAMMENU_H

namespace vgui
{
class Label;
class ScrollPanel;
class TextPanel;
}

class CommandButton;

#include "vgui_MenuPanel.h"

class CTeamMenuPanel : public CMenuPanel
{
public:
	vgui::ScrollPanel   *m_pScrollPanel;
	CTransparentPanel	*m_pTeamWindow;
	vgui::Label			*m_pMapTitle;
	vgui::TextPanel		*m_pBriefing;
	//TODO: why 6? define constant - Solokiller
	vgui::TextPanel		*m_pTeamInfoPanel[ 6 ];
	CommandButton		*m_pButtons[ 6 ];
	bool				m_bUpdatedMapName;
	CommandButton		*m_pCancelButton;
	CommandButton		*m_pSpectateButton;

	int					m_iCurrentInfo;

public:
	CTeamMenuPanel( int iTrans, int iRemoveMe, int x, int y, int wide, int tall );

	virtual bool SlotInput( int iSlot );
	virtual void Open( void );
	virtual void Update( void );
	virtual void SetActiveInfo( int iInput );
	virtual void paintBackground( void );

	virtual void Initialize( void );

	virtual void Reset( void )
	{
		CMenuPanel::Reset();
		m_iCurrentInfo = 0;
	}
};

#endif //GAME_CLIENT_UI_VGUI_VGUI_TEAMMENU_H
