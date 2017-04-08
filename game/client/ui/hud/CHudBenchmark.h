/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
#ifndef GAME_CLIENT_UI_HUD_CHUDBENCHMARK_H
#define GAME_CLIENT_UI_HUD_CHUDBENCHMARK_H

#include "shared/hud/CHudElement.h"
#include "hud.h"

class CHudBenchmark : public CBaseHudElement<CHLHud>
{
public:
	DECLARE_CLASS( CHudBenchmark, CBaseHudElement<CHLHud> );

	CHudBenchmark( const char* const pszName, CHLHud& hud );

	void Init() override;
	void VidInit() override;
	bool Draw( float flTime ) override;

	void SetScore( float score );

	void Think() override;

	void StartNextSection( int section );

	void MsgFunc_Bench( const char *pszName, int iSize, void *pbuf );

	void CountFrame( float dt );

	int GetObjects() { return m_nObjects; };

	void SetCompositeScore();

	void Restart();

	void UserCmd_BenchMark()
	{
		Restart();
	}

	int Bench_ScoreForValue( int stage, float raw );

private:
	float	m_fDrawTime;
	float	m_fDrawScore;
	float	m_fAvgScore;

	float   m_fSendTime;
	float	m_fReceiveTime;

	int		m_nFPSCount;
	float	m_fAverageFT;
	float	m_fAvgFrameRate;

	int		m_nSentFinish;
	float	m_fStageStarted;

	float	m_StoredLatency;
	float	m_StoredPacketLoss;
	int		m_nStoredHopCount;
	int		m_nTraceDone;

	int		m_nObjects;

	int		m_nScoreComputed;
	int 	m_nCompositeScore;
};

#endif //GAME_CLIENT_UI_HUD_CHUDBENCHMARK_H