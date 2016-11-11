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
#ifndef GAME_CLIENT_UI_HUD_CHUDFLASHLIGHT_H
#define GAME_CLIENT_UI_HUD_CHUDFLASHLIGHT_H

class CHudFlashlight : public CHudBase
{
public:
	DECLARE_CLASS( CHudFlashlight, CHudBase );

	CHudFlashlight( const char* const pszName );

	bool Init() override;
	bool VidInit() override;
	bool Draw( float flTime ) override;
	void Reset() override;
	int MsgFunc_Flashlight( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_FlashBat( const char *pszName, int iSize, void *pbuf );

private:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	HSPRITE m_hBeam;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	wrect_t *m_prcBeam;
	float m_flBat;
	int	  m_iBat;
	int	  m_fOn;
	float m_fFade;
	int	  m_iWidth;		// width of the battery innards
};

#endif //GAME_CLIENT_UI_HUD_CHUDFLASHLIGHT_H