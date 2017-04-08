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
// CBaseSpectator

// YWB:  UNDONE

// Spectator functions
// 
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"entities/NPCs/Monsters.h"
#include	"CBaseSpectator.h"

/*
===========
SpectatorConnect

called when a spectator connects to a server
============
*/
void CBaseSpectator::SpectatorConnect()
{
	GetFlags().Set( FL_SPECTATOR );
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NOCLIP );
	
	m_hGoalEnt = nullptr;
}

/*
===========
SpectatorDisconnect

called when a spectator disconnects from a server
============
*/
void CBaseSpectator::SpectatorDisconnect()
{
}

/*
================
SpectatorImpulseCommand

Called by SpectatorThink if the spectator entered an impulse
================
*/
void CBaseSpectator::SpectatorImpulseCommand()
{
	static EHANDLE hGoal = nullptr;
	CBaseEntity* pPreviousGoal;
	CBaseEntity* pCurrentGoal;
	bool bFound;
	
	switch( GetImpulse() )
	{
	case 1:
		// teleport the spectator to the next spawn point
		// note that if the spectator is tracking, this doesn't do
		// much
		pPreviousGoal = hGoal;
		pCurrentGoal  = hGoal;
		// Start at the current goal, skip the world, and stop if we looped
		//  back around

		bFound = false;
		while (1)
		{
			pCurrentGoal = UTIL_FindEntityByClassname(pCurrentGoal, "info_player_deathmatch");
			// Looped around, failure
			if (pCurrentGoal == pPreviousGoal)
			{
				ALERT(at_console, "Could not find a spawn spot.\n");
				break;
			}
			// Found a non-world entity, set success, otherwise, look for the next one.
			if (!FNullEnt(pCurrentGoal))
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)  // Didn't find a good spot.
			break;
		
		hGoal = pCurrentGoal;
		SetAbsOrigin( pCurrentGoal->GetAbsOrigin() );
		pev->angles = pCurrentGoal->GetAbsAngles();
		SetFixAngleMode( FIXANGLE_NO );
		break;
	default:
		ALERT(at_console, "Unknown spectator impulse\n");
		break;
	}

	GetImpulse().Set( 0 );
}

/*
================
SpectatorThink

Called every frame after physics are run
================
*/
void CBaseSpectator::SpectatorThink()
{
	if( !GetFlags().Any( FL_SPECTATOR ) )
	{
		GetFlags().Set( FL_SPECTATOR );
	}

	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NOCLIP );

	if( GetImpulse() )
		SpectatorImpulseCommand();
}

/*
===========
Spawn

  Called when spectator is initialized:
  UNDONE:  Is this actually being called because spectators are not allocated in normal fashion?
============
*/
void CBaseSpectator::Spawn()
{
	GetFlags().Set( FL_SPECTATOR );
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NOCLIP );
	
	m_hGoalEnt = nullptr;
}
