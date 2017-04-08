#ifndef GAME_CLIENT_UI_SHARED_HUD_CHUDELEMENT_H
#define GAME_CLIENT_UI_SHARED_HUD_CHUDELEMENT_H

#include "Platform.h"

#include "shared_game_utils.h"
#include "entities/DataMapping.h"
#include "CBitSet.h"
#include "HudDefs.h"

//Must use single inheritance rules for pointers to member functions. - Solokiller
//Used by ForEachHudElem.
class SINGLE_INHERITANCE CHudElement;

/**
*	Base class for Hud elements.
*/
class CHudElement
{
public:
	DECLARE_CLASS_NOBASE( CHudElement );

	using Flags_t = CBitSet<int>;

public:
	/**
	*	@param pszName The name of this Hud element.
	*/
	CHudElement( const char* const pszName );
	virtual ~CHudElement();

	/**
	*	@return The name of this Hud element.
	*/
	const char* GetName() const { return m_pszName; }

	/**
	*	@return This Hud element's flags.
	*/
	const Flags_t& GetFlags() const { return m_Flags; }

	/**
	*	@copydoc GetFlags() const
	*/
	Flags_t& GetFlags() { return m_Flags; }

	/**
	*	@return Whether the element should draw.
	*/
	virtual bool ShouldDraw()
	{
		return ( GetFlags() & HUD_ACTIVE ) != 0;
	}

	/**
	*	@return Whether this element is active.
	*/
	virtual bool IsActive()
	{
		return ( GetFlags() & HUD_ACTIVE ) != 0;
	}

	/**
	*	Sets whether this element is active.
	*/
	virtual void SetActive( const bool bState )
	{
		if( bState )
			GetFlags() |= HUD_ACTIVE;
		else
			GetFlags().ClearFlags( HUD_ACTIVE );
	}

	/**
	*	Called when the client is initializing.
	*/
	virtual void Init() {}

	/**
	*	Called after a connection to a server has been established.
	*/
	virtual void VidInit() {}

	/**
	*	Step through the local data,  placing the appropriate graphics & text as appropriate
	*	@param flTime Current time.
	*	@return true if they've changed, false otherwise
	*/
	virtual bool Draw( float flTime ) { return false; }

	/**
	*	Called every time shared client dll/engine data gets changed.
	*/
	virtual void Think() {}

	/**
	*	Called when the HUD needs to be reset.
	*/
	virtual void Reset() {}

	/**
	*	Called every time a server is connected to.
	*/
	virtual void InitHUDData() {}

	/**
	*	Called every frame when the element is active.
	*/
	virtual void ProcessInput() {}

private:
	const char* const m_pszName;
	// active, moving,
	Flags_t m_Flags;

private:
	CHudElement( const CHudElement& ) = delete;
	CHudElement& operator=( const CHudElement& ) = delete;
};

/**
*	Helper class to store a reference to the owning Hud in each Hud element.
*/
template<typename HUD>
class CBaseHudElement : public CHudElement
{
public:
	DECLARE_CLASS( CBaseHudElement, CHudElement );

	/**
	*	@param hud Hud that this element belongs to.
	*/
	CBaseHudElement( const char* const pszName, HUD& hud )
		: BaseClass( pszName )
		, m_Hud( hud )
	{
	}

	HUD& GetHud() { return m_Hud; }

protected:
	HUD& m_Hud;
};

#endif //GAME_CLIENT_UI_SHARED_HUD_CHUDELEMENT_H
