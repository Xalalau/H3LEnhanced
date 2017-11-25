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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/NPCs/CTalkMonster.h"

#include "CSpeaker.h"

BEGIN_DATADESC( CSpeaker )
	DEFINE_FIELD( m_preset, FIELD_INTEGER ),

	DEFINE_USEFUNC( ToggleUse ),
	DEFINE_THINKFUNC( SpeakerThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( speaker, CSpeaker );

// KeyValue - load keyvalue pairs into member data
// NOTE: called BEFORE spawn!

void CSpeaker::KeyValue( KeyValueData *pkvd )
{

	// preset
	if( FStrEq( pkvd->szKeyName, "preset" ) )
	{
		m_preset = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CSpeaker::Spawn( void )
{
	const char* szSoundFile = GetMessage();

	if( !m_preset && ( !HasMessage() || strlen( szSoundFile ) < 1 ) )
	{
		ALERT( at_error, "SPEAKER with no Level/Sentence! at: %f, %f, %f\n", GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z );
		SetNextThink( gpGlobals->time + 0.1 );
		SetThink( &CSpeaker::SUB_Remove );
		return;
	}
	SetSolidType( SOLID_NOT );
	SetMoveType( MOVETYPE_NONE );


	SetThink( &CSpeaker::SpeakerThink );
	SetNextThink( 0.0 );

	// allow on/off switching via 'use' function.

	SetUse( &CSpeaker::ToggleUse );

	Precache();
}

void CSpeaker::Precache( void )
{
	if( !GetSpawnFlags().Any( SPEAKER_START_SILENT ) )
		// set first announcement time for random n second
		SetNextThink( gpGlobals->time + RANDOM_FLOAT( 5.0, 15.0 ) );
}

//
// ToggleUse - if an announcement is pending, cancel it.  If no announcement is pending, start one.
//
void CSpeaker::ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	const bool fActive = ( GetNextThink() > 0.0 );

	// fActive is true only if an announcement is pending

	if( useType != USE_TOGGLE )
	{
		// ignore if we're just turning something on that's already on, or
		// turning something off that's already off.
		if( ( fActive && useType == USE_ON ) || ( !fActive && useType == USE_OFF ) )
			return;
	}

	if( useType == USE_ON )
	{
		// turn on announcements
		SetNextThink( gpGlobals->time + 0.1 );
		return;
	}

	if( useType == USE_OFF )
	{
		// turn off announcements
		SetNextThink( 0.0 );
		return;

	}

	// Toggle announcements


	if( fActive )
	{
		// turn off announcements
		SetNextThink( 0.0 );
	}
	else
	{
		// turn on announcements
		SetNextThink( gpGlobals->time + 0.1 );
	}
}

void CSpeaker::SpeakerThink( void )
{
	const char* szSoundFile;
	float flvolume = GetHealth() * 0.1;
	float flattenuation = 0.3;
	int flags = 0;
	int pitch = 100;


	// Wait for the talkmonster to finish first.
	if( gpGlobals->time <= CTalkMonster::g_talkWaitTime )
	{
		SetNextThink( CTalkMonster::g_talkWaitTime + RANDOM_FLOAT( 5, 10 ) );
		return;
	}

	if( m_preset )
	{
		// go lookup preset text, assign szSoundFile
		switch( m_preset )
		{
		default:
		case 1: szSoundFile = "C1A0_"; break;
		case 2: szSoundFile = "C1A1_"; break;
		case 3: szSoundFile = "C1A2_"; break;
		case 4: szSoundFile = "C1A3_"; break;
		case 5: szSoundFile = "C1A4_"; break;
		case 6: szSoundFile = "C2A1_"; break;
		case 7: szSoundFile = "C2A2_"; break;
		case 8: szSoundFile = "C2A3_"; break;
		case 9: szSoundFile = "C2A4_"; break;
		case 10: szSoundFile = "C2A5_"; break;
		case 11: szSoundFile = "C3A1_"; break;
		case 12: szSoundFile = "C3A2_"; break;
		}
	}
	else
		szSoundFile = GetMessage();

	if( szSoundFile[ 0 ] == '!' )
	{
		// play single sentence, one shot
		UTIL_EmitAmbientSound( this, GetAbsOrigin(), szSoundFile,
							   flvolume, flattenuation, flags, pitch );

		// shut off and reset
		SetNextThink( 0.0 );
	}
	else
	{
		// make random announcement from sentence group

		if( g_Sentences.PlayRndSz( this, szSoundFile, flvolume, flattenuation, flags, pitch ) < 0 )
			ALERT( at_console, "Level Design Error!\nSPEAKER has bad sentence group name: %s\n", szSoundFile );

		// set next announcement time for random 5 to 10 minute delay
		SetNextThink( gpGlobals->time +
			RANDOM_FLOAT( ANNOUNCE_MINUTES_MIN * 60.0, ANNOUNCE_MINUTES_MAX * 60.0 ) );

		CTalkMonster::g_talkWaitTime = gpGlobals->time + 5;		// time delay until it's ok to speak: used so that two NPCs don't talk at once
	}

	return;
}