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
//=========================================================
// barnacle - stationary ceiling mounted 'fishing' monster
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"
#include "entities/effects/CGib.h"

#include "CBarnacle.h"

BEGIN_DATADESC(	CBarnacle )
	DEFINE_FIELD( m_flAltitude, FIELD_FLOAT ),
	DEFINE_FIELD( m_flKillVictimTime, FIELD_TIME ),
	DEFINE_FIELD( m_cGibs, FIELD_INTEGER ),// barnacle loads up on gibs each time it kills something.
	DEFINE_FIELD( m_fTongueExtended, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fLiftingPrey, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flTongueAdj, FIELD_FLOAT ),
	DEFINE_THINKFUNC( BarnacleThink ),
	DEFINE_THINKFUNC( WaitTillDead ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_barnacle, CBarnacle );

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CBarnacle :: Classify ( void )
{
	return	CLASS_ALIEN_MONSTER;
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//
// Returns number of events handled, 0 if none.
//=========================================================
void CBarnacle :: HandleAnimEvent( AnimEvent_t& event )
{
	switch( event.event )
	{
	case BARNACLE_AE_PUKEGIB:
		CGib::SpawnRandomGibs( this, 1, 1 );
		break;
	default:
		CBaseMonster::HandleAnimEvent( event );
		break;
	}
}

//=========================================================
// Spawn
//=========================================================
void CBarnacle :: Spawn()
{
	Precache( );

	SetModel( "models/barnacle.mdl");
	SetSize( Vector(-16, -16, -32), Vector(16, 16, 0) );

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_NONE;
	pev->takedamage		= DAMAGE_AIM;
	m_bloodColor		= BLOOD_COLOR_RED;
	pev->effects		= EF_INVLIGHT; // take light from the ceiling 
	// ############ hu3lifezado ############ //
	// Abaixei o sangue para ela não encher tanto o saco. (25)
	pev->health			= 1;
	// ############ //
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;
	m_flKillVictimTime	= 0;
	m_cGibs				= 0;
	m_fLiftingPrey		= false;
	m_flTongueAdj		= -100;

	InitBoneControllers();

	SetActivity ( ACT_IDLE );

	SetThink ( &CBarnacle::BarnacleThink );
	pev->nextthink = gpGlobals->time + 0.5;

	SetAbsOrigin( GetAbsOrigin() );
}

void CBarnacle::OnTakeDamage( const CTakeDamageInfo& info )
{
	CTakeDamageInfo newInfo = info;

	if ( newInfo.GetDamageTypes() & DMG_CLUB )
	{
		newInfo.GetMutableDamage() = pev->health;
	}

	CBaseMonster::OnTakeDamage( newInfo );
}

//=========================================================
//=========================================================
void CBarnacle :: BarnacleThink ( void )
{
	CBaseEntity *pTouchEnt;
	CBaseMonster *pVictim;
	float flLength;

	pev->nextthink = gpGlobals->time + 0.1;

	if ( m_hEnemy != NULL )
	{
// barnacle has prey.

		if ( !m_hEnemy->IsAlive() )
		{
			// someone (maybe even the barnacle) killed the prey. Reset barnacle.
			m_fLiftingPrey = false;// indicate that we're not lifting prey.
			m_hEnemy = NULL;
			return;
		}

		if ( m_fLiftingPrey )
		{
			if ( m_hEnemy != NULL && m_hEnemy->pev->deadflag != DEAD_NO )
			{
				// crap, someone killed the prey on the way up.
				m_hEnemy = NULL;
				m_fLiftingPrey = false;
				return;
			}

	// still pulling prey.
			Vector vecNewEnemyOrigin = m_hEnemy->GetAbsOrigin();
			vecNewEnemyOrigin.x = GetAbsOrigin().x;
			vecNewEnemyOrigin.y = GetAbsOrigin().y;

			// guess as to where their neck is
			vecNewEnemyOrigin.x -= 6 * cos(m_hEnemy->pev->angles.y * M_PI/180.0);	
			vecNewEnemyOrigin.y -= 6 * sin(m_hEnemy->pev->angles.y * M_PI/180.0);

			m_flAltitude -= BARNACLE_PULL_SPEED;
			vecNewEnemyOrigin.z += BARNACLE_PULL_SPEED;

			if ( fabs( GetAbsOrigin().z - ( vecNewEnemyOrigin.z + m_hEnemy->pev->view_ofs.z - 8 ) ) < BARNACLE_BODY_HEIGHT )
			{
		// prey has just been lifted into position ( if the victim origin + eye height + 8 is higher than the bottom of the barnacle, it is assumed that the head is within barnacle's body )
				m_fLiftingPrey = false;

				EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_bite3.wav", 1, ATTN_NORM );

				pVictim = m_hEnemy->MyMonsterPointer();

				m_flKillVictimTime = gpGlobals->time + 10;// now that the victim is in place, the killing bite will be administered in 10 seconds.

				if ( pVictim )
				{
					pVictim->BarnacleVictimBitten( this );
					SetActivity ( ACT_EAT );
				}
			}

			m_hEnemy->SetAbsOrigin( vecNewEnemyOrigin );
		}
		else
		{
	// prey is lifted fully into feeding position and is dangling there.

			pVictim = m_hEnemy->MyMonsterPointer();

			if ( m_flKillVictimTime != -1 && gpGlobals->time > m_flKillVictimTime )
			{
				// kill!
				if ( pVictim )
				{
					pVictim->TakeDamage ( this, this, pVictim->pev->health, DMG_SLASH | DMG_ALWAYSGIB );
					m_cGibs = 3;
				}

				return;
			}

			// bite prey every once in a while
			if ( pVictim && ( RANDOM_LONG(0,49) == 0 ) )
			{
				switch ( RANDOM_LONG(0,2) )
				{
				case 0:	EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_chew1.wav", 1, ATTN_NORM );	break;
				case 1:	EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_chew2.wav", 1, ATTN_NORM );	break;
				case 2:	EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_chew3.wav", 1, ATTN_NORM );	break;
				}

				pVictim->BarnacleVictimBitten( this );
			}

		}
	}
	else
	{
// barnacle has no prey right now, so just idle and check to see if anything is touching the tongue.

		// If idle and no nearby client, don't think so often
		if ( !UTIL_FindClientInPVS( this ) )
			pev->nextthink = gpGlobals->time + RANDOM_FLOAT(1,1.5);	// Stagger a bit to keep barnacles from thinking on the same frame

		if ( m_fSequenceFinished )
		{// this is done so barnacle will fidget.
			SetActivity ( ACT_IDLE );
			m_flTongueAdj = -100;
		}

		if ( m_cGibs && RANDOM_LONG(0,99) == 1 )
		{
			// cough up a gib.
			CGib::SpawnRandomGibs( this, 1, 1 );
			m_cGibs--;

			switch ( RANDOM_LONG(0,2) )
			{
			case 0:	EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_chew1.wav", 1, ATTN_NORM );	break;
			case 1:	EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_chew2.wav", 1, ATTN_NORM );	break;
			case 2:	EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_chew3.wav", 1, ATTN_NORM );	break;
			}
		}

		pTouchEnt = TongueTouchEnt( &flLength );

		if ( pTouchEnt != NULL && m_fTongueExtended )
		{
			// tongue is fully extended, and is touching someone.
			if ( pTouchEnt->BarnacleVictimGrabbed( this ) )
			{
				EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_alert2.wav", 1, ATTN_NORM );

				SetSequenceByName ( "attack1" );
				m_flTongueAdj = -20;

				m_hEnemy = pTouchEnt;

				pTouchEnt->pev->movetype = MOVETYPE_FLY;
				pTouchEnt->pev->velocity = g_vecZero;
				pTouchEnt->pev->basevelocity = g_vecZero;
				pTouchEnt->pev->origin.x = GetAbsOrigin().x;
				pTouchEnt->pev->origin.y = GetAbsOrigin().y;

				m_fLiftingPrey = true;// indicate that we should be lifting prey.
				m_flKillVictimTime = -1;// set this to a bogus time while the victim is lifted.

				m_flAltitude = (GetAbsOrigin().z - pTouchEnt->EyePosition().z);
			}
		}
		else
		{
			// calculate a new length for the tongue to be clear of anything else that moves under it. 
			if ( m_flAltitude < flLength )
			{
				// if tongue is higher than is should be, lower it kind of slowly.
				m_flAltitude += BARNACLE_PULL_SPEED;
				m_fTongueExtended = false;
			}
			else
			{
				m_flAltitude = flLength;
				m_fTongueExtended = true;
			}

		}

	}

	// ALERT( at_console, "tounge %f\n", m_flAltitude + m_flTongueAdj );
	SetBoneController( 0, -(m_flAltitude + m_flTongueAdj) );
	StudioFrameAdvance( 0.1 );
}

//=========================================================
// Killed.
//=========================================================
void CBarnacle::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	CBaseMonster *pVictim;

	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;

	if ( m_hEnemy != NULL )
	{
		pVictim = m_hEnemy->MyMonsterPointer();

		if ( pVictim )
		{
			pVictim->BarnacleVictimReleased();
		}
	}

//	CGib::SpawnRandomGibs( pev, 4, 1 );

	switch ( RANDOM_LONG ( 0, 1 ) )
	{
	case 0:	EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_die1.wav", 1, ATTN_NORM );	break;
	case 1:	EMIT_SOUND( this, CHAN_WEAPON, "barnacle/bcl_die3.wav", 1, ATTN_NORM );	break;
	}
	
	SetActivity ( ACT_DIESIMPLE );
	SetBoneController( 0, 0 );

	StudioFrameAdvance( 0.1 );

	pev->nextthink = gpGlobals->time + 0.1;
	SetThink ( &CBarnacle::WaitTillDead );
}

//=========================================================
//=========================================================
void CBarnacle :: WaitTillDead ( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	float flInterval = StudioFrameAdvance( 0.1 );
	DispatchAnimEvents ( flInterval );

	if ( m_fSequenceFinished )
	{
		// death anim finished. 
		StopAnimation();
		SetThink ( NULL );
	}
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CBarnacle :: Precache()
{
	PRECACHE_MODEL("models/barnacle.mdl");

	PRECACHE_SOUND("barnacle/bcl_alert2.wav");//happy, lifting food up
	PRECACHE_SOUND("barnacle/bcl_bite3.wav");//just got food to mouth
	PRECACHE_SOUND("barnacle/bcl_chew1.wav");
	PRECACHE_SOUND("barnacle/bcl_chew2.wav");
	PRECACHE_SOUND("barnacle/bcl_chew3.wav");
	PRECACHE_SOUND("barnacle/bcl_die1.wav" );
	PRECACHE_SOUND("barnacle/bcl_die3.wav" );
}	

//=========================================================
// TongueTouchEnt - does a trace along the barnacle's tongue
// to see if any entity is touching it. Also stores the length
// of the trace in the int pointer provided.
//=========================================================
#define BARNACLE_CHECK_SPACING	8
CBaseEntity *CBarnacle :: TongueTouchEnt ( float *pflLength )
{
	TraceResult	tr;
	float		length;

	// trace once to hit architecture and see if the tongue needs to change position.
	UTIL_TraceLine ( GetAbsOrigin(), GetAbsOrigin() - Vector ( 0 , 0 , 2048 ), ignore_monsters, ENT(pev), &tr );
	length = fabs( GetAbsOrigin().z - tr.vecEndPos.z );
	if ( pflLength )
	{
		*pflLength = length;
	}

	Vector delta = Vector( BARNACLE_CHECK_SPACING, BARNACLE_CHECK_SPACING, 0 );
	Vector mins = GetAbsOrigin() - delta;
	Vector maxs = GetAbsOrigin() + delta;
	maxs.z = GetAbsOrigin().z;
	mins.z -= length;

	CBaseEntity *pList[10];
	int count = UTIL_EntitiesInBox( pList, 10, mins, maxs, (FL_CLIENT|FL_MONSTER) );
	if ( count )
	{
		for ( int i = 0; i < count; i++ )
		{
			// only clients and monsters
			if ( pList[i] != this && IRelationship( pList[i] ) > R_NO && pList[ i ]->pev->deadflag == DEAD_NO )	// this ent is one of our enemies. Barnacle tries to eat it.
			{
				return pList[i];
			}
		}
	}

	return NULL;
}
