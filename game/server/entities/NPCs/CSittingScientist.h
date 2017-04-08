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
#ifndef GAME_SERVER_ENTITIES_NPCS_CSITTINGCIENTIST_H
#define GAME_SERVER_ENTITIES_NPCS_CSITTINGCIENTIST_H

#include "CScientist.h"

// animation sequence aliases 
enum SITTING_ANIM
{
	SITTING_ANIM_sitlookleft,
	SITTING_ANIM_sitlookright,
	SITTING_ANIM_sitscared,
	SITTING_ANIM_sitting2,
	SITTING_ANIM_sitting3
};

//=========================================================
// Sitting Scientist PROP
//=========================================================
class CSittingScientist : public CScientist // kdb: changed from public CBaseMonster so he can speak
{
public:
	DECLARE_CLASS( CSittingScientist, CScientist );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void  Precache( void ) override;

	void SittingThink( void );
	EntityClassification_t GetClassification() override;

	virtual void SetAnswerQuestion( CTalkMonster *pSpeaker ) override;
	int FriendNumber( int arrayNumber ) const override;

	bool FIdleSpeak();
	int		m_baseSequence;
	int		m_headTurn;
	float	m_flResponseDelay;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CSITTINGCIENTIST_H