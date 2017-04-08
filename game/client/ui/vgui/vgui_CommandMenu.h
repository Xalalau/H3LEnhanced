//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef GAME_CLIENT_UI_VGUI_VGUI_COMMANDMENU_H
#define GAME_CLIENT_UI_VGUI_VGUI_COMMANDMENU_H

#include <VGUI_Panel.h>

#include "vgui_Defs.h"

class CommandButton;

//============================================================
// Command Menus
class CCommandMenu : public vgui::Panel
{
private:
	CCommandMenu *m_pParentMenu;
	int			  m_iXOffset;
	int			  m_iYOffset;

	// Buttons in this menu
	CommandButton *m_aButtons[ MAX_BUTTONS ];
	int			  m_iButtons;

	// opens menu from top to bottom (0 = default), or from bottom to top (1)?
	int				m_iDirection;
public:
	CCommandMenu( CCommandMenu *pParentMenu, int x, int y, int wide, int tall )
		: vgui::Panel( x, y, wide, tall )
	{
		m_pParentMenu = pParentMenu;
		m_iXOffset = x;
		m_iYOffset = y;
		m_iButtons = 0;
		m_iDirection = 0;
	}


	CCommandMenu( CCommandMenu *pParentMenu, int direction, int x, int y, int wide, int tall )
		: vgui::Panel( x, y, wide, tall )
	{
		m_pParentMenu = pParentMenu;
		m_iXOffset = x;
		m_iYOffset = y;
		m_iButtons = 0;
		m_iDirection = direction;
	}

	float		m_flButtonSizeY;
	int			m_iSpectCmdMenu;
	void		AddButton( CommandButton *pButton );
	bool		RecalculateVisibles( int iNewYPos, bool bHideAll );
	void		RecalculatePositions( int iYOffset );
	void		MakeVisible( CCommandMenu *pChildMenu );

	CCommandMenu *GetParentMenu() { return m_pParentMenu; };
	int			GetXOffset() { return m_iXOffset; };
	int			GetYOffset() { return m_iYOffset; };
	int			GetDirection() { return m_iDirection; };
	int			GetNumButtons() { return m_iButtons; };
	CommandButton *FindButtonWithSubmenu( CCommandMenu *pSubMenu );

	void		ClearButtonsOfArmedState( void );

	void		RemoveAllButtons( void );


	bool		KeyInput( int keyNum );

	virtual void paintBackground();
};

#endif //GAME_CLIENT_UI_VGUI_VGUI_COMMANDMENU_H
