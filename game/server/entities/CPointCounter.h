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

#ifndef GAME_SERVER_CPOINTCOUNTER_H
#define GAME_SERVER_CPOINTCOUNTER_H

class CPointCounter : public CPointEntity
{
public:
	DECLARE_CLASS(CPointCounter, CPointEntity);
	DECLARE_DATADESC();

	void Spawn(void);
	void KeyValue(KeyValueData *pkvd) override;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	int m_iCount;
	int m_iCountTotal;
	string_t m_sMessageTitle;
	string_t m_sMessageFinished;

private:
	hudtextparms_t m_sCountText;
};

#endif //GAME_SERVER_CPOINTCOUNTER_H