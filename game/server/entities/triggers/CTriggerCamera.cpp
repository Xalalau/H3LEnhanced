#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CBasePlayer.h"
#include "entities/plats/CPathCorner.h"

#include "CTriggerCamera.h"

// Global Savedata for changelevel friction modifier
BEGIN_DATADESC( CTriggerCamera )
	DEFINE_FIELD( m_hPlayer, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hTarget, FIELD_EHANDLE ),
	DEFINE_FIELD( m_pentPath, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_sPath, FIELD_STRING ),
	DEFINE_FIELD( m_flWait, FIELD_FLOAT ),
	DEFINE_FIELD( m_flReturnTime, FIELD_TIME ),
	DEFINE_FIELD( m_flStopTime, FIELD_TIME ),
	DEFINE_FIELD( m_moveDistance, FIELD_FLOAT ),
	DEFINE_FIELD( m_targetSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_initialSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_acceleration, FIELD_FLOAT ),
	DEFINE_FIELD( m_deceleration, FIELD_FLOAT ),
	DEFINE_FIELD( m_state, FIELD_BOOLEAN ),

	DEFINE_THINKFUNC( FollowTarget ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( trigger_camera, CTriggerCamera );

void CTriggerCamera::Spawn( void )
{
	SetMoveType( MOVETYPE_NOCLIP );
	SetSolidType( SOLID_NOT );							// Remove model & collisions
	SetRenderAmount( 0 );								// The engine won't draw this model if this is set to 0 and blending is on
	SetRenderMode( kRenderTransTexture );

	m_initialSpeed = GetSpeed();
	if( m_acceleration == 0 )
		m_acceleration = 500;
	if( m_deceleration == 0 )
		m_deceleration = 500;
}

void CTriggerCamera::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		m_flWait = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "moveto" ) )
	{
		m_sPath = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "acceleration" ) )
	{
		m_acceleration = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "deceleration" ) )
	{
		m_deceleration = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerCamera::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( !ShouldToggle( useType, m_state ) )
		return;

	// Toggle state
	m_state = !m_state;
	if( !m_state )
	{
		m_flReturnTime = gpGlobals->time;
		return;
	}
	if( !pActivator || !pActivator->IsPlayer() )
	{
		pActivator = CBaseEntity::Instance( g_engfuncs.pfnPEntityOfEntIndex( 1 ) );
	}

	m_hPlayer = pActivator;

	CBasePlayer* pPlayer = static_cast<CBasePlayer*>( pActivator );

	m_flReturnTime = gpGlobals->time + m_flWait;
	SetSpeed( m_initialSpeed );
	m_targetSpeed = m_initialSpeed;

	if( GetSpawnFlags().Any( SF_CAMERA_PLAYER_TARGET ) )
	{
		m_hTarget = m_hPlayer;
	}
	else
	{
		m_hTarget = GetNextTarget();
	}

	// Nothing to look at!
	if( m_hTarget == NULL )
	{
		return;
	}


	if( GetSpawnFlags().Any( SF_CAMERA_PLAYER_TAKECONTROL ) )
	{
		pPlayer->EnableControl( false );
	}

	if( m_sPath )
	{
		//TODO: trigger_camera uses path_corner, so check the classname? - Solokiller
		m_pentPath = UTIL_FindEntityByTargetname( nullptr, STRING( m_sPath ) );
	}
	else
	{
		m_pentPath = NULL;
	}

	m_flStopTime = gpGlobals->time;
	if( m_pentPath )
	{
		if( m_pentPath->GetSpeed() != 0 )
			m_targetSpeed = m_pentPath->GetSpeed();

		m_flStopTime += m_pentPath->GetDelay();
	}

	// copy over player information
	if( GetSpawnFlags().Any( SF_CAMERA_PLAYER_POSITION ) )
	{
		SetAbsOrigin( pPlayer->GetAbsOrigin() + pPlayer->GetViewOffset() );
		SetAbsAngles( Vector(
			-pPlayer->GetAbsAngles().x,
			pPlayer->GetAbsAngles().y,
			0
		) );
		SetAbsVelocity( pPlayer->GetAbsVelocity() );
	}
	else
	{
		SetAbsVelocity( Vector( 0, 0, 0 ) );
	}

	SET_VIEW( pPlayer->edict(), edict() );

	pPlayer->m_hCamera = this;

	SetModel( pPlayer->GetModelName() );

	// follow the player down
	SetThink( &CTriggerCamera::FollowTarget );
	SetNextThink( gpGlobals->time );

	m_moveDistance = 0;
	Move();
}

void CTriggerCamera::FollowTarget()
{
	if( m_hPlayer == NULL )
		return;

	CBasePlayer* pPlayer = EHANDLE_cast<CBasePlayer*>( m_hPlayer );

	if( m_hTarget == NULL || m_flReturnTime < gpGlobals->time )
	{
		if( pPlayer->IsAlive() )
		{
			SET_VIEW( pPlayer->edict(), pPlayer->edict() );
			pPlayer->EnableControl( true );
			pPlayer->m_hCamera = nullptr;
		}
		SUB_UseTargets( this, USE_TOGGLE, 0 );
		SetAngularVelocity( g_vecZero );
		m_state = false;
		return;
	}

	Vector vecGoal = UTIL_VecToAngles( m_hTarget->GetAbsOrigin() - GetAbsOrigin() );
	vecGoal.x = -vecGoal.x;

	Vector vecAngles = GetAbsAngles();

	if( vecAngles.y > 360 )
		vecAngles.y -= 360;

	if( vecAngles.y < 0 )
		vecAngles.y += 360;

	SetAbsAngles( vecAngles );

	float dx = vecGoal.x - GetAbsAngles().x;
	float dy = vecGoal.y - GetAbsAngles().y;

	if( dx < -180 )
		dx += 360;
	if( dx > 180 )
		dx = dx - 360;

	if( dy < -180 )
		dy += 360;
	if( dy > 180 )
		dy = dy - 360;

	Vector vecAVelocity = GetAngularVelocity();
	vecAVelocity.x = dx * 40 * gpGlobals->frametime;
	vecAVelocity.y = dy * 40 * gpGlobals->frametime;
	SetAngularVelocity( vecAVelocity );

	if( !GetSpawnFlags().Any( SF_CAMERA_PLAYER_TAKECONTROL ) )
	{
		SetAbsVelocity( GetAbsVelocity() * 0.8 );
		if( GetAbsVelocity().Length() < 10.0 )
			SetAbsVelocity( g_vecZero );
	}

	SetNextThink( gpGlobals->time );

	Move();
}

void CTriggerCamera::Move()
{
	// Not moving on a path, return
	if( !m_pentPath )
		return;

	// Subtract movement from the previous frame
	m_moveDistance -= GetSpeed() * gpGlobals->frametime;

	// Have we moved enough to reach the target?
	if( m_moveDistance <= 0 )
	{
		// Fire the passtarget if there is one
		if( m_pentPath->HasMessage() )
		{
			FireTargets( m_pentPath->GetMessage(), this, this, USE_TOGGLE, 0 );
			if( m_pentPath->GetSpawnFlags().Any( SF_CORNER_FIREONCE ) )
				m_pentPath->ClearMessage();
		}
		// Time to go to the next target
		m_pentPath = m_pentPath->GetNextTarget();

		// Set up next corner
		if( !m_pentPath )
		{
			SetAbsVelocity( g_vecZero );
		}
		else
		{
			if( m_pentPath->GetSpeed() != 0 )
				m_targetSpeed = m_pentPath->GetSpeed();

			Vector delta = m_pentPath->GetAbsOrigin() - GetAbsOrigin();
			m_moveDistance = delta.Length();
			SetMoveDir( delta.Normalize() );
			m_flStopTime = gpGlobals->time + m_pentPath->GetDelay();
		}
	}

	if( m_flStopTime > gpGlobals->time )
		SetSpeed( UTIL_Approach( 0, GetSpeed(), m_deceleration * gpGlobals->frametime ) );
	else
		SetSpeed( UTIL_Approach( m_targetSpeed, GetSpeed(), m_acceleration * gpGlobals->frametime ) );

	float fraction = 2 * gpGlobals->frametime;
	SetAbsVelocity( ( ( GetMoveDir() * GetSpeed() ) * fraction ) + ( GetAbsVelocity() * ( 1 - fraction ) ) );
}