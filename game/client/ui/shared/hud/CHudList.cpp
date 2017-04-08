#include "hud.h"
#include "cl_util.h"

#include "CHudElement.h"

#if USE_VGUI2
#include <KeyValues.h>
#include <vgui_controls/Panel.h>
#include "vgui2/VGUI2Paths.h"
#include "vgui2/CBaseViewport.h"
#endif

#include "CHudList.h"

CHudList::CHudList()
{
}

CHudList::~CHudList()
{
	//Thanks to Valve not making the engine call IClientVGUI::Shutdown for games other than CS & CZero, we can't clean up properly.
	//So instead we have to remove all elements only when the game hasn't shut down, because that means the instance is being destructed on dll unload.
	//We can't free panels anymore because the parents are gone, as is vgui2's library. - Solokiller
	//TODO: if Valve ever fixes this, destruct the Hud in CClientVGUI::Shutdown.
	if( !m_bGameShutdown )
	{
		RemoveAllElements();
	}
}

void CHudList::InitHudElements()
{
	ForEachHudElem( &CHudElement::Init );

#if USE_VGUI2
	KeyValues *kv = new KeyValues( "layout" );
	if( kv )
	{
		if( kv->LoadFromFile( g_pFileSystem, UI_HUDLAYOUT_FILENAME ) )
		{
			int numelements = m_Elements.Size();

			for( int i = 0; i < numelements; i++ )
			{
				CHudElement *element = m_Elements[ i ];

				vgui2::Panel *pPanel = dynamic_cast<vgui2::Panel*>( element );
				if( !pPanel )
				{
					Msg( "Non-vgui hud element %s\n", m_Elements[ i ]->GetName() );
					continue;
				}

				KeyValues *key = kv->FindKey( pPanel->GetName(), false );
				if( !key )
				{
					Msg( "Hud element '%s' doesn't have an entry '%s' in scripts/HudLayout.res\n", m_Elements[ i ]->GetName(), pPanel->GetName() );
				}

				// Note:  When a panel is parented to the module root, it's "parent" is returned as NULL.
				if( /*!element->IsParentedToClientDLLRootPanel() &&*/
					!pPanel->GetParent() )
				{
					Msg( "Hud element '%s'/'%s' doesn't have a parent\n", m_Elements[ i ]->GetName(), pPanel->GetName() );
				}
			}
		}

		kv->deleteThis();
	}

	g_pViewport->LoadControlSettings( UI_HUDLAYOUT_FILENAME );
	g_pViewport->InvalidateLayout( true, true );
#endif
}

void CHudList::GameShutdown()
{
	m_bGameShutdown = true;
}

int CHudList::GetElementCount() const
{
	return m_Elements.Count();
}

CHudElement* CHudList::GetElementByIndex( int iIndex )
{
	ASSERT( iIndex >= 0 && iIndex < m_Elements.Count() );

	if( iIndex < 0 || iIndex >= m_Elements.Count() )
		return nullptr;

	return m_Elements[ iIndex ];
}

CHudElement* CHudList::GetElementByName( const char* const pszName )
{
	ASSERT( pszName );

	auto count = m_Elements.Count();

	for( decltype( count ) iIndex = 0; iIndex < count; ++iIndex )
	{
		if( Q_strcmp( m_Elements[ iIndex ]->GetName(), pszName ) == 0 )
			return m_Elements[ iIndex ];
	}

	return nullptr;
}

bool CHudList::AddElement( CHudElement* pElement )
{
	if( !pElement )
		return false;

	if( GetElementByName( pElement->GetName() ) )
	{
		Con_Printf( "CHudList::AddElement: Attempted to add element '%s' multiple times.\n", pElement->GetName() );
		return false;
	}

	m_Elements.AddToTail( pElement );

	return true;
}

void CHudList::RemoveElement( CHudElement* pElement, const bool bDelete )
{
	if( !pElement )
		return;

	auto count = m_Elements.Count();

	for( decltype( count ) iIndex = 0; iIndex < count; ++iIndex )
	{
		if( m_Elements[ iIndex ] == pElement )
		{
			m_Elements.Remove( iIndex );

			if( bDelete )
				delete pElement;

			return;
		}
	}
}

void CHudList::RemoveAllElements( const bool bDelete )
{
	if( bDelete )
		m_Elements.PurgeAndDeleteElements();
	else
		m_Elements.Purge();
}
