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
#ifndef GAME_SERVER_ENTITIES_NPCS_CBARNACLE_H
#define GAME_SERVER_ENTITIES_NPCS_CBARNACLE_H

#define	BARNACLE_BODY_HEIGHT	44 // how 'tall' the barnacle's model is.
// ############ hu3lifezado ############ //
// Agora a puxada e bem mais rapida. Muito mais emocao! (8)
#define BARNACLE_PULL_SPEED		30
// ############ //
#define BARNACLE_KILL_VICTIM_DELAY	5 // how many seconds after pulling prey in to gib them. 

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	BARNACLE_AE_PUKEGIB	2

class CBarnacle : public CBaseMonster
{
public:
	DECLARE_CLASS( CBarnacle, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	CBaseEntity *TongueTouchEnt( float *pflLength );
	EntityClassification_t GetClassification() override;
	void HandleAnimEvent( AnimEvent_t& event ) override;
	void BarnacleThink( void );
	void WaitTillDead( void );
	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;
	void OnTakeDamage( const CTakeDamageInfo& info ) override;

	float m_flAltitude;
	float m_flKillVictimTime;
	int	  m_cGibs;// barnacle loads up on gibs each time it kills something.
	bool  m_fTongueExtended;
	bool  m_fLiftingPrey;
	float m_flTongueAdj;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CBARNACLE_H