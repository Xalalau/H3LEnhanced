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
#ifndef GAME_SERVER_ENTITIES_NPCS_CBABYCRAB_H
#define GAME_SERVER_ENTITIES_NPCS_CBABYCRAB_H

#include "CHeadCrab.h"

class CBabyCrab : public CHeadCrab
{
public:
	DECLARE_CLASS( CBabyCrab, CHeadCrab );

	void Spawn( void ) override;
	void Precache( void ) override;
	void UpdateYawSpeed() override;
	float GetDamageAmount( void ) override { return gSkillData.GetHeadcrabDmgBite() * 0.3; }
	bool CheckRangeAttack1( float flDot, float flDist ) override;
	Schedule_t* GetScheduleOfType( int Type ) override;
	virtual int GetVoicePitch( void ) override { return PITCH_NORM + RANDOM_LONG( 40, 50 ); }
	virtual float GetSoundVolume() const override { return 0.8; }
};

#endif //GAME_SERVER_ENTITIES_NPCS_CBABYCRAB_H