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
/*

===== player.cpp ========================================================

  functions dealing with the player

*/

#include "extdll.h"
#include "util.h"

#include "cbase.h"
#include "CBasePlayer.h"
#include "Weapons.h"
#include "entities/CCorpse.h"
#include "entities/CSoundEnt.h"
#include "entities/NPCs/Monsters.h"
#include "entities/spawnpoints/CBaseSpawnPoint.h"

#include "gamerules/GameRules.h"
#include "nodes/Nodes.h"
#include "hltv.h"

extern DLL_GLOBAL unsigned int	g_ulModelIndexPlayer;
extern DLL_GLOBAL bool			g_fGameOver;
extern DLL_GLOBAL bool			gDisplayTitle;

bool gInitHUD = true;

// This is a glorious hack to find free space when you've crouched into some solid space
// Our crouching collisions do not work correctly for some reason and this is easier
// than fixing the problem :(
void FixPlayerCrouchStuck( edict_t *pPlayer )
{
	TraceResult trace;

	// Move up as many as 18 pixels if the player is stuck.
	for( int i = 0; i < 18; i++ )
	{
		UTIL_TraceHull( pPlayer->v.origin, pPlayer->v.origin, dont_ignore_monsters, Hull::HEAD, pPlayer, &trace );
		if( trace.fStartSolid )
			pPlayer->v.origin.z++;
		else
			break;
	}
}

int TrainSpeed( int iSpeed, int iMax )
{
	const float fMax = ( float ) iMax;
	const float fSpeed = ( ( float ) iSpeed ) / fMax;

	int iRet = 0;

	if( iSpeed < 0 )
		iRet = TRAIN_BACK;
	else if( iSpeed == 0 )
		iRet = TRAIN_NEUTRAL;
	else if( fSpeed < 0.33 )
		iRet = TRAIN_SLOW;
	else if( fSpeed < 0.66 )
		iRet = TRAIN_MEDIUM;
	else
		iRet = TRAIN_FAST;

	return iRet;
}

//
// ID's player as such.
//
EntityClassification_t CBasePlayer::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::PLAYER );
}

void CBasePlayer::Precache()
{
	// SOUNDS / MODELS ARE PRECACHED in ClientPrecache() (game specific)
	// because they need to precache before any clients have connected

	// init geiger counter vars during spawn and each time
	// we cross a level transition

	m_flgeigerRange = 1000;
	m_igeigerRangePrev = 1000;

	m_bitsDamageType = 0;
	m_bitsHUDDamage = -1;

	m_iClientBattery = -1;

	m_iTrain = TRAIN_NEW;

	m_iUpdateTime = 5;  // won't update for 1/2 a second

	if( gInitHUD )
		m_fInitHUD = true;
}

void CBasePlayer::InitialSpawn()
{
	SetCustomDecalFrames( -1 ); // Assume none;

	// Reset interpolation during first frame
	GetEffects() |= EF_NOINTERP;

	InternalSetObserverMode( 0 );	// disable any spec modes
	SetObserverTargetIndex( 0 );

	SetConnectTime( gpGlobals->time );
	SetConnectState( ConnectState::CONNECTED );

	Spawn();
}

void CBasePlayer::Spawn()
{
	SetClassname( "player" );
	// ############ hu3lifezado ############ //
	// Era 100
	SetHealth(169);
	// ############ //
	SetArmorAmount( 0 );
	SetTakeDamageMode( DAMAGE_AIM );
	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_WALK );
	SetMaxHealth( GetHealth() );
	GetFlags().ClearFlags( ~FL_PROXY );	// keep proxy flag sey by engine
	GetFlags() |= FL_CLIENT;
	pev->air_finished	= gpGlobals->time + 12;
	SetDamage( 2 );				// initial water damage
	GetEffects().ClearAll();
	SetDeadFlag( DEAD_NO );
	pev->dmg_take		= 0;
	pev->dmg_save		= 0;
	SetFriction( 1.0 );
	SetGravity( 1.0 );
	m_bitsHUDDamage		= -1;
	m_bitsDamageType	= 0;
	m_afPhysicsFlags	= 0;
	m_fLongJump			= false;// no longjump module. 

	g_engfuncs.pfnSetPhysicsKeyValue( edict(), "slj", "0" );
	g_engfuncs.pfnSetPhysicsKeyValue( edict(), "hl", "1" );

	SetFOV( 0 );
	m_iFOV = 0;// init field of view.
	m_iClientFOV = -1; // make sure fov reset is sent

	m_flNextDecalTime = 0;// let this player decal as soon as he spawns.

	m_flgeigerDelay = gpGlobals->time + 2.0;	// wait a few seconds until user-defined message registrations
												// are recieved by all clients

	m_flTimeStepSound = 0;
	m_iStepLeft = 0;
	m_flFieldOfView = 0.5;// some monsters use this to determine whether or not the player is looking at them.

	m_bloodColor = BLOOD_COLOR_RED;
	m_flNextAttack = UTIL_WeaponTimeBase();

	m_iFlashBattery = 99;
	m_flFlashLightTime = 1; // force first message

	// dont let uninitialized value here hurt the player
	m_flFallVelocity = 0;

	g_pGameRules->SetDefaultPlayerTeam( this );
	g_pGameRules->GetPlayerSpawnSpot( this );

	SetModel( "models/player.mdl" );
	g_ulModelIndexPlayer = GetModelIndex();
	SetSequence( LookupActivity( ACT_IDLE ) );

	if( GetFlags().Any( FL_DUCKING ) )
		SetSize( VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX );
	else
		SetSize( VEC_HULL_MIN, VEC_HULL_MAX );

	SetViewOffset( VEC_VIEW );
	Precache();
	m_HackedGunPos = Vector( 0, 32, 0 );

	if( m_iPlayerSound == SOUNDLIST_EMPTY )
	{
		ALERT( at_console, "Couldn't alloc player sound slot!\n" );
	}

	m_fNoPlayerSound = false;// normal sound behavior.

	m_pLastItem = NULL;
	m_fInitHUD = true;
	m_iClientHideHUD = -1;  // force this to be recalculated
	m_fWeapon = false;
	m_pClientActiveItem = NULL;
	m_iClientBattery = -1;

	// reset all ammo values to 0
	for( int i = 0; i < CAmmoTypes::MAX_AMMO_TYPES; i++ )
	{
		m_rgAmmo[ i ] = 0;
		m_rgAmmoLast[ i ] = 0;  // client ammo values also have to be reset  (the death hud clear messages does on the client side)
	}

	m_lastx = m_lasty = 0;

	m_flNextChatTime = gpGlobals->time;

	g_pGameRules->PlayerSpawn( this );
}

Vector CBasePlayer::GetGunPosition()
{
	//m_HackedGunPos = GetViewOffset();
	return GetAbsOrigin() + GetViewOffset();
}

void CBasePlayer::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult& tr )
{
	CTakeDamageInfo newInfo = info;

	if ( GetTakeDamageMode() != DAMAGE_NO )
	{
		m_LastHitGroup = tr.iHitgroup;

		switch ( tr.iHitgroup )
		{
		case HITGROUP_GENERIC:
			break;
		case HITGROUP_HEAD:
			newInfo.GetMutableDamage() *= gSkillData.GetPlrHead();
			break;
		case HITGROUP_CHEST:
			newInfo.GetMutableDamage() *= gSkillData.GetPlrChest();
			break;
		case HITGROUP_STOMACH:
			newInfo.GetMutableDamage() *= gSkillData.GetPlrStomach();
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			newInfo.GetMutableDamage() *= gSkillData.GetPlrArm();
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			newInfo.GetMutableDamage() *= gSkillData.GetPlrLeg();
			break;
		default:
			break;
		}

		SpawnBlood( tr.vecEndPos, BloodColor(), newInfo .GetDamage());// a little surface blood.
		TraceBleed( newInfo, vecDir, tr );
		g_MultiDamage.AddMultiDamage( newInfo, this );
	}
}

/*
*	Take some damage.  
*	NOTE: each call to OnTakeDamage with bitsDamageType set to a time-based damage
*	type will cause the damage time countdown to be reset.  Thus the ongoing effects of poison, radiation
*	etc are implemented with subsequent calls to OnTakeDamage using DMG_GENERIC.
*/
void CBasePlayer::OnTakeDamage( const CTakeDamageInfo& info )
{
	//The inflictor must be valid. - Solokiller
	ASSERT( info.GetInflictor() );

	CTakeDamageInfo newInfo = info;

	// have suit diagnose the problem - ie: report damage type
	int bitsDamage = newInfo.GetDamageTypes();
	float flHealthPrev = GetHealth();

	float flBonus = PLAYER_ARMOR_BONUS;
	float flRatio = PLAYER_ARMOR_RATIO;

	if ( ( newInfo.GetDamageTypes() & DMG_BLAST ) && g_pGameRules->IsMultiplayer() )
	{
		// blasts damage armor more.
		flBonus *= 2;
	}

	// Already dead
	if ( !IsAlive() )
		return;
	// go take the damage first

	if ( !g_pGameRules->FPlayerCanTakeDamage( this, newInfo ) )
	{
		// Refuse the damage
		return;
	}

	// keep track of amount of damage last sustained
	m_lastDamageAmount = newInfo.GetDamage();

	// Armor. 
	if ( GetArmorAmount() && !( newInfo.GetDamageTypes() & (DMG_FALL | DMG_DROWN)) )// armor doesn't protect against fall or drown damage!
	{
		float flNew = newInfo.GetDamage() * flRatio;

		float flArmor = ( newInfo.GetDamage() - flNew) * flBonus;

		float flNewArmorValue = GetArmorAmount();

		// Does this use more armor than we have?
		if (flArmor > GetArmorAmount() )
		{
			flArmor = GetArmorAmount();
			flArmor *= (1/flBonus);
			flNew = newInfo.GetDamage() - flArmor;
			flNewArmorValue = 0;
		}
		else
			flNewArmorValue -= flArmor;

		if( !GetFlags().Any( FL_GODMODE ) )
			SetArmorAmount( flNewArmorValue );
		
		newInfo.GetMutableDamage() = flNew;
	}

	// this cast to INT is critical!!! If a player ends up with 0.5 health, the engine will get that
	// as an int (zero) and think the player is dead! (this will incite a clientside screentilt, etc)
	newInfo.GetMutableDamage() = int( newInfo.GetDamage() );
	const float flOldHealth = GetHealth();
	CBaseMonster::OnTakeDamage( newInfo );

	const bool bTookDamage = flOldHealth != GetHealth();

	// reset damage time countdown for each type of time based damage player just sustained

	{
		for (int i = 0; i < CDMG_TIMEBASED; i++)
			if ( newInfo.GetDamageTypes() & (DMG_PARALYZE << i))
				m_rgbTimeBasedDamage[i] = 0;
	}

	// tell director about it
	MESSAGE_BEGIN( MSG_SPEC, SVC_DIRECTOR );
		WRITE_BYTE ( 9 );	// command length in bytes
		WRITE_BYTE ( DRC_CMD_EVENT );	// take damage event
		WRITE_SHORT( entindex() );	// index number of primary entity
		WRITE_SHORT( newInfo.GetInflictor()->entindex() );	// index number of secondary entity
		WRITE_LONG( 5 );   // eventflags (priority and flags)
	MESSAGE_END();


	// how bad is it, doc?

	const bool ftrivial = ( GetHealth() > 75 || m_lastDamageAmount < 5);
	const bool fmajor = (m_lastDamageAmount > 25);
	const bool fcritical = ( GetHealth() < 30);

	// handle all bits set in this damage message,
	// let the suit give player the diagnosis

	// UNDONE: add sounds for types of damage sustained (ie: burn, shock, slash )

	// UNDONE: still need to record damage and heal messages for the following types

		// DMG_BURN	
		// DMG_FREEZE
		// DMG_BLAST
		// DMG_SHOCK

	m_bitsDamageType |= bitsDamage; // Save this so we can report it to the client
	m_bitsHUDDamage = -1;  // make sure the damage bits get resent

	bool bFound = true;

	while ( bTookDamage && (!ftrivial || (bitsDamage & DMG_TIMEBASED)) && bFound && bitsDamage)
	{
		bFound = false;

		if (bitsDamage & DMG_CLUB)
		{
			if (fmajor)
				SetSuitUpdate("!HEV_DMG4", SUIT_SENTENCE, SUIT_NEXT_IN_30SEC);	// minor fracture
			bitsDamage &= ~DMG_CLUB;
			bFound = true;
		}
		if (bitsDamage & (DMG_FALL | DMG_CRUSH))
		{
			if (fmajor)
				SetSuitUpdate("!HEV_DMG5", SUIT_SENTENCE, SUIT_NEXT_IN_30SEC);	// major fracture
			else
				SetSuitUpdate("!HEV_DMG4", SUIT_SENTENCE, SUIT_NEXT_IN_30SEC);	// minor fracture
	
			bitsDamage &= ~(DMG_FALL | DMG_CRUSH);
			bFound = true;
		}
		
		if (bitsDamage & DMG_BULLET)
		{
			if (m_lastDamageAmount > 5)
				SetSuitUpdate("!HEV_DMG6", SUIT_SENTENCE, SUIT_NEXT_IN_30SEC);	// blood loss detected
			//else
			//	SetSuitUpdate("!HEV_DMG0", SUIT_SENTENCE, SUIT_NEXT_IN_30SEC);	// minor laceration
			
			bitsDamage &= ~DMG_BULLET;
			bFound = true;
		}

		if (bitsDamage & DMG_SLASH)
		{
			if (fmajor)
				SetSuitUpdate("!HEV_DMG1", SUIT_SENTENCE, SUIT_NEXT_IN_30SEC);	// major laceration
			else
				SetSuitUpdate("!HEV_DMG0", SUIT_SENTENCE, SUIT_NEXT_IN_30SEC);	// minor laceration

			bitsDamage &= ~DMG_SLASH;
			bFound = true;
		}
		
		if (bitsDamage & DMG_SONIC)
		{
			if (fmajor)
				SetSuitUpdate("!HEV_DMG2", SUIT_SENTENCE, SUIT_NEXT_IN_1MIN);	// internal bleeding
			bitsDamage &= ~DMG_SONIC;
			bFound = true;
		}

		if (bitsDamage & (DMG_POISON | DMG_PARALYZE))
		{
			SetSuitUpdate("!HEV_DMG3", SUIT_SENTENCE, SUIT_NEXT_IN_1MIN);	// blood toxins detected
			bitsDamage &= ~(DMG_POISON | DMG_PARALYZE);
			bFound = true;
		}

		if (bitsDamage & DMG_ACID)
		{
			SetSuitUpdate("!HEV_DET1", SUIT_SENTENCE, SUIT_NEXT_IN_1MIN);	// hazardous chemicals detected
			bitsDamage &= ~DMG_ACID;
			bFound = true;
		}

		if (bitsDamage & DMG_NERVEGAS)
		{
			SetSuitUpdate("!HEV_DET0", SUIT_SENTENCE, SUIT_NEXT_IN_1MIN);	// biohazard detected
			bitsDamage &= ~DMG_NERVEGAS;
			bFound = true;
		}

		if (bitsDamage & DMG_RADIATION)
		{
			SetSuitUpdate("!HEV_DET2", SUIT_SENTENCE, SUIT_NEXT_IN_1MIN);	// radiation detected
			bitsDamage &= ~DMG_RADIATION;
			bFound = true;
		}
		if (bitsDamage & DMG_SHOCK)
		{
			bitsDamage &= ~DMG_SHOCK;
			bFound = true;
		}
	}

	Vector vecPunchAngle = GetPunchAngle();
	vecPunchAngle.x = -2;
	SetPunchAngle( vecPunchAngle );

	if ( bTookDamage && !ftrivial && fmajor && flHealthPrev >= 75)
	{
		// first time we take major damage...
		// turn automedic on if not on
		SetSuitUpdate("!HEV_MED1", SUIT_SENTENCE, SUIT_NEXT_IN_30MIN);	// automedic on

		// give morphine shot if not given recently
		SetSuitUpdate("!HEV_HEAL7", SUIT_SENTENCE, SUIT_NEXT_IN_30MIN);	// morphine shot
	}
	
	if ( bTookDamage && !ftrivial && fcritical && flHealthPrev < 75)
	{

		// already took major damage, now it's critical...
		if ( GetHealth() < 6)
			SetSuitUpdate("!HEV_HLTH3", SUIT_SENTENCE, SUIT_NEXT_IN_10MIN);	// near death
		else if ( GetHealth() < 20)
			SetSuitUpdate("!HEV_HLTH2", SUIT_SENTENCE, SUIT_NEXT_IN_10MIN);	// health critical
	
		// give critical health warnings
		if (!RANDOM_LONG(0,3) && flHealthPrev < 50)
			SetSuitUpdate("!HEV_DMG7", SUIT_SENTENCE, SUIT_NEXT_IN_5MIN); //seek medical attention
	}

	// if we're taking time based damage, warn about its continuing effects
	if ( bTookDamage && ( newInfo.GetDamageTypes() & DMG_TIMEBASED) && flHealthPrev < 75)
	{
		if (flHealthPrev < 50)
		{
			if (!RANDOM_LONG(0,3))
				SetSuitUpdate("!HEV_DMG7", SUIT_SENTENCE, SUIT_NEXT_IN_5MIN); //seek medical attention
		}
		else
			SetSuitUpdate("!HEV_HLTH1", SUIT_SENTENCE, SUIT_NEXT_IN_10MIN);	// health dropping
	}
}

/*
 * GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
 */
void CBasePlayer::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
	// Holster weapon immediately, to allow it to cleanup
	if ( m_pActiveItem )
		m_pActiveItem->Holster();

	g_pGameRules->PlayerKilled( this, info );

	if ( m_pTank != NULL )
	{
		m_pTank->Use( this, this, USE_OFF, 0 );
		m_pTank = NULL;
	}

	// this client isn't going to be thinking for a while, so reset the sound until they respawn
	if ( CSound* pSound = CSoundEnt::SoundPointerForIndex( CSoundEnt::ClientSoundIndex( this ) ) )
	{
		pSound->Reset();
	}

	SetAnimation( PLAYER_DIE );
	
	m_iRespawnFrames = 0;

	SetModelIndex( g_ulModelIndexPlayer );    // don't use eyes

	SetDeadFlag( DEAD_DYING );
	SetMoveType( MOVETYPE_TOSS );
	GetFlags().ClearFlags( FL_ONGROUND );
	if ( GetAbsVelocity().z < 10)
	{
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.z += RANDOM_FLOAT(0,300);
		SetAbsVelocity( vecVelocity );
	}

	// clear out the suit message cache so we don't keep chattering
	SetSuitUpdate(NULL, SUIT_SENTENCE, 0);

	// send "health" update message to zero
	m_iClientHealth = 0;
	MESSAGE_BEGIN( MSG_ONE, gmsgHealth, NULL, this );
		WRITE_BYTE( m_iClientHealth );
	MESSAGE_END();

	// Tell Ammo Hud that the player is dead
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, this );
		WRITE_BYTE(0);
		WRITE_BYTE(0XFF);
		WRITE_BYTE(0xFF);
	MESSAGE_END();

	// reset FOV
	SetFOV( m_iFOV );
	m_iFOV = m_iClientFOV = 0;

	MESSAGE_BEGIN( MSG_ONE, gmsgSetFOV, NULL, this );
		WRITE_BYTE(0);
	MESSAGE_END();


	// UNDONE: Put this in, but add FFADE_PERMANENT and make fade time 8.8 instead of 4.12
	// UTIL_ScreenFade( edict(), Vector(128,0,0), 6, 15, 255, FFADE_OUT | FFADE_MODULATE );

	if ( ( GetHealth() < -40 && gibAction != GIB_NEVER ) || gibAction == GIB_ALWAYS )
	{
		SetSolidType( SOLID_NOT );
		GibMonster();	// This clears pev->model
		GetEffects() |= EF_NODRAW;
		return;
	}

	DeathSound();
	
	Vector vecAngles = GetAbsAngles();
	vecAngles.x = 0;
	vecAngles.z = 0;
	SetAbsAngles( vecAngles );

	SetThink(&CBasePlayer::PlayerDeathThink);
	SetNextThink( gpGlobals->time + 0.1 );
}

const char *CBasePlayer::TeamID() const
{
	if( pev == NULL )		// Not fully connected yet
		return "";

	// return their team name
	return m_szTeamName;
}

bool CBasePlayer::Restore( CRestore &restore )
{
	if( !BaseClass::Restore( restore ) )
		return false;

	SAVERESTOREDATA *pSaveData = ( SAVERESTOREDATA * ) gpGlobals->pSaveData;
	// landmark isn't present.
	if( !pSaveData->fUseLandmark )
	{
		ALERT( at_console, "No Landmark:%s\n", pSaveData->szLandmarkName );

		// default to normal spawn
		CBaseEntity* pSpawnSpot = EntSelectSpawnPoint( this );
		SetAbsOrigin( pSpawnSpot->GetAbsOrigin() + Vector( 0, 0, 1 ) );
		SetAbsAngles( pSpawnSpot->GetAbsAngles() );
	}
	Vector vecViewAngle = GetViewAngle();
	vecViewAngle.z = 0;	// Clear out roll
	SetViewAngle( vecViewAngle );
	SetAbsAngles( GetViewAngle() );

	SetFixAngleMode( FIXANGLE_SET );		// turn this way immediately

											// Copied from spawn() for now
	m_bloodColor = BLOOD_COLOR_RED;

	g_ulModelIndexPlayer = GetModelIndex();

	if( GetFlags().Any( FL_DUCKING ) )
	{
		// Use the crouch HACK
		//FixPlayerCrouchStuck( edict() );
		// Don't need to do this with new player prediction code.
		SetSize( VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX );
	}
	else
	{
		SetSize( VEC_HULL_MIN, VEC_HULL_MAX );
	}

	g_engfuncs.pfnSetPhysicsKeyValue( edict(), "hl", "1" );

	if( m_fLongJump )
	{
		g_engfuncs.pfnSetPhysicsKeyValue( edict(), "slj", "1" );
	}
	else
	{
		g_engfuncs.pfnSetPhysicsKeyValue( edict(), "slj", "0" );
	}

	RenewItems();

#if defined( CLIENT_WEAPONS )
	// HACK:	This variable is saved/restored in CBaseMonster as a time variable, but we're using it
	//			as just a counter.  Ideally, this needs its own variable that's saved as a plain float.
	//			Barring that, we clear it out here instead of using the incorrect restored time value.
	m_flNextAttack = UTIL_WeaponTimeBase();
#endif

	//If restored, always connected. - Solokiller
	m_ConnectState = ConnectState::CONNECTED;
	m_flConnectTime = gpGlobals->time;
	m_bNeedsNewConnectTime = true;

	return true;
}

//=========================================================
// BarnacleVictimGrabbed - Overridden for the player to set the proper
// physics flags when a barnacle grabs player.
//=========================================================
bool CBasePlayer::BarnacleVictimGrabbed( CBaseEntity* pBarnacle )
{
	m_afPhysicsFlags |= PFLAG_ONBARNACLE;
	return true;
}

//=========================================================
// BarnacleVictimBitten - bad name for a function that is called
// by Barnacle victims when the barnacle pulls their head
// into its mouth. For the player, just die.
//=========================================================
void CBasePlayer::BarnacleVictimBitten( CBaseEntity* pBarnacle )
{
	TakeDamage( pBarnacle, pBarnacle, GetHealth() + GetArmorAmount(), DMG_SLASH | DMG_ALWAYSGIB );
}

//=========================================================
// BarnacleVictimReleased - overridden for player who has
// physics flags concerns. 
//=========================================================
void CBasePlayer::BarnacleVictimReleased()
{
	m_afPhysicsFlags &= ~PFLAG_ONBARNACLE;
}

//=========================================================
// Illumination 
// return player light level plus virtual muzzle flash
//=========================================================
int CBasePlayer::Illumination() const
{
	int iIllum = CBaseEntity::Illumination();

	iIllum += m_iWeaponFlash;
	if( iIllum > 255 )
		return 255;
	return iIllum;
}

/*
=============
SetCustomDecalFrames

UNDONE:  Determine real frame limit, 8 is a placeholder.
Note:  -1 means no custom frames present.
=============
*/
void CBasePlayer::SetCustomDecalFrames( int nFrames )
{
	if( nFrames > 0 &&
		nFrames < 8 )
		m_nCustomSprayFrames = nFrames;
	else
		m_nCustomSprayFrames = -1;
}

/*
=============
GetCustomDecalFrames

Returns the # of custom frames this player's custom clan logo contains.
=============
*/
int CBasePlayer::GetCustomDecalFrames()
{
	return m_nCustomSprayFrames;
}

//Player ID
void CBasePlayer::InitStatusBar()
{
	m_flStatusBarDisappearDelay = 0;
	m_SbarString1[0] = m_SbarString0[0] = 0; 
}

void CBasePlayer::UpdateStatusBar()
{
	int newSBarState[ SBAR_END ];
	char sbuf0[ SBAR_STRING_SIZE ];
	char sbuf1[ SBAR_STRING_SIZE ];

	memset( newSBarState, 0, sizeof(newSBarState) );
	strcpy( sbuf0, m_SbarString0 );
	strcpy( sbuf1, m_SbarString1 );

	// Find an ID Target
	TraceResult tr;
	UTIL_MakeVectors( GetViewAngle() + GetPunchAngle() );
	Vector vecSrc = EyePosition();
	Vector vecEnd = vecSrc + (gpGlobals->v_forward * MAX_ID_RANGE);
	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, edict(), &tr);

	if (tr.flFraction != 1.0)
	{
		if ( !FNullEnt( tr.pHit ) )
		{
			CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );

			//Use my own classification instead of classify::PLAYER, this accounts for class changes. - Solokiller
			const auto myClassId = Classify();

			//Only do this when my class is not NONE. Otherwise every bit of scenery shows status info.
			//Unfortunately this will cause issues when resetting classes, so adding the PLAYER class back is a requirement to fix this. - Solokiller
			if( myClassId != EntityClassifications().GetNoneId() && pEntity->Classify() == myClassId )
			{
				newSBarState[ SBAR_ID_TARGETNAME ] = pEntity->entindex();
				strcpy( sbuf1, "1 %p1\n2 Health: %i2%%\n3 Armor: %i3%%" );

				// allies and medics get to see the targets health
				if ( g_pGameRules->PlayerRelationship( this, pEntity ) == GR_TEAMMATE )
				{
					newSBarState[ SBAR_ID_TARGETHEALTH ] = 100 * (pEntity->GetHealth() / pEntity->GetMaxHealth() );
					newSBarState[ SBAR_ID_TARGETARMOR ] = pEntity->GetArmorAmount(); //No need to get it % based since 100 it's the max. TODO unless you're a modder - Solokiller
				}

				m_flStatusBarDisappearDelay = gpGlobals->time + 1.0;
			}
		}
		else if ( m_flStatusBarDisappearDelay > gpGlobals->time )
		{
			// hold the values for a short amount of time after viewing the object
			newSBarState[ SBAR_ID_TARGETNAME ] = m_izSBarState[ SBAR_ID_TARGETNAME ];
			newSBarState[ SBAR_ID_TARGETHEALTH ] = m_izSBarState[ SBAR_ID_TARGETHEALTH ];
			newSBarState[ SBAR_ID_TARGETARMOR ] = m_izSBarState[ SBAR_ID_TARGETARMOR ];
		}
	}

	bool bForceResend = false;

	if ( strcmp( sbuf0, m_SbarString0 ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgStatusText, NULL, this );
			WRITE_BYTE( 0 );
			WRITE_STRING( sbuf0 );
		MESSAGE_END();

		strcpy( m_SbarString0, sbuf0 );

		// make sure everything's resent
		bForceResend = true;
	}

	if ( strcmp( sbuf1, m_SbarString1 ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgStatusText, NULL, this );
			WRITE_BYTE( 1 );
			WRITE_STRING( sbuf1 );
		MESSAGE_END();

		strcpy( m_SbarString1, sbuf1 );

		// make sure everything's resent
		bForceResend = true;
	}

	// Check values and send if they don't match
	for (int i = 1; i < SBAR_END; i++)
	{
		if ( newSBarState[i] != m_izSBarState[i] || bForceResend )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgStatusValue, NULL, this );
				WRITE_BYTE( i );
				WRITE_SHORT( newSBarState[i] );
			MESSAGE_END();

			m_izSBarState[i] = newSBarState[i];
		}
	}
}
