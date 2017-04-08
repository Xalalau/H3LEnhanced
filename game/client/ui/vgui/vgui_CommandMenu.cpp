//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "hud.h"
#include "cl_util.h"

#include "vgui_CommandButton.h"
#include "vgui_CommandMenu.h"
#include "vgui_TeamFortressViewport.h"

using namespace vgui;

//================================================================
// COMMAND MENU
//================================================================
void CCommandMenu::AddButton( CommandButton *pButton )
{
	if( m_iButtons >= MAX_BUTTONS )
		return;

	m_aButtons[ m_iButtons ] = pButton;
	m_iButtons++;
	pButton->setParent( this );
	pButton->setFont( Scheme::sf_primary3 );

	// give the button a default key binding
	if( m_iButtons < 10 )
	{
		pButton->setBoundKey( m_iButtons + '0' );
	}
	else if( m_iButtons == 10 )
	{
		pButton->setBoundKey( '0' );
	}
}

void CCommandMenu::RemoveAllButtons( void )
{
	/*
	for(int i=0;i<m_iButtons;i++)
	{
	CommandButton *pTemp = m_aButtons[i];
	m_aButtons[i] = NULL;

	pTemp
	if(pTemp)
	{
	delete(pTemp);
	}

	}
	*/
	removeAllChildren();
	m_iButtons = 0;

}

//-----------------------------------------------------------------------------
// Purpose: Tries to find a button that has a key bound to the input, and
//			presses the button if found
// Input  : keyNum - the character number of the input key
// Output : Returns true if the command menu should close, false otherwise
//-----------------------------------------------------------------------------
bool CCommandMenu::KeyInput( int keyNum )
{
	// loop through all our buttons looking for one bound to keyNum
	for( int i = 0; i < m_iButtons; i++ )
	{
		if( !m_aButtons[ i ]->IsNotValid() )
		{
			if( m_aButtons[ i ]->getBoundKey() == keyNum )
			{
				// hit the button
				if( m_aButtons[ i ]->GetSubMenu() )
				{
					// open the sub menu
					gViewPort->SetCurrentCommandMenu( m_aButtons[ i ]->GetSubMenu() );
					return false;
				}
				else
				{
					// run the bound command
					m_aButtons[ i ]->fireActionSignal();
					return true;
				}
			}
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: clears the current menus buttons of any armed (highlighted) 
//			state, and all their sub buttons
//-----------------------------------------------------------------------------
void CCommandMenu::ClearButtonsOfArmedState( void )
{
	for( int i = 0; i < GetNumButtons(); i++ )
	{
		m_aButtons[ i ]->setArmed( false );

		if( m_aButtons[ i ]->GetSubMenu() )
		{
			m_aButtons[ i ]->GetSubMenu()->ClearButtonsOfArmedState();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pSubMenu - 
// Output : CommandButton
//-----------------------------------------------------------------------------
CommandButton *CCommandMenu::FindButtonWithSubmenu( CCommandMenu *pSubMenu )
{
	for( int i = 0; i < GetNumButtons(); i++ )
	{
		if( m_aButtons[ i ]->GetSubMenu() == pSubMenu )
			return m_aButtons[ i ];
	}

	return NULL;
}

// Recalculate the visible buttons
bool CCommandMenu::RecalculateVisibles( int iYOffset, bool bHideAll )
{
	int		i, iCurrentY = 0;
	int		iVisibleButtons = 0;

	// Cycle through all the buttons in this menu, and see which will be visible
	for( i = 0; i < m_iButtons; i++ )
	{
		int iClass = m_aButtons[ i ]->GetPlayerClass();

		if( ( iClass && iClass != g_iPlayerClass ) || ( m_aButtons[ i ]->IsNotValid() ) || bHideAll )
		{
			m_aButtons[ i ]->setVisible( false );
			if( m_aButtons[ i ]->GetSubMenu() != NULL )
			{
				( m_aButtons[ i ]->GetSubMenu() )->RecalculateVisibles( 0, true );
			}
		}
		else
		{
			// If it's got a submenu, force it to check visibilities
			if( m_aButtons[ i ]->GetSubMenu() != NULL )
			{
				if( !( m_aButtons[ i ]->GetSubMenu() )->RecalculateVisibles( 0, false ) )
				{
					// The submenu had no visible buttons, so don't display this button
					m_aButtons[ i ]->setVisible( false );
					continue;
				}
			}

			m_aButtons[ i ]->setVisible( true );
			iVisibleButtons++;
		}
	}

	// Set Size
	setSize( _size[ 0 ], ( iVisibleButtons * ( m_flButtonSizeY - 1 ) ) + 1 );

	if( iYOffset )
	{
		m_iYOffset = iYOffset;
	}

	for( i = 0; i < m_iButtons; i++ )
	{
		if( m_aButtons[ i ]->isVisible() )
		{
			if( m_aButtons[ i ]->GetSubMenu() != NULL )
				( m_aButtons[ i ]->GetSubMenu() )->RecalculateVisibles( iCurrentY + m_iYOffset, false );


			// Make sure it's at the right Y position
			// m_aButtons[i]->getPos( iXPos, iYPos );

			if( m_iDirection )
			{
				m_aButtons[ i ]->setPos( 0, ( iVisibleButtons - 1 ) * ( m_flButtonSizeY - 1 ) - iCurrentY );
			}
			else
			{
				m_aButtons[ i ]->setPos( 0, iCurrentY );
			}

			iCurrentY += ( m_flButtonSizeY - 1 );
		}
	}

	return iVisibleButtons ? true : false;
}

// Make sure all submenus can fit on the screen
void CCommandMenu::RecalculatePositions( int iYOffset )
{
	int iTop;
	int iAdjust = 0;

	m_iYOffset += iYOffset;

	if( m_iDirection )
		iTop = ScreenHeight - ( m_iYOffset + _size[ 1 ] );
	else
		iTop = m_iYOffset;

	if( iTop < 0 )
		iTop = 0;

	// Calculate if this is going to fit onscreen, and shuffle it up if it won't
	int iBottom = iTop + _size[ 1 ];

	if( iBottom > ScreenHeight )
	{
		// Move in increments of button sizes
		while( iAdjust < ( iBottom - ScreenHeight ) )
		{
			iAdjust += m_flButtonSizeY - 1;
		}

		iTop -= iAdjust;

		// Make sure it doesn't move off the top of the screen (the menu's too big to fit it all)
		if( iTop < 0 )
		{
			iAdjust -= ( 0 - iTop );
			iTop = 0;
		}
	}

	setPos( _pos[ 0 ], iTop );

	// We need to force all menus below this one to update their positions now, because they
	// might have submenus riding off buttons in this menu that have just shifted.
	for( int i = 0; i < m_iButtons; i++ )
		m_aButtons[ i ]->UpdateSubMenus( iAdjust );
}


// Make this menu and all menus above it in the chain visible
void CCommandMenu::MakeVisible( CCommandMenu *pChildMenu )
{
	/*
	// Push down the button leading to the child menu
	for (int i = 0; i < m_iButtons; i++)
	{
	if ( (pChildMenu != NULL) && (m_aButtons[i]->GetSubMenu() == pChildMenu) )
	{
	m_aButtons[i]->setArmed( true );
	}
	else
	{
	m_aButtons[i]->setArmed( false );
	}
	}
	*/

	setVisible( true );
	if( m_pParentMenu )
		m_pParentMenu->MakeVisible( this );
}

//===========================================================
// Various overloaded paint functions for Custom VGUI objects
void CCommandMenu::paintBackground()
{
	// Transparent black background

	if( m_iSpectCmdMenu )
		drawSetColor( 0, 0, 0, 64 );
	else
		drawSetColor( Scheme::sc_primary3 );

	drawFilledRect( 0, 0, _size[ 0 ], _size[ 1 ] );
}
