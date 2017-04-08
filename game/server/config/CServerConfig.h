#ifndef GAME_SERVER_CONFIG_CSERVERCONFIG_H
#define GAME_SERVER_CONFIG_CSERVERCONFIG_H

#include <memory>

#include "KeyValues.h"

/**
*	Parses a server config file and stores off persistent data.
*/
class CServerConfig
{
public:
	CServerConfig() = default;
	~CServerConfig() = default;

	/**
	*	Parses the given config file.
	*	@param pszFilename Name of the file, starting in Half-Life/
	*	@param pszPathID If not null, ID of the path to search in. If null, all paths are searched
	*	@return Whether the file was successfully parsed
	*/
	bool Parse( const char* pszFilename, const char* pszPathID = nullptr, bool bOptional = false );

	const char* GetFilename() const { return m_szFilename; }

	bool IsLoaded() const { return m_KeyValues != nullptr; }

	/**
	*	Processes all CVars specified by this file.
	*/
	void ProcessCVars();

	/**
	*	Processes the entity classifications specified by this file.
	*/
	void ProcessEntityClassifications();

private:
	char m_szFilename[ MAX_PATH ] = {};

	std::unique_ptr<KeyValues, KeyValuesDeleter> m_KeyValues;

private:
	CServerConfig( const CServerConfig& ) = delete;
	CServerConfig& operator=( const CServerConfig& ) = delete;
};

#endif //GAME_SERVER_CONFIG_CSERVERCONFIG_H
