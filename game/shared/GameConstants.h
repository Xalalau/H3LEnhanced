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
#ifndef GAME_SHARED_GAMECONSTANTS_H
#define GAME_SHARED_GAMECONSTANTS_H

#ifdef CLIENT_DLL
#define LIBRARY_NAME "client"
#elif defined( SERVER_DLL )
#define LIBRARY_NAME "server"
#else
#error "Unhandled library name"
#endif

/**
*	Approx 60 feet.
*/
#define PLAYER_FATAL_FALL_SPEED 1024

/**
*	Approx 20 feet.
*/
#define PLAYER_MAX_SAFE_FALL_SPEED 580

/**
*	Damage per unit per second.
*/
#define DAMAGE_FOR_FALL_SPEED ( ( float ) 100 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED ) )

/**
*	Minimum impact velocity at which step sounds are played.
*/
#define PLAYER_MIN_BOUNCE_SPEED 200

/**
*	 Won't punch player's screen/make scrape noise unless player falling at least this fast.
*/
#define PLAYER_FALL_PUNCH_THRESHHOLD ( ( float ) 350 )

/**
*	Fastest vertical climbing speed possible.
*/
#define	CLIMB_MAX_SPEED 200

/**
*	How many frames in between screen shakes when climbing.
*/
#define CLIMB_SHAKE_FREQUENCY 22

/**
*	Climbing deceleration rate.
*/
#define	CLIMB_SPEED_DEC 15

/**
*	How far to 'punch' client X axis when climbing.
*/
#define	CLIMB_PUNCH_X -7

/**
*	How far to 'punch' client Z axis when climbing.
*/
#define CLIMB_PUNCH_Z 7

/**
*	How fast we longjump.
*/
#define PLAYER_LONGJUMP_SPEED 350

/**
*	Minimum horizontal velocity that a player must be moving at before step sounds are played.
*/
#define PLAYER_STEP_SOUND_SPEED 220

/**
*	Delay between geiger updates.
*/
#define PLAYER_GEIGERDELAY 0.25

/**
*	Armor Takes 80% of the damage.
*/
#define PLAYER_ARMOR_RATIO 0.2

/**
*	Each Point of Armor is work 1/x points of health.
*/
#define PLAYER_ARMOR_BONUS 0.5

/**
*	Lung full of air lasts this many seconds.
*/
#define PLAYER_SWIM_AIRTIME 12

/**
*	How high the player jumps when jumping out of water.
*/
#define PLAYER_WATERJUMP_HEIGHT 8

/**
*	The radius around the player to search for entities to use.
*/
#define	PLAYER_USE_SEARCH_RADIUS ( ( float ) 64 )

/**
*	Time between suit playback events.
*/
#define PLAYER_SUITUPDATETIME 3.5

/**
*	Suit play queue is empty, don't delay too long before playback.
*/
#define PLAYER_SUITFIRSTUPDATETIME 0.1

#define	SOUND_FLASHLIGHT_ON		"items/flashlight1.wav"
#define	SOUND_FLASHLIGHT_OFF	"items/flashlight1.wav"

#define	FLASH_DRAIN_TIME	 1.2 //100 units/3 minutes
#define	FLASH_CHARGE_TIME	 0.2 // 100 units/20 seconds  (seconds per unit)

// defaults for clientinfo messages
#define	DEFAULT_VIEWHEIGHT 28

//TODO: change into const Vector - Solokiller
#define VEC_HULL_MIN		Vector( -16, -16, -36 )
#define VEC_HULL_MAX		Vector( 16,  16,  36 )
#define VEC_HUMAN_HULL_MIN	Vector( -16, -16, 0 )
#define VEC_HUMAN_HULL_MAX	Vector( 16, 16, 72 )
#define VEC_HUMAN_HULL_DUCK	Vector( 16, 16, 36 )

#define VEC_VIEW			Vector( 0, 0, DEFAULT_VIEWHEIGHT )

#define VEC_DUCK_HULL_MIN	Vector( -16, -16, -18 )
#define VEC_DUCK_HULL_MAX	Vector( 16,  16,  18 )
#define VEC_DUCK_VIEW		Vector( 0, 0, 12 )

#define PM_DEAD_VIEWHEIGHT -8

/**
*	@defgroup AutoAimDegrees AutoAim Degrees
*
*	AutoAim degrees for use in weapons code.
*
*	Calculated as follows:
*	UTIL_MakeVectors( Vector( 0, 0, 0 ) );
*
*	AngleVectors( Vector( degrees, 0, 0 ), outVector, nullptr, nullptr );
*
*	double dot = fabs( DotProduct( outVector, gpGlobals->v_right ) )
*		+ fabs( DotProduct( outVector, gpGlobals->v_up ) ) * 0.5;
*
*	const double dot2 = dot * 2;
*
*	Use doubles when calculating for inproved accuracy.
*
*	@{
*/
#define AUTOAIM_1DEGREE		0.01745240643728
#define AUTOAIM_2DEGREES	0.03489949670250
#define AUTOAIM_3DEGREES	0.05233595624294
#define AUTOAIM_4DEGREES	0.06975647374413
#define AUTOAIM_5DEGREES	0.08715574274766
#define AUTOAIM_6DEGREES	0.10452846326765
#define AUTOAIM_7DEGREES	0.12186934340515
#define AUTOAIM_8DEGREES	0.13917310096007
#define AUTOAIM_9DEGREES	0.15643446504023
#define AUTOAIM_10DEGREES	0.17364817766690

/** @} */

/**
*	@defgroup DotDegrees Dot Product Degrees
*
*	Dot product results for vectors that have the given amount of degrees between them.
*
*	@{
*/
#define DOT_1DEGREE   0.9998476951564
#define DOT_2DEGREE   0.9993908270191
#define DOT_3DEGREE   0.9986295347546
#define DOT_4DEGREE   0.9975640502598
#define DOT_5DEGREE   0.9961946980917
#define DOT_6DEGREE   0.9945218953683
#define DOT_7DEGREE   0.9925461516413
#define DOT_8DEGREE   0.9902680687416
#define DOT_9DEGREE   0.9876883405951
#define DOT_10DEGREE  0.9848077530122
#define DOT_15DEGREE  0.9659258262891
#define DOT_20DEGREE  0.9396926207859
#define DOT_25DEGREE  0.9063077870367

/** @} */

/**
*	@defgroup ViewFields View Fields
*
*	Dot products for view cone checking
*
*	@{
*/

/**
*	+-180 degrees
*/
#define VIEW_FIELD_FULL ( ( float ) -1.0 )

/**
*	+-135 degrees 0.1 // +-85 degrees, used for full FOV checks
*/
#define	VIEW_FIELD_WIDE ( ( float ) -0.7 )

/**
*	+-45 degrees, more narrow check used to set up ranged attacks
*/
#define	VIEW_FIELD_NARROW ( ( float ) 0.7 )

/**
*	+-25 degrees, more narrow check used to set up ranged attacks
*/
#define	VIEW_FIELD_ULTRA_NARROW	( ( float ) 0.9 )

/** @} */

/**
*	Longest the intermission can last, in seconds.
*/
#define MAX_INTERMISSION_TIME		120

#define INTERMISSION_TIME		6

/**
*	@defgroup ObjectRespawnTimes Object multiplayer respawn times
*
*	@{
*/

#define ITEM_RESPAWN_TIME	30
#define WEAPON_RESPAWN_TIME	20
#define AMMO_RESPAWN_TIME	20

/** @} */

/**
*	When we are within this close to running out of entities, items 
*	marked with the ITEM_FLAG_LIMITINWORLD will delay their respawn
*/
#define ENTITY_INTOLERANCE	100

#endif //GAME_SHARED_GAMECONSTANTS_H