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
#ifndef GAME_SERVER_ENTITIES_PLATS_CFUNCTRACKTRAIN_H
#define GAME_SERVER_ENTITIES_PLATS_CFUNCTRACKTRAIN_H

// Tracktrain spawn flags
#define SF_TRACKTRAIN_NOPITCH		0x0001
#define SF_TRACKTRAIN_NOCONTROL		0x0002
#define SF_TRACKTRAIN_FORWARDONLY	0x0004
#define SF_TRACKTRAIN_PASSABLE		0x0008

#define TRAIN_STARTPITCH	60
#define TRAIN_MAXPITCH		200
#define TRAIN_MAXSPEED		1000	// approx max speed for sound pitch calculation

class CPathTrack;

/*QUAKED func_train (0 .5 .8) ?
Trains are moving platforms that players can ride.
The targets origin specifies the min point of the train at each corner.
The train spawns at the first target it is pointing at.
If the train is the target of a button or trigger, it will not begin moving until activated.
speed	default 100
dmg		default	2
sounds
1) ratchet metal
*/
class CFuncTrackTrain : public CBaseEntity
{
public:
	DECLARE_CLASS( CFuncTrackTrain, CBaseEntity );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;

	void Blocked( CBaseEntity *pOther ) override;
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void KeyValue( KeyValueData* pkvd ) override;

	void Next( void );
	void Find( void );
	void NearestPath( void );
	void DeadEnd( void );

	void		NextThink( float thinkTime, const bool alwaysThink );

	void SetTrack( CPathTrack *track );
	void SetControls( CBaseEntity* pControls );
	bool OnControls( const CBaseEntity* const pTest ) const override;

	void StopSound( void );
	void UpdateSound( void );

	static CFuncTrackTrain* Instance( CBaseEntity* pEntity );

	virtual int	ObjectCaps() const override { return ( CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ) | FCAP_DIRECTIONAL_USE; }

	virtual void	OverrideReset( void ) override;


	// ############ hu3lifezado ############ //
	// [MODO COOP]
	// Um pequeno pulo extra
	void PreNextHu3(void);
	// ############ //

	CPathTrack	*m_ppath;
	float		m_length;
	float		m_height;
	float		m_speed;
	float		m_dir;
	float		m_startSpeed;
	Vector		m_controlMins;
	Vector		m_controlMaxs;
	int			m_soundPlaying;
	int			m_sounds;
	float		m_flVolume;
	float		m_flBank;
	float		m_oldSpeed;

private:
	unsigned short m_usAdjustPitch;
};

#endif //GAME_SERVER_ENTITIES_PLATS_CFUNCTRACKTRAIN_H