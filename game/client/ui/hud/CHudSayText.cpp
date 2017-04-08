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
// saytext.cpp
//
// implementation of CHudSayText class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#include "vgui_TeamFortressViewport.h"

#include "CHudSayText.h"

// allow 20 pixels on either side of the text
#define MAX_LINE_WIDTH  ( ScreenWidth - 40 )
#define LINE_START  10

CHudSayText::CHudSayText( const char* const pszName, CHLHud& hud )
	: BaseClass( pszName, hud )
{
}

void CHudSayText::Init()
{
	HOOK_MESSAGE( SayText );

	InitHUDData();

	m_HUD_saytext =			gEngfuncs.pfnRegisterVariable( "hud_saytext", "1", 0 );
	m_HUD_saytext_time =	gEngfuncs.pfnRegisterVariable( "hud_saytext_time", "5", 0 );

	GetFlags() |= HUD_INTERMISSION; // is always drawn during an intermission
}


void CHudSayText::InitHUDData()
{
	memset( m_szLineBuffer, 0, sizeof( m_szLineBuffer ) );
	memset( m_pvecNameColors, 0, sizeof( m_pvecNameColors ) );
	memset( m_iNameLengths, 0, sizeof( m_iNameLengths ) );
}

void CHudSayText::VidInit()
{
}


int CHudSayText::ScrollTextUp()
{
	ConsolePrint( m_szLineBuffer[0] ); // move the first line into the console buffer
	m_szLineBuffer[MAX_LINES][0] = '\0';
	memmove( m_szLineBuffer[0], m_szLineBuffer[1], sizeof(m_szLineBuffer) - sizeof(m_szLineBuffer[0]) ); // overwrite the first line
	memmove( &m_pvecNameColors[0], &m_pvecNameColors[1], sizeof( m_pvecNameColors ) - sizeof( m_pvecNameColors[0]) );
	memmove( &m_iNameLengths[0], &m_iNameLengths[1], sizeof(m_iNameLengths) - sizeof(m_iNameLengths[0]) );
	m_szLineBuffer[MAX_LINES-1][0] = '\0';

	if ( m_szLineBuffer[0][0] == ' ' ) // also scroll up following lines
	{
		m_szLineBuffer[0][0] = 2;
		return 1 + ScrollTextUp();
	}

	return 1;
}

bool CHudSayText::Draw( float flTime )
{
	int y = m_iYStart;

	if ( ( gViewPort && !gViewPort->AllowedToPrintText() ) || !m_HUD_saytext->value )
		return true;

	// make sure the scrolltime is within reasonable bounds,  to guard against the clock being reset
	m_flScrollTime = min( m_flScrollTime, flTime + m_HUD_saytext_time->value );

	// make sure the scrolltime is within reasonable bounds,  to guard against the clock being reset
	m_flScrollTime = min( m_flScrollTime, flTime + m_HUD_saytext_time->value );

	if ( m_flScrollTime <= flTime )
	{
		if ( *m_szLineBuffer[0] )
		{
			m_flScrollTime = flTime + m_HUD_saytext_time->value;
			// push the console up
			ScrollTextUp();
		}
		else
		{ // buffer is empty,  just disable drawing of this section
			GetFlags() &= ~HUD_ACTIVE;
		}
	}

	//Must fit either an entire saytext line or an entire player name. - Solokiller
	char szBuffer[ MAX_CHARS_PER_LINE > MAX_PLAYER_NAME_LENGTH ? MAX_CHARS_PER_LINE : MAX_PLAYER_NAME_LENGTH ];

	for ( int i = 0; i < MAX_LINES; i++ )
	{
		if ( *m_szLineBuffer[i] )
		{
			if ( *m_szLineBuffer[i] == 2 && m_pvecNameColors[i] )
			{
				// it's a saytext string

				// draw the first x characters in the player color
				strncpy( szBuffer, m_szLineBuffer[i], min(m_iNameLengths[i], MAX_PLAYER_NAME_LENGTH+32) );
				szBuffer[ min(m_iNameLengths[i], MAX_PLAYER_NAME_LENGTH+31) ] = '\0';
				gEngfuncs.pfnDrawSetTextColor( *( m_pvecNameColors[ i ] )[0], *( m_pvecNameColors[ i ] )[1], *( m_pvecNameColors[ i ] )[2] );

				int x = DrawConsoleString( LINE_START, y, szBuffer + 1 ); // don't draw the control code at the start
				strncpy( szBuffer, m_szLineBuffer[i] + m_iNameLengths[i], strlen( m_szLineBuffer[i] ));
				szBuffer[ strlen( m_szLineBuffer[i] + m_iNameLengths[i] ) - 1 ] = '\0';
				// color is reset after each string draw
				gEngfuncs.pfnDrawSetTextColor( g_ColorYellow[ 0 ], g_ColorYellow[ 1 ], g_ColorYellow[ 2 ] );
				DrawConsoleString( x, y, szBuffer );
			}
			else
			{
				// normal draw
				gEngfuncs.pfnDrawSetTextColor( g_ColorYellow[ 0 ], g_ColorYellow[ 1 ], g_ColorYellow[ 2 ] );
				DrawConsoleString( LINE_START, y, m_szLineBuffer[i] );
			}
		}

		y += m_iLineHeight;
	}

	return true;
}

void CHudSayText::MsgFunc_SayText( const char *pszName, int iSize, void *pbuf )
{
	CBufferReader reader( pbuf, iSize );

	int client_index = reader.ReadByte();		// the client who spoke the message
	SayTextPrint( reader.ReadString(), iSize - 1,  client_index );
}

void CHudSayText :: SayTextPrint( const char *pszBuf, size_t uiBufSize, int clientIndex )
{
	if ( gViewPort && !gViewPort->AllowedToPrintText() )
	{
		// Print it straight to the console
		ConsolePrint( pszBuf );
		return;
	}

	int i;
	// find an empty string slot
	for ( i = 0; i < MAX_LINES; i++ )
	{
		if ( ! *m_szLineBuffer[i] )
			break;
	}
	if ( i == MAX_LINES )
	{
		// force scroll buffer up
		ScrollTextUp();
		i = MAX_LINES - 1;
	}

	m_iNameLengths[i] = 0;
	m_pvecNameColors[i] = nullptr;

	// if it's a say message, search for the players name in the string
	if ( *pszBuf == 2 && clientIndex > 0 )
	{
		gEngfuncs.pfnGetPlayerInfo( clientIndex, &g_PlayerInfoList[clientIndex] );
		const char *pName = g_PlayerInfoList[clientIndex].name;

		if ( pName )
		{
			const char *nameInString = strstr( pszBuf, pName );

			if ( nameInString )
			{
				m_iNameLengths[i] = strlen( pName ) + (nameInString - pszBuf);
				m_pvecNameColors[i] = &GetClientColor( clientIndex );
			}
		}
	}

	//Need to assign the constant to a local here because std::max takes it by reference, which won't work for this constant.
	//Static const integrals initialized in their declaration have no address, which causes segfaults on Linux.
	//See https://www.reddit.com/r/cpp_questions/comments/510sdc/strange_segfault_under_gcc_using_stdmax_and
	// - Solokiller
	const size_t uiMax = MAX_CHARS_PER_LINE;

	strncpy( m_szLineBuffer[i], pszBuf, max(uiBufSize , uiMax ) );

	// make sure the text fits in one line
	EnsureTextFitsInOneLineAndWrapIfHaveTo( i );

	// Set scroll time
	if ( i == 0 )
	{
		m_flScrollTime = Hud().GetTime() + m_HUD_saytext_time->value;
	}

	GetFlags() |= HUD_ACTIVE;
	PlaySound( "misc/talk.wav", 1 );

	// ############ hu3lifezado ############ //
	// Mensagens de chat agoram aparecem 40 unidades acima do normal
	m_iYStart = ScreenHeight - 60 - 40 - ( m_iLineHeight * (MAX_LINES+2) );
	// ############ //
}

void CHudSayText :: EnsureTextFitsInOneLineAndWrapIfHaveTo( int line )
{
	int line_width = 0;
	GetConsoleStringSize( m_szLineBuffer[line], &line_width, &m_iLineHeight );

	if ( (line_width + LINE_START) > MAX_LINE_WIDTH )
	{ // string is too long to fit on line
		// scan the string until we find what word is too long,  and wrap the end of the sentence after the word
		int length = LINE_START;
		int tmp_len = 0;
		char *last_break = NULL;
		for ( char *x = m_szLineBuffer[line]; *x != 0; x++ )
		{
			// check for a color change, if so skip past it
			if ( x[0] == '/' && x[1] == '(' )
			{
				x += 2;
				// skip forward until past mode specifier
				while ( *x != 0 && *x != ')' )
					x++;

				if ( *x != 0 )
					x++;

				if ( *x == 0 )
					break;
			}

			char buf[2];
			buf[1] = 0;

			if ( *x == ' ' && x != m_szLineBuffer[line] )  // store each line break,  except for the very first character
				last_break = x;

			buf[0] = *x;  // get the length of the current character
			GetConsoleStringSize( buf, &tmp_len, &m_iLineHeight );
			length += tmp_len;

			if ( length > MAX_LINE_WIDTH )
			{  // needs to be broken up
				if ( !last_break )
					last_break = x-1;

				x = last_break;

				// find an empty string slot
				int j;
				do 
				{
					for ( j = 0; j < MAX_LINES; j++ )
					{
						if ( ! *m_szLineBuffer[j] )
							break;
					}
					if ( j == MAX_LINES )
					{
						// need to make more room to display text, scroll stuff up then fix the pointers
						int linesmoved = ScrollTextUp();
						line -= linesmoved;
						last_break = last_break - (sizeof(m_szLineBuffer[0]) * linesmoved);
					}
				}
				while ( j == MAX_LINES );

				// copy remaining string into next buffer,  making sure it starts with a space character
				if ( (char)*last_break == (char)' ' )
				{
					int linelen = strlen(m_szLineBuffer[j]);
					int remaininglen = strlen(last_break);

					if ( (linelen - remaininglen) <= MAX_CHARS_PER_LINE )
						strcat( m_szLineBuffer[j], last_break );
				}
				else
				{
					if ( (strlen(m_szLineBuffer[j]) - strlen(last_break) - 2) < MAX_CHARS_PER_LINE )
					{
						strcat( m_szLineBuffer[j], " " );
						strcat( m_szLineBuffer[j], last_break );
					}
				}

				*last_break = 0; // cut off the last string

				EnsureTextFitsInOneLineAndWrapIfHaveTo( j );
				break;
			}
		}
	}
}
