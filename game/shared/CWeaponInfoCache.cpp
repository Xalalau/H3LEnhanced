#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"

#include "CWeaponInfo.h"

#ifdef CLIENT_DLL
#include "hud.h"
#include "CWeaponHUDInfo.h"
#endif

#include "CWeaponInfoCache.h"

CWeaponInfoCache g_WeaponInfoCache;

const char* const CWeaponInfoCache::WEAPON_INFO_DIR = "weapon_info";

CWeaponInfoCache::CWeaponInfoCache()
{
	//Set dummy HUD info so missing files don't cause crashes. - Solokiller
#ifdef CLIENT_DLL
	m_DefaultInfo.SetHUDInfo( new CWeaponHUDInfo() );
#endif
}

CWeaponInfoCache::~CWeaponInfoCache()
{
}

const CWeaponInfo* CWeaponInfoCache::FindWeaponInfo( const char* const pszWeaponName ) const
{
	ASSERT( pszWeaponName );

	if( !pszWeaponName )
		return nullptr;

	auto it = m_InfoMap.find( pszWeaponName );

	if( it != m_InfoMap.end() )
		return m_InfoList[ it->second ].get();

	return nullptr;
}

const CWeaponInfo* CWeaponInfoCache::LoadWeaponInfo( const int iID, const char* const pszWeaponName, const char* const pszSubDir )
{
	ASSERT( pszWeaponName );
	ASSERT( *pszWeaponName );

	if( auto pInfo = FindWeaponInfo( pszWeaponName ) )
		return pInfo;

	std::unique_ptr<CWeaponInfo> info = std::make_unique<CWeaponInfo>();

	if( LoadWeaponInfoFromFile( pszWeaponName, pszSubDir, *info ) )
	{
		CWeaponInfo* pInfo = info.get();

		m_InfoList.emplace_back( std::move( info ) );

		auto result = m_InfoMap.insert( std::make_pair( pInfo->GetWeaponName(), m_InfoList.size() - 1 ) );

		if( result.second )
		{
			pInfo->SetID( iID );

#ifdef CLIENT_DLL
			//Load HUD info right away.
			CWeaponHUDInfo* pHUDInfo = new CWeaponHUDInfo();

			if( !pHUDInfo->LoadFromFile( pszWeaponName ) )
			{
				Alert( at_warning, "CWeaponInfoCache::LoadWeaponInfo: Couldn't load weapon \"%s\" HUD info\n", pszWeaponName );
			}

			pInfo->SetHUDInfo( pHUDInfo );
#endif

			return pInfo;
		}

		Alert( at_error, "CWeaponInfoCache::LoadWeaponInfo: Failed to insert weapon info \"%s\" into cache!\n", pszWeaponName );

		m_InfoList.erase( m_InfoList.end() - 1 );
	}

	return &m_DefaultInfo;
}

void CWeaponInfoCache::ClearInfos()
{
	m_InfoMap.clear();
	m_InfoList.clear();
}

void CWeaponInfoCache::EnumInfos( EnumInfoCallback pCallback, void* pUserData ) const
{
	ASSERT( pCallback );

	if( !pCallback )
		return;

	for( const auto& info : m_InfoList )
	{
		if( !pCallback( *info, pUserData ) )
			break;
	}
}

size_t CWeaponInfoCache::GenerateHash() const
{
	size_t uiHash = 0;

	//TODO: not the best hash, but it'll do. - Solokiller
	for( const auto& info : m_InfoList )
	{
		uiHash += StringHash( info->GetWeaponName() );
	}

	return uiHash;
}

bool CWeaponInfoCache::LoadWeaponInfoFromFile(const char* const pszWeaponName, const char* const pszSubDir, CWeaponInfo& info)
{
	char szPath[MAX_PATH] = {};

	int iResult;

	if (pszSubDir)
	{
		iResult = snprintf(szPath, sizeof(szPath), "%s/%s/%s.txt", WEAPON_INFO_DIR, pszSubDir, pszWeaponName);
	}
	else
	{
		iResult = snprintf(szPath, sizeof(szPath), "%s/%s.txt", WEAPON_INFO_DIR, pszWeaponName);
	}

	if (iResult < 0 || static_cast<size_t>(iResult) >= sizeof(szPath))
	{
		//Subdir can be null, that's expected. - Solokiller
		Alert(at_error, "CWeaponInfoCache::LoadWeaponInfoFromFile: Failed to format file path for \"%s\" (subdir: \"%s\")!\n", pszWeaponName, pszSubDir);
		return false;
	}

	FileHandle_t hFile = g_pFileSystem->Open(szPath, "r");

	if (hFile == FILESYSTEM_INVALID_HANDLE)
	{
		Alert(at_error, "CWeaponInfoCache::LoadWeaponInfoFromFile: Failed to open file \"%s\"!\n", szPath);
		return false;
	}

	Alert(at_aiconsole, "CWeaponInfoCache::LoadWeaponInfoFromFile: Opened file \"%s\"\n", szPath);

	info.SetWeaponName(pszWeaponName);

	const size_t uiSize = g_pFileSystem->Size(hFile);

	auto buffer = std::make_unique<char[]>(uiSize + 1);

	g_pFileSystem->Read(buffer.get(), uiSize, hFile);

	g_pFileSystem->Close(hFile);

	const char* pszBuffer = buffer.get();

	pszBuffer = COM_Parse(pszBuffer);

	if (stricmp(com_token, pszWeaponName))
	{
		Alert(at_error, "CWeaponInfoCache::LoadWeaponInfoFromFile: Expected weapon name \"%s\", got \"%s\"\n", pszWeaponName, com_token);
		return false;
	}

	pszBuffer = COM_Parse(pszBuffer);

	if (com_token[0] != '{')
	{
		Alert(at_error, "CWeaponInfoCache::LoadWeaponInfoFromFile: Expected token '{', got \"%s\"!\n", com_token);
		return false;
	}

	//Parse in all values.
	char szKey[MAX_COM_TOKEN];

	while (true)
	{
		pszBuffer = COM_Parse(pszBuffer);

		if (!pszBuffer)
		{
			Alert(at_error, "CWeaponInfoCache::LoadWeaponInfoFromFile: Unexpected EOF while reading from file (searching key)!\n");
			return false;
		}

		if (com_token[0] == '}')
		{
			break;
		}

		strncpy(szKey, com_token, sizeof(szKey));
		szKey[sizeof(szKey) - 1] = '\0';

		pszBuffer = COM_Parse(pszBuffer);

		if (!pszBuffer)
		{
			Alert(at_error, "CWeaponInfoCache::LoadWeaponInfoFromFile: Unexpected EOF while reading from file (searching value for \"%s\"!\n", szKey);
			return false;
		}

		if (com_token[0] == '}')
		{
			break;
		}

		if (!info.KeyValue(szKey, com_token))
		{
			Alert(at_aiconsole, "CWeaponInfoCache::LoadWeaponInfoFromFile: Unhandled keyvalue \"%s\" \"%s\"\n", szKey, com_token);
		}
	}

	return true;
}