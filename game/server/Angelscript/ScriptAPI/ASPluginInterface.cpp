#include <string>

#include <angelscript.h>

#include <Angelscript/CASModule.h>

#include "Angelscript/HLASConstants.h"

#include "Angelscript/ASPluginConstants.h"
#include "Angelscript/CASPluginData.h"

#include "ASPluginInterface.h"

static PluginLifetime ASPluginLifeTimeFromString( const std::string& szString )
{
	return PluginLifetimeFromString( szString.c_str() );
}

static std::string ASPluginLifetimeToString( const PluginLifetime lifetime )
{
	return PluginLifetimeToString( lifetime );
}

/**
*	Registers PluginLifetime.
*	@param engine Script engine.
*/
static void RegisterScriptPluginLifetime( asIScriptEngine& engine )
{
	const char* const pszObjectName = "PluginLifetime";

	const std::string szOldNS = engine.GetDefaultNamespace();

	engine.SetDefaultNamespace( pszObjectName );

	engine.RegisterEnum( pszObjectName );

	engine.RegisterEnumValue( pszObjectName, "HOTRELOADABLE", static_cast<int>( PluginLifetime::HOTRELOADABLE ) );
	engine.RegisterEnumValue( pszObjectName, "MAP", static_cast<int>( PluginLifetime::MAP ) );
	engine.RegisterEnumValue( pszObjectName, "SERVER", static_cast<int>( PluginLifetime::SERVER ) );

	engine.RegisterGlobalFunction(
		"PluginLifetime FromString(const string& in szString)",
		asFUNCTION( ASPluginLifeTimeFromString ), asCALL_CDECL );

	engine.RegisterGlobalFunction(
		"string ToString(const PluginLifetime lifetime)",
		asFUNCTION( ASPluginLifetimeToString ), asCALL_CDECL );

	engine.SetDefaultNamespace( szOldNS.c_str() );
}

static CASPluginData* Plugin_GetPluginData()
{
	auto pModule = GetModuleFromScriptContext( asGetActiveContext() );

	if( !pModule )
		return nullptr;

	return CASModule_GetPluginData( pModule );
}

static std::string CPluginData_GetName( const CASPluginData* /*pThis*/ )
{
	auto pScriptModule = GetScriptModuleFromScriptContext( asGetActiveContext() );

	if( !pScriptModule )
		return "<Unknown>";

	return pScriptModule->GetName();
}

static void RegisterScriptCPluginData( asIScriptEngine& engine )
{
	const char* const pszObjectName = "CPluginData";

	engine.RegisterObjectType( pszObjectName, 0, asOBJ_REF | asOBJ_NOCOUNT );

	engine.RegisterObjectMethod(
		pszObjectName, "PluginLifetime::PluginLifetime GetLifetime() const",
		asMETHOD( CASPluginData, GetLifetime ), asCALL_THISCALL );

	engine.RegisterObjectMethod(
		pszObjectName, "void SetMinimumLifetime(const PluginLifetime::PluginLifetime lifetime)",
		asMETHOD( CASPluginData, SetMinimumLifetime ), asCALL_THISCALL );

	engine.RegisterObjectMethod(
		pszObjectName, "string GetName() const",
		asFUNCTION( CPluginData_GetName ), asCALL_CDECL_OBJFIRST );

	engine.RegisterGlobalFunction(
		"CPluginData@ get_PluginData()",
		asFUNCTION( Plugin_GetPluginData ), asCALL_CDECL );
}

void RegisterScriptPluginInterface( asIScriptEngine& engine )
{
	const auto oldAccess = engine.SetDefaultAccessMask( ModuleAccessMask::PLUGIN );

	RegisterScriptPluginLifetime( engine );
	RegisterScriptCPluginData( engine );

	engine.SetDefaultAccessMask( oldAccess );
}
