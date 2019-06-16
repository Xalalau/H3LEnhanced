/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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

#ifndef GAME_SERVER_SERVER_H
#define GAME_SERVER_SERVER_H

void GameDLLInit();

void GameDLLShutdown();

//Macros to make skill cvars easier to define
#define DECLARE_SKILL_CVARS( name )					\
cvar_t	sk_##name##1 = { "sk_" #name "1", "0" };	\
cvar_t	sk_##name##2 = { "sk_" #name "2", "0" };	\
cvar_t	sk_##name##3 = { "sk_" #name "3", "0" }

#define REGISTER_SKILL_CVARS( name )	\
CVAR_REGISTER( &sk_##name##1 );			\
CVAR_REGISTER( &sk_##name##2 );			\
CVAR_REGISTER( &sk_##name##3 )

extern cvar_t g_DummyCvar;

extern cvar_t	displaysoundlist;
extern cvar_t	coop;

// multiplayer server rules
extern cvar_t	teamplay;
extern cvar_t	fraglimit;
extern cvar_t	timelimit;
extern cvar_t	friendlyfire;
extern cvar_t	falldamage;
extern cvar_t	weaponstay;
extern cvar_t	forcerespawn;
extern cvar_t	flashlight;
extern cvar_t	aimcrosshair;
extern cvar_t	decalfrequency;
extern cvar_t	teamlist;
extern cvar_t	teamoverride;
extern cvar_t	defaultteam;
extern cvar_t	allowmonsters;
extern cvar_t	sv_new_impulse_check;
extern cvar_t	server_cfg;

// Engine Cvars
extern cvar_t	*g_psv_gravity;
extern cvar_t	*g_psv_aim;
extern cvar_t	*g_footsteps;

#endif //GAME_SERVER_SERVER_H
