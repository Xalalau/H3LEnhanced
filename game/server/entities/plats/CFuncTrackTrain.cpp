#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "mathlib.h"

#include "CPathTrack.h"

#include "CFuncTrackTrain.h"

// ############ hu3lifezado ############ //
// [MODO COOP]
#include "gamerules/GameRules.h"
#include "gamerules/CHu3LifeCoop.h"
// ############ //

BEGIN_DATADESC( CFuncTrackTrain )
	DEFINE_FIELD( m_ppath, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_length, FIELD_FLOAT ),
	DEFINE_FIELD( m_height, FIELD_FLOAT ),
	DEFINE_FIELD( m_speed, FIELD_FLOAT ),
	DEFINE_FIELD( m_dir, FIELD_FLOAT ),
	DEFINE_FIELD( m_startSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_controlMins, FIELD_VECTOR ),
	DEFINE_FIELD( m_controlMaxs, FIELD_VECTOR ),
	DEFINE_FIELD( m_sounds, FIELD_INTEGER ),
	DEFINE_FIELD( m_flVolume, FIELD_FLOAT ),
	DEFINE_FIELD( m_flBank, FIELD_FLOAT ),
	DEFINE_FIELD( m_oldSpeed, FIELD_FLOAT ),

	DEFINE_THINKFUNC( Next ),
	DEFINE_THINKFUNC( Find ),
	DEFINE_THINKFUNC( NearestPath ),
	DEFINE_THINKFUNC( DeadEnd ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_tracktrain, CFuncTrackTrain );

void CFuncTrackTrain::Spawn( void )
{
	// ############ hu3lifezado ############ //
	// [MODO COOP]
	// Delay para evitar que o trem se mova antes do jogador entrar no server
	if (g_pGameRules->IsCoOp())
		hu3_spawn_delay = true;
	else
		hu3_spawn_delay = false;
	// ############ //

	if( GetSpeed() == 0 )
		m_speed = 100;
	else
		m_speed = GetSpeed();

	SetSpeed( 0 );
	SetAbsVelocity( g_vecZero );
	SetAngularVelocity( g_vecZero );
	SetImpulse( m_speed );

	m_dir = 1;

	if( !HasTarget() )
		ALERT( at_console, "FuncTrain with no target" );

	if( GetSpawnFlags().Any( SF_TRACKTRAIN_PASSABLE ) )
		SetSolidType( SOLID_NOT );
	else
		SetSolidType( SOLID_BSP );
	SetMoveType( MOVETYPE_PUSH );

	SetModel( GetModelName() );

	SetSize( GetRelMin(), GetRelMax() );

	SetAbsOrigin( GetAbsOrigin() );

	// Cache off placed origin for train controls
	SetOldOrigin( GetAbsOrigin() );

	m_controlMins = GetRelMin();
	m_controlMaxs = GetRelMax();
	m_controlMaxs.z += 72;
	// start trains on the next frame, to make sure their targets have had
	// a chance to spawn/activate
	NextThink( GetLastThink() + 0.1, false );
	SetThink( &CFuncTrackTrain::Find );
	Precache();
}

void CFuncTrackTrain::Precache( void )
{
	if( m_flVolume == 0.0 )
		m_flVolume = 1.0;

	switch( m_sounds )
	{
	default:
		// no sound
		pev->noise = 0;
		break;
	case 1: PRECACHE_SOUND( "plats/ttrain1.wav" ); pev->noise = MAKE_STRING( "plats/ttrain1.wav" ); break;
	case 2: PRECACHE_SOUND( "plats/ttrain2.wav" ); pev->noise = MAKE_STRING( "plats/ttrain2.wav" ); break;
	case 3: PRECACHE_SOUND( "plats/ttrain3.wav" ); pev->noise = MAKE_STRING( "plats/ttrain3.wav" ); break;
	case 4: PRECACHE_SOUND( "plats/ttrain4.wav" ); pev->noise = MAKE_STRING( "plats/ttrain4.wav" ); break;
	case 5: PRECACHE_SOUND( "plats/ttrain6.wav" ); pev->noise = MAKE_STRING( "plats/ttrain6.wav" ); break;
	case 6: PRECACHE_SOUND( "plats/ttrain7.wav" ); pev->noise = MAKE_STRING( "plats/ttrain7.wav" ); break;
	}

	PRECACHE_SOUND( "plats/ttrain_brake1.wav" );
	PRECACHE_SOUND( "plats/ttrain_start1.wav" );

	m_usAdjustPitch = PRECACHE_EVENT( 1, "events/train.sc" );
}

void CFuncTrackTrain::Blocked( CBaseEntity *pOther )
{
	// Blocker is on-ground on the train
	if( pOther->GetFlags().Any( FL_ONGROUND ) && pOther->GetGroundEntity() == this )
	{
		float deltaSpeed = fabs( GetSpeed() );
		if( deltaSpeed > 50 )
			deltaSpeed = 50;
		if( !pOther->GetAbsVelocity().z )
		{
			Vector vecVelocity = pOther->GetAbsVelocity();
			vecVelocity.z += deltaSpeed;
			pOther->SetAbsVelocity( vecVelocity );
		}
		return;
	}
	else
		pOther->SetAbsVelocity( ( pOther->GetAbsOrigin() - GetAbsOrigin() ).Normalize() * GetDamage() );

	ALERT( at_aiconsole, "TRAIN(%s): Blocked by %s (dmg:%.2f)\n", GetTargetname(), pOther->GetClassname(), GetDamage() );
	if( GetDamage() <= 0 )
		return;
	// we can't hurt this thing, so we're not concerned with it
	pOther->TakeDamage( this, this, GetDamage(), DMG_CRUSH );
}

void CFuncTrackTrain::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( useType != USE_SET )
	{
		if( !ShouldToggle( useType, ( GetSpeed() != 0 ) ) )
			return;

		if( GetSpeed() == 0 )
		{
			SetSpeed( m_speed * m_dir );

			Next();
		}
		else
		{
			SetSpeed( 0 );
			SetAbsVelocity( g_vecZero );
			SetAngularVelocity( g_vecZero );
			StopSound();
			SetThink( NULL );
		}
	}
	else
	{
		float delta = ( ( int ) ( GetSpeed() * 4 ) / ( int ) m_speed )*0.25 + 0.25 * value;
		if( delta > 1 )
			delta = 1;
		else if( delta < -1 )
			delta = -1;
		if( GetSpawnFlags().Any( SF_TRACKTRAIN_FORWARDONLY ) )
		{
			if( delta < 0 )
				delta = 0;
		}
		SetSpeed( m_speed * delta );
		Next();
		ALERT( at_aiconsole, "TRAIN(%s), speed to %.2f\n", GetTargetname(), GetSpeed() );
	}
}

void CFuncTrackTrain::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "wheels" ) )
	{
		m_length = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "height" ) )
	{
		m_height = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "startspeed" ) )
	{
		m_startSpeed = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "sounds" ) )
	{
		m_sounds = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "volume" ) )
	{
		m_flVolume = ( float ) ( atoi( pkvd->szValue ) );
		m_flVolume *= 0.1;
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "bank" ) )
	{
		m_flBank = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CFuncTrackTrain::Next( void )
{
	float time = 0.5;

	if( !GetSpeed() )
	{
		ALERT( at_aiconsole, "TRAIN(%s): Speed is 0\n", GetTargetname() );
		StopSound();
		return;
	}

	//	if ( !m_ppath )
	//		m_ppath = CPathTrack::Instance(UTIL_FindEntityByTargetname( nullptr, GetTarget() ));
	if( !m_ppath )
	{
		ALERT( at_aiconsole, "TRAIN(%s): Lost path\n", GetTargetname() );
		StopSound();
		return;
	}

	UpdateSound();

	Vector nextPos = GetAbsOrigin();

	nextPos.z -= m_height;
	CPathTrack *pnext = m_ppath->LookAhead( nextPos, GetSpeed() * 0.1, true );
	nextPos.z += m_height;

	SetAbsVelocity( ( nextPos - GetAbsOrigin() ) * 10 );
	Vector nextFront = GetAbsOrigin();

	nextFront.z -= m_height;
	if( m_length > 0 )
		m_ppath->LookAhead( nextFront, m_length, false );
	else
		m_ppath->LookAhead( nextFront, 100, false );
	nextFront.z += m_height;

	Vector delta = nextFront - GetAbsOrigin();
	Vector angles = UTIL_VecToAngles( delta );
	// The train actually points west
	angles.y += 180;

	// !!!  All of this crap has to be done to make the angles not wrap around, revisit this.
	FixupAngles( angles );
	Vector vecAngles = GetAbsAngles();
	FixupAngles( vecAngles );
	SetAbsAngles( vecAngles );

	if( !pnext || ( delta.x == 0 && delta.y == 0 ) )
		angles = GetAbsAngles();

	float vy, vx;
	if( !GetSpawnFlags().Any( SF_TRACKTRAIN_NOPITCH ) )
		vx = UTIL_AngleDistance( angles.x, GetAbsAngles().x );
	else
		vx = 0;
	vy = UTIL_AngleDistance( angles.y, GetAbsAngles().y );

	Vector vecAVelocity = GetAngularVelocity();
	vecAVelocity.y = vy * 10;
	vecAVelocity.x = vx * 10;

	if( m_flBank != 0 )
	{
		if( vecAVelocity.y < -5 )
			vecAVelocity.z = UTIL_AngleDistance( UTIL_ApproachAngle( -m_flBank, GetAbsAngles().z, m_flBank * 2 ), GetAbsAngles().z );
		else if( vecAVelocity.y > 5 )
			vecAVelocity.z = UTIL_AngleDistance( UTIL_ApproachAngle( m_flBank, GetAbsAngles().z, m_flBank * 2 ), GetAbsAngles().z );
		else
			vecAVelocity.z = UTIL_AngleDistance( UTIL_ApproachAngle( 0, GetAbsAngles().z, m_flBank * 4 ), GetAbsAngles().z ) * 4;
	}

	SetAngularVelocity( vecAVelocity );

	if( pnext )
	{
		if( pnext != m_ppath )
		{
			CPathTrack *pFire;
			if( GetSpeed() >= 0 )
				pFire = pnext;
			else
				pFire = m_ppath;

			m_ppath = pnext;
			// Fire the pass target if there is one
			if( pFire->HasMessage() )
			{
				FireTargets( pFire->GetMessage(), this, this, USE_TOGGLE, 0 );
				if( pFire->GetSpawnFlags().Any( SF_PATH_FIREONCE ) )
					pFire->ClearMessage();
			}

			if( pFire->GetSpawnFlags().Any( SF_PATH_DISABLE_TRAIN ) )
				GetSpawnFlags() |= SF_TRACKTRAIN_NOCONTROL;

			// Don't override speed if under user control
			if( GetSpawnFlags().Any( SF_TRACKTRAIN_NOCONTROL ) )
			{
				if( pFire->GetSpeed() != 0 )
				{// don't copy speed from target if it is 0 (uninitialized)
					SetSpeed( pFire->GetSpeed() );
					ALERT( at_aiconsole, "TrackTrain %s speed to %4.2f\n", GetTargetname(), GetSpeed() );
				}
			}

		}
		SetThink( &CFuncTrackTrain::Next );
		NextThink( GetLastThink() + time, true );
	}
	else	// end of path, stop
	{
		StopSound();
		SetAbsVelocity( nextPos - GetAbsOrigin() );
		SetAngularVelocity( g_vecZero );
		float distance = GetAbsVelocity().Length();
		m_oldSpeed = GetSpeed();


		SetSpeed( 0 );

		// Move to the dead end

		// Are we there yet?
		if( distance > 0 )
		{
			// no, how long to get there?
			time = distance / m_oldSpeed;
			SetAbsVelocity( GetAbsVelocity() * ( m_oldSpeed / distance ) );
			SetThink( &CFuncTrackTrain::DeadEnd );
			NextThink( GetLastThink() + time, false );
		}
		else
		{
			DeadEnd();
		}
	}
}

void CFuncTrackTrain::Find(void)
{
	// ############ hu3lifezado ############ //
	// [MODO COOP]
	// Correcao da posicao de spawn do trem
	
	char* hTarget = nullptr;

	// Escolher o target correto usando o comando de console (usar path_tracks, de preferencia)
	if (g_pGameRules->IsCoOp() && strcmp(GetTargetname(), "train") == 0)
	{
		hTarget = (char*)CVAR_GET_STRING("mp_hu3_trainspawnpoint");
		if (strcmp(hTarget, "0") == 0)
			hTarget = nullptr;
	}
	if (!hTarget)
		hTarget = (char*)GetTarget();

	// Instanciar
	m_ppath = CPathTrack::Instance(UTIL_FindEntityByTargetname(nullptr, hTarget));
	// ############ //

	if( !m_ppath )
		return;

	if( !m_ppath->ClassnameIs( "path_track" ) )
	{
		ALERT( at_error, "func_track_train must be on a path of path_track\n" );
		m_ppath = nullptr;
		return;
	}

	Vector nextPos = m_ppath->GetAbsOrigin();
	nextPos.z += m_height;

	Vector look = nextPos;
	look.z -= m_height;
	m_ppath->LookAhead( look, m_length, false );
	look.z += m_height;

	Vector vecAngles = UTIL_VecToAngles( look - nextPos );
	// The train actually points west
	vecAngles.y += 180;

	if( GetSpawnFlags().Any( SF_TRACKTRAIN_NOPITCH ) )
		vecAngles.x = 0;
	SetAbsAngles( vecAngles );

	SetAbsOrigin( nextPos );

	// ############ hu3lifezado ############ //
	// [MODO COOP]
	// Delay para evitar que o trem se mova antes do jogador entrar no server (ele roda a parte de cima do codigo uma vez extra desnecessariamente, mas tanto faz)
	if (hu3_spawn_delay)
	{
		NextThink(gpGlobals->time + 20, false);
		SetThink(&CFuncTrackTrain::Find);
		hu3_spawn_delay = false;
		return;
	}
	// ############ //

	NextThink( GetLastThink() + 0.1, false );
	SetThink( &CFuncTrackTrain::Next );
	SetSpeed( m_startSpeed );

	UpdateSound();
}

void CFuncTrackTrain::NearestPath( void )
{
	CBaseEntity *pTrack = NULL;
	CBaseEntity *pNearest = NULL;
	float dist, closest;

	closest = 1024;

	while( ( pTrack = UTIL_FindEntityInSphere( pTrack, GetAbsOrigin(), 1024 ) ) != NULL )
	{
		// filter out non-tracks
		if( !pTrack->GetFlags().Any( FL_CLIENT | FL_MONSTER ) && pTrack->ClassnameIs( "path_track" ) )
		{
			dist = ( GetAbsOrigin() - pTrack->GetAbsOrigin() ).Length();
			if( dist < closest )
			{
				closest = dist;
				pNearest = pTrack;
			}
		}
	}

	if( !pNearest )
	{
		ALERT( at_console, "Can't find a nearby track !!!\n" );
		SetThink( NULL );
		return;
	}

	ALERT( at_aiconsole, "TRAIN: %s, Nearest track is %s\n", GetTargetname(), pNearest->GetTargetname() );
	// If I'm closer to the next path_track on this path, then it's my real path
	pTrack = ( ( CPathTrack * ) pNearest )->GetNext();
	if( pTrack )
	{
		if( ( GetAbsOrigin() - pTrack->GetAbsOrigin() ).Length() < ( GetAbsOrigin() - pNearest->GetAbsOrigin() ).Length() )
			pNearest = pTrack;
	}

	m_ppath = ( CPathTrack * ) pNearest;

	if( GetSpeed() != 0 )
	{
		NextThink( GetLastThink() + 0.1, false );
		SetThink( &CFuncTrackTrain::Next );
	}
}

void CFuncTrackTrain::DeadEnd( void )
{
	// Fire the dead-end target if there is one
	CPathTrack *pTrack, *pNext;

	pTrack = m_ppath;

	ALERT( at_aiconsole, "TRAIN(%s): Dead end ", GetTargetname() );
	// Find the dead end path node
	// HACKHACK -- This is bugly, but the train can actually stop moving at a different node depending on it's speed
	// so we have to traverse the list to it's end.
	if( pTrack )
	{
		if( m_oldSpeed < 0 )
		{
			do
			{
				pNext = pTrack->ValidPath( pTrack->GetPrevious(), true );
				if( pNext )
					pTrack = pNext;
			}
			while( pNext );
		}
		else
		{
			do
			{
				pNext = pTrack->ValidPath( pTrack->GetNext(), true );
				if( pNext )
					pTrack = pNext;
			}
			while( pNext );
		}
	}

	SetAbsVelocity( g_vecZero );
	SetAngularVelocity( g_vecZero );
	if( pTrack )
	{
		ALERT( at_aiconsole, "at %s\n", pTrack->GetTargetname() );
		if( pTrack->HasNetName() )
			FireTargets( pTrack->GetNetName(), this, this, USE_TOGGLE, 0 );
	}
	else
		ALERT( at_aiconsole, "\n" );
}

void CFuncTrackTrain::NextThink( float thinkTime, const bool alwaysThink )
{
	if( alwaysThink )
		GetFlags() |= FL_ALWAYSTHINK;
	else
		GetFlags().ClearFlags( FL_ALWAYSTHINK );

	SetNextThink( thinkTime );
}

void CFuncTrackTrain::SetTrack( CPathTrack *track )
{
	m_ppath = track->Nearest( GetAbsOrigin() );
}

void CFuncTrackTrain::SetControls( CBaseEntity* pControls )
{
	Vector offset = pControls->GetAbsOrigin() - GetOldOrigin();

	m_controlMins = pControls->GetRelMin() + offset;
	m_controlMaxs = pControls->GetRelMax() + offset;
}

bool CFuncTrackTrain::OnControls( const CBaseEntity* const pTest ) const
{
	const Vector offset = pTest->GetAbsOrigin() - GetAbsOrigin();

	if( GetSpawnFlags().Any( SF_TRACKTRAIN_NOCONTROL ) )
		return false;

	// Transform offset into local coordinates
	UTIL_MakeVectors( GetAbsAngles() );
	Vector local;
	local.x = DotProduct( offset, gpGlobals->v_forward );
	local.y = -DotProduct( offset, gpGlobals->v_right );
	local.z = DotProduct( offset, gpGlobals->v_up );

	if( local.x >= m_controlMins.x && local.y >= m_controlMins.y && local.z >= m_controlMins.z &&
		local.x <= m_controlMaxs.x && local.y <= m_controlMaxs.y && local.z <= m_controlMaxs.z )
		return true;

	return false;
}

void CFuncTrackTrain::StopSound( void )
{
	// if sound playing, stop it
	if( m_soundPlaying && pev->noise )
	{
		unsigned short us_encode;
		unsigned short us_sound = ( ( unsigned short ) ( m_sounds ) & 0x0007 ) << 12;

		us_encode = us_sound;

		PLAYBACK_EVENT_FULL( FEV_RELIABLE | FEV_UPDATE, edict(), m_usAdjustPitch, 0.0,
			g_vecZero, g_vecZero, 0.0, 0.0, us_encode, 0, 1, 0 );

		/*
		STOP_SOUND( this, CHAN_STATIC, (char*)STRING( pev->noise ) );
		*/
		EMIT_SOUND_DYN( this, CHAN_ITEM, "plats/ttrain_brake1.wav", m_flVolume, ATTN_NORM, 0, 100 );
	}

	m_soundPlaying = 0;
}

// update pitch based on speed, start sound if not playing
// NOTE: when train goes through transition, m_soundPlaying should go to 0, 
// which will cause the looped sound to restart.

void CFuncTrackTrain::UpdateSound( void )
{
	float flpitch;

	if( !pev->noise )
		return;

	flpitch = TRAIN_STARTPITCH + ( fabs( GetSpeed() ) * ( TRAIN_MAXPITCH - TRAIN_STARTPITCH ) / TRAIN_MAXSPEED );

	if( !m_soundPlaying )
	{
		// play startup sound for train
		EMIT_SOUND_DYN( this, CHAN_ITEM, "plats/ttrain_start1.wav", m_flVolume, ATTN_NORM, 0, 100 );
		EMIT_SOUND_DYN( this, CHAN_STATIC, ( char* ) STRING( pev->noise ), m_flVolume, ATTN_NORM, 0, ( int ) flpitch );
		m_soundPlaying = 1;
	}
	else
	{
		/*
		// update pitch
		EMIT_SOUND_DYN( this, CHAN_STATIC, (char*)STRING(pev->noise), m_flVolume, ATTN_NORM, SND_CHANGE_PITCH, (int) flpitch);
		*/
		// volume 0.0 - 1.0 - 6 bits
		// m_sounds 3 bits
		// flpitch = 6 bits
		// 15 bits total

		unsigned short us_encode;
		unsigned short us_sound = ( ( unsigned short ) ( m_sounds ) & 0x0007 ) << 12;
		unsigned short us_pitch = ( ( unsigned short ) ( flpitch / 10.0 ) & 0x003f ) << 6;
		unsigned short us_volume = ( ( unsigned short ) ( m_flVolume * 40.0 ) & 0x003f );

		us_encode = us_sound | us_pitch | us_volume;

		PLAYBACK_EVENT_FULL( FEV_RELIABLE | FEV_UPDATE, edict(), m_usAdjustPitch, 0.0,
			g_vecZero, g_vecZero, 0.0, 0.0, us_encode, 0, 0, 0 );
	}
}

CFuncTrackTrain* CFuncTrackTrain::Instance( CBaseEntity* pEntity )
{
	if( pEntity && pEntity->ClassnameIs( "func_tracktrain" ) )
		return static_cast<CFuncTrackTrain*>( pEntity );
	return nullptr;
}

void CFuncTrackTrain::OverrideReset( void )
{
	NextThink( GetLastThink() + 0.1, false );
	SetThink( &CFuncTrackTrain::NearestPath );
}