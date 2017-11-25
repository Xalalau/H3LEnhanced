#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "customentity.h"

#include "CBeam.h"

#include "CLightning.h"

BEGIN_DATADESC( CLightning )
	DEFINE_FIELD( m_active, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_iszStartEntity, FIELD_STRING ),
	DEFINE_FIELD( m_iszEndEntity, FIELD_STRING ),
	DEFINE_FIELD( m_life, FIELD_FLOAT ),
	DEFINE_FIELD( m_boltWidth, FIELD_INTEGER ),
	DEFINE_FIELD( m_noiseAmplitude, FIELD_INTEGER ),
	DEFINE_FIELD( m_brightness, FIELD_INTEGER ),
	DEFINE_FIELD( m_speed, FIELD_INTEGER ),
	DEFINE_FIELD( m_restrike, FIELD_FLOAT ),
	DEFINE_FIELD( m_spriteTexture, FIELD_INTEGER ),
	DEFINE_FIELD( m_iszSpriteName, FIELD_STRING ),
	DEFINE_FIELD( m_frameStart, FIELD_INTEGER ),
	DEFINE_FIELD( m_radius, FIELD_FLOAT ),
	DEFINE_THINKFUNC( StrikeThink ),
	DEFINE_THINKFUNC( DamageThink ),
	DEFINE_USEFUNC( StrikeUse ),
	DEFINE_USEFUNC( ToggleUse ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( env_lightning, CLightning );
LINK_ENTITY_TO_CLASS( env_beam, CLightning );

void CLightning::Spawn( void )
{
	if( FStringNull( m_iszSpriteName ) )
	{
		SetThink( &CLightning::SUB_Remove );
		return;
	}
	SetSolidType( SOLID_NOT );							// Remove model & collisions
	Precache();

	SetDamageTime( gpGlobals->time );

	if( ServerSide() )
	{
		SetThink( NULL );
		if( GetDamage() > 0 )
		{
			SetThink( &CLightning::DamageThink );
			SetNextThink( gpGlobals->time + 0.1 );
		}
		if( HasTargetname() )
		{
			if( !GetSpawnFlags().Any( SF_BEAM_STARTON ) )
			{
				GetEffects() = EF_NODRAW;
				m_active = false;
				SetNextThink( 0 );
			}
			else
				m_active = true;

			SetUse( &CLightning::ToggleUse );
		}
	}
	else
	{
		m_active = false;
		if( HasTargetname() )
		{
			SetUse( &CLightning::StrikeUse );
		}
		if( !HasTargetname() || GetSpawnFlags().Any( SF_BEAM_STARTON ) )
		{
			SetThink( &CLightning::StrikeThink );
			SetNextThink( gpGlobals->time + 1.0 );
		}
	}
}

void CLightning::Precache( void )
{
	m_spriteTexture = PRECACHE_MODEL( ( char * ) STRING( m_iszSpriteName ) );
	CBeam::Precache();
}

void CLightning::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "LightningStart" ) )
	{
		m_iszStartEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "LightningEnd" ) )
	{
		m_iszEndEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "life" ) )
	{
		m_life = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "BoltWidth" ) )
	{
		m_boltWidth = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "NoiseAmplitude" ) )
	{
		m_noiseAmplitude = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "TextureScroll" ) )
	{
		m_speed = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "StrikeTime" ) )
	{
		m_restrike = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "texture" ) )
	{
		m_iszSpriteName = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "framestart" ) )
	{
		m_frameStart = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "Radius" ) )
	{
		m_radius = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "damage" ) )
	{
		SetDamage( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CBeam::KeyValue( pkvd );
}

void CLightning::Activate( void )
{
	if( ServerSide() )
		BeamUpdateVars();
}

void CLightning::StrikeThink( void )
{
	if( m_life != 0 )
	{
		if( GetSpawnFlags().Any( SF_BEAM_RANDOM ) )
			SetNextThink( gpGlobals->time + m_life + RANDOM_FLOAT( 0, m_restrike ) );
		else
			SetNextThink( gpGlobals->time + m_life + m_restrike );
	}
	m_active = true;

	if( FStringNull( m_iszEndEntity ) )
	{
		if( FStringNull( m_iszStartEntity ) )
		{
			RandomArea();
		}
		else
		{
			CBaseEntity *pStart = UTIL_RandomTargetname( STRING( m_iszStartEntity ) );
			if( pStart != NULL )
				RandomPoint( pStart->GetAbsOrigin() );
			else
				ALERT( at_console, "env_beam: unknown entity \"%s\"\n", STRING( m_iszStartEntity ) );
		}
		return;
	}

	CBaseEntity *pStart = UTIL_RandomTargetname( STRING( m_iszStartEntity ) );
	CBaseEntity *pEnd = UTIL_RandomTargetname( STRING( m_iszEndEntity ) );

	if( pStart != NULL && pEnd != NULL )
	{
		if( UTIL_IsPointEntity( pStart ) || UTIL_IsPointEntity( pEnd ) )
		{
			if( GetSpawnFlags().Any( SF_BEAM_RING ) )
			{
				// don't work
				return;
			}
		}

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		if( UTIL_IsPointEntity( pStart ) || UTIL_IsPointEntity( pEnd ) )
		{
			if( !UTIL_IsPointEntity( pEnd ) )	// One point entity must be in pEnd
			{
				CBaseEntity *pTemp;
				pTemp = pStart;
				pStart = pEnd;
				pEnd = pTemp;
			}
			if( !UTIL_IsPointEntity( pStart ) )	// One sided
			{
				WRITE_BYTE( TE_BEAMENTPOINT );
				WRITE_SHORT( pStart->entindex() );
				WRITE_COORD( pEnd->GetAbsOrigin().x );
				WRITE_COORD( pEnd->GetAbsOrigin().y );
				WRITE_COORD( pEnd->GetAbsOrigin().z );
			}
			else
			{
				WRITE_BYTE( TE_BEAMPOINTS );
				WRITE_COORD( pStart->GetAbsOrigin().x );
				WRITE_COORD( pStart->GetAbsOrigin().y );
				WRITE_COORD( pStart->GetAbsOrigin().z );
				WRITE_COORD( pEnd->GetAbsOrigin().x );
				WRITE_COORD( pEnd->GetAbsOrigin().y );
				WRITE_COORD( pEnd->GetAbsOrigin().z );
			}


		}
		else
		{
			if( GetSpawnFlags().Any( SF_BEAM_RING ) )
				WRITE_BYTE( TE_BEAMRING );
			else
				WRITE_BYTE( TE_BEAMENTS );
			WRITE_SHORT( pStart->entindex() );
			WRITE_SHORT( pEnd->entindex() );
		}

			WRITE_SHORT( m_spriteTexture );
			WRITE_BYTE( m_frameStart ); // framestart
			WRITE_BYTE( ( int ) GetFrameRate() ); // framerate
			WRITE_BYTE( ( int ) ( m_life*10.0 ) ); // life
			WRITE_BYTE( m_boltWidth );  // width
			WRITE_BYTE( m_noiseAmplitude );   // noise
			WRITE_BYTE( ( int ) GetRenderColor().x );   // r, g, b
			WRITE_BYTE( ( int ) GetRenderColor().y );   // r, g, b
			WRITE_BYTE( ( int ) GetRenderColor().z );   // r, g, b
			WRITE_BYTE( GetRenderAmount() );	// brightness
			WRITE_BYTE( m_speed );		// speed
		MESSAGE_END();
		DoSparks( pStart->GetAbsOrigin(), pEnd->GetAbsOrigin() );
		if( GetDamage() > 0 )
		{
			TraceResult tr;
			UTIL_TraceLine( pStart->GetAbsOrigin(), pEnd->GetAbsOrigin(), dont_ignore_monsters, NULL, &tr );
			BeamDamageInstant( &tr, GetDamage() );
		}
	}
}

void CLightning::DamageThink( void )
{
	SetNextThink( gpGlobals->time + 0.1 );
	TraceResult tr;
	UTIL_TraceLine( GetStartPos(), GetEndPos(), dont_ignore_monsters, NULL, &tr );
	BeamDamage( &tr );
}

void CLightning::RandomArea( void )
{
	int iLoops = 0;

	for( iLoops = 0; iLoops < 10; iLoops++ )
	{
		Vector vecSrc = GetAbsOrigin();

		Vector vecDir1 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir1 = vecDir1.Normalize();
		TraceResult		tr1;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir1 * m_radius, ignore_monsters, ENT( pev ), &tr1 );

		if( tr1.flFraction == 1.0 )
			continue;

		Vector vecDir2;
		do {
			vecDir2 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ) );
		}
		while( DotProduct( vecDir1, vecDir2 ) > 0 );
		vecDir2 = vecDir2.Normalize();
		TraceResult		tr2;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir2 * m_radius, ignore_monsters, ENT( pev ), &tr2 );

		if( tr2.flFraction == 1.0 )
			continue;

		if( ( tr1.vecEndPos - tr2.vecEndPos ).Length() < m_radius * 0.1 )
			continue;

		UTIL_TraceLine( tr1.vecEndPos, tr2.vecEndPos, ignore_monsters, ENT( pev ), &tr2 );

		if( tr2.flFraction != 1.0 )
			continue;

		Zap( tr1.vecEndPos, tr2.vecEndPos );

		break;
	}
}

void CLightning::RandomPoint( const Vector &vecSrc )
{
	int iLoops = 0;

	for( iLoops = 0; iLoops < 10; iLoops++ )
	{
		Vector vecDir1 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir1 = vecDir1.Normalize();
		TraceResult		tr1;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir1 * m_radius, ignore_monsters, ENT( pev ), &tr1 );

		if( ( tr1.vecEndPos - vecSrc ).Length() < m_radius * 0.1 )
			continue;

		if( tr1.flFraction == 1.0 )
			continue;

		Zap( vecSrc, tr1.vecEndPos );
		break;
	}
}

void CLightning::Zap( const Vector &vecSrc, const Vector &vecDest )
{
#if 1
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD( vecSrc.x );
		WRITE_COORD( vecSrc.y );
		WRITE_COORD( vecSrc.z );
		WRITE_COORD( vecDest.x );
		WRITE_COORD( vecDest.y );
		WRITE_COORD( vecDest.z );
		WRITE_SHORT( m_spriteTexture );
		WRITE_BYTE( m_frameStart ); // framestart
		WRITE_BYTE( ( int ) GetFrameRate() ); // framerate
		WRITE_BYTE( ( int ) ( m_life*10.0 ) ); // life
		WRITE_BYTE( m_boltWidth );  // width
		WRITE_BYTE( m_noiseAmplitude );   // noise
		WRITE_BYTE( ( int ) GetRenderColor().x );   // r, g, b
		WRITE_BYTE( ( int ) GetRenderColor().y );   // r, g, b
		WRITE_BYTE( ( int ) GetRenderColor().z );   // r, g, b
		WRITE_BYTE( GetRenderAmount() );	// brightness
		WRITE_BYTE( m_speed );		// speed
	MESSAGE_END();
#else
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_LIGHTNING );
		WRITE_COORD( vecSrc.x );
		WRITE_COORD( vecSrc.y );
		WRITE_COORD( vecSrc.z );
		WRITE_COORD( vecDest.x );
		WRITE_COORD( vecDest.y );
		WRITE_COORD( vecDest.z );
		WRITE_BYTE( 10 );
		WRITE_BYTE( 50 );
		WRITE_BYTE( 40 );
		WRITE_SHORT( m_spriteTexture );
	MESSAGE_END();
#endif
	DoSparks( vecSrc, vecDest );
}

void CLightning::StrikeUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( !ShouldToggle( useType, m_active ) )
		return;

	if( m_active )
	{
		m_active = false;
		SetThink( NULL );
	}
	else
	{
		SetThink( &CLightning::StrikeThink );
		SetNextThink( gpGlobals->time + 0.1 );
	}

	if( !GetSpawnFlags().Any( SF_BEAM_TOGGLE ) )
		SetUse( NULL );
}

void CLightning::ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( !ShouldToggle( useType, m_active ) )
		return;
	if( m_active )
	{
		m_active = false;
		GetEffects() |= EF_NODRAW;
		SetNextThink( 0 );
	}
	else
	{
		m_active = true;
		GetEffects().ClearFlags( EF_NODRAW );
		DoSparks( GetStartPos(), GetEndPos() );
		if( GetDamage() > 0 )
		{
			SetNextThink( gpGlobals->time );
			SetDamageTime( gpGlobals->time );
		}
	}
}

void CLightning::BeamUpdateVars( void )
{
	int beamType;
	bool pointStart, pointEnd;

	CBaseEntity* pStart = UTIL_FindEntityByTargetname( nullptr, STRING( m_iszStartEntity ) );
	CBaseEntity* pEnd = UTIL_FindEntityByTargetname( nullptr, STRING( m_iszEndEntity ) );

	if( !pStart )
		pStart = CWorld::GetInstance();

	if( !pEnd )
		pEnd = CWorld::GetInstance();

	pointStart = UTIL_IsPointEntity( pStart );
	pointEnd = UTIL_IsPointEntity( pEnd );

	SetSkin( 0 );
	SetSequence( 0 );
	SetRenderMode( kRenderNormal );
	GetFlags() |= FL_CUSTOMENTITY;
	SetModelName( m_iszSpriteName );
	SetTexture( m_spriteTexture );

	beamType = BEAM_ENTS;
	if( pointStart || pointEnd )
	{
		if( !pointStart )	// One point entity must be in pStart
		{
			CBaseEntity* pTemp;
			// Swap start & end
			pTemp = pStart;
			pStart = pEnd;
			pEnd = pTemp;
			bool swap = pointStart;
			pointStart = pointEnd;
			pointEnd = swap;
		}
		if( !pointEnd )
			beamType = BEAM_ENTPOINT;
		else
			beamType = BEAM_POINTS;
	}

	SetType( beamType );
	if( beamType == BEAM_POINTS || beamType == BEAM_ENTPOINT || beamType == BEAM_HOSE )
	{
		SetStartPos( pStart->GetAbsOrigin() );
		if( beamType == BEAM_POINTS || beamType == BEAM_HOSE )
			SetEndPos( pEnd->GetAbsOrigin() );
		else
			SetEndEntity( pEnd->entindex() );
	}
	else
	{
		SetStartEntity( pStart->entindex() );
		SetEndEntity( pEnd->entindex() );
	}

	RelinkBeam();

	SetWidth( m_boltWidth );
	SetNoise( m_noiseAmplitude );
	SetFrame( m_frameStart );
	SetScrollRate( m_speed );
	if( GetSpawnFlags().Any( SF_BEAM_SHADEIN ) )
		SetBeamFlags( BEAM_FSHADEIN );
	else if( GetSpawnFlags().Any( SF_BEAM_SHADEOUT ) )
		SetBeamFlags( BEAM_FSHADEOUT );
}