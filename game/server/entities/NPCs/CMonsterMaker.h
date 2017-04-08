/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef GAME_SERVER_ENTITIES_NPCS_CMONSTERMAKER_H
#define GAME_SERVER_ENTITIES_NPCS_CMONSTERMAKER_H

// Monstermaker spawnflags
#define	SF_MONSTERMAKER_START_ON	1 // start active ( if has targetname )
#define	SF_MONSTERMAKER_CYCLIC		4 // drop one monster every time fired.
#define SF_MONSTERMAKER_MONSTERCLIP	8 // Children are blocked by monsterclip

//=========================================================
// MonsterMaker - this ent creates monsters during the game.
//=========================================================
class CMonsterMaker : public CBaseMonster
{
public:
	DECLARE_CLASS( CMonsterMaker, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void KeyValue( KeyValueData* pkvd ) override;
	void ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void CyclicUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void MakerThink( void );
	void DeathNotice( CBaseEntity* pChild ) override;// monster maker children use this to tell the monster maker that they have died.
	void MakeMonster( void );

	bool Save( CSave& save ) override;
	bool Restore( CRestore& restore ) override;

	string_t m_iszMonsterClassname;// classname of the monster(s) that will be created.

	int	 m_cNumMonsters;// max number of monsters this ent can create


	int  m_cLiveChildren;// how many monsters made by this monster maker that are currently alive
	int	 m_iMaxLiveChildren;// max number of monsters that this maker may have out at one time.

	float m_flGround; // z coord of the ground under me, used to make sure no monsters are under the maker when it drops a new child

	bool m_fActive;
	bool m_fFadeChildren;// should we make the children fadeout?

	EntityClassification_t m_MonsterClassificationOverride = INVALID_ENTITY_CLASSIFICATION;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CMONSTERMAKER_H