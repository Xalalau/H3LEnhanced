#include "extdll.h"
#include "util.h"
#include "cbase.h"

void CBaseEntity::UpdateOnRemove()
{
}

bool CBaseEntity::Save( CSave &save )
{
	return true;
}

bool CBaseEntity::Restore( CRestore &restore )
{
	return true;
}

void CBaseEntity::SetObjectCollisionBox( void )
{
}

CBaseEntity* CBaseEntity::Create( const char* const pszName, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner, const bool bSpawnEntity )
{
	return nullptr;
}

void CBaseEntity::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
}

void CBaseEntity::TraceBleed( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
}

void CBaseEntity::OnTakeDamage( const CTakeDamageInfo& info )
{
}

/*
=====================
CBaseEntity :: Killed

If weapons code "kills" an entity, just set its effects to EF_NODRAW
=====================
*/
void CBaseEntity::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	GetEffects() |= EF_NODRAW;
}

float CBaseEntity::GiveHealth( float flHealth, int bitsDamageType )
{
	return flHealth;
}

int	CBaseEntity::DamageDecal( int bitsDamageType ) const
{
	return -1;
}

bool CBaseEntity::IsInWorld() const
{
	return true;
}

CBaseEntity* CBaseEntity::GetNextTarget( void )
{
	return NULL;
}

void CBaseEntity::SUB_Remove( void )
{
}

void CBaseEntity::SUB_StartFadeOut()
{
}

void CBaseEntity::SUB_FadeOut()
{
}

bool CBaseEntity::ShouldToggle( USE_TYPE useType, const bool currentState ) const
{
	return false;
}

void CBaseEntity::FireBullets( const unsigned int cShots,
							   Vector vecSrc, Vector vecDirShooting, Vector vecSpread,
							   float flDistance, int iBulletType,
							   int iTracerFreq, int iDamage, CBaseEntity* pAttacker )
{
}

/*
=====================
CBaseEntity::FireBulletsPlayer

Only produces random numbers to match the server ones.
=====================
*/
Vector CBaseEntity::FireBulletsPlayer( const unsigned int cShots,
									   Vector vecSrc, Vector vecDirShooting, Vector vecSpread,
									   float flDistance, int iBulletType,
									   int iTracerFreq, int iDamage, CBaseEntity* pAttacker, int shared_rand )
{
	float x = 0, y = 0;

	for( unsigned int iShot = 1; iShot <= cShots; iShot++ )
	{
		if( pAttacker == NULL )
		{
			// get circular gaussian spread
			UTIL_GetCircularGaussianSpread( x, y );
		}
		else
		{
			//Use player's random seed.
			// get circular gaussian spread
			UTIL_GetSharedCircularGaussianSpread( shared_rand, iShot, x, y );
		}

	}

	return Vector( x * vecSpread.x, y * vecSpread.y, 0.0 );
}

void CBaseEntity::SUB_UseTargets( CBaseEntity *pActivator, USE_TYPE useType, float value )
{
}

bool CBaseEntity::Intersects( const CBaseEntity* const pOther ) const
{
	return false;
}

void CBaseEntity::MakeDormant( void )
{
}

bool CBaseEntity::IsDormant() const
{
	return false;
}

bool CBaseEntity::FVisible( const CBaseEntity *pEntity ) const
{
	return false;
}

bool CBaseEntity::FVisible( const Vector &vecOrigin ) const
{
	return false;
}

bool CBaseEntity::FBoxVisible( const CBaseEntity* pTarget, Vector& vecTargetOrigin, float flSize ) const
{
	return false;
}
