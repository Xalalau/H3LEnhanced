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
#ifndef GAME_SERVER_ENTITIES_EFFECTS_CBEAM_H
#define GAME_SERVER_ENTITIES_EFFECTS_CBEAM_H

#define SF_BEAM_STARTON			0x0001
#define SF_BEAM_TOGGLE			0x0002
#define SF_BEAM_RANDOM			0x0004
#define SF_BEAM_RING			0x0008
#define SF_BEAM_SPARKSTART		0x0010
#define SF_BEAM_SPARKEND		0x0020
#define SF_BEAM_DECALS			0x0040
#define SF_BEAM_SHADEIN			0x0080
#define SF_BEAM_SHADEOUT		0x0100
#define SF_BEAM_TEMPORARY		0x8000

class CBeam : public CBaseEntity
{
public:
	DECLARE_CLASS( CBeam, CBaseEntity );
	DECLARE_DATADESC();

	void	Spawn( void ) override;
	void	Precache( void ) override;
	int ObjectCaps() const override
	{
		int flags = 0;
		if( GetSpawnFlags().Any( SF_BEAM_TEMPORARY ) )
			flags = FCAP_DONT_SAVE;
		return ( CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ) | flags;
	}

	void TriggerTouch( CBaseEntity *pOther );

	//TODO: try to remove the repurposing of unrelated variables, this might be possible using another way - Solokiller

	// These functions are here to show the way beams are encoded as entities.
	// Encoding beams as entities simplifies their management in the client/server architecture
	inline void	SetType( int type ) { SetRenderMode( static_cast<RenderMode>( ( static_cast<int>( GetRenderMode() ) & 0xF0 ) | ( type & 0x0F ) ) ); }
	inline void	SetBeamFlags( int flags ) { SetRenderMode( static_cast<RenderMode>( ( static_cast<int>( GetRenderMode() ) & 0x0F ) | ( flags & 0xF0 ) ) ); }
	//TODO: can't use SetAbsOrigin since it calls into the engine, but can't keep pev exposed either - Solokiller
	inline void SetStartPos( const Vector& pos ) { pev->origin = pos; }
	inline void SetEndPos( const Vector& pos ) { SetAbsAngles( pos ); }
	void SetStartEntity( int entityIndex );
	void SetEndEntity( int entityIndex );

	inline void SetStartAttachment( int attachment ) { SetSequence( ( GetSequence() & 0x0FFF ) | ( ( attachment & 0xF ) << 12 ) ); }
	inline void SetEndAttachment( int attachment ) { SetSkin( ( GetSkin() & 0x0FFF ) | ( ( attachment & 0xF ) << 12 ) ); }

	inline void SetTexture( int spriteIndex ) { SetModelIndex( spriteIndex ); }
	inline void SetWidth( int width ) { SetScale( width ); }
	inline void SetNoise( int amplitude ) { SetBody( amplitude ); }
	inline void SetColor( int r, int g, int b ) { SetRenderColor( Vector( r, g, b ) ); }
	inline void SetBrightness( int brightness ) { SetRenderAmount( brightness ); }
	inline void SetScrollRate( int speed ) { SetAnimTime( speed ); }

	inline int	GetType() const { return static_cast<int>( GetRenderMode() ) & 0x0F; }
	inline int	GetBeamFlags() const { return static_cast<int>( GetRenderMode() ) & 0xF0; }
	inline int	GetStartEntity() const { return GetSequence() & 0xFFF; }
	inline int	GetEndEntity() const { return GetSkin() & 0xFFF; }

	const Vector& GetStartPos() const;
	const Vector& GetEndPos() const;

	Vector Center() const override { return ( GetStartPos() + GetEndPos() ) * 0.5; } // center point of beam

	inline int  GetTexture( void ) { return GetModelIndex(); }
	inline int  GetWidth( void ) { return GetScale(); }
	inline int  GetNoise( void ) { return GetBody(); }
	//TODO: this method is completely wrong, don't uncomment without fixing! - Solokiller
	// inline void GetColor( int r, int g, int b ) { SetRenderColor( Vector( r, g, b ) ); }
	inline int  GetBrightness( void ) { return GetRenderAmount(); }
	inline int  GetScrollRate( void ) { return GetAnimTime(); }

	// Call after you change start/end positions
	void		RelinkBeam( void );
	//	void		SetObjectCollisionBox( void );

	void		DoSparks( const Vector &start, const Vector &end );
	void		BeamDamage( TraceResult *ptr );
	// Init after BeamCreate()
	void		BeamInit( const char *pSpriteName, int width );
	void		PointsInit( const Vector &start, const Vector &end );
	void		PointEntInit( const Vector &start, int endIndex );
	void		EntsInit( int startIndex, int endIndex );
	void		HoseInit( const Vector &start, const Vector &direction );

	static CBeam *BeamCreate( const char *pSpriteName, int width );

	inline void LiveForTime( float time ) { SetThink( &CBeam::SUB_Remove ); SetNextThink( gpGlobals->time + time ); }
	inline void	BeamDamageInstant( TraceResult *ptr, float damage )
	{
		SetDamage( damage );
		SetDamageTime( gpGlobals->time - 1 );
		BeamDamage( ptr );
	}
};

#endif //GAME_SERVER_ENTITIES_EFFECTS_CBEAM_H