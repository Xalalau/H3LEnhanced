#include <cassert>

#include "extdll.h"
#include "util.h"

#include "CFile.h"

#include "CReplacementMap.h"

#include "CReplacementCache.h"

CReplacementMap* CReplacementCache::GetMap( const char* const pszFileName ) const
{
	assert( pszFileName );

	char szAbsoluteName[ MAX_PATH ];

	if( g_pFileSystem->GetLocalPath( pszFileName, szAbsoluteName, sizeof( szAbsoluteName ) ) )
	{
		auto it = m_Cache.find( szAbsoluteName );

		if( it != m_Cache.end() )
			return it->second.get();
	}

	return nullptr;
}

CReplacementMap* CReplacementCache::AcquireMap( const char* const pszFileName )
{
	assert( pszFileName );

	char szAbsoluteName[ MAX_PATH ];

	if( !g_pFileSystem->GetLocalPath( pszFileName, szAbsoluteName, sizeof( szAbsoluteName ) ) )
	{
		Alert( at_error, "CReplacementCache::AcquireMap: Couldn't format absolute filename for \"%s\"!\n", pszFileName );
		return nullptr;
	}

	auto it = m_Cache.find( szAbsoluteName );

	if( it != m_Cache.end() )
		return it->second.get();

	auto map = LoadMap( pszFileName, szAbsoluteName );

	auto pMap = map.get();

	//Insert it in either case so non-existent files aren't looked up a bunch. - Solokiller
	auto result = m_Cache.emplace( szAbsoluteName, std::move( map ) );

	if( !result.second )
		return nullptr;

	return pMap;
}

std::unique_ptr<CReplacementMap> CReplacementCache::LoadMap(const char* const pszFileName, const char* const pszAbsFileName) const
{
	assert(pszAbsFileName);

	CFile file(pszFileName, "r");

	if (!file.IsOpen())
	{
		Alert(at_error, "CReplacementCache::LoadMap: Couldn't open \"%s\"!\n", pszFileName);
		return nullptr;
	}

	//TODO: should define these constants - Solokiller
	char szLine[1024];

	char szOriginal[1024];
	char szReplacement[1024];

	const char* pszData;

	size_t uiLine = 1;

	auto map = std::make_unique<CReplacementMap>(pszAbsFileName);

	while (file.IsOk() && file.ReadLine(szLine, sizeof(szLine)))
	{
		if (!(*szLine))
			continue;

		pszData = COM_Parse(szLine, szOriginal, sizeof(szOriginal));

		if (!pszData)
		{
			Alert(at_error, "CReplacementCache::LoadMap: In file \"%s\" on line %u:\nError parsing original filename near \"%s\"\n", pszFileName, uiLine, szLine);
			return nullptr;
		}


		pszData = COM_Parse(pszData, szReplacement, sizeof(szReplacement));

		if (!pszData)
		{
			Alert(at_error, "CReplacementCache::LoadMap: In file \"%s\" on line %u:\nError parsing replacement filename near \"%s\"\n", pszFileName, uiLine, szLine);
			return nullptr;
		}

		if (!map->AddReplacement(szOriginal, szReplacement))
		{
			Alert(at_warning, "CReplacementCache::LoadMap: In file \"%s\" on line %u:\nDuplicate original filename \"%s\", ignoring\n", pszFileName, uiLine, szOriginal);
		}

		++uiLine;
	}

	return std::move(map);
}