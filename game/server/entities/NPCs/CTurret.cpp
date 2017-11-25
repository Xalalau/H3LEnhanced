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
#include "Skill.h"
#include "cbase.h"
#include "Effects.h"
#include "Weapons.h"

#include "CTurret.h"

BEGIN_DATADESC( CTurret )
	DEFINE_FIELD( m_iStartSpin, FIELD_INTEGER ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_turret, CTurret );

void CTurret::Spawn()
{
	Precache();
	SetModel( "models/turret.mdl" );
	SetHealth( gSkillData.GetTurretHealth() );
	m_HackedGunPos = Vector( 0, 0, 12.75 );
	m_flMaxSpin = TURRET_MAXSPIN;
	Vector vecView = GetViewOffset();
	vecView.z = 12.75;
	SetViewOffset( vecView );

	CBaseTurret::Spawn();

	m_iRetractHeight = 16;
	m_iDeployHeight = 32;
	m_iMinPitch = -15;
	SetSize( Vector( -32, -32, -m_iRetractHeight ), Vector( 32, 32, m_iRetractHeight ) );

	SetThink( &CTurret::Initialize );

	m_pEyeGlow = CSprite::SpriteCreate( TURRET_GLOW_SPRITE, GetAbsOrigin(), false );
	m_pEyeGlow->SetTransparency( kRenderGlow, 255, 0, 0, 0, kRenderFxNoDissipation );
	m_pEyeGlow->SetAttachment( this, 2 );
	m_eyeBrightness = 0;

	SetNextThink( gpGlobals->time + 0.3 );
}

void CTurret::Precache()
{
	CBaseTurret::Precache();
	PRECACHE_MODEL( "models/turret.mdl" );
	PRECACHE_MODEL( TURRET_GLOW_SPRITE );
}

void CTurret::SpinUpCall( void )
{
	StudioFrameAdvance();
	SetNextThink( gpGlobals->time + 0.1 );

	// Are we already spun up? If not start the two stage process.
	if( !m_iSpin )
	{
		SetTurretAnim( TURRET_ANIM_SPIN );
		// for the first pass, spin up the the barrel
		if( !m_iStartSpin )
		{
			SetNextThink( gpGlobals->time + 1.0 ); // spinup delay
			EMIT_SOUND( this, CHAN_BODY, "turret/tu_spinup.wav", TURRET_MACHINE_VOLUME, ATTN_NORM );
			m_iStartSpin = 1;
			SetFrameRate( 0.1 );
		}
		// after the barrel is spun up, turn on the hum
		else if( GetFrameRate() >= 1.0 )
		{
			SetNextThink( gpGlobals->time + 0.1 ); // retarget delay
			EMIT_SOUND( this, CHAN_STATIC, "turret/tu_active2.wav", TURRET_MACHINE_VOLUME, ATTN_NORM );
			SetThink( &CTurret::ActiveThink );
			m_iStartSpin = 0;
			m_iSpin = 1;
		}
		else
		{
			SetFrameRate( GetFrameRate() + 0.075 );
		}
	}

	if( m_iSpin )
	{
		SetThink( &CTurret::ActiveThink );
	}
}

void CTurret::SpinDownCall( void )
{
	if( m_iSpin )
	{
		SetTurretAnim( TURRET_ANIM_SPIN );
		if( GetFrameRate() == 1.0 )
		{
			EMIT_SOUND_DYN( this, CHAN_STATIC, "turret/tu_active2.wav", 0, 0, SND_STOP, 100 );
			EMIT_SOUND( this, CHAN_ITEM, "turret/tu_spindown.wav", TURRET_MACHINE_VOLUME, ATTN_NORM );
		}
		SetFrameRate( GetFrameRate() - 0.02 );
		if( GetFrameRate() <= 0 )
		{
			SetFrameRate( 0 );
			m_iSpin = 0;
		}
	}
}

void CTurret::Shoot( Vector &vecSrc, Vector &vecDirToEnemy )
{
	FireBullets( 1, vecSrc, vecDirToEnemy, TURRET_SPREAD, TURRET_RANGE, BULLET_MONSTER_12MM, 1 );
	EMIT_SOUND( this, CHAN_WEAPON, "turret/tu_fire1.wav", 1, 0.6 );
	GetEffects() |= EF_MUZZLEFLASH;
}