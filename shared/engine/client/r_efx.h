/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
#if !defined ( R_EFXH )
#define R_EFXH
#ifdef _WIN32
#pragma once
#endif

struct model_t;
struct particle_t;
struct pmtrace_t;
struct TEMPENTITY;

// particle_t
#if !defined( PARTICLEDEFH )  
#include "particledef.h"
#endif

// BEAM
#if !defined( BEAMDEFH )
#include "beamdef.h"
#endif

// dlight_t
#if !defined ( DLIGHTH )
#include "dlight.h"
#endif

// cl_entity_t
#if !defined( CL_ENTITYH )
#include "cl_entity.h"
#endif

/*
// FOR REFERENCE, These are the built-in tracer colors.  Note, color 4 is the one
//  that uses the tracerred/tracergreen/tracerblue and traceralpha cvar settings
color24 gTracerColors[] =
{
	{ 255, 255, 255 },		// White
	{ 255, 0, 0 },			// Red
	{ 0, 255, 0 },			// Green
	{ 0, 0, 255 },			// Blue
	{ 0, 0, 0 },			// Tracer default, filled in from cvars, etc.
	{ 255, 167, 17 },		// Yellow-orange sparks
	{ 255, 130, 90 },		// Yellowish streaks (garg)
	{ 55, 60, 144 },		// Blue egon streak
	{ 255, 130, 90 },		// More Yellowish streaks (garg)
	{ 255, 140, 90 },		// More Yellowish streaks (garg)
	{ 200, 130, 90 },		// More red streaks (garg)
	{ 255, 120, 70 },		// Darker red streaks (garg)
};
*/

/**
*	Temporary entity array
*/
enum TempEntPriority
{
	TENTPRIORITY_LOW	= 0,
	TENTPRIORITY_HIGH	= 1,
};

// TEMPENTITY flags
enum TempEntFlag
{
	FTENT_NONE				= 0x00000000,
	FTENT_SINEWAVE			= 0x00000001,
	FTENT_GRAVITY			= 0x00000002,
	FTENT_ROTATE			= 0x00000004,
	FTENT_SLOWGRAVITY		= 0x00000008,
	FTENT_SMOKETRAIL		= 0x00000010,
	FTENT_COLLIDEWORLD		= 0x00000020,
	FTENT_FLICKER			= 0x00000040,
	FTENT_FADEOUT			= 0x00000080,
	FTENT_SPRANIMATE		= 0x00000100,
	FTENT_HITSOUND			= 0x00000200,
	FTENT_SPIRAL			= 0x00000400,
	FTENT_SPRCYCLE			= 0x00000800,

	/**
	*	Will collide with world and slideboxes.
	*/
	FTENT_COLLIDEALL		= 0x00001000,

	/**
	*	Tent is not removed when unable to draw.
	*/
	FTENT_PERSIST			= 0x00002000,

	/**
	*	Tent is removed upon collision with anything.
	*/
	FTENT_COLLIDEKILL		= 0x00004000,

	/**
	*	Tent is attached to a player (owner).
	*/
	FTENT_PLYRATTACHMENT	= 0x00008000,

	/**
	*	Animating sprite doesn't die when last frame is displayed.
	*/
	FTENT_SPRANIMATELOOP	= 0x00010000,
	FTENT_SPARKSHOWER		= 0x00020000,

	/**
	*	Doesn't have a model, never try to draw ( it just triggers other things ).
	*/
	FTENT_NOMODEL			= 0x00040000,

	/**
	*	Must specify callback. Callback function is responsible for killing tempent and updating fields ( unless other flags specify how to do things ).
	*/
	FTENT_CLIENTCUSTOM		= 0x00080000, 
};

enum class RocketTrailType : int
{
	/**
	*	Rocket trail. Smoke with fire.
	*/
	ROCKET_TRAIL = 0,

	/**
	*	Smoke trail.
	*/
	SMOKE = 1,

	/**
	*	Trail of blood.
	*/
	BLOOD = 2,

	/**
	*	Creates 2 sets of yellow/green particles that move perpendicular to the plane defined by the start and end positions, to the left and right.
	*/
	PARTING_YELLOW_BLOOD = 3,

	/**
	*	Creates a stream of falling dark red blood. Only covers half the distance between the start and end points.
	*/
	FALLING_DARK_BLOOD = 4,

	/**
	*	Same movement as PARTING_YELLOW_BLOOD, but instead creates red and white particles. Similar to an airport landing strip.
	*	@see PARTING_YELLOW_BLOOD
	*/
	PARTING_RED_WHITE = 5,

	/**
	*	Creates a railgun trail. Smoke and fire particles.
	*/
	RAILGUN = 6,

	/**
	*	Creates a wave of fire particles moving towards the end position.
	*/
	FIRE_WAVE = 7,

	/**
	*	Flag used to indicate that a large number of particles should be used.
	*/
	LARGE_FLAG = 128,

	/**
	*	Large version of ROCKET_TRAIL.
	*	@see ROCKET_TRAIL
	*/
	ROCKET_TRAIL_LARGE = LARGE_FLAG | ROCKET_TRAIL,

	/**
	*	Large version of SMOKE.
	*	@see SMOKE
	*/
	SMOKE_LARGE = LARGE_FLAG | SMOKE,

	/**
	*	Large version of BLOOD.
	*	@see BLOOD
	*/
	BLOOD_LARGE = LARGE_FLAG | BLOOD,

	/**
	*	Large version of PARTING_YELLOW_BLOOD.
	*	@see PARTING_YELLOW_BLOOD
	*/
	PARTING_YELLOW_BLOOD_LARGE = LARGE_FLAG | PARTING_YELLOW_BLOOD,

	/**
	*	Large version of FALLING_DARK_BLOOD.
	*	May not work at all for larger distances.
	*	@see FALLING_DARK_BLOOD
	*/
	FALLING_DARK_BLOOD_LARGE = LARGE_FLAG | FALLING_DARK_BLOOD,

	/**
	*	Large version of PARTING_RED_WHITE.
	*	@see PARTING_RED_WHITE
	*/
	PARTING_RED_WHITE_LARGE = LARGE_FLAG | PARTING_RED_WHITE,

	/**
	*	Large version of RAILGUN.
	*	@see RAILGUN
	*/
	RAILGUN_LARGE = LARGE_FLAG | RAILGUN,

	//No large version for FIRE_WAVE since it's always large.
};

enum SpriteExplodeFlag
{
	FSPREXPL_NONE				= 0,

	/**
	*	Use normal render mode.
	*/
	FSPREXPL_USENORMALRENDER	= 1 << 0,
};

//These might not work properly.
enum class BeamCircleType
{
	CIRCLE		= 0,
	TORUS		= 0x13,
	DISK		= 0x14,
	CYLINDER	= 0x15,
	RING		= 0x18
};

using TempEntThink = void( * )( TEMPENTITY *ent, float frametime, float currenttime );

struct TEMPENTITY
{
	int			flags;
	float		die;
	float		frameMax;
	float		x;
	float		y;
	float		z;
	float		fadeSpeed;
	float		bounceFactor;
	int			hitSound;
	void		( *hitcallback )	( TEMPENTITY *ent, pmtrace_t *ptr );
	TempEntThink callback;
	TEMPENTITY	*next;
	int			priority;
	short		clientIndex;	// if attached, this is the index of the client to stick to
								// if COLLIDEALL, this is the index of the client to ignore
								// TENTS with FTENT_PLYRATTACHMENT MUST set the clientindex! 

	Vector		tentOffset;		// if attached, client origin + tentOffset = tent origin.
	cl_entity_t	entity;

	// baseline.origin		- velocity
	// baseline.renderamt	- starting fadeout intensity
	// baseline.angles		- angle velocity
};

struct efx_api_t
{
	/**
	*	Creates a new custom particle.
	*	@param callback Callback to set.
	*	@return Particle.
	*/
	particle_t  *( *R_AllocParticle )			( ParticleCallback callback );

	/**
	*	Creates an explosion of blob particles.
	*	@param vecOrigin Explosion origin.
	*/
	void		( *R_BlobExplosion )			( const Vector& vecOrigin );

	/**
	*	Creates blood particles.
	*	@param vecOrigin Origin.
	*	@param vecDir Direction.
	*	@param pcolor Particle color.
	*	@param speed Movement speed.
	*/
	void		( *R_Blood )					( const Vector& vecOrigin, const Vector& vecDir, int pcolor, int speed );

	/**
	*	Creates blood sprites.
	*	@param vecOrigin Origin.
	*	@param colorindex Color index in the base palette.
	*	@param modelIndex First sprite index.
	*	@param modelIndex2 Second sprite index.
	*	@param size Size of the sprites.
	*/
	void		( *R_BloodSprite )				( const Vector& vecOrigin, int colorindex, int modelIndex, int modelIndex2, float size );

	/**
	*	Creates a blood stream.
	*	@param vecOrigin Origin.
	*	@param vecDir Direction.
	*	@param pcolor Particle color.
	*	@param speed Movement speed.
	*/
	void		( *R_BloodStream )				( const Vector& vecOrigin, const Vector& vecDir, int pcolor, int speed );

	/**
	*	Creates a break model.
	*	@param vecOrigin Origin.
	*	@param vecSize Size of the box in which to spawn models.
	*	@param vecDir Direction.
	*	@param random Random spread.
	*	@param life How long the models should stick around.
	*	@param count Number of models to spawn.
	*	@param modelIndex Index of the model to spawn.
	*	@param flags Flags. TODO find constants - Solokliler
	*/
	void		( *R_BreakModel )				( const Vector& vecOrigin, const Vector& vecSize, const Vector& vecDir, float random, float life, int count, int modelIndex, char flags );

	/**
	*	Creates bubbles within a box.
	*	@param vecMins Minimum bounds.
	*	@param vecMaxs Maximum bounds.
	*	@param height How high the bubbles should float.
	*	@param modelIndex Index of the bubbles model to use.
	*	@param count Number of bubbles to spawn.
	*	@param speed Speed of the bubbles.
	*/
	void		( *R_Bubbles )					( const Vector& vecMins, const Vector& vecMaxs, float height, int modelIndex, int count, float speed );

	/**
	*	Creates a trail of bubbles between 2 points.
	*	@param vecStart Starting position.
	*	@param vecEnd End position.
	*	@param height How high the bubbles should float.
	*	@param modelIndex Index of the bubbles model to use.
	*	@param count Number of bubbles to spawn.
	*	@param speed Speed of the bubbles.
	*/
	void		( *R_BubbleTrail )				( const Vector& vecStart, const Vector& vecEnd, float height, int modelIndex, int count, float speed );

	/**
	*	Creates bullet impact particles.
	*	@param vecOrigin Origin.
	*/
	void		( *R_BulletImpactParticles )	( const Vector& vecOrigin );

	/**
	*	Creates particles around the given entity.
	*	@param ent Entity.
	*/
	void		( *R_EntityParticles )			( cl_entity_t* ent );

	/**
	*	Creates an explosion effect.
	*	@param vecOrigin Origin.
	*	@param model Index of the model to use.
	*	@param scale Scale of the explosion.
	*	@param framerate Frame rate.
	*	@param flags Flags. TODO figure out which flags apply - Solokiller
	*/
	void		( *R_Explosion )				( const Vector& vecOrigin, int model, float scale, float framerate, int flags );

	/**
	*	Does not appear to do anything.
	*	TODO: TE_FIZZ uses this. Check that as well. - Solokiller
	*/
	void		( *R_FizzEffect )				( cl_entity_t *pent, int modelIndex, int density );
	
	/**
	*	Creates a field of fire.
	*	@param vecOrigin Origin.
	*	@param radius Fire is made in a square around the origin. -radius, -radius to radius, radius.
	*	@param modelIndex Index of the model to spawn.
	*	@param count Number of models to spawn.
	*	@param flags Flags. @see TEFireFlag
	*	@param life How long the models should life for, in seconds.
	*/
	void		( *R_FireField ) 				( const Vector& vecOrigin, int radius, int modelIndex, int count, int flags, float life );

	/**
	*	Creates particles that flicker upwards, confetti style.
	*	@param vecOrigin Origin to spawn the particles at.
	*/
	void		( *R_FlickerParticles )			( const Vector& vecOrigin );

	/**
	*	Creates a funnel effect. Sprites moving along the surface of a cone that points downwards.
	*	@param vecOrigin Origin that the sprites should move towards or away from.
	*	@param modelIndex Index of the model to use.
	*	@param bReverse If false, the sprites move towards the origin. If true, the sprites move away from the origin.
	*/
	void		( *R_FunnelSprite )				( const Vector& vecOrigin, int modelIndex, int bReverse );

	/**
	*	Creates an implosion effect. Tracers moving towards a point.
	*	@param vecOrigin Point to move to.
	*	@param radius Radius around the point to spawn the tracers at.
	*	@param count Number of tracers.
	*	@param life How long the tracers should live for, in seconds.
	*/
	void		( *R_Implosion )				( const Vector& vecOrigin, float radius, int count, float life );

	/**
	*	Creates a large funnel of green particles.
	*	@param vecOrigin Origin.
	*	@param bReverse If false, particles move towards the origin. If true, particles move away from the origin.
	*	@see R_FunnelSprite
	*/
	void		( *R_LargeFunnel )				( const Vector& vecOrigin, int bReverse );

	/**
	*	Quake 1 lava splash. Creates red particles moving slightly upward.
	*	@param vecOrigin Origin to spawn the particles around.
	*/
	void		( *R_LavaSplash )				( const Vector& vecOrigin );

	/**
	*	Creates effects for multiple gunshots. Also plays ricochet sounds.
	*	@param vecOrigin Shoot origin.
	*	@param vecDir Direction to shoot in.
	*	@param vecNoise Noise.
	*	@param count Number of gunshots.
	*	@param decalCount Number of decals to use.
	*	@param pDecalIndices Array of decal indices to use.
	*/
	void		( *R_MultiGunshot )				( const Vector& vecOrigin, const Vector& vecDir, const Vector& vecNoise, int count, int decalCount, const int* pDecalIndices );

	/**
	*	Creates a muzzleflash sprite.
	*	@param vecOrigin Origin.
	*	@param type The scale and sprite to use.
	*				If not 0, calculates the scale as follows: ( type / 10 ) * 0.1
	*				Sprite is determined as follows: type % 10 % 3
	*				Maps to the following sprites:
	*				sprites/muzzleflash1.spr
	*				sprites/muzzleflash2.spr
	*				sprites/muzzleflash3.spr
	*/
	void		( *R_MuzzleFlash )				( const Vector& vecOrigin, int type );

	/**
	*	Creates particles in a box.
	*	@param vecMins Minimum bounds.
	*	@param vecMaxs Maximum bounds.
	*	@param r Red color. [ 0, 255 ].
	*	@param g Green color. [ 0, 255 ].
	*	@param b Blue color. [ 0, 255 ].
	*	@param life How long the particles should life for, in seconds.
	*/
	void		( *R_ParticleBox )				( const Vector& vecMins, const Vector& vecMaxs, unsigned char r, unsigned char g, unsigned char b, float life );

	/**
	*	Creates a burst of particles that move outward from the origin.
	*	Size has influence on speed. Will easily use up all available particles.
	*	@param vecOrigin Origin.
	*	@param size Size of the box that the particles should expand to.
	*	@param color Particle color.
	*	@param life How long the particles should live for, in seconds.
	*/
	void		( *R_ParticleBurst )			( const Vector& vecOrigin, int size, int color, float life );

	/**
	*	Creates yellowish particles around the origin that move upward.
	*	@param vecOrigin Origin.
	*/
	void		( *R_ParticleExplosion )		( const Vector& vecOrigin );

	/**
	*	Creates particles using the given color set around the origin that move upward.
	*	@param vecOrigin Origin.
	*	@param colorStart Index of the first color to use.
	*	@param colorLength Number of colors to use.
	*/
	void		( *R_ParticleExplosion2 )		( const Vector& vecOrigin, int colorStart, int colorLength );

	/**
	*	Creates a line of particles.
	*	@param vecStart Starting position.
	*	@param vecEnd End position.
	*	@param r Red color. [ 0, 255 ].
	*	@param g Green color. [ 0, 255 ].
	*	@param b Blue color. [ 0, 255 ].
	*	@param life How long the particles should live for, in seconds.
	*/
	void		( *R_ParticleLine )				( const Vector& vecStart, const Vector& vecEnd, unsigned char r, unsigned char g, unsigned char b, float life );

	/**
	*	Emits sprites from a player's bounding box (ONLY use for players!).
	*	@param client Client index. 1 based.
	*	@param modelIndex Index of the model to emit.
	*	@param count Number of sprites to emit.
	*	@param size Size of the sprites.
	*/
	void		( *R_PlayerSprites )			( int client, int modelIndex, int count, int size );

	/**
	*	Creates a projectile.
	*	@param vecOrigin Starting origin.
	*	@param vecVelocity Velocity.
	*	@param modelIndex Index of the model to use.
	*	@param life How long the projectile should live for, in seconds.
	*	@param owner If not 0, which entity should never be collided with. Must be a player.
	*	@param hitcallback Callback to invoke when it hits something.
	*	TODO use callback definition - Solokiller
	*/
	void		( *R_Projectile )				( const Vector& vecOrigin, const Vector& vecVelocity, int modelIndex, int life, int owner, void (*hitcallback)( TEMPENTITY *ent, pmtrace_t *ptr ) );
	
	/**
	*	Plays a ricochet sound at the given location.
	*	@param vecOrigin Origin.
	*/
	void		( *R_RicochetSound )			( const Vector& vecOrigin );

	/**
	*	Creates a ricochet sprite.
	*	@param vecOrigin Origin.
	*	@param pmodel Model to use.
	*	@param duration How long the sprite should life for, in seconds.
	*	@param scale Sprite scale.
	*/
	void		( *R_RicochetSprite )			( const Vector& vecOrigin, model_t* pmodel, float duration, float scale );

	/**
	*	Creates a rocket flare sprite at the given location. Will exist for 0.01 seconds.
	*	Uses the sprites/animglow01.spr sprite.
	*	@param vecOrigin Origin.
	*/
	void		( *R_RocketFlare )				( const Vector& vecOrigin );

	/**
	*	Creates a rocket trail.
	*	@param[ in, out ] vecStart Starting position. Modified to contain start + velocity.
	*	@param vecEnd End position.
	*	@param type Trail type.
	*	@see RocketTrailType
	*/
	void		( *R_RocketTrail )				( Vector& vecStart, const Vector& vecEnd, const RocketTrailType type );

	/**
	*	Particle effect that shows the appearance of running fast. See Team Fortress 2's Scout.
	*	@param vecOrigin Origin.
	*	@param vecDir Direction.
	*	@param color Particle color.
	*	@param count Number of particles.
	*/
	void		( *R_RunParticleEffect )		( const Vector& vecOrigin, const Vector& vecDir, int color, int count );

	/**
	*	Creates a line made up out of red particles.
	*	Lasts for 30 seconds.
	*	@param vecStart Starting position.
	*	@param vecEnd End position.
	*/
	void		( *R_ShowLine )					( const Vector& vecStart, const Vector& vecEnd );

	/**
	*	Creates a spark effect. Combines spark streaks and a ricochet sprite.
	*	@param vecOrigin Origin.
	*	@param count Number of sparks.
	*	@param velocityMin Minimum velocity.
	*	@param velocityMin Maximum velocity.
	*/
	void		( *R_SparkEffect )				( const Vector& vecOrigin, int count, int velocityMin, int velocityMax );

	/**
	*	Creates a shower of sparks.
	*	@param vecOrigin Origin.
	*/
	void		( *R_SparkShower )				( const Vector& vecOrigin );

	/**
	*	Creates spark streaks.
	*	@param vecOrigin Origin.
	*	@param count Number of sparks.
	*	@param velocityMin Minimum velocity.
	*	@param velocityMin Maximum velocity.
	*/
	void		( *R_SparkStreaks )				( const Vector& vecOrigin, int count, int velocityMin, int velocityMax );

	/**
	*	Sprays models out like a gib shooter.
	*	@param vecOrigin Origin.
	*	@param vecDir Direction.
	*	@param modelIndex Index of the model to spray.
	*	@param count Number of models to spray.
	*	@param speed Spray speed.
	*	@param spread Random spread.
	*	@param rendermode Render mode. @see RenderMode
	*/
	void		( *R_Spray )					( const Vector& vecOrigin, const Vector& vecDir, int modelIndex, int count, int speed, int spread, int rendermode );

	/**
	*	Sets the temp entity's parameters for an explosion sprite.
	*	@param pTemp Temporary entity.
	*	@param scale Scale.
	*	@param flags Flags. @see SpriteExplodeFlag
	*/
	void		( *R_Sprite_Explode )			( TEMPENTITY* pTemp, float scale, int flags );

	/**
	*	Sets the temp entity's parameters for a smoke sprite.
	*	Sets a dark gray color.
	*	@param pTemp Temporary entity.
	*	@param scale Scale.
	*/
	void		( *R_Sprite_Smoke )				( TEMPENTITY* pTemp, float scale );

	/**
	*	Variant of R_Spray that handles sprite properties automatically.
	*	Sets alpha test render mode.
	*	@param vecOrigin Origin.
	*	@param vecDir Direction.
	*	@param modelIndex Index of the model to spray.
	*	@param count Number of models to spray.
	*	@param speed Spray speed.
	*	@param iRand Random spread.
	*/
	void		( *R_Sprite_Spray )				( const Vector& vecOrigin, const Vector& vecDir, int modelIndex, int count, int speed, int iRand );

	/**
	*	Creates a sinusoidal wave of models between 2 points. The wave will fade out gradually from the start to end position.
	*	@param type Never used.
	*	@param vecStart Starting position.
	*	@param vecEnd End position.
	*	@param modelIndex Index of the model.
	*	@param count Number of models.
	*	@param life How long the models should live for, in seconds.
	*	@param size Scale.
	*	@param amplitude Wave amplitude.
	*	@param renderamt Render amount.
	*	@param speed Speed.
	*/
	void		( *R_Sprite_Trail )				( int type, const Vector& vecStart, const Vector& vecEnd, int modelIndex, int count, float life, float size, float amplitude, int renderamt, float speed );

	/**
	*	Sets the temp entity's parameters for a wall puff effect.
	*	Lasts for 0.01 seconds.
	*	@param pTemp Temporary entity.
	*	@param scale Scale.
	*/
	void		( *R_Sprite_WallPuff )			( TEMPENTITY* pTemp, float scale );

	/**
	*	Variant of R_SparkStreaks with more options.
	*	@param vecOrigin Origin.
	*	@param vecDir Direction.
	*	@param color Color.
	*	@param count Number of streaks.
	*	@param speed Streak speed.
	*	@param velocityMin Minimum velocity.
	*	@param velocityMax Maximum velocity.
	*/
	void		( *R_StreakSplash )				( const Vector& vecOrigin, const Vector& vecDir, int color, int count, float speed, int velocityMin, int velocityMax );

	/**
	*	Creates a tracer effect between the given positions. Moves from start to end.
	*	@param[ in, out ] Starting position. Will contain start + velocity.
	*	@param vecEnd End position.
	*/
	void		( *R_TracerEffect )				( Vector& vecStart, const Vector& vecEnd );

	/**
	*	@param vecOrigin Origin.
	*	@param vecVelocity Velocity.
	*	@param life How long the particle should live for, in seconds.
	*	@param colorIndex Particle color.
	*	@param length Length of the particle.
	*	@param deathcontext user defined context value.
	*	@param deathfunc Callback to invoke when the particle dies.
	*/
	void		( *R_UserTracerParticle )		( const Vector& vecOrigin, const Vector& vecVelocity, float life, int colorIndex, float length,
												  unsigned char deathcontext, void ( *deathfunc)( particle_t *particle ) );

	/**
	*	Creates tracer particles.
	*	@param vecOrigin Origin.
	*	@param vecVelocity Velocity.
	*	@param life How long the particle should live for, in seconds.
	*	@return Particle.
	*/
	particle_t *( *R_TracerParticles )			( const Vector& vecOrigin, const Vector& vecVelocity, float life );

	/**
	*	Creates a Quake 1 teleport splash effect.
	*/
	void		( *R_TeleportSplash )			( const Vector& vecOrigin );

	/**
	*	Creates particles and count temp entities with the given model that fall down.
	*	@param vecOrigin Origin.
	*	@param speed Speed.
	*	@param life How long the effect should live for, in seconds.
	*	@param count Number of temporary entities to create.
	*	@param modelIndex Index of the model to use.
	*/
	void		( *R_TempSphereModel )			( const Vector& vecOrigin, float speed, float life, int count, int modelIndex );
	
	/**
	*	Creates a temporary entity with the given model.
	*	@param vecOrigin Origin.
	*	@param vecDir Direction.
	*	@param vecAngles Angles.
	*	@param life How long the entity should live for, in seconds.
	*	@param modelIndex Index of the model to use.
	*	@param soundtype Bounce sound type. @see TE_Bounce
	*	@return Temporary entity. Can be null.
	*/
	TEMPENTITY*	( *R_TempModel )				( const Vector& vecOrigin, const Vector& vecDir, const Vector& vecAngles, float life, int modelIndex, int soundtype );

	/**
	*	Creates a temp entity with a sprite model with default settings.
	*	@param vecOrigin Origin.
	*	@param spriteIndex Index of the sprite to use.
	*	@param framerate Frame rate.
	*	@return Temporary entity. Can be null.
	*/
	TEMPENTITY*	( *R_DefaultSprite )			( const Vector& vecOrigin, int spriteIndex, float framerate );

	/**
	*	Creates a temp entity with a sprite model with given settings.
	*	@param vecOrigin Origin.
	*	@param vecDir Direction.
	*	@param scale Scale.
	*	@param modelIndex Index of the model to use.
	*	@param rendermode Render mode.
	*	@param renderfx Render FX.
	*	@param a Alpha value. [ 0, 1 ].
	*	@param life How long the entity should live for, in seconds.
	*	@param flags Flags. @see TempEntFlag.
	*	@return Temporary entity. Can be null.
	*/
	TEMPENTITY*	( *R_TempSprite )				( const Vector& vecOrigin, const Vector& vecDir, float scale, int modelIndex, int rendermode, int renderfx, float a, float life, int flags );

	/**
	*	Converts a decal index to a texture index.
	*	@param id Decal index.
	*	@return Texture index.
	*/
	int			( *Draw_DecalIndex )			( int id );

	/**
	*	Gets the decal index of a decal.
	*	@param pszName Decal name.
	*	@return Decal index, or 0 if the decal couldn't be found.
	*/
	int			( *Draw_DecalIndexFromName )	( const char* const pszName );

	/**
	*	Projects a decal onto the given brush entity's model.
	*	@param textureIndex Decal to project.
	*	@param entity Index of the entity to project onto.
	*	@param modelIndex Index of the entity's model to project onto.
	*	@param vecPosition Position in the world to project at.
	*	@param flags Flags. TODO: figure these out. - Solokiller
	*/
	void		( *R_DecalShoot )				( int textureIndex, int entity, int modelIndex, const Vector& vecPosition, int flags );

	/**
	*	Attaches a temp entity to a player.
	*	@param client Index of the client to attach to. 1 based.
	*	@param modelIndex Index of the model to attach.
	*	@param zoffset Z offset of the entity.
	*	@param life How long the entity should live for, in seconds.
	*/
	void		( *R_AttachTentToPlayer )		( int client, int modelIndex, float zoffset, float life );

	/**
	*	Removes all temp entities that have been attached to the given player.
	*	@param client Index of the client. 1 based.
	*/
	void		( *R_KillAttachedTents )		( int client );

	/**
	*	Creates a circular beam.
	*	@param type Beam type. @see BeamCircleType
	*	@param vecStart Starting point.
	*	@param vecEnd End point.
	*	@param modelIndex Index of the sprite to use.
	*	@param life How long the beam should live for, in seconds.
	*	@param width Beam width.
	*	@param amplitude Beam amplitude. If non-zero, creates sinusoidal wave effect.
	*	@param brightness Brightness.
	*	@param speed Beam speed.
	*	@param startFrame Starting frame.
	*	@param framerate Frame rate.
	*	@param r Red color. [ 0, 1 ].
	*	@param g Green color. [ 0, 1 ].
	*	@param b Blue color. [ 0, 1 ].
	*	@return Beam, or null if no beam could be created.
	*/
	BEAM*		( *R_BeamCirclePoints )		( const BeamCircleType type, const Vector& vecStart, const Vector& vecEnd, int modelIndex,
												  float life, float width, float amplitude, float brightness, float speed, 
												  int startFrame, float framerate, float r, float g, float b );

	/**
	*	Creates a beam between a fixed point and an entity.
	*	@param startEnt Entity to attach to.
	*	@param vecEnd Point to attach to.
	*	@param modelIndex Index of the sprite to use.
	*	@param life How long the beam should live for, in seconds.
	*	@param width Beam width.
	*	@param amplitude Beam amplitude. If non-zero, creates sinusoidal wave effect.
	*	@param brightness Brightness.
	*	@param speed Beam speed.
	*	@param startFrame Starting frame.
	*	@param framerate Frame rate.
	*	@param r Red color. [ 0, 1 ].
	*	@param g Green color. [ 0, 1 ].
	*	@param b Blue color. [ 0, 1 ].
	*	@return Beam, or null if no beam could be created.
	*/
	BEAM*		( *R_BeamEntPoint )			( int startEnt, const Vector& vecEnd, int modelIndex,
												  float life, float width, float amplitude, float brightness, float speed, 
												  int startFrame, float framerate, float r, float g, float b );

	/**
	*	Creates a beam between 2 entities.
	*	@param startEnt Entity to attach to.
	*	@param endEnt Other entity to attach to.
	*	@param modelIndex Index of the sprite to use.
	*	@param life How long the beam should live for, in seconds.
	*	@param width Beam width.
	*	@param amplitude Beam amplitude. If non-zero, creates sinusoidal wave effect.
	*	@param brightness Brightness.
	*	@param speed Beam speed.
	*	@param startFrame Starting frame.
	*	@param framerate Frame rate.
	*	@param r Red color. [ 0, 1 ].
	*	@param g Green color. [ 0, 1 ].
	*	@param b Blue color. [ 0, 1 ].
	*	@return Beam, or null if no beam could be created.
	*/
	BEAM*		( *R_BeamEnts )				( int startEnt, int endEnt, int modelIndex, 
												  float life, float width, float amplitude, float brightness, float speed, 
												  int startFrame, float framerate, float r, float g, float b );

	/**
	*	Creates a beam that follows the entity. Doesn't appear to actually create a beam.
	*	TODO
	*	@param startEnt Entity to attach to.
	*	@param modelIndex Index of the sprite to use.
	*	@param life How long the beam should live for, in seconds.
	*	@param width Beam width.
	*	@param r Red color. [ 0, 1 ].
	*	@param g Green color. [ 0, 1 ].
	*	@param b Blue color. [ 0, 1 ].
	*	@return Beam, or null if no beam could be created.
	*/
	BEAM*		( *R_BeamFollow )				( int startEnt, int modelIndex, float life, float width, float r, float g, float b, float brightness );

	/**
	*	Removes all beams that were attached to the given entity.
	*	@param deadEntity Index of the entity.
	*/
	void		( *R_BeamKill )					( int deadEntity );

	/**
	*	Creates a lightning bolt.
	*	Does not appear to do anything.
	*	TODO
	*	@param vecStart Starting position.
	*	@param vecEnd End position.
	*	@param modelIndex Index of the sprite to use.
	*	@param life How long the beam should live for, in seconds.
	*	@param width Beam width.
	*	@param amplitude Sinusoidal amplitude.
	*	@param brightness Brightness.
	*	@param speed Speed.
	*	@return Beam, or null if no beam could be created.
	*/
	BEAM*		( *R_BeamLightning )			( const Vector& vecStart, const Vector& vecEnd, int modelIndex, float life, float width, float amplitude, float brightness, float speed );

	/**
	*	Creates a beam between 2 points.
	*	@param vecStart Starting position.
	*	@param vecEnd End position.
	*	@param modelIndex Index of the sprite to use.
	*	@param life How long the beam should live for, in seconds.
	*	@param width Beam width.
	*	@param amplitude Sinusoidal amplitude.
	*	@param brightness Brightness.
	*	@param speed Speed.
	*	@param startFrame Starting frame.
	*	@param framerate Frame rate.
	*	@param r Red color. [ 0, 1 ].
	*	@param g Green color. [ 0, 1 ].
	*	@param b Blue color. [ 0, 1 ].
	*	@return Beam, or null if no beam could be created.
	*/
	BEAM*		( *R_BeamPoints )				( const Vector& vecStart, const Vector& vecEnd, int modelIndex,
												  float life, float width, float amplitude, float brightness, float speed, 
												  int startFrame, float framerate, float r, float g, float b );

	/**
	*	Creates a beam ring between the given entities.
	*	The starting and end entities define the diameter of the circle, with the circle being created between the points.
	*	@param startEnt Index of the starting entity.
	*	@param endEnt Index of the end entity.
	*	@param modelIndex Index of the sprite to use.
	*	@param life How long the beam should live for, in seconds.
	*	@param width Beam width.
	*	@param amplitude Sinusoidal amplitude.
	*	@param brightness Brightness.
	*	@param speed Speed.
	*	@param startFrame Starting frame.
	*	@param framerate Frame rate.
	*	@param r Red color. [ 0, 1 ].
	*	@param g Green color. [ 0, 1 ].
	*	@param b Blue color. [ 0, 1 ].
	*	@return Beam, or null if no beam could be created.
	*/
	BEAM*		( *R_BeamRing )				( int startEnt, int endEnt, int modelIndex, 
											  float life, float width, float amplitude, float brightness, float speed, 
											  int startFrame, float framerate, float r, float g, float b );

	/**
	*	Creates a dynamic light.
	*	@param key if non-zero, looks up the light with the given key. If 0, or the light couldn't be found, gets the first free light, or the first light if no free lights exist.
	*	@return Dynamic light.
	*/
	dlight_t*	( *CL_AllocDlight )			( int key );

	/**
	*	Creates a dynamic entity light.
	*	@param key if non-zero, looks up the light with the given key. If 0, or the light couldn't be found, gets the first free light, or the first light if no free lights exist.
	*	@return Dynamic entity light.
	*/
	dlight_t*	( *CL_AllocElight )			( int key );

	/**
	*	Allocates a low priority temp entity.
	*	@param vecOrigin Origin.
	*	@param model Model to use.
	*	@return Temporary entity, or null if no entity could be allocated.
	*/
	TEMPENTITY*	( *CL_TempEntAlloc )			( const Vector& vecOrigin, model_t* model );

	/**
	*	Allocates a low priority temp entity with no model.
	*	@param vecOrigin Origin.
	*	@return Temporary entity, or null if no entity could be allocated.
	*/
	TEMPENTITY*	( *CL_TempEntAllocNoModel )		( const Vector& vecOrigin );

	/**
	*	Allocates a high priority temp entity. High priority temp entities are never freed when a high priority entity is created.
	*	@param vecOrigin Origin.
	*	@param model Model to use.
	*	@return Temporary entity, or null if no entity could be allocated.
	*/
	TEMPENTITY*	( *CL_TempEntAllocHigh )		( const Vector& vecOrigin, model_t* model );

	/**
	*	@param vecOrigin Origin.
	*	@param model Model to use.
	*	@param high Whether this is a high or low priority entity. @see TempEntPriority
	*	@param callback Think callback.
	*	@return Temporary entity, or null if no entity could be allocated.
	*/
	TEMPENTITY*	( *CL_TentEntAllocCustom )		( const Vector& origin, model_t *model, int high, TempEntThink callback );

	/**
	*	Obsolete. Always zeroes out packed.
	*/
	void		( *R_GetPackedColor )			( short *packed, short color );

	/**
	*	Looks up the index of the given RGB color in the base palette.
	*	@param r Red color. [ 0, 255 ].
	*	@param g Green color. [ 0, 255 ].
	*	@param b Blue color. [ 0, 255 ].
	*	@return Index, or -1 if it couldn't be found.
	*/
	short		( *R_LookupColor )				( unsigned char r, unsigned char g, unsigned char b );

	/**
	*	Removes all instances of a particular decal.
	*	@param textureIndex Points to the decal index in the array, not the actual texture index.
	*/
	void		( *R_DecalRemoveAll )			( int textureIndex );

	/**
	*	Projects a decal onto the given brush entity's model.
	*	@param textureIndex Decal to project.
	*	@param entity Index of the entity to project onto.
	*	@param modelIndex Index of the entity's model to project onto.
	*	@param vecPosition Position in the world to project at.
	*	@param flags Flags. TODO: figure these out. - Solokiller
	*	@param scale Scale.
	*/
	void		( *R_FireCustomDecal )			( int textureIndex, int entity, int modelIndex, const Vector& vecPosition, int flags, float scale );
};

extern efx_api_t efx;

#endif
