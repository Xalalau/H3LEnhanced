#include <cstring>
#include <cstdlib>
#include <string>

#include <angelscript.h>

#include "ASString.h"

static void RegisterScriptStringExtensions( asIScriptEngine& /*engine*/ )
{
	//const char* const pszObjectName = "string";

	//Additional features for the string class can be added here. - Solokiller
}

static long ASstrtol( const std::string& szString, int iRadix )
{
	return strtol( szString.c_str(), nullptr, iRadix );
}

static long ASstrtol( const std::string& szString, unsigned int& uiEndIndex, int iRadix )
{
	char* pszEnd;

	const auto result = strtol( szString.c_str(), &pszEnd, iRadix );

	//uiEndIndex is the index of the first character after the number.
	uiEndIndex = ( pszEnd - szString.c_str() );

	return result;
}

static unsigned long ASstrtoul( const std::string& szString, int iRadix )
{
	return strtoul( szString.c_str(), nullptr, iRadix );
}

static unsigned long ASstrtoul( const std::string& szString, unsigned int& uiEndIndex, int iRadix )
{
	char* pszEnd;

	const auto result = strtoul( szString.c_str(), &pszEnd, iRadix );

	//uiEndIndex is the index of the first character after the number.
	uiEndIndex = ( pszEnd - szString.c_str() );

	return result;
}

static long long ASstrtoll( const std::string& szString, int iRadix )
{
	return strtoll( szString.c_str(), nullptr, iRadix );
}

static long long ASstrtoll( const std::string& szString, unsigned int& uiEndIndex, int iRadix )
{
	char* pszEnd;

	const auto result = strtoll( szString.c_str(), &pszEnd, iRadix );

	//uiEndIndex is the index of the first character after the number.
	uiEndIndex = ( pszEnd - szString.c_str() );

	return result;
}

static unsigned long long ASstrtoull( const std::string& szString, int iRadix )
{
	return strtoull( szString.c_str(), nullptr, iRadix );
}

static unsigned long long ASstrtoull( const std::string& szString, unsigned int& uiEndIndex, int iRadix )
{
	char* pszEnd;

	const auto result = strtoull( szString.c_str(), &pszEnd, iRadix );

	//uiEndIndex is the index of the first character after the number.
	uiEndIndex = ( pszEnd - szString.c_str() );

	return result;
}

static double ASstrtod( const std::string& szString )
{
	return strtod( szString.c_str(), nullptr );
}

static double ASstrtod( const std::string& szString, unsigned int& uiEndIndex )
{
	char* pszEnd;

	const auto result = strtod( szString.c_str(), &pszEnd );

	//uiEndIndex is the index of the first character after the number.
	uiEndIndex = ( pszEnd - szString.c_str() );

	return result;
}

static int ASatoi( const std::string& szString )
{
	return atoi( szString.c_str() );
}

static double ASatof( const std::string& szString )
{
	return atof( szString.c_str() );
}

void RegisterScriptStringUtils( asIScriptEngine& engine )
{
	RegisterScriptStringExtensions( engine );

	engine.RegisterGlobalFunction(
		"int strtol(const string& in szString, int iRadix)",
		asFUNCTIONPR( ASstrtol, ( const std::string&, int ), long ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"int strtol(const string& in szString, uint& out uiEndIndex, int iRadix)",
		asFUNCTIONPR( ASstrtol, ( const std::string&, unsigned int&, int ), long ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"uint strtoul(const string& in szString)",
		asFUNCTIONPR( ASstrtoul, ( const std::string&, int ), unsigned long ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"uint strtoul(const string& in szString, uint& out uiEndIndex, int iRadix)",
		asFUNCTIONPR( ASstrtoul, ( const std::string&, unsigned int&, int ), unsigned long ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"int strtoll(const string& in szString, int iRadix)",
		asFUNCTIONPR( ASstrtoll, ( const std::string&, int ), long long ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"int strtoll(const string& in szString, uint& out uiEndIndex, int iRadix)",
		asFUNCTIONPR( ASstrtoll, ( const std::string&, unsigned int&, int ), long long ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"uint strtoull(const string& in szString)",
		asFUNCTIONPR( ASstrtoull, ( const std::string&, int ), unsigned long long ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"uint strtoull(const string& in szString, uint& out uiEndIndex, int iRadix)",
		asFUNCTIONPR( ASstrtoull, ( const std::string&, unsigned int&, int ), unsigned long long ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"double strtod(const string& in szString)",
		asFUNCTIONPR( ASstrtod, ( const std::string& ), double ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"double strtod(const string& in szString, uint& out uiEndIndex)",
		asFUNCTIONPR( ASstrtod, ( const std::string&, unsigned int& ), double ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"int atoi(const string& in szString)",
		asFUNCTION( ASatoi ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"double atof(const string& in szString)",
		asFUNCTION( ASatof ), asCALL_CDECL );
}
