#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CLaserSpot.h"

BEGIN_DATADESC( CLaserSpot )
	DEFINE_THINKFUNC( Revive ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( laser_spot, CLaserSpot );

//=========================================================
//=========================================================
CLaserSpot *CLaserSpot::CreateSpot()
{
	auto pSpot = static_cast<CLaserSpot*>( UTIL_CreateNamedEntity( "laser_spot" ) );
	pSpot->Spawn();

	return pSpot;
}

//=========================================================
//=========================================================
void CLaserSpot::Spawn()
{
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	pev->rendermode = kRenderGlow;
	pev->renderfx = kRenderFxNoDissipation;
	pev->renderamt = 255;

	SetModel( "sprites/laserdot.spr" );
	SetAbsOrigin( GetAbsOrigin() );
};

//=========================================================
// Suspend- make the laser sight invisible. 
//=========================================================
void CLaserSpot::Suspend( float flSuspendTime )
{
	pev->effects |= EF_NODRAW;

	SetThink( &CLaserSpot::Revive );
	pev->nextthink = gpGlobals->time + flSuspendTime;
}

//=========================================================
// Revive - bring a suspended laser sight back.
//=========================================================
void CLaserSpot::Revive()
{
	pev->effects &= ~EF_NODRAW;

	SetThink( NULL );
}

void CLaserSpot::Precache()
{
	PRECACHE_MODEL( "sprites/laserdot.spr" );
}