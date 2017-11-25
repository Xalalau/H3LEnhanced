//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include <VGUI_App.h>
#include <VGUI_LineBorder.h>

#include "vgui_DragNDropPanel.h"
#include "vgui_InputSignalHandlers.h"

using namespace vgui;

//================================================================
// Input Handler for Drag N Drop panels
void CDragNDropHandler::cursorMoved( int x, int y, Panel* /*panel*/ )
{
	if( m_bDragging )
	{
		App::getInstance()->getCursorPos( x, y );
		m_pPanel->setPos( m_iaDragOrgPos[ 0 ] + ( x - m_iaDragStart[ 0 ] ), m_iaDragOrgPos[ 1 ] + ( y - m_iaDragStart[ 1 ] ) );

		if( m_pPanel->getParent() != null )
		{
			m_pPanel->getParent()->repaint();
		}
	}
}

void CDragNDropHandler::mousePressed( MouseCode /*code*/, Panel* panel )
{
	int x, y;
	App::getInstance()->getCursorPos( x, y );
	m_bDragging = true;
	m_iaDragStart[ 0 ] = x;
	m_iaDragStart[ 1 ] = y;
	m_pPanel->getPos( m_iaDragOrgPos[ 0 ], m_iaDragOrgPos[ 1 ] );
	App::getInstance()->setMouseCapture( panel );

	m_pPanel->setDragged( m_bDragging );
	m_pPanel->requestFocus();
}

void CDragNDropHandler::mouseReleased( MouseCode /*code*/, Panel* /*panel*/ )
{
	m_bDragging = false;
	m_pPanel->setDragged( m_bDragging );
	App::getInstance()->setMouseCapture( null );
}

DragNDropPanel::DragNDropPanel( int x, int y, int wide, int tall )
	: Panel( x, y, wide, tall )
{
	m_bBeingDragged = false;

	// Create the Drag Handler
	addInputSignal( new CDragNDropHandler( this ) );

	// Create the border (for dragging)
	m_pBorder = new LineBorder();
}
