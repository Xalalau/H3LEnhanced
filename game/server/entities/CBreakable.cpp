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

===== bmodels.cpp ========================================================

  spawn, think, and use functions for entities that use brush models

*/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "SaveRestore.h"
#include "CBreakable.h"
#include "Decals.h"
#include "entities/effects/CEnvExplosion.h"

extern DLL_GLOBAL Vector		g_vecAttackDir;

// =================== FUNC_Breakable ==============================================

// Just add more items to the bottom of this array and they will automagically be supported
// This is done instead of just a classname in the FGD so we can control which entities can
// be spawned, and still remain fairly flexible
const char *CBreakable::pSpawnObjects[] =
{
	NULL,				// 0
	"item_battery",		// 1
	"item_healthkit",	// 2
	"weapon_9mmhandgun",// 3
	"ammo_9mmclip",		// 4
	"weapon_9mmAR",		// 5
	"ammo_9mmAR",		// 6
	"ammo_ARgrenades",	// 7
	"weapon_shotgun",	// 8
	"ammo_buckshot",	// 9
	"weapon_crossbow",	// 10
	"ammo_crossbow",	// 11
	"weapon_357",		// 12
	"ammo_357",			// 13
	"weapon_rpg",		// 14
	"ammo_rpgclip",		// 15
	"ammo_gaussclip",	// 16
	"weapon_handgrenade",// 17
	"weapon_tripmine",	// 18
	"weapon_satchel",	// 19
	"weapon_snark",		// 20
	"weapon_hornetgun",	// 21
};

void CBreakable::KeyValue( KeyValueData* pkvd )
{
	// UNDONE_WC: explicitly ignoring these fields, but they shouldn't be in the map file!
	if (FStrEq(pkvd->szKeyName, "explosion"))
	{
		if (!stricmp(pkvd->szValue, "directed"))
			m_Explosion = expDirected;
		else if (!stricmp(pkvd->szValue, "random"))
			m_Explosion = expRandom;
		else
			m_Explosion = expRandom;

		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "material"))
	{
		int i = atoi( pkvd->szValue);

		// 0:glass, 1:metal, 2:flesh, 3:wood

		if ((i < 0) || (i >= matLastMaterial))
			m_Material = matWood;
		else
			m_Material = (Materials)i;

		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "deadmodel"))
	{
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "shards"))
	{
//			m_iShards = atof(pkvd->szValue);
			pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "gibmodel") )
	{
		m_iszGibModel = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "spawnobject") )
	{
		int object = atoi( pkvd->szValue );
		if ( object > 0 && static_cast<size_t>( object ) < ARRAYSIZE(pSpawnObjects) )
			m_iszSpawnObject = MAKE_STRING( pSpawnObjects[object] );
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "explodemagnitude") )
	{
		ExplosionSetMagnitude( atoi( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "lip") )
		pkvd->fHandled = true;
	else
		CBaseDelay::KeyValue( pkvd );
}

BEGIN_DATADESC( CBreakable )
	DEFINE_FIELD( m_Material, FIELD_INTEGER ),
	DEFINE_FIELD( m_Explosion, FIELD_INTEGER ),

// Don't need to save/restore these because we precache after restore
//	DEFINE_FIELD( m_idShard, FIELD_INTEGER ),

	DEFINE_FIELD( m_angle, FIELD_FLOAT ),
	DEFINE_FIELD( m_iszGibModel, FIELD_STRING ),
	DEFINE_FIELD( m_iszSpawnObject, FIELD_STRING ),

	DEFINE_TOUCHFUNC( BreakTouch ),
	DEFINE_THINKFUNC( Die ),

	// Explosion magnitude is stored in GetImpulse()
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_breakable, CBreakable );

void CBreakable::Spawn( void )
{
    Precache( );    

	if ( GetSpawnFlags().Any( SF_BREAK_TRIGGER_ONLY ) )
		SetTakeDamageMode( DAMAGE_NO );
	else
		SetTakeDamageMode( DAMAGE_YES );
  
	SetSolidType( SOLID_BSP );
	SetMoveType( MOVETYPE_PUSH );
    m_angle			= GetAbsAngles().y;
	Vector vecAngles = GetAbsAngles();
	vecAngles.y	= 0;
	SetAbsAngles( vecAngles );

	// HACK:  matGlass can receive decals, we need the client to know about this
	//  so use class to store the material flag
	//@see EV_HLDM_DamageDecal
	if ( m_Material == matGlass )
	{
		pev->playerclass = 1;
	}

	SetModel( GetModelName() );//set size and link into world.

	SetTouch( &CBreakable::BreakTouch );
	if ( GetSpawnFlags().Any( SF_BREAK_TRIGGER_ONLY ) )		// Only break on trigger
		SetTouch( NULL );

	// Flag unbreakable glass as "worldbrush" so it will block ALL tracelines
	if ( !IsBreakable() && GetRenderMode() != kRenderNormal )
		GetFlags() |= FL_WORLDBRUSH;
}

const char *CBreakable::pSoundsWood[] = 
{
	"debris/wood1.wav",
	"debris/wood2.wav",
	"debris/wood3.wav",
};

const char *CBreakable::pSoundsFlesh[] = 
{
	"debris/flesh1.wav",
	"debris/flesh2.wav",
	"debris/flesh3.wav",
	"debris/flesh5.wav",
	"debris/flesh6.wav",
	"debris/flesh7.wav",
};

const char *CBreakable::pSoundsMetal[] = 
{
	"debris/metal1.wav",
	"debris/metal2.wav",
	"debris/metal3.wav",
};

const char *CBreakable::pSoundsConcrete[] = 
{
	"debris/concrete1.wav",
	"debris/concrete2.wav",
	"debris/concrete3.wav",
};


const char *CBreakable::pSoundsGlass[] = 
{
	"debris/glass1.wav",
	"debris/glass2.wav",
	"debris/glass3.wav",
};

const char **CBreakable::MaterialSoundList( Materials precacheMaterial, int &soundCount )
{
	const char	**pSoundList = NULL;

    switch ( precacheMaterial ) 
	{
	case matWood:
		pSoundList = pSoundsWood;
		soundCount = ARRAYSIZE(pSoundsWood);
		break;
	case matFlesh:
		pSoundList = pSoundsFlesh;
		soundCount = ARRAYSIZE(pSoundsFlesh);
		break;
	case matComputer:
	case matUnbreakableGlass:
	case matGlass:
		pSoundList = pSoundsGlass;
		soundCount = ARRAYSIZE(pSoundsGlass);
		break;

	case matMetal:
		pSoundList = pSoundsMetal;
		soundCount = ARRAYSIZE(pSoundsMetal);
		break;

	case matCinderBlock:
	case matRocks:
		pSoundList = pSoundsConcrete;
		soundCount = ARRAYSIZE(pSoundsConcrete);
		break;
	
	
	case matCeilingTile:
	case matNone:
	default:
		soundCount = 0;
		break;
	}

	return pSoundList;
}

void CBreakable::MaterialSoundPrecache( Materials precacheMaterial )
{
	const char	**pSoundList;
	int			i, soundCount = 0;

	pSoundList = MaterialSoundList( precacheMaterial, soundCount );

	for ( i = 0; i < soundCount; i++ )
	{
		PRECACHE_SOUND( (char *)pSoundList[i] );
	}
}

void CBreakable::MaterialSoundRandom( CBaseEntity* pEntity, Materials soundMaterial, float volume )
{
	const char	**pSoundList;
	int			soundCount = 0;

	pSoundList = MaterialSoundList( soundMaterial, soundCount );

	if ( soundCount )
		EMIT_SOUND( pEntity, CHAN_BODY, pSoundList[ RANDOM_LONG(0,soundCount-1) ], volume, 1.0 );
}

void CBreakable::Precache( void )
{
	const char *pGibName;

    switch (m_Material) 
	{
	case matWood:
		pGibName = "models/woodgibs.mdl";
		
		PRECACHE_SOUND("debris/bustcrate1.wav");
		PRECACHE_SOUND("debris/bustcrate2.wav");
		break;
	case matFlesh:
		pGibName = "models/fleshgibs.mdl";
		
		PRECACHE_SOUND("debris/bustflesh1.wav");
		PRECACHE_SOUND("debris/bustflesh2.wav");
		break;
	case matComputer:
		PRECACHE_SOUND("buttons/spark5.wav");
		PRECACHE_SOUND("buttons/spark6.wav");
		pGibName = "models/computergibs.mdl";
		
		PRECACHE_SOUND("debris/bustmetal1.wav");
		PRECACHE_SOUND("debris/bustmetal2.wav");
		break;

	default:
	case matUnbreakableGlass:
	case matGlass:
		pGibName = "models/glassgibs.mdl";
		
		PRECACHE_SOUND("debris/bustglass1.wav");
		PRECACHE_SOUND("debris/bustglass2.wav");
		break;
	case matMetal:
		pGibName = "models/metalplategibs.mdl";
		
		PRECACHE_SOUND("debris/bustmetal1.wav");
		PRECACHE_SOUND("debris/bustmetal2.wav");
		break;
	case matCinderBlock:
		pGibName = "models/cindergibs.mdl";
		
		PRECACHE_SOUND("debris/bustconcrete1.wav");
		PRECACHE_SOUND("debris/bustconcrete2.wav");
		break;
	case matRocks:
		pGibName = "models/rockgibs.mdl";
		
		PRECACHE_SOUND("debris/bustconcrete1.wav");
		PRECACHE_SOUND("debris/bustconcrete2.wav");
		break;
	case matCeilingTile:
		pGibName = "models/ceilinggibs.mdl";
		
		PRECACHE_SOUND ("debris/bustceiling.wav");  
		break;
	}
	MaterialSoundPrecache( m_Material );
	if ( m_iszGibModel )
		pGibName = STRING(m_iszGibModel);

	m_idShard = PRECACHE_MODEL( (char *)pGibName );

	// Precache the spawn item's data
	if ( m_iszSpawnObject )
		UTIL_PrecacheOther( (char *)STRING( m_iszSpawnObject ) );
}

// play shard sound when func_breakable takes damage.
// the more damage, the louder the shard sound.
void CBreakable::DamageSound( void )
{
	int pitch;
	float fvol;
	const char* rgpsz[6];
	int i;
	int material = m_Material;

//	if (RANDOM_LONG(0,1))
//		return;

	if (RANDOM_LONG(0,2))
		pitch = PITCH_NORM;
	else
		pitch = 95 + RANDOM_LONG(0,34);

	fvol = RANDOM_FLOAT(0.75, 1.0);

	if (material == matComputer && RANDOM_LONG(0,1))
		material = matMetal;

	switch (material)
	{
	default:
	case matComputer:
	case matGlass:
	case matUnbreakableGlass:
		rgpsz[0] = "debris/glass1.wav";
		rgpsz[1] = "debris/glass2.wav";
		rgpsz[2] = "debris/glass3.wav";
		i = 3;
		break;

	case matWood:
		rgpsz[0] = "debris/wood1.wav";
		rgpsz[1] = "debris/wood2.wav";
		rgpsz[2] = "debris/wood3.wav";
		i = 3;
		break;

	case matMetal:
		rgpsz[0] = "debris/metal1.wav";
		rgpsz[1] = "debris/metal3.wav";
		rgpsz[2] = "debris/metal2.wav";
		i = 2;
		break;

	case matFlesh:
		rgpsz[0] = "debris/flesh1.wav";
		rgpsz[1] = "debris/flesh2.wav";
		rgpsz[2] = "debris/flesh3.wav";
		rgpsz[3] = "debris/flesh5.wav";
		rgpsz[4] = "debris/flesh6.wav";
		rgpsz[5] = "debris/flesh7.wav";
		i = 6;
		break;

	case matRocks:
	case matCinderBlock:
		rgpsz[0] = "debris/concrete1.wav";
		rgpsz[1] = "debris/concrete2.wav";
		rgpsz[2] = "debris/concrete3.wav";
		i = 3;
		break;

	case matCeilingTile:
		// UNDONE: no ceiling tile shard sound yet
		i = 0;
		break;
	}

	if (i)
		EMIT_SOUND_DYN( this, CHAN_VOICE, rgpsz[RANDOM_LONG(0,i-1)], fvol, ATTN_NORM, 0, pitch);
}

void CBreakable::BreakTouch( CBaseEntity *pOther )
{
	float flDamage;
	entvars_t*	pevToucher = pOther->pev;
	
	// only players can break these right now
	if ( !pOther->IsPlayer() || !IsBreakable() )
	{
        return;
	}

	if ( GetSpawnFlags().Any( SF_BREAK_TOUCH ) )
	{// can be broken when run into 
		flDamage = pevToucher->velocity.Length() * 0.01;

		if (flDamage >= GetHealth() )
		{
			SetTouch( NULL );
			TakeDamage( pOther, pOther, flDamage, DMG_CRUSH);

			// do a little damage to player if we broke glass or computer
			pOther->TakeDamage( this, this, flDamage/4, DMG_SLASH );
		}
	}

	if ( GetSpawnFlags().Any( SF_BREAK_PRESSURE ) && pevToucher->absmin.z >= GetRelMax().z - 2 )
	{// can be broken when stood upon
		
		// play creaking sound here.
		DamageSound();

		SetThink ( &CBreakable::Die );
		SetTouch( NULL );
		
		if ( m_flDelay == 0 )
		{// !!!BUGBUG - why doesn't zero delay work?
			m_flDelay = 0.1;
		}

		SetNextThink( GetLastThink() + m_flDelay );

	}

}

//
// Smash the our breakable object
//

// Break when triggered
void CBreakable::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( IsBreakable() )
	{
		Vector vecAngles = GetAbsAngles();
		vecAngles.y = m_angle;
		SetAbsAngles( vecAngles );
		UTIL_MakeVectors( GetAbsAngles() );
		g_vecAttackDir = gpGlobals->v_forward;

		Die();
	}
}

void CBreakable::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	// random spark if this is a 'computer' object
	if (RANDOM_LONG(0,1) )
	{
		switch( m_Material )
		{
		case matComputer:
			{
				UTIL_Sparks( tr.vecEndPos );

				float flVolume = RANDOM_FLOAT ( 0.7 , 1.0 );//random volume range
				switch ( RANDOM_LONG(0,1) )
				{
					case 0: EMIT_SOUND( this, CHAN_VOICE, "buttons/spark5.wav", flVolume, ATTN_NORM);	break;
					case 1: EMIT_SOUND( this, CHAN_VOICE, "buttons/spark6.wav", flVolume, ATTN_NORM);	break;
				}
				break;
			}
			
		case matUnbreakableGlass:
			{
				UTIL_Ricochet( tr.vecEndPos, RANDOM_FLOAT(0.5,1.5) );
				break;
			}

		default: break;
		}
	}

	CBaseDelay::TraceAttack( info, vecDir, tr );
}

//=========================================================
// Special takedamage for func_breakable. Allows us to make
// exceptions that are breakable-specific
// bitsDamageType indicates the type of damage sustained ie: DMG_CRUSH
//=========================================================
void CBreakable::OnTakeDamage( const CTakeDamageInfo& info )
{
	Vector	vecTemp;

	CTakeDamageInfo newInfo = info;

	// if Attacker == Inflictor, the attack was a melee or other instant-hit attack.
	// (that is, no actual entity projectile was involved in the attack so use the shooter's origin). 
	if ( newInfo.GetAttacker() == newInfo.GetInflictor() )
	{
		vecTemp = newInfo.GetInflictor()->GetAbsOrigin() - ( GetAbsMin() + ( GetBounds() * 0.5 ) );
		
		// if a client hit the breakable with a crowbar, and breakable is crowbar-sensitive, break it now.
		if ( newInfo.GetAttacker()->GetFlags().Any( FL_CLIENT ) &&
			 GetSpawnFlags().Any( SF_BREAK_CROWBAR ) && ( newInfo.GetDamageTypes() & DMG_CLUB))
			newInfo.GetMutableDamage() = GetHealth();
	}
	else
	// an actual missile was involved.
	{
		vecTemp = newInfo.GetInflictor()->GetAbsOrigin() - ( GetAbsMin() + ( GetBounds() * 0.5 ) );
	}
	
	if (!IsBreakable())
		return;

	// Breakables take double damage from the crowbar
	if ( newInfo.GetDamageTypes() & DMG_CLUB )
		newInfo.GetMutableDamage() *= 2;

	// Boxes / glass / etc. don't take much poison damage, just the impact of the dart - consider that 10%
	if ( newInfo.GetDamageTypes() & DMG_POISON )
		newInfo.GetMutableDamage() *= 0.1;

// this global is still used for glass and other non-monster killables, along with decals.
	g_vecAttackDir = vecTemp.Normalize();
		
// do the damage
	SetHealth( GetHealth() - newInfo.GetDamage() );
	if ( GetHealth() <= 0)
	{
		Killed( newInfo, GIB_NORMAL );
		Die();
		return;
	}

	// Make a shard noise each time func breakable is hit.
	// Don't play shard noise if cbreakable actually died.

	DamageSound();

	return;
}

void CBreakable::Die( void )
{
	Vector vecSpot;// shard origin
	Vector vecVelocity;// shard velocity
	char cFlag = 0;
	int pitch;
	float fvol;
	
	pitch = 95 + RANDOM_LONG(0,29);

	if (pitch > 97 && pitch < 103)
		pitch = 100;

	// The more negative GetHealth(), the louder
	// the sound should be.

	fvol = RANDOM_FLOAT(0.85, 1.0) + ( fabs( GetHealth() ) / 100.0);

	if (fvol > 1.0)
		fvol = 1.0;


	switch (m_Material)
	{
	case matGlass:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustglass1.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		case 1:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustglass2.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		}
		cFlag = BREAK_GLASS;
		break;

	case matWood:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustcrate1.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		case 1:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustcrate2.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		}
		cFlag = BREAK_WOOD;
		break;

	case matComputer:
	case matMetal:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustmetal1.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		case 1:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustmetal2.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		}
		cFlag = BREAK_METAL;
		break;

	case matFlesh:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustflesh1.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		case 1:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustflesh2.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		}
		cFlag = BREAK_FLESH;
		break;

	case matRocks:
	case matCinderBlock:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustconcrete1.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		case 1:	EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustconcrete2.wav", fvol, ATTN_NORM, 0, pitch);
			break;
		}
		cFlag = BREAK_CONCRETE;
		break;

	case matCeilingTile:
		EMIT_SOUND_DYN( this, CHAN_VOICE, "debris/bustceiling.wav", fvol, ATTN_NORM, 0, pitch);
		break;

	case matUnbreakableGlass:
		{
			//Unbreakable glass can't break so this is rather weird.
			//Report as much as possible information for debugging.
			ALERT( at_warning, "CBreakable::Die(%s:\"%s\", %f %f %f): Unbreakable glass broken!\n", GetClassname(), GetTargetname(), GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z );
		}
	case matNone:
	case matLastMaterial: break;
	}
    
		
	if (m_Explosion == expDirected)
		vecVelocity = g_vecAttackDir * 200;
	else
	{
		vecVelocity.x = 0;
		vecVelocity.y = 0;
		vecVelocity.z = 0;
	}

	vecSpot = GetAbsOrigin() + ( GetRelMin() + GetRelMax() ) * 0.5;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
		WRITE_BYTE( TE_BREAKMODEL);

		// position
		WRITE_COORD( vecSpot.x );
		WRITE_COORD( vecSpot.y );
		WRITE_COORD( vecSpot.z );

		// size
		WRITE_COORD_VECTOR( GetBounds() );

		// velocity
		WRITE_COORD( vecVelocity.x ); 
		WRITE_COORD( vecVelocity.y );
		WRITE_COORD( vecVelocity.z );

		// randomization
		WRITE_BYTE( 10 ); 

		// Model
		WRITE_SHORT( m_idShard );	//model id#

		// # of shards
		WRITE_BYTE( 0 );	// let client decide

		// duration
		WRITE_BYTE( 25 );// 2.5 seconds

		// flags
		WRITE_BYTE( cFlag );
	MESSAGE_END();

	float size = GetBounds().x;
	if ( size < GetBounds().y )
		size = GetBounds().y;
	if ( size < GetBounds().z )
		size = GetBounds().z;

	// !!! HACK  This should work!
	// Build a box above the entity that looks like an 8 pixel high sheet
	Vector mins = GetAbsMin();
	Vector maxs = GetAbsMax();
	mins.z = GetAbsMax().z;
	maxs.z += 8;

	// BUGBUG -- can only find 256 entities on a breakable -- should be enough
	CBaseEntity *pList[256];
	int count = UTIL_EntitiesInBox( pList, 256, mins, maxs, FL_ONGROUND );
	if ( count )
	{
		for ( int i = 0; i < count; i++ )
		{
			pList[i]->GetFlags().ClearFlags( FL_ONGROUND );
			pList[i]->SetGroundEntity( nullptr );
		}
	}

	// Don't fire something that could fire myself
	ClearTargetname();

	SetSolidType( SOLID_NOT );
	// Fire targets on break
	SUB_UseTargets( NULL, USE_TOGGLE, 0 );

	SetThink( &CBreakable::SUB_Remove );
	SetNextThink( GetLastThink() + 0.1 );
	if ( m_iszSpawnObject )
		CBaseEntity::Create( (char *)STRING(m_iszSpawnObject), VecBModelOrigin( this ), GetAbsAngles(), edict() );


	if ( Explodable() )
	{
		UTIL_CreateExplosion( Center(), GetAbsAngles(), this, ExplosionMagnitude(), true );
	}
}

bool CBreakable::IsBreakable() const
{ 
	return m_Material != matUnbreakableGlass;
}

int	CBreakable::DamageDecal( int bitsDamageType ) const
{
	if( m_Material == matGlass  )
		return DECAL_GLASSBREAK1 + RANDOM_LONG( 0, 2 );

	if( m_Material == matUnbreakableGlass )
		return DECAL_BPROOF1;

	return CBaseEntity::DamageDecal( bitsDamageType );
}