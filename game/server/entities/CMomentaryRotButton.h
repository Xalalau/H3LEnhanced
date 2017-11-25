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
#ifndef GAME_SERVER_CMOMENTARYROTBUTTON_H
#define GAME_SERVER_CMOMENTARYROTBUTTON_H

// Make this button behave like a door (HACKHACK)
// This will disable use and make the button solid
// rotating buttons were made SOLID_NOT by default since their were some
// collision problems with them...
#define SF_MOMENTARY_DOOR		0x0001

class CMomentaryRotButton : public CBaseToggle
{
public:
	DECLARE_CLASS( CMomentaryRotButton, CBaseToggle );
	DECLARE_DATADESC();

	void	Spawn( void ) override;
	void	KeyValue( KeyValueData *pkvd ) override;
	virtual int	ObjectCaps() const override
	{
		int flags = CBaseToggle::ObjectCaps() & ( ~FCAP_ACROSS_TRANSITION );
		if( GetSpawnFlags().Any( SF_MOMENTARY_DOOR ) )
			return flags;
		return flags | FCAP_CONTINUOUS_USE;
	}
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void	Off( void );
	void	Return( void );
	void	UpdateSelf( float value );
	void	UpdateSelfReturn( float value );
	void	UpdateAllButtons( float value, int start );

	void	PlaySound( void );
	void	UpdateTarget( float value );

	static CMomentaryRotButton *Instance( edict_t *pent ) { return ( CMomentaryRotButton * ) GET_PRIVATE( pent ); }
	static CMomentaryRotButton *Instance( CBaseEntity* pEntity ) { return ( CMomentaryRotButton * ) pEntity; }

	int		m_lastUsed;
	int		m_direction;
	float	m_returnSpeed;
	Vector	m_start;
	Vector	m_end;
	int		m_sounds;
};

#endif //GAME_SERVER_CMOMENTARYROTBUTTON_H