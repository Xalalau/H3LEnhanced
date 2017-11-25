#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CBlood.h"

LINK_ENTITY_TO_CLASS( env_blood, CBlood );

void CBlood::Spawn( void )
{
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );
	GetEffects().ClearAll();
	SetFrame( 0 );
	SetMovedir( this );
}

void CBlood::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( GetSpawnFlags().Any( SF_BLOOD_STREAM ) )
		UTIL_BloodStream( BloodPosition( pActivator ), Direction(), ( Color() == BLOOD_COLOR_RED ) ? 70 : Color(), BloodAmount() );
	else
		UTIL_BloodDrips( BloodPosition( pActivator ), Direction(), Color(), BloodAmount() );

	if( GetSpawnFlags().Any( SF_BLOOD_DECAL ) )
	{
		Vector forward = Direction();
		Vector start = BloodPosition( pActivator );
		TraceResult tr;

		UTIL_TraceLine( start, start + forward * BloodAmount() * 2, ignore_monsters, NULL, &tr );
		if( tr.flFraction != 1.0 )
			UTIL_BloodDecalTrace( &tr, Color() );
	}
}

void CBlood::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "color" ) )
	{
		int color = atoi( pkvd->szValue );
		switch( color )
		{
		case 1:
			SetColor( BLOOD_COLOR_YELLOW );
			break;
		default:
			SetColor( BLOOD_COLOR_RED );
			break;
		}

		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "amount" ) )
	{
		SetBloodAmount( atof( pkvd->szValue ) );
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

Vector CBlood::Direction( void )
{
	if( GetSpawnFlags().Any( SF_BLOOD_RANDOM ) )
		return UTIL_RandomBloodVector();

	return GetMoveDir();
}


Vector CBlood::BloodPosition( CBaseEntity *pActivator )
{
	if( GetSpawnFlags().Any( SF_BLOOD_PLAYER ) )
	{
		edict_t *pPlayer;

		if( pActivator && pActivator->IsPlayer() )
		{
			pPlayer = pActivator->edict();
		}
		else
			pPlayer = g_engfuncs.pfnPEntityOfEntIndex( 1 );
		if( pPlayer )
			return ( pPlayer->v.origin + pPlayer->v.view_ofs ) + Vector( RANDOM_FLOAT( -10, 10 ), RANDOM_FLOAT( -10, 10 ), RANDOM_FLOAT( -10, 10 ) );
	}

	return GetAbsOrigin();
}