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
#ifndef GAME_SERVER_CBASEBUTTON_H
#define GAME_SERVER_CBASEBUTTON_H

#define SF_BUTTON_DONTMOVE		1
#define SF_ROTBUTTON_NOTSOLID	1
#define	SF_BUTTON_TOGGLE		32	// button stays pushed until reactivated
#define	SF_BUTTON_SPARK_IF_OFF	64	// button sparks in OFF state
#define SF_BUTTON_TOUCH_ONLY	256	// button only fires as a result of USE key.

/*QUAKED func_button (0 .5 .8) ?
When a button is touched, it moves some distance in the direction of it's angle,
triggers all of it's targets, waits some time, then returns to it's original position
where it can be triggered again.

"angle"		determines the opening direction
"target"	all entities with a matching targetname will be used
"speed"		override the default 40 speed
"wait"		override the default 1 second wait (-1 = never return)
"lip"		override the default 4 pixel lip remaining at end of move
"health"	if set, the button must be killed instead of touched
"sounds"
0) steam metal
1) wooden clunk
2) metallic click
3) in-out
*/
class CBaseButton : public CBaseToggle
{
public:
	DECLARE_CLASS( CBaseButton, CBaseToggle );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	virtual void Precache( void ) override;
	virtual void KeyValue( KeyValueData* pkvd ) override;

	void ButtonActivate();
	void ButtonTouch( CBaseEntity *pOther );
	void ButtonSpark( void );
	void TriggerAndWait( void );
	void ButtonReturn( void );
	void ButtonBackHome( void );
	void ButtonUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual void OnTakeDamage( const CTakeDamageInfo& info ) override;

	enum BUTTON_CODE { BUTTON_NOTHING, BUTTON_ACTIVATE, BUTTON_RETURN };
	BUTTON_CODE	ButtonResponseToTouch( void );

	// Buttons that don't take damage can be IMPULSE used
	virtual int	ObjectCaps() const override { return ( CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ) | ( ( GetTakeDamageMode() != DAMAGE_NO ) ? 0 : FCAP_IMPULSE_USE ); }

	bool	m_fStayPushed;	// button stays pushed in until touched again?
	bool	m_fRotating;		// a rotating button?  default is a sliding button.

	string_t m_strChangeTarget;	// if this field is not null, this is an index into the engine string array.
								// when this button is touched, it's target entity's TARGET field will be set
								// to the button's ChangeTarget. This allows you to make a func_train switch paths, etc.

	locksound_t m_ls;			// door lock sounds

	byte	m_bLockedSound;		// ordinals from entity selection
	byte	m_bLockedSentence;
	byte	m_bUnlockedSound;
	byte	m_bUnlockedSentence;
	int		m_sounds;
};

#endif //GAME_SERVER_CBASEBUTTON_H