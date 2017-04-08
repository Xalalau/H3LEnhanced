//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_MenuPanel.h"

void CMenuPanel::Open( void )
{
	setVisible( true );

	// Note the open time, so we can delay input for a bit
	m_flOpenTime = Hud().GetTime();
}

void CMenuPanel::Close( void )
{
	setVisible( false );
	m_iIsActive = false;

	if( m_iRemoveMe )
		gViewPort->removeChild( this );

	// This MenuPanel has now been deleted. Don't append code here.
}
