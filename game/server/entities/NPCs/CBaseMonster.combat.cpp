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
/*

===== combat.cpp ========================================================

  functions dealing with damage infliction & death

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/NPCs/Monsters.h"
#include "entities/CSoundEnt.h"
#include "Decals.h"
#include "animation.h"
#include "Weapons.h"
#include "entities/effects/CGib.h"

bool CBaseMonster::HasHumanGibs()
{
	EntityClassification_t myClass = Classify();

	if ( myClass == EntityClassifications().GetClassificationId( classify::HUMAN_MILITARY ) ||
		 myClass == EntityClassifications().GetClassificationId( classify::PLAYER_ALLY )	||
		 myClass == EntityClassifications().GetClassificationId( classify::HUMAN_PASSIVE )  ||
		 myClass == EntityClassifications().GetClassificationId( classify::PLAYER ) )

		 return true;

	return false;
}


bool CBaseMonster::HasAlienGibs()
{
	EntityClassification_t myClass = Classify();

	if ( myClass == EntityClassifications().GetClassificationId( classify::ALIEN_MILITARY ) ||
		 myClass == EntityClassifications().GetClassificationId( classify::ALIEN_MONSTER )	||
		 myClass == EntityClassifications().GetClassificationId( classify::ALIEN_PASSIVE )  ||
		 myClass == EntityClassifications().GetClassificationId( classify::INSECT )  ||
		 myClass == EntityClassifications().GetClassificationId( classify::ALIEN_PREDATOR )  ||
		 myClass == EntityClassifications().GetClassificationId( classify::ALIEN_PREY ) )

		 return true;

	return false;
}


void CBaseMonster::FadeMonster( void )
{
	StopAnimation();
	pev->velocity = g_vecZero;
	pev->movetype = MOVETYPE_NONE;
	pev->avelocity = g_vecZero;
	pev->animtime = gpGlobals->time;
	pev->effects |= EF_NOINTERP;
	SUB_StartFadeOut();
}

//=========================================================
// GibMonster - create some gore and get rid of a monster's
// model.
//=========================================================
void CBaseMonster :: GibMonster( void )
{
	TraceResult	tr;
	bool		gibbed = false;

	EMIT_SOUND( this, CHAN_WEAPON, "common/bodysplat.wav", 1, ATTN_NORM);

	// only humans throw skulls !!!UNDONE - eventually monsters will have their own sets of gibs
	if ( HasHumanGibs() )
	{
		if ( CVAR_GET_FLOAT("violence_hgibs") != 0 )	// Only the player will ever get here
		{
			CGib::SpawnHeadGib( this );
			// ############ hu3lifezado ############ //
			// Mais tripas (this,4,1)
			CGib::SpawnRandomGibs( this, 5, 1 );	// throw some human gibs.
			// ############ //
		}
		gibbed = true;
	}
	else if ( HasAlienGibs() )
	{
		if ( CVAR_GET_FLOAT("violence_agibs") != 0 )	// Should never get here, but someone might call it directly
		{
			// ############ hu3lifezado ############ //
			// Mais tripas (this,4,1)
			CGib::SpawnRandomGibs( this, 5, 1 );	// throw alien gibs.
			// ############ //
		}
		gibbed = true;
	}

	if ( !IsPlayer() )
	{
		if ( gibbed )
		{
			// don't remove players!
			SetThink ( &CBaseMonster::SUB_Remove );
			pev->nextthink = gpGlobals->time;
		}
		else
		{
			FadeMonster();
		}
	}
}

//=========================================================
// GetDeathActivity - determines the best type of death
// anim to play.
//=========================================================
Activity CBaseMonster :: GetDeathActivity ( void )
{
	Activity	deathActivity;
	bool		fTriedDirection;
	float		flDot;
	TraceResult	tr;
	Vector		vecSrc;

	if ( pev->deadflag != DEAD_NO )
	{
		// don't run this while dying.
		return m_IdealActivity;
	}

	vecSrc = Center();

	fTriedDirection = false;
	deathActivity = ACT_DIESIMPLE;// in case we can't find any special deaths to do.

	UTIL_MakeVectors ( pev->angles );
	flDot = DotProduct ( gpGlobals->v_forward, g_vecAttackDir * -1 );

	switch ( m_LastHitGroup )
	{
		// try to pick a region-specific death.
	case HITGROUP_HEAD:
		deathActivity = ACT_DIE_HEADSHOT;
		break;

	case HITGROUP_STOMACH:
		deathActivity = ACT_DIE_GUTSHOT;
		break;

	case HITGROUP_GENERIC:
		// try to pick a death based on attack direction
		fTriedDirection = true;

		if ( flDot > 0.3 )
		{
			deathActivity = ACT_DIEFORWARD;
		}
		else if ( flDot <= -0.3 )
		{
			deathActivity = ACT_DIEBACKWARD;
		}
		break;

	default:
		// try to pick a death based on attack direction
		fTriedDirection = true;

		if ( flDot > 0.3 )
		{
			deathActivity = ACT_DIEFORWARD;
		}
		else if ( flDot <= -0.3 )
		{
			deathActivity = ACT_DIEBACKWARD;
		}
		break;
	}


	// can we perform the prescribed death?
	if ( LookupActivity ( deathActivity ) == ACTIVITY_NOT_AVAILABLE )
	{
		// no! did we fail to perform a directional death? 
		if ( fTriedDirection )
		{
			// if yes, we're out of options. Go simple.
			deathActivity = ACT_DIESIMPLE;
		}
		else
		{
			// cannot perform the ideal region-specific death, so try a direction.
			if ( flDot > 0.3 )
			{
				deathActivity = ACT_DIEFORWARD;
			}
			else if ( flDot <= -0.3 )
			{
				deathActivity = ACT_DIEBACKWARD;
			}
		}
	}

	if ( LookupActivity ( deathActivity ) == ACTIVITY_NOT_AVAILABLE )
	{
		// if we're still invalid, simple is our only option.
		deathActivity = ACT_DIESIMPLE;
	}

	if ( deathActivity == ACT_DIEFORWARD )
	{
			// make sure there's room to fall forward
			UTIL_TraceHull ( vecSrc, vecSrc + gpGlobals->v_forward * 64, dont_ignore_monsters, Hull::HEAD, edict(), &tr );

			if ( tr.flFraction != 1.0 )
			{
				deathActivity = ACT_DIESIMPLE;
			}
	}

	if ( deathActivity == ACT_DIEBACKWARD )
	{
			// make sure there's room to fall backward
			UTIL_TraceHull ( vecSrc, vecSrc - gpGlobals->v_forward * 64, dont_ignore_monsters, Hull::HEAD, edict(), &tr );

			if ( tr.flFraction != 1.0 )
			{
				deathActivity = ACT_DIESIMPLE;
			}
	}

	return deathActivity;
}

//=========================================================
// GetSmallFlinchActivity - determines the best type of flinch
// anim to play.
//=========================================================
Activity CBaseMonster :: GetSmallFlinchActivity ( void )
{
	Activity	flinchActivity;
	bool		fTriedDirection;
	float		flDot;

	fTriedDirection = false;
	UTIL_MakeVectors ( pev->angles );
	flDot = DotProduct ( gpGlobals->v_forward, g_vecAttackDir * -1 );
	
	switch ( m_LastHitGroup )
	{
		// pick a region-specific flinch
	case HITGROUP_HEAD:
		flinchActivity = ACT_FLINCH_HEAD;
		break;
	case HITGROUP_STOMACH:
		flinchActivity = ACT_FLINCH_STOMACH;
		break;
	case HITGROUP_LEFTARM:
		flinchActivity = ACT_FLINCH_LEFTARM;
		break;
	case HITGROUP_RIGHTARM:
		flinchActivity = ACT_FLINCH_RIGHTARM;
		break;
	case HITGROUP_LEFTLEG:
		flinchActivity = ACT_FLINCH_LEFTLEG;
		break;
	case HITGROUP_RIGHTLEG:
		flinchActivity = ACT_FLINCH_RIGHTLEG;
		break;
	case HITGROUP_GENERIC:
	default:
		// just get a generic flinch.
		flinchActivity = ACT_SMALL_FLINCH;
		break;
	}


	// do we have a sequence for the ideal activity?
	if ( LookupActivity ( flinchActivity ) == ACTIVITY_NOT_AVAILABLE )
	{
		flinchActivity = ACT_SMALL_FLINCH;
	}

	return flinchActivity;
}


void CBaseMonster::BecomeDead( void )
{
	pev->takedamage = DAMAGE_YES;// don't let autoaim aim at corpses.
	
	// give the corpse half of the monster's original maximum health. 
	pev->health = pev->max_health / 2;
	// ############ hu3lifezado ############ //
	// Mais sangue por corpo (5)
	pev->max_health = pev->health; // max_health now becomes a counter for how many blood decals the corpse can place.
	// ############ //
	// make the corpse fly away from the attack vector
	pev->movetype = MOVETYPE_TOSS;
	//pev->flags &= ~FL_ONGROUND;
	//GetAbsOrigin().z += 2;
	//pev->velocity = g_vecAttackDir * -1;
	//pev->velocity = pev->velocity * RANDOM_FLOAT( 300, 400 );
}


bool CBaseMonster::ShouldGibMonster( GibAction gibAction ) const
{
	if ( ( gibAction == GIB_NORMAL && pev->health < GIB_HEALTH_VALUE ) || ( gibAction == GIB_ALWAYS ) )
		return true;
	
	return false;
}


void CBaseMonster::CallGibMonster( void )
{
	bool fade = false;

	if ( HasHumanGibs() )
	{
		if ( CVAR_GET_FLOAT("violence_hgibs") == 0 )
			fade = true;
	}
	else if ( HasAlienGibs() )
	{
		if ( CVAR_GET_FLOAT("violence_agibs") == 0 )
			fade = true;
	}

	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_NOT;// do something with the body. while monster blows up

	if ( fade )
	{
		FadeMonster();
	}
	else
	{
		pev->effects = EF_NODRAW; // make the model invisible.
		GibMonster();
	}

	pev->deadflag = DEAD_DEAD;
	FCheckAITrigger();

	// don't let the status bar glitch for players.with <0 health.
	if (pev->health < -99)
	{
		pev->health = 0;
	}
	
	if ( ShouldFadeOnDeath() && !fade )
		UTIL_Remove(this);
}


/*
============
Killed
============
*/
void CBaseMonster::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	unsigned int	cCount = 0;
	bool			fDone = false;

	if ( HasMemory( bits_MEMORY_KILLED ) )
	{
		if ( ShouldGibMonster( gibAction ) )
			CallGibMonster();
		return;
	}

	Remember( bits_MEMORY_KILLED );

	// clear the deceased's sound channels.(may have been firing or reloading when killed)
	EMIT_SOUND( this, CHAN_WEAPON, "common/null.wav", 1, ATTN_NORM);
	m_IdealMonsterState = MONSTERSTATE_DEAD;
	// Make sure this condition is fired too (OnTakeDamage breaks out before this happens on death)
	SetConditions( bits_COND_LIGHT_DAMAGE );
	
	// tell owner ( if any ) that we're dead.This is mostly for MonsterMaker functionality.
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	if ( pOwner )
	{
		pOwner->DeathNotice( this );
	}

	if	( ShouldGibMonster( gibAction ) )
	{
		CallGibMonster();
		return;
	}
	else if ( pev->flags & FL_MONSTER )
	{
		SetTouch( NULL );
		BecomeDead();
	}

	// ############ hu3lifezado ############ //
	// Os npcs ao serem mortos sao arremessados
	pev->flags &= ~FL_ONGROUND;
	pev->origin.z += RANDOM_LONG(0, 45);
	pev->velocity = g_vecAttackDir * -1;
	pev->velocity = pev->velocity * RANDOM_FLOAT(0, 800);
	// ############ //

	// don't let the status bar glitch for players.with <0 health.
	if (pev->health < -99)
	{
		pev->health = 0;
	}
	
	//pev->enemy = ENT( info.GetAttacker() );//why? (sjb)
	
	m_IdealMonsterState = MONSTERSTATE_DEAD;
}

/*
============
OnTakeDamage

The damage is coming from inflictor, but get mad at attacker
This should be the only function that ever reduces health.
bitsDamageType indicates the type of damage sustained, ie: DMG_SHOCK

Time-based damage: only occurs while the monster is within the trigger_hurt.
When a monster is poisoned via an arrow etc it takes all the poison damage at once.



GLOBALS ASSUMED SET:  gSkillData.GetSkillLevel()
============
*/
void CBaseMonster::OnTakeDamage( const CTakeDamageInfo& info )
{
	float	flTake;
	Vector	vecDir;

	if (!pev->takedamage)
		return;

	if ( !IsAlive() )
	{
		DeadTakeDamage( info );
		return;
	}

	if ( pev->deadflag == DEAD_NO )
	{
		// no pain sound during death animation.
		PainSound();// "Ouch!"
	}

	//!!!LATER - make armor consideration here!
	flTake = info.GetDamage();

	// set damage type sustained
	m_bitsDamageType |= info.GetDamageTypes();

	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	vecDir = Vector( 0, 0, 0 );
	if( !FNullEnt( info.GetInflictor() ) )
	{
		vecDir = ( info.GetInflictor()->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize();
		vecDir = g_vecAttackDir = vecDir.Normalize();
	}

	// add to the damage total for clients, which will be sent as a single
	// message at the end of the frame
	// todo: remove after combining shotgun blasts?
	if ( IsPlayer() )
	{
		if ( info.GetInflictor() )
			pev->dmg_inflictor = info.GetInflictor()->edict();

		pev->dmg_take += flTake;

		// check for godmode or invincibility
		if ( pev->flags & FL_GODMODE )
		{
			return;
		}
	}

	// if this is a player, move him around!
	if ( ( !FNullEnt( info.GetInflictor() ) ) && (pev->movetype == MOVETYPE_WALK) && (!info.GetAttacker() || info.GetAttacker()->pev->solid != SOLID_TRIGGER) )
	{
		pev->velocity = pev->velocity + vecDir * -DamageForce( info.GetDamage() );
	}

	// do the damage
	pev->health -= flTake;

	
	// HACKHACK Don't kill monsters in a script.  Let them break their scripts first
	if ( m_MonsterState == MONSTERSTATE_SCRIPT )
	{
		SetConditions( bits_COND_LIGHT_DAMAGE );
		return;
	}

	if ( pev->health <= 0 )
	{
		if ( info.GetDamageTypes() & DMG_ALWAYSGIB )
		{
			Killed( info, GIB_ALWAYS );
		}
		else if ( info.GetDamageTypes() & DMG_NEVERGIB )
		{
			Killed( info, GIB_NEVER );
		}
		else
		{
			Killed( info, GIB_NORMAL );
		}

		return;
	}

	// react to the damage (get mad)
	if ( (pev->flags & FL_MONSTER) && !FNullEnt( info.GetAttacker() ) )
	{
		if ( info.GetAttacker()->pev->flags & (FL_MONSTER | FL_CLIENT) )
		{// only if the attack was a monster or client!
			
			// enemy's last known position is somewhere down the vector that the attack came from.
			if ( info.GetInflictor() )
			{
				if (m_hEnemy == NULL || info.GetInflictor()->pev == m_hEnemy->pev || !HasConditions(bits_COND_SEE_ENEMY))
				{
					m_vecEnemyLKP = info.GetInflictor()->GetAbsOrigin();
				}
			}
			else
			{
				m_vecEnemyLKP = GetAbsOrigin() + ( g_vecAttackDir * 64 ); 
			}

			MakeIdealYaw( m_vecEnemyLKP );

			// add pain to the conditions 
			// !!!HACKHACK - fudged for now. Do we want to have a virtual function to determine what is light and 
			// heavy damage per monster class?
			if ( info.GetDamage() > 0 )
			{
				SetConditions(bits_COND_LIGHT_DAMAGE);
			}

			if ( info.GetDamage() >= 20 )
			{
				SetConditions(bits_COND_HEAVY_DAMAGE);
			}
		}
	}
}

//=========================================================
// DeadTakeDamage - takedamage function called when a monster's
// corpse is damaged.
//=========================================================
void CBaseMonster::DeadTakeDamage( const CTakeDamageInfo& info )
{
	Vector vecDir;

	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	vecDir = Vector( 0, 0, 0 );
	if( !FNullEnt( info.GetInflictor() ) )
	{
		vecDir = ( info.GetInflictor()->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize();
		vecDir = g_vecAttackDir = vecDir.Normalize();
	}

#if 0// turn this back on when the bounding box issues are resolved.

	pev->flags &= ~FL_ONGROUND;
	GetAbsOrigin().z += 1;
	
	// let the damage scoot the corpse around a bit.
	if ( !FNullEnt( pAttacker ) && ( pAttacker->pev->solid != SOLID_TRIGGER) )
	{
		pev->velocity = pev->velocity + vecDir * -DamageForce( flDamage );
	}

#endif

	// kill the corpse if enough damage was done to destroy the corpse and the damage is of a type that is allowed to destroy the corpse.
	if ( info.GetDamageTypes() & DMG_GIB_CORPSE && !( info.GetDamageTypes() & DMG_NEVERGIB ) )
	{
		if ( pev->health <= info.GetDamage() )
		{
			pev->health = -50;
			Killed( info, GIB_ALWAYS );
			return;
		}
		// Accumulate corpse gibbing damage, so you can gib with multiple hits
		pev->health -= info.GetDamage() * 0.1;
	}
}


float CBaseMonster :: DamageForce( float damage )
{ 
	float force = damage * ((32 * 32 * 72.0) / (pev->size.x * pev->size.y * pev->size.z)) * 5;
	
	if ( force > 1000.0) 
	{
		force = 1000.0;
	}

	return force;
}

//
// RadiusDamage - this entity is exploding, or otherwise needs to inflict damage upon entities within a certain range.
// 
// only damage ents that can clearly be seen by the explosion!

	
void RadiusDamage( Vector vecSrc, const CTakeDamageInfo& info, float flRadius, EntityClassification_t iClassIgnore )
{
	CBaseEntity *pEntity = NULL;
	TraceResult	tr;
	float		flAdjustedDamage, falloff;
	Vector		vecSpot;

	CTakeDamageInfo newInfo = info;

	if ( flRadius )
		falloff = newInfo.GetDamage() / flRadius;
	else
		falloff = 1.0;

	int bInWater = (UTIL_PointContents ( vecSrc ) == CONTENTS_WATER);

	vecSrc.z += 1;// in case grenade is lying on the ground

	if ( !newInfo.GetAttacker() )
		newInfo.SetAttacker( newInfo.GetInflictor() );

	// iterate on all entities in the vicinity.
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, vecSrc, flRadius )) != NULL)
	{
		if ( pEntity->pev->takedamage != DAMAGE_NO )
		{
			// UNDONE: this should check a damage mask, not an ignore
			if ( iClassIgnore != EntityClassifications().GetNoneId() && pEntity->Classify() == iClassIgnore )
			{// houndeyes don't hurt other houndeyes with their attack
				continue;
			}

			// blast's don't tavel into or out of water
			if( bInWater && pEntity->GetWaterLevel() == WATERLEVEL_DRY )
				continue;
			if( !bInWater && pEntity->GetWaterLevel() == WATERLEVEL_HEAD )
				continue;

			vecSpot = pEntity->BodyTarget( vecSrc );
			
			UTIL_TraceLine ( vecSrc, vecSpot, dont_ignore_monsters, newInfo.GetInflictor()->edict(), &tr );

			if ( tr.flFraction == 1.0 || tr.pHit == pEntity->edict() )
			{// the explosion can 'see' this entity, so hurt them!
				if (tr.fStartSolid)
				{
					// if we're stuck inside them, fixup the position and distance
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0.0;
				}
				
				// decrease damage for an ent that's farther from the bomb.
				flAdjustedDamage = ( vecSrc - tr.vecEndPos ).Length() * falloff;
				flAdjustedDamage = newInfo.GetDamage() - flAdjustedDamage;
			
				if ( flAdjustedDamage < 0 )
				{
					flAdjustedDamage = 0;
				}
			
				// ALERT( at_console, "hit %s\n", pEntity->GetClassname() );
				if (tr.flFraction != 1.0)
				{
					g_MultiDamage.Clear( );
					pEntity->TraceAttack( CTakeDamageInfo( newInfo.GetInflictor(), newInfo.GetAttacker(), flAdjustedDamage, newInfo.GetDamageTypes() ), 
						(tr.vecEndPos - vecSrc).Normalize( ), &tr );
					g_MultiDamage.ApplyMultiDamage( newInfo.GetInflictor(), newInfo.GetAttacker() );
				}
				else
				{
					pEntity->TakeDamage( CTakeDamageInfo( newInfo.GetInflictor(), newInfo.GetAttacker(), flAdjustedDamage, newInfo.GetDamageTypes() ) );
				}
			}
		}
	}
}


void CBaseMonster::RadiusDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, EntityClassification_t iClassIgnore, int bitsDamageType )
{
	::RadiusDamage( GetAbsOrigin(), CTakeDamageInfo( pInflictor, pAttacker, flDamage, bitsDamageType ), flDamage * 2.5, iClassIgnore );
}


void CBaseMonster :: RadiusDamage( Vector vecSrc, CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, EntityClassification_t iClassIgnore, int bitsDamageType )
{
	::RadiusDamage( vecSrc, CTakeDamageInfo( pInflictor, pAttacker, flDamage, bitsDamageType ), flDamage * 2.5, iClassIgnore );
}


//=========================================================
// CheckTraceHullAttack - expects a length to trace, amount 
// of damage to do, and damage type. Returns a pointer to
// the damaged entity in case the monster wishes to do
// other stuff to the victim (punchangle, etc)
//
// Used for many contact-range melee attacks. Bites, claws, etc.
//=========================================================
CBaseEntity* CBaseMonster :: CheckTraceHullAttack( float flDist, int iDamage, int iDmgType )
{
	TraceResult tr;

	if (IsPlayer())
		UTIL_MakeVectors( pev->angles );
	else
		UTIL_MakeAimVectors( pev->angles );

	Vector vecStart = GetAbsOrigin();
	vecStart.z += pev->size.z * 0.5;
	Vector vecEnd = vecStart + (gpGlobals->v_forward * flDist );

	UTIL_TraceHull( vecStart, vecEnd, dont_ignore_monsters, Hull::HEAD, ENT(pev), &tr );
	
	if ( tr.pHit )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );

		if ( iDamage > 0 )
		{
			pEntity->TakeDamage( this, this, iDamage, iDmgType );
		}

		return pEntity;
	}

	return NULL;
}


//=========================================================
// FInViewCone - returns true is the passed ent is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall. 
//=========================================================
bool CBaseMonster::FInViewCone( const CBaseEntity *pEntity ) const
{
	Vector2D	vec2LOS;
	float	flDot;

	UTIL_MakeVectors ( pev->angles );
	
	vec2LOS = ( pEntity->GetAbsOrigin() - GetAbsOrigin() ).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );

	if ( flDot > m_flFieldOfView )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//=========================================================
// FInViewCone - returns true is the passed vector is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall. 
//=========================================================
bool CBaseMonster::FInViewCone( const Vector& vecOrigin ) const
{
	Vector2D	vec2LOS;
	float		flDot;

	UTIL_MakeVectors ( pev->angles );
	
	vec2LOS = ( vecOrigin - GetAbsOrigin() ).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );

	if ( flDot > m_flFieldOfView )
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
//=========================================================
// TraceAttack
//=========================================================
void CBaseMonster::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr )
{
	Vector vecOrigin = ptr->vecEndPos - vecDir * 4;

	ALERT ( at_console, "%d\n", ptr->iHitgroup );


	if ( pev->takedamage )
	{
		g_MultiDamage.AddMultiDamage( info.GetAttacker(), this, info.GetDamage(), info.GetDamageTypes() );

		int blood = BloodColor();
		
		if ( blood != DONT_BLEED )
		{
			SpawnBlood( info, vecOrigin, blood );// a little surface blood.
		}
	}
}
*/

//=========================================================
// TraceAttack
//=========================================================
void CBaseMonster::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr )
{
	CTakeDamageInfo newInfo = info;

	if ( pev->takedamage )
	{
		m_LastHitGroup = ptr->iHitgroup;

		switch ( ptr->iHitgroup )
		{
		case HITGROUP_GENERIC:
			break;
		case HITGROUP_HEAD:
			newInfo.GetMutableDamage() *= gSkillData.GetMonHead();
			break;
		case HITGROUP_CHEST:
			newInfo.GetMutableDamage() *= gSkillData.GetMonChest();
			break;
		case HITGROUP_STOMACH:
			newInfo.GetMutableDamage() *= gSkillData.GetMonStomach();
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			newInfo.GetMutableDamage() *= gSkillData.GetMonArm();
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			newInfo.GetMutableDamage() *= gSkillData.GetMonLeg();
			break;
		default:
			break;
		}

		SpawnBlood(ptr->vecEndPos, BloodColor(), newInfo.GetDamage());// a little surface blood.
		TraceBleed( newInfo, vecDir, ptr );
		g_MultiDamage.AddMultiDamage( newInfo, this );
	}
}

//=========================================================
//=========================================================
void CBaseMonster :: MakeDamageBloodDecal ( int cCount, float flNoise, TraceResult *ptr, const Vector &vecDir )
{
	// make blood decal on the wall! 
	TraceResult Bloodtr;
	Vector vecTraceDir; 
	int i;

	if ( !IsAlive() )
	{
		// dealing with a dead monster. 
		if ( pev->max_health <= 0 )
		{
			// no blood decal for a monster that has already decalled its limit.
			return; 
		}
		else
		{
			pev->max_health--;
		}
	}

	// ############ hu3lifezado ############ //
	// Alien solta mais sangue (nao tem padrao, eu adicionei, apenas delete)
	cCount += 5;
	// ############ //

	for ( i = 0 ; i < cCount ; i++ )
	{
		vecTraceDir = vecDir;

		vecTraceDir.x += RANDOM_FLOAT( -flNoise, flNoise );
		vecTraceDir.y += RANDOM_FLOAT( -flNoise, flNoise );
		vecTraceDir.z += RANDOM_FLOAT( -flNoise, flNoise );

		UTIL_TraceLine( ptr->vecEndPos, ptr->vecEndPos + vecTraceDir * 172, ignore_monsters, ENT(pev), &Bloodtr);

/*
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SHOWLINE);
			WRITE_COORD( ptr->vecEndPos.x );
			WRITE_COORD( ptr->vecEndPos.y );
			WRITE_COORD( ptr->vecEndPos.z );
			
			WRITE_COORD( Bloodtr.vecEndPos.x );
			WRITE_COORD( Bloodtr.vecEndPos.y );
			WRITE_COORD( Bloodtr.vecEndPos.z );
		MESSAGE_END();
*/

		if ( Bloodtr.flFraction != 1.0 )
		{
			UTIL_BloodDecalTrace( &Bloodtr, BloodColor() );
		}
	}
}
