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
#include "eiface.h"
#include "util.h"

#include "UserMessages.h"

#include "CServerGameInterface.h"

#include "Server.h"

cvar_t g_DummyCvar = { "_not_a_real_cvar_", "0" };

cvar_t	displaysoundlist = {"displaysoundlist","0"};

//The engine has its own cvar for this, but it's reset so it's unreliable. - Solokiller
cvar_t	coop		= { "sv_coop", "0", FCVAR_SERVER };

// multiplayer server rules
cvar_t	fragsleft	= {"mp_fragsleft","0", FCVAR_SERVER | FCVAR_UNLOGGED };	  // Don't spam console/log files/users with this changing
cvar_t	timeleft	= {"mp_timeleft","0" , FCVAR_SERVER | FCVAR_UNLOGGED };	  // "      "

// multiplayer server rules
cvar_t	teamplay	= {"mp_teamplay","0", FCVAR_SERVER };
cvar_t	fraglimit	= {"mp_fraglimit","0", FCVAR_SERVER };
cvar_t	timelimit	= {"mp_timelimit","0", FCVAR_SERVER };
cvar_t	friendlyfire= {"mp_friendlyfire","0", FCVAR_SERVER };
cvar_t	falldamage	= {"mp_falldamage","0", FCVAR_SERVER };
cvar_t	weaponstay	= {"mp_weaponstay","0", FCVAR_SERVER };
cvar_t	forcerespawn= {"mp_forcerespawn","1", FCVAR_SERVER };
cvar_t	flashlight	= {"mp_flashlight","0", FCVAR_SERVER };
cvar_t	aimcrosshair= {"mp_autocrosshair","1", FCVAR_SERVER };
cvar_t	decalfrequency = {"decalfrequency","30", FCVAR_SERVER };
cvar_t	teamlist = {"mp_teamlist","hgrunt;scientist", FCVAR_SERVER };
cvar_t	teamoverride = {"mp_teamoverride","1" };
cvar_t	defaultteam = {"mp_defaultteam","0" };
// ############ hu3lifezado ############ //
// Permitir monstros por padrao no multiplayer (0)
cvar_t	allowmonsters = { "mp_allowmonsters","1", FCVAR_SERVER };
// Remover as armas do jogador
cvar_t	hu3_mp_strip = { "hu3_mp_strip","0", FCVAR_SERVER };
// Tripas cagarrentas
cvar_t hu3_gore = { "hu3_gore", "1", FCVAR_SERVER };

// [Modo COOP] :

// Corrigir o spawn de trens
cvar_t	coop_train_spawnpoint = { "coop_train_spawnpoint","0", FCVAR_SERVER };
// Adicionar delay no inicio do movimento de trens
cvar_t	coop_train_delay = { "coop_train_delay","0", FCVAR_SERVER | FCVAR_UNLOGGED };
// Forcar uma velocidade de inicio no trem (resolve ele nao andar no c0a0)
cvar_t	coop_train_startspeed = { "coop_train_startspeed","0", FCVAR_SERVER | FCVAR_UNLOGGED };
// Remover entidades no coop
cvar_t	coop_remove = { "coop_remove","", FCVAR_SERVER };
// Remover entidades no singleplayer
cvar_t	sp_remove = { "sp_remove","", FCVAR_SERVER };
// Desativar a fisica de entidades no coop
cvar_t	coop_nophysics = { "coop_nophysics","", FCVAR_SERVER };
// Teletransporte que deve funcionar em todos os players no coop
cvar_t coop_teleport_plys = { "coop_teleport_plys","", FCVAR_SERVER };
// Muda de mapa instantaneamente quando algum jogador toca no changelevel
cvar_t coop_force_changelevel = { "coop_force_changelevel","", FCVAR_SERVER };
// ############ //

cvar_t  allow_spectators = { "allow_spectators", "0.0", FCVAR_SERVER };		// 0 prevents players from being spectators

cvar_t  mp_chattime = {"mp_chattime","10", FCVAR_SERVER };

//Whether to use the new way to check for impulse commands (unaffected by weapon state) - Solokiller
cvar_t	sv_new_impulse_check = { "sv_new_impulse_check", "0", FCVAR_SERVER };

cvar_t	server_cfg = { "server_cfg", "server/default_server_config.txt", FCVAR_SERVER | FCVAR_UNLOGGED };

// Engine Cvars
cvar_t 	*g_psv_gravity = NULL;
cvar_t	*g_psv_aim = NULL;
cvar_t	*g_footsteps = NULL;

//CVARS FOR SKILL LEVEL SETTINGS

// Agrunt
DECLARE_SKILL_CVARS(agrunt_health);
DECLARE_SKILL_CVARS(agrunt_dmg_punch);

// Apache
DECLARE_SKILL_CVARS(apache_health);

// Barney
DECLARE_SKILL_CVARS(barney_health);

// Bullsquid
DECLARE_SKILL_CVARS(bullsquid_health);
DECLARE_SKILL_CVARS(bullsquid_dmg_bite);
DECLARE_SKILL_CVARS(bullsquid_dmg_whip);
DECLARE_SKILL_CVARS(bullsquid_dmg_spit);

// Big Momma
DECLARE_SKILL_CVARS(bigmomma_health_factor);
DECLARE_SKILL_CVARS(bigmomma_dmg_slash);
DECLARE_SKILL_CVARS(bigmomma_dmg_blast);
DECLARE_SKILL_CVARS(bigmomma_radius_blast);

// Gargantua
DECLARE_SKILL_CVARS(gargantua_health);
DECLARE_SKILL_CVARS(gargantua_dmg_slash);
DECLARE_SKILL_CVARS(gargantua_dmg_fire);
DECLARE_SKILL_CVARS(gargantua_dmg_stomp);

// Hassassin
DECLARE_SKILL_CVARS(hassassin_health);

// Headcrab
DECLARE_SKILL_CVARS(headcrab_health);
DECLARE_SKILL_CVARS(headcrab_dmg_bite);

// Hgrunt 
DECLARE_SKILL_CVARS(hgrunt_health);
DECLARE_SKILL_CVARS(hgrunt_kick);
DECLARE_SKILL_CVARS(hgrunt_pellets);
DECLARE_SKILL_CVARS(hgrunt_gspeed);

// HORNET
DECLARE_SKILL_CVARS(hornet_dmg);

// Houndeye
DECLARE_SKILL_CVARS(houndeye_health);
DECLARE_SKILL_CVARS(houndeye_dmg_blast);

// ISlave
DECLARE_SKILL_CVARS(islave_health);
DECLARE_SKILL_CVARS(islave_dmg_claw);
DECLARE_SKILL_CVARS(islave_dmg_clawrake);
DECLARE_SKILL_CVARS(islave_dmg_zap);

// Icthyosaur
DECLARE_SKILL_CVARS(ichthyosaur_health);
DECLARE_SKILL_CVARS(ichthyosaur_shake);

// Leech
DECLARE_SKILL_CVARS(leech_health);
DECLARE_SKILL_CVARS(leech_dmg_bite);

// Controller
DECLARE_SKILL_CVARS(controller_health);
DECLARE_SKILL_CVARS(controller_dmgzap);
DECLARE_SKILL_CVARS(controller_speedball);
DECLARE_SKILL_CVARS(controller_dmgball);

// Nihilanth
DECLARE_SKILL_CVARS(nihilanth_health);
DECLARE_SKILL_CVARS(nihilanth_zap);

// Scientist
DECLARE_SKILL_CVARS(scientist_health);

// Snark
DECLARE_SKILL_CVARS(snark_health);
DECLARE_SKILL_CVARS(snark_dmg_bite);
DECLARE_SKILL_CVARS(snark_dmg_pop);

// Zombie
DECLARE_SKILL_CVARS(zombie_health);
DECLARE_SKILL_CVARS(zombie_dmg_one_slash);
DECLARE_SKILL_CVARS(zombie_dmg_both_slash);

//Turret
DECLARE_SKILL_CVARS(turret_health);

// MiniTurret
DECLARE_SKILL_CVARS(miniturret_health);

// Sentry Turret
DECLARE_SKILL_CVARS(sentry_health);

// PLAYER WEAPONS

// Crowbar whack
DECLARE_SKILL_CVARS(plr_crowbar);
DECLARE_SKILL_CVARS(plr_crowbar_mp);

// Glock Round
DECLARE_SKILL_CVARS(plr_9mm_bullet);
DECLARE_SKILL_CVARS(plr_9mm_bullet_mp);

// 357 Round
DECLARE_SKILL_CVARS(plr_357_bullet);
DECLARE_SKILL_CVARS(plr_357_bullet_mp);

// MP5 Round
DECLARE_SKILL_CVARS(plr_9mmAR_bullet);
DECLARE_SKILL_CVARS(plr_9mmAR_bullet_mp);

// M203 grenade
DECLARE_SKILL_CVARS(plr_9mmAR_grenade);
DECLARE_SKILL_CVARS(plr_9mmAR_grenade_mp);

// Shotgun buckshot
DECLARE_SKILL_CVARS(plr_buckshot);
DECLARE_SKILL_CVARS(plr_buckshot_mp);

// Crossbow
DECLARE_SKILL_CVARS(plr_xbow_bolt_client);
DECLARE_SKILL_CVARS(plr_xbow_bolt_client_mp);
DECLARE_SKILL_CVARS(plr_xbow_bolt_monster);

// RPG
DECLARE_SKILL_CVARS(plr_rpg);
DECLARE_SKILL_CVARS(plr_rpg_mp);

// Zero Point Generator
DECLARE_SKILL_CVARS(plr_gauss);
DECLARE_SKILL_CVARS(plr_gauss_mp);

// Tau Cannon
DECLARE_SKILL_CVARS(plr_egon_narrow);
DECLARE_SKILL_CVARS(plr_egon_narrow_mp);
DECLARE_SKILL_CVARS(plr_egon_wide);
DECLARE_SKILL_CVARS(plr_egon_wide_mp);

// Hand Grendade
DECLARE_SKILL_CVARS(plr_hand_grenade);
DECLARE_SKILL_CVARS(plr_hand_grenade_mp);

// Satchel Charge
DECLARE_SKILL_CVARS(plr_satchel);
DECLARE_SKILL_CVARS(plr_satchel_mp);

// Tripmine
DECLARE_SKILL_CVARS(plr_tripmine);
DECLARE_SKILL_CVARS(plr_tripmine_mp);

#if USE_OPFOR
// Knife whack
DECLARE_SKILL_CVARS(plr_knife);
DECLARE_SKILL_CVARS(plr_knife_mp);

// Pipewrench whack
DECLARE_SKILL_CVARS(plr_pipewrench);
DECLARE_SKILL_CVARS(plr_pipewrench_mp);

// Grapple
DECLARE_SKILL_CVARS(plr_grapple);
DECLARE_SKILL_CVARS(plr_grapple_mp);

// 556 Round
DECLARE_SKILL_CVARS(plr_556_bullet);
DECLARE_SKILL_CVARS(plr_556_bullet_mp);

// 762 Round
DECLARE_SKILL_CVARS(plr_762_bullet);
DECLARE_SKILL_CVARS(plr_762_bullet_mp);

// 762 Round
DECLARE_SKILL_CVARS(plr_eagle);
DECLARE_SKILL_CVARS(plr_eagle_mp);

// Shock Roach
DECLARE_SKILL_CVARS(plr_shockroachs);
DECLARE_SKILL_CVARS(plr_shockroachs_mp);
DECLARE_SKILL_CVARS(plr_shockroachm);
DECLARE_SKILL_CVARS(plr_shockroachm_mp);

// Displacer Cannon
DECLARE_SKILL_CVARS(plr_displacer_self);
DECLARE_SKILL_CVARS(plr_displacer_self_mp);
DECLARE_SKILL_CVARS(plr_displacer_other);
DECLARE_SKILL_CVARS(plr_displacer_other_mp);
DECLARE_SKILL_CVARS(plr_displacer_radius);
DECLARE_SKILL_CVARS(plr_displacer_radius_mp);

// Spore Launcher 
DECLARE_SKILL_CVARS(plr_spore);
DECLARE_SKILL_CVARS(plr_spore_mp);
#endif

// WORLD WEAPONS
DECLARE_SKILL_CVARS(12mm_bullet);
DECLARE_SKILL_CVARS(9mmAR_bullet);
DECLARE_SKILL_CVARS(9mm_bullet);

// HORNET
DECLARE_SKILL_CVARS(plr_hornet_dmg);
DECLARE_SKILL_CVARS(plr_hornet_dmg_mp);

// HEALTH/CHARGE
DECLARE_SKILL_CVARS(suitcharger);
DECLARE_SKILL_CVARS(suitcharger_mp);
DECLARE_SKILL_CVARS(battery);
DECLARE_SKILL_CVARS(healthcharger);
DECLARE_SKILL_CVARS(healthkit);
DECLARE_SKILL_CVARS(scientist_heal);

// monster damage adjusters
DECLARE_SKILL_CVARS(monster_head);
DECLARE_SKILL_CVARS(monster_chest);
DECLARE_SKILL_CVARS(monster_stomach);
DECLARE_SKILL_CVARS(monster_arm);
DECLARE_SKILL_CVARS(monster_leg);

// player damage adjusters
DECLARE_SKILL_CVARS(player_head);
DECLARE_SKILL_CVARS(player_chest);
DECLARE_SKILL_CVARS(player_stomach);
DECLARE_SKILL_CVARS(player_arm);
DECLARE_SKILL_CVARS(player_leg);

// END Cvars for Skill Level settings

void ShutdownGame()
{
	//No other way to signal failure. - Solokiller
	SERVER_COMMAND( "quit\n" );
}

// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit( void )
{
	// Register cvars here:

	g_psv_gravity = CVAR_GET_POINTER( "sv_gravity" );
	g_psv_aim = CVAR_GET_POINTER( "sv_aim" );
	g_footsteps = CVAR_GET_POINTER( "mp_footsteps" );

	//Needs to be registered so the engine handles the string value correctly. - Solokiller
	CVAR_REGISTER( &g_DummyCvar );

	CVAR_REGISTER (&displaysoundlist);
	CVAR_REGISTER( &coop );
	CVAR_REGISTER( &allow_spectators );

	CVAR_REGISTER (&teamplay);
	CVAR_REGISTER (&fraglimit);
	CVAR_REGISTER (&timelimit);

	CVAR_REGISTER (&fragsleft);
	CVAR_REGISTER (&timeleft);

	CVAR_REGISTER (&friendlyfire);
	CVAR_REGISTER (&falldamage);
	CVAR_REGISTER (&weaponstay);
	CVAR_REGISTER (&forcerespawn);
	CVAR_REGISTER (&flashlight);
	CVAR_REGISTER (&aimcrosshair);
	CVAR_REGISTER (&decalfrequency);
	CVAR_REGISTER (&teamlist);
	CVAR_REGISTER (&teamoverride);
	CVAR_REGISTER (&defaultteam);
	CVAR_REGISTER (&allowmonsters);

	CVAR_REGISTER (&mp_chattime);

	CVAR_REGISTER( &sv_new_impulse_check );
	CVAR_REGISTER( &server_cfg );

	// ############ hu3lifezado ############ //
	// Remover as armas do jogador
	CVAR_REGISTER(&hu3_mp_strip);
	// Tripas cagarrentas
	CVAR_REGISTER(&hu3_gore);

	// [Modo COOP] :

	// Corrigir o spawn de trens
	CVAR_REGISTER(&coop_train_spawnpoint);
	// Adicionar delay no inicio do movimento de trens
	CVAR_REGISTER(&coop_train_delay);
	// Forcar uma velocidade de inicio no trem (resolve ele nao andar no c0a0)
	CVAR_REGISTER(&coop_train_startspeed);
	// Remover entidades no coop
	CVAR_REGISTER(&coop_remove);
	// Remover entidades no singleplayer
	CVAR_REGISTER(&sp_remove);
	// Desativar a fisica de entidades no coop
	CVAR_REGISTER(&coop_nophysics);
	// Teletransporte que deve funcionar em todos os players no coop
	CVAR_REGISTER(&coop_teleport_plys);
	// Muda de mapa instantaneamente quando algum jogador toca no changelevel
	CVAR_REGISTER(&coop_force_changelevel);
	// ############ //

	// REGISTER CVARS FOR SKILL LEVEL STUFF

	// Agrunt
	REGISTER_SKILL_CVARS(agrunt_health);
	REGISTER_SKILL_CVARS(agrunt_dmg_punch);

	// Apache
	REGISTER_SKILL_CVARS(apache_health);

	// Barney
	REGISTER_SKILL_CVARS(barney_health);

	// Bullsquid
	REGISTER_SKILL_CVARS(bullsquid_health);
	REGISTER_SKILL_CVARS(bullsquid_dmg_bite);
	REGISTER_SKILL_CVARS(bullsquid_dmg_whip);
	REGISTER_SKILL_CVARS(bullsquid_dmg_spit);

	// Big Momma
	REGISTER_SKILL_CVARS(bigmomma_health_factor);
	REGISTER_SKILL_CVARS(bigmomma_dmg_slash);
	REGISTER_SKILL_CVARS(bigmomma_dmg_blast);
	REGISTER_SKILL_CVARS(bigmomma_radius_blast);

	// Gargantua
	REGISTER_SKILL_CVARS(gargantua_health);
	REGISTER_SKILL_CVARS(gargantua_dmg_slash);
	REGISTER_SKILL_CVARS(gargantua_dmg_fire);
	REGISTER_SKILL_CVARS(gargantua_dmg_stomp);

	// Hassassin
	REGISTER_SKILL_CVARS(hassassin_health);

	// Headcrab
	REGISTER_SKILL_CVARS(headcrab_health);
	REGISTER_SKILL_CVARS(headcrab_dmg_bite);

	// Hgrunt
	REGISTER_SKILL_CVARS(hgrunt_health);
	REGISTER_SKILL_CVARS(hgrunt_kick);
	REGISTER_SKILL_CVARS(hgrunt_pellets);
	REGISTER_SKILL_CVARS(hgrunt_gspeed);

	// HORNET
	REGISTER_SKILL_CVARS(hgrunt_gspeed);
	CVAR_REGISTER( &sk_hornet_dmg1 );// {"sk_hornet_dmg1","0"};
	CVAR_REGISTER( &sk_hornet_dmg2 );// {"sk_hornet_dmg2","0"};
	CVAR_REGISTER( &sk_hornet_dmg3 );// {"sk_hornet_dmg3","0"};

	// Houndeye
	REGISTER_SKILL_CVARS(houndeye_health);
	REGISTER_SKILL_CVARS(houndeye_dmg_blast);

	// ISlave
	REGISTER_SKILL_CVARS(islave_health);
	REGISTER_SKILL_CVARS(islave_dmg_claw);
	REGISTER_SKILL_CVARS(islave_dmg_clawrake);
	REGISTER_SKILL_CVARS(islave_dmg_zap);

	// Icthyosaur
	REGISTER_SKILL_CVARS(ichthyosaur_health);
	REGISTER_SKILL_CVARS(ichthyosaur_shake);

	// Leech
	REGISTER_SKILL_CVARS(leech_health);
	REGISTER_SKILL_CVARS(leech_dmg_bite);

	// Controller
	REGISTER_SKILL_CVARS(controller_health);
	REGISTER_SKILL_CVARS(controller_dmgzap);
	REGISTER_SKILL_CVARS(controller_speedball);
	REGISTER_SKILL_CVARS(controller_dmgball);

	// Nihilanth
	REGISTER_SKILL_CVARS(nihilanth_health);
	REGISTER_SKILL_CVARS(nihilanth_zap);

	// Scientist
	REGISTER_SKILL_CVARS(scientist_health);

	// Snark
	REGISTER_SKILL_CVARS(snark_health);
	REGISTER_SKILL_CVARS(snark_dmg_bite);
	REGISTER_SKILL_CVARS(snark_dmg_pop);

	// Zombie
	REGISTER_SKILL_CVARS(zombie_health);
	REGISTER_SKILL_CVARS(zombie_dmg_one_slash);
	REGISTER_SKILL_CVARS(zombie_dmg_both_slash);

	//Turret
	REGISTER_SKILL_CVARS(turret_health);

	// MiniTurret
	REGISTER_SKILL_CVARS(miniturret_health);

	// Sentry Turret
	REGISTER_SKILL_CVARS(sentry_health);

	// PLAYER WEAPONS

	// Crowbar whack
	REGISTER_SKILL_CVARS(plr_crowbar);
	REGISTER_SKILL_CVARS(plr_crowbar_mp);

	// Glock Round
	REGISTER_SKILL_CVARS(plr_9mm_bullet);
	REGISTER_SKILL_CVARS(plr_9mm_bullet_mp);

	// 357 Round
	REGISTER_SKILL_CVARS(plr_357_bullet);
	REGISTER_SKILL_CVARS(plr_357_bullet_mp);

	// MP5 Round
	REGISTER_SKILL_CVARS(plr_9mmAR_bullet);
	REGISTER_SKILL_CVARS(plr_9mmAR_bullet_mp);

	// M203 grenade
	REGISTER_SKILL_CVARS(plr_9mmAR_grenade);
	REGISTER_SKILL_CVARS(plr_9mmAR_grenade_mp);

	// Shotgun buckshot
	REGISTER_SKILL_CVARS(plr_buckshot);
	REGISTER_SKILL_CVARS(plr_buckshot_mp);

	// Crossbow
	REGISTER_SKILL_CVARS(plr_xbow_bolt_monster);
	REGISTER_SKILL_CVARS(plr_xbow_bolt_client);
	REGISTER_SKILL_CVARS(plr_xbow_bolt_client_mp);

	// RPG
	REGISTER_SKILL_CVARS(plr_rpg);
	REGISTER_SKILL_CVARS(plr_rpg_mp);

	// Gauss Gun
	REGISTER_SKILL_CVARS(plr_gauss);
	REGISTER_SKILL_CVARS(plr_gauss_mp);

	// Egon Gun
	REGISTER_SKILL_CVARS(plr_egon_narrow);
	REGISTER_SKILL_CVARS(plr_egon_narrow_mp);
	REGISTER_SKILL_CVARS(plr_egon_wide);
	REGISTER_SKILL_CVARS(plr_egon_wide_mp);

	// Hand Grendade
	REGISTER_SKILL_CVARS(plr_hand_grenade);
	REGISTER_SKILL_CVARS(plr_hand_grenade_mp);

	// Satchel Charge
	REGISTER_SKILL_CVARS(plr_satchel);
	REGISTER_SKILL_CVARS(plr_satchel_mp);

	// Tripmine
	REGISTER_SKILL_CVARS(plr_tripmine);
	REGISTER_SKILL_CVARS(plr_tripmine_mp);

#if USE_OPFOR
	// Knife whack
	REGISTER_SKILL_CVARS(plr_knife);
	REGISTER_SKILL_CVARS(plr_knife_mp);

	// Pipe wrench whack
	REGISTER_SKILL_CVARS(plr_pipewrench);
	REGISTER_SKILL_CVARS(plr_pipewrench_mp);

	// Grapple
	REGISTER_SKILL_CVARS(plr_grapple);
	REGISTER_SKILL_CVARS(plr_grapple_mp);

	// 556 Round
	REGISTER_SKILL_CVARS(plr_556_bullet);
	REGISTER_SKILL_CVARS(plr_556_bullet_mp);

	// 762 Round
	REGISTER_SKILL_CVARS(plr_762_bullet);
	REGISTER_SKILL_CVARS(plr_762_bullet_mp);

	// Desert Eagle Round
	REGISTER_SKILL_CVARS(plr_eagle);
	REGISTER_SKILL_CVARS(plr_eagle_mp);

	// Shock Roach
	REGISTER_SKILL_CVARS(plr_shockroachs);
	REGISTER_SKILL_CVARS(plr_shockroachs_mp);
	REGISTER_SKILL_CVARS(plr_shockroachm);
	REGISTER_SKILL_CVARS(plr_shockroachm_mp);

	// Displacer Cannon
	REGISTER_SKILL_CVARS(plr_displacer_self);
	REGISTER_SKILL_CVARS(plr_displacer_self_mp);
	REGISTER_SKILL_CVARS(plr_displacer_other);
	REGISTER_SKILL_CVARS(plr_displacer_other_mp);
	REGISTER_SKILL_CVARS(plr_displacer_radius);
	REGISTER_SKILL_CVARS(plr_displacer_radius_mp);

	// Spore
	REGISTER_SKILL_CVARS(plr_spore);
	REGISTER_SKILL_CVARS(plr_spore_mp);
#endif

	// WORLD WEAPONS
	REGISTER_SKILL_CVARS(12mm_bullet);
	REGISTER_SKILL_CVARS(9mmAR_bullet);
	REGISTER_SKILL_CVARS(9mm_bullet);

	// HORNET
	REGISTER_SKILL_CVARS(plr_hornet_dmg);
	REGISTER_SKILL_CVARS(plr_hornet_dmg_mp);

	// HEALTH/SUIT CHARGE DISTRIBUTION
	REGISTER_SKILL_CVARS(suitcharger);
	REGISTER_SKILL_CVARS(suitcharger_mp);
	REGISTER_SKILL_CVARS(battery);
	REGISTER_SKILL_CVARS(healthcharger);
	REGISTER_SKILL_CVARS(healthkit);
	REGISTER_SKILL_CVARS(scientist_heal);

// monster damage adjusters
	REGISTER_SKILL_CVARS(monster_head);
	REGISTER_SKILL_CVARS(monster_chest);
	REGISTER_SKILL_CVARS(monster_stomach);
	REGISTER_SKILL_CVARS(monster_arm);
	REGISTER_SKILL_CVARS(monster_leg);

// player damage adjusters
	REGISTER_SKILL_CVARS(player_head);
	REGISTER_SKILL_CVARS(player_chest);
	REGISTER_SKILL_CVARS(player_stomach);
	REGISTER_SKILL_CVARS(player_arm);
	REGISTER_SKILL_CVARS(player_leg);

// END REGISTER CVARS FOR SKILL LEVEL STUFF

	//Link user messages now.
	LinkUserMessages();

	SERVER_COMMAND( "exec skill.cfg\n" );

#if USE_OPFOR
	SERVER_COMMAND("exec skill_opfor.cfg\n");
#endif

	if( !g_Server.Initialize() )
	{
		ShutdownGame();
	}
}

void GameDLLShutdown()
{
	g_Server.Shutdown();
}