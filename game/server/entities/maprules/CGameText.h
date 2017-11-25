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
#ifndef GAME_SERVER_ENTITIES_MAPRULES_CGAMETEXT_H
#define GAME_SERVER_ENTITIES_MAPRULES_CGAMETEXT_H

#include "CRulePointEntity.h"

#define SF_ENVTEXT_ALLPLAYERS			0x0001

//
// CGameText / game_text	-- NON-Localized HUD Message (use env_message to display a titles.txt message)
//	Flag: All players					SF_ENVTEXT_ALLPLAYERS
//
class CGameText : public CRulePointEntity
{
public:
	DECLARE_CLASS( CGameText, CRulePointEntity );
	DECLARE_DATADESC();

	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void	KeyValue( KeyValueData *pkvd ) override;

	inline	bool	MessageToAll() const { return GetSpawnFlags().Any( SF_ENVTEXT_ALLPLAYERS ); }
	inline	void	MessageSet( const char *pMessage ) { SetMessage( ALLOC_STRING( pMessage ) ); }
	inline	const char *MessageGet() const { return GetMessage(); }

private:

	hudtextparms_t	m_textParms;
};

#endif //GAME_SERVER_ENTITIES_MAPRULES_CGAMETEXT_H