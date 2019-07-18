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
#include "cbase.h"
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"
#include "CSatchel.h"
#include "nodes/Nodes.h"
#include "CBasePlayer.h"
#include "gamerules/GameRules.h"

class CSatchelCharge : public CGrenade
{
public:
	DECLARE_CLASS( CSatchelCharge, CGrenade );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void BounceSound( void ) override;

	void SatchelSlide( CBaseEntity *pOther );
	void SatchelThink( void );

public:
	void Deactivate( void );
};

BEGIN_DATADESC( CSatchelCharge )
	DEFINE_TOUCHFUNC( SatchelSlide ),
	DEFINE_THINKFUNC( SatchelThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_satchel, CSatchelCharge );

//=========================================================
// Deactivate - do whatever it is we do to an orphaned 
// satchel when we don't want it in the world anymore.
//=========================================================
void CSatchelCharge::Deactivate( void )
{
	SetSolidType( SOLID_NOT );
	UTIL_Remove( this );
}


void CSatchelCharge :: Spawn( void )
{
	Precache( );
	// motor
	SetMoveType( MOVETYPE_BOUNCE );
	SetSolidType( SOLID_BBOX );

	SetModel( "models/w_satchel.mdl");
	//SetSize( Vector( -16, -16, -4), Vector(16, 16, 32) );	// Old box -- size of headcrab monsters/players get blocked by this
	SetSize( Vector( -4, -4, -4), Vector(4, 4, 4) );	// Uses point-sized, and can be stepped over
	SetAbsOrigin( GetAbsOrigin() );

	SetTouch( &CSatchelCharge::SatchelSlide );
	SetUse( &CSatchelCharge::DetonateUse );
	SetThink( &CSatchelCharge::SatchelThink );
	SetNextThink( gpGlobals->time + 0.1 );

	SetGravity( 0.5 );
	SetFriction( 0.8 );

	SetDamage( gSkillData.GetPlrDmgSatchel() );
	// ResetSequenceInfo( );
	SetSequence( 1 );
}


void CSatchelCharge::SatchelSlide( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther == GetOwner() )
		return;

	// SetAngularVelocity( Vector (300, 300, 300) );
	SetGravity( 1 );// normal gravity now

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr;
	UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() - Vector(0,0,10), ignore_monsters, edict(), &tr );

	if ( tr.flFraction < 1.0 )
	{
		// add a bit of static friction
		SetAbsVelocity( GetAbsVelocity() * 0.95 );
		SetAngularVelocity( GetAngularVelocity() * 0.9 );
		// play sliding sound, volume based on velocity
	}
	if ( !GetFlags().Any( FL_ONGROUND ) && GetAbsVelocity().Length2D() > 10 )
	{
		BounceSound();
	}
	StudioFrameAdvance( );
}


void CSatchelCharge :: SatchelThink( void )
{
	StudioFrameAdvance( );
	SetNextThink( gpGlobals->time + 0.1 );

	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	if ( GetWaterLevel() == WATERLEVEL_HEAD )
	{
		SetMoveType( MOVETYPE_FLY );
		Vector vecVelocity = GetAbsVelocity() * 0.8;
		SetAngularVelocity( GetAngularVelocity() * 0.9 );
		vecVelocity.z += 8;
		SetAbsVelocity( vecVelocity );
	}
	else if ( GetWaterLevel() == WATERLEVEL_DRY)
	{
		SetMoveType( MOVETYPE_BOUNCE );
	}
	else
	{
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.z -= 8;
		SetAbsVelocity( vecVelocity );
	}	
}

void CSatchelCharge :: Precache( void )
{
	PRECACHE_MODEL("models/grenade.mdl");
	PRECACHE_SOUND("weapons/g_bounce1.wav");
	PRECACHE_SOUND("weapons/g_bounce2.wav");
	PRECACHE_SOUND("weapons/g_bounce3.wav");
	// ############ hu3lifezado ############ //
	// Novos sons
	PRECACHE_SOUND("weapons/churrasqueira_liga.wav");
	PRECACHE_SOUND("weapons/churrasqueira_puta_vida.wav");
	// ############
}

void CSatchelCharge :: BounceSound( void )
{
	switch ( RANDOM_LONG( 0, 2 ) )
	{
	case 0:	EMIT_SOUND( this, CHAN_VOICE, "weapons/g_bounce1.wav", 1, ATTN_NORM);	break;
	case 1:	EMIT_SOUND( this, CHAN_VOICE, "weapons/g_bounce2.wav", 1, ATTN_NORM);	break;
	case 2:	EMIT_SOUND( this, CHAN_VOICE, "weapons/g_bounce3.wav", 1, ATTN_NORM);	break;
	}
}

#ifdef SERVER_DLL
BEGIN_DATADESC( CSatchel )
	//NOTENOTE: this works because enum class is int. If its type is changed, update this. - Solokiller
	DEFINE_FIELD( m_chargeReady, FIELD_INTEGER ),
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS( weapon_satchel, CSatchel );

CSatchel::CSatchel()
	: BaseClass( WEAPON_SATCHEL )
{
}

//=========================================================
// CALLED THROUGH the newly-touched weapon's instance. The existing player weapon is pOriginal
//=========================================================
bool CSatchel::AddDuplicate( CBasePlayerWeapon *pOriginal )
{
	CSatchel *pSatchel;

	if ( bIsMultiplayer() )
	{
		pSatchel = (CSatchel *)pOriginal;

		if ( pSatchel->m_chargeReady != ChargeState::NONE )
		{
			// player has some satchels deployed. Refuse to add more.
			return false;
		}
	}

	return CBasePlayerWeapon::AddDuplicate ( pOriginal );
}

//=========================================================
//=========================================================
bool CSatchel::AddToPlayer( CBasePlayer *pPlayer )
{
	int bResult = CBasePlayerWeapon::AddToPlayer( pPlayer );
	
	//TODO: if the player had no satchels left, this adds it back to the Hud weapon list. replace with CBasePlayer method - Solokiller
	pPlayer->GetWeapons() |= ( 1 << m_iId );
	//TODO: maybe do this when the satchel is dropped so it doesn't have invalid state until picked up again? - Solokiller
	//TODO: Could probably re-check if the current player has any deployed satchels on pick-up, would be more robust anyway - Solokiller
	m_chargeReady = ChargeState::NONE;// this satchel charge weapon now forgets that any satchels are deployed by it.

	if ( bResult )
	{
		return AddWeapon( );
	}
	return false;
}

void CSatchel::Spawn( )
{
	Precache( );
	SetModel( "models/w_satchel.mdl");
		
	FallInit();// get ready to fall down.
}


void CSatchel::Precache( void )
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/v_satchel.mdl");
	PRECACHE_MODEL("models/v_satchel_radio.mdl");
	PRECACHE_MODEL("models/w_satchel.mdl");
	PRECACHE_MODEL("models/p_satchel.mdl");
	PRECACHE_MODEL("models/p_satchel_radio.mdl");

	UTIL_PrecacheOther( "monster_satchel" );
}

//=========================================================
//=========================================================
bool CSatchel::IsUseable()
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 ) 
	{
		// player is carrying some satchels
		return true;
	}

	if ( m_chargeReady != ChargeState::NONE )
	{
		// player isn't carrying any satchels, but has some out
		return true;
	}

	return false;
}

bool CSatchel::CanDeploy() const
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 ) 
	{
		// player is carrying some satchels
		return true;
	}

	if ( m_chargeReady != ChargeState::NONE )
	{
		// player isn't carrying any satchels, but has some out
		return true;
	}

	return false;
}

bool CSatchel::Deploy()
{

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );

	if ( m_chargeReady != ChargeState::NONE )
		return DefaultDeploy( "models/v_satchel_radio.mdl", "models/p_satchel_radio.mdl", SATCHEL_RADIO_DRAW, "hive" );
	else
		return DefaultDeploy( "models/v_satchel.mdl", "models/p_satchel.mdl", SATCHEL_DRAW, "trip" );

	
	return true;
}


void CSatchel::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	
	if ( m_chargeReady != ChargeState::NONE )
	{
		SendWeaponAnim( SATCHEL_RADIO_HOLSTER );
	}
	else
	{
		SendWeaponAnim( SATCHEL_DROP );
	}
	EMIT_SOUND( m_pPlayer, CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);

	if ( !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] && m_chargeReady == ChargeState::NONE )
	{
		m_pPlayer->GetWeapons().ClearFlags( 1 << m_iId );
		SetThink( &CSatchel::DestroyItem );
		SetNextThink( gpGlobals->time + 0.1 );
	}
}



void CSatchel::PrimaryAttack()
{
	switch (m_chargeReady)
	{
	case ChargeState::NONE:
		{
		Throw( );
		}
		break;
	case ChargeState::DEPLOYED:
		{
		SendWeaponAnim( SATCHEL_RADIO_FIRE );

		// ############ hu3lifezado ############ //
		// Faustao diz "PUTA VIDA" + som de explosao da churrasqueira
		EMIT_SOUND(m_pPlayer, CHAN_WEAPON, "weapons/churrasqueira_puta_vida.wav", 1.0, ATTN_NORM);
		// ############

		DeactivateSatchels( m_pPlayer, SatchelAction::DETONATE );

		m_chargeReady = ChargeState::TRIGGERED;
		m_flNextPrimaryAttack = GetNextAttackDelay(0.5);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		break;
		}

	case ChargeState::TRIGGERED:
		// we're reloading, don't allow fire
		{
		}
		break;
	}
}


void CSatchel::SecondaryAttack( void )
{
	if ( m_chargeReady != ChargeState::TRIGGERED )
	{
		Throw( );
	}
}


void CSatchel::Throw( void )
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
	{
#ifndef CLIENT_DLL
		// ############ hu3lifezado ############ //
		// Filosofo do Caiscais diz "Numero 1 liga"
		EMIT_SOUND(m_pPlayer, CHAN_WEAPON, "weapons/churrasqueira_liga.wav", 1.0, ATTN_NORM);
		// ############

		Vector vecSrc = m_pPlayer->GetAbsOrigin();

		Vector vecThrow = gpGlobals->v_forward * 274 + m_pPlayer->GetAbsVelocity();

		CBaseEntity *pSatchel = Create( "monster_satchel", vecSrc, Vector( 0, 0, 0), m_pPlayer->edict() );
		pSatchel->SetAbsVelocity( vecThrow );
		pSatchel->SetAngularVelocity( Vector( 0, 400, 0 ) );

		m_pPlayer->SetViewModelName( "models/v_satchel_radio.mdl" );
		m_pPlayer->SetWeaponModelName( "models/p_satchel_radio.mdl" );
#else
		LoadVModel ( "models/v_satchel_radio.mdl", m_pPlayer );
#endif

		SendWeaponAnim( SATCHEL_RADIO_DRAW );

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_chargeReady = ChargeState::DEPLOYED;
		
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ]--;

		m_flNextPrimaryAttack = GetNextAttackDelay(1.0);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}
}


void CSatchel::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	switch( m_chargeReady )
	{
	case ChargeState::NONE:
		SendWeaponAnim( SATCHEL_FIDGET1 );
		// use tripmine animations
		m_pPlayer->SetWeaponAnimType( "trip" );
		break;
	case ChargeState::DEPLOYED:
		SendWeaponAnim( SATCHEL_RADIO_FIDGET1 );
		// use hivehand animations
		m_pPlayer->SetWeaponAnimType( "hive" );
		break;
	case ChargeState::TRIGGERED:
		if ( !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
		{
			m_chargeReady = ChargeState::NONE;
			RetireWeapon();
			return;
		}

#ifndef CLIENT_DLL
		m_pPlayer->SetViewModelName( "models/v_satchel.mdl" );
		m_pPlayer->SetWeaponModelName( "models/p_satchel.mdl" );
#else
		LoadVModel ( "models/v_satchel.mdl", m_pPlayer );
#endif

		SendWeaponAnim( SATCHEL_DRAW );

		// use tripmine animations
		m_pPlayer->SetWeaponAnimType( "trip" );

		m_flNextPrimaryAttack = GetNextAttackDelay(0.5);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_chargeReady = ChargeState::NONE;
		break;
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );// how long till we do this again.
}

size_t DeactivateSatchels( CBasePlayer* const pOwner, const SatchelAction action )
{
	size_t uiCount = 0;

	CBaseEntity* pEntity = nullptr;

	while( ( pEntity = UTIL_FindEntityByClassname( pEntity, "monster_satchel" ) ) != nullptr )
	{
		CSatchelCharge* pSatchel = ( CSatchelCharge* ) pEntity;

		if( pSatchel )
		{
			if( pSatchel->GetOwner() == pOwner )
			{
				if( action == SatchelAction::DETONATE )
					pSatchel->Use( pOwner, pOwner, USE_ON, 0 );
				else
					pSatchel->Deactivate();

				++uiCount;
			}
		}
	}

	return uiCount;
}