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
#ifndef GAME_SERVER_ENTITIES_EFFECTS_CSPRITE_H
#define GAME_SERVER_ENTITIES_EFFECTS_CSPRITE_H

#define SF_SPRITE_STARTON		0x0001
#define SF_SPRITE_ONCE			0x0002
#define SF_SPRITE_TEMPORARY		0x8000

class CSprite : public CPointEntity
{
public:
	DECLARE_CLASS( CSprite, CPointEntity );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;

	int ObjectCaps() const override
	{
		int flags = 0;
		if( GetSpawnFlags().Any( SF_SPRITE_TEMPORARY ) )
			flags = FCAP_DONT_SAVE;
		return ( CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ) | flags;
	}
	void AnimateThink( void );
	void ExpandThink( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void Animate( float frames );
	void Expand( float scaleSpeed, float fadeSpeed );
	void SpriteInit( const char *pSpriteName, const Vector &origin );

	inline void SetAttachment( CBaseEntity* pEntity, int attachment )
	{
		if( pEntity )
		{
			SetSkin( pEntity->entindex() );
			SetBody( attachment );
			SetAimEntity( pEntity );
			SetMoveType( MOVETYPE_FOLLOW );
		}
	}
	void TurnOff( void );
	void TurnOn( void );
	inline float Frames( void ) { return m_maxFrame; }
	inline void SetTransparency( RenderMode rendermode, int r, int g, int b, int a, RenderFX fx )
	{
		SetRenderMode( rendermode );
		SetRenderColor( Vector( r, g, b ) );
		SetRenderAmount( a );
		SetRenderFX( fx );
	}
	inline void SetTexture( int spriteIndex ) { SetModelIndex( spriteIndex ); }
	inline void SetColor( int r, int g, int b ) { SetRenderColor( Vector( r, g, b ) ); }
	inline void SetBrightness( int brightness ) { SetRenderAmount( brightness ); }

	inline void AnimateAndDie( float framerate )
	{
		SetThink( &CSprite::AnimateUntilDead );
		SetFrameRate( framerate );
		SetDamageTime( gpGlobals->time + ( m_maxFrame / framerate ) );
		SetNextThink( gpGlobals->time );
	}

	void AnimateUntilDead( void );

	static CSprite *SpriteCreate( const char *pSpriteName, const Vector &origin, const bool animate );

private:

	float		m_lastTime;
	float		m_maxFrame;
};

#endif //GAME_SERVER_ENTITIES_EFFECTS_CSPRITE_H