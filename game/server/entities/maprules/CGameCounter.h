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
#ifndef GAME_SERVER_ENTITIES_MAPRULES_CGAMECOUNTER_H
#define GAME_SERVER_ENTITIES_MAPRULES_CGAMECOUNTER_H

#include "CRulePointEntity.h"

#define SF_GAMECOUNT_FIREONCE			0x0001
#define SF_GAMECOUNT_RESET				0x0002

//
// CGameCounter / game_counter	-- Counts events and fires target
// Flag: Fire once
// Flag: Reset on Fire
class CGameCounter : public CRulePointEntity
{
public:
	DECLARE_CLASS( CGameCounter, CRulePointEntity );

	void		Spawn() override;
	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	inline bool RemoveOnFire() const { return GetSpawnFlags().Any( SF_GAMECOUNT_FIREONCE ); }
	inline bool ResetOnFire() const { return GetSpawnFlags().Any( SF_GAMECOUNT_RESET ); }

	inline void CountUp() { SetFrags( GetFrags() + 1 ); }
	inline void CountDown() { SetFrags( GetFrags() - 1 ); }
	inline void ResetCount() { SetFrags( GetDamage() ); }
	inline int  CountValue() const { return GetFrags(); }
	inline int	LimitValue() const { return GetHealth(); }

	inline bool HitLimit() const { return CountValue() == LimitValue(); }

private:

	inline void SetCountValue( int value ) { SetFrags( value ); }
	inline void SetInitialValue( int value ) { SetDamage( value ); }
};

#endif //GAME_SERVER_ENTITIES_MAPRULES_CGAMECOUNTER_H