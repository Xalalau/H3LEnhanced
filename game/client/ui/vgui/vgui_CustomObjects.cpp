//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Contains implementation of various VGUI-derived objects
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#include "VGUI_Font.h"

#include "hud.h"
#include "cl_util.h"
#include "kbutton.h"
#include "cvardef.h"
#include "usercmd.h"
#include "const.h"
#include "mathlib.h"
#include "parsemsg.h"

#include "vgui_InputSignalHandlers.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_loadtga.h"

// Arrow filenames
const char* const sArrowFilenames[] =
{
	"arrowup",
	"arrowdn", 
	"arrowlt",
	"arrowrt", 
};

// Get the name of TGA file, without a gamedir
char *GetTGANameForRes(const char *pszName)
{
	int i;
	char sz[256]; 
	static char gd[256]; 
	if (ScreenWidth < 640)
		i = 320;
	else
		i = 640;
	snprintf(sz, sizeof( sz ), pszName, i);
	snprintf(gd, sizeof( gd ), "gfx/vgui/%s.tga", sz);
	return gd;
}

//-----------------------------------------------------------------------------
// Purpose: Loads a .tga file and returns a pointer to the VGUI tga object
//-----------------------------------------------------------------------------
BitmapTGA *LoadTGAForRes( const char* pImageName )
{
	BitmapTGA	*pTGA;

	char sz[256];
	sprintf(sz, "%%d_%s", pImageName);
	pTGA = vgui_LoadTGA(GetTGANameForRes(sz));

	return pTGA;
}

//===========================================================
int ClassButton::IsNotValid()
{
	// If this is the main ChangeClass button, remove it if the player's only able to be civilians
	if ( m_iPlayerClass == -1 )
	{
		if (gViewPort->GetValidClasses(g_iTeamNumber) == -1)
			return true;

		return false;
	}

	// Is it an illegal class?
#ifdef _TFC
	if ((gViewPort->GetValidClasses(0) & sTFValidClassInts[ m_iPlayerClass ]) || (gViewPort->GetValidClasses(g_iTeamNumber) & sTFValidClassInts[ m_iPlayerClass ]))
		return true;
#endif

	// Only check current class if they've got autokill on
	bool bAutoKill = CVAR_GET_FLOAT( "hud_classautokill" ) != 0;
	if ( bAutoKill )
	{	
		// Is it the player's current class?
		if ( 
#ifdef _TFC
			(gViewPort->IsRandomPC() && m_iPlayerClass == PC_RANDOM) || 
#endif
			(!gViewPort->IsRandomPC() && (m_iPlayerClass == g_iPlayerClass)) )
			return true;
	}

	return false;
}

//===========================================================
// Button with Class image beneath it

//=================================================================================
// CUSTOM SCROLLPANEL
//=================================================================================
CTFScrollButton::CTFScrollButton(int iArrow, const char* text,int x,int y,int wide,int tall) : CommandButton(text,x,y,wide,tall)
{
	// Set text color to orange
	setFgColor(Scheme::sc_primary1);

	// Load in the arrow
	m_pTGA = LoadTGAForRes( sArrowFilenames[iArrow] );
	setImage( m_pTGA );

	// Highlight signal
	InputSignal *pISignal = new CHandler_CommandButtonHighlight(this);
	addInputSignal(pISignal);
}

void CTFScrollButton::paint( void )
{
	if (!m_pTGA)
		return;

	// draw armed button text in white
	if ( isArmed() )
	{
		m_pTGA->setColor( vgui::Color(255,255,255, 0) );
	}
	else
	{
		m_pTGA->setColor( vgui::Color(255,255,255, 128) );
	}

	m_pTGA->doPaint(this);
}

void CTFScrollButton::paintBackground( void )
{
/*
	if ( isArmed() )
	{
		// Orange highlight background
		drawSetColor( Scheme::sc_primary2 );
		drawFilledRect(0,0,_size[0],_size[1]);
	}

	// Orange Border
	drawSetColor( Scheme::sc_secondary1 );
	drawOutlinedRect(0,0,_size[0]-1,_size[1]);
*/
}

void CTFSlider::paintBackground( void )
{
	int wide,tall,nobx,noby;
	getPaintSize(wide,tall);
	getNobPos(nobx,noby);

	// Border
	drawSetColor( Scheme::sc_secondary1 );
	drawOutlinedRect( 0,0,wide,tall );

	if( isVertical() )
	{
		// Nob Fill
		drawSetColor( Scheme::sc_primary2 );
		drawFilledRect( 0,nobx,wide,noby );

		// Nob Outline
		drawSetColor( Scheme::sc_primary1 );
		drawOutlinedRect( 0,nobx,wide,noby );
	}
	else
	{
		// Nob Fill
		drawSetColor( Scheme::sc_primary2 );
		drawFilledRect( nobx,0,noby,tall );

		// Nob Outline
		drawSetColor( Scheme::sc_primary1 );
		drawOutlinedRect( nobx,0,noby,tall );
	}
}

CTFScrollPanel::CTFScrollPanel(int x,int y,int wide,int tall) : ScrollPanel(x,y,wide,tall)
{
	ScrollBar *pScrollBar = getVerticalScrollBar();
	pScrollBar->setButton( new CTFScrollButton( ARROW_UP, "", 0,0,16,16 ), 0 );
	pScrollBar->setButton( new CTFScrollButton( ARROW_DOWN, "", 0,0,16,16 ), 1 );
	pScrollBar->setSlider( new CTFSlider(0,wide-1,wide,(tall-(wide*2))+2,true) ); 
	pScrollBar->setPaintBorderEnabled(false);
	pScrollBar->setPaintBackgroundEnabled(false);
	pScrollBar->setPaintEnabled(false);

	pScrollBar = getHorizontalScrollBar();
	pScrollBar->setButton( new CTFScrollButton( ARROW_LEFT, "", 0,0,16,16 ), 0 );
	pScrollBar->setButton( new CTFScrollButton( ARROW_RIGHT, "", 0,0,16,16 ), 1 );
	pScrollBar->setSlider( new CTFSlider(tall,0,wide-(tall*2),tall,false) );
	pScrollBar->setPaintBorderEnabled(false);
	pScrollBar->setPaintBackgroundEnabled(false);
	pScrollBar->setPaintEnabled(false);
}


//=================================================================================
// CUSTOM HANDLERS
//=================================================================================
void CMenuHandler_StringCommandClassSelect::actionPerformed(Panel* panel)
{
	CMenuHandler_StringCommand::actionPerformed( panel );

	// THIS IS NOW BEING DONE ON THE TFC SERVER TO AVOID KILLING SOMEONE THEN 
	// HAVE THE SERVER SAY "SORRY...YOU CAN'T BE THAT CLASS".

#if !defined _TFC
	bool bAutoKill = CVAR_GET_FLOAT( "hud_classautokill" ) != 0;
	if ( bAutoKill && g_iPlayerClass != 0 )
		gEngfuncs.pfnClientCmd("kill");
#endif
}

