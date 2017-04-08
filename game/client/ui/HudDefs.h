#ifndef GAME_CLIENT_UI_HUDDEFS_H
#define GAME_CLIENT_UI_HUDDEFS_H

#include "Color.h"
#include "HudColors.h"

#include "global_consts.h"

class Vector;

#define DHN_DRAWZERO 1
#define DHN_2DIGITS  2
#define DHN_3DIGITS  4
#define MIN_ALPHA	 100

enum HudFlag
{
	HUD_ACTIVE			= 1,
	HUD_INTERMISSION	= 2,
};

#define FADE_TIME 100

#define MAX_SPRITE_NAME_LENGTH	24

const int maxHUDMessages = 16;

const Vector& GetClientColor( int clientIndex );
extern const Vector g_ColorYellow;

struct extra_player_info_t
{
	short frags;
	short deaths;
	short playerclass;
	short health; // UNUSED currently, spectator UI would like this
	bool dead; // UNUSED currently, spectator UI would like this
	short teamnumber;
	char teamname[ MAX_TEAM_NAME ];
};

struct team_info_t
{
	char name[ MAX_TEAM_NAME ];
	short frags;
	short deaths;
	short ping;
	short packetloss;
	short ownteam;
	short players;
	bool already_drawn;
	bool scores_overriden;
	int teamnumber;
};

#include "player_info.h"

#endif //GAME_CLIENT_UI_HUDDEFS_H
