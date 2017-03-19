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
#ifndef GAME_SERVER_CPARENTDOOR_H
#define GAME_SERVER_CPARENTDOOR_H

/*QUAKED func_door (0 .5 .8) ? START_OPEN x DOOR_DONT_LINK TOGGLE
if two doors touch, they are assumed to be connected and operate as a unit.

TOGGLE causes the door to wait in both the start and end states for a trigger event.

START_OPEN causes the door to move to its destination when spawned, and operate in reverse.
It is used to temporarily or permanently close off an area when triggered (not usefull for
touch or takedamage doors).

"angle"         determines the opening direction
"targetname"	if set, no touch field will be spawned and a remote button or trigger
field activates the door.
"health"        if set, door must be shot open
"speed"         movement speed (100 default)
"wait"          wait before returning (3 default, -1 = never return)
"lip"           lip remaining at end of move (8 default)
"dmg"           damage to inflict when blocked (2 default)
"sounds"
0)      no sound
1)      stone
2)      base
3)      stone chain
4)      screechy metal
*/
class CParentDoor : public CBaseDoor
{
public:
	DECLARE_CLASS(CParentDoor, CBaseDoor);
	DECLARE_DATADESC();

	void Spawn(void) override;
	virtual void KeyValue(KeyValueData *pkvd) override;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;

	entvars_t * SetEntVars_t(string_t parent01_nome);
	void PrepareModel();
	void ProcessMovement(entvars_t *parent, string_t targetName);

	float speed;

	Vector m_vecPosition2Backup;

	entvars_t *door;

	Vector destination;

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

#endif //GAME_SERVER_CPARENTDOOR_H