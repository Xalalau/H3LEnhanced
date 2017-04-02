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
#ifndef GAME_CLIENT_UI_HUD_CHUDSAYTEXT_H
#define GAME_CLIENT_UI_HUD_CHUDSAYTEXT_H

struct cvar_t;

class CHudSayText : public CHudBase
{
private:
	// ############ hu3lifezado ############ //
	// Numero de linhas aumentado de 5 para 8
	static const size_t MAX_LINES = 8;
	// ############ //
	/**
	*	it can be less than this, depending on char size
	*/
	static const size_t MAX_CHARS_PER_LINE = 256;

public:
	bool Init() override;
	void InitHUDData() override;
	bool VidInit() override;
	bool Draw( float flTime ) override;
	int MsgFunc_SayText( const char *pszName, int iSize, void *pbuf );
	void SayTextPrint( const char *pszBuf, size_t uiBufSize, int clientIndex = -1 );
	void EnsureTextFitsInOneLineAndWrapIfHaveTo( int line );
	friend class CHudSpectator;

private:
	int ScrollTextUp();

private:

	cvar_t*	m_HUD_saytext;
	cvar_t*	m_HUD_saytext_time;

	char m_szLineBuffer[ MAX_LINES + 1 ][ MAX_CHARS_PER_LINE ] = {};
	const Vector* m_pvecNameColors[ MAX_LINES + 1 ] = {};
	int m_iNameLengths[ MAX_LINES + 1 ] = {};

	// the time at which the lines next scroll up
	float m_flScrollTime = 0;

	int m_iYStart = 0;
	int m_iLineHeight = 0;
};

#endif //GAME_CLIENT_UI_HUD_CHUDSAYTEXT_H