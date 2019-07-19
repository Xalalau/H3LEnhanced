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
#ifndef GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERCMD_H
#define GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERCMD_H

class CTriggerCMD : public CBaseTrigger
{
public:
	DECLARE_CLASS( CTriggerCMD, CBaseTrigger );
	DECLARE_DATADESC();

	void Spawn( void );

	void KeyValue( KeyValueData *pkvd );

	void Touch ( CBaseEntity *pOther );

	string_t m_command;		// Comando
	string_t m_target;      // Alvo(s)
};


#endif //GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERCMD_H