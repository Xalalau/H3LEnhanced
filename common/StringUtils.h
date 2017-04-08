#ifndef COMMON_STRINGUTILS_H
#define COMMON_STRINGUTILS_H

#include <algorithm>
#include <cctype>
#include <cstring>
#include <functional>
#include <string>

#include "Platform.h"

/**
*	Checks if a printf operation was successful
*/
inline bool PrintfSuccess( const int iRet, const size_t uiBufferSize )
{
	return iRet >= 0 && static_cast<size_t>( iRet ) < uiBufferSize;
}

/**
*	String hashing function
*	Ripped from the MSVC std::hash<std::string> implementation
*/
inline size_t StringHash( const char* pszString )
{
	if( !pszString )
		pszString = "";

	size_t _Val = 2166136261U;
	size_t _First = 0;
	size_t _Last = strlen( pszString );
	size_t _Stride = 1 + _Last / 10;

	for( ; _First < _Last; _First += _Stride )
		_Val = 16777619U * _Val ^ ( size_t ) pszString[ _First ];
	return ( _Val );
}

/**
*	String hashing function
*	Ripped from the MSVC std::hash<std::string> implementation
*	Case insensitive
*/
inline size_t StringHashI( const char* pszString )
{
	if( !pszString )
		pszString = "";

	size_t _Val = 2166136261U;
	size_t _First = 0;
	size_t _Last = strlen( pszString );
	size_t _Stride = 1 + _Last / 10;

	for( ; _First < _Last; _First += _Stride )
		_Val = 16777619U * _Val ^ ( size_t ) tolower( pszString[ _First ] );
	return ( _Val );
}

/**
*	Functor for char* hashing.
*/
template<size_t ( *HASHER )( const char* )>
struct BaseRawCharHash : public std::unary_function<const char*, size_t>
{
	size_t operator()( const char* pszString ) const
	{
		return HASHER( pszString );
	}
};

typedef BaseRawCharHash<StringHash> RawCharHash;
typedef BaseRawCharHash<StringHashI> RawCharHashI;

template<int ( *COMPARE )( const char*, const char* )>
struct BaseRawCharEqualTo : public std::binary_function<const char*, const char*, bool>
{
	bool operator()( const char* pszLHS, const char* pszRHS ) const
	{
		return COMPARE( pszLHS, pszRHS ) == 0;
	}
};

typedef BaseRawCharEqualTo<strcmp> RawCharEqualTo;
typedef BaseRawCharEqualTo<stricmp> RawCharEqualToI;

/**
*	Case insensitive hash for std::string
*/
inline size_t StdStringHashI( const std::string& szString )
{
	return StringHashI( szString.c_str() );
}

/**
*	Functor for case insensitive std::string hashing.
*/
struct CStdStringHashI : public std::unary_function<const std::string&, size_t>
{
	size_t operator()( const std::string& szString ) const
	{
		return StdStringHashI( szString );
	}
};

/**
*	Functor for case insensitive std::string comparison.
*/
struct CStdStringEqualToI : public std::binary_function<const std::string&, const std::string&, bool>
{
	bool operator()( const std::string& szLHS, const std::string& szRHS ) const
	{
		return stricmp( szLHS.c_str(), szRHS.c_str() ) == 0;
	}
};

/**
*	Works like strstr, but the substring length is given.
*/
const char* UTIL_strnstr( const char* pszString, const char* pszSubString, const size_t uiLength );

/**
*	Works like strrstr, but the substring length is given.
*/
const char* UTIL_strnrstr( const char* pszString, const char* pszSubString, const size_t uiLength );

/**
*	Checks whether a token matches a string.
*	The token can have '*' characters to signal 0 or more characters that can span the space between given characters.
*	@param pszString String to match against.
*	@param pszToken Token to match.
*	@return Whether the token matches.
*/
bool UTIL_TokenMatches( const char* pszString, const char* pszToken );

/**
*	Fixes slashes to use the correct platform specific slashes.
*/
char* UTIL_FixSlashes( char* pszPath );

char* UTIL_SafeStrncpy( char* pszDest, const char* pszSource, const size_t uiSizeInCharacters );

/*
*	This code is based on the following Stack Overflow answer: http://stackoverflow.com/a/217605
*/
// trim from start
inline std::string& LTrim( std::string& s )
{
	s.erase( s.begin(), std::find_if( s.begin(), s.end(),
									  std::not1( std::ptr_fun<int, int>( std::isspace ) ) ) );
	return s;
}

// trim from end
inline std::string& RTrim( std::string& s )
{
	s.erase( std::find_if( s.rbegin(), s.rend(),
						   std::not1( std::ptr_fun<int, int>( std::isspace ) ) ).base(), s.end() );
	return s;
}

// trim from both ends
inline std::string& Trim( std::string& s )
{
	return LTrim( RTrim( s ) );
}

/*
*	End based on code.
*/

/**
*	If the given string ends with a newline character, the newline is removed.
*/
inline void UTIL_StripEndNewlineFromString( char* str )
{
	int s = strlen( str ) - 1;
	if( str[ s ] == '\n' || str[ s ] == '\r' )
		str[ s ] = '\0';
}

/**
*	Converts all '\r' characters to '\n', so that the engine can deal with them properly
*	@return A pointer to str
*/
inline char* UTIL_ConvertCRtoNL( char* str )
{
	for( char *ch = str; *ch != '\0'; ++ch )
	{
		if( *ch == '\r' )
			*ch = '\n';
	}

	return str;
}

#endif //COMMON_STRINGUTILS_H
