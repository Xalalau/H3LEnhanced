#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CPathTrack.h"

BEGIN_DATADESC( CPathTrack )
	DEFINE_FIELD( m_length, FIELD_FLOAT ),
	DEFINE_FIELD( m_pnext, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_paltpath, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_pprevious, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_altName, FIELD_STRING ),

#ifdef PATH_SPARKLE_DEBUG
	DEFINE_THINKFUNC( Sparkle ),
#endif
END_DATADESC()

LINK_ENTITY_TO_CLASS( path_track, CPathTrack );

void CPathTrack::Spawn( void )
{
	SetSolidType( SOLID_TRIGGER );
	SetSize( Vector( -8, -8, -8 ), Vector( 8, 8, 8 ) );

	m_pnext = NULL;
	m_pprevious = NULL;
	// DEBUGGING CODE
#if PATH_SPARKLE_DEBUG
	SetThink( Sparkle );
	SetNextThink( gpGlobals->time + 0.5 );
#endif
}

void CPathTrack::Activate( void )
{
	if( HasTargetname() )		// Link to next, and back-link
		Link();
}

//
// Cache user-entity-field values until spawn is called.
//
void CPathTrack::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "altpath" ) )
	{
		m_altName = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CPathTrack::SetPrevious( CPathTrack *pprev )
{
	// Only set previous if this isn't my alternate path
	if( pprev && !FStrEq( pprev->GetTargetname(), STRING( m_altName ) ) )
		m_pprevious = pprev;
}

void CPathTrack::Link( void )
{
	CBaseEntity* pTarget = nullptr;

	if( HasTarget() )
	{
		pTarget = UTIL_FindEntityByTargetname( nullptr, GetTarget() );
		if( pTarget )
		{
			m_pnext = CPathTrack::Instance( pTarget );

			if( m_pnext )		// If no next pointer, this is the end of a path
			{
				m_pnext->SetPrevious( this );
			}
		}
		else
			ALERT( at_console, "Dead end link %s\n", GetTarget() );
	}

	// Find "alternate" path
	if( m_altName )
	{
		pTarget = UTIL_FindEntityByTargetname( nullptr, STRING( m_altName ) );
		if( pTarget )
		{
			m_paltpath = CPathTrack::Instance( pTarget );

			if( m_paltpath )		// If no next pointer, this is the end of a path
			{
				m_paltpath->SetPrevious( this );
			}
		}
	}
}

void CPathTrack::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	bool on;

	// Use toggles between two paths
	if( m_paltpath )
	{
		on = !GetSpawnFlags().Any( SF_PATH_ALTERNATE );
		if( ShouldToggle( useType, on ) )
		{
			if( on )
				GetSpawnFlags().AddFlags( SF_PATH_ALTERNATE );
			else
				GetSpawnFlags().ClearFlags( SF_PATH_ALTERNATE );
		}
	}
	else	// Use toggles between enabled/disabled
	{
		on = !GetSpawnFlags().Any( SF_PATH_DISABLED );

		if( ShouldToggle( useType, on ) )
		{
			if( on )
				GetSpawnFlags().AddFlags( SF_PATH_DISABLED );
			else
				GetSpawnFlags().ClearFlags( SF_PATH_DISABLED );
		}
	}
}

CPathTrack	*CPathTrack::ValidPath( CPathTrack *ppath, const bool bTestFlag )
{
	if( !ppath )
		return NULL;

	if( bTestFlag && ppath->GetSpawnFlags().Any( SF_PATH_DISABLED ) )
		return NULL;

	return ppath;
}

void CPathTrack::Project( CPathTrack *pstart, CPathTrack *pend, Vector& origin, float dist )
{
	if( pstart && pend )
	{
		Vector dir = ( pend->GetAbsOrigin() - pstart->GetAbsOrigin() );
		dir = dir.Normalize();
		origin = pend->GetAbsOrigin() + dir * dist;
	}
}

CPathTrack* CPathTrack::Instance( CBaseEntity* pEntity )
{
	if( pEntity && pEntity->ClassnameIs( "path_track" ) )
		return static_cast<CPathTrack*>( pEntity );

	return nullptr;
}

// Assumes this is ALWAYS enabled
CPathTrack *CPathTrack::LookAhead( Vector& origin, float dist, const bool bMove )
{
	CPathTrack *pcurrent;
	float originalDist = dist;

	pcurrent = this;
	Vector currentPos = origin;

	if( dist < 0 )		// Travelling backwards through path
	{
		dist = -dist;
		while( dist > 0 )
		{
			Vector dir = pcurrent->GetAbsOrigin() - currentPos;
			float length = dir.Length();
			if( !length )
			{
				if( !ValidPath( pcurrent->GetPrevious(), bMove ) ) 	// If there is no previous node, or it's disabled, return now.
				{
					if( !bMove )
						Project( pcurrent->GetNext(), pcurrent, origin, dist );
					return NULL;
				}
				pcurrent = pcurrent->GetPrevious();
			}
			else if( length > dist )	// enough left in this path to move
			{
				origin = currentPos + ( dir * ( dist / length ) );
				return pcurrent;
			}
			else
			{
				dist -= length;
				currentPos = pcurrent->GetAbsOrigin();
				origin = currentPos;
				if( !ValidPath( pcurrent->GetPrevious(), bMove ) )	// If there is no previous node, or it's disabled, return now.
					return NULL;

				pcurrent = pcurrent->GetPrevious();
			}
		}
		origin = currentPos;
		return pcurrent;
	}
	else
	{
		while( dist > 0 )
		{
			if( !ValidPath( pcurrent->GetNext(), bMove ) )	// If there is no next node, or it's disabled, return now.
			{
				if( !bMove )
					Project( pcurrent->GetPrevious(), pcurrent, origin, dist );
				return NULL;
			}
			Vector dir = pcurrent->GetNext()->GetAbsOrigin() - currentPos;
			float length = dir.Length();
			if( !length && !ValidPath( pcurrent->GetNext()->GetNext(), bMove ) )
			{
				if( dist == originalDist ) // HACK -- up against a dead end
					return NULL;
				return pcurrent;
			}
			if( length > dist )	// enough left in this path to move
			{
				origin = currentPos + ( dir * ( dist / length ) );
				return pcurrent;
			}
			else
			{
				dist -= length;
				currentPos = pcurrent->GetNext()->GetAbsOrigin();
				pcurrent = pcurrent->GetNext();
				origin = currentPos;
			}
		}
		origin = currentPos;
	}

	return pcurrent;
}

// Assumes this is ALWAYS enabled
CPathTrack *CPathTrack::Nearest( Vector origin )
{
	int			deadCount;
	float		minDist, dist;
	Vector		delta;
	CPathTrack	*ppath, *pnearest;


	delta = origin - GetAbsOrigin();
	delta.z = 0;
	minDist = delta.Length();
	pnearest = this;
	ppath = GetNext();

	// Hey, I could use the old 2 racing pointers solution to this, but I'm lazy :)
	deadCount = 0;
	while( ppath && ppath != this )
	{
		deadCount++;
		if( deadCount > 9999 )
		{
			ALERT( at_error, "Bad sequence of path_tracks from %s", GetTargetname() );
			return NULL;
		}
		delta = origin - ppath->GetAbsOrigin();
		delta.z = 0;
		dist = delta.Length();
		if( dist < minDist )
		{
			minDist = dist;
			pnearest = ppath;
		}
		ppath = ppath->GetNext();
	}
	return pnearest;
}

CPathTrack *CPathTrack::GetNext( void )
{
	if( m_paltpath && GetSpawnFlags().Any( SF_PATH_ALTERNATE ) && !GetSpawnFlags().Any( SF_PATH_ALTREVERSE ) )
		return m_paltpath;

	return m_pnext;
}

CPathTrack *CPathTrack::GetPrevious( void )
{
	if( m_paltpath && GetSpawnFlags().Any( SF_PATH_ALTERNATE ) && GetSpawnFlags().Any( SF_PATH_ALTREVERSE ) )
		return m_paltpath;

	return m_pprevious;
}

// DEBUGGING CODE
#if PATH_SPARKLE_DEBUG
void CPathTrack::Sparkle( void )
{

	SetNextThink( gpGlobals->time + 0.2 );
	if( GetSpawnFlags().Any( SF_PATH_DISABLED ) )
		UTIL_ParticleEffect( GetAbsOrigin(), Vector( 0, 0, 100 ), 210, 10 );
	else
		UTIL_ParticleEffect( GetAbsOrigin(), Vector( 0, 0, 100 ), 84, 10 );
}
#endif

