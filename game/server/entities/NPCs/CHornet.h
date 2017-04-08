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
//=========================================================
// Hornets
//=========================================================

//=========================================================
// Hornet Defines
//=========================================================
#define HORNET_TYPE_RED			0
#define HORNET_TYPE_ORANGE		1
#define HORNET_RED_SPEED		(float)600
#define HORNET_ORANGE_SPEED		(float)800
#define	HORNET_BUZZ_VOLUME		(float)0.8

extern int iHornetPuff;

//=========================================================
// Hornet - this is the projectile that the Alien Grunt fires.
//=========================================================
class CHornet : public CBaseMonster
{
public:
	DECLARE_CLASS( CHornet, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	EntityClassification_t GetClassification() override;
	Relationship IRelationship( CBaseEntity *pTarget ) override;

	void IgniteTrail( void );
	void StartTrack ( void );
	void StartDart ( void );
	void TrackTarget ( void );
	void TrackTouch ( CBaseEntity *pOther );
	void DartTouch( CBaseEntity *pOther );
	void DieTouch ( CBaseEntity *pOther );
	
	void OnTakeDamage( const CTakeDamageInfo& info ) override;

	float			m_flStopAttack;
	int				m_iHornetType;
	float			m_flFlySpeed;
};

