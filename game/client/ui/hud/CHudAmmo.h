/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef GAME_CLIENT_UI_HUD_CHUDAMMO_H
#define GAME_CLIENT_UI_HUD_CHUDAMMO_H

#include "shared/hud/CHudElement.h"
#include "hud.h"

#include "Color.h"

class CBasePlayerWeapon;

class CHudAmmo : public CBaseHudElement<CHLHud>
{
private:
	/**
	*	Crosshair render modes.
	*/
	enum CrosshairMode
	{
		/**
		*	Always render at native image scale.
		*/
		CROSS_NATIVESCALE = 0,

		/**
		*	Scale with resolution.
		*/
		CROSS_RESSCALE = 1,

		/**
		*	Use user defined scale.
		*/
		CROSS_USERSCALE = 2,
	};
public:
	DECLARE_CLASS( CHudAmmo, CBaseHudElement<CHLHud> );

	CHudAmmo( const char* const pszName, CHLHud& hud );

	void Init() override;
	void VidInit() override;
	bool Draw( float flTime ) override;
	void Think() override;
	void Reset() override;

	int DrawWList( float flTime );
	bool MsgFunc_CurWeapon( const char *pszName, int iSize, void *pbuf );
	void MsgFunc_AmmoX( const char *pszName, int iSize, void *pbuf );
	void MsgFunc_AmmoPickup( const char *pszName, int iSize, void *pbuf );
	void MsgFunc_WeapPickup( const char *pszName, int iSize, void *pbuf );
	void MsgFunc_ItemPickup( const char *pszName, int iSize, void *pbuf );
	void MsgFunc_HideWeapon( const char *pszName, int iSize, void *pbuf );

	void SlotInput( int iSlot );
	void _cdecl UserCmd_Slot1( void );
	void _cdecl UserCmd_Slot2( void );
	void _cdecl UserCmd_Slot3( void );
	void _cdecl UserCmd_Slot4( void );
	void _cdecl UserCmd_Slot5( void );
	void _cdecl UserCmd_Slot6( void );
	void _cdecl UserCmd_Slot7( void );
	void _cdecl UserCmd_Slot8( void );
	void _cdecl UserCmd_Slot9( void );
	void _cdecl UserCmd_Slot10( void );
	void _cdecl UserCmd_Close( void );
	void _cdecl UserCmd_NextWeapon( void );
	void _cdecl UserCmd_PrevWeapon( void );

	void SelectSlot( int iSlot, const bool fAdvance, int iDirection );

	void SetCrosshair( HSPRITE hCrosshair, const wrect_t& rect, int r, int g, int b );

	// ############ hu3lifezado ############ //
	// Funcao para religar o crosshair na primeira pessoa
	void hu3ReativarCrosshair(void);
	bool isPlayerDead(void);
	// ############ //

private:
	int DrawBar( int x, int y, int width, int height, float f );
	void DrawAmmoBar( CBasePlayerWeapon *p, int x, int y, int width, int height );

	void UpdateWeaponHUD( CBasePlayerWeapon* pWeapon, bool bOnTarget );

private:
	float m_fFade;
	::Color  m_rgba;
	CBasePlayerWeapon *m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;

	HSPRITE m_hCrosshair;
	wrect_t m_CrosshairRC;
	int m_iR, m_iG, m_iB;

	cvar_t* m_pCrosshair;
	cvar_t* m_pCrosshairMode;
	cvar_t* m_pCrosshairScale;

	//The server sent a message before the client was ready, so update as soon as possible - Solokiller
	bool m_bNeedsLocalUpdate = false;
	bool m_bOnTarget = false;

	/**
	*	NULL means off, 1 means just the menu bar, otherwise
	*	this points to the active weapon menu item
	*/
	CBasePlayerWeapon* m_pActiveSel;

	/**
	*	Last weapon menu selection.
	*/
	CBasePlayerWeapon* m_pLastSel;

	// Sprite for top row of weapons menu
	HSPRITE m_hsprBuckets;

	// Ammo Bar width and height
	int m_iBucketHeight, m_iBucketWidth, m_iABHeight, m_iABWidth;

	bool m_bPlayerDead = false;
};

#endif //GAME_CLIENT_UI_HUD_CHUDAMMO_H
