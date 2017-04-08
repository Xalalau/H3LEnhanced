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
#ifndef GAME_SERVER_ENTITIES_WEAPONS_CSQUEAKGRENADE_H
#define GAME_SERVER_ENTITIES_WEAPONS_CSQUEAKGRENADE_H

//TODO: correct name - Solokiller
#define SQUEEK_DETONATE_DELAY	15.0

class CSqueakGrenade : public CGrenade
{
public:
	DECLARE_CLASS( CSqueakGrenade, CGrenade );
	DECLARE_DATADESC();

	void Spawn() override;
	void Precache() override;
	EntityClassification_t GetClassification() override;
	void SuperBounceTouch( CBaseEntity *pOther );
	void HuntThink();
	int BloodColor() const override { return BLOOD_COLOR_YELLOW; }
	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;
	void GibMonster() override;

	static float m_flNextBounceSoundTime;

	// CBaseEntity *m_pTarget;
	float m_flDie;
	Vector m_vecTarget;
	float m_flNextHunt;
	float m_flNextHit;
	Vector m_posPrev;
	EHANDLE m_hOwner;
	EntityClassification_t  m_iMyClass = EntityClassifications().GetNoneId();
};

#endif //GAME_SERVER_ENTITIES_WEAPONS_CSQUEAKGRENADE_H
