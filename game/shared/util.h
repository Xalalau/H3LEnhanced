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
#ifndef GAME_SHARED_UTIL_H
#define GAME_SHARED_UTIL_H

#include "archtypes.h"     // DAL

//
// Misc utility code
//
#include "entities/NPCs/Activity.h"

#ifndef ENGINECALLBACK_H
#include "enginecallback.h"
#endif

#include "mathlib.h"

#include "shared_game_utils.h"

#include "SVC.h"

#include "sound/Sound.h"

struct texture_t;

extern globalvars_t				*gpGlobals;

// Use this instead of ALLOC_STRING on constant strings
#define STRING(offset)		((const char *)(gpGlobals->pStringBase + (unsigned int)(offset)))
#define MAKE_STRING(str)	((uint64)(str) - (uint64)(STRING(0)))	

// Keeps clutter down a bit, when using a float as a bit-vector
#define SetBits(flBitVector, bits)		((flBitVector) = (int)(flBitVector) | (bits))
#define ClearBits(flBitVector, bits)	((flBitVector) = (int)(flBitVector) & ~(bits))
#define FBitSet(flBitVector, bit)		((int)(flBitVector) & (bit))

//
// Conversion among the three types of "entity", including identity-conversions.
//
#ifdef DEBUG
	extern edict_t *DBG_EntOfVars(const entvars_t *pev);
	inline edict_t *ENT(const entvars_t *pev)	{ return DBG_EntOfVars(pev); }
#else
	inline edict_t *ENT(const entvars_t *pev)	{ return pev->pContainingEntity; }
#endif
inline edict_t *ENT(edict_t *pent)		{ return pent; }
inline edict_t *ENT(EOFFSET eoffset)			{ return (*g_engfuncs.pfnPEntityOfEntOffset)(eoffset); }
edict_t* ENT( CBaseEntity* pEntity );
inline EOFFSET OFFSET(EOFFSET eoffset)			{ return eoffset; }
inline EOFFSET OFFSET(const edict_t *pent)	
{ 
#if _DEBUG
	if ( !pent )
		ALERT( at_error, "Bad ent in OFFSET()\n" );
#endif
	return (*g_engfuncs.pfnEntOffsetOfPEntity)(pent); 
}
inline EOFFSET OFFSET( const entvars_t *pev )				
{ 
#if _DEBUG
	if ( !pev )
		ALERT( at_error, "Bad pev in OFFSET()\n" );
#endif
	return OFFSET( ENT( pev ) ); 
}
inline entvars_t *VARS(entvars_t *pev)					{ return pev; }

inline entvars_t *VARS(edict_t *pent)			
{ 
	if ( !pent )
		return NULL;

	return &pent->v; 
}

inline entvars_t* VARS(EOFFSET eoffset)				{ return VARS(ENT(eoffset)); }
entvars_t* VARS( CBaseEntity* pEntity );
inline int ENTINDEX( const edict_t *pEdict )			{ return (*g_engfuncs.pfnIndexOfEdict)(pEdict); }
inline edict_t* INDEXENT( int iEdictNum )		{ return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum); }
inline void MESSAGE_BEGIN( int iMsgType, int iMsgID, const float *pOrigin, entvars_t *ent ) {
	(*g_engfuncs.pfnMessageBegin)( iMsgType, iMsgID, pOrigin, ENT(ent));
}

void MESSAGE_BEGIN( int iMsgType, int iMsgID, const float* pVecOrigin, CBaseEntity* pPlayer );

// Testing the three types of "entity" for nullity
#define eoNullEntity 0
inline bool FNullEnt(EOFFSET eoffset)		{ return eoffset == eoNullEntity; }
inline bool FNullEnt(const edict_t* pent)	{ return pent == NULL || FNullEnt(OFFSET(pent)); }
inline bool FNullEnt(entvars_t* pev)		{ return pev == NULL || FNullEnt(OFFSET(pev)); }
bool FNullEnt( const CBaseEntity* pEntity );

// Misc useful
CBaseEntity* UTIL_FindClientInPVS( const CBaseEntity* const pPVSEntity );

struct TYPEDESCRIPTION;

class CBaseEntity;
class CBasePlayerWeapon;
class CBasePlayer;

// Misc. Prototypes
void UTIL_SetSize( CBaseEntity* pEntity, const Vector& vecMin, const Vector& vecMax );

extern CBaseEntity	*UTIL_FindEntityInSphere(CBaseEntity *pStartEntity, const Vector &vecCenter, float flRadius);
extern CBaseEntity	*UTIL_FindEntityByString(CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue );
extern CBaseEntity	*UTIL_FindEntityByClassname(CBaseEntity *pStartEntity, const char *szName );
extern CBaseEntity	*UTIL_FindEntityByTargetname(CBaseEntity *pStartEntity, const char *szName );
extern CBaseEntity	*UTIL_FindEntityGeneric(const char *szName, const Vector &vecSrc, float flRadius );

/**
*	For doing a reverse lookup. Say you have a door, and want to find its button.
*/
CBaseEntity* UTIL_FindEntityByTarget( CBaseEntity* pStartEntity, const char* const pszTarget );

/**
*	Gets a CBasePlayer pointer to a player by index.
*	Now returns CBasePlayer - Solokiller
*	@param playerIndex Player's entity index.
*	@return Player if the player is spawned and connected, otherwise returns nullptr.
*/
CBasePlayer* UTIL_PlayerByIndex( int playerIndex );

/**
*	Find a player with a case-insensitive name search.
*	@param pszTestName Player name.
*	@return Player instance, or null if no player exists by that name.
*/
CBasePlayer* UTIL_FindPlayerByName( const char* pszTestName );

// Pass in an array of pointers and an array size, it fills the array and returns the number inserted
extern int			UTIL_MonstersInSphere( CBaseEntity **pList, int listMax, const Vector &center, float radius );
extern int			UTIL_EntitiesInBox( CBaseEntity **pList, int listMax, const Vector &mins, const Vector &maxs, int flagMask );

void UTIL_SetOrigin( CBaseEntity* pEntity, const Vector& vecOrigin );
extern void			UTIL_ParticleEffect		( const Vector &vecOrigin, const Vector &vecDirection, const unsigned int ulColor, const unsigned int ulCount );
extern void			UTIL_ScreenShake		( const Vector &center, float amplitude, float frequency, float duration, float radius );
extern void			UTIL_ScreenShakeAll		( const Vector &center, float amplitude, float frequency, float duration );
extern void			UTIL_ShowMessage		( const char *pString, CBaseEntity *pPlayer );
extern void			UTIL_ShowMessageAll		( const char *pString );
extern void			UTIL_ScreenFadeAll		( const Vector &color, float fadeTime, float holdTime, int alpha, int flags );
extern void			UTIL_ScreenFade			( CBaseEntity *pEntity, const Vector &color, float fadeTime, float fadeHold, int alpha, int flags );

enum IGNORE_MONSTERS
{
	ignore_monsters			= 1,
	dont_ignore_monsters	= 0,
	missile					= 2
};

enum IGNORE_GLASS
{
	ignore_glass		= 1,
	dont_ignore_glass	= 0
};

extern void			UTIL_TraceLine			(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr);
extern void			UTIL_TraceLine			(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr);
void UTIL_TraceMonsterHull( CBaseEntity* pEntity, const Vector& v1, const Vector& v2, IGNORE_MONSTERS igmon, CBaseEntity* pentToSkip, TraceResult& tr );
extern void			UTIL_TraceHull			(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, const Hull::Hull hullNumber, edict_t *pentIgnore, TraceResult *ptr);
TraceResult UTIL_GetGlobalTrace();
extern void			UTIL_TraceModel			(const Vector &vecStart, const Vector &vecEnd, const Hull::Hull hullNumber, edict_t *pentModel, TraceResult *ptr);
extern Vector UTIL_GetAimVector( const CBaseEntity* const pEntity, const float flSpeed );

extern bool			UTIL_IsMasterTriggered( string_t sMaster, const CBaseEntity* const pActivator );
extern void			UTIL_BloodStream( const Vector &origin, const Vector &direction, int color, int amount );
extern void			UTIL_BloodDrips( const Vector &origin, const Vector &direction, int color, int amount );
extern Vector		UTIL_RandomBloodVector( void );
extern bool			UTIL_ShouldShowBlood( int bloodColor );
extern void			UTIL_BloodDecalTrace( TraceResult *pTrace, int bloodColor );
extern void			UTIL_DecalTrace( TraceResult *pTrace, int decalNumber );
extern void			UTIL_PlayerDecalTrace( TraceResult *pTrace, int playernum, int decalNumber, const bool bIsCustom );

/**
*	Projects a gunshot decal.
*	@param pTrace Trace to use when projecting. The decal is projected onto the entity it has hit.
*	@param decalNumber Decal index to use.
*/
extern void			UTIL_GunshotDecalTrace( TraceResult *pTrace, int decalNumber );
void DoSpark( CBaseEntity* pEntity, const Vector &location );

bool UTIL_IsValidEntity( const CBaseEntity* const pEntity );
extern bool			UTIL_TeamsMatch( const char *pTeamName1, const char *pTeamName2 );

/**
*	Creates an entity by name.
*	@param pszClassName Name of the class to create. The string must have a lifetime matching or exceeding that of the map's.
*	@return Entity, or null if it couldn't be created.
*/
CBaseEntity* UTIL_CreateNamedEntity( const char* const pszClassName );

// allows precacheing of other entities
extern void			UTIL_PrecacheOther( const char *szClassname );

// prints a message to each client
extern void			UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL );
inline void			UTIL_CenterPrintAll( const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL ) 
{
	UTIL_ClientPrintAll( HUD_PRINTCENTER, msg_name, param1, param2, param3, param4 );
}

extern bool UTIL_GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerWeapon *pCurrentWeapon );

// prints messages through the HUD
extern void ClientPrint( CBaseEntity* pClient, int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL );

// prints a message to the HUD say (chat)
extern void			UTIL_SayText( const char *pText, CBaseEntity *pEntity );
extern void			UTIL_SayTextAll( const char *pText, CBaseEntity *pEntity );

struct hudtextparms_t
{
	float		x;
	float		y;
	int			effect;
	byte		r1, g1, b1, a1;
	byte		r2, g2, b2, a2;
	float		fadeinTime;
	float		fadeoutTime;
	float		holdTime;
	float		fxTime;
	int			channel;
};

// prints as transparent 'title' to the HUD
extern void			UTIL_HudMessageAll( const hudtextparms_t &textparms, const char *pMessage );
extern void			UTIL_HudMessage( CBaseEntity *pEntity, const hudtextparms_t &textparms, const char *pMessage );

// Writes message to console with timestamp and FragLog header.
extern void			UTIL_LogPrintf( const char* const pszFormat, ... );

/**
*	Writes a message to the server console, even if developer is disabled.
*	@param pszFormat Format string.
*	@param ... Arguments.
*/
void UTIL_ServerPrintf( const char* const pszFormat, ... );

#if !defined( _WIN32 ) && !defined( _rotr )
extern "C" {
	unsigned _rotr( unsigned val, int shift );
}
#endif

// Misc functions
extern void SetMovedir( CBaseEntity* pEntity );
extern Vector VecBModelOrigin( const CBaseEntity* const pBModel );
extern int BuildChangeList( LEVELLIST *pLevelList, int maxList );

//
// Constants that were used only by QC (maybe not used at all now)
//
// Un-comment only as needed
//
enum Language
{
	LANGUAGE_ENGLISH	= 0,
	LANGUAGE_GERMAN		= 1,
	LANGUAGE_FRENCH		= 2,
	LANGUAGE_BRITISH	= 3,
};

extern DLL_GLOBAL int			g_Language;

// func_rotating
#define SF_BRUSH_ROTATE_INSTANT		1
#define SF_PENDULUM_AUTO_RETURN		16

#define PLAYBACK_EVENT( flags, who, index ) PLAYBACK_EVENT_FULL( flags, who, index, 0, g_vecZero, g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#define PLAYBACK_EVENT_DELAY( flags, who, index, delay ) PLAYBACK_EVENT_FULL( flags, who, index, delay, g_vecZero, g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

enum GroupTraceOp
{
	GROUP_OP_AND	= 0,
	GROUP_OP_NAND	= 1
};

extern int g_groupmask;
extern int g_groupop;

class UTIL_GroupTrace
{
public:
	UTIL_GroupTrace( int groupmask, int op );
	~UTIL_GroupTrace( void );

private:
	int m_oldgroupmask, m_oldgroupop;
};

void UTIL_SetGroupTrace( int groupmask, int op );
void UTIL_UnsetGroupTrace( void );

/**
*	From the set of entities named pszName, randomly selects one entity.
*	@param pszName Entity targetname.
*	@return Randomly selected entity, or null if no entities by that name exist.
*/
CBaseEntity* UTIL_RandomTargetname( const char* const pszName );

/**
*	From the set of entities of class pszName, randomly selects one entity.
*	@param pszName Entity classname.
*	@return Randomly selected entity, or null if no entities by that name exist.
*/
CBaseEntity* UTIL_RandomClassname( const char* pszName );

/**
*	Sets the value for a given entity and type description.
*	@param pEntity Entity to use as the base address.
*	@param desc Type description that describes the variable to set.
*	@param pszValue Value to set.
*	@return Whether the field type supports conversion from string.
*/
bool UTIL_SetTypeDescValue( void* pEntity, const TYPEDESCRIPTION& desc, const char* const pszValue );

/**
*	Sets an entvars_t keyvalue, if the key can be found in the datamap.
*/
void EntvarsKeyvalue( entvars_t *pev, KeyValueData *pkvd );

void SetObjectCollisionBox( entvars_t *pev );

/**
*	Finds the entity that the given entity is looking at.
*	Modifies gpGlobals vectors.
*	@param pMe Entity whose angles to use.
*	@return Entity being looked at, or null if no entity could be found.
*/
CBaseEntity* UTIL_FindEntityForward( CBaseEntity* pMe );

/**
*	@copydoc enginefuncs_t::pfnEntitiesInPVS
*/
CBaseEntity* UTIL_FindEntitiesInPVS( CBaseEntity* pEntity );

/**
*	@copydoc enginefuncs_t::pfnMakeStatic
*/
void UTIL_MakeStatic( CBaseEntity* pEntity );

/**
*	@copydoc enginefuncs_t::pfnEntIsOnFloor
*/
bool UTIL_EntIsOnFloor( CBaseEntity* pEntity );

const texture_t* UTIL_TraceTexture( CBaseEntity* pEntity, const Vector& vecStart, const Vector& vecEnd );

enum class DropToFloor
{
	STUCK		= -1,
	TOOFAR		= 0,
	DROPPED		= 1
};

DropToFloor UTIL_DropToFloor( CBaseEntity* pEntity );

bool UTIL_WalkMove( CBaseEntity* pEntity, float yaw, float dist, WalkMove mode );

int UTIL_GetPlayerUserId( CBaseEntity* pEntity );

const char* UTIL_GetPlayerAuthId( const CBaseEntity* pEntity );

bool UTIL_IsBrushEntity( const CBaseEntity* const pEnt );

bool UTIL_IsPointEntity( const CBaseEntity* const pEnt );

inline void Cvar_DirectSet( cvar_t* pCvar, const char* pszValue )
{
	g_engfuncs.pfnCvar_DirectSet( pCvar, pszValue );
}

void Cvar_DirectSet( cvar_t* pCvar, const float flValue );

/*
*	Determine the current # of active players on the server for map cycling logic
*/
int UTIL_CountPlayers();

#endif //GAME_SHARED_UTIL_H
