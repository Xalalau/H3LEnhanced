#include <cassert>
#include <memory>

#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>

#include <xercesc/framework/MemBufInputSource.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>

#include <xercesc/sax/InputSource.hpp>

#include <xercesc/util/IOException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>

#include "extdll.h"
#include "util.h"
#include "CFile.h"

#include "CConsoleFormatTarget.h"
#include "CDOMPrintErrorHandler.h"
#include "CErrorHandler.h"
#include "CGameFileFormatTarget.h"
#include "CStdOutFormatTarget.h"
#include "CStreamFormatTarget.h"
#include "CStrX.h"
#include "CXMLManager.h"

//TODO: should have additional logging using a separate cvar - Solokiller

namespace xml
{
namespace
{
CXMLManager g_XMLManager;
}

CXMLManager& XMLManager()
{
	return g_XMLManager;
}

CXMLManager::CXMLManager()
{
}

CXMLManager::~CXMLManager()
{
}

bool CXMLManager::Initialize()
{
	try
	{
		xercesc::XMLPlatformUtils::Initialize();

		m_bInitialized = true;

		m_Parser = std::make_unique<xercesc::XercesDOMParser>();

		m_ErrorHandler = std::make_unique<CErrorHandler>();

		m_Parser->setErrorHandler( m_ErrorHandler.get() );

		//Prevent external entity exploits
		//See https://www.owasp.org/index.php/XML_External_Entity_(XXE)_Prevention_Cheat_Sheet
		m_Parser->setCreateEntityReferenceNodes( false );

		m_PrintErrorHandler = std::make_unique<CDOMPrintErrorHandler>();

		const XMLCh tempStr[ 3 ] = { xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull };
		m_pDOMImpl = xercesc::DOMImplementationRegistry::getDOMImplementation( tempStr );

		assert( m_pDOMImpl );

		Alert( at_console, "Xerces-C initialized\n" );
	}
	catch( const xercesc::XMLException& e )
	{
		Alert( at_error, "Error initializing Xerces XML: %s\n", CStrX( e.getMessage() ).LocalForm() );
	}

	return m_bInitialized;
}

void CXMLManager::Shutdown()
{
	if( m_bInitialized )
	{
		m_pDOMImpl = nullptr;

		if( m_PrintErrorHandler )
			m_PrintErrorHandler.reset();

		if( m_Parser )
			m_Parser.reset();

		//Referenced by parser, delete after
		if( m_ErrorHandler )
			m_ErrorHandler.reset();

		xercesc::XMLPlatformUtils::Terminate();

		m_bInitialized = false;
	}
}

unique_xml_ptr<xercesc::DOMDocument> CXMLManager::ParseInputSource( const xercesc::InputSource& source )
{
	bool bErrorsOccurred = true;

	try
	{
		//TODO: all Xerces parsers except the DOM parser do this automatically. Could be a bug - Solokiller
		m_ErrorHandler->resetErrors();

		m_Parser->parse( source );

		bErrorsOccurred = false;
	}
	catch( const xercesc::OutOfMemoryException& )
	{
		Alert( at_error, "Ran out of memory while parsing XML file\n" );
	}
	catch( const xercesc::XMLException& e )
	{
		Alert( at_error, "Error parsing XML file: %s\n", CStrX( e.getMessage() ).LocalForm() );
	}
	catch( const xercesc::DOMException& e )
	{
		const unsigned int maxChars = 2047;
		XMLCh errText[ maxChars + 1 ];

		Alert( at_error, "\nDOM Error during parsing: '%s'\nDOMException code is:  %hd\n", CStrX( source.getSystemId() ).LocalForm(), e.code );

		if( xercesc::DOMImplementation::loadDOMExceptionMsg( e.code, errText, maxChars ) )
			Alert( at_error, "Message is: %s\n", CStrX( errText ).LocalForm() );
	}
	catch( ... )
	{
		Alert( at_error, "An error occurred during parsing\n " );
	}

	if( !bErrorsOccurred && !m_ErrorHandler->GetSawErrors() )
	{
		return unique_xml_ptr<xercesc::DOMDocument>{ m_Parser->adoptDocument() };
	}

	return nullptr;
}

unique_xml_ptr<xercesc::DOMDocument> CXMLManager::ParseBuffer( const char* pszBufId, const char* pszBuffer, const size_t uiBufferSize )
{
	assert( pszBufId );
	assert( pszBuffer );

	if( !pszBufId )
	{
		Alert( at_error, "NULL buffer id given to CXMLManager::ParseBuffer!" );
		return nullptr;
	}

	if( !pszBuffer )
	{
		Alert( at_error, "NULL buffer given to CXMLManager::ParseBuffer!" );
		return nullptr;
	}

	//This will likely crash if the buffer size doesn't match the actual buffer size
	if( uiBufferSize > 0 && pszBuffer[ ( uiBufferSize / sizeof( char ) ) - 1 ] != '\0' )
	{
		Alert( at_error, "Unterminated buffer given to CXMLManager::ParseBuffer!" );
		return nullptr;
	}

	return ParseInputSource( xercesc::MemBufInputSource{ reinterpret_cast<const XMLByte*>( pszBuffer ), uiBufferSize, pszBufId, false } );
}

unique_xml_ptr<xercesc::DOMDocument> CXMLManager::ParseFile( const char* pszBufId, FileHandle_t hFile )
{
	assert( pszBufId );
	assert( FILESYSTEM_INVALID_HANDLE != hFile );

	if( !pszBufId )
	{
		Alert( at_error, "NULL buffer id given to CXMLManager::ParseFile!" );
		return nullptr;
	}

	if( FILESYSTEM_INVALID_HANDLE == hFile )
	{
		Alert( at_error, "NULL file handle given to CXMLManager::ParseFile!" );
		return nullptr;
	}

	g_pFileSystem->Seek( hFile, 0, FILESYSTEM_SEEK_HEAD );

	const auto fileSize = g_pFileSystem->Size( hFile );

	//Space for null terminator
	auto buffer = std::make_unique<XMLByte[]>( fileSize + 1 );

	if( static_cast<decltype( fileSize )>( g_pFileSystem->Read( buffer.get(), fileSize, hFile ) ) != fileSize )
	{
		Alert( at_error, "Couldn't read file \"%s\" for XML parsing\n", pszBufId );
		return nullptr;
	}

	buffer[ fileSize ] = '\0';

	return ParseInputSource( xercesc::MemBufInputSource{ buffer.get(), fileSize + 1, pszBufId, false } );
}

unique_xml_ptr<xercesc::DOMDocument> CXMLManager::ParseFile( const char* pszFilename, const char* pszPathID )
{
	assert( pszFilename );

	if( !pszFilename )
	{
		Alert( at_error, "NULL filename given to CXMLManager::ParseFile!" );
		return nullptr;
	}

	CFile file{ pszFilename, "rb", pszPathID };

	if( !file.IsOpen() )
	{
		Alert( at_error, "Couldn't open file \"%s\" for XML parsing\n", pszFilename );
		return nullptr;
	}

	return ParseFile( pszFilename, file.GetFileHandle() );
}

bool CXMLManager::WriteToFormTarget( const xercesc::DOMDocument& document, xercesc::XMLFormatTarget& formTarget )
{
	try
	{
		// get a serializer, an instance of DOMLSSerializer
		unique_xml_ptr<xercesc::DOMLSSerializer> serializer{ m_pDOMImpl->createLSSerializer() };
		unique_xml_ptr<xercesc::DOMLSOutput> outputDesc{ m_pDOMImpl->createLSOutput() };

		auto pSerializerConfig = serializer->getDomConfig();
		pSerializerConfig->setParameter( xercesc::XMLUni::fgDOMErrorHandler, m_PrintErrorHandler.get() );

		outputDesc->setByteStream( &formTarget );

		return serializer->write( &document, outputDesc.get() );
	}
	catch( const xercesc::OutOfMemoryException& )
	{
		Alert( at_error, "OutOfMemoryException" );
	}
	catch( const xercesc::DOMLSException& e )
	{
		Alert( at_error, "An error occurred during serialization of the DOM tree. Msg is:\n%s\n", CStrX( e.getMessage() ).LocalForm() );
	}
	catch( const xercesc::XMLException& e )
	{
		Alert( at_error, "An error occurred during creation of output transcoder. Msg is:\n%s\n", CStrX( e.getMessage() ).LocalForm() );
	}

	return false;
}

bool CXMLManager::WriteToStdOut( const xercesc::DOMDocument& document )
{
	CStdOutFormatTarget target{};
	return WriteToFormTarget( document, target );
}

bool CXMLManager::WriteToConsole( const xercesc::DOMDocument& document, const bool bDebug )
{
	CConsoleFormatTarget target{ bDebug };
	return WriteToFormTarget( document, target );
}

bool CXMLManager::WriteToStream( const xercesc::DOMDocument& document, std::ostream& stream )
{
	CStreamFormatTarget target{ stream };
	return WriteToFormTarget( document, target );
}

bool CXMLManager::WriteToFile( const xercesc::DOMDocument& document, const char* pszFilename, const char* pszPathID )
{
	try
	{
		CGameFileFormatTarget target{ g_pFileSystem, pszFilename, pszPathID };
		return WriteToFormTarget( document, target );
	}
	catch( const xercesc::IOException& e )
	{
		Alert( at_error, "\nError during writing: '%s'\nException message is:  %hd\n", pszFilename, CStrX( e.getMessage() ).LocalForm() );
	}

	return false;
}

bool CXMLManager::WriteToFile( const xercesc::DOMDocument& document, FileHandle_t hFile, const bool bAdoptHandle )
{
	try
	{
		CGameFileFormatTarget target{ g_pFileSystem, hFile, bAdoptHandle };
		return WriteToFormTarget( document, target );
	}
	catch( const xercesc::IOException& e )
	{
		Alert( at_error, "\nError during writing:\nException message is:  %hd\n", CStrX( e.getMessage() ).LocalForm() );
	}

	return false;
}
}
