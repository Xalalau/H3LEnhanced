//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "hud.h"
#include "cl_util.h"

#include "vgui_TeamFortressViewport.h"

#include "CImageLabel.h"

//GCC complains about deleting vgui objects due to them having no virtual destructor - Solokiller
#ifdef POSIX
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif

using namespace vgui;

CImageLabel::CImageLabel( const char* pImageName, int x, int y ) : Label( "", x, y )
{
	setContentFitted( true );
	m_pTGA = LoadTGAForRes( pImageName );
	setImage( m_pTGA );
}

CImageLabel::CImageLabel( const char* pImageName, int x, int y, int wide, int tall ) : Label( "", x, y, wide, tall )
{
	setContentFitted( true );
	m_pTGA = LoadTGAForRes( pImageName );
	setImage( m_pTGA );
}

//===========================================================
// Image size
int CImageLabel::getImageWide( void )
{
	if( m_pTGA )
	{
		int iXSize, iYSize;
		m_pTGA->getSize( iXSize, iYSize );
		return iXSize;
	}
	else
	{
		return 1;
	}
}

int CImageLabel::getImageTall( void )
{
	if( m_pTGA )
	{
		int iXSize, iYSize;
		m_pTGA->getSize( iXSize, iYSize );
		return iYSize;
	}
	else
	{
		return 1;
	}
}

void CImageLabel::LoadImage( const char * pImageName )
{
	if( m_pTGA )
		delete m_pTGA;

	// Load the Image
	m_pTGA = LoadTGAForRes( pImageName );

	if( m_pTGA == NULL )
	{
		// we didn't find a matching image file for this resolution
		// try to load file resolution independent

		char sz[ 256 ];
		sprintf( sz, "%s/%s", gEngfuncs.pfnGetGameDirectory(), pImageName );
		FileInputStream* fis = new FileInputStream( sz, false );
		m_pTGA = new BitmapTGA( fis, true );
		fis->close();
	}

	if( m_pTGA == NULL )
		return;	// unable to load image

	int w, t;

	m_pTGA->getSize( w, t );

	setSize( XRES( w ), YRES( t ) );
	setImage( m_pTGA );
}

#ifdef POSIX
#pragma GCC diagnostic pop
#endif
