#include <cstdio>
#include <memory>

#include <Angelscript/CASModule.h>
#include <Angelscript/wrapper/ASCallable.h>

#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "Angelscript/CASPluginData.h"

#include "Angelscript/CASClassWriter.h"

#include "StringUtils.h"

#include "ASCustomEntitiesUtils.h"

#include "IASCustomEntity.h"

#include "CCustomCBaseEntity.h"

#include "CASCustomEntities.h"

void ServerCommand_OutputBaseClasses()
{
	if( CMD_ARGC() != 2 )
	{
		Alert( at_console, "as_outputbaseclasses usage: as_outputbaseclasses <filename including extension>\n" );
		return;
	}

	g_CustomEntities.OutputBaseClassesToFile( CMD_ARGV( 1 ) );
}

CASCustomEntities g_CustomEntities;

bool CASCustomEntities::Initialize()
{
	g_engfuncs.pfnAddServerCommand( "as_outputbaseclasses", &::ServerCommand_OutputBaseClasses );

	GenerateBaseClasses();

	return true;
}

void CASCustomEntities::Shutdown()
{
	m_BaseClasses.clear();
	m_BaseClasses.shrink_to_fit();
}

void CASCustomEntities::WorldCreated()
{
	m_bAllowRegistration = true;
}

void CASCustomEntities::WorldActivated()
{
	m_bAllowRegistration = false;
}

void CASCustomEntities::WorldEnded()
{
	m_ClassList.clear();
	m_ClassList.shrink_to_fit();
}

const BaseClassData_t* CASCustomEntities::FindBaseClassByClassName( const char* const pszClassName ) const
{
	for( const auto& baseClass : m_BaseClasses )
	{
		if( baseClass.szClassName == pszClassName )
			return &baseClass;
	}

	return nullptr;
}

const CCustomEntityClass* CASCustomEntities::FindCustomClassByMapName( const char* const pszMapName ) const
{
	ASSERT( pszMapName );

	for( const auto& clazz : m_ClassList )
	{
		if( clazz->GetMapName() == pszMapName )
			return clazz.get();
	}

	return nullptr;
}

bool CASCustomEntities::RegisterCustomEntity( const std::string& szMapName, const std::string& szClassName )
{
	std::string szNewMapName = szMapName;

	Trim( szNewMapName );

	if( szNewMapName.empty() )
	{
		Alert( at_error, "CCustomEntities::RegisterCustomEntity: Empty map name given!\n" );
		return false;
	}

	std::string szNewClassName = szClassName;

	Trim( szNewClassName );

	if( szNewClassName.empty() )
	{
		Alert( at_error, "CCustomEntities::RegisterCustomEntity: Empty class name given!\n" );
		return false;
	}

	if( !m_bAllowRegistration )
	{
		Alert( at_error, "CCustomEntities::RegisterCustomEntity: Not allowed to register a custom entity at this time!\n" );
		return false;
	}

	if( FindCustomClassByMapName( szNewMapName.c_str() ) )
	{
		Alert( at_warning, "CCustomEntities::RegisterCustomEntity: Attempted to register a custom entity with the same map name \"%s\" twice!\n", szNewMapName.c_str() );
		return false;
	}

	auto pModule = GetModuleFromScriptContext( asGetActiveContext() );

	if( !pModule )
	{
		Alert( at_error, "CCustomEntities::RegisterCustomEntity: Couldn't get module from current script!\n" );
		return false;
	}

	auto pScriptModule = pModule->GetModule();

	if( !pScriptModule )
	{
		Alert( at_error, "CCustomEntities::RegisterCustomEntity: Couldn't get module from current script!\n" );
		return false;
	}

	const auto szNamespace = as::ExtractNamespaceFromName( szClassName );
	const auto szName = as::ExtractNameFromName( szClassName );

	const std::string szOldNS = pScriptModule->GetDefaultNamespace();
	pScriptModule->SetDefaultNamespace( szNamespace.c_str() );

	auto pTypeInfo = pScriptModule->GetTypeInfoByName( szName.c_str() );

	pScriptModule->SetDefaultNamespace( szOldNS.c_str() );

	if( !pTypeInfo )
	{
		Alert( at_error, "CCustomEntities::RegisterCustomEntity: Couldn't find class \"%s\" in module \"%s\"!\n", szNewClassName.c_str(), pScriptModule->GetName() );
		return false;
	}

	if( !( pTypeInfo->GetTypeId() & asTYPEID_MASK_OBJECT ) )
	{
		Alert( at_error, "CCustomEntities::RegisterCustomEntity: Type \"%s\" in module \"%s\" is not a class!\n", szNewClassName.c_str(), pScriptModule->GetName() );
		return false;
	}

	const BaseClassData_t* pBaseClassData = nullptr;

	auto pBase = pTypeInfo;

	//Walk up the inheritance tree, checking to see if the base class is one of the custom entity base classes.
	//If it doesn't match, this is not a custom entity class.
	do
	{
		pBase = pBase->GetBaseType();

		if( !pBase )
		{
			Alert( at_error, "CCustomEntities::RegisterCustomEntity: Class \"%s\" in module \"%s\" does not have a custom entity base class!\n", szNewClassName.c_str(), pScriptModule->GetName() );
			return false;
		}

		//Check to see if it's in a namespace.
		//This could cause crashes later if it's not checked.
		if( !( *pBase->GetNamespace() ) )
			pBaseClassData = FindBaseClassByClassName( pBase->GetName() );
	}
	while( !pBaseClassData );

	Alert( at_console, "Registering custom entity \"%s\" (class \"%s\")\n", szNewMapName.c_str(), szNewClassName.c_str() );

	if( auto pData = CASModule_GetPluginData( pModule ) )
	{
		//Promote the lifetime to MAP if needed.
		if( pData->GetLifetime() < PluginLifetime::MAP )
		{
			Alert( at_console, "Promoting plugin \"%s\" lifetime from %s to %s: Custom entities require at least %s lifetime\n", 
				   pModule->GetModuleName(), PluginLifetimeToString( pData->GetLifetime() ), 
				   PluginLifetimeToString( PluginLifetime::MAP ), PluginLifetimeToString( PluginLifetime::MAP ) );
			pData->SetMinimumLifetime( PluginLifetime::MAP );
		}
	}

	m_ClassList.emplace_back( std::make_unique<CCustomEntityClass>( std::move( szNewMapName ), pTypeInfo, *pBaseClassData ) );

	return true;
}

bool CASCustomEntities::UnregisterCustomEntity( const std::string& szMapName, const std::string& szClassName )
{
	const auto szNamespace = as::ExtractNamespaceFromName( szClassName );
	const auto szName = as::ExtractNameFromName( szClassName );

	for( auto it = m_ClassList.begin(), end = m_ClassList.end(); it != end; ++it )
	{
		const auto& clazz = *it;

		if( clazz->GetMapName() == szMapName && 
			clazz->GetTypeInfo()->GetNamespace() == szNamespace && 
			clazz->GetTypeInfo()->GetName() == szName )
		{
			Alert( at_console, "Unregistering custom entity \"%s\" (class \"%s\")\n", szMapName.c_str(), szClassName.c_str() );

			m_ClassList.erase( it );

			return true;
		}
	}

	return false;
}

CBaseEntity* CASCustomEntities::CreateCustomEntity( const std::string& szMapName )
{
	return CreateCustomEntity( STRING( ALLOC_STRING( szMapName.c_str() ) ) );
}

CBaseEntity* CASCustomEntities::CreateCustomEntity( const char* const pszMapName )
{
	ASSERT( pszMapName );

	auto pEdict = CREATE_ENTITY();

	if( !pEdict )
	{
		Alert( at_warning, "CASCustomEntities::CreateCustomEntity: Couldn't create edict instance for \"%s\"\n", pszMapName );
		return nullptr;
	}

	return CreateCustomEntity( pszMapName, pEdict );
}

CBaseEntity* CASCustomEntities::CreateCustomEntity( const char* const pszMapName, edict_t* pEdict )
{
	auto pEntvars = &pEdict->v;

	auto pCustomClass = g_CustomEntities.FindCustomClassByMapName( pszMapName );

	if( !pCustomClass )
	{
		Alert( at_error, "Custom entity creation: Class \"%s\" does not exist\n", pszMapName );
		g_engfuncs.pfnRemoveEntity( pEdict );
		return nullptr;
	}

	const auto& typeInfo = pCustomClass->GetTypeInfo();

	auto pInstance = reinterpret_cast<asIScriptObject*>( typeInfo->GetEngine()->CreateScriptObject( typeInfo.Get() ) );

	if( !pInstance )
	{
		Alert( at_error, "Custom entity creation: Couldn't instance class \"%s\"\n", typeInfo->GetName() );
		g_engfuncs.pfnRemoveEntity( pEdict );
		return nullptr;
	}

	pCustomClass->GetBaseClassData().createFn( pEntvars );

	auto pCPPInstance = GET_PRIVATE( pEdict );

	if( !pCPPInstance )
	{
		Alert( at_error, "Custom entity creation: Couldn't instance C++ class for custom entity \"%s\"\n", pszMapName );
		g_engfuncs.pfnRemoveEntity( pEdict );
		typeInfo->GetEngine()->ReleaseScriptObject( pInstance, typeInfo.Get() );
		return nullptr;
	}

	//Note: the string has to be ALLOC_STRING'd or a static string.
	pCPPInstance->SetClassname( pszMapName );

	auto pCustom = dynamic_cast<IASCustomEntity*>( pCPPInstance );

	if( !pCustom )
	{
		Alert( at_error, "Custom entity creation: C++ class for custom entity \"%s\" is not a custom entity handler\n", pszMapName );
		UTIL_RemoveNow( pCPPInstance );
		typeInfo->GetEngine()->ReleaseScriptObject( pInstance, typeInfo.Get() );
		return nullptr;
	}

	//The C++ class is now responsible for the instance's lifetime.
	//Note: must transfer ownership here since CreateScriptObject added a reference for us.
	pCustom->SetObject( CASObjPtr( pInstance, typeInfo, true ) );

	pCustom->SetClass( pCustomClass );

	if( !CallInitMethod( *typeInfo, pInstance, pCPPInstance, ( std::string( "void SetSelf(" ) + pCustomClass->GetBaseClassData().szCPPClassName + "@)" ).c_str() ) )
	{
		UTIL_RemoveNow( pCPPInstance );
		return nullptr;
	}

	if( !CallInitMethod( *typeInfo, pInstance, pCPPInstance, ( std::string( "void SetBaseClass(" ) + pCustomClass->GetBaseClassData().szBaseClassName + "@)" ).c_str() ) )
	{
		UTIL_RemoveNow( pCPPInstance );
		return nullptr;
	}

	//Note: pCustom is required here since the CBaseEntity pointer isn't adjusted for the class's vtable.
	if( !CallInitMethod( *typeInfo, pInstance, pCustom, "void SetCallbackHandler(CCallbackHandler@)" ) )
	{
		UTIL_RemoveNow( pCPPInstance );
		return nullptr;
	}

	if( !pCustom->OnScriptClassReady() )
	{
		Alert( at_error, "Custom entity creation: Class \"%s\" failed post initialization\n", typeInfo->GetName() );
		UTIL_RemoveNow( pCPPInstance );
		return nullptr;
	}

	pCPPInstance->OnCreate();

	return pCPPInstance;
}

void CASCustomEntities::OutputBaseClassesToFile( const char* const pszFileName ) const
{
	ASSERT( pszFileName );

	char szGameDir[ MAX_PATH ];

	if( !UTIL_GetGameDir( szGameDir, sizeof( szGameDir ) ) )
	{
		Alert( at_error, "CASCustomEntities::OutputBaseClassesToFile: Couldn't get game directory!\n" );
		return;
	}

	char szPath[ MAX_PATH ];

	const int iResult = snprintf( szPath, sizeof( szPath ), "%s/%s", szGameDir, pszFileName );

	if( !PrintfSuccess( iResult, sizeof( szPath ) ) )
	{
		Alert( at_error, "CASCustomEntities::OutputBaseClassesToFile: Failed to format file path!\n" );
		return;
	}

	std::unique_ptr<FILE, int ( * )( FILE* )> file( fopen( szPath, "w" ), fclose );

	if( !file )
	{
		Alert( at_error, "CASCustomEntities::OutputBaseClassesToFile: Couldn't open file \"%s\"!\n", szPath );
		return;
	}

	for( const auto& baseClass : m_BaseClasses )
	{
		fprintf( file.get(), "%s", baseClass.szClassDeclaration.c_str() );
	}

	Alert( at_console, "Wrote base classes to file \"%s\"\n", szPath );
}

void CASCustomEntities::GenerateBaseClasses()
{
	{
		CASBaseClassCreator creator( "CCustomCBaseEntity", "CBaseEntity", "BaseEntity", &::CreateCustomEntityClass<CCustomCBaseEntity>, m_BaseClasses );
	}
}

bool CASCustomEntities::CallInitMethod( const asITypeInfo& typeInfo, void* pInstance, void* pCPPInstance, const char* const pszMethod )
{
	bool bSuccess = false;

	if( auto pFunction = typeInfo.GetMethodByDecl( pszMethod ) )
	{
		bSuccess = as::Call( pInstance, pFunction, pCPPInstance );
	}
	else
	{
		Alert( at_error, "Custom entity creation: Class \"%s::%s\" does not have a required method \"%s\"\n", typeInfo.GetNamespace(), typeInfo.GetName(), pszMethod );
	}

	if( !bSuccess )
	{
		Alert( at_error, "Custom entity creation: Class \"%s::%s\" failed to call \"%s\"\n", typeInfo.GetNamespace(), typeInfo.GetName(), pszMethod );
	}

	return bSuccess;
}
