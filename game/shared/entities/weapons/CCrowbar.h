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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CCROWBAR_H
#define GAME_SHARED_ENTITIES_WEAPONS_CCROWBAR_H

enum crowbar_e
{
	CROWBAR_IDLE = 0,
	CROWBAR_DRAW,
	CROWBAR_HOLSTER,
	CROWBAR_ATTACK1HIT,
	CROWBAR_ATTACK1MISS,
	CROWBAR_ATTACK2MISS,
	CROWBAR_ATTACK2HIT,
	CROWBAR_ATTACK3MISS,
	CROWBAR_ATTACK3HIT
};

class CCrowbar : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CCrowbar, CBasePlayerWeapon );
	DECLARE_DATADESC();

	CCrowbar();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SwingAgain( void );
	void Smack( void );

	void PrimaryAttack( void ) override;

	// ############ hu3lifezado ############ //
	// Tiro secundario, adaptado de: 
	// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
	void SecondaryAttack(void);
	// ############ //

	bool Swing( const bool bFirst );
	bool Deploy() override;
	void Holster() override;
	int m_iSwing;
	TraceResult m_trHit;

private:
	unsigned short m_usCrowbar;
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CCROWBAR_H