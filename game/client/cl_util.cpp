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
// cl_util.cpp
//
// implementation of class-less helper functions
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>

#include "CHudSayText.h"

#include "vgui_TeamFortressViewport.h"

#include "shared/CLocalize.h"

HSPRITE LoadSprite( const char* const pszName )
{
	char sz[ MAX_PATH ];

	const int iResolution = ScreenWidth < 640 ? 320 : 640;

	const int iResult = snprintf( sz, sizeof( sz ), pszName, iResolution );

	//Handle formatting failure. - Solokiller
	if( iResult < 0 || static_cast<size_t>( iResult ) >= sizeof( sz ) )
	{
		gEngfuncs.Con_Printf( "LoadSprite: Failed to load HUD sprite \"%s\" for resolution %d\n", pszName, iResolution );
		return INVALID_HSPRITE;
	}

	return SPR_Load( sz );
}

/* =================================
GetSpriteList

Finds and returns the matching
sprite name 'psz' and resolution 'iRes'
in the given sprite list 'pList'
iCount is the number of items in the pList
================================= */
client_sprite_t *GetSpriteList( client_sprite_t *pList, const char *psz, int iRes, int iCount )
{
	if( !pList )
		return NULL;

	int i = iCount;
	client_sprite_t *p = pList;

	while( i-- )
	{
		if( ( !strcmp( psz, p->szName ) ) && ( p->iRes == iRes ) )
			return p;
		p++;
	}

	return NULL;
}

void ScaleColors( int& r, int& g, int& b, const int a )
{
	const float x = ( float ) a / 255;
	r = ( int ) ( r * x );
	g = ( int ) ( g * x );
	b = ( int ) ( b * x );
}

bool UTIL_GetPlayerUniqueID( int iPlayer, char playerID[ PLAYERID_BUFFER_SIZE ] )
{
	return !!gEngfuncs.GetPlayerUniqueID( iPlayer, playerID );
}

void UTIL_TextMsg( const ClientPrintDest msgDest,
				   const char* pszMessage,
				   const char* pszString1, const char* pszString2, const char* pszString3, const char* pszString4 )
{
	static char szBuf[ MSG_BUF_SIZE ];

	if( gViewPort && !gViewPort->AllowedToPrintText() )
		return;

	switch( msgDest )
	{
	case HUD_PRINTCENTER:
		safe_sprintf( szBuf, MSG_BUF_SIZE, pszMessage, pszString1, pszString2, pszString3, pszString4 );
		CenterPrint( UTIL_ConvertCRtoNL( szBuf ) );
		break;

	case HUD_PRINTNOTIFY:
		szBuf[ 0 ] = 1;  // mark this message to go into the notify buffer
		safe_sprintf( szBuf + 1, MSG_BUF_SIZE, pszMessage, pszString1, pszString2, pszString3, pszString4 );
		ConsolePrint( UTIL_ConvertCRtoNL( szBuf ) );
		break;

	case HUD_PRINTTALK:
		if( auto pSayText = GETHUDCLASS( CHudSayText ) )
		{
			safe_sprintf( szBuf, MSG_BUF_SIZE, pszMessage, pszString1, pszString2, pszString3, pszString4 );
			pSayText->SayTextPrint( UTIL_ConvertCRtoNL( szBuf ), MSG_BUF_SIZE );
		}
		break;

	case HUD_PRINTCONSOLE:
		safe_sprintf( szBuf, MSG_BUF_SIZE, pszMessage, pszString1, pszString2, pszString3, pszString4 );
		ConsolePrint( UTIL_ConvertCRtoNL( szBuf ) );
		break;
	}
}

void UTIL_LocalizedTextMsg( const ClientPrintDest msgDest,
							const char* pszMessage,
							const char* pszString1, const char* pszString2, const char* pszString3, const char* pszString4 )
{
	if( gViewPort && !gViewPort->AllowedToPrintText() )
		return;

	char szBuf[ 5 ][ MSG_BUF_SIZE ];

	int msg_dest = msgDest;

	char* msg_text = safe_strcpy( szBuf[ 0 ], Localize().LookupString( pszMessage, &msg_dest ), MSG_BUF_SIZE );

	// keep reading strings and using C format strings for subsituting the strings into the localised text string
	char* sstr1 = safe_strcpy( szBuf[ 1 ], Localize().LookupString( pszString1 ), MSG_BUF_SIZE );
	UTIL_StripEndNewlineFromString( sstr1 );  // these strings are meant for subsitution into the main strings, so cull the automatic end newlines
	char* sstr2 = safe_strcpy( szBuf[ 2 ], Localize().LookupString( pszString2 ), MSG_BUF_SIZE );
	UTIL_StripEndNewlineFromString( sstr2 );
	char* sstr3 = safe_strcpy( szBuf[ 3 ], Localize().LookupString( pszString3 ), MSG_BUF_SIZE );
	UTIL_StripEndNewlineFromString( sstr3 );
	char* sstr4 = safe_strcpy( szBuf[ 4 ], Localize().LookupString( pszString4 ), MSG_BUF_SIZE );
	UTIL_StripEndNewlineFromString( sstr4 );

	UTIL_TextMsg( static_cast<ClientPrintDest>( msg_dest ), msg_text, sstr1, sstr2, sstr3, sstr4 );
}

static const char LEVEL_PREFIX[] = "maps/";
static const size_t LEVEL_PREFIX_LENGTH = 5;

static const char LEVEL_SUFFIX[] = ".bsp";
static const size_t LEVEL_SUFFIX_LENGTH = 4;

bool UTIL_GetMapName( char* pszBuffer, const size_t uiSizeInBytes )
{
	ASSERT( pszBuffer );
	ASSERT( uiSizeInBytes > 0 );

	if( !pszBuffer || uiSizeInBytes == 0 )
		return false;

	const char* pszLevelName = gEngfuncs.pfnGetLevelName();
	
	if( !pszLevelName || !( *pszLevelName ) )
		return false;

	auto length = strlen( pszLevelName );

	//Note: the server can use case insensitive names, so make sure to use case insensitive comparison here. - Solokiller

	if( length <= LEVEL_PREFIX_LENGTH || strnicmp( LEVEL_PREFIX, pszLevelName, LEVEL_PREFIX_LENGTH ) )
		return false;

	pszLevelName += LEVEL_PREFIX_LENGTH;

	length -= LEVEL_PREFIX_LENGTH;

	if( length <= LEVEL_SUFFIX_LENGTH )
		return false;

	if( strnicmp( LEVEL_SUFFIX, pszLevelName + ( length - LEVEL_SUFFIX_LENGTH ), LEVEL_SUFFIX_LENGTH ) )
		return false;

	length -= LEVEL_SUFFIX_LENGTH;

	if( length >= uiSizeInBytes )
		return false;

	UTIL_SafeStrncpy( pszBuffer, pszLevelName, length + 1 );

	return true;
}
