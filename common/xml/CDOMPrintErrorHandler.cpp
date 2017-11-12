#include <iostream>

#include <xercesc/dom/DOMError.hpp>

#include "CDOMPrintErrorHandler.h"
#include "CStrX.h"

namespace xml
{
bool CDOMPrintErrorHandler::handleError( const xercesc::DOMError& domError )
{
	// Display whatever error message passed from the serializer
	if( domError.getSeverity() == xercesc::DOMError::DOM_SEVERITY_WARNING )
		std::cerr << "\nWarning Message: ";
	else if( domError.getSeverity() == xercesc::DOMError::DOM_SEVERITY_ERROR )
		std::cerr << "\nError Message: ";
	else
		std::cerr << "\nFatal Message: ";

	std::cerr << CStrX( domError.getMessage() ).LocalForm() << std::endl;

	// Instructs the serializer to continue serialization if possible.
	return true;
}
}
