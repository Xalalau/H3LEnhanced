#ifndef GAME_CLIENT_UI_SHARED_HUD_CHUD_H
#define GAME_CLIENT_UI_SHARED_HUD_CHUD_H

class CBaseHud;

/**
*	Maintains Hud state that is shared between different Huds.
*/
class CHud final
{
public:
	CHud() = default;
	~CHud() = default;

	void Init();

	void VidInit();

	void GameShutdown();

	bool Redraw( float flTime, bool intermission );

	bool UpdateClientData( client_data_t* cdata );

private:
	bool PreThinkUpdateClient( client_data_t* cdata );

	bool PostThinkUpdateClient( client_data_t* cdata );

public:
	/**
	*	Updates the FOV. Can be overridden for custom handling.
	*	@param iNewFOV New Field Of Fiew
	*	@param bForce Whether to force the FOV to this setting
	*/
	void UpdateFOV( int iNewFOV, bool bForce );

	float GetTime() const { return m_flTime; }

	float GetOldTime() const { return m_flOldTime; }

	double GetTimeDelta() const { return m_flTimeDelta; }

	float GetSnapshotTime() const { return m_flSnapshotTime; }

	void SetSnapshotTime( float flTime )
	{
		m_flSnapshotTime = flTime;
	}

	const SCREENINFO& ScreenInfo() const { return m_scrinfo; }

	int GetFOV() const { return m_iFOV; }

	void SetFOV( int iFOV )
	{
		m_iFOV = iFOV;
	}

	float GetSensitivity() const
	{
		return m_flMouseSensitivity;
	}

	void SetSensitivity( float flMouseSensitivity )
	{
		m_flMouseSensitivity = flMouseSensitivity;
	}

	int GetResolution() const { return m_iResolution; }

	bool IsInIntermission() const { return m_bIntermission; }

	void SetInIntermission( bool bIntermission )
	{
		m_bIntermission = bIntermission;
	}

	const Vector& GetOrigin() const { return m_vecOrigin; }

	const Vector& GetAngles() const { return m_vecAngles; }

	int GetKeyBits() const { return m_iKeyBits; }

	void ClearKeyBits( int bits )
	{
		m_iKeyBits &= ~bits;
	}

	int GetWeaponBits() const { return m_iWeaponBits; }

	bool IsTeamplay() const { return m_bIsTeamplay; }

	void SetIsTeamplay( bool bIsTeamplay )
	{
		m_bIsTeamplay = bIsTeamplay;
	}

	bool IsMultiplayer() const { return m_bIsMultiplayer; }

	void SetIsMultiplayer( bool bIsMultiplayer )
	{
		m_bIsMultiplayer = bIsMultiplayer;
	}

	cvar_t* GetDefaultFOVCVar() { return default_fov; }

	/**
	*	@return The current Hud. If no Hud was set, this will cause a crash.
	*/
	CBaseHud& GetHud() const
	{
		ASSERT( m_pCurrentHud );

		return *m_pCurrentHud;
	}

	/**
	*	@copydoc GetHud() const
	*/
	CBaseHud& GetHud()
	{
		return const_cast<const CHud*>( this )->GetHud();
	}

	/**
	*	Sets the current Hud.
	*	@param pHud Hud to set. Can be null
	*/
	void SetHud( CBaseHud* pHud );

	//Message handlers
	void MsgFunc_SetFOV( const char *pszName, int iSize, void *pbuf );

private:
	float m_flTime = 1.f;			// the current client time
	float m_flOldTime = 0;			// the time at which the HUD was last redrawn
	double m_flTimeDelta = 0;		// the difference between flTime and fOldTime

	float m_flSnapshotTime = 0;		//! If non-zero, the time at which to take a snapshot.

	SCREENINFO m_scrinfo;			// Screen information

	int	m_iFOV = 0;
	float m_flMouseSensitivity = 0;

	int m_iResolution = 0;

	//Game state
	bool			m_bIntermission = false;

	//TODO: these should probably be taken from the local CBasePlayer - Solokiller
	Vector			m_vecOrigin;
	Vector			m_vecAngles;

	int				m_iKeyBits = 0;
	int				m_iWeaponBits = 0;

	bool			m_bIsTeamplay = false;
	bool			m_bIsMultiplayer = false;

	//CVars
	cvar_t* default_fov = nullptr;
	cvar_t* hud_takesshots = nullptr;

	CBaseHud* m_pCurrentHud = nullptr;

	//TODO: list of all Huds registered by client. - Solokiller

private:
	CHud( const CHud& ) = delete;
	CHud& operator=( const CHud& ) = delete;
};

/**
*	Gets the singleton Hud instance.
*/
CHud& Hud();

#endif //GAME_CLIENT_UI_SHARED_HUD_CHUD_H
