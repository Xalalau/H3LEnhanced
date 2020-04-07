#if USE_OPFOR
// ---------------------------------------------------------------
// Based on Flying Crowbar Entity. Ver 1.0 as seen in Lambda BubbleMod
// 
// AUTHOR
//        Tyler Lund <halflife@briareos.net>
//
// LICENSE                                                            
//                                                                    
//        Permission is granted to anyone to use this  software  for  
//        any purpose on any computer system, and to redistribute it  
//        in any way, subject to the following restrictions:          
//                                                                    
//        1. The author is not responsible for the  consequences  of  
//           use of this software, no matter how awful, even if they  
//           arise from defects in it.                                
//        2. The origin of this software must not be misrepresented,  
//           either by explicit claim or by omission.                 
//        3. Altered  versions  must  be plainly marked as such, and  
//           must  not  be  misrepresented  (by  explicit  claim  or  
//           omission) as being the original software.                
//        3a. It would be nice if I got  a  copy  of  your  improved  
//            version  sent to tlund@briareos.net. 
//        4. This notice must not be removed or altered.              
//                                                                    
// ---------------------------------------------------------------

// ############ hu3lifezado ############ //
// Tiro secundario da arma Touros, adaptado de:
// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
// ############ //

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"
#include "gamerules/GameRules.h"
#include "entities/weapons/CWeaponBox.h"

#include "CFlyingTouros.h"

BEGIN_DATADESC(CFlyingTouros)
DEFINE_TOUCHFUNC(SpinTouch),
DEFINE_THINKFUNC(BubbleThink),
END_DATADESC()

LINK_ENTITY_TO_CLASS(flying_touros, CFlyingTouros);

void CFlyingTouros::Spawn()
{
	Precache();

	// The flying Touros is MOVETYPE_TOSS, and SOLID_BBOX.
	// We want it to be affected by gravity, and hit objects
	// within the game.
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;

	// Use the world Touros model.
	SetModel("models/w_desert_eagle.mdl");

	// Set the origin and size for the HL engine collision
	// tables.
	UTIL_SetOrigin(this, pev->origin);
	UTIL_SetSize(this, Vector(-4, -4, -4), Vector(4, 4, 4));

	// Store the owner for later use. We want the owner to be able 
	// to hit themselves with the Touros. The pev->owner gets cleared
	// later to avoid hitting the player as they throw the Touros. 
	if (pev->owner)
		m_hOwner = Instance(pev->owner);

	// Set the think funtion. 
	SetThink(&CFlyingTouros::BubbleThink);
	pev->nextthink = gpGlobals->time + 0.25;

	// Set the touch function.
	SetTouch(&CFlyingTouros::SpinTouch);
}

void CFlyingTouros::Precache()
{
	PRECACHE_MODEL("models/w_desert_eagle.mdl");
	PRECACHE_SOUND("weapons/cbar_hitbod1.wav");
	PRECACHE_SOUND("weapons/cbar_hit1.wav");
	PRECACHE_SOUND("weapons/cbar_miss1.wav");
}

void CFlyingTouros::SpinTouch(CBaseEntity *pOther)
{
	SetTouch(NULL);
	SetThink(NULL);

	// We touched something in the game. Look to see if the object
	// is allowed to take damage. 
	if (pOther->pev->takedamage)
	{
		// Get the traceline info to the target.
		TraceResult tr = UTIL_GetGlobalTrace();

		// Apply damage to the target. If we have an owner stored, use that one, 
		// otherwise count it as self-inflicted.
		g_MultiDamage.Clear();
		pOther->TraceAttack(CTakeDamageInfo(m_pPlayer, 1, DMG_BLAST), GetAbsVelocity().Normalize(), tr); // 1 de dano
		if (m_hOwner != NULL)
			g_MultiDamage.ApplyMultiDamage(this, m_hOwner);
		else
			g_MultiDamage.ApplyMultiDamage(this, m_pPlayer);
	}

	// If we hit a player, make a nice squishy thunk sound. Otherwise
	// make a clang noise and throw a bunch of sparks. 
	if (pOther->IsPlayer())
		EMIT_SOUND_DYN(this, CHAN_WEAPON, "weapons/cbar_hitbod1.wav", 1.0, ATTN_NORM, 0, 100);
	else
	{
		EMIT_SOUND_DYN(this, CHAN_WEAPON, "weapons/cbar_hit1.wav", 1.0, ATTN_NORM, 0, 100);
		if (UTIL_PointContents(pev->origin) != CONTENTS_WATER)
		{
			UTIL_Sparks(pev->origin);
			UTIL_Sparks(pev->origin);
			UTIL_Sparks(pev->origin);
		}
	}

	// Get the unit vector in the direction of motion.
	Vector vecDir = pev->velocity.Normalize();

	// Reposiciona virado para cima
#ifndef CLIENT_DLL
	pev->angles = vecDir;
#endif

	// Trace a line along the velocity vector to get the normal at impact.
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + vecDir * 100, dont_ignore_monsters, ENT(pev), &tr);

	// Se o jogador nao pode pegar a arma do chao, arremesso ela
	if (mode == 1)
		pev->velocity = tr.vecPlaneNormal * 100;
	// Caso contrario
	else if (mode == 0)
	{
		// Don't draw the flying Touros anymore. 
		pev->effects |= EF_NODRAW;
		pev->solid = SOLID_NOT;

		// Spawn a Touros weapon
		CBasePlayerWeapon *pItem = (CBasePlayerWeapon *)Create("weapon_eagle", pev->origin, pev->angles, edict());

		// Ligo o item para acesso externo
		CDesertEagle *pItem_hu3 = (CDesertEagle *)pItem;

		// Salvo a qualidade incial da arma
		pItem_hu3->m_quality = quality;

		// Salvo a quantidade inicial de balas
		pItem_hu3->m_iClip2 = iClip;

		// Spawn a weapon box
		CWeaponBox *pWeaponBox = (CWeaponBox *)CBaseEntity::Create("weaponbox", GetAbsOrigin(), pev->angles, edict());

		// don't let weapon box tilt.
		pWeaponBox->pev->angles.x = 0;
		pWeaponBox->pev->angles.z = 0;

		// remove the weapon box after 2 mins. // Nope, 10 minutos
		pWeaponBox->pev->nextthink = gpGlobals->time + 1200;
		pWeaponBox->SetThink(&CWeaponBox::Kill);

		// Pack the Touros in the weapon box
		pWeaponBox->PackWeapon(pItem);

		// Throw Touros or WB along the normal so it looks kinda
		// like a ricochet. This would be better if I actually 
		// calcualted the reflection angle, but I'm lazy. :)
		pWeaponBox->pev->velocity = tr.vecPlaneNormal * 100;

		// Remove this flying_Touros from the world.
		SetThink(&CBaseEntity::SUB_Remove);
	}

	pev->nextthink = gpGlobals->time + .1;
}

void CFlyingTouros::SetQuality(int m_quality, int m_iClip)
{
	// Pego a qualidade na arma
	quality = m_quality;
	// Pego a municao carregada na arma
	iClip = m_iClip;
}

void CFlyingTouros::SetMode(int m_mode)
{
	// Pego o modo da arma;
	mode = m_mode;
}

void CFlyingTouros::BubbleThink(void)
{
	// We have no owner. We do this .25 seconds AFTER the Touros
	// is thrown so that we don't hit the owner immediately when throwing
	// it. If is comes back later, we want to be able to hit the owner.
	pev->owner = NULL;

	// Only think every .25 seconds. 
	pev->nextthink = gpGlobals->time + 0.25;

	// Make a whooshy sound. 
	EMIT_SOUND_DYN(this, CHAN_VOICE, "weapons/cbar_miss1.wav", 1, ATTN_NORM, 0, 120);

	// If the Touros enters water, make some bubbles. 
	if (pev->waterlevel)
		UTIL_BubbleTrail(pev->origin - pev->velocity * 0.1, pev->origin, 1);
}
#endif //USE_OPFOR