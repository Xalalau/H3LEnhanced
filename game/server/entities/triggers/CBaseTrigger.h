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
#ifndef GAME_SERVER_ENTITIES_TRIGGERS_CBASETRIGGER_H
#define GAME_SERVER_ENTITIES_TRIGGERS_CBASETRIGGER_H

enum
{	
	SPAWNFLAG_NOMESSAGE			= 1,

	/**
	*	Monsters allowed to fire this trigger.
	*/
	SF_TRIGGER_ALLOWMONSTERS	= 1,

	/**
	*	Players not allowed to fire this trigger.
	*/
	SF_TRIGGER_NOCLIENTS		= 2,

	/**
	*	Only pushables can fire this trigger.
	*/
	SF_TRIGGER_PUSHABLES		= 4,
};

class CBaseTrigger : public CBaseToggle
{
public:
	DECLARE_CLASS( CBaseTrigger, CBaseToggle );
	DECLARE_DATADESC();

	void TeleportTouch( CBaseEntity *pOther );
	void KeyValue( KeyValueData *pkvd ) override;
	void MultiTouch( CBaseEntity *pOther );
	void ActivateMultiTrigger( CBaseEntity *pActivator );
	void MultiWaitOver();
	void CounterUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void InitTrigger();

	virtual int	ObjectCaps() const override { return CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	// ############ hu3lifezado ############ //
	// [MODO COOP]
	// Se for true, esse teletransporte deve funcionar com todos os jogadores de uma vez
	bool teleport_all_coop;
	// ############ //
};

#endif //GAME_SERVER_ENTITIES_TRIGGERS_CBASETRIGGER_H