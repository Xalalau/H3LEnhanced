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
#ifndef GAME_SERVER_ENTITIES_WEAPONS_CCROSSBOWBOLT_H
#define GAME_SERVER_ENTITIES_WEAPONS_CCROSSBOWBOLT_H

#define BOLT_AIR_VELOCITY	2000
#define BOLT_WATER_VELOCITY	1000

// UNDONE: Save/restore this?  Don't forget to set classname and LINK_ENTITY_TO_CLASS()
// 
// OVERLOADS SOME ENTVARS:
//
// speed - the ideal magnitude of my velocity
class CCrossbowBolt : public CBaseEntity
{
public:
	DECLARE_CLASS( CCrossbowBolt, CBaseEntity );
	DECLARE_DATADESC();

	void Spawn() override;
	void Precache() override;
	EntityClassification_t GetClassification() override;
	void BubbleThink();
	void BoltTouch( CBaseEntity *pOther );
	void ExplodeThink();

	int m_iTrail;

public:
	static CCrossbowBolt* BoltCreate();
};

#endif //GAME_SERVER_ENTITIES_WEAPONS_CCROSSBOWBOLT_H
