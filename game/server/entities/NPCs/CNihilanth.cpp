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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Weapons.h"
#include "nodes/Nodes.h"
#include "Effects.h"

#include "CNihilanthHVR.h"

#include "CNihilanth.h"

BEGIN_DATADESC(	CNihilanth )
	DEFINE_FIELD( m_flForce, FIELD_FLOAT ),
	DEFINE_FIELD( m_flNextPainSound, FIELD_TIME ),
	DEFINE_FIELD( m_velocity, FIELD_VECTOR ),
	DEFINE_FIELD( m_avelocity, FIELD_VECTOR ),
	DEFINE_FIELD( m_vecTarget, FIELD_VECTOR ),
	DEFINE_FIELD( m_posTarget, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecDesired, FIELD_VECTOR ),
	DEFINE_FIELD( m_posDesired, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_flMinZ, FIELD_FLOAT ),
	DEFINE_FIELD( m_flMaxZ, FIELD_FLOAT ),
	DEFINE_FIELD( m_vecGoal, FIELD_VECTOR ),
	DEFINE_FIELD( m_flLastSeen, FIELD_TIME ),
	DEFINE_FIELD( m_flPrevSeen, FIELD_TIME ),
	DEFINE_FIELD( m_irritation, FIELD_INTEGER ),
	DEFINE_FIELD( m_iLevel, FIELD_INTEGER ),
	DEFINE_FIELD( m_iTeleport, FIELD_INTEGER ),
	DEFINE_FIELD( m_hRecharger, FIELD_EHANDLE ),
	DEFINE_ARRAY( m_hSphere, FIELD_EHANDLE, N_SPHERES ),
	DEFINE_FIELD( m_iActiveSpheres, FIELD_INTEGER ),
	DEFINE_FIELD( m_flAdj, FIELD_FLOAT ),
	DEFINE_FIELD( m_pBall, FIELD_CLASSPTR ),
	DEFINE_ARRAY( m_szRechargerTarget, FIELD_CHARACTER, 64 ),
	DEFINE_ARRAY( m_szDrawUse, FIELD_CHARACTER, 64 ),
	DEFINE_ARRAY( m_szTeleportUse, FIELD_CHARACTER, 64 ),
	DEFINE_ARRAY( m_szTeleportTouch, FIELD_CHARACTER, 64 ),
	DEFINE_ARRAY( m_szDeadUse, FIELD_CHARACTER, 64 ),
	DEFINE_ARRAY( m_szDeadTouch, FIELD_CHARACTER, 64 ),
	DEFINE_FIELD( m_flShootEnd, FIELD_TIME ),
	DEFINE_FIELD( m_flShootTime, FIELD_TIME ),
	DEFINE_ARRAY( m_hFriend, FIELD_EHANDLE, 3 ),
	DEFINE_THINKFUNC( StartupThink ),
	DEFINE_THINKFUNC( HuntThink ),
	DEFINE_TOUCHFUNC( CrashTouch ),
	DEFINE_THINKFUNC( DyingThink ),
	DEFINE_USEFUNC( StartupUse ),
	DEFINE_THINKFUNC( NullThink ),
	DEFINE_USEFUNC( CommandUse ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_nihilanth, CNihilanth );

//=========================================================
// Nihilanth, final Boss monster
//=========================================================

const char *CNihilanth::pAttackSounds[] = 
{
	"X/x_attack1.wav",
	"X/x_attack2.wav",
	"X/x_attack3.wav",
};

const char *CNihilanth::pBallSounds[] = 
{
	"X/x_ballattack1.wav",
};

const char *CNihilanth::pShootSounds[] = 
{
	"X/x_shoot1.wav",
};

const char *CNihilanth::pRechargeSounds[] = 
{
	"X/x_recharge1.wav",
	"X/x_recharge2.wav",
	"X/x_recharge3.wav",
};

const char *CNihilanth::pLaughSounds[] = 
{
	"X/x_laugh1.wav",
	"X/x_laugh2.wav",
};

const char *CNihilanth::pPainSounds[] = 
{
	"X/x_pain1.wav",
	"X/x_pain2.wav",
};

const char *CNihilanth::pDeathSounds[] = 
{
	"X/x_die1.wav",
};


void CNihilanth :: Spawn( void )
{
	Precache( );
	// motor
	SetMoveType( MOVETYPE_FLY );
	SetSolidType( SOLID_BBOX );

	SetModel( "models/nihilanth.mdl");
	// SetSize( Vector( -300, -300, 0), Vector(300, 300, 512) );
	SetSize( Vector( -32, -32, 0), Vector(32, 32, 64) );
	SetAbsOrigin( GetAbsOrigin() );

	GetFlags() |= FL_MONSTER;
	SetTakeDamageMode( DAMAGE_AIM );
	SetHealth( gSkillData.GetNihilanthHealth() );
	SetViewOffset( Vector( 0, 0, 300 ) );

	m_flFieldOfView = -1; // 360 degrees

	SetSequence( 0 );
	ResetSequenceInfo( );

	InitBoneControllers();

	SetThink( &CNihilanth::StartupThink );
	SetNextThink( gpGlobals->time + 0.1 );

	m_vecDesired = Vector( 1, 0, 0 );
	m_posDesired = Vector( GetAbsOrigin().x, GetAbsOrigin().y, 512 );

	m_iLevel = 1; 
	m_iTeleport = 1;

	if (m_szRechargerTarget[0] == '\0')	strcpy( m_szRechargerTarget, "n_recharger" );
	if (m_szDrawUse[0] == '\0')			strcpy( m_szDrawUse, "n_draw" );
	if (m_szTeleportUse[0] == '\0')		strcpy( m_szTeleportUse, "n_leaving" );
	if (m_szTeleportTouch[0] == '\0')	strcpy( m_szTeleportTouch, "n_teleport" );
	if (m_szDeadUse[0] == '\0')			strcpy( m_szDeadUse, "n_dead" );
	if (m_szDeadTouch[0] == '\0')		strcpy( m_szDeadTouch, "n_ending" );

	// near death
	/*
	m_iTeleport = 10;
	m_iLevel = 10;
	m_irritation = 2;
	SetHealth( 100 );
	*/
}


void CNihilanth::Precache( void )
{
	PRECACHE_MODEL("models/nihilanth.mdl");
	PRECACHE_MODEL("sprites/lgtning.spr");
	UTIL_PrecacheOther( "nihilanth_energy_ball" );
	UTIL_PrecacheOther( "monster_alien_controller" );
	UTIL_PrecacheOther( "monster_alien_slave" );

	PRECACHE_SOUND_ARRAY( pAttackSounds );
	PRECACHE_SOUND_ARRAY( pBallSounds );
	PRECACHE_SOUND_ARRAY( pShootSounds );
	PRECACHE_SOUND_ARRAY( pRechargeSounds );
	PRECACHE_SOUND_ARRAY( pLaughSounds );
	PRECACHE_SOUND_ARRAY( pPainSounds );
	PRECACHE_SOUND_ARRAY( pDeathSounds );
	PRECACHE_SOUND("debris/beamstart7.wav");
}

void CNihilanth::UpdateOnRemove()
{
	BaseClass::UpdateOnRemove();

	if( m_pBall )
	{
		UTIL_Remove( m_pBall );
		m_pBall = nullptr;
	}

	for( auto& hSphere : m_hSphere )
	{
		if( CBaseEntity* pSphere = hSphere )
		{
			UTIL_Remove( pSphere );
			hSphere = nullptr;
		}
	}
}

void CNihilanth :: PainSound( void )
{
	if (m_flNextPainSound > gpGlobals->time)
		return;
	
	m_flNextPainSound = gpGlobals->time + RANDOM_FLOAT( 2, 5 );

	if ( GetHealth() > gSkillData.GetNihilanthHealth() / 2)
	{
		EMIT_SOUND( this, CHAN_VOICE, RANDOM_SOUND_ARRAY( pLaughSounds ), 1.0, 0.2 );
	}
	else if (m_irritation >= 2)
	{
		EMIT_SOUND( this, CHAN_VOICE, RANDOM_SOUND_ARRAY( pPainSounds ), 1.0, 0.2 );
	}
}	

void CNihilanth :: DeathSound( void )
{
	EMIT_SOUND( this, CHAN_VOICE, RANDOM_SOUND_ARRAY( pDeathSounds ), 1.0, 0.1 );
}


void CNihilanth::NullThink( void )
{
	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.5 );
}


void CNihilanth::StartupUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink( &CNihilanth::HuntThink );
	SetNextThink( gpGlobals->time + 0.1 );
	SetUse( &CNihilanth::CommandUse );
}


void CNihilanth::StartupThink( void )
{
	m_irritation = 0;
	m_flAdj = 512;

	CBaseEntity *pEntity;

	pEntity = UTIL_FindEntityByTargetname( NULL, "n_min");
	if (pEntity)
		m_flMinZ = pEntity->GetAbsOrigin().z;
	else
		m_flMinZ = -4096;

	pEntity = UTIL_FindEntityByTargetname( NULL, "n_max");
	if (pEntity)
		m_flMaxZ = pEntity->GetAbsOrigin().z;
	else
		m_flMaxZ = 4096;

	m_hRecharger = this;
	for (int i = 0; i < N_SPHERES; i++)
	{
		EmitSphere( );
	}
	m_hRecharger = NULL;

	SetThink( &CNihilanth::HuntThink);
	SetUse( &CNihilanth::CommandUse );
	SetNextThink( gpGlobals->time + 0.1 );
}


void CNihilanth::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	CBaseMonster::Killed( info, gibAction );
}

void CNihilanth :: DyingThink( void )
{
	SetNextThink( gpGlobals->time + 0.1 );
	DispatchAnimEvents( );
	StudioFrameAdvance( );

	if ( GetDeadFlag() == DEAD_NO)
	{
		DeathSound( );
		SetDeadFlag( DEAD_DYING );

		m_posDesired.z = m_flMaxZ;
	}

	if ( GetDeadFlag() == DEAD_DYING)
	{
		Flight( );

		if (fabs( GetAbsOrigin().z - m_flMaxZ ) < 16)
		{
			SetAbsVelocity( Vector( 0, 0, 0 ) );
			FireTargets( m_szDeadUse, this, this, USE_ON, 1.0 );
			SetDeadFlag( DEAD_DEAD );
		}
	}

	if (m_fSequenceFinished)
	{
		Vector vecAVelocity = GetAngularVelocity();

		vecAVelocity.y += RANDOM_FLOAT( -100, 100 );

		if ( vecAVelocity.y < -100 )
			vecAVelocity.y = -100;
		if ( vecAVelocity.y > 100 )
			vecAVelocity.y = 100;

		SetAngularVelocity( vecAVelocity );

		SetSequence( LookupSequence( "die1" ) );
	}

	if (m_pBall)
	{
		if (m_pBall->GetRenderAmount() > 0)
		{
			m_pBall->SetRenderAmount( max( 0.0f, m_pBall->GetRenderAmount() - 2) );
		}
		else
		{
			UTIL_Remove( m_pBall );
			m_pBall = NULL;
		}
	}

	Vector vecDir, vecSrc, vecAngles;

	UTIL_MakeAimVectors( GetAbsAngles() );
	int iAttachment = RANDOM_LONG( 1, 4 );

	do {
		vecDir = Vector( RANDOM_FLOAT( -1, 1 ), RANDOM_FLOAT( -1, 1 ), RANDOM_FLOAT( -1, 1 ));
	} while (DotProduct( vecDir, vecDir) > 1.0);

	switch( RANDOM_LONG( 1, 4 ))
	{
	case 1: // head
		vecDir.z = fabs( vecDir.z ) * 0.5;
		vecDir = vecDir + 2 * gpGlobals->v_up;
		break;
	case 2: // eyes
		if (DotProduct( vecDir, gpGlobals->v_forward ) < 0)
			vecDir = vecDir * -1;

		vecDir = vecDir + 2 * gpGlobals->v_forward;
		break;
	case 3: // left hand
		if (DotProduct( vecDir, gpGlobals->v_right ) > 0)
			vecDir = vecDir * -1;
		vecDir = vecDir - 2 * gpGlobals->v_right;
		break;
	case 4: // right hand
		if (DotProduct( vecDir, gpGlobals->v_right ) < 0)
			vecDir = vecDir * -1;
		vecDir = vecDir + 2 * gpGlobals->v_right;
		break;
	}

	GetAttachment( iAttachment - 1, vecSrc, vecAngles );

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecDir * 4096, ignore_monsters, ENT(pev), &tr );
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMENTPOINT );
		WRITE_SHORT( entindex() + 0x1000 * iAttachment );
		WRITE_COORD( tr.vecEndPos.x);
		WRITE_COORD( tr.vecEndPos.y);
		WRITE_COORD( tr.vecEndPos.z);
		WRITE_SHORT( g_sModelIndexLaser );
		WRITE_BYTE( 0 ); // frame start
		WRITE_BYTE( 10 ); // framerate
		WRITE_BYTE( 5 ); // life
		WRITE_BYTE( 100 );  // width
		WRITE_BYTE( 120 );   // noise
		WRITE_BYTE( 64 );   // r, g, b
		WRITE_BYTE( 128 );   // r, g, b
		WRITE_BYTE( 255);   // r, g, b
		WRITE_BYTE( 255 );	// brightness
		WRITE_BYTE( 10 );		// speed
	MESSAGE_END();

	GetAttachment( 0, vecSrc, vecAngles ); 
	CNihilanthHVR *pEntity = (CNihilanthHVR *)Create( "nihilanth_energy_ball", vecSrc, GetAbsAngles(), edict() );
	pEntity->SetAbsVelocity( Vector ( RANDOM_FLOAT( -0.7, 0.7 ), RANDOM_FLOAT( -0.7, 0.7 ), 1.0 ) * 600.0 );
	pEntity->GreenBallInit( );

	return;
}



void CNihilanth::CrashTouch( CBaseEntity *pOther )
{
	// only crash if we hit something solid
	if ( pOther->GetSolidType() == SOLID_BSP) 
	{
		SetTouch( NULL );
		SetNextThink( gpGlobals->time );
	}
}



void CNihilanth :: GibMonster( void )
{
	// EMIT_SOUND_DYN( this, CHAN_VOICE, "common/bodysplat.wav", 0.75, ATTN_NORM, 0, 200);		
}



void CNihilanth :: FloatSequence( void )
{
	if (m_irritation >= 2)
	{
		SetSequence( LookupSequence( "float_open" ) );
	}
	else if (m_avelocity.y > 30)
	{
		SetSequence( LookupSequence( "walk_r" ) );
	}
	else if (m_avelocity.y < -30)
	{
		SetSequence( LookupSequence( "walk_l" ) );
	}
	else if (m_velocity.z > 30)
	{
		SetSequence( LookupSequence( "walk_u" ) );
	} 
	else if (m_velocity.z < -30)
	{
		SetSequence( LookupSequence( "walk_d" ) );
	}
	else
	{
		SetSequence( LookupSequence( "float" ) );
	}
}


void CNihilanth :: ShootBalls( void )
{
	if (m_flShootEnd > gpGlobals->time)
	{
		Vector vecHand, vecAngle;
		
		while (m_flShootTime < m_flShootEnd && m_flShootTime < gpGlobals->time)
		{
			if (m_hEnemy != NULL)
			{
				Vector vecSrc, vecDir;
				CNihilanthHVR *pEntity;

				GetAttachment( 2, vecHand, vecAngle );
				vecSrc = vecHand + GetAbsVelocity() * (m_flShootTime - gpGlobals->time);
				// vecDir = (m_posTarget - vecSrc).Normalize( );
				vecDir = (m_posTarget - GetAbsOrigin()).Normalize( );
				vecSrc = vecSrc + vecDir * (gpGlobals->time - m_flShootTime);
				pEntity = (CNihilanthHVR *)Create( "nihilanth_energy_ball", vecSrc, GetAbsAngles(), edict() );
				pEntity->SetAbsVelocity( vecDir * 200.0 ); 
				pEntity->ZapInit( m_hEnemy );

				GetAttachment( 3, vecHand, vecAngle );
				vecSrc = vecHand + GetAbsVelocity() * (m_flShootTime - gpGlobals->time);
				// vecDir = (m_posTarget - vecSrc).Normalize( );
				vecDir = (m_posTarget - GetAbsOrigin()).Normalize( );
				vecSrc = vecSrc + vecDir * (gpGlobals->time - m_flShootTime);
				pEntity = (CNihilanthHVR *)Create( "nihilanth_energy_ball", vecSrc, GetAbsAngles(), edict() );
				pEntity->SetAbsVelocity( vecDir * 200.0 ); 
				pEntity->ZapInit( m_hEnemy );
			}
			m_flShootTime += 0.2;
		}
	}
}


void CNihilanth :: MakeFriend( Vector vecStart )
{
	int i;

	for (i = 0; i < 3; i++)
	{
		if (m_hFriend[i] != NULL && !m_hFriend[i]->IsAlive())
		{
			if (GetRenderMode() == kRenderNormal) // don't do it if they are already fading
				m_hFriend[i]->MyMonsterPointer()->FadeMonster( );
			m_hFriend[i] = NULL;
		}

		if (m_hFriend[i] == NULL)
		{
			if (RANDOM_LONG(0, 1) == 0)
			{
				int iNode = WorldGraph.FindNearestNode ( vecStart, bits_NODE_AIR );
				if (iNode != NO_NODE)
				{
					CNode &node = WorldGraph.Node( iNode );
					TraceResult tr;
					UTIL_TraceHull( node.m_vecOrigin + Vector( 0, 0, 32 ), node.m_vecOrigin + Vector( 0, 0, 32 ), dont_ignore_monsters, Hull::LARGE, NULL, &tr );
					if (tr.fStartSolid == 0)
						m_hFriend[i] = Create("monster_alien_controller", node.m_vecOrigin, GetAbsAngles() );
				}
			}
			else
			{
				int iNode = WorldGraph.FindNearestNode ( vecStart, bits_NODE_LAND | bits_NODE_WATER );
				if (iNode != NO_NODE)
				{
					CNode &node = WorldGraph.Node( iNode );
					TraceResult tr;
					UTIL_TraceHull( node.m_vecOrigin + Vector( 0, 0, 36 ), node.m_vecOrigin + Vector( 0, 0, 36 ), dont_ignore_monsters, Hull::HUMAN, NULL, &tr );
					if (tr.fStartSolid == 0)
						m_hFriend[i] = Create("monster_alien_slave", node.m_vecOrigin, GetAbsAngles() );
				}
			}
			if (m_hFriend[i] != NULL)
			{
				EMIT_SOUND( m_hFriend[i], CHAN_WEAPON, "debris/beamstart7.wav", 1.0, ATTN_NORM );
			}

			return;
		}
	}
}


void CNihilanth :: NextActivity( )
{
	UTIL_MakeAimVectors( GetAbsAngles() );

	if (m_irritation >= 2)
	{
		if (m_pBall == NULL)
		{
			m_pBall = CSprite::SpriteCreate( "sprites/tele1.spr", GetAbsOrigin(), true );
			if (m_pBall)
			{
				m_pBall->SetTransparency( kRenderTransAdd, 255, 255, 255, 255, kRenderFxNoDissipation );
				m_pBall->SetAttachment( this, 1 );
				m_pBall->SetScale( 4.0 );
				m_pBall->SetFrameRate( 10.0 );
				m_pBall->TurnOn( );
			}
		}

		if (m_pBall)
		{
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_ELIGHT );
				WRITE_SHORT( entindex( ) + 0x1000 );		// entity, attachment
				WRITE_COORD( GetAbsOrigin().x );		// origin
				WRITE_COORD( GetAbsOrigin().y );
				WRITE_COORD( GetAbsOrigin().z );
				WRITE_COORD( 256 );	// radius
				WRITE_BYTE( 255 );	// R
				WRITE_BYTE( 192 );	// G
				WRITE_BYTE( 64 );	// B
				WRITE_BYTE( 200 );	// life * 10
				WRITE_COORD( 0 ); // decay
			MESSAGE_END();
		}
	}

	if (( GetHealth() < gSkillData.GetNihilanthHealth() / 2 || m_iActiveSpheres < N_SPHERES / 2) && m_hRecharger == NULL && m_iLevel <= 9)
	{
		char szName[64];

		CBaseEntity *pEnt = NULL;
		CBaseEntity *pRecharger = NULL;
		float flDist = 8192;

		sprintf(szName, "%s%d", m_szRechargerTarget, m_iLevel );

		while ((pEnt = UTIL_FindEntityByTargetname( pEnt, szName )) != NULL)
		{
			float flLocal = (pEnt->GetAbsOrigin() - GetAbsOrigin()).Length();
			if (flLocal < flDist)
			{
				flDist = flLocal;
				pRecharger = pEnt;
			}	
		}
		
		if (pRecharger)
		{
			m_hRecharger = pRecharger;
			m_posDesired = Vector( GetAbsOrigin().x, GetAbsOrigin().y, pRecharger->GetAbsOrigin().z );
			m_vecDesired = (pRecharger->GetAbsOrigin() - m_posDesired).Normalize( );
			m_vecDesired.z = 0;
			m_vecDesired = m_vecDesired.Normalize();
		}
		else
		{
			m_hRecharger = NULL;
			ALERT( at_aiconsole, "nihilanth can't find %s\n", szName );
			m_iLevel++;
			if (m_iLevel > 9)
				m_irritation = 2;
		}
	}

	float flDist = (m_posDesired - GetAbsOrigin()).Length();
	float flDot = DotProduct( m_vecDesired, gpGlobals->v_forward );

	if (m_hRecharger != NULL)
	{
		// at we at power up yet?
		if (flDist < 128.0)
		{
			int iseq = LookupSequence( "recharge" );

			if (iseq != GetSequence() )
			{
				char szText[64];

				sprintf( szText, "%s%d", m_szDrawUse, m_iLevel );
				FireTargets( szText, this, this, USE_ON, 1.0 );

				ALERT( at_console, "fireing %s\n", szText );
			}
			SetSequence( LookupSequence( "recharge" ) );
		}
		else
		{
			FloatSequence( );
		}
		return;
	}

	if (m_hEnemy != NULL && !m_hEnemy->IsAlive())
	{
		m_hEnemy = NULL;
	}

	if (m_flLastSeen + 15 < gpGlobals->time)
	{
		m_hEnemy = NULL;
	}

	if (m_hEnemy == NULL)
	{
		Look( 4096 );
		m_hEnemy = BestVisibleEnemy( );
	}

	if (m_hEnemy != NULL && m_irritation != 0)
	{
		if (m_flLastSeen + 5 > gpGlobals->time && flDist < 256 && flDot > 0)
		{
			if (m_irritation >= 2 && GetHealth() < gSkillData.GetNihilanthHealth() / 2.0)
			{
				SetSequence( LookupSequence( "attack1_open" ) );
			}
			else 
			{
				if (RANDOM_LONG(0, 1 ) == 0)
				{
					SetSequence( LookupSequence( "attack1" ) ); // zap
				}
				else
				{
					char szText[64];

					sprintf( szText, "%s%d", m_szTeleportTouch, m_iTeleport );
					CBaseEntity *pTouch = UTIL_FindEntityByTargetname( NULL, szText );

					sprintf( szText, "%s%d", m_szTeleportUse, m_iTeleport );
					CBaseEntity *pTrigger = UTIL_FindEntityByTargetname( NULL, szText );

					if (pTrigger != NULL || pTouch != NULL)
					{
						SetSequence( LookupSequence( "attack2" ) ); // teleport
					}
					else
					{
						m_iTeleport++;
						SetSequence( LookupSequence( "attack1" ) ); // zap
					}
				}
			}
			return;
		}
	}

	FloatSequence( );		
}

void CNihilanth :: HuntThink( void )
{
	SetNextThink( gpGlobals->time + 0.1 );
	DispatchAnimEvents( );
	StudioFrameAdvance( );

	ShootBalls( );

	// if dead, force cancelation of current animation
	if ( GetHealth() <= 0)
	{
		SetThink( &CNihilanth::DyingThink );
		m_fSequenceFinished = true;
		return;
	}

	// ALERT( at_console, "health %.0f\n", GetHealth() );

	// if damaged, try to abosorb some spheres
	if ( GetHealth() < gSkillData.GetNihilanthHealth() && AbsorbSphere( ))
	{
		SetHealth( GetHealth() + ( gSkillData.GetNihilanthHealth() / N_SPHERES ) );
	}

	// get new sequence
	if (m_fSequenceFinished)
	{
		// if (!m_fSequenceLoops)
		SetFrame( 0 );
		NextActivity( );
		ResetSequenceInfo( );
		SetFrameRate( 2.0 - 1.0 * ( GetHealth() / gSkillData.GetNihilanthHealth() ) );
	}

	// look for current enemy	
	if (m_hEnemy != NULL && m_hRecharger == NULL)
	{
		if (FVisible( m_hEnemy ))
		{
			if (m_flLastSeen < gpGlobals->time - 5)
				m_flPrevSeen = gpGlobals->time;
			m_flLastSeen = gpGlobals->time;
			m_posTarget = m_hEnemy->GetAbsOrigin();
			m_vecTarget = (m_posTarget - GetAbsOrigin()).Normalize();
			m_vecDesired = m_vecTarget;
			m_posDesired = Vector( GetAbsOrigin().x, GetAbsOrigin().y, m_posTarget.z + m_flAdj );
		}
		else
		{
			m_flAdj = min( m_flAdj + 10, 1000.0f );
		}
	}

	// don't go too high
	if (m_posDesired.z > m_flMaxZ)
		m_posDesired.z = m_flMaxZ;

	// don't go too low
	if (m_posDesired.z < m_flMinZ)
		m_posDesired.z = m_flMinZ;

	Flight( );
}



void CNihilanth :: Flight( void )
{
	// estimate where I'll be facing in one seconds
	UTIL_MakeAimVectors( GetAbsAngles() + m_avelocity );
	// Vector vecEst1 = GetAbsOrigin() + m_velocity + gpGlobals->v_up * m_flForce - Vector( 0, 0, 384 );
	// float flSide = DotProduct( m_posDesired - vecEst1, gpGlobals->v_right );
	
	float flSide = DotProduct( m_vecDesired, gpGlobals->v_right );

	if (flSide < 0)
	{
		if (m_avelocity.y < 180)
		{
			m_avelocity.y += 6; // 9 * (3.0/2.0);
		}
	}
	else
	{
		if (m_avelocity.y > -180)
		{
			m_avelocity.y -= 6; // 9 * (3.0/2.0);
		}
	}
	m_avelocity.y *= 0.98;

	// estimate where I'll be in two seconds
	Vector vecEst = GetAbsOrigin() + m_velocity * 2.0 + gpGlobals->v_up * m_flForce * 20;

	// add immediate force
	UTIL_MakeAimVectors( GetAbsAngles() );
	m_velocity.x += gpGlobals->v_up.x * m_flForce;
	m_velocity.y += gpGlobals->v_up.y * m_flForce;
	m_velocity.z += gpGlobals->v_up.z * m_flForce;


	float flSpeed = m_velocity.Length();
	float flDir = DotProduct( Vector( gpGlobals->v_forward.x, gpGlobals->v_forward.y, 0 ), Vector( m_velocity.x, m_velocity.y, 0 ) );
	if (flDir < 0)
		flSpeed = -flSpeed;

	// sideways drag
	m_velocity.x = m_velocity.x * (1.0 - fabs( gpGlobals->v_right.x ) * 0.05);
	m_velocity.y = m_velocity.y * (1.0 - fabs( gpGlobals->v_right.y ) * 0.05);
	m_velocity.z = m_velocity.z * (1.0 - fabs( gpGlobals->v_right.z ) * 0.05);

	// general drag
	m_velocity = m_velocity * 0.995;
	
	// apply power to stay correct height
	if (m_flForce < 100 && vecEst.z < m_posDesired.z) 
	{
		m_flForce += 10;
	}
	else if (m_flForce > -100 && vecEst.z > m_posDesired.z)
	{
		m_flForce -= 10;
	}

	SetAbsOrigin( GetAbsOrigin() + m_velocity * 0.1 );
	SetAbsAngles( GetAbsAngles() + m_avelocity * 0.1 );

	// ALERT( at_console, "%5.0f %5.0f : %4.0f : %3.0f : %2.0f\n", m_posDesired.z, GetAbsOrigin().z, m_velocity.z, m_avelocity.y, m_flForce ); 
}


bool CNihilanth::AbsorbSphere()
{
	for (int i = 0; i < N_SPHERES; i++)
	{
		if (m_hSphere[i] != NULL)
		{
			CNihilanthHVR *pSphere = (CNihilanthHVR *)((CBaseEntity *)m_hSphere[i]);
			pSphere->AbsorbInit( );
			m_hSphere[i] = NULL;
			m_iActiveSpheres--;
			return true;
		}
	}
	return false;
}


bool CNihilanth::EmitSphere()
{
	m_iActiveSpheres = 0;
	int empty = 0;

	for (int i = 0; i < N_SPHERES; i++)
	{
		if (m_hSphere[i] != NULL)
		{
			m_iActiveSpheres++;
		}
		else
		{
			empty = i;
		}
	}

	if (m_iActiveSpheres >= N_SPHERES)
		return false;

	Vector vecSrc = m_hRecharger->GetAbsOrigin();
	CNihilanthHVR *pEntity = (CNihilanthHVR *)Create( "nihilanth_energy_ball", vecSrc, GetAbsAngles(), edict() );
	pEntity->SetAbsVelocity( GetAbsOrigin() - vecSrc );
	pEntity->CircleInit( this );

	m_hSphere[empty] = pEntity;
	return true;
}


void CNihilanth :: 	TargetSphere( USE_TYPE useType, float value )
{
	CBaseMonster *pSphere = nullptr;
	int i;
	for (i = 0; i < N_SPHERES; i++)
	{
		if (m_hSphere[i] != NULL)
		{
			pSphere = m_hSphere[i]->MyMonsterPointer();
			if (pSphere->m_hEnemy == NULL)
				break;
		}
	}
	if (i == N_SPHERES)
	{
		return;
	}

	Vector vecSrc, vecAngles;
	GetAttachment( 2, vecSrc, vecAngles ); 
	pSphere->SetAbsOrigin( vecSrc );
	pSphere->Use( this, this, useType, value );
	pSphere->SetAbsVelocity( m_vecDesired * RANDOM_FLOAT( 50, 100 ) + Vector( RANDOM_FLOAT( -50, 50 ), RANDOM_FLOAT( -50, 50 ), RANDOM_FLOAT( -50, 50 ) ) );
}



void CNihilanth :: HandleAnimEvent( AnimEvent_t& event )
{
	//TODO: no pass through to base? - Solokiller
	switch( event.event )
	{
	case 1:	// shoot 
		break;
	case 2:	// zen
		if (m_hEnemy != NULL)
		{
			if (RANDOM_LONG(0,4) == 0)
				EMIT_SOUND( this, CHAN_VOICE, RANDOM_SOUND_ARRAY( pAttackSounds ), 1.0, 0.2 );

			EMIT_SOUND( this, CHAN_WEAPON, RANDOM_SOUND_ARRAY( pBallSounds ), 1.0, 0.2 );

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_ELIGHT );
				WRITE_SHORT( entindex( ) + 0x3000 );		// entity, attachment
				WRITE_COORD( GetAbsOrigin().x );		// origin
				WRITE_COORD( GetAbsOrigin().y );
				WRITE_COORD( GetAbsOrigin().z );
				WRITE_COORD( 256 );	// radius
				WRITE_BYTE( 128 );	// R
				WRITE_BYTE( 128 );	// G
				WRITE_BYTE( 255 );	// B
				WRITE_BYTE( 10 );	// life * 10
				WRITE_COORD( 128 ); // decay
			MESSAGE_END();

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_ELIGHT );
				WRITE_SHORT( entindex( ) + 0x4000 );		// entity, attachment
				WRITE_COORD( GetAbsOrigin().x );		// origin
				WRITE_COORD( GetAbsOrigin().y );
				WRITE_COORD( GetAbsOrigin().z );
				WRITE_COORD( 256 );	// radius
				WRITE_BYTE( 128 );	// R
				WRITE_BYTE( 128 );	// G
				WRITE_BYTE( 255 );	// B
				WRITE_BYTE( 10 );	// life * 10
				WRITE_COORD( 128 ); // decay
			MESSAGE_END();
			
			m_flShootTime = gpGlobals->time;
			m_flShootEnd = gpGlobals->time + 1.0;
		}
		break;
	case 3:	// prayer
		if (m_hEnemy != NULL)
		{
			char szText[32];

			sprintf( szText, "%s%d", m_szTeleportTouch, m_iTeleport );
			CBaseEntity *pTouch = UTIL_FindEntityByTargetname( NULL, szText );

			sprintf( szText, "%s%d", m_szTeleportUse, m_iTeleport );
			CBaseEntity *pTrigger = UTIL_FindEntityByTargetname( NULL, szText );

			if (pTrigger != NULL || pTouch != NULL)
			{
				EMIT_SOUND( this, CHAN_VOICE, RANDOM_SOUND_ARRAY( pAttackSounds ), 1.0, 0.2 );

				Vector vecSrc, vecAngles;
				GetAttachment( 2, vecSrc, vecAngles ); 
				CNihilanthHVR *pEntity = (CNihilanthHVR *)Create( "nihilanth_energy_ball", vecSrc, GetAbsAngles(), edict() );
				pEntity->SetAbsVelocity( GetAbsOrigin() - vecSrc );
				pEntity->TeleportInit( this, m_hEnemy, pTrigger, pTouch );
			}
			else
			{
				m_iTeleport++; // unexpected failure

				EMIT_SOUND( this, CHAN_WEAPON, RANDOM_SOUND_ARRAY( pBallSounds ), 1.0, 0.2 );

				ALERT( at_aiconsole, "nihilanth can't target %s\n", szText );

				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_ELIGHT );
					WRITE_SHORT( entindex( ) + 0x3000 );		// entity, attachment
					WRITE_COORD( GetAbsOrigin().x );		// origin
					WRITE_COORD( GetAbsOrigin().y );
					WRITE_COORD( GetAbsOrigin().z );
					WRITE_COORD( 256 );	// radius
					WRITE_BYTE( 128 );	// R
					WRITE_BYTE( 128 );	// G
					WRITE_BYTE( 255 );	// B
					WRITE_BYTE( 10 );	// life * 10
					WRITE_COORD( 128 ); // decay
				MESSAGE_END();

				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_ELIGHT );
					WRITE_SHORT( entindex( ) + 0x4000 );		// entity, attachment
					WRITE_COORD( GetAbsOrigin().x );		// origin
					WRITE_COORD( GetAbsOrigin().y );
					WRITE_COORD( GetAbsOrigin().z );
					WRITE_COORD( 256 );	// radius
					WRITE_BYTE( 128 );	// R
					WRITE_BYTE( 128 );	// G
					WRITE_BYTE( 255 );	// B
					WRITE_BYTE( 10 );	// life * 10
					WRITE_COORD( 128 ); // decay
				MESSAGE_END();

				m_flShootTime = gpGlobals->time;
				m_flShootEnd = gpGlobals->time + 1.0;
			}
		}
		break;
	case 4:	// get a sphere
		{
			if (m_hRecharger != NULL)
			{
				if (!EmitSphere( ))
				{
					m_hRecharger = NULL;
				}
			}
		}
		break;
	case 5:	// start up sphere machine
		{
			EMIT_SOUND( this, CHAN_VOICE , RANDOM_SOUND_ARRAY( pRechargeSounds ), 1.0, 0.2 );
		}
		break;
	case 6:
		if (m_hEnemy != NULL)
		{
			Vector vecSrc, vecAngles;
			GetAttachment( 2, vecSrc, vecAngles ); 
			CNihilanthHVR *pEntity = (CNihilanthHVR *)Create( "nihilanth_energy_ball", vecSrc, GetAbsAngles(), edict() );
			pEntity->SetAbsVelocity( GetAbsOrigin() - vecSrc );
			pEntity->ZapInit( m_hEnemy );
		}
		break;
	case 7:
		/*
		Vector vecSrc, vecAngles;
		GetAttachment( 0, vecSrc, vecAngles ); 
		CNihilanthHVR *pEntity = (CNihilanthHVR *)Create( "nihilanth_energy_ball", vecSrc, GetAbsAngles(), edict() );
		pEntity->SetAbsVelocity( Vector ( RANDOM_FLOAT( -0.7, 0.7 ), RANDOM_FLOAT( -0.7, 0.7 ), 1.0 ) * 600.0 );
		pEntity->GreenBallInit( );
		*/
		break;
	}
}



void CNihilanth::CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	switch (useType)
	{
	case USE_OFF:
		{
			CBaseEntity *pTouch = UTIL_FindEntityByTargetname( NULL, m_szDeadTouch );

			if ( pTouch )
			{
				if ( m_hEnemy != NULL )
				{
					pTouch->Touch( m_hEnemy );
				}
				// if the player is using "notarget", the ending sequence won't fire unless we catch it here
				else
				{
					CBaseEntity *pEntity = UTIL_FindEntityByClassname( NULL, "player" );				
					if ( pEntity != NULL && pEntity->IsAlive() )
					{
						pTouch->Touch( pEntity );
					}
				}
			}
		}
		break;
	case USE_ON:
		if (m_irritation == 0)
		{
			m_irritation = 1;
		}
		break;
	case USE_SET:
		break;
	case USE_TOGGLE:
		break;
	}
}


void CNihilanth::OnTakeDamage( const CTakeDamageInfo& info )
{
	if ( info.GetInflictor()->GetOwner() == this )
		return;

	if (info.GetDamage() >= GetHealth() )
	{
		SetHealth( 1 );
		if (m_irritation != 3)
			return;
	}
	
	PainSound( );

	SetHealth( GetHealth() - info.GetDamage() );
}



void CNihilanth::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	if (m_irritation == 3)
		m_irritation = 2;

	if (m_irritation == 2 && tr.iHitgroup == 2 && info.GetDamage() > 2)
		m_irritation = 3;

	if (m_irritation != 3)
	{
		Vector vecBlood = ( tr.vecEndPos - GetAbsOrigin()).Normalize( );

		UTIL_BloodStream( tr.vecEndPos, vecBlood, BloodColor(), info.GetDamage() + (100 - 100 * ( GetHealth() / gSkillData.GetNihilanthHealth() )));
	}

	// SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage * 5.0);// a little surface blood.
	g_MultiDamage.AddMultiDamage( info, this );
}
