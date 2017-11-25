#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Effects.h"
#include "CBasePlayer.h"
#include "Weapons.h"

#include "CFuncTank.h"

BEGIN_DATADESC( CFuncTank )
	DEFINE_FIELD( m_yawCenter, FIELD_FLOAT ),
	DEFINE_FIELD( m_yawRate, FIELD_FLOAT ),
	DEFINE_FIELD( m_yawRange, FIELD_FLOAT ),
	DEFINE_FIELD( m_yawTolerance, FIELD_FLOAT ),
	DEFINE_FIELD( m_pitchCenter, FIELD_FLOAT ),
	DEFINE_FIELD( m_pitchRate, FIELD_FLOAT ),
	DEFINE_FIELD( m_pitchRange, FIELD_FLOAT ),
	DEFINE_FIELD( m_pitchTolerance, FIELD_FLOAT ),
	DEFINE_FIELD( m_fireLast, FIELD_TIME ),
	DEFINE_FIELD( m_fireRate, FIELD_FLOAT ),
	DEFINE_FIELD( m_lastSightTime, FIELD_TIME ),
	DEFINE_FIELD( m_persist, FIELD_FLOAT ),
	DEFINE_FIELD( m_minRange, FIELD_FLOAT ),
	DEFINE_FIELD( m_maxRange, FIELD_FLOAT ),
	DEFINE_FIELD( m_barrelPos, FIELD_VECTOR ),
	DEFINE_FIELD( m_spriteScale, FIELD_FLOAT ),
	DEFINE_FIELD( m_iszSpriteSmoke, FIELD_STRING ),
	DEFINE_FIELD( m_iszSpriteFlash, FIELD_STRING ),
	DEFINE_FIELD( m_bulletType, FIELD_INTEGER ),
	DEFINE_FIELD( m_sightOrigin, FIELD_VECTOR ),
	DEFINE_FIELD( m_spread, FIELD_INTEGER ),
	DEFINE_FIELD( m_pController, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_vecControllerUsePos, FIELD_VECTOR ),
	DEFINE_FIELD( m_flNextAttack, FIELD_TIME ),
	DEFINE_FIELD( m_iBulletDamage, FIELD_INTEGER ),
	DEFINE_FIELD( m_iszMaster, FIELD_STRING ),
END_DATADESC()

const Vector gTankSpread[] =
{
	Vector( 0, 0, 0 ),		// perfect
	Vector( 0.025, 0.025, 0.025 ),	// small cone
	Vector( 0.05, 0.05, 0.05 ),  // medium cone
	Vector( 0.1, 0.1, 0.1 ),	// large cone
	Vector( 0.25, 0.25, 0.25 ),	// extra-large cone
};

#define MAX_FIRING_SPREADS ARRAYSIZE(gTankSpread)

void CFuncTank::Spawn( void )
{
	Precache();

	SetMoveType( MOVETYPE_PUSH );  // so it doesn't get pushed by anything
	SetSolidType( SOLID_BSP );
	SetModel( GetModelName() );

	m_yawCenter = GetAbsAngles().y;
	m_pitchCenter = GetAbsAngles().x;

	if( IsActive() )
		SetNextThink( GetLastThink() + 1.0 );

	m_sightOrigin = BarrelPosition(); // Point at the end of the barrel

	if( m_fireRate <= 0 )
		m_fireRate = 1;
	//The old version used signed ints. This allowed negative indices. Negative values become larger values, so the problem fixes itself. - Solokiller
	if( static_cast<size_t>( m_spread ) > MAX_FIRING_SPREADS )
		m_spread = 0;

	SetOldOrigin( GetAbsOrigin() );
}

void CFuncTank::Precache( void )
{
	if( m_iszSpriteSmoke )
		PRECACHE_MODEL( ( char * ) STRING( m_iszSpriteSmoke ) );
	if( m_iszSpriteFlash )
		PRECACHE_MODEL( ( char * ) STRING( m_iszSpriteFlash ) );

	if( pev->noise )
		PRECACHE_SOUND( ( char * ) STRING( pev->noise ) );
}

void CFuncTank::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "yawrate" ) )
	{
		m_yawRate = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "yawrange" ) )
	{
		m_yawRange = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "yawtolerance" ) )
	{
		m_yawTolerance = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "pitchrange" ) )
	{
		m_pitchRange = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "pitchrate" ) )
	{
		m_pitchRate = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "pitchtolerance" ) )
	{
		m_pitchTolerance = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "firerate" ) )
	{
		m_fireRate = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "barrel" ) )
	{
		m_barrelPos.x = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "barrely" ) )
	{
		m_barrelPos.y = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "barrelz" ) )
	{
		m_barrelPos.z = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "spritescale" ) )
	{
		m_spriteScale = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "spritesmoke" ) )
	{
		m_iszSpriteSmoke = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "spriteflash" ) )
	{
		m_iszSpriteFlash = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "rotatesound" ) )
	{
		pev->noise = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "persistence" ) )
	{
		m_persist = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "bullet" ) )
	{
		m_bulletType = ( TANKBULLET ) atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "bullet_damage" ) )
	{
		m_iBulletDamage = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "firespread" ) )
	{
		m_spread = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "minRange" ) )
	{
		m_minRange = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "maxRange" ) )
	{
		m_maxRange = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "master" ) )
	{
		m_iszMaster = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CFuncTank::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( GetSpawnFlags().Any( SF_TANK_CANCONTROL ) )
	{  // player controlled turret

		if( pActivator->Classify() != EntityClassifications().GetClassificationId( classify::PLAYER ) )
			return;

		if( value == 2 && useType == USE_SET )
		{
			ControllerPostFrame();
		}
		else if( !m_pController && useType != USE_OFF )
		{
			( ( CBasePlayer* ) pActivator )->m_pTank = this;
			StartControl( ( CBasePlayer* ) pActivator );
		}
		else
		{
			StopControl();
		}
	}
	else
	{
		if( !ShouldToggle( useType, IsActive() ) )
			return;

		if( IsActive() )
			TankDeactivate();
		else
			TankActivate();
	}
}

void CFuncTank::Think( void )
{
	SetAngularVelocity( g_vecZero );
	TrackTarget();

	if( fabs( GetAngularVelocity().x ) > 1 || fabs( GetAngularVelocity().y ) > 1 )
		StartRotSound();
	else
		StopRotSound();
}

void CFuncTank::TrackTarget( void )
{
	TraceResult tr;
	CBaseEntity* pPlayer = UTIL_FindClientInPVS( this );
	bool updateTime = false;
	Vector angles, direction, targetPosition, barrelEnd;

	CBaseEntity* pTarget = nullptr;

	// Get a position to aim for
	if( m_pController )
	{
		// Tanks attempt to mirror the player's angles
		angles = m_pController->GetViewAngle();
		angles[ 0 ] = 0 - angles[ 0 ];
		SetNextThink( GetLastThink() + 0.05 );
	}
	else
	{
		if( IsActive() )
			SetNextThink( GetLastThink() + 0.1 );
		else
			return;

		if( FNullEnt( pPlayer ) )
		{
			if( IsActive() )
				SetNextThink( GetLastThink() + 2 );	// Wait 2 secs
			return;
		}

		pTarget = FindTarget( pPlayer );

		if( !pTarget )
			return;

		// Calculate angle needed to aim at target
		barrelEnd = BarrelPosition();
		targetPosition = pTarget->GetAbsOrigin() + pTarget->GetViewOffset();
		float range = ( targetPosition - barrelEnd ).Length();

		if( !InRange( range ) )
			return;

		UTIL_TraceLine( barrelEnd, targetPosition, dont_ignore_monsters, edict(), &tr );

		//bool lineOfSight = false;
		// No line of sight, don't track
		if( tr.flFraction == 1.0 || tr.pHit == pTarget->edict() )
		{
			//lineOfSight = true;

			if( InRange( range ) && pTarget && pTarget->IsAlive() )
			{
				updateTime = true;
				m_sightOrigin = UpdateTargetPosition( pTarget );
			}
		}

		// Track sight origin

		// !!! I'm not sure what i changed
		direction = m_sightOrigin - GetAbsOrigin();
		//		direction = m_sightOrigin - barrelEnd;
		angles = UTIL_VecToAngles( direction );

		// Calculate the additional rotation to point the end of the barrel at the target (not the gun's center) 
		AdjustAnglesForBarrel( angles, direction.Length() );
	}

	angles.x = -angles.x;

	// Force the angles to be relative to the center position
	angles.y = m_yawCenter + UTIL_AngleDistance( angles.y, m_yawCenter );
	angles.x = m_pitchCenter + UTIL_AngleDistance( angles.x, m_pitchCenter );

	// Limit against range in y
	if( angles.y > m_yawCenter + m_yawRange )
	{
		angles.y = m_yawCenter + m_yawRange;
		updateTime = false;	// Don't update if you saw the player, but out of range
	}
	else if( angles.y < ( m_yawCenter - m_yawRange ) )
	{
		angles.y = ( m_yawCenter - m_yawRange );
		updateTime = false; // Don't update if you saw the player, but out of range
	}

	if( updateTime )
		m_lastSightTime = gpGlobals->time;

	// Move toward target at rate or less
	float distY = UTIL_AngleDistance( angles.y, GetAbsAngles().y );

	Vector vecAVelocity = GetAngularVelocity();

	vecAVelocity.y = distY * 10;
	if( vecAVelocity.y > m_yawRate )
		vecAVelocity.y = m_yawRate;
	else if( vecAVelocity.y < -m_yawRate )
		vecAVelocity.y = -m_yawRate;

	// Limit against range in x
	if( angles.x > m_pitchCenter + m_pitchRange )
		angles.x = m_pitchCenter + m_pitchRange;
	else if( angles.x < m_pitchCenter - m_pitchRange )
		angles.x = m_pitchCenter - m_pitchRange;

	// Move toward target at rate or less
	float distX = UTIL_AngleDistance( angles.x, GetAbsAngles().x );
	vecAVelocity.x = distX * 10;

	if( vecAVelocity.x > m_pitchRate )
		vecAVelocity.x = m_pitchRate;
	else if( vecAVelocity.x < -m_pitchRate )
		vecAVelocity.x = -m_pitchRate;

	SetAngularVelocity( vecAVelocity );

	if( m_pController )
		return;

	if( CanFire() && ( ( fabs( distX ) < m_pitchTolerance && fabs( distY ) < m_yawTolerance ) || GetSpawnFlags().Any( SF_TANK_LINEOFSIGHT ) ) )
	{
		bool fire = false;
		Vector forward;
		UTIL_MakeVectorsPrivate( GetAbsAngles(), &forward, nullptr, nullptr );

		if( GetSpawnFlags().Any( SF_TANK_LINEOFSIGHT ) )
		{
			float length = direction.Length();
			UTIL_TraceLine( barrelEnd, barrelEnd + forward * length, dont_ignore_monsters, edict(), &tr );
			if( tr.pHit == pTarget->edict() )
				fire = true;
		}
		else
			fire = true;

		if( fire )
		{
			Fire( BarrelPosition(), forward, this );
		}
		else
			m_fireLast = 0;
	}
	else
		m_fireLast = 0;
}

// Fire targets and spawn sprites
void CFuncTank::Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity* pAttacker )
{
	if( m_fireLast != 0 )
	{
		if( m_iszSpriteSmoke )
		{
			CSprite *pSprite = CSprite::SpriteCreate( STRING( m_iszSpriteSmoke ), barrelEnd, true );
			pSprite->AnimateAndDie( RANDOM_FLOAT( 15.0, 20.0 ) );
			pSprite->SetTransparency( kRenderTransAlpha, GetRenderColor().x, GetRenderColor().y, GetRenderColor().z, 255, kRenderFxNone );
			Vector vecVelocity = pSprite->GetAbsVelocity();
			vecVelocity.z = RANDOM_FLOAT( 40, 80 );
			pSprite->SetAbsVelocity( vecVelocity );
			pSprite->SetScale( m_spriteScale );
		}
		if( m_iszSpriteFlash )
		{
			CSprite *pSprite = CSprite::SpriteCreate( STRING( m_iszSpriteFlash ), barrelEnd, true );
			pSprite->AnimateAndDie( 60 );
			pSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 255, kRenderFxNoDissipation );
			pSprite->SetScale( m_spriteScale );

			// Hack Hack, make it stick around for at least 100 ms.
			pSprite->SetNextThink( GetNextThink() + 0.1 );
		}
		SUB_UseTargets( this, USE_TOGGLE, 0 );
	}
	m_fireLast = gpGlobals->time;
}

void CFuncTank::StartRotSound( void )
{
	if( !pev->noise || GetSpawnFlags().Any( SF_TANK_SOUNDON ) )
		return;
	GetSpawnFlags() |= SF_TANK_SOUNDON;
	EMIT_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noise ), 0.85, ATTN_NORM );
}

void CFuncTank::StopRotSound( void )
{
	if( GetSpawnFlags().Any( SF_TANK_SOUNDON ) )
		STOP_SOUND( this, CHAN_STATIC, ( char* ) STRING( pev->noise ) );
	GetSpawnFlags().ClearFlags( SF_TANK_SOUNDON );
}

bool CFuncTank::InRange( float range ) const
{
	if( range < m_minRange )
		return false;
	if( m_maxRange > 0 && range > m_maxRange )
		return false;

	return true;
}

CBaseEntity* CFuncTank::FindTarget( CBaseEntity* pPlayer )
{
	return pPlayer;
}

void CFuncTank::TankTrace( const Vector &vecStart, const Vector &vecForward, const Vector &vecSpread, TraceResult &tr )
{
	// get circular gaussian spread
	float x, y;
	UTIL_GetCircularGaussianSpread( x, y );

	Vector vecDir = vecForward +
		x * vecSpread.x * gpGlobals->v_right +
		y * vecSpread.y * gpGlobals->v_up;
	Vector vecEnd;

	vecEnd = vecStart + vecDir * 4096;
	UTIL_TraceLine( vecStart, vecEnd, dont_ignore_monsters, edict(), &tr );
}

// If barrel is offset, add in additional rotation
void CFuncTank::AdjustAnglesForBarrel( Vector &angles, float distance )
{
	float r2, d2;


	if( m_barrelPos.y != 0 || m_barrelPos.z != 0 )
	{
		distance -= m_barrelPos.z;
		d2 = distance * distance;
		if( m_barrelPos.y )
		{
			r2 = m_barrelPos.y * m_barrelPos.y;
			angles.y += ( 180.0 / M_PI ) * atan2( m_barrelPos.y, sqrt( d2 - r2 ) );
		}
		if( m_barrelPos.z )
		{
			r2 = m_barrelPos.z * m_barrelPos.z;
			angles.x += ( 180.0 / M_PI ) * atan2( -m_barrelPos.z, sqrt( d2 - r2 ) );
		}
	}
}

////////////// START NEW STUFF //////////////

//==================================================================================
// TANK CONTROLLING
bool CFuncTank::OnControls( const CBaseEntity* const pTest ) const
{
	if( !GetSpawnFlags().Any( SF_TANK_CANCONTROL ) )
		return false;

	if( ( m_vecControllerUsePos - pTest->GetAbsOrigin() ).Length() < 30 )
		return true;

	return false;
}

bool CFuncTank::StartControl( CBasePlayer *pController )
{
	if( m_pController != NULL )
		return false;

	// Team only or disabled?
	if( m_iszMaster )
	{
		if( !UTIL_IsMasterTriggered( m_iszMaster, pController ) )
			return false;
	}

	ALERT( at_console, "using TANK!\n" );

	m_pController = pController;
	if( m_pController->m_pActiveItem )
	{
		m_pController->m_pActiveItem->Holster();
		m_pController->ClearWeaponModelName();
		m_pController->ClearViewModelName();

	}

	m_pController->m_iHideHUD |= HIDEHUD_WEAPONS;
	m_vecControllerUsePos = m_pController->GetAbsOrigin();

	SetNextThink( GetLastThink() + 0.1 );

	return true;
}

void CFuncTank::StopControl()
{
	// TODO: bring back the controllers current weapon
	if( !m_pController )
		return;

	if( m_pController->m_pActiveItem )
		m_pController->m_pActiveItem->Deploy();

	ALERT( at_console, "stopped using TANK\n" );

	m_pController->m_iHideHUD &= ~HIDEHUD_WEAPONS;

	SetNextThink( 0 );
	m_pController = NULL;

	if( IsActive() )
		SetNextThink( GetLastThink() + 1.0 );
}

// Called each frame by the player's ItemPostFrame
void CFuncTank::ControllerPostFrame( void )
{
	ASSERT( m_pController != NULL );

	if( gpGlobals->time < m_flNextAttack )
		return;

	if( m_pController->GetButtons().Any( IN_ATTACK ) )
	{
		Vector vecForward;
		UTIL_MakeVectorsPrivate( GetAbsAngles(), &vecForward, nullptr, nullptr );

		m_fireLast = gpGlobals->time - ( 1 / m_fireRate ) - 0.01;  // to make sure the gun doesn't fire too many bullets

		Fire( BarrelPosition(), vecForward, m_pController );

		// HACKHACK -- make some noise (that the AI can hear)
		if( m_pController && m_pController->IsPlayer() )
			( ( CBasePlayer * ) m_pController )->m_iWeaponVolume = LOUD_GUN_VOLUME;

		m_flNextAttack = gpGlobals->time + ( 1 / m_fireRate );
	}
}
////////////// END NEW STUFF //////////////