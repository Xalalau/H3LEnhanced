#include "extdll.h"
#include "util.h"
#include "Server.h"
#include "cbase.h"
#include "CBasePlayer.h"

#include "GameRules.h"
#include "CHalfLifeCoop.h"
#include "CHalfLifeMultiplay.h"
#include "CHalfLifeRules.h"
#include "CHalfLifeTeamplay.h"

DLL_GLOBAL CGameRules* g_pGameRules = nullptr;

int g_teamplay = 0;

namespace GameRules
{
Coop::Coop DetermineCoopMode()
{
	const int iCoop = static_cast<int>( coop.value );

	if( iCoop == 0 )
		return Coop::NO;

	if( iCoop == 1 )
		return Coop::SINGLEPLAYRULES;

	return Coop::MULTIPLAYRULES;
}
}

static CGameRules* CreateGameRules()
{
	g_teamplay = 0;

	//Determine whether co-op mode is enabled, and which rules should be used.
	const Coop::Coop coopMode = GameRules::DetermineCoopMode();

	if( coopMode != Coop::NO )
	{
		//We're in co-op mode, so tweak the global vars to match a co-op environment.
		//Needed since some code relies on them. - Solokiller
		gpGlobals->coop = true;
		gpGlobals->deathmatch = false;
		gpGlobals->teamplay = false;

		CVAR_SET_FLOAT( "coop", 1 );
		CVAR_SET_FLOAT( "deathmatch", 0 );

		switch( coopMode )
		{
			//Case to catch any missing cases if more modes are added. - Solokiller
		case Coop::NO:					break;
		case Coop::SINGLEPLAYRULES:		return new CHalfLifeSingleCoop;
		case Coop::MULTIPLAYRULES:		return new CHalfLifeMultiCoop;
		}
	}

	//Make sure this is reset if cvars are changed. - Solokiller
	gpGlobals->coop = false;

	CVAR_SET_FLOAT( "coop", 0 );

	if( !gpGlobals->deathmatch )
	{
		// generic half-life
		return new CHalfLifeRules;
	}
	else
	{
		if( teamplay.value > 0 )
		{
			// teamplay

			g_teamplay = 1;
			return new CHalfLifeTeamplay;
		}
		if( ( int ) gpGlobals->deathmatch == 1 )
		{
			// vanilla deathmatch
			return new CHalfLifeMultiplay;
		}
		else
		{
			// vanilla deathmatch??
			return new CHalfLifeMultiplay;
		}
	}
}

CGameRules* InstallGameRules()
{
	SERVER_COMMAND( "exec game.cfg\n" );
	SERVER_EXECUTE();

	g_pGameRules = CreateGameRules();

	g_pGameRules->OnCreate();

	return g_pGameRules;
}