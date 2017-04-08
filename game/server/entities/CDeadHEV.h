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
#ifndef GAME_SERVER_ENTITIES_CDEADHEV_H
#define GAME_SERVER_ENTITIES_CDEADHEV_H

//=========================================================
// Dead HEV suit prop
//=========================================================
class CDeadHEV : public CBaseMonster
{
public:
	DECLARE_CLASS( CDeadHEV, CBaseMonster );

	void Spawn( void ) override;
	//TODO: why is this human military? Shouldn't this be human passive like dead scientists? - Solokiller
	EntityClassification_t GetClassification() override { return EntityClassifications().GetClassificationId( classify::HUMAN_MILITARY ); }

	void KeyValue( KeyValueData *pkvd ) override;

	int	m_iPose;// which sequence to display	-- temporary, don't need to save
	static const char* const m_szPoses[ 4 ];
};

#endif //GAME_SERVER_ENTITIES_CDEADHEV_H