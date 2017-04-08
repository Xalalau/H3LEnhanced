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
//
// death notice
//
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#include "vgui_TeamFortressViewport.h"

#include "CHudDeathNotice.h"

#define DEATHNOTICE_TOP		32

CHudDeathNotice::CHudDeathNotice( const char* const pszName, CHLHud& hud )
	: BaseClass( pszName, hud )
{
}

void CHudDeathNotice::Init()
{
	HOOK_MESSAGE( DeathMsg );

	m_phud_deathnotice_time = CVAR_CREATE( "hud_deathnotice_time", "6", 0 );
}


void CHudDeathNotice::InitHUDData()
{
	memset( m_rgDeathNoticeList, 0, sizeof( m_rgDeathNoticeList ) );
}


void CHudDeathNotice::VidInit()
{
	m_HUD_d_skull = GetHud().GetSpriteIndex( "d_skull" );
}

bool CHudDeathNotice::Draw( float flTime )
{
	int x, y, r, g, b;

	for ( int i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( m_rgDeathNoticeList[i].iId == 0 )
			break;  // we've gone through them all

		if ( m_rgDeathNoticeList[i].flDisplayTime < flTime )
		{ // display time has expired
			// remove the current item from the list
			memmove( &m_rgDeathNoticeList[i], &m_rgDeathNoticeList[i+1], sizeof(DeathNoticeItem) * (MAX_DEATHNOTICES - i) );
			i--;  // continue on the next item;  stop the counter getting incremented
			continue;
		}

		m_rgDeathNoticeList[i].flDisplayTime = min( m_rgDeathNoticeList[i].flDisplayTime, Hud().GetTime() + m_phud_deathnotice_time->value );

		// Only draw if the viewport will let me
		if ( gViewPort && gViewPort->AllowedToPrintText() )
		{
			// Draw the death notice
			y = DEATHNOTICE_TOP + 2 + (20 * i);  //!!!

			int id = ( m_rgDeathNoticeList[i].iId == -1) ? m_HUD_d_skull : m_rgDeathNoticeList[i].iId;
			x = ScreenWidth - ConsoleStringLen( m_rgDeathNoticeList[i].szVictim) - ( GetHud().GetSpriteRect(id).right - GetHud().GetSpriteRect(id).left);

			if ( !m_rgDeathNoticeList[i].bSuicide )
			{
				x -= (5 + ConsoleStringLen( m_rgDeathNoticeList[i].szKiller ) );

				// Draw killers name
				if ( m_rgDeathNoticeList[i].KillerColor )
					gEngfuncs.pfnDrawSetTextColor( ( *m_rgDeathNoticeList[i].KillerColor )[0], ( *m_rgDeathNoticeList[ i ].KillerColor )[1], ( *m_rgDeathNoticeList[ i ].KillerColor )[2] );
				x = 5 + DrawConsoleString( x, y, m_rgDeathNoticeList[i].szKiller );
			}

			r = 255;  g = 80;	b = 0;
			if ( m_rgDeathNoticeList[i].bTeamKill )
			{
				r = 10;	g = 240; b = 10;  // display it in sickly green
			}

			// Draw death weapon
			SPR_Set( GetHud().GetSprite(id), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetHud().GetSpriteRect(id) );

			x += ( GetHud().GetSpriteRect(id).right - GetHud().GetSpriteRect(id).left);

			// Draw victims name (if it was a player that was killed)
			if ( !m_rgDeathNoticeList[i].bNonPlayerKill )
			{
				if ( m_rgDeathNoticeList[i].VictimColor )
					gEngfuncs.pfnDrawSetTextColor( ( *m_rgDeathNoticeList[i].VictimColor )[0], ( *m_rgDeathNoticeList[ i ].VictimColor )[1], ( *m_rgDeathNoticeList[ i ].VictimColor )[2] );
				x = DrawConsoleString( x, y, m_rgDeathNoticeList[i].szVictim );
			}
		}
	}

	return true;
}

// This message handler may be better off elsewhere
void CHudDeathNotice::MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf )
{
	GetFlags() |= HUD_ACTIVE;

	CBufferReader reader( pbuf, iSize );

	int killer = reader.ReadByte();
	int victim = reader.ReadByte();

	char killedwith[32];
	strcpy( killedwith, "d_" );
	strncat( killedwith, reader.ReadString(), sizeof( killedwith ) - strlen( killedwith ) - 1 );

	if (gViewPort)
		gViewPort->DeathMsg( killer, victim );

	if( auto pSpectator = GETHUDCLASS( CHudSpectator ) )
		pSpectator->DeathMessage(victim);
	int i;
	for ( i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( m_rgDeathNoticeList[i].iId == 0 )
			break;
	}
	if ( i == MAX_DEATHNOTICES )
	{ // move the rest of the list forward to make room for this item
		memmove( m_rgDeathNoticeList, m_rgDeathNoticeList +1, sizeof(DeathNoticeItem) * MAX_DEATHNOTICES );
		i = MAX_DEATHNOTICES - 1;
	}

	if (gViewPort)
		gViewPort->GetAllPlayersInfo();

	// Get the Killer's name
	const char* killer_name = g_PlayerInfoList[ killer ].name;
	if ( !killer_name )
	{
		killer_name = "";
		m_rgDeathNoticeList[i].szKiller[0] = 0;
	}
	else
	{
		m_rgDeathNoticeList[i].KillerColor = &GetClientColor( killer );
		strncpy( m_rgDeathNoticeList[i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH );
		m_rgDeathNoticeList[i].szKiller[MAX_PLAYER_NAME_LENGTH-1] = 0;
	}

	// Get the Victim's name
	const char* victim_name = nullptr;
	// If victim is -1, the killer killed a specific, non-player object (like a sentrygun)
	if ( ((char)victim) != -1 )
		victim_name = g_PlayerInfoList[ victim ].name;
	if ( !victim_name )
	{
		victim_name = "";
		m_rgDeathNoticeList[i].szVictim[0] = 0;
	}
	else
	{
		m_rgDeathNoticeList[i].VictimColor = &GetClientColor( victim );
		strncpy( m_rgDeathNoticeList[i].szVictim, victim_name, MAX_PLAYER_NAME_LENGTH );
		m_rgDeathNoticeList[i].szVictim[MAX_PLAYER_NAME_LENGTH-1] = 0;
	}

	// Is it a non-player object kill?
	if ( ((char)victim) == -1 )
	{
		m_rgDeathNoticeList[i].bNonPlayerKill = true;

		// Store the object's name in the Victim slot (skip the d_ bit)
		strcpy( m_rgDeathNoticeList[i].szVictim, killedwith+2 );
	}
	else
	{
		if ( killer == victim || killer == 0 )
			m_rgDeathNoticeList[i].bSuicide = true;

		if ( !strcmp( killedwith, "d_teammate" ) )
			m_rgDeathNoticeList[i].bTeamKill = true;
	}

	// Find the sprite in the list
	int spr = GetHud().GetSpriteIndex( killedwith );

	m_rgDeathNoticeList[i].iId = spr;

	m_rgDeathNoticeList[i].flDisplayTime = Hud().GetTime() + m_phud_deathnotice_time->value;

	if ( m_rgDeathNoticeList[i].bNonPlayerKill )
	{
		ConsolePrint( m_rgDeathNoticeList[i].szKiller );
		ConsolePrint( " killed a " );
		ConsolePrint( m_rgDeathNoticeList[i].szVictim );
		ConsolePrint( "\n" );
	}
	else
	{
		// record the death notice in the console
		if ( m_rgDeathNoticeList[i].bSuicide )
		{
			ConsolePrint( m_rgDeathNoticeList[i].szVictim );

			if ( !strcmp( killedwith, "d_world" ) )
			{
				ConsolePrint( " died" );
			}
			else
			{
				ConsolePrint( " killed self" );
			}
		}
		else if ( m_rgDeathNoticeList[i].bTeamKill )
		{
			ConsolePrint( m_rgDeathNoticeList[i].szKiller );
			ConsolePrint( " killed his teammate " );
			ConsolePrint( m_rgDeathNoticeList[i].szVictim );
		}
		else
		{
			ConsolePrint( m_rgDeathNoticeList[i].szKiller );
			ConsolePrint( " killed " );
			ConsolePrint( m_rgDeathNoticeList[i].szVictim );
		}

		if ( *killedwith && (*killedwith > 13 ) && strcmp( killedwith, "d_world" ) && !m_rgDeathNoticeList[i].bTeamKill )
		{
			ConsolePrint( " with " );

			// replace the code names with the 'real' names
			if ( !strcmp( killedwith+2, "egon" ) )
				strcpy( killedwith, "d_gluon gun" );
			if ( !strcmp( killedwith+2, "gauss" ) )
				strcpy( killedwith, "d_tau cannon" );

			ConsolePrint( killedwith+2 ); // skip over the "d_" part
		}

		ConsolePrint( "\n" );
	}
}




