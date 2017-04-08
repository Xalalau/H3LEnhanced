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
#ifndef GAME_CLIENT_UI_HUD_CHUDMESSAGE_H
#define GAME_CLIENT_UI_HUD_CHUDMESSAGE_H

#include "shared/hud/CHudElement.h"
#include "hud.h"

struct message_parms_t
{
	client_textmessage_t	*pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

class CHudMessage : public CBaseHudElement<CHLHud>
{
public:
	DECLARE_CLASS( CHudMessage, CBaseHudElement<CHLHud> );

	CHudMessage( const char* const pszName, CHLHud& hud );

	void Init() override;
	void VidInit() override;
	bool Draw( float flTime ) override;
	void MsgFunc_HudText( const char *pszName, int iSize, void *pbuf );
	void MsgFunc_HudTextPro( const char *pszName, int iSize, void *pbuf );
	void MsgFunc_GameTitle( const char *pszName, int iSize, void *pbuf );

	float FadeBlend( float fadein, float fadeout, float hold, float localTime );
	int	XPosition( float x, int width, int lineWidth );
	int YPosition( float y, int height );

	void MessageAdd( const char *pName, float time );
	void MessageAdd( client_textmessage_t * newMessage );
	void MessageDrawScan( client_textmessage_t *pMessage, float time );
	void MessageScanStart();
	void MessageScanNextChar();
	void Reset() override;

private:
	client_textmessage_t		*m_pMessages[ maxHUDMessages ];
	float						m_startTime[ maxHUDMessages ];
	message_parms_t				m_parms;
	float						m_gameTitleTime;
	client_textmessage_t		*m_pGameTitle;

	int m_HUD_title_life;
	int m_HUD_title_half;

	// 1 Global client_textmessage_t for custom messages that aren't in the titles.txt
	client_textmessage_t m_CustomMessage;
	const char* const m_pszCustomName = "Custom";
	char m_szCustomText[ 1024 ] = {};
};

#endif //GAME_CLIENT_UI_HUD_CHUDMESSAGE_H