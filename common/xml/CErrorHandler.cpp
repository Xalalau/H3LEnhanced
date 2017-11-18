#include <xercesc/sax/SAXParseException.hpp>

#include "extdll.h"
#include "util.h"

#include "CErrorHandler.h"
#include "CStrX.h"

namespace xml
{
void CErrorHandler::warning( const xercesc::SAXParseException& toCatch )
{
	Con_Printf( "Warning: %s\n,", CStrX( toCatch.getMessage() ).LocalForm() );
}

void CErrorHandler::error( const xercesc::SAXParseException& toCatch )
{
	Con_Printf( "Error: %s\n", CStrX( toCatch.getMessage() ).LocalForm() );

	m_bSawErrors = true;
}

void CErrorHandler::fatalError( const xercesc::SAXParseException& toCatch )
{
	Con_Printf( "Fatal error: %s\n", CStrX( toCatch.getMessage() ).LocalForm() );

	m_bSawErrors = true;
}

void CErrorHandler::resetErrors()
{
	m_bSawErrors = false;
}
}
