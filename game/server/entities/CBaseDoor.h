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
#ifndef GAME_SERVER_CBASEDOOR_H
#define GAME_SERVER_CBASEDOOR_H

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
class CBaseDoor : public CBaseToggle
{
public:
	DECLARE_CLASS( CBaseDoor, CBaseToggle );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	virtual void KeyValue( KeyValueData *pkvd ) override;
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	virtual void Blocked( CBaseEntity *pOther ) override;


	virtual int	ObjectCaps() const override
	{
		if( GetSpawnFlags().Any( SF_ITEM_USE_ONLY ) )
			return ( CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ) | FCAP_IMPULSE_USE;
		else
			return ( CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION );
	}

	virtual void SetToggleState( int state ) override;

	// used to selectivly override defaults
	void DoorTouch( CBaseEntity *pOther );

	// local functions
	int DoorActivate();
	void DoorGoUp( void );
	void DoorGoDown( void );
	void DoorHitTop( void );
	void DoorHitBottom( void );

	byte	m_bHealthValue;// some doors are medi-kit doors, they give players health

	byte	m_bMoveSnd;			// sound a door makes while moving
	byte	m_bStopSnd;			// sound a door makes when it stops

	locksound_t m_ls;			// door lock sounds

	byte	m_bLockedSound;		// ordinals from entity selection
	byte	m_bLockedSentence;
	byte	m_bUnlockedSound;
	byte	m_bUnlockedSentence;
};

#define noiseMoving noise1
#define noiseArrived noise2

#endif //GAME_SERVER_CBASEDOOR_H