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
#ifndef GAME_SERVER_CPOINTCMD_H
#define GAME_SERVER_CPOINTCMD_H

class CPointCMD : public CPointEntity
{
public:
	DECLARE_CLASS( CPointCMD, CPointEntity );
	DECLARE_DATADESC();

	void Spawn(void);

	void KeyValue(KeyValueData *pkvd);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void Think(void);
	void RunCommand(void);

	string_t m_Command;		// command to execute
};

#endif //GAME_SERVER_CPOINTCMD_H