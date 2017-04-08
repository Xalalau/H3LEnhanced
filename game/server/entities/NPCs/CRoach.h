/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#ifndef GAME_SERVER_ENTITIES_NPCS_CROACH_H
#define GAME_SERVER_ENTITIES_NPCS_CROACH_H

#define		ROACH_IDLE				0
#define		ROACH_BORED				1
#define		ROACH_SCARED_BY_ENT		2
#define		ROACH_SCARED_BY_LIGHT	3
#define		ROACH_SMELL_FOOD		4
#define		ROACH_EAT				5

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
class CRoach : public CBaseMonster
{
public:
	DECLARE_CLASS( CRoach, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetYawSpeed( void ) override;
	void MonsterThink( void ) override;
	void Move( float flInterval ) override;
	void PickNewDest( int iCondition );
	void Touch( CBaseEntity *pOther ) override;
	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;

	float	m_flLastLightLevel;
	float	m_flNextSmellTime;
	EntityClassification_t GetClassification() override;
	void	Look( int iDistance ) override;
	int		ISoundMask( void ) override;

	// UNDONE: These don't necessarily need to be save/restored, but if we add more data, it may
	bool	m_fLightHacked;
	int		m_iMode;
	// -----------------------------
};

#endif //GAME_SERVER_ENTITIES_NPCS_CROACH_H