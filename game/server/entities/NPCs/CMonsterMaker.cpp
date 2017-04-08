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
//=========================================================
// Monster Maker - this is an entity that creates monsters
// in the game.
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "SaveRestore.h"
#include "ServerInterface.h"

#include "CMonsterMaker.h"

BEGIN_DATADESC(	CMonsterMaker )
	DEFINE_FIELD( m_iszMonsterClassname, FIELD_STRING ),
	DEFINE_FIELD( m_cNumMonsters, FIELD_INTEGER ),
	DEFINE_FIELD( m_cLiveChildren, FIELD_INTEGER ),
	DEFINE_FIELD( m_flGround, FIELD_FLOAT ),
	DEFINE_FIELD( m_iMaxLiveChildren, FIELD_INTEGER ),
	DEFINE_FIELD( m_fActive, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fFadeChildren, FIELD_BOOLEAN ),
	DEFINE_USEFUNC( ToggleUse ),
	DEFINE_USEFUNC( CyclicUse ),
	DEFINE_THINKFUNC( MakerThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monstermaker, CMonsterMaker );

void CMonsterMaker :: KeyValue( KeyValueData *pkvd )
{
	
	if ( FStrEq(pkvd->szKeyName, "monstercount") )
	{
		m_cNumMonsters = atoi(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if ( FStrEq(pkvd->szKeyName, "m_imaxlivechildren") )
	{
		m_iMaxLiveChildren = atoi(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if ( FStrEq(pkvd->szKeyName, "monstertype") )
	{
		m_iszMonsterClassname = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	if( FStrEq( "makerClassificationOverride", pkvd->szKeyName ) )
	{
		m_MonsterClassificationOverride = EntityClassifications().GetClassificationId( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}


void CMonsterMaker :: Spawn( )
{
	pev->solid = SOLID_NOT;

	m_cLiveChildren = 0;
	Precache();
	if ( HasTargetname() )
	{
		if ( pev->spawnflags & SF_MONSTERMAKER_CYCLIC )
		{
			SetUse ( &CMonsterMaker::CyclicUse );// drop one monster each time we fire
		}
		else
		{
			SetUse ( &CMonsterMaker::ToggleUse );// so can be turned on/off
		}

		if ( FBitSet ( pev->spawnflags, SF_MONSTERMAKER_START_ON ) )
		{// start making monsters as soon as monstermaker spawns
			m_fActive = true;
			SetThink ( &CMonsterMaker::MakerThink );
		}
		else
		{// wait to be activated.
			m_fActive = false;
			SetThink ( &CMonsterMaker::SUB_DoNothing );
		}
	}
	else
	{// no targetname, just start.
			pev->nextthink = gpGlobals->time + m_flDelay;
			m_fActive = true;
			SetThink ( &CMonsterMaker::MakerThink );
	}

	if ( m_cNumMonsters == 1 )
	{
		m_fFadeChildren = false;
	}
	else
	{
		m_fFadeChildren = true;
	}

	m_flGround = 0;
}

void CMonsterMaker :: Precache( void )
{
	CBaseMonster::Precache();

	UTIL_PrecacheOther( STRING( m_iszMonsterClassname ) );
}

//=========================================================
// MakeMonster-  this is the code that drops the monster
//=========================================================
void CMonsterMaker::MakeMonster( void )
{
	if ( m_iMaxLiveChildren > 0 && m_cLiveChildren >= m_iMaxLiveChildren )
	{// not allowed to make a new one yet. Too many live ones out right now.
		return;
	}

	if ( !m_flGround )
	{
		// set altitude. Now that I'm activated, any breakables, etc should be out from under me. 
		TraceResult tr;

		UTIL_TraceLine ( GetAbsOrigin(), GetAbsOrigin() - Vector ( 0, 0, 2048 ), ignore_monsters, ENT(pev), &tr );
		m_flGround = tr.vecEndPos.z;
	}

	Vector mins = GetAbsOrigin() - Vector( 34, 34, 0 );
	Vector maxs = GetAbsOrigin() + Vector( 34, 34, 0 );
	maxs.z = GetAbsOrigin().z;
	mins.z = m_flGround;

	CBaseEntity *pList[2];
	int count = UTIL_EntitiesInBox( pList, 2, mins, maxs, FL_CLIENT|FL_MONSTER );
	if ( count )
	{
		// don't build a stack of monsters!
		return;
	}

	CBaseEntity* pEntity = CBaseEntity::Create( STRING( m_iszMonsterClassname ), GetAbsOrigin(), GetAbsAngles(), nullptr, false );

	if( !pEntity )
	{
		ALERT( at_console, "NULL Ent in MonsterMaker!\n" );
		return;
	}
	
	// If I have a target, fire!
	if ( HasTarget() )
	{
		// delay already overloaded for this entity, so can't call SUB_UseTargets()
		FireTargets( GetTarget(), this, this, USE_TOGGLE, 0 );
	}

	pEntity->SetAbsOrigin( GetAbsOrigin() );
	pEntity->SetAbsAngles( GetAbsAngles() );
	pEntity->GetSpawnFlags() |= SF_MONSTER_FALL_TO_GROUND;

	// Children hit monsterclip brushes
	if ( GetSpawnFlags().Any(SF_MONSTERMAKER_MONSTERCLIP ) )
		pEntity->GetSpawnFlags() |= SF_MONSTER_HITMONSTERCLIP;

	DispatchSpawn( pEntity->edict() );

	pEntity->SetOwner( this );

	if ( HasNetName() )
	{
		// if I have a netname (overloaded), give the child monster that name as a targetname
		pEntity->SetTargetname( GetNetName() );
	}

	if( m_MonsterClassificationOverride != INVALID_ENTITY_CLASSIFICATION )
	{
		//If i have an overridden classification, give the child monster that classification.
		pEntity->SetClassificationOverride( m_MonsterClassificationOverride );
	}

	m_cLiveChildren++;// count this monster
	m_cNumMonsters--;

	if ( m_cNumMonsters == 0 )
	{
		// Disable this forever.  Don't kill it because it still gets death notices
		SetThink( NULL );
		SetUse( NULL );
	}
}

bool CMonsterMaker::Save( CSave& save )
{
	if( !BaseClass::Save( save ) )
		return false;

	save.WriteString( "makerClassificationOverride", EntityClassifications().GetClassificationName( m_MonsterClassificationOverride ).c_str() );

	return true;
}

bool CMonsterMaker::Restore( CRestore& restore )
{
	if( !BaseClass::Restore( restore ) )
		return false;

	m_MonsterClassificationOverride = EntityClassifications().GetClassificationId( restore.ReadNamedString( "makerClassificationOverride" ) );

	return true;
}

//=========================================================
// CyclicUse - drops one monster from the monstermaker
// each time we call this.
//=========================================================
void CMonsterMaker::CyclicUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	MakeMonster();
}

//=========================================================
// ToggleUse - activates/deactivates the monster maker
//=========================================================
void CMonsterMaker :: ToggleUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, m_fActive ) )
		return;

	if ( m_fActive )
	{
		m_fActive = false;
		SetThink ( NULL );
	}
	else
	{
		m_fActive = true;
		SetThink ( &CMonsterMaker::MakerThink );
	}

	pev->nextthink = gpGlobals->time;
}

//=========================================================
// MakerThink - creates a new monster every so often
//=========================================================
void CMonsterMaker :: MakerThink ( void )
{
	pev->nextthink = gpGlobals->time + m_flDelay;

	MakeMonster();
}


//=========================================================
//=========================================================
void CMonsterMaker::DeathNotice( CBaseEntity* pChild )
{
	// ok, we've gotten the deathnotice from our child, now clear out its owner if we don't want it to fade.
	--m_cLiveChildren;

	if( !m_fFadeChildren )
	{
		pChild->pev->owner = nullptr;
	}
}


