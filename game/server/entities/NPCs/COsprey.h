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
#ifndef GAME_SERVER_ENTITIES_NPCS_COSPREY_H
#define GAME_SERVER_ENTITIES_NPCS_COSPREY_H

struct t_ospreygrunt
{
	int isValid;
	EHANDLE hGrunt;
	Vector	vecOrigin;
	Vector  vecAngles;
};

#define SF_OSPREY_WAITFORTRIGGER	0x40


#define MAX_CARRY	24

class COsprey : public CBaseMonster
{
public:
	DECLARE_CLASS( COsprey, CBaseMonster );
	DECLARE_DATADESC();

	int		ObjectCaps() const override { return CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	void Spawn() override;
	void Precache() override;
	EntityClassification_t GetClassification() override { return EntityClassifications().GetClassificationId( classify::MACHINE ); }
	int  BloodColor() const override { return DONT_BLEED; }
	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;

	void UpdateGoal();
	bool HasDead();
	void FlyThink( void );
	void DeployThink( void );
	void Flight( void );
	void HitTouch( CBaseEntity *pOther );
	void FindAllThink( void );
	void HoverThink( void );
	CBaseMonster *MakeGrunt( Vector vecSrc );
	void CrashTouch( CBaseEntity *pOther );
	void DyingThink( void );
	void CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	//void OnTakeDamage( const CTakeDamageInfo& info ) override;
	void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr ) override;
	void ShowDamage( void );

	Vector m_vel1;
	Vector m_vel2;
	Vector m_pos1;
	Vector m_pos2;
	Vector m_ang1;
	Vector m_ang2;
	float m_startTime;
	float m_dTime;

	Vector m_velocity;

	float m_flRotortilt;

	float m_flRightHealth;
	float m_flLeftHealth;

	int	m_iUnits;
	EHANDLE m_hGrunt[ MAX_CARRY ];
	Vector m_vecOrigin[ MAX_CARRY ];
	EHANDLE m_hRepel[ 4 ];

	int m_iSoundState;
	int m_iSpriteTexture;

	int m_iPitch;

	int m_iExplode;
	int	m_iTailGibs;
	int	m_iBodyGibs;
	int	m_iEngineGibs;

	int m_iDoLeftSmokePuff;
	int m_iDoRightSmokePuff;
};

#endif //GAME_SERVER_ENTITIES_NPCS_COSPREY_H