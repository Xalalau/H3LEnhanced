#include "cl_dll.h"

#include "HudDefs.h"

const Vector g_ColorBlue	= { 0.6f, 0.8f, 1.0f };
const Vector g_ColorRed		= { 1.0f, 0.25f, 0.25f };
const Vector g_ColorGreen	= { 0.6f, 1.0f, 0.6f };
const Vector g_ColorYellow	= { 1.0f, 0.7f, 0.0f };
const Vector g_ColorGrey	= { 0.8f, 0.8f, 0.8f };

const Vector& GetClientColor( int clientIndex )
{
	switch( g_PlayerExtraInfo[ clientIndex ].teamnumber )
	{
		//TODO: define these team numbers. - Solokiller
	case 1:	return g_ColorBlue;
	case 2: return g_ColorRed;
	case 3: return g_ColorYellow;
	case 4: return g_ColorGreen;
	case 0: return g_ColorYellow;

	default: return g_ColorGrey;
	}
}

hud_player_info_t	 g_PlayerInfoList[ MAX_PLAYERS + 1 ];
extra_player_info_t  g_PlayerExtraInfo[ MAX_PLAYERS + 1 ];
team_info_t			 g_TeamInfo[ MAX_TEAMS + 1 ];
int					 g_IsSpectator[ MAX_PLAYERS + 1 ];
