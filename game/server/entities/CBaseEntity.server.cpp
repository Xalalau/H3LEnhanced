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
#include "ServerInterface.h"

#include "Decals.h"

#include "cbase.h"

extern DLL_GLOBAL Vector		g_vecAttackDir;

void SetObjectCollisionBox( entvars_t *pev );

// give health
float CBaseEntity::GiveHealth( float flHealth, int bitsDamageType )
{
	if( !pev->takedamage )
		return 0;

	const float flOldHealth = pev->health;

	// ############ hu3lifezado ############ //
	// Mudei o limite de sangue de pev->max_health

// heal
	pev->health += flHealth;

	// Nao pode sangue acima do maximo
	if (pev->health > pev->max_health)
		pev->health = pev->max_health;

	// Nao pode sangue baixo de 0
	if (pev->health < 0)
		pev->health = 1;

	// Calculamos o sangue atual
	float currentHeath = pev->health - flOldHealth;

	// Corrigimos o valor caso o SUS esteja nos matando
	if (currentHeath < 0 )
		currentHeath = currentHeath * -1;

	return currentHeath;

	// Original:
	/*
// heal
	if( pev->health >= pev->max_health )
	return 0;

	const float flOldHealth = pev->health;

	pev->health += flHealth;

	//TODO: if the entity's health drops below 1, kill it. - Solokiller

	if( pev->health > pev->max_health )
		pev->health = pev->max_health;

	return pev->health - flOldHealth;
	*/
	// ############ //
}

// inflict damage on this entity.  bitsDamageType indicates type of damage inflicted, ie: DMG_CRUSH

void CBaseEntity::OnTakeDamage( const CTakeDamageInfo& info )
{
	if( !pev->takedamage )
		return;

	auto pInflictor = info.GetInflictor();

	// UNDONE: some entity types may be immune or resistant to some bitsDamageType

	// if Attacker == Inflictor, the attack was a melee or other instant-hit attack.
	// (that is, no actual entity projectile was involved in the attack so use the shooter's origin). 
	// Otherwise, an actual missile was involved.
	Vector vecTemp = pInflictor->GetAbsOrigin() - ( VecBModelOrigin( this ) );

	// this global is still used for glass and other non-monster killables, along with decals.
	g_vecAttackDir = vecTemp.Normalize();

	// save damage based on the target's armor level

	// figure momentum add (don't let hurt brushes or other triggers move player)
	if( ( !FNullEnt( pInflictor ) ) && ( pev->movetype == MOVETYPE_WALK || pev->movetype == MOVETYPE_STEP ) && ( info.GetAttacker()->pev->solid != SOLID_TRIGGER ) )
	{
		Vector vecDir = GetAbsOrigin() - ( pInflictor->pev->absmin + pInflictor->pev->absmax ) * 0.5;
		vecDir = vecDir.Normalize();

		float flForce = info.GetDamage() * ( ( 32 * 32 * 72.0 ) / ( pev->size.x * pev->size.y * pev->size.z ) ) * 5;

		if( flForce > 1000.0 )
			flForce = 1000.0;
		pev->velocity = pev->velocity + vecDir * flForce;
	}

	// do the damage
	pev->health -= info.GetDamage();
	if( pev->health <= 0 )
	{
		Killed( info, GIB_NORMAL );
		return;
	}

	return;
}

void CBaseEntity::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	pev->takedamage = DAMAGE_NO;
	pev->deadflag = DEAD_DEAD;
	UTIL_Remove( this );
}


CBaseEntity *CBaseEntity::GetNextTarget()
{
	if( !HasTarget() )
		return nullptr;

	return UTIL_FindEntityByTargetname( nullptr, GetTarget() );
}

bool CBaseEntity::Save( CSave& save )
{
	if( save.WriteEntVars( "ENTVARS", pev ) )
	{
		const DataMap_t* pInstanceDataMap = GetDataMap();
		const DataMap_t* pDataMap = pInstanceDataMap;

		bool bResult = true;

		while( pDataMap )
		{
			bResult = save.WriteFields( pDataMap->pszClassName, this, *pInstanceDataMap, pDataMap->pTypeDesc, pDataMap->uiNumDescriptors );

			if( !bResult )
				return false;

			pDataMap = pDataMap->pParent;
		}

		return bResult;
	}

	return false;
}

bool CBaseEntity::Restore( CRestore &restore )
{
	bool bResult = restore.ReadEntVars( "ENTVARS", pev );

	if( bResult )
	{
		const DataMap_t* pInstanceDataMap = GetDataMap();
		const DataMap_t* pDataMap = pInstanceDataMap;

		while( pDataMap )
		{
			bResult = restore.ReadFields( pDataMap->pszClassName, this, *pInstanceDataMap, pDataMap->pTypeDesc, pDataMap->uiNumDescriptors );

			if( !bResult )
				break;

			pDataMap = pDataMap->pParent;
		}
	}

	if( pev->modelindex != 0 && !FStringNull( pev->model ) )
	{
		Vector mins, maxs;
		mins = pev->mins;	// Set model is about to destroy these
		maxs = pev->maxs;


		PRECACHE_MODEL( ( char * ) STRING( pev->model ) );
		SetModel( STRING( pev->model ) );
		SetSize( mins, maxs );	// Reset them
	}

	return bResult;
}

void CBaseEntity::SetObjectCollisionBox( void )
{
	::SetObjectCollisionBox( pev );
}

bool CBaseEntity::Intersects( const CBaseEntity* const pOther ) const
{
	if( pOther->pev->absmin.x > pev->absmax.x ||
		pOther->pev->absmin.y > pev->absmax.y ||
		pOther->pev->absmin.z > pev->absmax.z ||
		pOther->pev->absmax.x < pev->absmin.x ||
		pOther->pev->absmax.y < pev->absmin.y ||
		pOther->pev->absmax.z < pev->absmin.z )
		return false;
	return true;
}

void CBaseEntity::MakeDormant( void )
{
	SetBits( pev->flags, FL_DORMANT );

	// Don't touch
	pev->solid = SOLID_NOT;
	// Don't move
	pev->movetype = MOVETYPE_NONE;
	// Don't draw
	SetBits( pev->effects, EF_NODRAW );
	// Don't think
	pev->nextthink = 0;
	// Relink
	SetAbsOrigin( GetAbsOrigin() );
}

bool CBaseEntity::IsDormant() const
{
	return FBitSet( pev->flags, FL_DORMANT ) != 0;
}

bool CBaseEntity::IsInWorld() const
{
	// position 
	if( GetAbsOrigin().x >= WORLD_BOUNDARY ) return false;
	if( GetAbsOrigin().y >= WORLD_BOUNDARY ) return false;
	if( GetAbsOrigin().z >= WORLD_BOUNDARY ) return false;
	if( GetAbsOrigin().x <= -WORLD_BOUNDARY ) return false;
	if( GetAbsOrigin().y <= -WORLD_BOUNDARY ) return false;
	if( GetAbsOrigin().z <= -WORLD_BOUNDARY ) return false;
	// speed
	if( pev->velocity.x >= MAX_VELOCITY ) return false;
	if( pev->velocity.y >= MAX_VELOCITY ) return false;
	if( pev->velocity.z >= MAX_VELOCITY ) return false;
	if( pev->velocity.x <= -MAX_VELOCITY ) return false;
	if( pev->velocity.y <= -MAX_VELOCITY ) return false;
	if( pev->velocity.z <= -MAX_VELOCITY ) return false;

	return true;
}

bool CBaseEntity::ShouldToggle( USE_TYPE useType, const bool currentState ) const
{
	if( useType != USE_TOGGLE && useType != USE_SET )
	{
		if( ( currentState && useType == USE_ON ) || ( !currentState && useType == USE_OFF ) )
			return false;
	}
	return true;
}


int	CBaseEntity::DamageDecal( int bitsDamageType ) const
{
	if( pev->rendermode == kRenderTransAlpha )
		return -1;

	if( pev->rendermode != kRenderNormal )
		return DECAL_BPROOF1;

	return DECAL_GUNSHOT1 + RANDOM_LONG( 0, 4 );
}



// NOTE: szName must be a pointer to constant memory, e.g. "monster_class" because the entity
// will keep a pointer to it after this call.
CBaseEntity* CBaseEntity::Create( const char* const pszName, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner, const bool bSpawnEntity )
{
	edict_t* pent;
	CBaseEntity* pEntity;

	//TODO: alloc for custom ents - Solokiller
	pent = CREATE_NAMED_ENTITY( MAKE_STRING( pszName ) );
	if( FNullEnt( pent ) )
	{
		ALERT( at_console, "NULL Ent in Create!\n" );
		return nullptr;
	}
	pEntity = Instance( pent );
	pEntity->pev->owner = pentOwner;
	pEntity->pev->origin = vecOrigin;
	pEntity->pev->angles = vecAngles;

	if( bSpawnEntity )
	{
		//This didn't use to handle self removing entities. - Solokiller
		if( -1 == DispatchSpawn( pEntity->edict() ) )
			return nullptr;
	}

	return pEntity;
}


