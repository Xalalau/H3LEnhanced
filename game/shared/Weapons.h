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
#ifndef GAME_SHARED_WEAPONS_H
#define GAME_SHARED_WEAPONS_H

#include "WeaponsConst.h"

#include "weaponinfo.h"

#include "Effects.h"

#ifdef SERVER_DLL
#include "CMultiDamage.h"
#endif

#include "entities/weapons/CAmmoTypes.h"

class CBasePlayer;

/**
*	Actions that can be taken by calls to DeactivateSatchels.
*	@see DeactivateSatchels
*/
enum class SatchelAction
{
	DETONATE = 0,
	RELEASE
};

/**
*	Removes all satchels owned by the provided player.
*	
*	Made this global on purpose.
*	@param pOwner Owner whose satchels will be removed.
*	@param action Action to take.
*	@return Number of satchels that were handled.
*/
size_t DeactivateSatchels( CBasePlayer* const pOwner, const SatchelAction action );

#include "entities/CGrenade.h"

#include "entities/weapons/CBasePlayerWeapon.h"

#include "entities/ammo/CBasePlayerAmmo.h"

extern DLL_GLOBAL	short	g_sModelIndexLaser;// holds the index for the laser beam
extern DLL_GLOBAL	const char *g_pModelNameLaser;

extern DLL_GLOBAL	short	g_sModelIndexLaserDot;// holds the index for the laser beam dot
extern DLL_GLOBAL	short	g_sModelIndexFireball;// holds the index for the fireball
extern DLL_GLOBAL	short	g_sModelIndexSmoke;// holds the index for the smoke cloud
extern DLL_GLOBAL	short	g_sModelIndexWExplosion;// holds the index for the underwater explosion
extern DLL_GLOBAL	short	g_sModelIndexBubbles;// holds the index for the bubbles model
extern DLL_GLOBAL	short	g_sModelIndexBloodDrop;// holds the sprite index for blood drops
extern DLL_GLOBAL	short	g_sModelIndexBloodSpray;// holds the sprite index for blood spray (bigger)

extern void DecalGunshot( TraceResult *pTrace, int iBulletType );
extern void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage);
extern int DamageDecal( CBaseEntity *pEntity, int bitsDamageType );
extern void RadiusDamage( Vector vecSrc, const CTakeDamageInfo& info, float flRadius, int iClassIgnore );

#ifdef SERVER_DLL
extern CMultiDamage g_MultiDamage;
#endif

/**
*	@return Whether this is a multiplayer game.
*/
bool bIsMultiplayer();

#ifdef CLIENT_DLL
void LoadVModel( const char* const pszViewModel, CBasePlayer* pPlayer );
#endif

void RegisterAmmoTypes();

/**
*	Precaches weapons for this map.
*	Used on the server side to precache all weapons and load their data.
*	Used on the client side to prepare all weapons for prediction.
*/
void PrecacheWeapons();

/**
*	Used by melee weapons to calculate the point of intersection of the line (or hull) and the object we hit.
*	This is an approximation of the "best" intersection.
*	@param vecSrc Origin to perform the calculation from.
*	@param tr Result of line trace.
*	@param vecMins Minimums of the hull.
*	@param vecMaxs Maximums of the hull.
*	@param pEntity Entity to ignore during the line trace (usually the attacker).
*/
void FindHullIntersection( const Vector &vecSrc, TraceResult &tr, const Vector& vecMins, const Vector& vecMaxs, CBaseEntity* pEntity );

#endif //GAME_SHARED_WEAPONS_H
