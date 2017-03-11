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

class CHudTextMessage : public CHudBase
{
public:
	bool Init() override;
	static char *LocaliseTextString( const char *msg, char *dst_buffer, int buffer_size );
	static char *BufferedLocaliseTextString( const char *msg );
	const char* LookupString( const char* msg_name, int *msg_dest = NULL );
	int MsgFunc_TextMsg( const char *pszName, int iSize, void *pbuf );
};

// ############ hu3lifezado ############ //
// Nova funcao para facilitar a escrita de mensagens
void hu3_mensagem(char string[128], int opcao);
// ############ //

#endif //GAME_CLIENT_UI_HUD_CHUDTEXTMESSAGE_H