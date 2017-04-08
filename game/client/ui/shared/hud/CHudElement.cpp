#include "hud.h"
#include "cl_util.h"

#include "CHudElement.h"

CHudElement::CHudElement( const char* const pszName )
	: m_pszName( pszName )
{
	ASSERT( pszName );
}

CHudElement::~CHudElement()
{
}
