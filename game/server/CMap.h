#ifndef GAME_SERVER_CMAP_H
#define GAME_SERVER_CMAP_H

#include <memory>

#include "Color.h"
#include "HudColors.h"

#include "SaveRestore.h"

#include "CReplacementCache.h"
#include "CReplacementMap.h"

class CServerConfig;

/**
*	Stores global per-map data.
*/
class CMap final
{
public:
	DECLARE_CLASS_NOBASE( CMap );
	DECLARE_DATADESC_FINAL();

	static CMap* CreateInstance();

	static CMap* GetInstance();

	static void DestroyInstance();

	static bool Exists();

	static CMap* CreateIfNeeded();

	CMap();
	~CMap();

private:
	void LoadMapConfig();

public:
	bool Save( CSave& save );
	bool Restore( CRestore& restore );

	void WorldInit();
	void WorldActivated();

	/**
	*	Runs per-frame think operations.
	*/
	void Think();

	/**
	*	Sends Hud colors for the given player if needed.
	*	@param pPlayer Player to send colors to.
	*	@param bForce If true, always send, regardless of conditions.
	*/
	void SendHudColors( CBasePlayer* pPlayer, const bool bForce = false );

private:
	static void SendHudColors( CBasePlayer* pPlayer, const CHudColors& colors );

public:
	/**
	*	@return The primary HUD color.
	*/
	const Color& GetPrimaryHudColor() const { return m_HudColors.m_PrimaryColor; }

	/**
	*	Sets the primary HUD color.
	*	@param color Color to set.
	*/
	void SetPrimaryHudColor( const Color& color )
	{
		m_HudColors.m_PrimaryColor = color;

		m_bUseCustomHudColors = true;
	}

	/**
	*	@return The empty / nearly empty HUD color.
	*/
	const Color& GetEmptyItemHudColor() const { return m_HudColors.m_EmptyItemColor; }

	/**
	*	Sets the empty / nearly empty HUD color.
	*	@param color Color to set.
	*/
	void SetEmptyItemHudColor( const Color& color )
	{
		m_HudColors.m_EmptyItemColor = color;

		m_bUseCustomHudColors = true;
	}

	/**
	*	@return The ammo bar HUD color.
	*/
	const Color& GetAmmoBarHudColor() const { return m_HudColors.m_AmmoBarColor; }

	/**
	*	Sets the ammo bar HUD color.
	*	@param color Color to set.
	*/
	void SetAmmoBarHudColor( const Color& color )
	{
		m_HudColors.m_AmmoBarColor = color;

		m_bUseCustomHudColors = true;
	}

	/**
	*	Resets Hud colors to their defaults.
	*/
	void ResetHudColors();

	const CReplacementMap* GetGlobalModelReplacement() const { return m_pGlobalModelReplacement; }

	/**
	*	Loads the global model replacement file.
	*/
	void LoadGlobalModelReplacement( const char* const pszFileName );

	void InitializeEntityClassifications();

private:
	/**
	*	Runs right after the constructor. Makes it easier to separate init and setup code.
	*/
	void Create();

private:
	static CMap* m_pInstance;

	//Last time we ran think.
	float m_flPrevFrameTime = 0;

	bool m_bUseCustomHudColors = false;

	float m_flLastHudColorChangeTime = 0;

	CHudColors m_HudColors;

	//Cache of model replacement files. In the future, model replacement may be used by individual entities to replace effects models. - Solokiller
	CReplacementCache m_ModelReplacement;
	CReplacementMap* m_pGlobalModelReplacement = nullptr;

	std::unique_ptr<CServerConfig> m_MapConfig;

private:
	CMap( const CMap& ) = delete;
	CMap& operator=( const CMap& ) = delete;
};

#endif //GAME_SERVER_CMAP_H