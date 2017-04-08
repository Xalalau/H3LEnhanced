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
//=========================================================
// sound.cpp 
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"
#include "CBasePlayer.h"
#include "entities/NPCs/CTalkMonster.h"
#include "gamerules/GameRules.h"
#include "com_model.h"
#include "materials/Materials.h"

#include "Sound.h"

CSentenceGroups g_Sentences;

void EMIT_SOUND_DYN( CBaseEntity* pEntity, int channel, const char *sample, float volume, float attenuation,
						   int flags, int pitch)
{
	if (sample && *sample == '!')
	{
		char name[32];
		if ( g_Sentences.Lookup(sample, name) >= 0)
				EMIT_SOUND_DYN2( pEntity->edict(), channel, name, volume, attenuation, flags, pitch);
		else
			ALERT( at_aiconsole, "Unable to find %s in sentences.txt\n", sample );
	}
	else
		EMIT_SOUND_DYN2( pEntity->edict(), channel, sample, volume, attenuation, flags, pitch);
}

void EMIT_SOUND( CBaseEntity* pEntity, int channel, const char *sample, float volume, float attenuation )
{
	EMIT_SOUND_DYN( pEntity, channel, sample, volume, attenuation, 0, PITCH_NORM );
}

void STOP_SOUND( CBaseEntity* pEntity, int channel, const char* const pszSample )
{
	EMIT_SOUND_DYN( pEntity, channel, pszSample, 0, 0, SND_STOP, PITCH_NORM );
}

// play a specific sentence over the HEV suit speaker - just pass player entity, and !sentencename

void EMIT_SOUND_SUIT( CBaseEntity* pEntity, const char *sample )
{
	float fvol;
	int pitch = PITCH_NORM;

	fvol = CVAR_GET_FLOAT("suitvolume");
	if (RANDOM_LONG(0,1))
		pitch = RANDOM_LONG(0,6) + 98;

	if (fvol > 0.05)
		EMIT_SOUND_DYN( pEntity, CHAN_STATIC, sample, fvol, ATTN_NORM, 0, pitch );
}

// play a sentence, randomly selected from the passed in group id, over the HEV suit speaker

void EMIT_GROUPID_SUIT( CBaseEntity* pEntity, int isentenceg )
{
	float fvol;
	int pitch = PITCH_NORM;

	fvol = CVAR_GET_FLOAT("suitvolume");
	if (RANDOM_LONG(0,1))
		pitch = RANDOM_LONG(0,6) + 98;

	if (fvol > 0.05)
		g_Sentences.PlayRndI( pEntity, isentenceg, fvol, ATTN_NORM, 0, pitch );
}

// play a sentence, randomly selected from the passed in groupname

void EMIT_GROUPNAME_SUIT( CBaseEntity* pEntity, const char *groupname )
{
	float fvol;
	int pitch = PITCH_NORM;

	fvol = CVAR_GET_FLOAT("suitvolume");
	if (RANDOM_LONG(0,1))
		pitch = RANDOM_LONG(0,6) + 98;

	if (fvol > 0.05)
		g_Sentences.PlayRndSz( pEntity, groupname, fvol, ATTN_NORM, 0, pitch);
}

void UTIL_EmitAmbientSound( CBaseEntity* pEntity, const Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch )
{
	if( samp && *samp == '!' )
	{
		char name[ 32 ];
		if( g_Sentences.Lookup( samp, name ) >= 0 )
			EMIT_AMBIENT_SOUND( pEntity->edict(), vecOrigin, name, vol, attenuation, fFlags, pitch );
	}
	else
		EMIT_AMBIENT_SOUND( pEntity->edict(), vecOrigin, samp, vol, attenuation, fFlags, pitch );
}

// ===================== MATERIAL TYPE DETECTION, MAIN ROUTINES ========================
// 
// Used to detect the texture the player is standing on, map the
// texture name to a material type.  Play footstep sound based
// on material type.

// play a strike sound based on the texture that was hit by the attack traceline.  VecSrc/VecEnd are the
// original traceline endpoints used by the attacker, iBulletType is the type of bullet that hit the texture.
// returns volume of strike instrument (crowbar) to play
float TEXTURETYPE_PlaySound(const TraceResult& tr, Vector vecSrc, Vector vecEnd, int iBulletType)
{
	// hit the world, try to play sound based on texture material type
	
	char szbuffer[64];
	const texture_t* pTexture;

	if ( !g_pGameRules->PlayTextureSounds() )
		return 0.0;

	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

	char chTextureType = CHAR_TEX_CONCRETE;

	if (pEntity && pEntity->Classify() != EntityClassifications().GetNoneId() && pEntity->Classify() != EntityClassifications().GetClassificationId( classify::MACHINE ))
		// hit body
		chTextureType = CHAR_TEX_FLESH;
	else
	{
		// hit world

		// find texture under strike, get material type

		// get texture from entity or world
		pTexture = UTIL_TraceTexture( pEntity ? pEntity : CWorld::GetInstance(), vecSrc, vecEnd );
			
		if ( pTexture )
		{
			const char* pszTextureName = pTexture->name;

			// strip leading '-0' or '+0~' or '{' or '!'
			if (*pszTextureName == '-' || *pszTextureName == '+')
				pszTextureName += 2;

			if (*pszTextureName == '{' || *pszTextureName == '!' || *pszTextureName == '~' || *pszTextureName == ' ')
				++pszTextureName;
			// '}}'
			strcpy( szbuffer, pszTextureName );
			szbuffer[CBTEXTURENAMEMAX - 1] = 0;
				
			// ALERT ( at_console, "texture hit: %s\n", szbuffer);

			// get texture type
			chTextureType = g_MaterialsList.FindTextureType(szbuffer);	
		}
	}

	float fvol;
	float fvolbar;
	float fattn;
	const char* rgsz[ 4 ];
	size_t cnt;

	if( !mat::GetMaterialForType( chTextureType, iBulletType, fvol, fvolbar, fattn, rgsz, cnt ) )
		return 0.0; // crowbar already makes this sound

	ASSERT( cnt < ARRAYSIZE( rgsz ) );

	// did we hit a breakable?

	if( pEntity && pEntity->ClassnameIs( "func_breakable" ) )
	{
		// drop volumes, the object will already play a damaged sound
		fvol /= 1.5;
		fvolbar /= 2.0;	
	}
	else if (chTextureType == CHAR_TEX_COMPUTER)
	{
		// play random spark if computer

		if ( tr.flFraction != 1.0 && RANDOM_LONG(0,1))
		{
			UTIL_Sparks( tr.vecEndPos );

			float flVolume = RANDOM_FLOAT ( 0.7 , 1.0 );//random volume range
			switch ( RANDOM_LONG(0,1) )
			{
				case 0: UTIL_EmitAmbientSound( CWorld::GetInstance(), tr.vecEndPos, "buttons/spark5.wav", flVolume, ATTN_NORM, 0, 100 ); break;
				case 1: UTIL_EmitAmbientSound( CWorld::GetInstance(), tr.vecEndPos, "buttons/spark6.wav", flVolume, ATTN_NORM, 0, 100 ); break;
				// case 0: EMIT_SOUND( this, CHAN_VOICE, "buttons/spark5.wav", flVolume, ATTN_NORM );	break;
				// case 1: EMIT_SOUND( this, CHAN_VOICE, "buttons/spark6.wav", flVolume, ATTN_NORM );	break;
			}
		}
	}

	// play material hit sound
	UTIL_EmitAmbientSound( CWorld::GetInstance(), tr.vecEndPos, rgsz[RANDOM_LONG(0,cnt-1)], fvol, fattn, 0, 96 + RANDOM_LONG(0,0xf));
	//EMIT_SOUND_DYN( m_pPlayer, CHAN_WEAPON, rgsz[RANDOM_LONG(0,cnt-1)], fvol, ATTN_NORM, 0, 96 + RANDOM_LONG(0,0xf));
			
	return fvolbar;
}