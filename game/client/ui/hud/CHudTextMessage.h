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
#ifndef GAME_CLIENT_UI_HUD_CHUDTEXTMESSAGE_H
#define GAME_CLIENT_UI_HUD_CHUDTEXTMESSAGE_H

#include "shared/hud/CHudElement.h"
#include "hud.h"

class CHudTextMessage : public CBaseHudElement<CHLHud>
{
public:
	DECLARE_CLASS( CHudTextMessage, CBaseHudElement<CHLHud> );

	CHudTextMessage( const char* const pszName, CHLHud& hud );

	void Init() override;
	void MsgFunc_TextMsg( const char *pszName, int iSize, void *pbuf );
	// ############ hu3lifezado ############ //
	// Nova funcao para facilitar a escrita de mensagens
	void hu3_mensagem(char string[128], int opcao);
	// ############ //
};

#endif //GAME_CLIENT_UI_HUD_CHUDTEXTMESSAGE_H