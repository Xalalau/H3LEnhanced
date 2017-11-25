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
#include "entities/DoorConstants.h"

#include "CBaseDoor.h"

BEGIN_DATADESC( CBaseDoor )
	DEFINE_FIELD( m_bHealthValue, FIELD_CHARACTER ),
	DEFINE_FIELD( m_bMoveSnd, FIELD_CHARACTER ),
	DEFINE_FIELD( m_bStopSnd, FIELD_CHARACTER ),
	
	DEFINE_FIELD( m_bLockedSound, FIELD_CHARACTER ),
	DEFINE_FIELD( m_bLockedSentence, FIELD_CHARACTER ),
	DEFINE_FIELD( m_bUnlockedSound, FIELD_CHARACTER ),
	DEFINE_FIELD( m_bUnlockedSentence, FIELD_CHARACTER ),

	DEFINE_TOUCHFUNC( DoorTouch ),
	DEFINE_THINKFUNC( DoorGoUp ),
	DEFINE_THINKFUNC( DoorGoDown ),
	DEFINE_THINKFUNC( DoorHitTop ),
	DEFINE_THINKFUNC( DoorHitBottom ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_door, CBaseDoor );
//
// func_water - same as a door. 
//
LINK_ENTITY_TO_CLASS( func_water, CBaseDoor );

//
// Cache user-entity-field values until spawn is called.
//

void CBaseDoor::KeyValue( KeyValueData *pkvd )
{

	if( FStrEq( pkvd->szKeyName, "skin" ) )//skin is used for content type
	{
		SetSkin( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "movesnd" ) )
	{
		m_bMoveSnd = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "stopsnd" ) )
	{
		m_bStopSnd = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "healthvalue" ) )
	{
		m_bHealthValue = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "locked_sound" ) )
	{
		m_bLockedSound = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "locked_sentence" ) )
	{
		m_bLockedSentence = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "unlocked_sound" ) )
	{
		m_bUnlockedSound = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "unlocked_sentence" ) )
	{
		m_bUnlockedSentence = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "WaveHeight" ) )
	{
		SetScale( atof( pkvd->szValue ) * ( 1.0 / 8.0 ) );
		pkvd->fHandled = true;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CBaseDoor::Spawn()
{
	Precache();
	SetMovedir( this );

	if( GetSkin() == 0 )
	{//normal door
		if( GetSpawnFlags().Any( SF_DOOR_PASSABLE ) )
			SetSolidType( SOLID_NOT );
		else
			SetSolidType( SOLID_BSP );
	}
	else
	{// special contents
		SetSolidType( SOLID_NOT );
		GetSpawnFlags().AddFlags( SF_DOOR_SILENT );	// water is silent for now
	}

	SetMoveType( MOVETYPE_PUSH );
	SetAbsOrigin( GetAbsOrigin() );
	SetModel( GetModelName() );

	if( GetSpeed() == 0 )
		SetSpeed( 100 );

	m_vecPosition1 = GetAbsOrigin();
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2 = m_vecPosition1 + ( GetMoveDir() * ( fabs( GetMoveDir().x * ( GetBounds().x - 2 ) ) + fabs( GetMoveDir().y * ( GetBounds().y - 2 ) ) + fabs( GetMoveDir().z * ( GetBounds().z - 2 ) ) - m_flLip ) );
	ASSERTSZ( m_vecPosition1 != m_vecPosition2, "door start/end positions are equal" );
	if( GetSpawnFlags().Any( SF_DOOR_START_OPEN ) )
	{	// swap pos1 and pos2, put door at pos2
		SetAbsOrigin( m_vecPosition2 );
		m_vecPosition2 = m_vecPosition1;
		m_vecPosition1 = GetAbsOrigin();
	}

	m_toggle_state = TS_AT_BOTTOM;

	// if the door is flagged for USE button activation only, use NULL touch function
	if( GetSpawnFlags().Any( SF_DOOR_USE_ONLY ) )
	{
		SetTouch( NULL );
	}
	else // touchable button
		SetTouch( &CBaseDoor::DoorTouch );
}


void CBaseDoor::SetToggleState( int state )
{
	if( state == TS_AT_TOP )
		SetAbsOrigin( m_vecPosition2 );
	else
		SetAbsOrigin( m_vecPosition1 );
}


void CBaseDoor::Precache( void )
{
	const char* pszSound;

	// set the door's "in-motion" sound
	pszSound = DoorMoveSound( m_bMoveSnd );

	PRECACHE_SOUND( pszSound );

	pev->noiseMoving = MAKE_STRING( pszSound );

	// set the door's 'reached destination' stop sound
	pszSound = DoorStopSound( m_bStopSnd );

	PRECACHE_SOUND( pszSound );

	pev->noiseArrived = MAKE_STRING( pszSound );

	// get door button sounds, for doors which are directly 'touched' to open

	if( m_bLockedSound )
	{
		pszSound = ButtonSound( ( int ) m_bLockedSound );
		PRECACHE_SOUND( pszSound );
		m_ls.sLockedSound = ALLOC_STRING( pszSound );
	}

	if( m_bUnlockedSound )
	{
		pszSound = ButtonSound( ( int ) m_bUnlockedSound );
		PRECACHE_SOUND( pszSound );
		m_ls.sUnlockedSound = ALLOC_STRING( pszSound );
	}

	// get sentence group names, for doors which are directly 'touched' to open
	pszSound = LockedSentence( m_bLockedSentence );

	if( pszSound )
		m_ls.sLockedSentence = MAKE_STRING( pszSound );
	else
		m_ls.sLockedSentence = iStringNull;

	pszSound = UnlockedSentence( m_bUnlockedSentence );

	if( pszSound )
		m_ls.sUnlockedSentence = MAKE_STRING( pszSound );
	else
		m_ls.sUnlockedSentence = iStringNull;
}

//
// Doors not tied to anything (e.g. button, another door) can be touched, to make them activate.
//
void CBaseDoor::DoorTouch( CBaseEntity *pOther )
{
	// Ignore touches by anything but players
	if( !pOther->IsPlayer() )
		return;

	// If door has master, and it's not ready to trigger, 
	// play 'locked' sound

	if( m_sMaster && !UTIL_IsMasterTriggered( m_sMaster, pOther ) )
		PlayLockSounds( this, &m_ls, true, false );

	// If door is somebody's target, then touching does nothing.
	// You have to activate the owner (e.g. button).

	if( HasTargetname() )
	{
		// play locked sound
		PlayLockSounds( this, &m_ls, true, false );
		return;
	}

	m_hActivator = pOther;// remember who activated the door

	if( DoorActivate() )
		SetTouch( NULL ); // Temporarily disable the touch function, until movement is finished.
}


//
// Used by SUB_UseTargets, when a door is the target of a button.
//
void CBaseDoor::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_hActivator = pActivator;
	// if not ready to be used, ignore "use" command.
	if( m_toggle_state == TS_AT_BOTTOM || ( GetSpawnFlags().Any( SF_DOOR_NO_AUTO_RETURN ) && m_toggle_state == TS_AT_TOP ) )
		DoorActivate();
}

//
// Causes the door to "do its thing", i.e. start moving, and cascade activation.
//
int CBaseDoor::DoorActivate()
{
	if( !UTIL_IsMasterTriggered( m_sMaster, m_hActivator ) )
		return 0;

	if( GetSpawnFlags().Any( SF_DOOR_NO_AUTO_RETURN ) && m_toggle_state == TS_AT_TOP )
	{// door should close
		DoorGoDown();
	}
	else
	{// door should open

		if( m_hActivator != NULL && m_hActivator->IsPlayer() )
		{
			// give health if player opened the door (medikit)
			m_hActivator->GiveHealth( m_bHealthValue, DMG_GENERIC );
		}

		// play door unlock sounds
		PlayLockSounds( this, &m_ls, false, false );

		DoorGoUp();
	}

	return 1;
}

//
// Starts the door going to its "up" position (simply ToggleData->vecPosition2).
//
void CBaseDoor::DoorGoUp( void )
{
	entvars_t	*pevActivator;

	// It could be going-down, if blocked.
	ASSERT( m_toggle_state == TS_AT_BOTTOM || m_toggle_state == TS_GOING_DOWN );

	// emit door moving and stop sounds on CHAN_STATIC so that the multicast doesn't
	// filter them out and leave a client stuck with looping door sounds!
	if( !GetSpawnFlags().Any( SF_DOOR_SILENT ) )
	{
		if( m_toggle_state != TS_GOING_UP && m_toggle_state != TS_GOING_DOWN )
			EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMoving ), 1, ATTN_NORM );
	}

	m_toggle_state = TS_GOING_UP;

	SetMoveDone( &CBaseDoor::DoorHitTop );
	if( ClassnameIs( "func_door_rotating" ) )		// !!! BUGBUG Triggered doors don't work with this yet
	{
		float	sign = 1.0;

		if( m_hActivator != NULL )
		{
			pevActivator = m_hActivator->pev;

			if( !GetSpawnFlags().Any( SF_DOOR_ONEWAY ) && GetMoveDir().y ) 		// Y axis rotation, move away from the player
			{
				Vector vec = pevActivator->origin - GetAbsOrigin();
				Vector angles = pevActivator->angles;
				angles.x = 0;
				angles.z = 0;
				UTIL_MakeVectors( angles );
				//			Vector vnext = (pevToucher->origin + (pevToucher->velocity * 10)) - GetAbsOrigin();
				UTIL_MakeVectors( pevActivator->angles );
				Vector vnext = ( pevActivator->origin + ( gpGlobals->v_forward * 10 ) ) - GetAbsOrigin();
				if( ( vec.x*vnext.y - vec.y*vnext.x ) < 0 )
					sign = -1.0;
			}
		}
		AngularMove( m_vecAngle2*sign, GetSpeed() );
	}
	else
		LinearMove( m_vecPosition2, GetSpeed() );
}


//
// The door has reached the "up" position.  Either go back down, or wait for another activation.
//
void CBaseDoor::DoorHitTop( void )
{
	if( !GetSpawnFlags().Any( SF_DOOR_SILENT ) )
	{
		STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMoving ) );
		EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseArrived ), 1, ATTN_NORM );
	}

	ASSERT( m_toggle_state == TS_GOING_UP );
	m_toggle_state = TS_AT_TOP;

	// toggle-doors don't come down automatically, they wait for refire.
	if( GetSpawnFlags().Any( SF_DOOR_NO_AUTO_RETURN ) )
	{
		// Re-instate touch method, movement is complete
		if( !GetSpawnFlags().Any( SF_DOOR_USE_ONLY ) )
			SetTouch( &CBaseDoor::DoorTouch );
	}
	else
	{
		// In flWait seconds, DoorGoDown will fire, unless wait is -1, then door stays open
		SetNextThink( GetLastThink() + m_flWait );
		SetThink( &CBaseDoor::DoorGoDown );

		if( m_flWait == -1 )
		{
			SetNextThink( -1 );
		}
	}

	// Fire the close target (if startopen is set, then "top" is closed) - netname is the close target
	if( HasNetName() && GetSpawnFlags().Any( SF_DOOR_START_OPEN ) )
		FireTargets( GetNetName(), m_hActivator, this, USE_TOGGLE, 0 );

	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 ); // this isn't finished
}


//
// Starts the door going to its "down" position (simply ToggleData->vecPosition1).
//
void CBaseDoor::DoorGoDown( void )
{
	if( !GetSpawnFlags().Any( SF_DOOR_SILENT ) )
	{
		if( m_toggle_state != TS_GOING_UP && m_toggle_state != TS_GOING_DOWN )
			EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMoving ), 1, ATTN_NORM );
	}

#ifdef DOOR_ASSERT
	ASSERT( m_toggle_state == TS_AT_TOP );
#endif // DOOR_ASSERT
	m_toggle_state = TS_GOING_DOWN;

	SetMoveDone( &CBaseDoor::DoorHitBottom );
	if( ClassnameIs( "func_door_rotating" ) )//rotating door
		AngularMove( m_vecAngle1, GetSpeed() );
	else
		LinearMove( m_vecPosition1, GetSpeed() );
}

//
// The door has reached the "down" position.  Back to quiescence.
//
void CBaseDoor::DoorHitBottom( void )
{
	if( !GetSpawnFlags().Any( SF_DOOR_SILENT ) )
	{
		STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMoving ) );
		EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseArrived ), 1, ATTN_NORM );
	}

	ASSERT( m_toggle_state == TS_GOING_DOWN );
	m_toggle_state = TS_AT_BOTTOM;

	// Re-instate touch method, cycle is complete
	if( GetSpawnFlags().Any( SF_DOOR_USE_ONLY ) )
	{// use only door
		SetTouch( NULL );
	}
	else // touchable door
		SetTouch( &CBaseDoor::DoorTouch );

	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 ); // this isn't finished

												   // Fire the close target (if startopen is set, then "top" is closed) - netname is the close target
	if( HasNetName() && !GetSpawnFlags().Any( SF_DOOR_START_OPEN ) )
		FireTargets( GetNetName(), m_hActivator, this, USE_TOGGLE, 0 );
}

void CBaseDoor::Blocked( CBaseEntity *pOther )
{
	CBaseDoor	*pDoor = NULL;


	// Hurt the blocker a little.
	if( GetDamage() )
		pOther->TakeDamage( this, this, GetDamage(), DMG_CRUSH );

	// if a door has a negative wait, it would never come back if blocked,
	// so let it just squash the object to death real fast

	if( m_flWait >= 0 )
	{
		if( m_toggle_state == TS_GOING_DOWN )
		{
			DoorGoUp();
		}
		else
		{
			DoorGoDown();
		}
	}

	// Block all door pieces with the same targetname here.
	if( HasTargetname() )
	{
		CBaseEntity* pTarget = nullptr;
		while( ( pTarget = UTIL_FindEntityByTargetname( pTarget, GetTargetname() ) ) != nullptr )
		{
			if( pTarget != this )
			{
				if( pTarget->ClassnameIs( "func_door" ) || pTarget->ClassnameIs( "func_door_rotating" ) )
				{
					pDoor = static_cast<CBaseDoor*>( pTarget );

					if( pDoor->m_flWait >= 0 )
					{
						//TODO: comparing avel with vel is probably wrong - Solokiller
						if( pDoor->GetAbsVelocity() == GetAbsVelocity() && pDoor->GetAngularVelocity() == GetAbsVelocity() )
						{
							// this is the most hacked, evil, bastardized thing I've ever seen. kjb
							if( pTarget->ClassnameIs( "func_door" ) )
							{// set origin to realign normal doors
								pDoor->SetAbsOrigin( GetAbsOrigin() );
								pDoor->SetAbsVelocity( g_vecZero );// stop!
							}
							else
							{// set angles to realign rotating doors
								pDoor->SetAbsAngles( GetAbsAngles() );
								pDoor->SetAngularVelocity( g_vecZero );
							}
						}

						if( !GetSpawnFlags().Any( SF_DOOR_SILENT ) )
							STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noiseMoving ) );

						if( pDoor->m_toggle_state == TS_GOING_DOWN )
							pDoor->DoorGoUp();
						else
							pDoor->DoorGoDown();
					}
				}
			}
		}
	}
}