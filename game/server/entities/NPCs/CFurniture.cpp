#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CFurniture.h"

LINK_ENTITY_TO_CLASS( monster_furniture, CFurniture );

//=========================================================
// This used to have something to do with bees flying, but 
// now it only initializes moving furniture in scripted sequences
//=========================================================
void CFurniture::Spawn()
{
	PRECACHE_MODEL( ( char * ) STRING( pev->model ) );
	SetModel( STRING( pev->model ) );

	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_BBOX;
	pev->health = 80000;
	pev->takedamage = DAMAGE_AIM;
	pev->effects = 0;
	pev->yaw_speed = 0;
	pev->sequence = 0;
	pev->frame = 0;

	//	pev->nextthink += 1.0;
	//	SetThink (WalkMonsterDelay);

	ResetSequenceInfo();
	pev->frame = 0;
	MonsterInit();
}

//=========================================================
// Furniture is killed
//=========================================================
void CFurniture::Die( void )
{
	SetThink( &CFurniture::SUB_Remove );
	pev->nextthink = gpGlobals->time;
}

//=========================================================
// ID's Furniture as neutral (noone will attack it)
//=========================================================
EntityClassification_t CFurniture::GetClassification()
{
	return EntityClassifications().GetNoneId();
}