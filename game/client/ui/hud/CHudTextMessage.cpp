/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
//
// text_message.cpp
//
// implementation of CHudTextMessage class
//
// this class routes messages through titles.txt for localisation
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"

#include "vgui_TeamFortressViewport.h"

#include "ui/shared/CLocalize.h"

#include "CHudSayText.h"
#include "CHudTextMessage.h"

CHudTextMessage::CHudTextMessage( const char* const pszName, CHLHud& hud )
	: BaseClass( pszName, hud )
{
}

void CHudTextMessage::Init()
{
	HOOK_MESSAGE( TextMsg );

	Reset();
}

// Message handler for text messages
// displays a string, looking them up from the titles.txt file, which can be localised
// parameters:
//   byte:   message direction  ( HUD_PRINTCONSOLE, HUD_PRINTNOTIFY, HUD_PRINTCENTER, HUD_PRINTTALK )
//   string: message
// optional parameters:
//   string: message parameter 1
//   string: message parameter 2
//   string: message parameter 3
//   string: message parameter 4
// any string that starts with the character '#' is a message name, and is used to look up the real message in titles.txt
// the next (optional) one to four strings are parameters for that string (which can also be message names if they begin with '#')
void CHudTextMessage::MsgFunc_TextMsg( const char *pszName, int iSize, void *pbuf )
{
	CBufferReader reader( pbuf, iSize );

	int msg_dest = reader.ReadByte();

	char szBuf[ 5 ][ MSG_BUF_SIZE ];

	char* msg_text = safe_strcpy( szBuf[ 0 ], Localize().LookupString( reader.ReadString(), &msg_dest ), MSG_BUF_SIZE );

	// keep reading strings and using C format strings for subsituting the strings into the localised text string
	char* sstr1 = safe_strcpy( szBuf[ 1 ], Localize().LookupString( reader.ReadString() ), MSG_BUF_SIZE );
	UTIL_StripEndNewlineFromString( sstr1 );  // these strings are meant for subsitution into the main strings, so cull the automatic end newlines
	char* sstr2 = safe_strcpy( szBuf[ 2 ], Localize().LookupString( reader.ReadString() ), MSG_BUF_SIZE );
	UTIL_StripEndNewlineFromString( sstr2 );
	char* sstr3 = safe_strcpy( szBuf[ 3 ], Localize().LookupString( reader.ReadString() ), MSG_BUF_SIZE );
	UTIL_StripEndNewlineFromString( sstr3 );
	char* sstr4 = safe_strcpy( szBuf[ 4 ], Localize().LookupString( reader.ReadString() ), MSG_BUF_SIZE );
	UTIL_StripEndNewlineFromString( sstr4 );

	UTIL_TextMsg( static_cast<ClientPrintDest>( msg_dest ), msg_text, sstr1, sstr2, sstr3, sstr4 );
}

// ############ hu3lifezado ############ //
// Nova funcao para facilitar a escrita de mensagens
void CHudTextMessage::hu3_mensagem(char string[], int opcao)
{
	char string_aux[MSG_BUF_SIZE];

	sprintf(string_aux, "%c%s", opcao, string);

	const char *pszName = NULL;
	int iSize = strlen(string_aux) + 1;
	void *pbuf = string_aux;

	MsgFunc_TextMsg(NULL, iSize, pbuf);

	return;
}
// ############ //