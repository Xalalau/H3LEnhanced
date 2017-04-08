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
#ifndef GAME_SERVER_ENTITIES_NPCS_CGMAN_H
#define GAME_SERVER_ENTITIES_NPCS_CGMAN_H

//=========================================================
// Monster's Anim Events Go Here
//=========================================================

class CGMan : public CBaseMonster
{
public:
	DECLARE_CLASS( CGMan, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetYawSpeed( void ) override;
	EntityClassification_t GetClassification() override;
	void HandleAnimEvent( AnimEvent_t& event ) override;
	int ISoundMask( void ) override;

	void StartTask( const Task_t* pTask ) override;
	void RunTask( const Task_t* pTask ) override;
	void OnTakeDamage( const CTakeDamageInfo& info ) override;
	void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr ) override;

	void PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, const bool bConcurrent, CBaseEntity *pListener ) override;

	EHANDLE m_hPlayer;
	EHANDLE m_hTalkTarget;
	float m_flTalkTime;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CGMAN_H