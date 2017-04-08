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
#ifndef GAME_SERVER_ENTITIES_XEN_CXENSPORE_H
#define GAME_SERVER_ENTITIES_XEN_CXENSPORE_H

#include "entities/CActAnimating.h"

// UNDONE:	These need to smoke somehow when they take damage
//			Touch behavior?
//			Cause damage in smoke area

//
// Spores
//
class CXenSpore : public CActAnimating
{
public:
	DECLARE_CLASS( CXenSpore, CActAnimating );

	void		Spawn( void ) override;
	void		Precache( void ) override;
	void		Touch( CBaseEntity *pOther ) override;
	void		Think( void ) override;
	void		OnTakeDamage( const CTakeDamageInfo& info ) override { Attack(); }
	//	void		HandleAnimEvent( AnimEvent_t& event );
	void		Attack( void ) {}

	static const char *pModelNames[];
};

class CXenSporeSmall : public CXenSpore
{
public:
	DECLARE_CLASS( CXenSporeSmall, CXenSpore );

	void		Spawn( void ) override;
};

class CXenSporeMed : public CXenSpore
{
public:
	DECLARE_CLASS( CXenSporeMed, CXenSpore );

	void		Spawn( void ) override;
};

class CXenSporeLarge : public CXenSpore
{
public:
	DECLARE_CLASS( CXenSporeLarge, CXenSpore );

	void		Spawn( void ) override;

	static const Vector m_hullSizes[];
};

// Fake collision box for big spores
class CXenHull : public CPointEntity
{
public:
	DECLARE_CLASS( CXenHull, CPointEntity );

	static CXenHull	*CreateHull( CBaseEntity *source, const Vector &mins, const Vector &maxs, const Vector &offset );
	EntityClassification_t GetClassification() override { return EntityClassifications().GetNoneId(); }
};

#endif //GAME_SERVER_ENTITIES_XEN_CXENSPORE_H