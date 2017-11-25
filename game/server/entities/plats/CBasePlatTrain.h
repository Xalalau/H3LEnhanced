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
#ifndef GAME_SERVER_ENTITIES_PLATS_CBASEPLATTRAIN_H
#define GAME_SERVER_ENTITIES_PLATS_CBASEPLATTRAIN_H

#define SF_PLAT_TOGGLE		0x0001

#define noiseMoving noise
#define noiseArrived noise1

#define noiseMovement noise
#define noiseStopMoving noise1

class CBasePlatTrain : public CBaseToggle
{
public:
	DECLARE_CLASS( CBasePlatTrain, CBaseToggle );
	DECLARE_DATADESC();

	virtual int	ObjectCaps() const override { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	void KeyValue( KeyValueData* pkvd ) override;
	void Precache() override;

	// This is done to fix spawn flag collisions between this class and a derived class
	virtual bool IsTogglePlat() const { return GetSpawnFlags().Any( SF_PLAT_TOGGLE ); }

	byte	m_bMoveSnd;			// sound a plat makes while moving
	byte	m_bStopSnd;			// sound a plat makes when it stops
	float	m_volume;			// Sound volume
};

#endif //GAME_SERVER_ENTITIES_PLATS_CBASEPLATTRAIN_H