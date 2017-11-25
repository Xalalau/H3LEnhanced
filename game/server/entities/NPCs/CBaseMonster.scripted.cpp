/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
/*


===== scripted.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/NPCs/Monsters.h"

#ifndef ANIMATION_H
#include "animation.h"
#endif

#ifndef SAVERESTORE_H
#include "SaveRestore.h"
#endif

#include "entities/NPCs/Schedule.h"
#include "entities/NPCs/DefaultAI.h"

#include "scripted/CCineMonster.h"

bool CBaseMonster::ExitScriptedSequence()
{
	if ( GetDeadFlag() == DEAD_DYING )
	{
		// is this legal?
		// BUGBUG -- This doesn't call Killed()
		m_IdealMonsterState = MONSTERSTATE_DEAD;
		return false;
	}

	if (m_pCine)
	{
		m_pCine->CancelScript( );
	}

	return true;
}
		
bool CBaseMonster::CineCleanup()
{
	CCineMonster *pOldCine = m_pCine;

	// am I linked to a cinematic?
	if (m_pCine)
	{
		// okay, reset me to what it thought I was before
		m_pCine->m_hTargetEnt = NULL;
		SetMoveType( m_pCine->m_saved_movetype );
		SetSolidType( m_pCine->m_saved_solid );
		GetEffects() = m_pCine->m_saved_effects;
	}
	else
	{
		// arg, punt
		SetMoveType( MOVETYPE_STEP );// this is evil
		SetSolidType( SOLID_SLIDEBOX );
	}
	m_pCine = NULL;
	m_hTargetEnt = NULL;
	m_hGoalEnt = NULL;
	if ( GetDeadFlag() == DEAD_DYING)
	{
		// last frame of death animation?
		SetHealth( 0 );
		SetFrameRate( 0.0 );
		SetSolidType( SOLID_NOT );
		SetState( MONSTERSTATE_DEAD );
		SetDeadFlag( DEAD_DEAD );
		SetSize( GetRelMin(), Vector( GetRelMax().x, GetRelMax().y, GetRelMin().z + 2) );

		if ( pOldCine && pOldCine->GetSpawnFlags().Any( SF_SCRIPT_LEAVECORPSE ) )
		{
			SetUse( NULL );		// BUGBUG -- This doesn't call Killed()
			SetThink( NULL );	// This will probably break some stuff
			SetTouch( NULL );
		}
		else
			SUB_StartFadeOut(); // SetThink( SUB_DoNothing );
		// This turns off animation & physics in case their origin ends up stuck in the world or something
		StopAnimation();
		SetMoveType( MOVETYPE_NONE );
		GetEffects() |= EF_NOINTERP;	// Don't interpolate either, assume the corpse is positioned in its final resting place
		return false;
	}

	// If we actually played a sequence
	if ( pOldCine && pOldCine->m_iszPlay )
	{
		if ( !GetSpawnFlags().Any( SF_SCRIPT_NOSCRIPTMOVEMENT ) )
		{
			// reset position
			Vector new_origin, new_angle;
			GetBonePosition( 0, new_origin, new_angle );

			// Figure out how far they have moved
			// We can't really solve this problem because we can't query the movement of the origin relative
			// to the sequence.  We can get the root bone's position as we do here, but there are
			// cases where the root bone is in a different relative position to the entity's origin
			// before/after the sequence plays.  So we are stuck doing this:

			// !!!HACKHACK: Float the origin up and drop to floor because some sequences have
			// irregular motion that can't be properly accounted for.

			// UNDONE: THIS SHOULD ONLY HAPPEN IF WE ACTUALLY PLAYED THE SEQUENCE.
			Vector oldOrigin = GetAbsOrigin();

			// UNDONE: ugly hack.  Don't move monster if they don't "seem" to move
			// this really needs to be done with the AX,AY,etc. flags, but that aren't consistantly
			// being set, so animations that really do move won't be caught.
			if ((oldOrigin - new_origin).Length2D() < 8.0)
				new_origin = oldOrigin;

			Vector vecOrigin = GetAbsOrigin();

			vecOrigin.x = new_origin.x;
			vecOrigin.y = new_origin.y;
			vecOrigin.z += 1;

			GetFlags() |= FL_ONGROUND;
			const DropToFloor drop = UTIL_DropToFloor( this );
			
			// Origin in solid?  Set to org at the end of the sequence
			if ( drop == DropToFloor::STUCK )
				vecOrigin = oldOrigin;
			else if ( drop == DropToFloor::TOOFAR ) // Hanging in air?
			{
				vecOrigin.z = new_origin.z;
				GetFlags().ClearFlags( FL_ONGROUND );
			}
			// else entity hit floor, leave there

			/*
			Vector vecEntOrigin = pEntity->GetAbsOrigin();
			vecEntOrigin.z = new_origin.z + 5.0; // damn, got to fix this
			pEntity->SetAbsOrigin( vecEntOrigin );
			*/

			SetAbsOrigin( vecOrigin );
			GetEffects() |= EF_NOINTERP;
		}

		// We should have some animation to put these guys in, but for now it's idle.
		// Due to NOINTERP above, there won't be any blending between this anim & the sequence
		m_Activity = ACT_RESET;
	}
	// set them back into a normal state
	pev->enemy = NULL;
	if ( GetHealth() > 0 )
		m_IdealMonsterState = MONSTERSTATE_IDLE; // m_previousState;
	else
	{
		// Dropping out because he got killed
		// Can't call killed() no attacker and weirdness (late gibbing) may result
		m_IdealMonsterState = MONSTERSTATE_DEAD;
		SetConditions( bits_COND_LIGHT_DAMAGE );
		SetDeadFlag( DEAD_DYING );
		FCheckAITrigger();
		SetDeadFlag( DEAD_NO );
	}


	//	SetAnimation( m_MonsterState );
	GetSpawnFlags().ClearFlags( SF_MONSTER_WAIT_FOR_SCRIPT );

	return true;
}