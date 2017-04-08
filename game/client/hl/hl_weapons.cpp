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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"

#include "pm_defs.h"
#include "event_api.h"

#include "hud.h"
#include "cl_util.h"

#include "com_weapons.h"

#include "CClientPrediction.h"

#include "hl_weapons.h"

extern int g_iUser1;

float g_flApplyVel = 0.0;
bool   g_brunninggausspred = false;

Vector previousorigin;

/*
=====================
HUD_PostRunCmd

Client calls this during prediction, after it has moved the player and updated any info changed into to->
time is the current client clock based on prediction
cmd is the command that caused the movement, etc
runfuncs is 1 if this is the first time we've predicted this command.  If so, sounds and effects should play, otherwise, they should
be ignored
=====================
*/
void DLLEXPORT HUD_PostRunCmd( local_state_t *from, local_state_t *to, usercmd_t *cmd, int runfuncs, double time, unsigned int random_seed )
{
	g_runfuncs = runfuncs;

#if defined( CLIENT_WEAPONS )
	if ( cl_lw && cl_lw->value )
	{
		g_Prediction.WeaponsPostThink( from, to, cmd, time, random_seed );
	}
	else
#endif
	{
		to->client.fov = g_lastFOV;
	}

	if ( g_brunninggausspred )
	{
		Vector forward;
		gEngfuncs.pfnAngleVectors( v_angles, forward, NULL, NULL );
		to->client.velocity = to->client.velocity - forward * g_flApplyVel * 5; 
		g_brunninggausspred = false;
	}
	
	// All games can use FOV state
	g_lastFOV = to->client.fov;

	//Update user's FOV now. Fixes crosshairs using the wrong FOV if we just changed weapons. - Solokiller
	Hud().UpdateFOV( g_lastFOV, true );
}

bool bIsMultiplayer()
{
	return Hud().IsMultiplayer();
}
//Just loads a v_ model.
void LoadVModel( const char* const pszViewModel, CBasePlayer* pPlayer )
{
	gEngfuncs.CL_LoadModel( pszViewModel, &pPlayer->pev->viewmodel );
}

void HUD_GetLastOrg( Vector& vecOrigin )
{
	vecOrigin = previousorigin;
}

void HUD_SetLastOrg()
{
	// Offset final origin by view_offset
	previousorigin = g_finalstate->playerstate.origin + g_finalstate->client.view_ofs;
}

void UTIL_ParticleBox( const CBasePlayer* const pPlayer, const Vector& vecMins, const Vector& vecMaxs, float life, unsigned char r, unsigned char g, unsigned char b )
{
	gEngfuncs.pEfxAPI->R_ParticleBox( pPlayer->GetAbsOrigin() + vecMins, pPlayer->GetAbsOrigin() + vecMaxs, 5.0, 0, 255, 0 );
}

void UTIL_ParticleBoxes()
{
	int idx;
	physent_t *pe;
	cl_entity_t *player;
	Vector mins, maxs;

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	player = gEngfuncs.GetLocalPlayer();
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers( player->index - 1 );

	for( idx = 1; idx < 100; idx++ )
	{
		pe = gEngfuncs.pEventAPI->EV_GetPhysent( idx );
		if( !pe )
			break;

		if( pe->info >= 1 && pe->info <= gEngfuncs.GetMaxClients() )
		{
			mins = pe->origin + pe->mins;
			maxs = pe->origin + pe->maxs;

			gEngfuncs.pEfxAPI->R_ParticleBox( mins, maxs, 0, 0, 255, 2.0 );
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

void UTIL_ParticleLine( const Vector& vecStart, const Vector& vecEnd, float life, unsigned char r, unsigned char g, unsigned char b )
{
	gEngfuncs.pEfxAPI->R_ParticleLine( vecStart, vecEnd, r, g, b, life );
}
