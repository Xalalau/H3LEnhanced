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
	pev->solid = SOLID_NOT;
	UTIL_Remove( this );
}


void CSatchelCharge :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SetModel( "models/w_satchel.mdl");
	//SetSize( Vector( -16, -16, -4), Vector(16, 16, 32) );	// Old box -- size of headcrab monsters/players get blocked by this
	SetSize( Vector( -4, -4, -4), Vector(4, 4, 4) );	// Uses point-sized, and can be stepped over
	SetAbsOrigin( GetAbsOrigin() );

	SetTouch( &CSatchelCharge::SatchelSlide );
	SetUse( &CSatchelCharge::DetonateUse );
	SetThink( &CSatchelCharge::SatchelThink );
	pev->nextthink = gpGlobals->time + 0.1;

	pev->gravity = 0.5;
	pev->friction = 0.8;

	pev->dmg = gSkillData.GetPlrDmgSatchel();
	// ResetSequenceInfo( );
	pev->sequence = 1;
}


void CSatchelCharge::SatchelSlide( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	// pev->avelocity = Vector (300, 300, 300);
	pev->gravity = 1;// normal gravity now

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr;
	UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() - Vector(0,0,10), ignore_monsters, edict(), &tr );

	if ( tr.flFraction < 1.0 )
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;
		pev->avelocity = pev->avelocity * 0.9;
		// play sliding sound, volume based on velocity
	}
	if ( !(pev->flags & FL_ONGROUND) && pev->velocity.Length2D() > 10 )
	{
		BounceSound();
	}
	StudioFrameAdvance( );
}


void CSatchelCharge :: SatchelThink( void )
{
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	if ( GetWaterLevel() == WATERLEVEL_HEAD )
	{
		pev->movetype = MOVETYPE_FLY;
		pev->velocity = pev->velocity * 0.8;
		pev->avelocity = pev->avelocity * 0.9;
		pev->velocity.z += 8;
	}
	else if ( GetWaterLevel() == WATERLEVEL_DRY)
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}
	else
	{
		pev->velocity.z -= 8;
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
	
	//TODO: why is this here? - Solokiller
	pPlayer->pev->weapons |= (1<<m_iId);
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
		m_pPlayer->pev->weapons &= ~(1<<m_iId);
		SetThink( &CSatchel::DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
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
		// ############ hu3lifezado ############ //
		// Filosofo do Caiscais diz "Numero 1 liga"
		EMIT_SOUND(m_pPlayer, CHAN_WEAPON, "weapons/churrasqueira_liga.wav", 1.0, ATTN_NORM);
		// ############

		Vector vecSrc = m_pPlayer->GetAbsOrigin();

		Vector vecThrow = gpGlobals->v_forward * 274 + m_pPlayer->pev->velocity;

#ifndef CLIENT_DLL
		CBaseEntity *pSatchel = Create( "monster_satchel", vecSrc, Vector( 0, 0, 0), m_pPlayer->edict() );
		pSatchel->pev->velocity = vecThrow;
		pSatchel->pev->avelocity.y = 400;

		m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_satchel_radio.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel_radio.mdl");
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
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_satchel.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel.mdl");
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

	while( (pEntity = UTIL_FindEntityByClassname( pEntity, "monster_satchel" )) )
	{
		CSatchelCharge* pSatchel = ( CSatchelCharge* ) pEntity;

		if( pSatchel )
		{
			if( pSatchel->pev->owner == pOwner->edict() )
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