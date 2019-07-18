#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"

#include "customentity.h"
#include "Decals.h"

#include "CBeam.h"

BEGIN_DATADESC( CBeam )
	DEFINE_TOUCHFUNC( TriggerTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( beam, CBeam );

void CBeam::Spawn( void )
{
	SetSolidType( SOLID_NOT );							// Remove model & collisions
	Precache();
}

void CBeam::Precache( void )
{
	if( GetOwner() )
		SetStartEntity( GetOwner()->entindex() );

	if( auto pAimEnt = GetAimEntity() )
		SetEndEntity( pAimEnt->entindex() );
}

void CBeam::TriggerTouch( CBaseEntity *pOther )
{
	if( pOther->GetFlags().Any( FL_CLIENT | FL_MONSTER ) )
	{
		if( GetOwner() )
		{
			CBaseEntity *pOwner = GetOwner();
			pOwner->Use( pOther, this, USE_TOGGLE, 0 );
		}
		ALERT( at_console, "Firing targets!!!\n" );
	}
}

void CBeam::SetStartEntity( int entityIndex )
{
	SetSequence( ( entityIndex & 0x0FFF ) | ( ( GetSequence() & 0xF000 ) << 12 ) );
	SetOwner( UTIL_EntityByIndex( entityIndex ) );
}

void CBeam::SetEndEntity( int entityIndex )
{
	SetSkin( ( entityIndex & 0x0FFF ) | ( ( GetSkin() & 0xF000 ) << 12 ) );
	SetAimEntity( UTIL_EntityByIndex( entityIndex ) );
}

// These don't take attachments into account
const Vector& CBeam::GetStartPos() const
{
	if( GetType() == BEAM_ENTS )
	{
		edict_t *pent = g_engfuncs.pfnPEntityOfEntIndex( GetStartEntity() );
		return pent->v.origin;
	}
	return GetAbsOrigin();
}

const Vector& CBeam::GetEndPos() const
{
	int type = GetType();
	if( type == BEAM_POINTS || type == BEAM_HOSE )
	{
		return GetAbsAngles();
	}

	edict_t *pent = g_engfuncs.pfnPEntityOfEntIndex( GetEndEntity() );
	if( pent )
		return pent->v.origin;
	return GetAbsAngles();
}

void CBeam::RelinkBeam( void )
{
	const Vector &startPos = GetStartPos(), &endPos = GetEndPos();

	Vector vecMins(
		min( startPos.x, endPos.x ),
		min( startPos.y, endPos.y ),
		min( startPos.z, endPos.z )
	);
	Vector vecMaxs(
		max( startPos.x, endPos.x ),
		max( startPos.y, endPos.y ),
		max( startPos.z, endPos.z )
	);
	vecMins = vecMins - GetAbsOrigin();
	vecMaxs = vecMaxs - GetAbsOrigin();

	SetSize( vecMins, vecMaxs );
	SetAbsOrigin( GetAbsOrigin() );
}

#if 0
void CBeam::SetObjectCollisionBox( void )
{
	const Vector &startPos = GetStartPos(), &endPos = GetEndPos();

	SetAbsMin( Vector(
		min( startPos.x, endPos.x ),
		min( startPos.y, endPos.y ),
		min( startPos.z, endPos.z )
		) );

	SetAbsMax( Vector(
		max( startPos.x, endPos.x ),
		max( startPos.y, endPos.y ),
		max( startPos.z, endPos.z )
	) );
}
#endif

void CBeam::DoSparks( const Vector &start, const Vector &end )
{
	if( GetSpawnFlags().Any( SF_BEAM_SPARKSTART | SF_BEAM_SPARKEND ) )
	{
		if( GetSpawnFlags().Any( SF_BEAM_SPARKSTART ) )
		{
			UTIL_Sparks( start );
		}
		if( GetSpawnFlags().Any( SF_BEAM_SPARKEND ) )
		{
			UTIL_Sparks( end );
		}
	}
}

void CBeam::BeamDamage( TraceResult *ptr )
{
	RelinkBeam();
	if( ptr->flFraction != 1.0 && ptr->pHit != NULL )
	{
		CBaseEntity *pHit = CBaseEntity::Instance( ptr->pHit );
		if( pHit )
		{
			g_MultiDamage.Clear();
			pHit->TraceAttack( CTakeDamageInfo( this, GetDamage() * ( gpGlobals->time - GetDamageTime() ), DMG_ENERGYBEAM ), ( ptr->vecEndPos - GetAbsOrigin() ).Normalize(), *ptr );
			g_MultiDamage.ApplyMultiDamage( this, this );
			if( GetSpawnFlags().Any( SF_BEAM_DECALS ) )
			{
				if( pHit->IsBSPModel() )
					UTIL_DecalTrace( ptr, DECAL_BIGSHOT1 + RANDOM_LONG( 0, 4 ) );
			}
		}
	}
	SetDamageTime( gpGlobals->time );
}

void CBeam::BeamInit( const char *pSpriteName, int width )
{
	GetFlags() |= FL_CUSTOMENTITY;
	SetColor( 255, 255, 255 );
	SetBrightness( 255 );
	SetNoise( 0 );
	SetFrame( 0 );
	SetScrollRate( 0 );
	SetModelName( pSpriteName );
	SetTexture( PRECACHE_MODEL( pSpriteName ) );
	SetWidth( width );
	SetSkin( 0 );
	SetSequence( 0 );
	SetRenderMode( kRenderNormal );
}

void CBeam::PointsInit( const Vector &start, const Vector &end )
{
	SetType( BEAM_POINTS );
	SetStartPos( start );
	SetEndPos( end );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::PointEntInit( const Vector &start, int endIndex )
{
	SetType( BEAM_ENTPOINT );
	SetStartPos( start );
	SetEndEntity( endIndex );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::EntsInit( int startIndex, int endIndex )
{
	SetType( BEAM_ENTS );
	SetStartEntity( startIndex );
	SetEndEntity( endIndex );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::HoseInit( const Vector &start, const Vector &direction )
{
	SetType( BEAM_HOSE );
	SetStartPos( start );
	SetEndPos( direction );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

CBeam *CBeam::BeamCreate( const char *pSpriteName, int width )
{
	// Create a new entity with CBeam private data
	auto pBeam = static_cast<CBeam*>( UTIL_CreateNamedEntity( "beam" ) );

	// ############ hu3lifezado ############ //
	// Lasers grossos, sempre.
	//pBeam->BeamInit( pSpriteName, width );
	pBeam->BeamInit(pSpriteName, width + 50);
	// ############ //

	return pBeam;
}