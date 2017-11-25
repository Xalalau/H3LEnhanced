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
#ifndef GAME_SERVER_CMULTIMANAGER_H
#define GAME_SERVER_CMULTIMANAGER_H

//**********************************************************
// The Multimanager Entity - when fired, will fire up to 16 targets 
// at specified times.
// FLAG:		THREAD (create clones when triggered)
// FLAG:		CLONE (this is a clone for a threaded execution)

#define MAX_MULTI_TARGETS	16 // maximum number of targets a single multi_manager entity may be assigned.

#define SF_MULTIMAN_CLONE		0x80000000
#define SF_MULTIMAN_THREAD		0x00000001

class CMultiManager : public CBaseToggle
{
public:
	DECLARE_CLASS( CMultiManager, CBaseToggle );
	DECLARE_DATADESC();

	void KeyValue( KeyValueData *pkvd ) override;
	void Spawn( void ) override;
	void ManagerThink( void );
	void ManagerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

#if _DEBUG
	void ManagerReport( void );
#endif

	bool		HasTarget( string_t targetname ) const override;

	int ObjectCaps() const override { return CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	int		m_cTargets;	// the total number of targets in this manager's fire list.
	int		m_index;	// Current target
	float	m_startTime;// Time we started firing
	int		m_iTargetName[ MAX_MULTI_TARGETS ];// list if indexes into global string array
	float	m_flTargetDelay[ MAX_MULTI_TARGETS ];// delay (in seconds) from time of manager fire to target fire
private:
	inline bool IsClone() const { return GetSpawnFlags().Any( SF_MULTIMAN_CLONE ); }
	inline bool ShouldClone() const
	{
		if( IsClone() )
			return false;

		return GetSpawnFlags().Any( SF_MULTIMAN_THREAD );
	}

	CMultiManager *Clone( void );
};

#endif //GAME_SERVER_CMULTIMANAGER_H