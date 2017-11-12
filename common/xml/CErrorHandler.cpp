#include <iostream>

#include <xercesc/sax/SAXParseException.hpp>

#include "CErrorHandler.h"
#include "CStrX.h"

namespace xml
{
void CErrorHandler::warning( const xercesc::SAXParseException& toCatch )
{
	std::cout << "Warning: " << CStrX( toCatch.getMessage() ).LocalForm() << std::endl;
}

void CErrorHandler::error( const xercesc::SAXParseException& toCatch )
{
	std::cout << "Error: " << CStrX( toCatch.getMessage() ).LocalForm() << std::endl;

	m_bSawErrors = true;
}

void CErrorHandler::fatalError( const xercesc::SAXParseException& toCatch )
{
	std::cout << "Fatal error: " << CStrX( toCatch.getMessage() ).LocalForm() << std::endl;

	m_bSawErrors = true;
}

void CErrorHandler::resetErrors()
{
	m_bSawErrors = false;
}
}
