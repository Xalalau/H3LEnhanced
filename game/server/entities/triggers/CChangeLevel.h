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
#ifndef GAME_SERVER_ENTITIES_TRIGGERS_CCHANGELEVEL_H
#define GAME_SERVER_ENTITIES_TRIGGERS_CCHANGELEVEL_H

#include "CBaseTrigger.h"

#define SF_CHANGELEVEL_USEONLY		0x0002

/*QUAKED trigger_changelevel (0.5 0.5 0.5) ? NO_INTERMISSION
When the player touches this, he gets sent to the map listed in the "map" variable.  Unless the NO_INTERMISSION flag is set, the view will go to the info_intermission spot and display stats.
*/
class CChangeLevel : public CBaseTrigger
{
public:
	DECLARE_CLASS( CChangeLevel, CBaseTrigger );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void UseChangeLevel( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void ExecuteChangeLevel( void );
	void TouchChangeLevel( CBaseEntity *pOther );
	void ChangeLevelNow( CBaseEntity *pActivator );

	static CBaseEntity* FindLandmark( const char* const pszLandmarkName );
	static int ChangeList( LEVELLIST *pLevelList, int maxList );
	static int AddTransitionToList( LEVELLIST *pLevelList, int listCount, const char *pMapName, const char *pLandmarkName, CBaseEntity* pLandmark );
	static bool InTransitionVolume( CBaseEntity *pEntity, char *pVolumeName );

	char m_szMapName[ cchMapNameMost ];		// trigger_changelevel only:  next map
	char m_szLandmarkName[ cchMapNameMost ];		// trigger_changelevel only:  landmark on next map
	string_t m_changeTarget;
	float	m_changeTargetDelay;

private:
	// ############ hu3lifezado ############ //
	// Garante que todos os jogadores estao no mesmo trigger_changelevel
	static int hu3TriggerChangelevelActivated;
	// ############ //
};

#endif //GAME_SERVER_ENTITIES_TRIGGERS_CCHANGELEVEL_H