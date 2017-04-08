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
#ifndef GAME_SHARED_CBASEENTITY_SHARED_H
#define GAME_SHARED_CBASEENTITY_SHARED_H

/*

Class Hierachy

CBaseEntity
	CBaseDelay
		CBaseToggle
			CBaseItem
			CBaseMonster
				CBaseCycler
				CBasePlayer
*/

#include <cstddef>

#include "CBitSet.h"

#include "archtypes.h"     // DAL

#include "CBaseForward.h"

#include "SaveRestore.h"
#include "entities/NPCs/Schedule.h"

#include "entities/AnimationEvent.h"

#include "entities/DataMapping.h"

#include "entities/EHandle.h"

#include "Damage.h"

#include "ButtonSounds.h"

#include "CTakeDamageInfo.h"

#include "EntityClasses.h"

#if USE_ANGELSCRIPT && defined( SERVER_DLL )
#include <Angelscript/util/CASRefPtr.h>

#include <Angelscript/add_on/scriptdictionary.h>
#endif

#undef GetMessage

using CEntBitSet = CBitSet<int>;

//
// Base Entity.  All entity types derive from this
//
class CBaseEntity
{
public:
	DECLARE_CLASS_NOBASE( CBaseEntity );
	DECLARE_DATADESC_NOBASE();

	// Constructor.  Set engine to use C/C++ callback functions
	// pointers to engine data
	entvars_t *pev;		// Don't need to save/restore this pointer, the engine resets it

						// path corners
	EHANDLE				m_hGoalEnt;// path corner we are heading towards
	CBaseEntity			*m_pLink;// used for temporary link-list operations. 

	/*
	*	Getters and setters for entvars_t.
	*/
public:
	/**
	*	@return This entity's classname.
	*/
	const char* GetClassname() const { return STRING( pev->classname ); }

	/**
	*	Sets this entity's classname.
	*	It is assumed that pszClassName is either a string in the program's string table or allocated using ALLOC_STRING,
	*	or otherwise has a lifetime that is at least as long as the rest of the map.
	*/
	void SetClassname( const char* pszClassName )
	{
		pev->classname = MAKE_STRING( pszClassName );
	}

	/**
	*	@return Whether this entity's classname matches the given classname.
	*/
	bool ClassnameIs( const char* const pszClassName ) const
	{
		return FStrEq( GetClassname(), pszClassName );
	}

	/**
	*	@copydoc ClassnameIs( const char* const pszClassName ) const
	*/
	bool ClassnameIs( const string_t iszClassName ) const
	{
		return ClassnameIs( STRING( iszClassName ) );
	}

	/**
	*	@return Whether this entity has a global name.
	*/
	bool HasGlobalName() const
	{
		return !!( *STRING( pev->globalname ) );
	}

	/**
	*	@return The global name.
	*/
	const char* GetGlobalName() const { return STRING( pev->globalname ); }

	/**
	*	Sets the global name.
	*	@param iszGlobalName Name to set.
	*/
	void SetGlobalName( const string_t iszGlobalName )
	{
		pev->globalname = iszGlobalName;
	}

	/**
	*	Sets the global name.
	*	@param pszGlobalName Name to set.
	*/
	void SetGlobalName( const char* const pszGlobalName )
	{
		SetGlobalName( MAKE_STRING( pszGlobalName ) );
	}

	/**
	*	Clears the global name.
	*/
	void ClearGlobalName()
	{
		pev->globalname = iStringNull;
	}

	/**
	*	@return Whether this entity has a targetname.
	*/
	bool HasTargetname() const
	{
		return !!( *STRING( pev->targetname ) );
	}

	/**
	*	@return The targetname.
	*/
	const char* GetTargetname() const { return STRING( pev->targetname ); }

	/**
	*	Sets the targetname.
	*	@param iszTargetName Name to set.
	*/
	void SetTargetname( const string_t iszTargetName )
	{
		pev->targetname = iszTargetName;
	}

	/**
	*	Sets the targetname.
	*	@param pszTargetName Name to set.
	*/
	void SetTargetname( const char* const pszTargetName )
	{
		SetTargetname( MAKE_STRING( pszTargetName ) );
	}

	/**
	*	Clears the targetname.
	*/
	void ClearTargetname()
	{
		pev->targetname = iStringNull;
	}

	/**
	*	@return Whether this entity has a target.
	*/
	bool HasTarget() const
	{
		return !!( *STRING( pev->target ) );
	}

	/**
	*	@return The target.
	*/
	const char* GetTarget() const { return STRING( pev->target ); }

	/**
	*	Sets the target.
	*	@param iszTarget Target to set.
	*/
	void SetTarget( const string_t iszTarget )
	{
		pev->target = iszTarget;
	}

	/**
	*	Sets the target.
	*	@param pszTarget Target to set.
	*/
	void SetTarget( const char* const pszTarget )
	{
		SetTarget( MAKE_STRING( pszTarget ) );
	}

	/**
	*	Clears the target.
	*/
	void ClearTarget()
	{
		pev->target = iStringNull;
	}

	/**
	*	@return Whether this entity has a net name.
	*/
	bool HasNetName() const
	{
		return !!( *STRING( pev->netname ) );
	}

	/**
	*	@return The net name.
	*/
	const char* GetNetName() const { return STRING( pev->netname ); }

	/**
	*	Sets the net name.
	*	@param iszNetName Net name to set.
	*/
	void SetNetName( const string_t iszNetName )
	{
		pev->netname = iszNetName;
	}

	/**
	*	Sets the net name.
	*	@param pszNetName Name to set.
	*/
	void SetNetName( const char* const pszNetName )
	{
		SetNetName( MAKE_STRING( pszNetName ) );
	}

	/**
	*	Clears the net name.
	*/
	void ClearNetName()
	{
		pev->netname = iStringNull;
	}

	/**
	*	@return The absolute origin.
	*/
	const Vector& GetAbsOrigin() const { return pev->origin; }

	/**
	*	Sets the absolute origin.
	*	@param vecOrigin Origin to set.
	*/
	void SetAbsOrigin( const Vector& vecOrigin )
	{
		UTIL_SetOrigin( this, vecOrigin );
	}

	/**
	*	@return The old origin.
	*/
	const Vector& GetOldOrigin() const { return pev->oldorigin; }

	/**
	*	@return The old origin, in mutable form.
	*/
	Vector& GetMutableOldOrigin() { return pev->oldorigin; }

	/**
	*	Sets the old origin.
	*	@param vecOrigin Origin to set.
	*/
	void SetOldOrigin( const Vector& vecOrigin )
	{
		pev->oldorigin = vecOrigin;
	}

	/**
	*	@return The absolute velocity.
	*/
	const Vector& GetAbsVelocity() const { return pev->velocity; }

	/**
	*	Sets the absolute velocity.
	*	@param vecVelocity Velocity to set.
	*/
	void SetAbsVelocity( const Vector& vecVelocity )
	{
		pev->velocity = vecVelocity;
	}

	/**
	*	@return The base velocity.
	*/
	const Vector& GetBaseVelocity() const { return pev->basevelocity; }

	/**
	*	Sets the base velocity.
	*	@param vecVelocity Velocity to set.
	*/
	void SetBaseVelocity( const Vector& vecVelocity )
	{
		pev->basevelocity = vecVelocity;
	}

	/**
	*	@return This entity's move direction.
	*/
	const Vector& GetMoveDir() const { return pev->movedir; }

	/**
	*	Sets the move direction.
	*	@param vecMoveDir Move direction to set.
	*/
	void SetMoveDir( const Vector& vecMoveDir )
	{
		pev->movedir = vecMoveDir;
	}

	/**
	*	@return The absolute angles.
	*/
	const Vector& GetAbsAngles() const { return pev->angles; }

	/**
	*	Sets the absolute angles.
	*	@param vecAngles Angles to set.
	*/
	void SetAbsAngles( const Vector& vecAngles )
	{
		pev->angles = vecAngles;
	}

	/**
	*	@return The angular velocity.
	*/
	const Vector& GetAngularVelocity() const { return pev->avelocity; }

	/**
	*	@return The angular velocity, in mutable form.
	*/
	Vector& GetMutableAVelocity() { return pev->avelocity; }

	/**
	*	Sets the angular velocity.
	*	@param vecAVelocity Angular velocity to set.
	*/
	void SetAngularVelocity( const Vector& vecAVelocity )
	{
		pev->avelocity = vecAVelocity;
	}

	/**
	*	@return The punch angle.
	*/
	const Vector& GetPunchAngle() const { return pev->punchangle; }

	/**
	*	@return This entity's punch angle, in mutable form.
	*/
	Vector& GetMutablePunchAngle() { return pev->punchangle; }

	/**
	*	Sets the punch angle.
	*	@param vecPunchAngle Punch angle to set.
	*/
	void SetPunchAngle( const Vector& vecPunchAngle )
	{
		pev->punchangle = vecPunchAngle;
	}

	/**
	*	@return The view angle.
	*/
	const Vector& GetViewAngle() const { return pev->v_angle; }

	/**
	*	@return The view angle, in mutable form.
	*/
	Vector& GetMutableViewAngle() { return pev->v_angle; }

	/**
	*	Sets the view angle.
	*	@param vecViewAngle View angle to set.
	*/
	void SetViewAngle( const Vector& vecViewAngle )
	{
		pev->v_angle = vecViewAngle;
	}

	/**
	*	@return The fixangle mode.
	*	@see FixAngleMode
	*/
	FixAngleMode GetFixAngleMode() const { return static_cast<FixAngleMode>( pev->fixangle ); }

	/**
	*	Sets the fixangle mode.
	*	@param mode Mode.
	*	@see FixAngleMode
	*/
	void SetFixAngleMode( const FixAngleMode mode )
	{
		pev->fixangle = mode;
	}

	/**
	*	@return The ideal pitch.
	*/
	float GetIdealPitch() const { return pev->idealpitch; }

	/**
	*	Sets the ideal pitch.
	*	@param flIdealPitch Ideal pitch to set.
	*/
	void SetIdealPitch( const float flIdealPitch )
	{
		pev->idealpitch = flIdealPitch;
	}

	/**
	*	@return The pitch speed.
	*/
	float GetPitchSpeed() const { return pev->pitch_speed; }

	/**
	*	Sets the pitch speed.
	*	@param flPitchSpeed Pitch speed to set.
	*/
	void SetPitchSpeed( const float flPitchSpeed )
	{
		pev->pitch_speed = flPitchSpeed;
	}

	/**
	*	@return The ideal yaw.
	*/
	float GetIdealYaw() const { return pev->ideal_yaw; }

	/**
	*	Sets the ideal yaw.
	*	@param flIdealYaw Ideal yaw to set.
	*/
	void SetIdealYaw( const float flIdealYaw )
	{
		pev->ideal_yaw = flIdealYaw;
	}

	/**
	*	@return The pitch speed.
	*/
	float GetYawSpeed() const { return pev->yaw_speed; }

	/**
	*	Sets the yaw speed.
	*	@param flYawSpeed Yaw speed to set.
	*/
	void SetYawSpeed( const float flYawSpeed )
	{
		pev->yaw_speed = flYawSpeed;
	}

	/**
	*	@return The model index.
	*/
	int GetModelIndex() const { return pev->modelindex; }

	/**
	*	Sets the model index.
	*	@param iModelIndex Model index to set.
	*/
	void SetModelIndex( const int iModelIndex )
	{
		pev->modelindex = iModelIndex;
	}

	/**
	*	@return Whether this entity has a model.
	*/
	bool HasModel() const { return !!( *STRING( pev->model ) ); }

	/**
	*	@return The model's name.
	*/
	const char* GetModelName() const { return STRING( pev->model ); }

	/**
	*	Sets the model name. Does not set the model itself.
	*	@param pszModelName Name of the model.
	*/
	void SetModelName( const char* const pszModelName )
	{
		pev->model = MAKE_STRING( pszModelName );
	}

	/**
	*	Sets the model name. Does not set the model itself.
	*	@param iszModelName Name of the model.
	*/
	void SetModelName( const string_t iszModelName )
	{
		pev->model = iszModelName;
	}

	/**
	*	Sets the model.
	*	@param pszModelName Name of the model.
	*/
	void SetModel( const char* const pszModelName )
	{
		SET_MODEL( edict(), pszModelName );
	}

	/**
	*	Sets the model.
	*	@param iszModelName Name of the model.
	*/
	void SetModel( const string_t iszModelName )
	{
		SET_MODEL( edict(), STRING( iszModelName ) );
	}

	/**
	*	Clears the model.
	*/
	void ClearModel()
	{
		pev->model = iStringNull;
		SetModelIndex( 0 );
	}

	/**
	*	@return The view model name.
	*/
	const char* GetViewModelName() const { return STRING( pev->viewmodel ); }

	/**
	*	Sets the view model name.
	*	@param pszModelName Model name.
	*/
	void SetViewModelName( const char* const pszModelName )
	{
		pev->viewmodel = MAKE_STRING( pszModelName );
	}

	/**
	*	Sets the view model name.
	*	@param iszModelName Model name.
	*/
	void SetViewModelName( const string_t iszModelName )
	{
		pev->viewmodel = iszModelName;
	}

	/**
	*	Clears the view model name.
	*/
	void ClearViewModelName()
	{
		pev->viewmodel = iStringNull;
	}

	/**
	*	@return The third person weapon model name.
	*/
	const char* GetWeaponModelName() const { return STRING( pev->weaponmodel ); }

	/**
	*	Sets the weapon model name.
	*	@param pszModelName Model name.
	*/
	void SetWeaponModelName( const char* const pszModelName )
	{
		pev->weaponmodel = MAKE_STRING( pszModelName );
	}

	/**
	*	Sets the weapon model name.
	*	@param iszModelName Model name.
	*/
	void SetWeaponModelName( const string_t iszModelName )
	{
		pev->weaponmodel = iszModelName;
	}

	/**
	*	Clears the weapon model name.
	*/
	void ClearWeaponModelName()
	{
		pev->weaponmodel = iStringNull;
	}

	/**
	*	@return The weapon animation.
	*/
	int GetWeaponAnim() const { return pev->weaponanim; }

	/**
	*	Sets the weapon animation.
	*	@param iWeaponAnim Weapon animation to set.
	*/
	void SetWeaponAnim( const int iWeaponAnim )
	{
		pev->weaponanim = iWeaponAnim;
	}

	/**
	*	@return Whether the player is ducking.
	*/
	bool IsDucking() const
	{
		return pev->bInDuck != 0;
	}

	/**
	*	Sets whether the player is ducking.
	*	@param bDucking Duck state to set.
	*/
	void SetDucking( const bool bDucking )
	{
		pev->bInDuck = bDucking;
	}

	/**
	*	@return The time at which a step sound was last played.
	*/
	int GetStepSoundTime() const { return pev->flTimeStepSound; }

	/**
	*	Sets the time at which a step sound was last played.
	*	@param iTime Time to set.
	*/
	void SetStepSoundTime( const int iTime )
	{
		pev->flTimeStepSound = iTime;
	}

	/**
	*	@return The time at which a swim sound was last played.
	*/
	int GetSwimSoundTime() const { return pev->flSwimTime; }

	/**
	*	Sets the time at which a swim sound was last played.
	*	@param iTime Time to set.
	*/
	void SetSwimSoundTime( const int iTime )
	{
		pev->flSwimTime = iTime;
	}

	/**
	*	@return The time at which a duck sound was last played.
	*/
	int GetDuckSoundTime() const { return pev->flDuckTime; }

	/**
	*	Sets the time at which a duck sound was last played.
	*	@param iTime Time to set.
	*/
	void SetDuckSoundTime( const int iTime )
	{
		pev->flDuckTime = iTime;
	}

	/**
	*	@return Whether the last step was a left step sound.
	*/
	bool IsStepLeft() const
	{
		return pev->iStepLeft != 0;
	}

	/**
	*	Steps whether the last step was a left step sound.
	*	@param bStepLeft State to set.
	*/
	void SetStepLeft( const bool bStepLeft )
	{
		pev->iStepLeft = bStepLeft;
	}

	/**
	*	@return Fall velocity.
	*/
	float GetFallVelocity() const { return pev->flFallVelocity; }

	/**
	*	Sets the fall velocity.
	*	@param flFallVelocity Fall velocity to set.
	*/
	void SetFallVelocity( const float flFallVelocity )
	{
		pev->flFallVelocity = flFallVelocity;
	}

	/**
	*	@return The absolute minimum bounds.
	*/
	const Vector& GetAbsMin() const { return pev->absmin; }

	/**
	*	Sets the absolute minimum bounds.
	*	@param vecMin Minimum bounds to set.
	*/
	void SetAbsMin( const Vector& vecMin )
	{
		pev->absmin = vecMin;
	}

	/**
	*	@return The absolute maximum bounds.
	*/
	const Vector& GetAbsMax() const { return pev->absmax; }

	/**
	*	Sets the absolute maximum bounds.
	*	@param vecMax Maximum bounds to set.
	*/
	void SetAbsMax( const Vector& vecMax )
	{
		pev->absmax = vecMax;
	}

	/**
	*	@return The relative minimum bounds.
	*/
	const Vector& GetRelMin() const { return pev->mins; }

	/**
	*	Sets the relative minimum bounds.
	*	@param vecMin Minimum bounds to set.
	*/
	void SetRelMin( const Vector& vecMin )
	{
		pev->mins = vecMin;
	}

	/**
	*	@return The relative maximum bounds.
	*/
	const Vector& GetRelMax() const { return pev->maxs; }

	/**
	*	Sets the relative maximum bounds.
	*	@param vecMax Maximum bounds to set.
	*/
	void SetRelMax( const Vector& vecMax )
	{
		pev->maxs = vecMax;
	}

	/**
	*	@return The entity's bounds.
	*/
	const Vector& GetBounds() const { return pev->size; }

	/**
	*	Sets the size. The size is centered around the entity's origin.
	*	@param vecSize Size to set.
	*/
	void SetSize( const Vector& vecSize )
	{
		SetSize( -( vecSize / 2 ), vecSize / 2 );
	}

	/**
	*	Sets the size.
	*	@param vecMin Minimum bounds.
	*	@param vecMax Maximum bounds.
	*/
	void SetSize( const Vector& vecMin, const Vector& vecMax )
	{
		UTIL_SetSize( this, vecMin, vecMax );
	}

	/**
	*	Gets the last think time. Set by the engine for brush entities only (MOVETYPE_PUSH).
	*	Should be used with brush entities when setting next think times (MOVETYPE_PUSH); use GetLastThink() + delay.
	*	For other movetypes, use gpGlobals->time + delay.
	*	@return The last think time.
	*/
	float GetLastThink() const { return pev->ltime; }

	/**
	*	Sets the last think time. Should never be used in game code.
	*	@param flLastThink Last think time to set.
	*/
	void SetLastThink( const float flLastThink )
	{
		pev->ltime = flLastThink;
	}

	/**
	*	@return The next think time.
	*/
	float GetNextThink() const { return pev->nextthink; }

	/**
	*	Sets the next think time.
	*	@param flNextThink Next think time to set.
	*/
	void SetNextThink( const float flNextThink )
	{
		pev->nextthink = flNextThink;
	}

	/**
	*	@return The movetype.
	*/
	MoveType GetMoveType() const { return static_cast<MoveType>( pev->movetype ); }

	/**
	*	Sets the movetype.
	*	@param moveType Movetype to set.
	*/
	void SetMoveType( const MoveType moveType )
	{
		pev->movetype = moveType;
	}

	/**
	*	@return The solid type.
	*/
	Solid GetSolidType() const { return static_cast<Solid>( pev->solid ); }

	/**
	*	Sets the solid type.
	*	@param solidType Solid type to set.
	*/
	void SetSolidType( const Solid solidType )
	{
		pev->solid = solidType;
	}

	/**
	*	@return The skin.
	*/
	int GetSkin() const { return pev->skin; }

	/**
	*	Sets the skin.
	*	@param iSkin Skin to set.
	*/
	void SetSkin( const int iSkin )
	{
		pev->skin = iSkin;
	}

	/**
	*	@return The body.
	*/
	int GetBody() const { return pev->body; }

	/**
	*	Sets the body.
	*	@param iBody Body to set.
	*/
	void SetBody( const int iBody )
	{
		pev->body = iBody;
	}

	/**
	*	@return The entity's effects flags.
	*/
	const CBitSet<int>& GetEffects() const
	{
		return *reinterpret_cast<const CBitSet<int>*>( &pev->effects );
	}

	/**
	*	@copydoc GetEffects() const
	*/
	CBitSet<int>& GetEffects()
	{
		return *reinterpret_cast<CBitSet<int>*>( &pev->effects );
	}

	/**
	*	@return The gravity multiplier.
	*/
	float GetGravity() const { return pev->gravity; }

	/**
	*	@return The gravity multiplier, in mutable form.
	*/
	float& GetMutableGravity() { return pev->gravity; }

	/**
	*	Sets the gravity multiplier.
	*	@param flGravity Gravity to set.
	*/
	void SetGravity( const float flGravity )
	{
		pev->gravity = flGravity;
	}

	/**
	*	@return The friction.
	*/
	float GetFriction() const { return pev->friction; }

	/**
	*	@return The friction, in mutable form.
	*/
	float& GetMutableFriction() { return pev->friction; }

	/**
	*	Sets the friction.
	*	@param flFriction Friction to set.
	*/
	void SetFriction( const float flFriction )
	{
		pev->friction = flFriction;
	}

	/**
	*	@return The light level.
	*/
	int GetLightLevel() const
	{
		//pev->lightlevel is not always the actual light level.
		return GETENTITYILLUM( const_cast<edict_t*>( edict() ) );
	}

	/**
	*	@return The sequence.
	*/
	int GetSequence() const { return pev->sequence; }

	/**
	*	Sets the sequence.
	*	@param iSequence Sequence to set.
	*/
	void SetSequence( const int iSequence )
	{
		pev->sequence = iSequence;
	}

	/**
	*	Movement animation sequence for player.
	*	@return The gait sequence.
	*/
	int GetGaitSequence() const { return pev->gaitsequence; }

	/**
	*	Sets the gait sequence.
	*	@param iGaitSequence Gait sequence to set.
	*/
	void SetGaitSequence( const int iGaitSequence )
	{
		pev->gaitsequence = iGaitSequence;
	}

	/**
	*	% playback position in animation sequences (0..255).
	*	@return The frame number, as a float.
	*/
	float GetFrame() const { return pev->frame; }

	/**
	*	@return The frame number, as a mutable float.
	*/
	float& GetMutableFrame() { return pev->frame; }

	/**
	*	Sets the frame number.
	*	@param flFrame Frame to set.
	*/
	void SetFrame( const float flFrame )
	{
		pev->frame = flFrame;
	}

	/**
	*	@return World time when the frame was changed.
	*/
	float GetAnimTime() const { return pev->animtime; }

	/**
	*	Sets the world time when the frame was changed.
	*	@param flAnimTime Time to set.
	*/
	void SetAnimTime( const float flAnimTime )
	{
		pev->animtime = flAnimTime;
	}

	/**
	*	@return The animation playback rate (-8x to 8x).
	*/
	float GetFrameRate() const { return pev->framerate; }

	/**
	*	Sets the animation playback rate.
	*	@param flFrameRate Frame rate to set.
	*/
	void SetFrameRate( const float flFrameRate )
	{
		pev->framerate = flFrameRate;
	}

	/**
	*	@return The sprite rendering scale (0..255).
	*/
	float GetScale() const { return pev->scale; }

	/**
	*	Sets the scale.
	*	@param flScale Scale to set.
	*/
	void SetScale( const float flScale )
	{
		pev->scale = flScale;
	}

	/**
	*	@return The render mode.
	*/
	RenderMode GetRenderMode() const
	{
		return static_cast<RenderMode>( pev->rendermode );
	}

	/**
	*	Sets the render mode.
	*	@param renderMode Render mode.
	*/
	void SetRenderMode( const RenderMode renderMode )
	{
		pev->rendermode = renderMode;
	}

	/**
	*	@return The render amount.
	*/
	float GetRenderAmount() const { return pev->renderamt; }

	/**
	*	Sets the render amount.
	*	@param flRenderAmount Render amount.
	*/
	void SetRenderAmount( const float flRenderAmount )
	{
		pev->renderamt = flRenderAmount;
	}

	/**
	*	@return The render color.
	*/
	const Vector& GetRenderColor() const { return pev->rendercolor; }

	/**
	*	@return The render color, in mutable form.
	*/
	Vector& GetMutableRenderColor() { return pev->rendercolor; }

	/**
	*	Sets the render color.
	*	@param vecColor Render color to set.
	*/
	void SetRenderColor( const Vector& vecColor )
	{
		pev->rendercolor = vecColor;
	}

	/**
	*	@return The render FX.
	*/
	RenderFX GetRenderFX() const
	{
		return static_cast<RenderFX>( pev->renderfx );
	}

	/**
	*	Sets the render FX.
	*	@param renderFX Render FX to set.
	*/
	void SetRenderFX( const RenderFX renderFX )
	{
		pev->renderfx = renderFX;
	}

	/**
	*	@return Health amount.
	*/
	float GetHealth() const { return pev->health; }

	/**
	*	Sets the entity's health.
	*	@param flHealth Health amount to set.
	*/
	void SetHealth( const float flHealth )
	{
		//TODO: this could cause inconsistent behavior if health < 1. - Solokiller
		pev->health = flHealth;
	}

	/**
	*	@return Maximum health.
	*/
	float GetMaxHealth() const { return pev->max_health; }

	/**
	*	Sets the maximum health.
	*	@param flMaxHealth Maximum health.
	*/
	void SetMaxHealth( const float flMaxHealth )
	{
		pev->max_health = flMaxHealth;
	}

	/**
	*	@return The armor amount.
	*/
	float GetArmorAmount() const { return pev->armorvalue; }

	/**
	*	Sets the armor amount.
	*	@param flArmorAmount Armor amount to set.
	*/
	void SetArmorAmount( const float flArmorAmount )
	{
		pev->armorvalue = flArmorAmount;
	}

	/**
	*	Never used in the SDK, but can be used to store different types of armor.
	*	@return The armor type.
	*/
	float GetArmorType() const { return pev->armortype; }

	/**
	*	Sets the armor type.
	*	@param flArmorType Armor type to set.
	*/
	void SetArmorType( const float flArmorType )
	{
		pev->armortype = flArmorType;
	}

	/**
	*	Player/monster score.
	*	@return Frags amount.
	*/
	float GetFrags() const { return pev->frags; }

	/**
	*	Sets the frags amount.
	*	@param flFrags Frags to set.
	*/
	void SetFrags( const float flFrags )
	{
		pev->frags = flFrags;
	}

	/**
	*	@return The entity's weapons flags.
	*/
	const CBitSet<int>& GetWeapons() const
	{
		return *reinterpret_cast<const CBitSet<int>*>( &pev->weapons );
	}

	/**
	*	@copydoc GetWeapons() const
	*/
	CBitSet<int>& GetWeapons()
	{
		return *reinterpret_cast<CBitSet<int>*>( &pev->weapons );
	}

	/**
	*	@return Takedamage mode.
	*/
	TakeDamageMode GetTakeDamageMode() const
	{
		return static_cast<TakeDamageMode>( static_cast<int>( pev->takedamage ) );
	}

	/**
	*	Sets the takedamage mode.
	*	@param takeDamageMode Takedamage mode to set.
	*/
	void SetTakeDamageMode( const TakeDamageMode takeDamageMode )
	{
		pev->takedamage = takeDamageMode;
	}

	/**
	*	@return DeadFlag.
	*/
	DeadFlag GetDeadFlag() const
	{
		return static_cast<DeadFlag>( pev->deadflag );
	}

	/**
	*	Sets the dead flag.
	*	@param deadFlag Dead flag to set.
	*/
	void SetDeadFlag( const DeadFlag deadFlag )
	{
		pev->deadflag = deadFlag;
	}

	/**
	*	@return View offset.
	*/
	const Vector& GetViewOffset() const { return pev->view_ofs; }

	/**
	*	@return View offset, in mutable form.
	*/
	Vector& GetMutableViewOffset() { return pev->view_ofs; }

	/**
	*	Sets the view offset.
	*	@param vecViewOffset View offset to set.
	*/
	void SetViewOffset( const Vector& vecViewOffset )
	{
		pev->view_ofs = vecViewOffset;
	}

	/**
	*	@return The entity's buttons flags.
	*/
	const CBitSet<int>& GetButtons() const
	{
		return *reinterpret_cast<const CBitSet<int>*>( &pev->button );
	}

	/**
	*	@copydoc GetButtons() const
	*/
	CBitSet<int>& GetButtons()
	{
		return *reinterpret_cast<CBitSet<int>*>( &pev->button );
	}

	/**
	*	@return The entity's old buttons flags.
	*/
	const CBitSet<int>& GetOldButtons() const
	{
		return *reinterpret_cast<const CBitSet<int>*>( &pev->oldbuttons );
	}

	/**
	*	@copydoc GetOldButtons() const
	*/
	CBitSet<int>& GetOldButtons()
	{
		return *reinterpret_cast<CBitSet<int>*>( &pev->oldbuttons );
	}

	/**
	*	@return The entity's impulse flags.
	*/
	const CBitSet<int>& GetImpulse() const
	{
		return *reinterpret_cast<const CBitSet<int>*>( &pev->impulse );
	}

	/**
	*	@copydoc GetImpulse() const
	*/
	CBitSet<int>& GetImpulse()
	{
		return *reinterpret_cast<CBitSet<int>*>( &pev->impulse );
	}

	/**
	*	@return The entity's spawn flags.
	*/
	const CBitSet<int>& GetSpawnFlags() const
	{
		return *reinterpret_cast<const CBitSet<int>*>( &pev->spawnflags );
	}

	/**
	*	@copydoc GetSpawnFlags() const
	*/
	CBitSet<int>& GetSpawnFlags()
	{
		return *reinterpret_cast<CBitSet<int>*>( &pev->spawnflags );
	}

	/**
	*	@return The entity's flags.
	*/
	const CBitSet<int>& GetFlags() const
	{
		return *reinterpret_cast<const CBitSet<int>*>( &pev->flags );
	}

	/**
	*	@copydoc GetFlags() const
	*/
	CBitSet<int>& GetFlags()
	{
		return *reinterpret_cast<CBitSet<int>*>( &pev->flags );
	}

	/**
	*	@return The color map.
	*/
	int GetColorMap() const { return pev->colormap; }

	/**
	*	Sets the color map.
	*	@param iColorMap Color map to set.
	*/
	void SetColorMap( const int iColorMap )
	{
		pev->colormap = iColorMap;
	}

	/**
	*	Gets the color map as its top and bottom values.
	*	@param[ out ] iTopColor Top color.
	*	@param[ out ] iBottomColor Bottom color.
	*/
	void GetColorMap( int& iTopColor, int& iBottomColor ) const
	{
		iTopColor = GetColorMap() & 0xFF;
		iBottomColor = ( GetColorMap() & 0xFF00 ) >> 8;
	}

	/**
	*	Sets the color map as its top and bottom values.
	*/
	void SetColorMap( const int iTopColor, const int iBottomColor )
	{
		SetColorMap( ( iTopColor & 0xFF ) | ( ( iBottomColor << 8 ) & 0xFF00 ) );
	}

	/**
	*	@return The team ID.
	*/
	int GetTeamID() const { return pev->team; }

	/**
	*	Sets the team ID.
	*/
	void SetTeamID( const int iTeamID )
	{
		pev->team = iTeamID;
	}

	/**
	*	@return The player class.
	*/
	int GetPlayerClass() const { return pev->playerclass; }

	/**
	*	Sets the player class.
	*	@param iPlayerClass Player class to set.
	*/
	void SetPlayerClass( const int iPlayerClass )
	{
		pev->playerclass = iPlayerClass;
	}

	/**
	*	@return The entity's water level.
	*/
	WaterLevel GetWaterLevel() const
	{
		return static_cast<WaterLevel>( pev->waterlevel );
	}

	/**
	*	Sets the water level.
	*	@param waterLevel Water level to set.
	*/
	void SetWaterLevel( const WaterLevel waterLevel )
	{
		pev->waterlevel = waterLevel;
	}

	/**
	*	@return Water type.
	*/
	Contents GetWaterType() const
	{
		return static_cast<Contents>( pev->watertype );
	}

	/**
	*	Sets the water type.
	*	@param waterType Water type to set.
	*/
	void SetWaterType( const Contents waterType )
	{
		pev->watertype = waterType;
	}

	/**
	*	@return Whether this entity has a message.
	*/
	bool HasMessage() const
	{
		return !!( *STRING( pev->message ) );
	}

	/**
	*	@return The message.
	*/
	const char* GetMessage() const { return STRING( pev->message ); }

	/**
	*	Sets the message.
	*	@param iszMessage Message to set.
	*/
	void SetMessage( const string_t iszMessage )
	{
		pev->message = iszMessage;
	}

	/**
	*	Sets the message.
	*	@param pszMessage Message to set.
	*/
	void SetMessage( const char* const pszMessage )
	{
		SetMessage( MAKE_STRING( pszMessage ) );
	}

	/**
	*	Clears the message.
	*/
	void ClearMessage()
	{
		pev->message = iStringNull;
	}

	/**
	*	@return Speed.
	*/
	float GetSpeed() const { return pev->speed; }

	/**
	*	Sets the speed.
	*	@param flSpeed Speed to set.
	*/
	void SetSpeed( const float flSpeed )
	{
		pev->speed = flSpeed;
	}

	/**
	*	@return SMaximum seed.
	*/
	float GetMaxSpeed() const { return pev->maxspeed; }

	/**
	*	Sets the maximum speed.
	*	@param flSpeed Maximum speed to set.
	*/
	void SetMaxSpeed( const float flSpeed )
	{
		pev->maxspeed = flSpeed;
	}

	/**
	*	Time at which this entity runs out of air. Used while swimming.
	*	@return The air finished time.
	*/
	float GetAirFinishedTime() const { return pev->air_finished; }

	/**
	*	Sets the air finished time.
	*	@param flTime Time when this entity runs out of air.
	*/
	void SetAirFinishedTime( const float flTime )
	{
		pev->air_finished = flTime;
	}

	/**
	*	Time at which this entity can be hurt again while drowning.
	*	@return The pain finished time.
	*/
	float GetPainFinishedTime() const { return pev->pain_finished; }

	/**
	*	Sets the pain finished time.
	*	@param flTime Time when this entity should get hurt again.
	*/
	void SetPainFinishedTime( const float flTime )
	{
		pev->pain_finished = flTime;
	}

	/**
	*	@return Field of view.
	*/
	float GetFOV() const { return pev->fov; }
	
	/**
	*	Sets the field of view.
	*	@param flFOV Field of view to set.
	*/
	void SetFOV( const float flFOV )
	{
		pev->fov = flFOV;
	}

	/**
	*	@return The damage value.
	*/
	float GetDamage() const { return pev->dmg; }

	/**
	*	Sets the damage value.
	*	@param flDamage Damage value to set.
	*/
	void SetDamage( const float flDamage )
	{
		pev->dmg = flDamage;
	}

	/**
	*	@return This entity's owner.
	*/
	CBaseEntity* GetOwner()
	{
		return pev->owner ? Instance( pev->owner ) : nullptr;
	}

	/**
	*	Sets this entity's owner.
	*	@param pOwner Owner to set. Can be null.
	*/
	void SetOwner( CBaseEntity* pOwner )
	{
		pev->owner = pOwner ? pOwner->edict() : nullptr;
	}

	/**
	*	@return The entity that this entity is standing on, if any.
	*/
	CBaseEntity* GetGroundEntity()
	{
		return pev->groundentity ? Instance( pev->groundentity ) : nullptr;
	}

	/**
	*	Sets this entity's ground entity.
	*	Don't forget to set or clear FL_ONGROUND.
	*	@see FL_ONGROUND
	*	@param pEntity Entity to set. Can be null.
	*/
	void SetGroundEntity( CBaseEntity* pEntity )
	{
		pev->groundentity = pEntity ? pEntity->edict() : nullptr;
	}

	/**
	*	TODO: hardly ever used, remove all uses. - Solokiller
	*	@return The PEV enemy.
	*/
	CBaseEntity* GetPEVEnemy()
	{
		return pev->enemy ? Instance( pev->enemy ) : nullptr;
	}

	/**
	*	@return The next entity in the chain of entities.
	*/
	CBaseEntity* GetChain()
	{
		return pev->chain ? Instance( pev->chain ) : nullptr;
	}

	/**
	*	Sets the next entity in the chain.
	*	@param pEntity Entity to set. Can be null.
	*/
	void SetChain( CBaseEntity* pEntity )
	{
		pev->chain = pEntity ? pEntity->edict() : nullptr;
	}

	/**
	*	@return Whether this entity has a noise.
	*/
	bool HasNoise() const
	{
		return !!( *STRING( pev->noise ) );
	}

	/**
	*	@return The noise.
	*/
	const char* GetNoise() const { return STRING( pev->noise ); }

	/**
	*	Sets the noise.
	*	@param iszNoise Noise to set.
	*/
	void SetNoise( const string_t iszNoise )
	{
		pev->noise = iszNoise;
	}

	/**
	*	Sets the noise.
	*	@param pszNoise Noise to set.
	*/
	void SetNoise( const char* const pszNoise )
	{
		SetNoise( MAKE_STRING( pszNoise ) );
	}

	/**
	*	Clears the noise.
	*/
	void ClearNoise()
	{
		pev->noise = iStringNull;
	}

	/**
	*	@return Whether this entity has a noise1.
	*/
	bool HasNoise1() const
	{
		return !!( *STRING( pev->noise1 ) );
	}

	/**
	*	@return The noise1.
	*/
	const char* GetNoise1() const { return STRING( pev->noise1 ); }

	/**
	*	Sets the noise1.
	*	@param iszNoise Noise1 to set.
	*/
	void SetNoise1( const string_t iszNoise )
	{
		pev->noise1 = iszNoise;
	}

	/**
	*	Sets the noise1.
	*	@param pszNoise Noise1 to set.
	*/
	void SetNoise1( const char* const pszNoise )
	{
		SetNoise1( MAKE_STRING( pszNoise ) );
	}

	/**
	*	Clears the noise1.
	*/
	void ClearNoise1()
	{
		pev->noise1 = iStringNull;
	}

	/**
	*	@return Whether this entity has a noise2.
	*/
	bool HasNoise2() const
	{
		return !!( *STRING( pev->noise2 ) );
	}

	/**
	*	@return The noise2.
	*/
	const char* GetNoise2() const { return STRING( pev->noise2 ); }

	/**
	*	Sets the noise2.
	*	@param iszNoise Noise2 to set.
	*/
	void SetNoise2( const string_t iszNoise )
	{
		pev->noise2 = iszNoise;
	}

	/**
	*	Sets the noise2.
	*	@param pszNoise Noise2 to set.
	*/
	void SetNoise2( const char* const pszNoise )
	{
		SetNoise2( MAKE_STRING( pszNoise ) );
	}

	/**
	*	Clears the noise.
	*/
	void ClearNoise2()
	{
		pev->noise2 = iStringNull;
	}

	/**
	*	@return Whether this entity has a noise.
	*/
	bool HasNoise3() const
	{
		return !!( *STRING( pev->noise3 ) );
	}

	/**
	*	@return The noise.
	*/
	const char* GetNoise3() const { return STRING( pev->noise3 ); }

	/**
	*	Sets the noise3.
	*	@param iszNoise Noise3 to set.
	*/
	void SetNoise3( const string_t iszNoise )
	{
		pev->noise3 = iszNoise;
	}

	/**
	*	Sets the noise3.
	*	@param pszNoise Noise3 to set.
	*/
	void SetNoise3( const char* const pszNoise )
	{
		SetNoise3( MAKE_STRING( pszNoise ) );
	}

	/**
	*	Clears the noise3.
	*/
	void ClearNoise3()
	{
		pev->noise3 = iStringNull;
	}

	//TODO: edict_t* pointers - Solokiller
	//TODO: dmg* vars

public:
	CBaseEntity()
	{
	}

	virtual ~CBaseEntity()
	{
	}

	/**
	*	Called when the entity is first created. - Solokiller
	*/
	virtual void OnCreate() {}

	/**
	*	Called when the entity is destroyed. - Solokiller
	*	Do not access other entities or do anything beyond simple cleanup and memory freeing.
	*/
	virtual void  OnDestroy() {}

	/**
	*	Called when an entity is removed at runtime. Gives entities a chance to respond to it. Not called during map change or shutdown.
	*	Call the baseclass version after handling it.
	*	Used to be non-virtual - Solokiller
	*/
	virtual void UpdateOnRemove();

	// initialization functions

	/**
	*	Called once for each keyvalue provided in the bsp file for this entity.
	*	@param pkvd Keyvalue data. Set pkvd->fHandled to true if you handled the key.
	*/
	virtual void KeyValue( KeyValueData* pkvd );

	/**
	*	Called when the entity should precache its resources.
	*	Should call the baseclass implementation first.
	*/
	virtual void Precache() {}

	/**
	*	Called after all keyvalues have been passed in.
	*	Should call Precache.
	*/
	virtual void Spawn() {}

	/**
	*	Called when the server activates. Gives entities a chance to connect with eachother.
	*	Is not called if the entity is created at runtime.
	*	If the entity has the FL_DORMANT set, this will not be called.
	*/
	virtual void Activate() {}

	/**
	*	Called when the entity is being saved to a save game file.
	*	Call the baseclass implementation first, return false on failure.
	*	@param save Save data.
	*	@return true if the entity was successfully saved, false otherwise.
	*/
	virtual bool Save( CSave &save );

	/**
	*	Called when the entity is being restored from a save game file.
	*	Call the baseclass implementation first, return false on failure.
	*	@param restore Restore data.
	*	@return true if the entity was successfully restored, false otherwise.
	*/
	virtual bool Restore( CRestore &restore );

	/**
	*	Object capabilities.
	*	@return A bit vector of FCapability values.
	*	@see FCapability
	*/
	virtual int ObjectCaps() const { return FCAP_ACROSS_TRANSITION; }

	/**
	*	Setup the object->object collision box (pev->mins / pev->maxs is the object->world collision box)
	*/
	virtual void SetObjectCollisionBox();

	/**
	*	Respawns the entity. Entities that can be respawned should override this and return a new instance.
	*/
	virtual CBaseEntity* Respawn() { return nullptr; }

	/**
	*	@return This entity's edict.
	*/
	edict_t* edict() const { return ENT( pev ); }

	/**
	*	@copydoc edict() const
	*/
	edict_t* edict() { return ENT( pev ); }

	/**
	*	@return Offset of this entity. This is the byte offset in the edict array.
	*	DO NOT USE THIS. Use entindex instead.
	*/
	EOFFSET eoffset() const { return OFFSET( pev ); }

	/**
	*	@return The index of this entity.
	*	0 is worldspawn.
	*	[ 1, gpGlobals->maxClients ] are players.
	*	] gpGlobals->maxClients, gpGlobals->maxEntities [ are normal entities.
	*/
	int entindex() const { return ENTINDEX( edict() ); }

	// fundamental callbacks
	BASEPTR			m_pfnThink;
	ENTITYFUNCPTR	m_pfnTouch;
	USEPTR			m_pfnUse;
	ENTITYFUNCPTR	m_pfnBlocked;

	virtual void Think()
	{
		if( m_pfnThink )
			( this->*m_pfnThink )();
	}

	virtual void Touch( CBaseEntity *pOther )
	{
		if( m_pfnTouch )
			( this->*m_pfnTouch )( pOther );
	}

	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
	{
		if( m_pfnUse )
			( this->*m_pfnUse )( pActivator, pCaller, useType, value );
	}

	virtual void Blocked( CBaseEntity *pOther )
	{
		if( m_pfnBlocked )
			( this->*m_pfnBlocked )( pOther );
	}

	// Ugly code to lookup all functions to make sure they are exported when set.
#ifdef _DEBUG
	template<typename FUNCPTR>
	void FunctionCheck( FUNCPTR pFunction, char *name )
	{
		if( pFunction && !UTIL_NameFromFunction( *GetDataMap(), pFunction ) )
		{
			//Note: pointers to members may be larger than 4 bytes, so the address won't always be printed out correctly.
			//In the case of CBaseEntity, it *should* be fine since it uses single inheritance, but there's no guarantee it'll be the same on every platform. - Solokiller
			union Convert
			{
				BASEPTR ptr;
				uint32 uiValue;
			};

			Convert convert;

			convert.ptr = pFunction;

			ALERT( at_error, "No EXPORT: %s:%s (%08lx)\n", GetClassname(), name, convert.uiValue );
		}
	}

	BASEPTR ThinkSet( BASEPTR func, char *name )
	{
		m_pfnThink = func;
		FunctionCheck( *( ( BASEPTR* ) ( ( char* ) this + ( OFFSETOF( CBaseEntity, m_pfnThink ) ) ) ), name );
		return func;
	}
	ENTITYFUNCPTR TouchSet( ENTITYFUNCPTR func, char *name )
	{
		m_pfnTouch = func;
		FunctionCheck( *( ( BASEPTR* ) ( ( char* ) this + ( OFFSETOF( CBaseEntity, m_pfnTouch ) ) ) ), name );
		return func;
	}
	USEPTR UseSet( USEPTR func, char *name )
	{
		m_pfnUse = func;
		FunctionCheck( *( ( BASEPTR* ) ( ( char* ) this + ( OFFSETOF( CBaseEntity, m_pfnUse ) ) ) ), name );
		return func;
	}
	ENTITYFUNCPTR BlockedSet( ENTITYFUNCPTR func, char *name )
	{
		m_pfnBlocked = func;
		FunctionCheck( *( ( BASEPTR* ) ( ( char* ) this + ( OFFSETOF( CBaseEntity, m_pfnBlocked ) ) ) ), name );
		return func;
	}

#endif

	// allow engine to allocate instance data
	void *operator new( size_t stAllocateBlock, entvars_t *pev )
	{
		return ( void* ) ALLOC_PRIVATE( ENT( pev ), stAllocateBlock );
	}

	// don't use this.
#if _MSC_VER >= 1200 // only build this code if MSVC++ 6.0 or higher
	void operator delete( void *pMem, entvars_t *pev )
	{
		pev->flags |= FL_KILLME;
	}
#endif

	/**
	*	Returns the CBaseEntity instance of the given edict.
	*	@param pent Edict whose instance should be returned. If this is null, uses worldspawn.
	*	@return Entity instance, or null if no instance is assigned to it.
	*/
	static CBaseEntity *Instance( edict_t *pent )
	{
		if( !pent )
			pent = ENT( 0 );
		CBaseEntity *pEnt = ( CBaseEntity * ) GET_PRIVATE( pent );
		return pEnt;
	}

	/**
	*	Returns the CBaseEntity instance of the given entvars.
	*	@param pev Entvars whose instance should be returned.
	*	@return Entity instance, or null if no instance is assigned to it.
	*/
	static CBaseEntity *Instance( entvars_t *pev ) { return Instance( ENT( pev ) ); }

	/**
	*	Returns the CBaseEntity instance of the given eoffset.
	*	@param eoffset Entity offset whose instance should be returned.
	*	@return Entity instance, or null if no instance is assigned to it.
	*/
	static CBaseEntity *Instance( EOFFSET eoffset ) { return Instance( ENT( eoffset ) ); }

	/**
	*	Creates an entity by class name.
	*	@param pszName Class name of the entity. This string must continue to exist for at least as long as the map itself.
	*	@param vecOrigin Intended entity origin.
	*	@param vecAngles Intended entity angles.
	*	@param pentOwner Optional. The owner of the newly created entity.
	*	@param bSpawnEntity Whether to call spawn on the entity or not.
	*	@return Newly created entity, or null if the entity could not be created.
	*/
	static CBaseEntity* Create( const char* const pszName, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner = nullptr, const bool bSpawnEntity = true );

	/**
	*	@return This entity's blood color.
	*	@see BloodColor
	*/
	virtual int BloodColor() const { return DONT_BLEED; }

	/**
	*	Used when handling damage.
	*	This gives the entity a chance to filter damage. This should add the given damage to the global multi-damage instance if damage is not filtered.
	*	This is where blood, ricochets, and other effects should be handled.
	*	@param info Damage info.
	*	@param vecDir Direction of the attack.
	*	@param ptr Traceline that represents the attack.
	*	@see g_MultiDamage
	*/
	virtual void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult* ptr );

	/**
	*	Projects blood decals based on the given damage and traceline.
	*	@param info Damage info.
	*	@param vecDir attack direction.
	*	@param ptr Attack traceline.
	*	@see Damage
	*/
	virtual void TraceBleed( const CTakeDamageInfo& info, Vector vecDir, TraceResult* ptr );

	/**
	*	Deals damage to this entity.
	*	@param info Damage info.
	*/
	void TakeDamage( const CTakeDamageInfo& info );

	/**
	*	Deals damage to this entity.
	*	@param pInflictor The entity that is responsible for dealing the damage.
	*	@param pAttacker The entity that owns pInflictor. This may be identical to pInflictor if it directly attacked this entity.
	*	@param flDamage Amount of damage to deal.
	*	@param bitsDamageType Bit vector of damage types.
	*	@return Whether any damage was dealt.
	*/
	void TakeDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, int bitsDamageType );

protected:
	template<typename CLASS>
	friend void BaseEntity_OnTakeDamage( CLASS* pThis, const CTakeDamageInfo& info );

	/**
	*	Called when this entity is damaged.
	*	Should not be called directly, call TakeDamage.
	*	@param info Damage info.
	*/
	virtual void OnTakeDamage( const CTakeDamageInfo& info );

public:

	/**
	*	Called when the entity has been killed.
	*	@param info Damage info.
	*	@param gibAction how to handle the gibbing of this entity.
	*	@see GibAction
	*/
	virtual void Killed( const CTakeDamageInfo& info, GibAction gibAction );

	/**
	*	Gives health to this entity. Negative values take health.
	*	Used to be called TakeHealth.
	*	- Solokiller
	*	@param flHealth Amount of health to give. Negative values take health.
	*	@param bitsDamageType Damage types bit vector. @see Damage enum.
	*	@return Actual amount of health that was given/taken.
	*/
	virtual float GiveHealth( float flHealth, int bitsDamageType );

	/**
	*	@param pActivator Activator.
	*	@return Whether this entity would be triggered by the given activator.
	*/
	virtual bool IsTriggered( const CBaseEntity* const pActivator ) const { return true; }

	/**
	*	@return This entity as a CBaseMonster instance, or null if it isn't a monster.
	*/
	virtual CBaseMonster* MyMonsterPointer() { return nullptr; }

	/**
	*	@return This entity as a CSquadMonster instance, or null if it isn't a squad monster.
	*/
	virtual CSquadMonster* MySquadMonsterPointer() { return nullptr; }

	//TODO: entities that use this function should check the classname, so casting to the actual type and using it is better than a costly virtual function hack - Solokiller
	virtual float GetDelay() { return 0; }

	/**
	*	@return Whether this entity is moving.
	*/
	virtual bool IsMoving() const { return pev->velocity != g_vecZero; }

	/**
	*	Called when the entity is restored, and the entity either has a global name or was transitioned over. Resets the entity for the current level.
	*/
	virtual void OverrideReset() {}

	/**
	*	Returns the decal to project onto this entity given the damage types inflicted upon it. If this entity is alpha tested, returns -1.
	*	@param bitsDamageType
	*	@return Decal to use, or -1.
	*/
	virtual int DamageDecal( int bitsDamageType ) const;

	// This is ONLY used by the node graph to test movement through a door
	virtual void SetToggleState( int state ) {}

	/**
	*	Checks if the given entity can control this entity.
	*	@param pTest Entity to check for control.
	*	@return true if this entity can be controlled, false otherwise.
	*/
	virtual bool OnControls( const CBaseEntity* const pTest ) const { return false; }

	/**
	*	@return Whether this entity is alive.
	*/
	virtual bool IsAlive() const { return ( pev->deadflag == DEAD_NO ) && pev->health > 0; }

	/**
	*	@return Whether this is a BSP model.
	*/
	virtual bool IsBSPModel() const { return pev->solid == SOLID_BSP || pev->movetype == MOVETYPE_PUSHSTEP; }

	/**
	*	@return Whether gauss gun beams should reflect off of this entity.
	*/
	virtual bool ReflectGauss() const { return ( IsBSPModel() && !pev->takedamage ); }

	/**
	*	@return Whether this entity has the given target.
	*/
	virtual bool HasTarget( string_t targetname ) const { return FStrEq( STRING( targetname ), GetTargetname() ); }

	/**
	*	@return Whether this entity has the given target.
	*/
	bool HasTarget( const char* const pszTargetName ) const { return HasTarget( MAKE_STRING( pszTargetName ) ); }

	/**
	*	@return Whether this entity is positioned in the world.
	*/
	virtual bool IsInWorld() const;

	/**
	*	@return Whether this is a player.
	*/
	virtual	bool IsPlayer() const { return false; }

	/**
	*	@return Whether this is a connected client. Fake clients do not qualify.
	*	TODO: this only applies to players and spectators. Move it? - Solokiller
	*/
	virtual bool IsNetClient() const { return false; }

	/**
	*	@return This entity's team name.
	*/
	virtual const char* TeamID() const { return ""; }

	/**
	*	@return The next entity that this entity should target.
	*/
	virtual CBaseEntity *GetNextTarget();

	// common member functions
	/**
	*	Think function. Removes this entity.
	*/
	void SUB_Remove();

	/**
	*	Think function. Does nothing. Useful for when you need a think function that doesn't actually do anything.
	*/
	void SUB_DoNothing();

	/**
	*	Think function. Sets up the entity to start fading out.
	*/
	void SUB_StartFadeOut();

	/**
	*	Think function. Fades the entity out.
	*/
	void SUB_FadeOut();

	/**
	*	Think function. Calls this entity's use method with USE_TOGGLE.
	*/
	void SUB_CallUseToggle() { this->Use( this, this, USE_TOGGLE, 0 ); }

	/**
	*	Returns whether the entity should toggle, given the use type and current state.
	*	@param useType Use type.
	*	@param currentState The current entity state.
	*	@return Whether the entity should toggle.
	*/
	bool ShouldToggle( USE_TYPE useType, const bool currentState ) const;

	/**
	*	Fires a number of bullets of a given bullet type.
	*	@param cShots Number of shots to fire.
	*	@param vecSrc Bullet origin.
	*	@param vecDirShooting Bullet direction.
	*	@param vecSpread Random bullet spread to apply.
	*	@param flDistance Maximum bullet distance.
	*	@param iBulletType Bullet type to shoot. @see Bullet.
	*	@param iTracerFreq Show a tracer every this many bullets.
	*	@param iDamage Amount of damage to deal. If 0, uses skill cfg settings for the given bullet type.
	*	@param pAttacker Entity responsible for firing the bullets.
	*/
	void FireBullets( const unsigned int cShots,
					  Vector vecSrc, Vector vecDirShooting, Vector vecSpread, 
					  float flDistance, int iBulletType, 
					  int iTracerFreq = 4, int iDamage = 0, CBaseEntity* pAttacker = nullptr );

	/**
	*	Fires a number of bullets of a given bullet type.
	*	@param cShots Number of shots to fire.
	*	@param vecSrc Bullet origin.
	*	@param vecDirShooting Bullet direction.
	*	@param vecSpread Random bullet spread to apply.
	*	@param flDistance Maximum bullet distance.
	*	@param iBulletType Bullet type to shoot. @see Bullet.
	*	@param iTracerFreq Show a tracer every this many bullets.
	*	@param iDamage Amount of damage to deal. If 0, uses skill cfg settings for the given bullet type.
	*	@param pAttacker Entity responsible for firing the bullets.
	*	@param shared_rand Player specific shared random number seed.
	*	@return Bullet spread angle of the last shot for the X and Y axes.
	*/
	Vector FireBulletsPlayer( const unsigned int cShots,
							  Vector vecSrc, Vector vecDirShooting, Vector vecSpread, 
							  float flDistance, int iBulletType, 
							  int iTracerFreq = 4, int iDamage = 0, CBaseEntity* pAttacker = nullptr, int shared_rand = 0 );

	/**
	*	Triggers all of the entities named this->pev->targetname.
	*	@param pActivator Activator to pass.
	*	@param useType Use type to pass.
	*	@param value Value to pass.
	*/
	void SUB_UseTargets( CBaseEntity *pActivator, USE_TYPE useType, float value );

	/**
	*	@return Whether the bounding boxes of this and the given entity intersect.
	*/
	bool Intersects( const CBaseEntity* const pOther ) const;

	/**
	*	Makes this entity dormant. Dormant entities are not solid, don't move, don't think and have the FL_DORMANT flag set.
	*	Activate will not be called on server activation for dormant entities.
	*	@see Activate()
	*/
	void MakeDormant();

	/**
	*	@return Whether this entity is dormant.
	*/
	bool IsDormant() const;

	/**
	*	Made this virtual. Used to be non-virtual and redeclared in CBaseToggle - Solokiller
	*	@return Whether this entity is locked by its master. Only applies to entities that support having a master.
	*/
	virtual bool IsLockedByMaster() const { return false; }

	// virtual functions used by a few classes

	/**
	*	Monster maker children use this to tell the monster maker that they have died.
	*/
	virtual void DeathNotice( CBaseEntity* pChild ) {}

	/**
	*	A barnacle is trying to grab this entity.
	*	@return Whether the entity can be grabbed by the barnacle.
	*/
	virtual bool BarnacleVictimGrabbed( CBaseEntity* pBarnacle ) { return false; }

	/**
	*	@return Center point of entity.
	*/
	virtual Vector Center() const { return ( pev->absmax + pev->absmin ) * 0.5; }

	/**
	*	@return Position of eyes.
	*/
	virtual Vector EyePosition() const { return GetAbsOrigin() + pev->view_ofs; }

	/**
	*	@return Position of ears.
	*/
	virtual Vector EarPosition() const { return GetAbsOrigin() + pev->view_ofs; }

	/**
	*	@return Position to shoot at.
	*/
	virtual Vector BodyTarget( const Vector &posSrc ) const { return Center(); }

	/**
	*	@return Entity illumination.
	*/
	virtual int Illumination() const { return GETENTITYILLUM( ENT( pev ) ); }

	/**
	*	@return Whether this entity is visible to the given entity.
	*/
	virtual	bool FVisible( const CBaseEntity *pEntity ) const;

	/**
	*	@return Whether this entity is visible from the given position.
	*/
	virtual	bool FVisible( const Vector &vecOrigin ) const;

	/**
	*	A more accurate ( and slower ) version of FVisible. This will check if this entity can see the target's bounding box.
	*	@param pTarget Entity to check if it's visible.
	*	@param[ out ] vecTargetOrigin If this function returns true, this is the position that the looker can see.
	*	@param flSize Amount to shrink the target's bounding box in all axes.
	*	@return true if the target is visible, false otherwise.
	*/
	bool FBoxVisible( const CBaseEntity* pTarget, Vector& vecTargetOrigin, float flSize = 0.0 ) const;

	//New classification system start. - Solokiller
private:
	//Overrideable by user. Save/restored manually to store off names instead of temporary Ids.
	EntityClassification_t m_ClassificationOverride = INVALID_ENTITY_CLASSIFICATION;

protected:
	/*
	*	The classification for this entity, as defined by the entity itself.
	*	Subclasses can override this to define a classification for themselves.
	*	Classification override is handled by Classify.
	*	Do not call directly.
	*	@see Classify
	*/
	virtual EntityClassification_t GetClassification()
	{
		return EntityClassifications().GetNoneId();
	}

public:
	/**
	*	Gets the original classification. If the classification is overridden, this is the only way to get this value.
	*	Always use Classify unless you need the original classification defined by the entity.
	*	Note: do not replace with GetClassification(), if caching is needed then this is where that caching will be implemented.
	*	@see Classify
	*/
	EntityClassification_t GetOriginalClassification()
	{
		return GetClassification();
	}

	/**
	*	Gets the overridden classification, or INVALID_ENTITY_CLASSIFICATION if it isn't overridden.
	*/
	EntityClassification_t GetClassificationOverride() { return m_ClassificationOverride; }

	/**
	*	Sets the classification of this entity.
	*/
	void SetClassificationOverride( EntityClassification_t classification )
	{
		m_ClassificationOverride = classification;
	}

	/**
	*	Clears the overridden classification.
	*/
	void ClearOverriddenClassification()
	{
		SetClassificationOverride( INVALID_ENTITY_CLASSIFICATION );
	}

	/**
	*	@return Whether the classification for this entity is overridden.
	*/
	bool IsClassificationOverridden() { return GetClassificationOverride() != INVALID_ENTITY_CLASSIFICATION; }

	/*
	*	Returns the type of group (i.e, "houndeye", or "human military" so that monsters with different classnames
	*	still realize that they are teammates. (overridden for monsters that form groups)
	*/
	EntityClassification_t Classify()
	{
		//User has overridden the classification, return setting.
		if( m_ClassificationOverride != INVALID_ENTITY_CLASSIFICATION )
		{
			return m_ClassificationOverride;
		}

		return GetOriginalClassification();
	}

	//New classification system end. - Solokiller

#if USE_ANGELSCRIPT && defined( SERVER_DLL )
public:
	//Angelscript specific extensions. - Solokiller
	CScriptDictionary* GetUserData() const;

private:
	mutable CASRefPtr<CScriptDictionary> m_UserData;

public:
#endif
};

// Ugly technique to override base member functions
// Normally it's illegal to cast a pointer to a member function of a derived class to a pointer to a 
// member function of a base class.  static_cast is a sleezy way around that problem.

#ifdef _DEBUG

#define SetThink( a ) ThinkSet( static_cast<BASEPTR>( a ), #a )
#define SetTouch( a ) TouchSet( static_cast<ENTITYFUNCPTR>( a ), #a )
#define SetUse( a ) UseSet( static_cast<USEPTR>( a ), #a )
#define SetBlocked( a ) BlockedSet( static_cast<ENTITYFUNCPTR>( a ), #a )

#else

#define SetThink( a ) m_pfnThink = static_cast<BASEPTR>( a )
#define SetTouch( a ) m_pfnTouch = static_cast<ENTITYFUNCPTR>( a )
#define SetUse( a ) m_pfnUse = static_cast<USEPTR>( a )
#define SetBlocked( a ) m_pfnBlocked = static_cast<ENTITYFUNCPTR>( a )

#endif

/**
*	Notify the main server interface when entities are created.
*	Defined as a global function to avoid pulling in more dependencies. - Solokiller
*/
void Server_EntityCreated( entvars_t* pev );

/**
*	Converts a entvars_t * to a class pointer
*	It will allocate the class and entity if necessary
*/
template<typename T>
T* GetClassPtr( T* a )
{
	entvars_t* pev = reinterpret_cast<entvars_t*>( a );

	// allocate entity if necessary
	if( pev == nullptr )
		pev = VARS( CREATE_ENTITY() );

	// get the private data
	a = static_cast<T*>( GET_PRIVATE( ENT( pev ) ) );

	if( a == nullptr )
	{
		Server_EntityCreated( pev );
		// allocate private data 
		a = new( pev ) T;
		a->pev = pev;
		//Now calls OnCreate - Solokiller
		a->OnCreate();
	}

	return a;
}

#include "CEntityRegistry.h"

// This is the glue that hooks .MAP entity class names to our CPP classes
// The _declspec forces them to be exported by name so we can do a lookup with GetProcAddress()
// The function is used to intialize / allocate the object for the entity
#define LINK_ENTITY_TO_CLASS(mapClassName,DLLClassName)												\
static CEntityRegistry<DLLClassName> __g_Entity##mapClassName##Reg( #mapClassName, #DLLClassName );	\
																									\
extern "C" DLLEXPORT void mapClassName( entvars_t *pev );											\
void mapClassName( entvars_t *pev ) { GetClassPtr( (DLLClassName *)pev ); }

#endif //GAME_SHARED_CBASEENTITY_SHARED_H