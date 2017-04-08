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
#ifndef GAME_SERVER_ENTITIES_NPCS_CBLOATER_H
#define GAME_SERVER_ENTITIES_NPCS_CBLOATER_H

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	BLOATER_AE_ATTACK_MELEE1		0x01

class CBloater : public CBaseMonster
{
public:
	DECLARE_CLASS( CBloater, CBaseMonster );

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetYawSpeed( void ) override;
	EntityClassification_t GetClassification() override;
	void HandleAnimEvent( AnimEvent_t& event ) override;

	void PainSound( void ) override;
	void AlertSound( void ) override;
	void IdleSound( void ) override;
	void AttackSnd( void );

	// No range attacks
	bool CheckRangeAttack1( float flDot, float flDist ) override { return false; }
	bool CheckRangeAttack2( float flDot, float flDist ) override { return false; }
	void OnTakeDamage( const CTakeDamageInfo& info ) override;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CBLOATER_H