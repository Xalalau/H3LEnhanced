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
#ifndef GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERCHANGETARGET_H
#define GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERCHANGETARGET_H

// this is a really bad idea.
class CTriggerChangeTarget : public CBaseDelay
{
public:
	DECLARE_CLASS( CTriggerChangeTarget, CBaseDelay );
	DECLARE_DATADESC();

	void KeyValue( KeyValueData *pkvd ) override;
	void Spawn() override;
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;

	int ObjectCaps() const override { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

private:
	string_t m_iszNewTarget;
};

#endif //GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERCHANGETARGET_H