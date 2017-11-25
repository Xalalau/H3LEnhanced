#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "ASCustomEntitiesUtils.h"

void CASBaseClassCreator::GenerateCommonBaseClassContents( CASClassWriter& writer, const char* const pszEntityClass, const char* const pszBaseClass )
{
	ASSERT( pszEntityClass );
	ASSERT( pszBaseClass );

	const std::string szEntHandle = pszEntityClass + std::string( "@" );
	const std::string szBaseHandle = pszBaseClass + std::string( "@" );

	writer.StartClassDeclaration( true, "ICustomEntity" );

	writer.WriteProperty( CASClassWriter::Visibility::PRIVATE, szEntHandle.c_str(), "m_pSelf", "null" );
	writer.StartPropDeclaration( CASClassWriter::Visibility::PUBLIC, szEntHandle.c_str(), "self" );
		writer.WritePropGetter( "return @m_pSelf;", true );
	writer.EndPropDeclaration();

	writer.NewLine();

	writer.WriteProperty( CASClassWriter::Visibility::PRIVATE, szBaseHandle.c_str(), "m_pBaseClass", "null" );
	writer.StartPropDeclaration( CASClassWriter::Visibility::PUBLIC, szBaseHandle.c_str(), "BaseClass" );
		writer.WritePropGetter( "return @m_pBaseClass;", true );
	writer.EndPropDeclaration();

	writer.WriteProperty( CASClassWriter::Visibility::PRIVATE, "CCallbackHandler@", "m_pCallbackHandler", "null" );
	writer.StartPropDeclaration( CASClassWriter::Visibility::PUBLIC, "CCallbackHandler@", "CallbackHandler" );
		writer.WritePropGetter( "return @m_pCallbackHandler;", true );
	writer.EndPropDeclaration();

	writer.NewLine();

	writer.WriteConstructorHeader();
	writer.StartBracket();
	writer.EndBracket();

	writer.NewLine();

	writer.WriteMethodHeader( CASClassWriter::Visibility::PUBLIC, "void", "SetSelf", ( szEntHandle + " pSelf" ).c_str(), false, true );
	writer.StartBracket();
		writer.Write(
			"if( m_pSelf !is null )\n"
			"\treturn;\n"
			"@m_pSelf = @pSelf;\n" );
	writer.EndBracket();

	writer.NewLine();

	writer.WriteMethodHeader( CASClassWriter::Visibility::PUBLIC, "void", "SetBaseClass", ( szBaseHandle + " pBaseClass" ).c_str(), false, true );
	writer.StartBracket();
		writer.Write(
			"if( m_pBaseClass !is null )\n"
			"\treturn;\n"
			"@m_pBaseClass = @pBaseClass;\n" );
	writer.EndBracket();

	writer.NewLine();

	writer.WriteMethodHeader( CASClassWriter::Visibility::PUBLIC, "void", "SetCallbackHandler", "CCallbackHandler@ pHandler", false, true );
	writer.StartBracket();
		writer.Write(
			"if( m_pCallbackHandler !is null )\n"
			"\treturn;\n"
			"@m_pCallbackHandler = @pHandler;\n" );
	writer.EndBracket();

	writer.NewLine();

	writer.WriteMethodHeader( CASClassWriter::Visibility::PUBLIC, "void", "SetThink", "ThinkFunc@ pFunc", false, true );
	writer.StartBracket();
		writer.Write(
			"m_pCallbackHandler.SetThink( pFunc );" );
	writer.EndBracket();

	writer.NewLine();

	writer.WriteMethodHeader( CASClassWriter::Visibility::PUBLIC, "void", "SetTouch", "TouchFunc@ pFunc", false, true );
	writer.StartBracket();
		writer.Write(
			"m_pCallbackHandler.SetTouch( pFunc );" );
	writer.EndBracket();

	writer.NewLine();

	writer.WriteMethodHeader( CASClassWriter::Visibility::PUBLIC, "void", "SetUse", "UseFunc@ pFunc", false, true );
	writer.StartBracket();
		writer.Write(
			"m_pCallbackHandler.SetUse( pFunc );" );
	writer.EndBracket();

	writer.NewLine();

	writer.WriteMethodHeader( CASClassWriter::Visibility::PUBLIC, "void", "SetBlocked", "BlockedFunc@ pFunc", false, true );
	writer.StartBracket();
		writer.Write(
			"m_pCallbackHandler.SetBlocked( pFunc );" );
	writer.EndBracket();

	writer.NewLine();

	//Implicit conversion to CBaseEntity@ to allow passing this for CBaseEntity@ parameters.
	writer.WriteMethodHeader( CASClassWriter::Visibility::PUBLIC, szEntHandle.c_str(), "opImplCast", "", true, true );
	writer.StartBracket();
		writer.Write( "return @self;" );
	writer.EndBracket();

	writer.NewLine();
}

void CCustomEntityHandler::KeyValue( KeyValueData* pkvd )
{
	edict_t* pEdict = edict();

	//Detach this entity instance from the edict.
	pEdict->pvPrivateData = nullptr;
	pev = nullptr;

	//First keyvalue will be "customclass"
	if( !FStrEq( pkvd->szKeyName, "customclass" ) )
	{
		Alert( at_error, "Custom entity creation: Expected \"customclass\" keyvalue, got \"%s\"\n", pkvd->szKeyName );
		g_engfuncs.pfnRemoveEntity( pEdict );
		return;
	}

	pkvd->fHandled = true;

	g_CustomEntities.CreateCustomEntity( pkvd->szValue, pEdict );
}

//Handler for custom entities. Dynamically allocated so the constructor can call engine functions.
static CCustomEntityHandler* g_pCustomHandler = nullptr;

/**
*	The engine will call into this function to create entities if it fails to find an exported function for it.
*/
extern "C" void DLLEXPORT custom( entvars_t* pev )
{
	if( !g_pCustomHandler )
	{
		//Using array new is a way to get around operator new being overloaded, since operator new[] is not overloaded.
		//This is a tiny memory leak, nothing to worry about unless entities start getting tracked on creation.
		g_pCustomHandler = new CCustomEntityHandler[ 1 ];
	}

	pev->pContainingEntity->pvPrivateData = g_pCustomHandler;
	g_pCustomHandler->pev = pev;

	//Don't call OnCreate.
}
