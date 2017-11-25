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
#ifndef GAME_SERVER_ENTITIES_EFFECTS_CLIGHTNING_H
#define GAME_SERVER_ENTITIES_EFFECTS_CLIGHTNING_H

class CLightning : public CBeam
{
public:
	DECLARE_CLASS( CLightning, CBeam );
	DECLARE_DATADESC();

	void	Spawn( void ) override;
	void	Precache( void ) override;
	void	KeyValue( KeyValueData *pkvd ) override;
	void	Activate( void ) override;

	void	StrikeThink( void );
	void	DamageThink( void );
	void	RandomArea( void );
	void	RandomPoint( const Vector &vecSrc );
	void	Zap( const Vector &vecSrc, const Vector &vecDest );
	void	StrikeUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	inline bool ServerSide() const
	{
		if( m_life == 0 && !GetSpawnFlags().Any( SF_BEAM_RING ) )
			return true;
		return false;
	}

	void	BeamUpdateVars( void );

	bool		m_active;
	int		m_iszStartEntity;
	int		m_iszEndEntity;
	float	m_life;
	int		m_boltWidth;
	int		m_noiseAmplitude;
	int		m_brightness;
	int		m_speed;
	float	m_restrike;
	int		m_spriteTexture;
	int		m_iszSpriteName;
	int		m_frameStart;

	float	m_radius;
};

#endif //GAME_SERVER_ENTITIES_EFFECTS_CLIGHTNING_H