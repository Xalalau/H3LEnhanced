#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CPathTrack.h"
#include "CFuncTrackTrain.h"

#include "CFuncTrackChange.h"

BEGIN_DATADESC( CFuncTrackChange )
	DEFINE_GLOBAL_FIELD( m_trackTop, FIELD_CLASSPTR ),
	DEFINE_GLOBAL_FIELD( m_trackBottom, FIELD_CLASSPTR ),
	DEFINE_GLOBAL_FIELD( m_train, FIELD_CLASSPTR ),
	DEFINE_GLOBAL_FIELD( m_trackTopName, FIELD_STRING ),
	DEFINE_GLOBAL_FIELD( m_trackBottomName, FIELD_STRING ),
	DEFINE_GLOBAL_FIELD( m_trainName, FIELD_STRING ),
	DEFINE_FIELD( m_code, FIELD_INTEGER ),
	DEFINE_FIELD( m_targetState, FIELD_INTEGER ),
	DEFINE_FIELD( m_use, FIELD_INTEGER ),

	DEFINE_THINKFUNC( GoUp ),
	DEFINE_THINKFUNC( GoDown ),
	DEFINE_THINKFUNC( Find ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_trackchange, CFuncTrackChange );

void CFuncTrackChange::Spawn( void )
{
	Setup();
	if( GetSpawnFlags().Any( SF_TRACK_DONT_MOVE ) )
		m_vecPosition2.z = GetAbsOrigin().z;

	SetupRotation();

	if( GetSpawnFlags().Any( SF_TRACK_STARTBOTTOM ) )
	{
		SetAbsOrigin( m_vecPosition2 );
		m_toggle_state = TS_AT_BOTTOM;
		SetAbsAngles( m_start );
		m_targetState = TS_AT_TOP;
	}
	else
	{
		SetAbsOrigin( m_vecPosition1 );
		m_toggle_state = TS_AT_TOP;
		SetAbsAngles( m_end );
		m_targetState = TS_AT_BOTTOM;
	}

	EnableUse();
	SetNextThink( GetLastThink() + 2.0 );
	SetThink( &CFuncTrackChange::Find );
	Precache();
}

void CFuncTrackChange::Precache( void )
{
	// Can't trigger sound
	PRECACHE_SOUND( "buttons/button11.wav" );

	CFuncPlatRot::Precache();
}

//
// Platform is at bottom, now starts moving up
//
void CFuncTrackChange::GoUp( void )
{
	if( m_code == TRAIN_BLOCKING )
		return;

	// HitTop may get called during CFuncPlat::GoUp(), so set up for that
	// before you call GoUp();

	UpdateAutoTargets( TS_GOING_UP );
	if( GetSpawnFlags().Any( SF_TRACK_DONT_MOVE ) )
	{
		m_toggle_state = TS_GOING_UP;
		SetMoveDone( &CFuncTrackChange::CallHitTop );
		AngularMove( m_end, GetSpeed() );
	}
	else
	{
		// If ROTMOVE, move & rotate
		CFuncPlat::GoUp();
		SetMoveDone( &CFuncTrackChange::CallHitTop );
		RotMove( m_end, GetNextThink() - GetLastThink() );
	}

	// Otherwise, move first, rotate second

	// If the train is moving with the platform, update it
	if( m_code == TRAIN_FOLLOWING )
	{
		UpdateTrain( m_end );
		m_train->m_ppath = NULL;
	}
}

void CFuncTrackChange::GoDown( void )
{
	if( m_code == TRAIN_BLOCKING )
		return;

	// HitBottom may get called during CFuncPlat::GoDown(), so set up for that
	// before you call GoDown()

	UpdateAutoTargets( TS_GOING_DOWN );
	// If ROTMOVE, move & rotate
	if( GetSpawnFlags().Any( SF_TRACK_DONT_MOVE ) )
	{
		SetMoveDone( &CFuncTrackChange::CallHitBottom );
		m_toggle_state = TS_GOING_DOWN;
		AngularMove( m_start, GetSpeed() );
	}
	else
	{
		CFuncPlat::GoDown();
		SetMoveDone( &CFuncTrackChange::CallHitBottom );
		RotMove( m_start, GetNextThink() - GetLastThink() );
	}
	// Otherwise, rotate first, move second

	// If the train is moving with the platform, update it
	if( m_code == TRAIN_FOLLOWING )
	{
		UpdateTrain( m_start );
		m_train->m_ppath = NULL;
	}
}

void CFuncTrackChange::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "train" ) )
	{
		m_trainName = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "toptrack" ) )
	{
		m_trackTopName = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "bottomtrack" ) )
	{
		m_trackBottomName = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
	{
		CFuncPlatRot::KeyValue( pkvd );		// Pass up to base class
	}
}

void CFuncTrackChange::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( m_toggle_state != TS_AT_TOP && m_toggle_state != TS_AT_BOTTOM )
		return;

	// If train is in "safe" area, but not on the elevator, play alarm sound
	if( m_toggle_state == TS_AT_TOP )
		m_code = EvaluateTrain( m_trackTop );
	else if( m_toggle_state == TS_AT_BOTTOM )
		m_code = EvaluateTrain( m_trackBottom );
	else
		m_code = TRAIN_BLOCKING;
	if( m_code == TRAIN_BLOCKING )
	{
		// Play alarm and return
		EMIT_SOUND( this, CHAN_VOICE, "buttons/button11.wav", 1, ATTN_NORM );
		return;
	}

	// Otherwise, it's safe to move
	// If at top, go down
	// at bottom, go up

	DisableUse();
	if( m_toggle_state == TS_AT_TOP )
		GoDown();
	else
		GoUp();
}

void CFuncTrackChange::Find( void )
{
	// Find track entities
	CBaseEntity* pTarget = nullptr;

	pTarget = UTIL_FindEntityByTargetname( nullptr, STRING( m_trackTopName ) );
	if( pTarget )
	{
		m_trackTop = CPathTrack::Instance( pTarget );
		pTarget = UTIL_FindEntityByTargetname( nullptr, STRING( m_trackBottomName ) );
		if( pTarget )
		{
			m_trackBottom = CPathTrack::Instance( pTarget );
			pTarget = UTIL_FindEntityByTargetname( nullptr, STRING( m_trainName ) );
			if( pTarget )
			{
				m_train = CFuncTrackTrain::Instance( UTIL_FindEntityByTargetname( nullptr, STRING( m_trainName ) ) );
				if( !m_train )
				{
					ALERT( at_error, "Can't find train for track change! %s\n", STRING( m_trainName ) );
					return;
				}
				Vector center = ( GetAbsMin() + GetAbsMax() ) * 0.5;
				m_trackBottom = m_trackBottom->Nearest( center );
				m_trackTop = m_trackTop->Nearest( center );
				UpdateAutoTargets( m_toggle_state );
				SetThink( NULL );
				return;
			}
			else
			{
				ALERT( at_error, "Can't find train for track change! %s\n", STRING( m_trainName ) );
			}
		}
		else
			ALERT( at_error, "Can't find bottom track for track change! %s\n", STRING( m_trackBottomName ) );
	}
	else
		ALERT( at_error, "Can't find top track for track change! %s\n", STRING( m_trackTopName ) );
}

TRAIN_CODE CFuncTrackChange::EvaluateTrain( CPathTrack *pcurrent )
{
	// Go ahead and work, we don't have anything to switch, so just be an elevator
	if( !pcurrent || !m_train )
		return TRAIN_SAFE;

	if( m_train->m_ppath == pcurrent || ( pcurrent->m_pprevious && m_train->m_ppath == pcurrent->m_pprevious ) ||
		( pcurrent->m_pnext && m_train->m_ppath == pcurrent->m_pnext ) )
	{
		if( m_train->GetSpeed() != 0 )
			return TRAIN_BLOCKING;

		Vector dist = GetAbsOrigin() - m_train->GetAbsOrigin();
		float length = dist.Length2D();
		if( length < m_train->m_length )		// Empirically determined close distance
			return TRAIN_FOLLOWING;
		else if( length >( 150 + m_train->m_length ) )
			return TRAIN_SAFE;

		return TRAIN_BLOCKING;
	}

	return TRAIN_SAFE;
}

void CFuncTrackChange::UpdateTrain( Vector &dest )
{
	float time = ( GetNextThink() - GetLastThink() );

	m_train->SetAbsVelocity( GetAbsVelocity() );
	m_train->SetAngularVelocity( GetAngularVelocity() );
	m_train->NextThink( m_train->GetLastThink() + time, false );

	// Attempt at getting the train to rotate properly around the origin of the trackchange
	if( time <= 0 )
		return;

	Vector offset = m_train->GetAbsOrigin() - GetAbsOrigin();
	Vector delta = dest - GetAbsAngles();
	// Transform offset into local coordinates
	UTIL_MakeInvVectors( delta, gpGlobals );
	Vector local;
	local.x = DotProduct( offset, gpGlobals->v_forward );
	local.y = DotProduct( offset, gpGlobals->v_right );
	local.z = DotProduct( offset, gpGlobals->v_up );

	local = local - offset;
	m_train->SetAbsVelocity( GetAbsVelocity() + ( local * ( 1.0 / time ) ) );
}

//
// Platform has hit bottom.  Stops and waits forever.
//
void CFuncTrackChange::HitBottom( void )
{
	CFuncPlatRot::HitBottom();
	if( m_code == TRAIN_FOLLOWING )
	{
		//		UpdateTrain();
		m_train->SetTrack( m_trackBottom );
	}
	SetThink( NULL );
	SetNextThink( -1 );

	UpdateAutoTargets( m_toggle_state );

	EnableUse();
}

//
// Platform has hit bottom.  Stops and waits forever.
//
void CFuncTrackChange::HitTop( void )
{
	CFuncPlatRot::HitTop();
	if( m_code == TRAIN_FOLLOWING )
	{
		//		UpdateTrain();
		m_train->SetTrack( m_trackTop );
	}

	// Don't let the plat go back down
	SetThink( NULL );
	SetNextThink( -1 );
	UpdateAutoTargets( m_toggle_state );
	EnableUse();
}

// UNDONE: Filter touches before re-evaluating the train.
void CFuncTrackChange::Touch( CBaseEntity *pOther )
{
#if 0
	TRAIN_CODE code;
#endif
}

// Normal track change
void CFuncTrackChange::UpdateAutoTargets( int toggleState )
{
	if( !m_trackTop || !m_trackBottom )
		return;

	if( toggleState == TS_AT_TOP )
		m_trackTop->GetSpawnFlags().ClearFlags( SF_PATH_DISABLED );
	else
		m_trackTop->GetSpawnFlags().AddFlags( SF_PATH_DISABLED );

	if( toggleState == TS_AT_BOTTOM )
		m_trackBottom->GetSpawnFlags().ClearFlags( SF_PATH_DISABLED );
	else
		m_trackBottom->GetSpawnFlags().AddFlags( SF_PATH_DISABLED );
}

void CFuncTrackChange::OverrideReset( void )
{
	SetNextThink( GetLastThink() + 1.0 );
	SetThink( &CFuncTrackChange::Find );
}