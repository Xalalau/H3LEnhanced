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
	PRECACHE_MODEL( GetModelName() );
	SetModel( GetModelName() );

	SetMoveType( MOVETYPE_NONE );
	SetSolidType( SOLID_BBOX );
	SetHealth( 80000 );
	SetTakeDamageMode( DAMAGE_AIM );
	GetEffects().ClearAll();
	SetYawSpeed( 0 );
	SetSequence( 0 );
	SetFrame( 0 );

	//	SetNextThink( GetNextThink() + 1.0 );
	//	SetThink (WalkMonsterDelay);

	ResetSequenceInfo();
	SetFrame( 0 );
	MonsterInit();
}

//=========================================================
// Furniture is killed
//=========================================================
void CFurniture::Die( void )
{
	SetThink( &CFurniture::SUB_Remove );
	SetNextThink( gpGlobals->time );
}

//=========================================================
// ID's Furniture as neutral (noone will attack it)
//=========================================================
EntityClassification_t CFurniture::GetClassification()
{
	return EntityClassifications().GetNoneId();
}