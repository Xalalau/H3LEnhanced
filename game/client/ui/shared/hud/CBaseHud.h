#ifndef GAME_CLIENT_UI_SHARED_HUD_CBASEHUD_H
#define GAME_CLIENT_UI_SHARED_HUD_CBASEHUD_H

#include "shared_game_utils.h"
//TODO: need to move DataMapping out of entities. - Solokiller
#include "entities/DataMapping.h"

#include "CHudList.h"
#include "HudDefs.h"

#include "CBitSet.h"

#include "MessageHandler.h"

#include "CHud.h"

/**
*	Helper macro to easily create Hud elements.
*	Should be used in CreateHudElements() only.
*/
#define CREATE_HUDELEMENT( name )	\
new name( #name, *this )

/**
*	Data for a single Hud sprite.
*/
struct HudSprite_t
{
	HSPRITE hSprite;
	wrect_t rect;
	char szName[ MAX_SPRITE_NAME_LENGTH ];
};

/**
*	Base class for the Hud.
*/
class CBaseHud
{
public:
	static const int INVALID_SPRITE_INDEX = -1;

protected:
	/**
	*	Callback used to evaluate whether an element should be drawn.
	*	@return true if the element should be drawn
	*/
	using HudElementEvaluatorFn = bool ( * )( CHudElement* pElement, void* pUserData );

public:
	DECLARE_CLASS_NOBASE( CBaseHud );

public:
	CBaseHud();
	virtual ~CBaseHud();

	void Init();

protected:
	virtual void PreInit();

	/**
	*	Creates the Hud elements for this Hud.
	*/
	virtual void CreateHudElements() {}

	virtual void PostInit();

public:
	/**
	*	Called when the active Hud is changed, and this Hud became or was active.
	*/
	virtual void ActiveHudStateChanged( bool bIsActive );

	virtual void VidInit();

protected:
	/**
	*	Called when the Hud can load its sprites.
	*/
	virtual void LoadSprites();

public:
	virtual void InitHud();
	virtual void ResetHud();

	bool Redraw( float flTime, bool intermission );

protected:
	/**
	*	Subclasses should override this to handle specific drawing needs.
	*/
	virtual bool DoDraw( float flTime, bool intermission ) = 0;

	/**
	*	Draws all Hud elements.
	*	@param flTime Current time
	*	@param evaluatorFn Function to evaluate if an element should be drawn
	*	@param pUserData Optional user data to pass to the evaluator function
	*/
	void DrawHudElements( float flTime, HudElementEvaluatorFn evaluatorFn, void* pUserData = nullptr );

public:
	/**
	*	Called before the Hud runs think.
	*/
	virtual bool PreThinkUpdateClient( client_data_t* cdata );

	/**
	*	Called after the Hud has ran think.
	*/
	virtual bool PostThinkUpdateClient( client_data_t* cdata );

	/**
	*	Called every time the client's data has changed, after PreThinkUpdateClient and before PostThinkUpdateClient.
	*/
	virtual void Think();

public:
	/*
	*	Called when the FOV is updated, lets Huds respond to the change.
	*	Called by CHud, do not call directly.
	*/
	virtual void UpdateFOV( int iNewFOV, bool bForce );

	int DrawHudNumber( int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int DrawHudString( int x, int y, int iMaxX, char *szString, int r, int g, int b );
	int DrawHudStringReverse( int xpos, int ypos, int iMinX, char *szString, int r, int g, int b );
	int DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b );
	int GetNumWidth( int iNumber, int iFlags ) const;

	/**
	*	Called when the game is being shut down.
	*/
	virtual void GameShutdown();

	/**
	*	@return The Hud element list manager.
	*/
	CHudList& HudList() { return m_HudList; }

	CMessageHandlers& GetMessageHandlers() { return m_MessageHandlers; }

	int GetFontHeight() const { return m_iFontHeight; }

	const CBitSet<int>& GetHideHudBits() const { return m_HideHUDDisplay; }

	CBitSet<int>& GetHideHudBits() { return m_HideHUDDisplay; }

	HSPRITE GetSprite( int index ) const;

	const wrect_t& GetSpriteRect( int index ) const;

	/**
	*	Searches through the sprite list loaded from hud.txt for a name matching SpriteName
	*	returns an index into the Hud().m_pSprites[] array
	*	returns INVALID_SPRITE_INDEX if sprite not found
	*/
	int GetSpriteIndex( const char* SpriteName ) const;

	int GetHudNumber0Index() const { return m_HUD_number_0; }

private:
	CHudList m_HudList;
	CMessageHandlers m_MessageHandlers;

	int m_iFontHeight = 0;

	//Hud element state.
	CBitSet<int>	m_HideHUDDisplay;

	//Hud sprites
	// the memory for these arrays are allocated in the first call to CBaseHud::VidInit(), when the hud.txt and associated sprites are loaded.
	// freed in ~CBaseHud()
	client_sprite_t*	m_pSpriteList = nullptr;
	int					m_iSpriteCount = 0;
	int					m_iSpriteCountAllRes = 0;	//!Sprite count for all resolutions.

	//Only one array now to make things a little saner.
	HudSprite_t*		m_pSprites = nullptr;	// the sprites loaded from hud.txt, total m_iSpriteCount

	// sprite indexes
	int					m_HUD_number_0;

private:
	CBaseHud( const CBaseHud& ) = delete;
	CBaseHud& operator=( const CBaseHud& ) = delete;
};

#endif //GAME_CLIENT_UI_SHARED_HUD_CBASEHUD_H
