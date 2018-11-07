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

// ############ hu3lifezado ############ //
// Capacidade do jogo processar sangue negativo
// give health
float CBaseEntity::GiveHealth( float flHealth, int bitsDamageType )
{
	if( GetTakeDamageMode() == DAMAGE_NO )
		return 0;

	const float flOldHealth = GetHealth();

	float flNewHealth = flOldHealth + flHealth;

	if( flOldHealth >= GetMaxHealth() && flNewHealth > flOldHealth)
		return 0;

	if( flNewHealth > GetMaxHealth() )
		flNewHealth = GetMaxHealth();

	else if (flNewHealth <= 0 && flOldHealth == 1)
		return 0;
	else if (flNewHealth <= 0)
		flNewHealth = 1;

	SetHealth( flNewHealth );
	
	return GetHealth() - flOldHealth;
}
// ############ //

// inflict damage on this entity.  bitsDamageType indicates type of damage inflicted, ie: DMG_CRUSH

void CBaseEntity::OnTakeDamage( const CTakeDamageInfo& info )
{
	if( GetTakeDamageMode() == DAMAGE_NO )
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
	if( ( !FNullEnt( pInflictor ) ) && ( GetMoveType() == MOVETYPE_WALK || GetMoveType() == MOVETYPE_STEP ) && ( info.GetAttacker()->GetSolidType() != SOLID_TRIGGER ) )
	{
		Vector vecDir = GetAbsOrigin() - ( pInflictor->GetAbsMin() + pInflictor->GetAbsMax() ) * 0.5;
		vecDir = vecDir.Normalize();

		//TODO: use human hull here? - Solokiller
		float flForce = info.GetDamage() * ( ( 32 * 32 * 72.0 ) / ( GetBounds().x * GetBounds().y * GetBounds().z ) ) * 5;

		if( flForce > 1000.0 )
			flForce = 1000.0;
		SetAbsVelocity( GetAbsVelocity() + vecDir * flForce );
	}

	// do the damage
	SetHealth( GetHealth() - info.GetDamage() );
	if( GetHealth() <= 0 )
	{
		Killed( info, GIB_NORMAL );
		return;
	}

	return;
}

void CBaseEntity::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	SetTakeDamageMode( DAMAGE_NO );
	SetDeadFlag( DEAD_DEAD );
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

		if( bResult )
		{
			save.WriteString( "classificationOverride", EntityClassifications().GetClassificationName( GetClassificationOverride() ).c_str() );
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

	if( bResult )
	{
		SetClassificationOverride( EntityClassifications().GetClassificationId( restore.ReadNamedString( "classificationOverride" ) ) );
	}

	if( GetModelIndex() != 0 && HasModel() )
	{
		Vector mins = GetRelMin();	// Set model is about to destroy these
		Vector maxs = GetRelMax();


		PRECACHE_MODEL( GetModelName() );
		SetModel( GetModelName() );
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
	if( pOther->GetAbsMin().x > GetAbsMax().x ||
		pOther->GetAbsMin().y > GetAbsMax().y ||
		pOther->GetAbsMin().z > GetAbsMax().z ||
		pOther->GetAbsMax().x < GetAbsMin().x ||
		pOther->GetAbsMax().y < GetAbsMin().y ||
		pOther->GetAbsMax().z < GetAbsMin().z )
		return false;
	return true;
}

void CBaseEntity::MakeDormant( void )
{
	GetFlags() |= FL_DORMANT;

	// Don't touch
	SetSolidType( SOLID_NOT );
	// Don't move
	SetMoveType( MOVETYPE_NONE );
	// Don't draw
	GetEffects() |= EF_NODRAW;
	// Don't think
	SetNextThink( 0 );
	// Relink
	SetAbsOrigin( GetAbsOrigin() );
}

bool CBaseEntity::IsDormant() const
{
	return GetFlags().Any( FL_DORMANT );
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
	if( GetAbsVelocity().x >= MAX_VELOCITY ) return false;
	if( GetAbsVelocity().y >= MAX_VELOCITY ) return false;
	if( GetAbsVelocity().z >= MAX_VELOCITY ) return false;
	if( GetAbsVelocity().x <= -MAX_VELOCITY ) return false;
	if( GetAbsVelocity().y <= -MAX_VELOCITY ) return false;
	if( GetAbsVelocity().z <= -MAX_VELOCITY ) return false;

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
	if( GetRenderMode() == kRenderTransAlpha )
		return -1;

	if( GetRenderMode() != kRenderNormal )
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
	pEntity->SetAbsOrigin( vecOrigin );
	pEntity->SetAbsAngles( vecAngles );

	if( bSpawnEntity )
	{
		//This didn't use to handle self removing entities. - Solokiller
		if( -1 == DispatchSpawn( pEntity->edict() ) )
			return nullptr;
	}

	return pEntity;
}


