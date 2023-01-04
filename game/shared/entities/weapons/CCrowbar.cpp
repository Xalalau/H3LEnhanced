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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"
#include "CCrowbar.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"
#include "gamerules/GameRules.h"

// ############ hu3lifezado ############ //
// Tiro secundario, adaptado de:
// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
#ifndef CLIENT_DLL
#include "entities/weapons/CFlyingCrowbar.h"
#endif
// ############ //

BEGIN_DATADESC( CCrowbar )
	DEFINE_THINKFUNC( SwingAgain ),
	DEFINE_THINKFUNC( Smack ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( weapon_crowbar, CCrowbar );

CCrowbar::CCrowbar()
	: BaseClass( WEAPON_CROWBAR )
{
}

void CCrowbar::Spawn( )
{
	Precache( );
	SetModel( "models/w_crowbar.mdl");
	m_iClip = -1;

	FallInit();// get ready to fall down.
}


void CCrowbar::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/v_crowbar.mdl");
	PRECACHE_MODEL("models/w_crowbar.mdl");
	PRECACHE_MODEL("models/p_crowbar.mdl");
	PRECACHE_SOUND("weapons/cbar_hit1.wav");
	PRECACHE_SOUND("weapons/cbar_hit2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod1.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod3.wav");
	PRECACHE_SOUND("weapons/cbar_miss1.wav");

	m_usCrowbar = PRECACHE_EVENT ( 1, "events/crowbar.sc" );

	// ############ hu3lifezado ############ //
	// Tiro secundario, adaptado de:
	// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
	UTIL_PrecacheOther("flying_crowbar");
	// ############ //
}

bool CCrowbar::Deploy()
{
	return DefaultDeploy( "models/v_crowbar.mdl", "models/p_crowbar.mdl", CROWBAR_DRAW, "crowbar" );
}

void CCrowbar::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( CROWBAR_HOLSTER );
}

void CCrowbar::PrimaryAttack()
{
	if( !Swing( true ) )
	{
#ifndef CLIENT_DLL
		SetThink( &CCrowbar::SwingAgain );
		SetNextThink( gpGlobals->time + 0.1 );
#endif
	}
}

// ############ hu3lifezado ############ //
// Tiro secundario, adaptado de:
// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
void CCrowbar::SecondaryAttack()
{
	// Don't throw underwater, and only throw if we were able to detatch 
	// from player.
	if ((m_pPlayer->pev->waterlevel != 3)) // && (m_pPlayer->RemovePlayerItem(this)))
	{
		// Get the origin, direction, and fix the angle of the throw.
		Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_right * 8 + gpGlobals->v_forward * 16;
		Vector vecDir = gpGlobals->v_forward;
		Vector vecAng = UTIL_VecToAngles(vecDir);
		vecAng.z = vecDir.z - 90;

#ifndef CLIENT_DLL
		// Create a flying crowbar.
		CFlyingCrowbar *pFCBar = (CFlyingCrowbar *)Create("flying_crowbar", vecSrc, Vector(0, 0, 0), m_pPlayer->edict());

		// Give the crowbar its velocity, angle, and spin. 
		// Lower the gravity a bit, so it flys. 
		pFCBar->pev->velocity = vecDir * 500 + m_pPlayer->pev->velocity;
		pFCBar->pev->angles = vecAng;
		pFCBar->pev->avelocity.x = -1000;
		pFCBar->pev->gravity = .5;
		pFCBar->m_pPlayer = m_pPlayer;

		// Do player weapon anim and sound effect. 
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
		EMIT_SOUND_DYN(this, CHAN_WEAPON, "weapons/cbar_miss1.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0, 0xF));
#endif

		// Just for kicks, set this. 
		// But we destroy this weapon anyway so... thppt. 
		m_flNextSecondaryAttack = GetNextAttackDelay(0.25);

		// take item off hud
		//m_pPlayer->pev->weapons &= ~(1 << this->m_iId);

		// Destroy this weapon
		//DestroyItem();

		switch (((m_iSwing++) % 2) + 1)
		{
		case 0:
			SendWeaponAnim(CROWBAR_ATTACK1HIT); break;
		case 1:
			SendWeaponAnim(CROWBAR_ATTACK2HIT); break;
		case 2:
			SendWeaponAnim(CROWBAR_ATTACK3HIT); break;
		}
	}
}
// ############ //

void CCrowbar::Smack( )
{
	DecalGunshot( &m_trHit, BULLET_PLAYER_CROWBAR );
}


void CCrowbar::SwingAgain( void )
{
	Swing( false );
}


bool CCrowbar::Swing( const bool bFirst )
{
	bool bDidHit = false;

	TraceResult tr;

	UTIL_MakeVectors( m_pPlayer->GetViewAngle() );
	Vector vecSrc	= m_pPlayer->GetGunPosition( );
	Vector vecEnd	= vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

#ifndef CLIENT_DLL
	if ( tr.flFraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, Hull::HEAD, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( !pHit || pHit->IsBSPModel() )
				FindHullIntersection( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer );
			vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	if( bFirst )
	{
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usCrowbar, 
							 0.0, g_vecZero, g_vecZero, 0, 0, 0,
							 0.0, 0, 0.0 );
	}


	if ( tr.flFraction >= 1.0 )
	{
		if( bFirst )
		{
			// miss
			m_flNextPrimaryAttack = GetNextAttackDelay(0.5);
			
			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		}
	}
	else
	{
		switch( ((m_iSwing++) % 2) + 1 )
		{
		case 0:
			SendWeaponAnim( CROWBAR_ATTACK1HIT ); break;
		case 1:
			SendWeaponAnim( CROWBAR_ATTACK2HIT ); break;
		case 2:
			SendWeaponAnim( CROWBAR_ATTACK3HIT ); break;
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		
#ifndef CLIENT_DLL

		// hit
		bDidHit = true;
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		if( pEntity )
		{
			g_MultiDamage.Clear( );

			if ( (m_flNextPrimaryAttack + 1 < UTIL_WeaponTimeBase() ) || g_pGameRules->IsMultiplayer() )
			{
				// first swing does full damage
				pEntity->TraceAttack( CTakeDamageInfo( m_pPlayer, gSkillData.GetPlrDmgCrowbar(), DMG_CLUB ), gpGlobals->v_forward, tr );
			}
			else
			{
				// subsequent swings do half
				pEntity->TraceAttack( CTakeDamageInfo( m_pPlayer, gSkillData.GetPlrDmgCrowbar() / 2, DMG_CLUB ), gpGlobals->v_forward, tr );
			}	
			g_MultiDamage.ApplyMultiDamage( m_pPlayer, m_pPlayer );
		}

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		bool bHitWorld = true;

		if (pEntity)
		{
			if ( pEntity->Classify() != EntityClassifications().GetNoneId() && pEntity->Classify() != EntityClassifications().GetClassificationId( classify::MACHINE ) )
			{
				// play thwack or smack sound
				switch( RANDOM_LONG(0,2) )
				{
				case 0:
					EMIT_SOUND( m_pPlayer, CHAN_ITEM, "weapons/cbar_hitbod1.wav", 1, ATTN_NORM); break;
				case 1:
					EMIT_SOUND( m_pPlayer, CHAN_ITEM, "weapons/cbar_hitbod2.wav", 1, ATTN_NORM); break;
				case 2:
					EMIT_SOUND( m_pPlayer, CHAN_ITEM, "weapons/cbar_hitbod3.wav", 1, ATTN_NORM); break;
				}
				m_pPlayer->m_iWeaponVolume = MELEE_BODYHIT_VOLUME;
				if ( !pEntity->IsAlive() )
					  return true;
				else
					  flVol = 0.1;

				bHitWorld = false;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		if( bHitWorld )
		{
			float fvolbar = TEXTURETYPE_PlaySound(tr, vecSrc, vecSrc + (vecEnd-vecSrc)*2, BULLET_PLAYER_CROWBAR);

			if ( g_pGameRules->IsMultiplayer() )
			{
				// override the volume here, cause we don't play texture sounds in multiplayer, 
				// and fvolbar is going to be 0 from the above call.

				fvolbar = 1;
			}

			// also play crowbar strike
			switch( RANDOM_LONG(0,1) )
			{
			case 0:
				EMIT_SOUND_DYN( m_pPlayer, CHAN_ITEM, "weapons/cbar_hit1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3)); 
				break;
			case 1:
				EMIT_SOUND_DYN( m_pPlayer, CHAN_ITEM, "weapons/cbar_hit2.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3)); 
				break;
			}

			// delay the decal a bit
			m_trHit = tr;
		}

		m_pPlayer->m_iWeaponVolume = flVol * MELEE_WALLHIT_VOLUME;

		SetThink( &CCrowbar::Smack );
		SetNextThink( UTIL_WeaponTimeBase() + 0.2 );
#endif
		m_flNextPrimaryAttack = GetNextAttackDelay(0.25);
	}
	return bDidHit;
}



