#include "extdll.h"
#include "util.h"
#include "Skill.h"
#include "cbase.h"
#include "Weapons.h"
#include "entities/weapons/CRpg.h"

#include "CRpgRocket.h"

BEGIN_DATADESC( CRpgRocket )
	DEFINE_FIELD( m_flIgniteTime, FIELD_TIME ),
	DEFINE_FIELD( m_hLauncher, FIELD_EHANDLE ),
	DEFINE_THINKFUNC( FollowThink ),
	DEFINE_THINKFUNC( IgniteThink ),
	DEFINE_TOUCHFUNC( RocketTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( rpg_rocket, CRpgRocket );

//=========================================================
//=========================================================
CRpgRocket *CRpgRocket::CreateRpgRocket( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner, CRpg *pLauncher )
{
	auto pRocket = static_cast<CRpgRocket*>( UTIL_CreateNamedEntity( "rpg_rocket" ) );

	pRocket->SetAbsOrigin( vecOrigin );
	pRocket->SetAbsAngles( vecAngles );
	pRocket->Spawn();
	pRocket->SetTouch( &CRpgRocket::RocketTouch );
	pRocket->m_hLauncher = pLauncher;// remember what RPG fired me. 
	pLauncher->m_cActiveRockets++;// register this missile as active for the launcher
	pRocket->SetOwner( pOwner );

	return pRocket;
}

//=========================================================
//=========================================================
void CRpgRocket::Spawn( void )
{
	Precache();
	// motor
	SetMoveType( MOVETYPE_BOUNCE );
	SetSolidType( SOLID_BBOX );

	SetModel( "models/rpgrocket.mdl" );
	SetSize( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );
	SetAbsOrigin( GetAbsOrigin() );

	SetThink( &CRpgRocket::IgniteThink );
	SetTouch( &CRpgRocket::ExplodeTouch );

	Vector vecAngles = GetAbsAngles();
	vecAngles.x -= 30;
	UTIL_MakeVectors( vecAngles );
	vecAngles.x = -( vecAngles.x + 30 );
	SetAbsAngles( vecAngles );

	SetAbsVelocity( gpGlobals->v_forward * 250 );
	SetGravity( 0.5 );

	SetNextThink( gpGlobals->time + 0.4 );

	SetDamage( gSkillData.GetPlrDmgRPG() );
}

//=========================================================
//=========================================================
void CRpgRocket::RocketTouch( CBaseEntity *pOther )
{
	if( CRpg* pLauncher = EHANDLE_cast<CRpg*>( m_hLauncher ) )
	{
		// my launcher is still around, tell it I'm dead.
		pLauncher->m_cActiveRockets--;
	}

	STOP_SOUND( this, CHAN_VOICE, "weapons/rocket1.wav" );
	ExplodeTouch( pOther );
}

//=========================================================
//=========================================================
void CRpgRocket::Precache( void )
{
	PRECACHE_MODEL( "models/rpgrocket.mdl" );
	m_iTrail = PRECACHE_MODEL( "sprites/smoke.spr" );
	PRECACHE_SOUND( "weapons/rocket1.wav" );
}


void CRpgRocket::IgniteThink( void )
{
	// SetMoveType( MOVETYPE_TOSS );

	SetMoveType( MOVETYPE_FLY );
	GetEffects() |= EF_LIGHT;

	// make rocket sound
	EMIT_SOUND( this, CHAN_VOICE, "weapons/rocket1.wav", 1, 0.5 );

	// rocket trail
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

	WRITE_BYTE( TE_BEAMFOLLOW );
	WRITE_SHORT( entindex() );	// entity
	WRITE_SHORT( m_iTrail );	// model
	WRITE_BYTE( 40 ); // life
	WRITE_BYTE( 5 );  // width
	WRITE_BYTE( 224 );   // r, g, b
	WRITE_BYTE( 224 );   // r, g, b
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 255 );	// brightness

	MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	m_flIgniteTime = gpGlobals->time;

	// set to follow laser spot
	SetThink( &CRpgRocket::FollowThink );
	SetNextThink( gpGlobals->time + 0.1 );
}


void CRpgRocket::FollowThink( void )
{
	CBaseEntity *pOther = NULL;
	Vector vecTarget;
	Vector vecDir;
	float flDist, flMax, flDot;
	TraceResult tr;

	UTIL_MakeAimVectors( GetAbsAngles() );

	vecTarget = gpGlobals->v_forward;
	flMax = 4096;

	// Examine all entities within a reasonable radius
	//NOTENOTE: This deliberately searches for all laser spots so other players can throw off the rockets with their own lasers. - Solokiller
	while( ( pOther = UTIL_FindEntityByClassname( pOther, "laser_spot" ) ) != NULL )
	{
		UTIL_TraceLine( GetAbsOrigin(), pOther->GetAbsOrigin(), dont_ignore_monsters, ENT( pev ), &tr );
		// ALERT( at_console, "%f\n", tr.flFraction );
		if( tr.flFraction >= 0.90 )
		{
			vecDir = pOther->GetAbsOrigin() - GetAbsOrigin();
			flDist = vecDir.Length();
			vecDir = vecDir.Normalize();
			flDot = DotProduct( gpGlobals->v_forward, vecDir );
			if( ( flDot > 0 ) && ( flDist * ( 1 - flDot ) < flMax ) )
			{
				flMax = flDist * ( 1 - flDot );
				vecTarget = vecDir;
			}
		}
	}

	SetAbsAngles( UTIL_VecToAngles( vecTarget ) );

	// this acceleration and turning math is totally wrong, but it seems to respond well so don't change it.
	float flSpeed = GetAbsVelocity().Length();
	if( gpGlobals->time - m_flIgniteTime < 1.0 )
	{
		SetAbsVelocity( GetAbsVelocity() * 0.2 + vecTarget * ( flSpeed * 0.8 + 400 ) );
		if( GetWaterLevel() == WATERLEVEL_HEAD )
		{
			// go slow underwater
			if( GetAbsVelocity().Length() > 300 )
			{
				SetAbsVelocity( GetAbsVelocity().Normalize() * 300 );
			}
			UTIL_BubbleTrail( GetAbsOrigin() - GetAbsVelocity() * 0.1, GetAbsOrigin(), 4 );
		}
		else
		{
			if( GetAbsVelocity().Length() > 2000 )
			{
				SetAbsVelocity( GetAbsVelocity().Normalize() * 2000 );
			}
		}
	}
	else
	{
		if( GetEffects().Any( EF_LIGHT ) )
		{
			GetEffects().ClearAll();
			STOP_SOUND( this, CHAN_VOICE, "weapons/rocket1.wav" );
		}
		SetAbsVelocity( GetAbsVelocity() * 0.2 + vecTarget * flSpeed * 0.798 );
		if( GetWaterLevel() == WATERLEVEL_DRY && GetAbsVelocity().Length() < 1500 )
		{
			Detonate();
		}
	}
	// ALERT( at_console, "%.0f\n", flSpeed );

	SetNextThink( gpGlobals->time + 0.1 );
}