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
#ifndef PROGDEFS_H
#define PROGDEFS_H
#ifdef _WIN32
#pragma once
#endif

struct globalvars_t
{	
	float		time;
	float		frametime;
	float		force_retouch;
	string_t	mapname;
	string_t	startspot;
	float		deathmatch;
	float		coop;
	float		teamplay;
	float		serverflags;
	float		found_secrets;
	Vector		v_forward;
	Vector		v_up;
	Vector		v_right;
	float		trace_allsolid;
	float		trace_startsolid;
	float		trace_fraction;
	Vector		trace_endpos;
	Vector		trace_plane_normal;
	float		trace_plane_dist;
	edict_t		*trace_ent;
	float		trace_inopen;
	float		trace_inwater;
	int			trace_hitgroup;
	int			trace_flags;
	int			msg_entity;
	int			cdAudioTrack;
	int			maxClients;
	int			maxEntities;
	const char	*pStringBase;

	void		*pSaveData;
	Vector		vecLandmarkOffset;
};


/**
*	Contains entity variables that the engine needs to access for graphics, networking and physics.
*/
struct entvars_t
{
	/**
	*	The classname of the entity.
	*/
	string_t	classname;

	/**
	*	Global entity name. Used for entities that carry their state across level transitions in singleplayer.
	*/
	string_t	globalname;

	/**
	*	Absolute origin in the world.
	*	Networked.
	*/
	Vector		origin;

	/**
	*	Old entity origin. Used in the engine to store the player's old origin when noclip is turned off.
	*/
	Vector		oldorigin;

	/**
	*	The entity's absolute velocity.
	*	Networked.
	*/
	Vector		velocity;

	/**
	*	Base velocity to apply when standing on conveyors or swimming in water with current contents set.
	*	Networked.
	*/
	Vector		basevelocity;

	/**
	*	Base velocity that was passed in to server physics so 
	*	client can predict conveyors correctly. Server zeroes it, so we need to store here, too.
	*	Zeroed out prior to DLL_FUNCTIONS::pfnPreThink being called, 
	*	set to the player's basevelocity after the player's think has been called, before DLL_FUNCTIONS::pfnPostThink is called.
	*/
	Vector      clbasevelocity;

	/**
	*	Movement direction and speed.
	*/
	Vector		movedir;

	/**
	*	Model angles.
	*	Networked.
	*/
	Vector		angles;

	/**
	*	angle velocity (degrees per second).
	*/
	Vector		avelocity;

	/**
	*	auto-decaying view angle adjustment.
	*	Decayed in PM_CheckParamters.
	*	Networked.
	*	@see PM_CheckParamters
	*/
	Vector		punchangle;

	/**
	*	Viewing angle (player only).
	*/
	Vector		v_angle;

	// For parametric entities
	/**
	*	Parametric end position.
	*	Networked.
	*/
	Vector		endpos;

	/**
	*	Parametric start position.
	*	Networked.
	*/
	Vector		startpos;

	/**
	*	Time at which the entity will have traversed the distance between startpos and endpos.
	*	Networked.
	*/
	float		impacttime;

	/**
	*	Time at which the entity has started its parametric movement.
	*	Networked.
	*/
	float		starttime;

	/**
	*	Player only. Sets the player's view angles on the client. The server's angles are not modified automatically.
	*	@see FixAngleMode
	*/
	int			fixangle;

	/**
	*	The ideal pitch for this entity. enginefuncs_t::pfnChangePitch will adjust the entity's pitch to approach this value.
	*	@see enginefuncs_t::pfnChangePitch
	*/
	float		idealpitch;

	/**
	*	The pitch speed for this entity. enginefuncs_t::pfnChangePitch will adjust the entity's pitch at this speed.
	*	@see enginefuncs_t::pfnChangePitch
	*/
	float		pitch_speed;

	/**
	*	The ideal yaw for this entity. enginefuncs_t::pfnChangeYaw will adjust the entity's yaw to approach this value.
	*	@see enginefuncs_t::pfnChangeYaw
	*/
	float		ideal_yaw;

	/**
	*	The yaw speed for this entity. enginefuncs_t::pfnChangeYaw will adjust the entity's yaw at this speed.
	*	@see enginefuncs_t::pfnChangeYaw
	*/
	float		yaw_speed;

	/**
	*	If this entity has a model, this is the index of that model. Otherwise, is 0.
	*	Set by enginefuncs_t::pfnSetModel
	*	Networked.
	*/
	int			modelindex;

	/**
	*	If this entity has a model, this is the name of that model. Otherwise, is iStringNull.
	*/
	string_t	model;

	//These are treated as string_t on the server side, model indices on the client. - Solokiller
	/**
	*	The player's viewmodel. (v_*.mdl)
	*	Networked.
	*/
	int			viewmodel;

	/**
	*	What other players see; the player's third person viewmodel. (p_*.mdl)
	*	Networked.
	*/
	int			weaponmodel;
	
	/**
	*	BB max translated to world coord.
	*	Set by the engine whenever the entity moves, set by game code to adjust the size.
	*/
	Vector		absmin;

	/**
	*	BB max translated to world coord.
	*	Set by the engine whenever the entity moves, set by game code to adjust the size.
	*/
	Vector		absmax;

	/**
	*	local BB min.
	*/
	Vector		mins;

	/**
	*	local BB max.
	*/
	Vector		maxs;

	/**
	*	maxs - mins.
	*/
	Vector		size;

	/**
	*	Last think time for brush entities. (MOVETYPE_PUSH)
	*/
	float		ltime;

	/**
	*	Next think time. Should be gpGlobals->time + delay for all entities except MOVETYPE_PUSH entities; use ltime + delay for those instead.
	*/
	float		nextthink;

	/**
	*	Movetype.
	*	Networked.
	*	@see MoveType
	*/
	int			movetype;

	/**
	*	Solid type.
	*	Networked.
	*	@see Solid
	*/
	int			solid;

	/**
	*	Studio model skin index. Seems to affect the player's z velocity if the FL_ONTARGET flag is set. TODO investigate. - Solokiller
	*	Networked.
	*/
	int			skin;

	/**
	*	Sub-model selection for studiomodels.
	*	Networked.
	*/
	int			body;

	/**
	*	Effects flags.
	*	Networked.
	*	@see Effect
	*/
	int 		effects;
	
	/**
	*	% of "normal" gravity.
	*	Default 1.0.
	*	Networked.
	*/
	float		gravity;

	/**
	*	Inverse elasticity of MOVETYPE_BOUNCE.
	*	Networked.
	*/
	float		friction;
	
	/**
	*	The stored light level for this entity. Use enginefuncs_t::pfngetEntityIllum to retrieve this.
	*/
	int			light_level;

	/**
	*	Animation sequence.
	*	Networked.
	*/
	int			sequence;

	/**
	*	Movement animation sequence for player (0 for none).
	*	Networked for clients only.
	*/
	int			gaitsequence;

	/**
	*	% playback position in animation sequences (0..255).
	*	Networked.
	*/
	float		frame;

	/**
	*	World time when frame was set.
	*	Networked.
	*/
	float		animtime;

	/**
	*	Animation playback rate (-8x to 8x).
	*	Networked.
	*/
	float		framerate;

	/**
	*	Bone controller setting (0..255).
	*	Networked.
	*/
	byte		controller[4];

	/**
	*	Blending amount between sub-sequences (0..255).
	*	Networked.
	*/
	byte		blending[2];

	/**
	*	Sprite rendering scale (0..255).
	*	Networked.
	*/
	float		scale;

	/**
	*	Render mode.
	*	Networked.
	*	@see RenderMode
	*/
	int			rendermode;

	/**
	*	Render amount.
	*	Networked.
	*/
	float		renderamt;

	/**
	*	Render color. Each component is in the range [ 0, 255 ].
	*	Networked.
	*/
	Vector		rendercolor;

	/**
	*	Render FX.
	*	Networked.
	*	@see RenderFX
	*/
	int			renderfx;

	/**
	*	Health.
	*	Networked.
	*/
	float		health;

	/**
	*	Frags. Player score.
	*/
	float		frags;

	/**
	*	Bit mask for available weapons.
	*	Networked.
	*/
	int			weapons;

	/**
	*	Whether this entity can take damage, and whether the player's autoaim can snap to this entity.
	*	@see TakeDamageMode
	*/
	float		takedamage;

	/**
	*	Current entity death state.
	*	Networked.
	*	@see DeadFlag
	*/
	int			deadflag;

	/**
	*	Eye position.
	*	Networked.
	*/
	Vector		view_ofs;

	/**
	*	Buttons that the player currently has pressed in.
	*	Stores the use type for delayed triggers.
	*	@see InputFlag
	*/
	int			button;

	/**
	*	Current player impulse command.
	*/
	int			impulse;

	/**
	*	Entity pointer when linked into a linked list.
	*	Used by enginefuncs_t::pfnEntitiesInPVS to return a list of entities.
	*/
	edict_t		*chain;

	/**
	*	Used in CBaseMonster::OnTakeDamage and CBasePlayer::UpdateClientData to track the last damage inflictor.
	*/
	edict_t		*dmg_inflictor;

	/**
	*	Used as a destination for enginefuncs_t::pfnMoveToOrigin when the entity is flying or swimming.
	*/
	edict_t		*enemy;

	/**
	*	Entity pointer when MOVETYPE_FOLLOW.
	*/
	edict_t		*aiment;
	
	/**
	*	The entity that owns this entity, or null if it has no owner.
	*/
	edict_t		*owner;

	/**
	*	The entity that this entity is currently standing on, or null if it isn't standing on anything.
	*/
	edict_t		*groundentity;

	/**
	*	Spawnflags. These are specific to each entity.
	*/
	int			spawnflags;

	/**
	*	Flags.
	*	@see EntFlag
	*/
	int			flags;
	
	/**
	*	Lowbyte topcolor, highbyte bottomcolor.
	*	Networked.
	*/
	int			colormap;

	/**
	*	The team that this entity belongs to. 0 For no team.
	*	The engine will check this to see if autoaim should snap to this entity. Entities on the same team are never targeted by autoaim.
	*/
	int			team;

	/**
	*	Maximum health that this entity can have.
	*	Entities cannot have a health higher than this when given using GiveHealth.
	*	Also used by some entities as a counter.
	*	The engine does not access this.
	*/
	float		max_health;

	/**
	*	Used in physics code as playermove_t::waterjumptime, is save/restored.
	*	Copied to clientdata_t::waterjumptime.
	*/
	float		teleport_time;

	/**
	*	Never used, is save/restored.
	*/
	float		armortype;

	/**
	*	The amount of armor that this player has.
	*	Also used by some entities for other purposes.
	*/
	float		armorvalue;

	/**
	*	The current water level.
	*	Networked.
	*	@see WaterLevel
	*/
	int			waterlevel;

	/**
	*	The type of water the player is currently in. Only valid if the player is in water, is CONTENTS_EMPTY otherwise.
	*	@see Contents
	*/
	int			watertype;
	
	/**
	*	Targetname of the entity to trigger when this entity fires its targets.
	*/
	string_t	target;

	/**
	*	Targetname of this entity.
	*/
	string_t	targetname;

	/**
	*	Name of this player. Set by players using the "name" command.
	*	Also used by other entities for various purposes.
	*/
	string_t	netname;

	/**
	*	Generic string member used by various entities. game_text stores its message here.
	*/
	string_t	message;

	/**
	*	Tracks the amount of damage that the player took since the last time CBasePlayer::UpdateClientData was called, or since they spawned.
	*	Also used by other entities for various purposes.
	*/
	float		dmg_take;

	/**
	*	Used by entities for various purposes.
	*/
	float		dmg_save;

	/**
	*	Used by MOVETYPE_PUSHSTEP and MOVETYPE_STEP while the entity is swimming.
	*	Damage taken while submerged.
	*	Engine handling is obsolete for players. The engine code has been superseded by game code player physics.
	*/
	float		dmg;

	/**
	*	Used by MOVETYPE_PUSHSTEP and MOVETYPE_STEP while the entity is swimming.
	*	Next time lava/slime damage should be taken.
	*	Engine handling is obsolete for players. The engine code has been superseded by game code player physics.
	*/
	float		dmgtime;
	
	/**
	*	Generic string 1.
	*/
	string_t	noise;

	/**
	*	Generic string 2.
	*/
	string_t	noise1;

	/**
	*	Generic string 3.
	*/
	string_t	noise2;

	/**
	*	Generic string 4.
	*/
	string_t	noise3;
	
	/**
	*	The entity's movement speed, in units/second.
	*/
	float		speed;

	/**
	*	Used by MOVETYPE_PUSHSTEP and MOVETYPE_STEP while the entity is swimming.
	*	When the entity runs out of air.
	*	Engine handling is obsolete for players. The engine code has been superseded by game code player physics.
	*/
	float		air_finished;

	/**
	*	Used by MOVETYPE_PUSHSTEP and MOVETYPE_STEP while the entity is swimming.
	*	Next time drown damage should be taken.
	*	Engine handling is obsolete for players. The engine code has been superseded by game code player physics.
	*/
	float		pain_finished;

	/**
	*	Used by MOVETYPE_PUSHSTEP and MOVETYPE_STEP while the entity is swimming.
	*	When the player's protection against lava/slime has run out.
	*	Engine handling is obsolete for players. The engine code has been superseded by game code player physics.
	*/
	float		radsuit_finished;
	
	/**
	*	The edict_t instance that this entvars_t instance belongs to.
	*	Should be reinterpret_cast<edict_t*>( reinterpret_cast<byte*>( &classname ) - offsetof( edict_t, v ) ).
	*/
	edict_t		*pContainingEntity;

	/**
	*	The player's class. Mod specific.
	*	If this is a SOLID_BSP, this will determine whether damage decals will be applied to the bsp model.
	*	A setting of 1 enables damage decals.
	*	@see EV_HLDM_DamageDecal
	*	Networked.
	*/
	int			playerclass;

	/**
	*	Maximum movement speed.
	*	Networked.
	*/
	float		maxspeed;

	/**
	*	This player's field of view.
	*	Networked in clientdata_t.
	*/
	float		fov;

	/**
	*	The player's current weapon animation.
	*	Networked in clientdata_t.
	*/
	int			weaponanim;

	/**
	*	Never used on the server side. This variable is completely unused.
	*/
	int			pushmsec;

	/**
	*	Whether the player is currently ducking. This state is synchronized with playermove_t.
	*	Networked in clientdata_t.
	*/
	int			bInDuck;

	/**
	*	Time until the next step sound should be played for this player. This state is synchronized with playermove_t.
	*	Networked in clientdata_t.
	*/
	int			flTimeStepSound;

	/**
	*	Time until the next swim sound should be played for this player. This state is synchronized with playermove_t.
	*	Networked in clientdata_t.
	*/
	int			flSwimTime;

	/**
	*	How much longer until the player is fully ducked. This state is synchronized with playermove_t.
	*	Networked in clientdata_t.
	*/
	int			flDuckTime;

	/**
	*	Whether the last step sound was a left or right step sound. This state is synchronized with playermove_t.
	*	Networked in clientdata_t.
	*/
	int			iStepLeft;

	/**
	*	The player's current fall velocity. This state is synchronized with playermove_t.
	*	Networked in clientdata_t.
	*/
	float		flFallVelocity;

	/**
	*	Special variable used to tell the engine to skip the shield hitbox for CS shield weapons.
	*	0: don't skip.
	*	1: skip.
	*	Hitgroup 21 is the shield.
	*/
	int			gamestate;

	/**
	*	Player button states for the previous frame.
	*	Networked.
	*	@see button
	*/
	int			oldbuttons;

	/**
	*	Traceline group.
	*/
	int			groupinfo;

	/**
	*	For mods: these variables are shared between the server's game code, the client's entity_state_t, and the player physics code (physent_t).
	*	If you use one of these, make sure to add the copying of the variable in UpdateClientData (server), HUD_TxferLocalOverrides (client), CClientPrediction::WeaponsPostThink(client).
	*	There may be other places where this information is copied over as well.
	*	@see UpdateClientData
	*	@see HUD_TxferLocalOverrides
	*	@see CClientPrediction::WeaponsPostThink
	*/
	int			iuser1;

	///@see iuser1
	int			iuser2;

	///@see iuser1
	int			iuser3;

	///@see iuser1
	int			iuser4;

	///@see iuser1
	float		fuser1;

	///@see iuser1
	float		fuser2;

	///@see iuser1
	float		fuser3;

	///@see iuser1
	float		fuser4;

	///@see iuser1
	Vector		vuser1;

	///@see iuser1
	Vector		vuser2;

	///@see iuser1
	Vector		vuser3;

	///@see iuser1
	Vector		vuser4;

	/**
	*	These aren't shared with anything, they're just here if you need them.
	*	I recommend avoiding these to avoid type casting issues.
	*/
	edict_t		*euser1;

	///@see euser1
	edict_t		*euser2;

	///@see euser1
	edict_t		*euser3;

	///@see euser1
	edict_t		*euser4;
};


#endif // PROGDEFS_H
