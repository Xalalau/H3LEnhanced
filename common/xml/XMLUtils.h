#ifndef COMMON_XML_XMLUTILS_H
#define COMMON_XML_XMLUTILS_H

#include <array>
#include <cstddef>
#include <memory>

#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_BEGIN

class DOMNode;

XERCES_CPP_NAMESPACE_END

namespace xml
{
/**
*	@brief Deleter that calls release() on Xerces instances
*/
template<typename T>
struct XMLDeleter
{
public:

	void operator()( T* pPtr )
	{
		pPtr->release();
	}
};

/**
*	@brief Unique pointer specialized for Xerces types that require calling of release() to destroy instances
*/
template<typename T>
using unique_xml_ptr = std::unique_ptr<T, XMLDeleter<T>>;

/**
*	@brief Default maximum for stack buffers used to transcode strings
*/
const size_t DEFAULT_MAX_TRANSCODE = 2048;

/**
*	@brief Gets a named item from a DOMNamedNodeMap using a stack allocated buffer to convert a native name to XMLCh
*	More efficient than dynamically allocating strings, names are usually small
*	@param map Map to get item from
*	@param pszName Name of item to retrieve
*	@param[ out ] pOutNode If found this is the node, otherwise null
*	@return Whether transcode succeeded
*	@tparam MAX Maximum number of characters to convert
*/
template<size_t MAX = DEFAULT_MAX_TRANSCODE>
inline bool GetNamedItem( const xercesc::DOMNamedNodeMap& map, const char* pszName, xercesc::DOMNode*& pOutNode )
{
	std::array<XMLCh, MAX + 1> buffer;

	if( xercesc::XMLString::transcode( pszName, buffer.data(), MAX ) )
	{
		pOutNode = map.getNamedItem( buffer.data() );

		return true;
	}

	pOutNode = nullptr;

	return false;
}
}

#endif //COMMON_XML_XMLUTILS_H
