#include <xercesc/dom/DOMError.hpp>

#include "extdll.h"
#include "util.h"

#include "CDOMPrintErrorHandler.h"
#include "CStrX.h"

namespace xml
{
bool CDOMPrintErrorHandler::handleError( const xercesc::DOMError& domError )
{
	// Display whatever error message passed from the serializer
	if( domError.getSeverity() == xercesc::DOMError::DOM_SEVERITY_WARNING )
		Con_Printf( "\nWarning Message: " );
	else if( domError.getSeverity() == xercesc::DOMError::DOM_SEVERITY_ERROR )
		Con_Printf( "\nError Message: " );
	else
		Con_Printf( "\nFatal Message: " );

	Con_Printf( "%s\n", CStrX( domError.getMessage() ).LocalForm() );

	// Instructs the serializer to continue serialization if possible.
	return true;
}
}
