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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CTalkMonster.h"

#include "CSittingScientist.h"

BEGIN_DATADESC( CSittingScientist )
	// Don't need to save/restore m_baseSequence (recalced)
	DEFINE_FIELD( m_headTurn, FIELD_INTEGER ),
	DEFINE_FIELD( m_flResponseDelay, FIELD_FLOAT ),
	DEFINE_THINKFUNC( SittingThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_sitting_scientist, CSittingScientist );

//
// ********** Scientist SPAWN **********
//
void CSittingScientist::Spawn()
{
	PRECACHE_MODEL( "models/scientist.mdl" );
	SetModel( "models/scientist.mdl" );
	Precache();
	InitBoneControllers();

	SetSize( Vector( -14, -14, 0 ), Vector( 14, 14, 36 ) );

	SetSolidType( SOLID_SLIDEBOX );
	SetMoveType( MOVETYPE_STEP );
	GetEffects().ClearAll();
	SetHealth( 50 );

	m_bloodColor = BLOOD_COLOR_RED;
	m_flFieldOfView = VIEW_FIELD_WIDE; // indicates the width of this monster's forward view cone ( as a dotproduct result )

	m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD;

	GetSpawnFlags().AddFlags( SF_MONSTER_PREDISASTER ); // predisaster only!

	if( GetBody() == -1 )
	{// -1 chooses a random head
		SetBody( RANDOM_LONG( 0, NUM_SCIENTIST_HEADS - 1 ) );// pick a head, any head
	}
	// Luther is black, make his hands black
	if( GetBody() == HEAD_LUTHER )
		SetSkin( 1 );

	m_baseSequence = LookupSequence( "sitlookleft" );
	SetSequence( m_baseSequence + RANDOM_LONG( 0, 4 ) );
	ResetSequenceInfo();

	SetThink( &CSittingScientist::SittingThink );
	SetNextThink( gpGlobals->time + 0.1 );

	UTIL_DropToFloor( this );
}

void CSittingScientist::Precache( void )
{
	m_baseSequence = LookupSequence( "sitlookleft" );
	TalkInit();
}

//=========================================================
// sit, do stuff
//=========================================================
void CSittingScientist::SittingThink( void )
{
	CBaseEntity *pent;

	StudioFrameAdvance();

	// try to greet player
	if( FIdleHello() )
	{
		pent = FindNearestFriend( true );
		if( pent )
		{
			float yaw = VecToYaw( pent->GetAbsOrigin() - GetAbsOrigin() ) - GetAbsAngles().y;

			if( yaw > 180 ) yaw -= 360;
			if( yaw < -180 ) yaw += 360;

			if( yaw > 0 )
				SetSequence( m_baseSequence + SITTING_ANIM_sitlookleft );
			else
				SetSequence( m_baseSequence + SITTING_ANIM_sitlookright );

			ResetSequenceInfo();
			SetFrame( 0 );
			SetBoneController( 0, 0 );
		}
	}
	else if( m_fSequenceFinished )
	{
		int i = RANDOM_LONG( 0, 99 );
		m_headTurn = 0;

		if( m_flResponseDelay && gpGlobals->time > m_flResponseDelay )
		{
			// respond to question
			IdleRespond();
			SetSequence( m_baseSequence + SITTING_ANIM_sitscared );
			m_flResponseDelay = 0;
		}
		else if( i < 30 )
		{
			SetSequence( m_baseSequence + SITTING_ANIM_sitting3 );

			// turn towards player or nearest friend and speak

			if( !FBitSet( m_bitsSaid, bit_saidHelloPlayer ) )
				pent = FindNearestFriend( true );
			else
				pent = FindNearestFriend( false );

			if( !FIdleSpeak() || !pent )
			{
				m_headTurn = RANDOM_LONG( 0, 8 ) * 10 - 40;
				SetSequence( m_baseSequence + SITTING_ANIM_sitting3 );
			}
			else
			{
				// only turn head if we spoke
				float yaw = VecToYaw( pent->GetAbsOrigin() - GetAbsOrigin() ) - GetAbsAngles().y;

				if( yaw > 180 ) yaw -= 360;
				if( yaw < -180 ) yaw += 360;

				if( yaw > 0 )
					SetSequence( m_baseSequence + SITTING_ANIM_sitlookleft );
				else
					SetSequence( m_baseSequence + SITTING_ANIM_sitlookright );

				//ALERT(at_console, "sitting speak\n");
			}
		}
		else if( i < 60 )
		{
			SetSequence( m_baseSequence + SITTING_ANIM_sitting3 );
			m_headTurn = RANDOM_LONG( 0, 8 ) * 10 - 40;
			if( RANDOM_LONG( 0, 99 ) < 5 )
			{
				//ALERT(at_console, "sitting speak2\n");
				FIdleSpeak();
			}
		}
		else if( i < 80 )
		{
			SetSequence( m_baseSequence + SITTING_ANIM_sitting2 );
		}
		else if( i < 100 )
		{
			SetSequence( m_baseSequence + SITTING_ANIM_sitscared );
		}

		ResetSequenceInfo();
		SetFrame( 0 );
		SetBoneController( 0, m_headTurn );
	}
	SetNextThink( gpGlobals->time + 0.1 );
}

//=========================================================
// ID as a passive human
//=========================================================
EntityClassification_t CSittingScientist::GetClassification()
{
	return EntityClassifications().GetClassificationId( classify::HUMAN_PASSIVE );
}

// prepare sitting scientist to answer a question
void CSittingScientist::SetAnswerQuestion( CTalkMonster *pSpeaker )
{
	m_flResponseDelay = gpGlobals->time + RANDOM_FLOAT( 3, 4 );
	m_hTalkTarget = ( CBaseMonster * ) pSpeaker;
}

int CSittingScientist::FriendNumber( int arrayNumber ) const
{
	static int array[ 3 ] = { 2, 1, 0 };
	if( arrayNumber < 3 )
		return array[ arrayNumber ];
	return arrayNumber;
}

//=========================================================
// FIdleSpeak
// ask question of nearby friend, or make statement
//=========================================================
bool CSittingScientist::FIdleSpeak()
{
	// try to start a conversation, or make statement
	int pitch;

	if( !FOkToSpeak() )
		return false;

	// set global min delay for next conversation
	CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 4.8, 5.2 );

	pitch = GetVoicePitch();

	// if there is a friend nearby to speak to, play sentence, set friend's response time, return

	// try to talk to any standing or sitting scientists nearby
	CBaseEntity *pentFriend = FindNearestFriend( false );

	if( pentFriend && RANDOM_LONG( 0, 1 ) )
	{
		auto pTalkMonster = static_cast<CTalkMonster*>( pentFriend );
		pTalkMonster->SetAnswerQuestion( this );

		IdleHeadTurn( pentFriend->GetAbsOrigin() );
		g_Sentences.PlayRndSz( this, m_szGrp[ TLK_PQUESTION ], 1.0, ATTN_IDLE, 0, pitch );
		// set global min delay for next conversation
		CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 4.8, 5.2 );
		return true;
	}

	// otherwise, play an idle statement
	if( RANDOM_LONG( 0, 1 ) )
	{
		g_Sentences.PlayRndSz( this, m_szGrp[ TLK_PIDLE ], 1.0, ATTN_IDLE, 0, pitch );
		// set global min delay for next conversation
		CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 4.8, 5.2 );
		return true;
	}

	// never spoke
	CTalkMonster::g_talkWaitTime = 0;
	return false;
}