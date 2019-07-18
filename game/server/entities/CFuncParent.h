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
#ifndef GAME_SERVER_CFUNCPARENT_H
#define GAME_SERVER_CFUNCPARENT_H

class CFuncParent : public CBaseToggle
{
public:
	DECLARE_CLASS(CFuncParent, CBaseToggle);
	DECLARE_DATADESC();

	void Spawn(void) override;
	virtual void KeyValue(KeyValueData *pkvd) override;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;

	entvars_t * SetEntVars_t(string_t parent01_nome);
	entvars_t * ProcessMovement(entvars_t *parent, string_t targetName);
	void LinearMove(entvars_t *entity, Vector vecDest, float flSpeed);
	void LinearMoveDone(void);

	float speed;
	float wait;
	float parent_time;
	bool blockThink;

	string_t parent01_name;
	entvars_t *parent01;

	string_t parent02_name;
	entvars_t *parent02;

	string_t parent03_name;
	entvars_t *parent03;

	string_t parent04_name;
	entvars_t *parent04;

	string_t parent05_name;
	entvars_t *parent05;

	string_t parent06_name;
	entvars_t *parent06;

	string_t parent07_name;
	entvars_t *parent07;

	string_t parent08_name;
	entvars_t *parent08;

	string_t parent09_name;
	entvars_t *parent09;

	string_t parent10_name;
	entvars_t *parent10;

	string_t parent11_name;
	entvars_t *parent11;

	string_t parent12_name;
	entvars_t *parent12;

	string_t parent13_name;
	entvars_t *parent13;

	string_t parent14_name;
	entvars_t *parent14;

	string_t parent15_name;
	entvars_t *parent15;

	string_t parent16_name;
	entvars_t *parent16;
};

#define noiseMoving noise1
#define noiseArrived noise2

#endif //GAME_SERVER_CFUNCPARENT_H