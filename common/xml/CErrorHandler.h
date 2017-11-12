#ifndef COMMON_XML_CERRORHANDLER_H
#define COMMON_XML_CERRORHANDLER_H

#include <xercesc/sax/ErrorHandler.hpp>

namespace xml
{
class CErrorHandler : public xercesc::ErrorHandler
{
public:
	CErrorHandler() = default;
	~CErrorHandler() = default;

	void warning( const xercesc::SAXParseException& toCatch ) override;
	void error( const xercesc::SAXParseException& toCatch ) override;
	void fatalError( const xercesc::SAXParseException& toCatch ) override;
	void resetErrors() override;

	bool GetSawErrors() const;

private:
	bool m_bSawErrors = false;
};

inline bool CErrorHandler::GetSawErrors() const
{
	return m_bSawErrors;
}
}

#endif //COMMON_XML_CERRORHANDLER_H
