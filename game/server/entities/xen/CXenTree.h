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
#ifndef GAME_SERVER_ENTITIES_XEN_CXENTREE_H
#define GAME_SERVER_ENTITIES_XEN_CXENTREE_H

#include "entities/CActAnimating.h"

#define TREE_AE_ATTACK		1

class CXenTree : public CActAnimating
{
public:
	DECLARE_CLASS( CXenTree, CActAnimating );
	DECLARE_DATADESC();

	void		Spawn( void ) override;
	void		Precache( void ) override;
	void		Touch( CBaseEntity *pOther ) override;
	void		Think( void ) override;
	void		OnTakeDamage( const CTakeDamageInfo& info ) override { Attack(); }
	void		HandleAnimEvent( AnimEvent_t& event ) override;
	void		Attack( void );
	EntityClassification_t GetClassification() override { return EntityClassifications().GetNoneId(); }

	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];

private:
	CXenTreeTrigger	*m_pTrigger;
};

#endif //GAME_SERVER_ENTITIES_XEN_CXENTREE_H