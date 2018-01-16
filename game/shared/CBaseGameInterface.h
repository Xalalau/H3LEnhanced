#ifndef GAME_SHARED_CBASEGAMEINTERFACE_H
#define GAME_SHARED_CBASEGAMEINTERFACE_H

/**
*	@brief Mixin class for shared game interface code
*/
class CBaseGameInterface
{
public:
	/**
	*	@brief Gets the game interface instance
	*/
	static CBaseGameInterface* GetInstance();

	CBaseGameInterface();
	~CBaseGameInterface();

	/**
	*	@brief Gets the game directory name
	*/
	const char* GetGameDirectory() const { return m_szGameDirectory; }

protected:
	/**
	*	Initializes common code.
	*	@return true on success, false on failure.
	*/
	bool InitializeCommon();

	/**
	*	Shuts down common code.
	*/
	void ShutdownCommon();

	/**
	*	Initializes the filesystem pointer.
	*	@return true on success, false on failure.
	*/
	bool InitFileSystem();

	/**
	*	Shuts down the filesystem.
	*/
	void ShutdownFileSystem();

private:
	//Cached off so nothing else needs to do error checking.
	//The game directory cannot change after startup, since it's extracted from the command line.
	//It used to be possible to change this using the custom game option, but that was replaced with Steam mod support.
	char m_szGameDirectory[ MAX_PATH ] = {};

private:
	CBaseGameInterface( const CBaseGameInterface& ) = delete;
	CBaseGameInterface& operator=( const CBaseGameInterface& ) = delete;
};

inline CBaseGameInterface* GameInterface()
{
	return CBaseGameInterface::GetInstance();
}

#endif //GAME_SHARED_CBASEGAMEINTERFACE_H