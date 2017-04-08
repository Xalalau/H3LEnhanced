//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef GAME_CLIENT_UI_VGUI_VGUI_MENUPANEL_H
#define GAME_CLIENT_UI_VGUI_VGUI_MENUPANEL_H

#include "vgui_TransparentPanel.h"

//================================================================
// Menu Panel that supports buffering of menus
class CMenuPanel : public CTransparentPanel
{
private:
	CMenuPanel *m_pNextMenu;
	int			m_iMenuID;
	int			m_iRemoveMe;
	int			m_iIsActive;
	float		m_flOpenTime;
public:
	CMenuPanel( int iRemoveMe, int x, int y, int wide, int tall )
		: CTransparentPanel( 100, x, y, wide, tall )
	{
		Reset();
		m_iRemoveMe = iRemoveMe;
	}

	CMenuPanel( int iTrans, int iRemoveMe, int x, int y, int wide, int tall )
		: CTransparentPanel( iTrans, x, y, wide, tall )
	{
		Reset();
		m_iRemoveMe = iRemoveMe;
	}

	virtual void Reset( void )
	{
		m_pNextMenu = NULL;
		m_iIsActive = false;
		m_flOpenTime = 0;
	}

	void SetNextMenu( CMenuPanel *pNextPanel )
	{
		if( m_pNextMenu )
			m_pNextMenu->SetNextMenu( pNextPanel );
		else
			m_pNextMenu = pNextPanel;
	}

	void SetMenuID( int iID )
	{
		m_iMenuID = iID;
	}

	void SetActive( int iState )
	{
		m_iIsActive = iState;
	}

	virtual void Open( void );

	virtual void Close( void );

	int			ShouldBeRemoved() { return m_iRemoveMe; };
	CMenuPanel* GetNextMenu() { return m_pNextMenu; };
	int			GetMenuID() { return m_iMenuID; };
	int			IsActive() { return m_iIsActive; };
	float		GetOpenTime() { return m_flOpenTime; };

	// Numeric input
	virtual bool SlotInput( int iSlot ) { return false; };
	virtual void SetActiveInfo( int iInput ) {};
};

#endif //GAME_CLIENT_UI_VGUI_VGUI_MENUPANEL_H
