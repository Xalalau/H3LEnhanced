#include "extdll.h"
#include "util.h"
#include "gamerules/GameRules.h"
#include "cbase.h"
#include "CBasePlayer.h"

#include "CTriggerHurt.h"

BEGIN_DATADESC( CTriggerHurt )
	DEFINE_TOUCHFUNC( HurtTouch ),
	DEFINE_THINKFUNC( RadiationThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( trigger_hurt, CTriggerHurt );

//
// trigger_hurt - hurts anything that touches it. if the trigger has a targetname, firing it will toggle state
//
//int gfToggleState = 0; // used to determine when all radiation trigger hurts have called 'RadiationThink'

void CTriggerHurt::Spawn( void )
{
	InitTrigger();
	SetTouch( &CTriggerHurt::HurtTouch );

	if( HasTargetname() )
	{
		SetUse( &CTriggerHurt::ToggleUse );
	}
	else
	{
		SetUse( NULL );
	}

	if( m_bitsDamageInflict & DMG_RADIATION )
	{
		SetThink( &CTriggerHurt::RadiationThink );
		SetNextThink( gpGlobals->time + RANDOM_FLOAT( 0.0, 0.5 ) );
	}

	if( GetSpawnFlags().Any( SF_TRIGGER_HURT_START_OFF ) )// if flagged to Start Turned Off, make trigger nonsolid.
		SetSolidType( SOLID_NOT );

	SetAbsOrigin( GetAbsOrigin() );		// Link into the list
}

// trigger hurt that causes radiation will do a radius
// check and set the player's geiger counter level
// according to distance from center of trigger

void CTriggerHurt::RadiationThink( void )
{
	CBasePlayer *pPlayer = NULL;
	float flRange;
	Vector vecSpot1;
	Vector vecSpot2;
	Vector vecRange;
	Vector origin;
	Vector view_ofs;

	// check to see if a player is in pvs
	// if not, continue	

	// set origin to center of trigger so that this check works
	origin = GetAbsOrigin();
	view_ofs = GetViewOffset();

	SetAbsOrigin( ( GetAbsMin() + GetAbsMax() ) * 0.5 );
	SetViewOffset( GetViewOffset() * 0.0 );

	CBaseEntity* pentPlayer = UTIL_FindClientInPVS( this );

	SetAbsOrigin( origin );
	SetViewOffset( view_ofs );

	// reset origin

	if( !FNullEnt( pentPlayer ) )
	{
		pPlayer = static_cast<CBasePlayer*>( pentPlayer );

		// get range to player;

		vecSpot1 = ( GetAbsMin() + GetAbsMax() ) * 0.5;
		vecSpot2 = ( pPlayer->GetAbsMin() + pPlayer->GetAbsMax() ) * 0.5;

		vecRange = vecSpot1 - vecSpot2;
		flRange = vecRange.Length();

		// if player's current geiger counter range is larger
		// than range to this trigger hurt, reset player's
		// geiger counter range 

		if( pPlayer->m_flgeigerRange >= flRange )
			pPlayer->m_flgeigerRange = flRange;
	}

	SetNextThink( gpGlobals->time + 0.25 );
}

// When touched, a hurt trigger does DMG points of damage each half-second
void CTriggerHurt::HurtTouch( CBaseEntity *pOther )
{
	float fldmg;

	if( pOther->GetTakeDamageMode() == DAMAGE_NO )
		return;

	if( GetSpawnFlags().Any( SF_TRIGGER_HURT_CLIENTONLYTOUCH ) && !pOther->IsPlayer() )
	{
		// this trigger is only allowed to touch clients, and this ain't a client.
		return;
	}

	if( GetSpawnFlags().Any( SF_TRIGGER_HURT_NO_CLIENTS ) && pOther->IsPlayer() )
		return;

	// HACKHACK -- In multiplayer, players touch this based on packet receipt.
	// So the players who send packets later aren't always hurt.  Keep track of
	// how much time has passed and whether or not you've touched that player
	if( g_pGameRules->IsMultiplayer() )
	{
		if( GetDamageTime() > gpGlobals->time )
		{
			if( gpGlobals->time != pev->pain_finished )
			{// too early to hurt again, and not same frame with a different entity
				if( pOther->IsPlayer() )
				{
					int playerMask = 1 << ( pOther->entindex() - 1 );

					// If I've already touched this player (this time), then bail out
					if( GetImpulse() & playerMask )
						return;

					// Mark this player as touched
					// BUGBUG - There can be only 32 players! TODO - Solokiller
					SetImpulse( GetImpulse() | playerMask );
				}
				else
				{
					return;
				}
			}
		}
		else
		{
			// New clock, "un-touch" all players
			SetImpulse( 0 );
			if( pOther->IsPlayer() )
			{
				int playerMask = 1 << ( pOther->entindex() - 1 );

				// Mark this player as touched
				// BUGBUG - There can be only 32 players!
				SetImpulse( GetImpulse() | playerMask );
			}
		}
	}
	else	// Original code -- single player
	{
		if( GetDamageTime() > gpGlobals->time && gpGlobals->time != pev->pain_finished )
		{// too early to hurt again, and not same frame with a different entity
			return;
		}
	}



	// If this is time_based damage (poison, radiation), override the GetDamage() with a 
	// default for the given damage type.  Monsters only take time-based damage
	// while touching the trigger.  Player continues taking damage for a while after
	// leaving the trigger

	fldmg = GetDamage() * 0.5;	// 0.5 seconds worth of damage, GetDamage() is damage/second


							// JAY: Cut this because it wasn't fully realized.  Damage is simpler now.
#if 0
	switch( m_bitsDamageInflict )
	{
	default: break;
	case DMG_POISON:		fldmg = POISON_DAMAGE / 4; break;
	case DMG_NERVEGAS:		fldmg = NERVEGAS_DAMAGE / 4; break;
	case DMG_RADIATION:		fldmg = RADIATION_DAMAGE / 4; break;
	case DMG_PARALYZE:		fldmg = PARALYZE_DAMAGE / 4; break; // UNDONE: cut this? should slow movement to 50%
	case DMG_ACID:			fldmg = ACID_DAMAGE / 4; break;
	case DMG_SLOWBURN:		fldmg = SLOWBURN_DAMAGE / 4; break;
	case DMG_SLOWFREEZE:	fldmg = SLOWFREEZE_DAMAGE / 4; break;
	}
#endif

	if( fldmg < 0 )
		pOther->GiveHealth( -fldmg, m_bitsDamageInflict );
	else
		pOther->TakeDamage( this, this, fldmg, m_bitsDamageInflict );

	// Store pain time so we can get all of the other entities on this frame
	pev->pain_finished = gpGlobals->time;

	// Apply damage every half second
	SetDamageTime( gpGlobals->time + 0.5 );// half second delay until this trigger can hurt toucher again



	if( HasTarget() )
	{
		// trigger has a target it wants to fire. 
		if( GetSpawnFlags().Any( SF_TRIGGER_HURT_CLIENTONLYFIRE ) )
		{
			// if the toucher isn't a client, don't fire the target!
			if( !pOther->IsPlayer() )
			{
				return;
			}
		}

		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
		if( GetSpawnFlags().Any( SF_TRIGGER_HURT_TARGETONCE ) )
			ClearTarget();
	}
}