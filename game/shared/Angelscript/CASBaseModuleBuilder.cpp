#include <algorithm>
#include <cassert>
#include <experimental/filesystem>
#include <memory>
#include <string>

#include <Angelscript/add_on/scriptbuilder.h>

#include "extdll.h"
#include "util.h"

#include "CFile.h"

#include "Angelscript/Entities/CASCustomEntities.h"

#include "CASBaseModuleBuilder.h"

namespace fs = std::experimental::filesystem;

CASBaseModuleBuilder::CASBaseModuleBuilder( std::string&& szBasePath, std::string&& szModuleTypeName )
	: m_szBasePath( std::move( szBasePath ) )
	, m_szModuleTypeName( std::move( szModuleTypeName ) )
{
	//TODO: need to add support to the client for predicted weapons. - Solokiller
#ifndef CLIENT_DLL
	//Add in the custom entity base classes.
	for( const auto baseClass : g_CustomEntities.GetBaseClassList() )
	{
		AddInternalScript( "__" + baseClass.szClassName, std::string( baseClass.szClassDeclaration ) );
	}
#endif
}

bool CASBaseModuleBuilder::HasInternalScript( const char* const pszName ) const
{
	assert( pszName );

	if( !pszName )
		return false;

	auto it = std::find_if( m_InternalScripts.begin(), m_InternalScripts.end(), 
				  [ = ]( const auto& script )
	{
		return script.first == pszName;
	} );

	return it != m_InternalScripts.end();
}

bool CASBaseModuleBuilder::AddInternalScript( std::string&& szName, std::string&& szContents )
{
	if( HasInternalScript( szName.c_str() ) )
	{
		return false;
	}

	m_InternalScripts.emplace_back( std::make_pair( std::move( szName ), std::move( szContents ) ) );

	return true;
}

bool CASBaseModuleBuilder::HasScript( const char* const pszName ) const
{
	assert( pszName );

	if( !pszName )
		return false;

	auto it = std::find_if( m_Scripts.begin(), m_Scripts.end(),
							[ = ]( const auto& script )
	{
		return script == pszName;
	} );

	return it != m_Scripts.end();
}

bool CASBaseModuleBuilder::AddScript( std::string&& szName )
{
	if( HasScript( szName.c_str() ) )
		return false;

	m_Scripts.emplace_back( std::move( szName ) );

	return true;
}

bool CASBaseModuleBuilder::DefineWords( CScriptBuilder& builder )
{
#ifdef CLIENT_DLL
	builder.DefineWord( "CLIENT_DLL" );
#else
	builder.DefineWord( "SERVER_DLL" );
#endif

	return true;
}

bool CASBaseModuleBuilder::AddScripts( CScriptBuilder& builder )
{
	for( auto& script : m_InternalScripts )
	{
		if( builder.AddSectionFromMemory( script.first.c_str(), script.second.c_str() ) < 0 )
		{
			Alert( at_console, "CASBaseModuleBuilder::AddScripts: Error adding internal script \"%s\"\n", script.first.c_str() );
			return false;
		}
	}

	char szRelativePath[ MAX_PATH ];

	for( auto& script : m_Scripts )
	{
		const int iResult = snprintf( szRelativePath, sizeof( szRelativePath ), "%s%s.as", m_szBasePath.c_str(), script.c_str() );

		if( iResult < 0 || static_cast<size_t>( iResult ) >= sizeof( szRelativePath ) )
		{
			Alert( at_console, "CASBaseModuleBuilder::AddScripts: Error formatting %s script \"%s\" path!\n", m_szModuleTypeName.c_str(), script.c_str() );
			return false;
		}

		char szAbsolutePath[ MAX_PATH ];

		if( g_pFileSystem->GetLocalPath( szRelativePath, szAbsolutePath, sizeof( szAbsolutePath ) ) )
		{
			if( builder.AddSectionFromFile( szAbsolutePath ) < 0 )
			{
				Alert( at_console, "CASBaseModuleBuilder::AddScripts: Error adding script \"%s\"!\n", szRelativePath );
				return false;
			}
		}
		else
		{
			Alert( at_console, "CASBaseModuleBuilder::AddScripts: Couldn't find %s script \"%s\"!\n", m_szModuleTypeName.c_str(), szRelativePath );
			return false;
		}
	}

	return true;
}

bool CASBaseModuleBuilder::IncludeScript( CScriptBuilder& builder, const char* const pszIncludeFileName, const char* const pszFromFileName )
{
	std::error_code error;

	fs::path path( pszFromFileName );

	path.remove_filename();

	path /= pszIncludeFileName;

	//TODO: define extension. - Solokiller
	path += ".as";

	path.make_preferred();

	//Convert the absolute path to a relative path.
	//CScriptBuilder converts relative paths to absolute, which is why we need to undo that.
	//This is pretty ugly, but there is nothing in the filesystem API to do this just yet.
	auto current = fs::current_path( error );

	if( error )
	{
		Alert( at_console, "CASBaseModuleBuilder::IncludeScript: Error composing path to include \"%s\" in \"%s\": error retrieving current path!\n", pszIncludeFileName, pszFromFileName );
		return false;
	}

	//Just to be safe, canonicalize current as well in case it was set to use relative paths or something.
	current = fs::canonical( current, current, error );

	if( error )
	{
		Alert( at_console, "CASBaseModuleBuilder::IncludeScript: Error composing path to include \"%s\" in \"%s\": error canonicalizing current path!\n", pszIncludeFileName, pszFromFileName );
		return false;
	}

	path = fs::canonical( path, current, error );

	if( error )
	{
		Alert( at_console, "CASBaseModuleBuilder::IncludeScript: Error composing path to include \"%s\" in \"%s\": error canonicalizing path!\n", pszIncludeFileName, pszFromFileName );
		return false;
	}

	auto it = path.begin();

	auto end = path.end();

	for( auto component : current )
	{
		//operator== is case sensitive so C:\ and c:\ are considered to be different.
		if( stricmp( it->u8string().c_str(), component.u8string().c_str() ) != 0 )
		{
			Alert( at_console, "CASBaseModuleBuilder::IncludeScript: Error composing path to include \"%s\" in \"%s\": current path differs!\n", pszIncludeFileName, pszFromFileName );
			return false;
		}

		if( it != end )
			++it;
	}

	//The current directory is the exe directory, but filesystem calls start in exedir/moddir, so advance once more.
	if( it != end )
		++it;

	fs::path relativePath;

	for( ; it != end; ++it )
	{
		relativePath /= *it;
	}

	const auto szRelativePath = relativePath.u8string();

	CFile file( szRelativePath.c_str(), "rb" );

	bool bSuccess = false;

	if( file.IsOpen() )
	{
		const auto size = file.Size();

		auto data = std::make_unique<char[]>( size + 1 );

		if( file.Read( data.get(), size ) == static_cast<int>( size ) )
		{
			const auto result = builder.AddSectionFromMemory( szRelativePath.c_str(), data.get(), size );

			if( result >= 0 )
			{
				if( result == 1 )
					Alert( at_console, "CASBaseModuleBuilder::IncludeScript: Included script \"%s\"\n", szRelativePath.c_str() );

				bSuccess = true;
			}
			else
			{
				Alert( at_console, "CASBaseModuleBuilder::IncludeScript: Error including script \"%s\" in \"%s\"!\n", szRelativePath.c_str(), pszFromFileName );
			}
		}
		else
		{
			Alert( at_console, "CASBaseModuleBuilder::IncludeScript: Error reading \"%s\" to include in \"%s\"!\n", szRelativePath.c_str(), pszFromFileName );
		}
	}
	else
	{
		Alert( at_console, "CASBaseModuleBuilder::IncludeScript: Couldn't open script \"%s\" to include in \"%s\"!\n", szRelativePath.c_str(), pszFromFileName );
	}

	return bSuccess;
}

bool CASBaseModuleBuilder::PreBuild( CScriptBuilder& builder )
{
	const auto& scripts = GetScripts();

	Alert( at_console, "%u script%s\nCompiling...\n", scripts.size(), scripts.size() == 1 ? "" : "s" );

	return true;
}

bool CASBaseModuleBuilder::PostBuild( CScriptBuilder& builder, const bool bSuccess, CASModule* pModule )
{
	Alert( at_console, "Done\n%s script compilation %s\n", m_szModuleTypeName.c_str(), bSuccess ? "succeeded" : "failed" );

	return true;
}
