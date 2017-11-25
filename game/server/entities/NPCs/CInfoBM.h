/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#ifndef GAME_SERVER_ENTITIES_NPCS_CINFOBM_H
#define GAME_SERVER_ENTITIES_NPCS_CINFOBM_H

#define SF_INFOBM_RUN		0x0001
#define SF_INFOBM_WAIT		0x0002

// AI Nodes for Big Momma
class CInfoBM : public CPointEntity
{
public:
	DECLARE_CLASS( CInfoBM, CPointEntity );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void KeyValue( KeyValueData* pkvd ) override;

	//TODO: use actual vars - Solokiller
	// name in GetTargetname()
	// next in GetTarget()
	// radius in GetScale()
	// health in GetHealth()
	// Reach target in GetMessage()
	// Reach delay in GetSpeed()
	// Reach sequence in GetNetname()

	int		m_preSequence;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CINFOBM_H