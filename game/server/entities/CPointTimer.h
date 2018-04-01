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

/*
	Hu3-Life Entity System

	point_timer
	Temporizador de disparo de entidades

	by: NickMBR Mar/2018
*/

#ifndef GAME_SERVER_CPOINTTIMER_H
#define GAME_SERVER_CPOINTTIMER_H

class CPointTimer : public CPointEntity
{
public:
	DECLARE_CLASS(CPointTimer, CPointEntity);
	DECLARE_DATADESC();

	void Spawn(void);
	void KeyValue(KeyValueData *pkvd) override;

	void TimerThink(void);
	void TimerStart(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	int m_iDuration;
	int m_iRepeat;
	float m_flendTime;
	CBaseEntity* e_Target;
};

#endif //GAME_SERVER_CPOINTTIMER_H