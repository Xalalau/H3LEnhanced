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
	pev->solid = SOLID_NOT;							// Remove model & collisions
	Precache();
}

void CBeam::Precache( void )
{
	if( pev->owner )
		SetStartEntity( ENTINDEX( pev->owner ) );
	if( pev->aiment )
		SetEndEntity( ENTINDEX( pev->aiment ) );
}

void CBeam::TriggerTouch( CBaseEntity *pOther )
{
	if( pOther->pev->flags & ( FL_CLIENT | FL_MONSTER ) )
	{
		if( pev->owner )
		{
			CBaseEntity *pOwner = CBaseEntity::Instance( pev->owner );
			pOwner->Use( pOther, this, USE_TOGGLE, 0 );
		}
		ALERT( at_console, "Firing targets!!!\n" );
	}
}

void CBeam::SetStartEntity( int entityIndex )
{
	pev->sequence = ( entityIndex & 0x0FFF ) | ( ( pev->sequence & 0xF000 ) << 12 );
	pev->owner = g_engfuncs.pfnPEntityOfEntIndex( entityIndex );
}

void CBeam::SetEndEntity( int entityIndex )
{
	pev->skin = ( entityIndex & 0x0FFF ) | ( ( pev->skin & 0xF000 ) << 12 );
	pev->aiment = g_engfuncs.pfnPEntityOfEntIndex( entityIndex );
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
		return pev->angles;
	}

	edict_t *pent = g_engfuncs.pfnPEntityOfEntIndex( GetEndEntity() );
	if( pent )
		return pent->v.origin;
	return pev->angles;
}

void CBeam::RelinkBeam( void )
{
	const Vector &startPos = GetStartPos(), &endPos = GetEndPos();

	pev->mins.x = min( startPos.x, endPos.x );
	pev->mins.y = min( startPos.y, endPos.y );
	pev->mins.z = min( startPos.z, endPos.z );
	pev->maxs.x = max( startPos.x, endPos.x );
	pev->maxs.y = max( startPos.y, endPos.y );
	pev->maxs.z = max( startPos.z, endPos.z );
	pev->mins = pev->mins - GetAbsOrigin();
	pev->maxs = pev->maxs - GetAbsOrigin();

	SetSize( pev->mins, pev->maxs );
	SetAbsOrigin( GetAbsOrigin() );
}

#if 0
void CBeam::SetObjectCollisionBox( void )
{
	const Vector &startPos = GetStartPos(), &endPos = GetEndPos();

	pev->absmin.x = min( startPos.x, endPos.x );
	pev->absmin.y = min( startPos.y, endPos.y );
	pev->absmin.z = min( startPos.z, endPos.z );
	pev->absmax.x = max( startPos.x, endPos.x );
	pev->absmax.y = max( startPos.y, endPos.y );
	pev->absmax.z = max( startPos.z, endPos.z );
}
#endif

void CBeam::DoSparks( const Vector &start, const Vector &end )
{
	if( pev->spawnflags & ( SF_BEAM_SPARKSTART | SF_BEAM_SPARKEND ) )
	{
		if( pev->spawnflags & SF_BEAM_SPARKSTART )
		{
			UTIL_Sparks( start );
		}
		if( pev->spawnflags & SF_BEAM_SPARKEND )
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
			pHit->TraceAttack( CTakeDamageInfo( this, pev->dmg * ( gpGlobals->time - pev->dmgtime ), DMG_ENERGYBEAM ), ( ptr->vecEndPos - GetAbsOrigin() ).Normalize(), ptr );
			g_MultiDamage.ApplyMultiDamage( this, this );
			if( pev->spawnflags & SF_BEAM_DECALS )
			{
				if( pHit->IsBSPModel() )
					UTIL_DecalTrace( ptr, DECAL_BIGSHOT1 + RANDOM_LONG( 0, 4 ) );
			}
		}
	}
	pev->dmgtime = gpGlobals->time;
}

void CBeam::BeamInit( const char *pSpriteName, int width )
{
	pev->flags |= FL_CUSTOMENTITY;
	SetColor( 255, 255, 255 );
	SetBrightness( 255 );
	SetNoise( 0 );
	SetFrame( 0 );
	SetScrollRate( 0 );
	pev->model = MAKE_STRING( pSpriteName );
	SetTexture( PRECACHE_MODEL( ( char * ) pSpriteName ) );
	SetWidth( width );
	pev->skin = 0;
	pev->sequence = 0;
	pev->rendermode = 0;
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
	CBeam *pBeam = GetClassPtr( ( CBeam * ) NULL );
	pBeam->pev->classname = MAKE_STRING( "beam" );

	// ############ hu3lifezado ############ //
	// Lasers grossos, sempre.
	//pBeam->BeamInit( pSpriteName, width );
	pBeam->BeamInit(pSpriteName, width + 50);
	// ############ //

	return pBeam;
}