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
#ifndef GAME_SERVER_ENTITIES_EFFECTS_CENVBEVERAGE_H
#define GAME_SERVER_ENTITIES_EFFECTS_CENVBEVERAGE_H

//=========================================================
// Beverage Dispenser
// overloaded GetFrags(), is now a flag for whether or not a can is stuck in the dispenser. 
// overloaded GetHealth(), is now how many cans remain in the machine.
//=========================================================
class CEnvBeverage : public CBaseDelay
{
public:
	DECLARE_CLASS( CEnvBeverage, CBaseDelay );

	void	Spawn( void ) override;
	void	Precache( void ) override;
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
};

#endif //GAME_SERVER_ENTITIES_EFFECTS_CENVBEVERAGE_H