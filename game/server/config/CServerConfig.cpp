#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CServerConfig.h"

bool CServerConfig::Parse( const char* pszFilename, const char* pszPathID, bool bOptional )
{
	ASSERT( pszFilename );

	Alert( at_aiconsole, "Parsing server config \"%s\"\n", pszFilename );

	Q_strncpy( m_szFilename, pszFilename, sizeof( m_szFilename ) );

	m_KeyValues.reset( new KeyValues( "config" ) );

	const bool bParsedSuccessfully = m_KeyValues->LoadFromFile( g_pFileSystem, pszFilename, pszPathID );

	if( !bParsedSuccessfully )
	{
		if( !bOptional )
			Alert( at_warning, "Couldn't load server config \"%s\"\n", pszFilename );
		m_KeyValues.reset();
	}

	return bParsedSuccessfully;
}

void CServerConfig::ProcessCVars()
{
	if( !m_KeyValues )
		return;

	//Process all cvars.
	for( auto pSection = m_KeyValues->GetFirstTrueSubKey(); pSection; pSection = pSection->GetNextTrueSubKey() )
	{
		if( !strcmp( "cvars", pSection->GetName() ) )
		{
			for( auto pCVar = pSection->GetFirstValue(); pCVar; pCVar = pCVar->GetNextValue() )
			{
				//Set the cvar.
				//TODO: filter cvars so map configs can't set important cvars like rcon nilly willy - Solokiller
				CVAR_SET_STRING( pCVar->GetName(), pCVar->GetString() );
			}
		}
	}
}

void CServerConfig::ProcessEntityClassifications()
{
	if( !m_KeyValues )
		return;

	auto pSection = m_KeyValues->FindKey( "entity_classifications" );

	if( !pSection || pSection->IsEmpty() )
		return;

	if( pSection->GetDataType() != KeyValues::TYPE_NONE )
	{
		Alert( at_warning, "Found entity classifications key with value, expected section, ignoring\n" );
		return;
	}

	const bool bReset = pSection->GetInt( "reset" ) == 1;

	if( bReset )
	{
		EntityClassifications().Reset();
	}

	//Parse classes first.
	for( auto pSetting = pSection->GetFirstSubKey(); pSetting; pSetting = pSetting->GetNextKey() )
	{
		if( !strcmp( "class", pSetting->GetName() ) )
		{
			if( pSetting->GetDataType() == KeyValues::TYPE_NONE )
			{
				//Class with default settings.
				EntityClassifications().AddClassification( pSetting->GetString() );
			}
			else
			{
				//Complete definition, get settings.
				auto pszName = pSetting->GetString( "name" );

				if( !pszName || !( *pszName ) )
				{
					Alert( at_warning, "Found entity classification section with no name, ignoring\n" );
					continue;
				}

				const auto defaultSourceRelationship = RelationshipFromString( pSetting->GetString( "defaultSourceRelationship", RelationshipToString( R_NO ) ) );
				auto pszDefaultTargetRelationship = pSetting->GetString( "defaultTargetRelationship", nullptr );

				if( pszDefaultTargetRelationship )
				{
					EntityClassifications().AddClassification( pszName, defaultSourceRelationship, RelationshipFromString( pszDefaultTargetRelationship ) );
				}
				else
				{
					EntityClassifications().AddClassification( pszName, defaultSourceRelationship );
				}
			}
		}
	}

	//Now parse aliases.
	for( auto pSetting = pSection->GetFirstSubKey(); pSetting; pSetting = pSetting->GetNextKey() )
	{
		if( !strcmp( "alias", pSetting->GetName() ) )
		{
			if( pSetting->GetDataType() != KeyValues::TYPE_NONE )
			{
				Alert( at_warning, "Found entity classification alias with value, expected section, ignoring\n" );
				continue;
			}

			auto pszSource = pSetting->GetString( "source" );

			if( !( *pszSource ) )
			{
				Alert( at_warning, "Found entity classification alias with no source name, ignoring\n" );
				continue;
			}

			auto pszTarget = pSetting->GetString( "target" );

			if( !( *pszTarget ) )
			{
				Alert( at_warning, "Found entity classification alias with no target name, ignoring\n" );
				continue;
			}

			EntityClassifications().AddAlias( pszSource, pszTarget );
		}
	}

	//Now parse relationships.
	for( auto pSetting = pSection->GetFirstSubKey(); pSetting; pSetting = pSetting->GetNextKey() )
	{
		if( !strcmp( "relationship", pSetting->GetName() ) )
		{
			if( pSetting->GetDataType() != KeyValues::TYPE_NONE )
			{
				Alert( at_warning, "Found entity classification relationship with value, expected section, ignoring\n" );
				continue;
			}

			auto pszSource = pSetting->GetString( "source" );

			if( !( *pszSource ) )
			{
				Alert( at_warning, "Found entity classification relationship with no source name, ignoring\n" );
				continue;
			}

			auto pszTarget = pSetting->GetString( "target" );

			if( !( *pszTarget ) )
			{
				Alert( at_warning, "Found entity classification relationship with no target name, ignoring\n" );
				continue;
			}

			auto pszRelationship = pSetting->GetString( "relationship", nullptr );

			if( !( *pszRelationship ) )
			{
				Alert( at_warning, "Found entity classification relationship with no relationship setting, ignoring\n" );
				continue;
			}

			const auto relationship = RelationshipFromString( pszRelationship );

			const bool bBiDirectional = pSetting->GetInt( "biDirectional" ) == 1;

			EntityClassifications().AddRelationship( pszSource, pszTarget, relationship, bBiDirectional );
		}
	}
}
