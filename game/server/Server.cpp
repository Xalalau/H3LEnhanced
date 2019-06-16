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
cvar_t	allowmonsters={"mp_allowmonsters","0", FCVAR_SERVER };

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
cvar_t	sk_controller_health1 = {"sk_controller_health1","0"};
cvar_t	sk_controller_health2 = {"sk_controller_health2","0"};
cvar_t	sk_controller_health3 = {"sk_controller_health3","0"};

cvar_t	sk_controller_dmgzap1 = {"sk_controller_dmgzap1","0"};
cvar_t	sk_controller_dmgzap2 = {"sk_controller_dmgzap2","0"};
cvar_t	sk_controller_dmgzap3 = {"sk_controller_dmgzap3","0"};

cvar_t	sk_controller_speedball1 = {"sk_controller_speedball1","0"};
cvar_t	sk_controller_speedball2 = {"sk_controller_speedball2","0"};
cvar_t	sk_controller_speedball3 = {"sk_controller_speedball3","0"};

cvar_t	sk_controller_dmgball1 = {"sk_controller_dmgball1","0"};
cvar_t	sk_controller_dmgball2 = {"sk_controller_dmgball2","0"};
cvar_t	sk_controller_dmgball3 = {"sk_controller_dmgball3","0"};

// Nihilanth
cvar_t	sk_nihilanth_health1 = {"sk_nihilanth_health1","0"};
cvar_t	sk_nihilanth_health2 = {"sk_nihilanth_health2","0"};
cvar_t	sk_nihilanth_health3 = {"sk_nihilanth_health3","0"};

cvar_t	sk_nihilanth_zap1 = {"sk_nihilanth_zap1","0"};
cvar_t	sk_nihilanth_zap2 = {"sk_nihilanth_zap2","0"};
cvar_t	sk_nihilanth_zap3 = {"sk_nihilanth_zap3","0"};

// Scientist
cvar_t	sk_scientist_health1 = {"sk_scientist_health1","0"};
cvar_t	sk_scientist_health2 = {"sk_scientist_health2","0"};
cvar_t	sk_scientist_health3 = {"sk_scientist_health3","0"};


// Snark
cvar_t	sk_snark_health1 = {"sk_snark_health1","0"};
cvar_t	sk_snark_health2 = {"sk_snark_health2","0"};
cvar_t	sk_snark_health3 = {"sk_snark_health3","0"};

cvar_t	sk_snark_dmg_bite1 = {"sk_snark_dmg_bite1","0"};
cvar_t	sk_snark_dmg_bite2 = {"sk_snark_dmg_bite2","0"};
cvar_t	sk_snark_dmg_bite3 = {"sk_snark_dmg_bite3","0"};

cvar_t	sk_snark_dmg_pop1 = {"sk_snark_dmg_pop1","0"};
cvar_t	sk_snark_dmg_pop2 = {"sk_snark_dmg_pop2","0"};
cvar_t	sk_snark_dmg_pop3 = {"sk_snark_dmg_pop3","0"};



// Zombie
cvar_t	sk_zombie_health1 = {"sk_zombie_health1","0"};
cvar_t	sk_zombie_health2 = {"sk_zombie_health2","0"};
cvar_t	sk_zombie_health3 = {"sk_zombie_health3","0"};

cvar_t	sk_zombie_dmg_one_slash1 = {"sk_zombie_dmg_one_slash1","0"};
cvar_t	sk_zombie_dmg_one_slash2 = {"sk_zombie_dmg_one_slash2","0"};
cvar_t	sk_zombie_dmg_one_slash3 = {"sk_zombie_dmg_one_slash3","0"};

cvar_t	sk_zombie_dmg_both_slash1 = {"sk_zombie_dmg_both_slash1","0"};
cvar_t	sk_zombie_dmg_both_slash2 = {"sk_zombie_dmg_both_slash2","0"};
cvar_t	sk_zombie_dmg_both_slash3 = {"sk_zombie_dmg_both_slash3","0"};


//Turret
cvar_t	sk_turret_health1 = {"sk_turret_health1","0"};
cvar_t	sk_turret_health2 = {"sk_turret_health2","0"};
cvar_t	sk_turret_health3 = {"sk_turret_health3","0"};


// MiniTurret
cvar_t	sk_miniturret_health1 = {"sk_miniturret_health1","0"};
cvar_t	sk_miniturret_health2 = {"sk_miniturret_health2","0"};
cvar_t	sk_miniturret_health3 = {"sk_miniturret_health3","0"};


// Sentry Turret
cvar_t	sk_sentry_health1 = {"sk_sentry_health1","0"};
cvar_t	sk_sentry_health2 = {"sk_sentry_health2","0"};
cvar_t	sk_sentry_health3 = {"sk_sentry_health3","0"};


// PLAYER WEAPONS

// Crowbar whack
cvar_t	sk_plr_crowbar1 = {"sk_plr_crowbar1","0"};
cvar_t	sk_plr_crowbar2 = {"sk_plr_crowbar2","0"};
cvar_t	sk_plr_crowbar3 = {"sk_plr_crowbar3","0"};

cvar_t	sk_plr_crowbar_mp1 = { "sk_plr_crowbar_mp1","0" };
cvar_t	sk_plr_crowbar_mp2 = { "sk_plr_crowbar_mp2","0" };
cvar_t	sk_plr_crowbar_mp3 = { "sk_plr_crowbar_mp3","0" };

// Glock Round
cvar_t	sk_plr_9mm_bullet1 = {"sk_plr_9mm_bullet1","0"};
cvar_t	sk_plr_9mm_bullet2 = {"sk_plr_9mm_bullet2","0"};
cvar_t	sk_plr_9mm_bullet3 = {"sk_plr_9mm_bullet3","0"};

cvar_t	sk_plr_9mm_bullet_mp1 = { "sk_plr_9mm_bullet_mp1","0" };
cvar_t	sk_plr_9mm_bullet_mp2 = { "sk_plr_9mm_bullet_mp2","0" };
cvar_t	sk_plr_9mm_bullet_mp3 = { "sk_plr_9mm_bullet_mp3","0" };

// 357 Round
cvar_t	sk_plr_357_bullet1 = {"sk_plr_357_bullet1","0"};
cvar_t	sk_plr_357_bullet2 = {"sk_plr_357_bullet2","0"};
cvar_t	sk_plr_357_bullet3 = {"sk_plr_357_bullet3","0"};

cvar_t	sk_plr_357_bullet_mp1 = { "sk_plr_357_bullet_mp1","0" };
cvar_t	sk_plr_357_bullet_mp2 = { "sk_plr_357_bullet_mp2","0" };
cvar_t	sk_plr_357_bullet_mp3 = { "sk_plr_357_bullet_mp3","0" };

// MP5 Round
cvar_t	sk_plr_9mmAR_bullet1 = {"sk_plr_9mmAR_bullet1","0"};
cvar_t	sk_plr_9mmAR_bullet2 = {"sk_plr_9mmAR_bullet2","0"};
cvar_t	sk_plr_9mmAR_bullet3 = {"sk_plr_9mmAR_bullet3","0"};

cvar_t	sk_plr_9mmAR_bullet_mp1 = { "sk_plr_9mmAR_bullet_mp1","0" };
cvar_t	sk_plr_9mmAR_bullet_mp2 = { "sk_plr_9mmAR_bullet_mp2","0" };
cvar_t	sk_plr_9mmAR_bullet_mp3 = { "sk_plr_9mmAR_bullet_mp3","0" };

// M203 grenade
cvar_t	sk_plr_9mmAR_grenade1 = {"sk_plr_9mmAR_grenade1","0"};
cvar_t	sk_plr_9mmAR_grenade2 = {"sk_plr_9mmAR_grenade2","0"};
cvar_t	sk_plr_9mmAR_grenade3 = {"sk_plr_9mmAR_grenade3","0"};

cvar_t	sk_plr_9mmAR_grenade_mp1 = { "sk_plr_9mmAR_grenade_mp1","0" };
cvar_t	sk_plr_9mmAR_grenade_mp2 = { "sk_plr_9mmAR_grenade_mp2","0" };
cvar_t	sk_plr_9mmAR_grenade_mp3 = { "sk_plr_9mmAR_grenade_mp3","0" };

// Shotgun buckshot
cvar_t	sk_plr_buckshot1 = {"sk_plr_buckshot1","0"};
cvar_t	sk_plr_buckshot2 = {"sk_plr_buckshot2","0"};
cvar_t	sk_plr_buckshot3 = {"sk_plr_buckshot3","0"};

cvar_t	sk_plr_buckshot_mp1 = { "sk_plr_buckshot_mp1","0" };
cvar_t	sk_plr_buckshot_mp2 = { "sk_plr_buckshot_mp2","0" };
cvar_t	sk_plr_buckshot_mp3 = { "sk_plr_buckshot_mp3","0" };

// Crossbow
cvar_t	sk_plr_xbow_bolt_client1 = {"sk_plr_xbow_bolt_client1","0"};
cvar_t	sk_plr_xbow_bolt_client2 = {"sk_plr_xbow_bolt_client2","0"};
cvar_t	sk_plr_xbow_bolt_client3 = {"sk_plr_xbow_bolt_client3","0"};

cvar_t	sk_plr_xbow_bolt_client_mp1 = { "sk_plr_xbow_bolt_client_mp1","0" };
cvar_t	sk_plr_xbow_bolt_client_mp2 = { "sk_plr_xbow_bolt_client_mp2","0" };
cvar_t	sk_plr_xbow_bolt_client_mp3 = { "sk_plr_xbow_bolt_client_mp3","0" };

cvar_t	sk_plr_xbow_bolt_monster1 = {"sk_plr_xbow_bolt_monster1","0"};
cvar_t	sk_plr_xbow_bolt_monster2 = {"sk_plr_xbow_bolt_monster2","0"};
cvar_t	sk_plr_xbow_bolt_monster3 = {"sk_plr_xbow_bolt_monster3","0"};

// RPG
cvar_t	sk_plr_rpg1 = {"sk_plr_rpg1","0"};
cvar_t	sk_plr_rpg2 = {"sk_plr_rpg2","0"};
cvar_t	sk_plr_rpg3 = {"sk_plr_rpg3","0"};

cvar_t	sk_plr_rpg_mp1 = { "sk_plr_rpg_mp1","0" };
cvar_t	sk_plr_rpg_mp2 = { "sk_plr_rpg_mp2","0" };
cvar_t	sk_plr_rpg_mp3 = { "sk_plr_rpg_mp3","0" };

// Zero Point Generator
cvar_t	sk_plr_gauss1 = {"sk_plr_gauss1","0"};
cvar_t	sk_plr_gauss2 = {"sk_plr_gauss2","0"};
cvar_t	sk_plr_gauss3 = {"sk_plr_gauss3","0"};


// Tau Cannon
cvar_t	sk_plr_egon_narrow1 = {"sk_plr_egon_narrow1","0"};
cvar_t	sk_plr_egon_narrow2 = {"sk_plr_egon_narrow2","0"};
cvar_t	sk_plr_egon_narrow3 = {"sk_plr_egon_narrow3","0"};

cvar_t	sk_plr_egon_narrow_mp1 = { "sk_plr_egon_narrow_mp1","0" };
cvar_t	sk_plr_egon_narrow_mp2 = { "sk_plr_egon_narrow_mp2","0" };
cvar_t	sk_plr_egon_narrow_mp3 = { "sk_plr_egon_narrow_mp3","0" };

cvar_t	sk_plr_egon_wide1 = {"sk_plr_egon_wide1","0"};
cvar_t	sk_plr_egon_wide2 = {"sk_plr_egon_wide2","0"};
cvar_t	sk_plr_egon_wide3 = {"sk_plr_egon_wide3","0"};

cvar_t	sk_plr_egon_wide_mp1 = { "sk_plr_egon_wide_mp1","0" };
cvar_t	sk_plr_egon_wide_mp2 = { "sk_plr_egon_wide_mp2","0" };
cvar_t	sk_plr_egon_wide_mp3 = { "sk_plr_egon_wide_mp3","0" };

// Hand Grendade
cvar_t	sk_plr_hand_grenade1 = {"sk_plr_hand_grenade1","0"};
cvar_t	sk_plr_hand_grenade2 = {"sk_plr_hand_grenade2","0"};
cvar_t	sk_plr_hand_grenade3 = {"sk_plr_hand_grenade3","0"};

cvar_t	sk_plr_hand_grenade_mp1 = { "sk_plr_hand_grenade_mp1","0" };
cvar_t	sk_plr_hand_grenade_mp2 = { "sk_plr_hand_grenade_mp2","0" };
cvar_t	sk_plr_hand_grenade_mp3 = { "sk_plr_hand_grenade_mp3","0" };

// Satchel Charge
cvar_t	sk_plr_satchel1	= {"sk_plr_satchel1","0"};
cvar_t	sk_plr_satchel2	= {"sk_plr_satchel2","0"};
cvar_t	sk_plr_satchel3	= {"sk_plr_satchel3","0"};

cvar_t	sk_plr_satchel_mp1 = { "sk_plr_satchel_mp1","0" };
cvar_t	sk_plr_satchel_mp2 = { "sk_plr_satchel_mp2","0" };
cvar_t	sk_plr_satchel_mp3 = { "sk_plr_satchel_mp3","0" };

// Tripmine
cvar_t	sk_plr_tripmine1 = {"sk_plr_tripmine1","0"};
cvar_t	sk_plr_tripmine2 = {"sk_plr_tripmine2","0"};
cvar_t	sk_plr_tripmine3 = {"sk_plr_tripmine3","0"};

cvar_t	sk_plr_tripmine_mp1 = { "sk_plr_tripmine_mp1","0" };
cvar_t	sk_plr_tripmine_mp2 = { "sk_plr_tripmine_mp2","0" };
cvar_t	sk_plr_tripmine_mp3 = { "sk_plr_tripmine_mp3","0" };

#if USE_OPFOR
// Knife whack
cvar_t	sk_plr_knife1 = { "sk_plr_knife1","0" };
cvar_t	sk_plr_knife2 = { "sk_plr_knife2","0" };
cvar_t	sk_plr_knife3 = { "sk_plr_knife3","0" };

// Pipewrench whack
cvar_t	sk_plr_pipewrench1 = { "sk_plr_pipewrench1","0" };
cvar_t	sk_plr_pipewrench2 = { "sk_plr_pipewrench2","0" };
cvar_t	sk_plr_pipewrench3 = { "sk_plr_pipewrench3","0" };

// Grapple
cvar_t	sk_plr_grapple1 = { "sk_plr_grapple1","0" };
cvar_t	sk_plr_grapple2 = { "sk_plr_grapple2","0" };
cvar_t	sk_plr_grapple3 = { "sk_plr_grapple3","0" };

// 556 Round
cvar_t	sk_plr_556_bullet1 = { "sk_plr_556_bullet1","0" };
cvar_t	sk_plr_556_bullet2 = { "sk_plr_556_bullet2","0" };
cvar_t	sk_plr_556_bullet3 = { "sk_plr_556_bullet3","0" };

// 762 Round
cvar_t	sk_plr_762_bullet1 = { "sk_plr_762_bullet1","0" };
cvar_t	sk_plr_762_bullet2 = { "sk_plr_762_bullet2","0" };
cvar_t	sk_plr_762_bullet3 = { "sk_plr_762_bullet3","0" };

// 762 Round
cvar_t	sk_plr_eagle1 = { "sk_plr_eagle1","0" };
cvar_t	sk_plr_eagle2 = { "sk_plr_eagle2","0" };
cvar_t	sk_plr_eagle3 = { "sk_plr_eagle3","0" };

// Shock Roach
cvar_t	sk_plr_shockroachs1 = { "sk_plr_shockroachs1","0" };
cvar_t	sk_plr_shockroachs2 = { "sk_plr_shockroachs2","0" };
cvar_t	sk_plr_shockroachs3 = { "sk_plr_shockroachs3","0" };

cvar_t	sk_plr_shockroachm1 = { "sk_plr_shockroachm1","0" };
cvar_t	sk_plr_shockroachm2 = { "sk_plr_shockroachm2","0" };
cvar_t	sk_plr_shockroachm3 = { "sk_plr_shockroachm3","0" };

// Displacer Cannon
cvar_t	sk_plr_displacer_other1 = { "sk_plr_displacer_other1","0" };
cvar_t	sk_plr_displacer_other2 = { "sk_plr_displacer_other2","0" };
cvar_t	sk_plr_displacer_other3 = { "sk_plr_displacer_other3","0" };

cvar_t	sk_plr_displacer_radius1 = { "sk_plr_displacer_radius1","0" };
cvar_t	sk_plr_displacer_radius2 = { "sk_plr_displacer_radius2","0" };
cvar_t	sk_plr_displacer_radius3 = { "sk_plr_displacer_radius3","0" };

cvar_t	sk_plr_spore1 = { "sk_plr_spore1","0" };
cvar_t	sk_plr_spore2 = { "sk_plr_spore2","0" };
cvar_t	sk_plr_spore3 = { "sk_plr_spore3","0" };
#endif

// WORLD WEAPONS
cvar_t	sk_12mm_bullet1 = {"sk_12mm_bullet1","0"};
cvar_t	sk_12mm_bullet2 = {"sk_12mm_bullet2","0"};
cvar_t	sk_12mm_bullet3 = {"sk_12mm_bullet3","0"};

cvar_t	sk_9mmAR_bullet1 = {"sk_9mmAR_bullet1","0"};
cvar_t	sk_9mmAR_bullet2 = {"sk_9mmAR_bullet2","0"};
cvar_t	sk_9mmAR_bullet3 = {"sk_9mmAR_bullet3","0"};

cvar_t	sk_9mm_bullet1 = {"sk_9mm_bullet1","0"};
cvar_t	sk_9mm_bullet2 = {"sk_9mm_bullet2","0"};
cvar_t	sk_9mm_bullet3 = {"sk_9mm_bullet3","0"};


// HORNET
cvar_t	sk_plr_hornet_dmg1 = { "sk_plr_hornet_dmg1","0" };
cvar_t	sk_plr_hornet_dmg2 = { "sk_plr_hornet_dmg2","0" };
cvar_t	sk_plr_hornet_dmg3 = { "sk_plr_hornet_dmg3","0" };

cvar_t	sk_plr_hornet_dmg_mp1 = { "sk_plr_hornet_dmg_mp1","0" };
cvar_t	sk_plr_hornet_dmg_mp2 = { "sk_plr_hornet_dmg_mp2","0" };
cvar_t	sk_plr_hornet_dmg_mp3 = { "sk_plr_hornet_dmg_mp3","0" };

// HEALTH/CHARGE
cvar_t	sk_suitcharger1	= { "sk_suitcharger1","0" };
cvar_t	sk_suitcharger2	= { "sk_suitcharger2","0" };
cvar_t	sk_suitcharger3	= { "sk_suitcharger3","0" };

cvar_t	sk_suitcharger_mp1 = { "sk_suitcharger_mp1","0" };
cvar_t	sk_suitcharger_mp2 = { "sk_suitcharger_mp2","0" };
cvar_t	sk_suitcharger_mp3 = { "sk_suitcharger_mp3","0" };

cvar_t	sk_battery1	= { "sk_battery1","0" };			
cvar_t	sk_battery2	= { "sk_battery2","0" };			
cvar_t	sk_battery3	= { "sk_battery3","0" };			

cvar_t	sk_healthcharger1	= { "sk_healthcharger1","0" };		
cvar_t	sk_healthcharger2	= { "sk_healthcharger2","0" };		
cvar_t	sk_healthcharger3	= { "sk_healthcharger3","0" };		

cvar_t	sk_healthkit1	= { "sk_healthkit1","0" };		
cvar_t	sk_healthkit2	= { "sk_healthkit2","0" };		
cvar_t	sk_healthkit3	= { "sk_healthkit3","0" };		

cvar_t	sk_scientist_heal1	= { "sk_scientist_heal1","0" };	
cvar_t	sk_scientist_heal2	= { "sk_scientist_heal2","0" };	
cvar_t	sk_scientist_heal3	= { "sk_scientist_heal3","0" };	


// monster damage adjusters
cvar_t	sk_monster_head1	= { "sk_monster_head1","2" };
cvar_t	sk_monster_head2	= { "sk_monster_head2","2" };
cvar_t	sk_monster_head3	= { "sk_monster_head3","2" };

cvar_t	sk_monster_chest1	= { "sk_monster_chest1","1" };
cvar_t	sk_monster_chest2	= { "sk_monster_chest2","1" };
cvar_t	sk_monster_chest3	= { "sk_monster_chest3","1" };

cvar_t	sk_monster_stomach1	= { "sk_monster_stomach1","1" };
cvar_t	sk_monster_stomach2	= { "sk_monster_stomach2","1" };
cvar_t	sk_monster_stomach3	= { "sk_monster_stomach3","1" };

cvar_t	sk_monster_arm1	= { "sk_monster_arm1","1" };
cvar_t	sk_monster_arm2	= { "sk_monster_arm2","1" };
cvar_t	sk_monster_arm3	= { "sk_monster_arm3","1" };

cvar_t	sk_monster_leg1	= { "sk_monster_leg1","1" };
cvar_t	sk_monster_leg2	= { "sk_monster_leg2","1" };
cvar_t	sk_monster_leg3	= { "sk_monster_leg3","1" };

// player damage adjusters
cvar_t	sk_player_head1	= { "sk_player_head1","2" };
cvar_t	sk_player_head2	= { "sk_player_head2","2" };
cvar_t	sk_player_head3	= { "sk_player_head3","2" };

cvar_t	sk_player_chest1	= { "sk_player_chest1","1" };
cvar_t	sk_player_chest2	= { "sk_player_chest2","1" };
cvar_t	sk_player_chest3	= { "sk_player_chest3","1" };

cvar_t	sk_player_stomach1	= { "sk_player_stomach1","1" };
cvar_t	sk_player_stomach2	= { "sk_player_stomach2","1" };
cvar_t	sk_player_stomach3	= { "sk_player_stomach3","1" };

cvar_t	sk_player_arm1	= { "sk_player_arm1","1" };
cvar_t	sk_player_arm2	= { "sk_player_arm2","1" };
cvar_t	sk_player_arm3	= { "sk_player_arm3","1" };

cvar_t	sk_player_leg1	= { "sk_player_leg1","1" };
cvar_t	sk_player_leg2	= { "sk_player_leg2","1" };
cvar_t	sk_player_leg3	= { "sk_player_leg3","1" };

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

// REGISTER CVARS FOR SKILL LEVEL STUFF
	// Agrunt
	CVAR_REGISTER ( &sk_agrunt_health1 );// {"sk_agrunt_health1","0"};
	CVAR_REGISTER ( &sk_agrunt_health2 );// {"sk_agrunt_health2","0"};
	CVAR_REGISTER ( &sk_agrunt_health3 );// {"sk_agrunt_health3","0"};

	CVAR_REGISTER ( &sk_agrunt_dmg_punch1 );// {"sk_agrunt_dmg_punch1","0"};
	CVAR_REGISTER ( &sk_agrunt_dmg_punch2 );// {"sk_agrunt_dmg_punch2","0"};
	CVAR_REGISTER ( &sk_agrunt_dmg_punch3 );// {"sk_agrunt_dmg_punch3","0"};

	// Apache
	CVAR_REGISTER ( &sk_apache_health1 );// {"sk_apache_health1","0"};
	CVAR_REGISTER ( &sk_apache_health2 );// {"sk_apache_health2","0"};
	CVAR_REGISTER ( &sk_apache_health3 );// {"sk_apache_health3","0"};

	// Barney
	CVAR_REGISTER ( &sk_barney_health1 );// {"sk_barney_health1","0"};
	CVAR_REGISTER ( &sk_barney_health2 );// {"sk_barney_health2","0"};
	CVAR_REGISTER ( &sk_barney_health3 );// {"sk_barney_health3","0"};

	// Bullsquid
	CVAR_REGISTER ( &sk_bullsquid_health1 );// {"sk_bullsquid_health1","0"};
	CVAR_REGISTER ( &sk_bullsquid_health2 );// {"sk_bullsquid_health2","0"};
	CVAR_REGISTER ( &sk_bullsquid_health3 );// {"sk_bullsquid_health3","0"};

	CVAR_REGISTER ( &sk_bullsquid_dmg_bite1 );// {"sk_bullsquid_dmg_bite1","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_bite2 );// {"sk_bullsquid_dmg_bite2","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_bite3 );// {"sk_bullsquid_dmg_bite3","0"};

	CVAR_REGISTER ( &sk_bullsquid_dmg_whip1 );// {"sk_bullsquid_dmg_whip1","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_whip2 );// {"sk_bullsquid_dmg_whip2","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_whip3 );// {"sk_bullsquid_dmg_whip3","0"};

	CVAR_REGISTER ( &sk_bullsquid_dmg_spit1 );// {"sk_bullsquid_dmg_spit1","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_spit2 );// {"sk_bullsquid_dmg_spit2","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_spit3 );// {"sk_bullsquid_dmg_spit3","0"};


	CVAR_REGISTER ( &sk_bigmomma_health_factor1 );// {"sk_bigmomma_health_factor1","1.0"};
	CVAR_REGISTER ( &sk_bigmomma_health_factor2 );// {"sk_bigmomma_health_factor2","1.0"};
	CVAR_REGISTER ( &sk_bigmomma_health_factor3 );// {"sk_bigmomma_health_factor3","1.0"};

	CVAR_REGISTER ( &sk_bigmomma_dmg_slash1 );// {"sk_bigmomma_dmg_slash1","50"};
	CVAR_REGISTER ( &sk_bigmomma_dmg_slash2 );// {"sk_bigmomma_dmg_slash2","50"};
	CVAR_REGISTER ( &sk_bigmomma_dmg_slash3 );// {"sk_bigmomma_dmg_slash3","50"};

	CVAR_REGISTER ( &sk_bigmomma_dmg_blast1 );// {"sk_bigmomma_dmg_blast1","100"};
	CVAR_REGISTER ( &sk_bigmomma_dmg_blast2 );// {"sk_bigmomma_dmg_blast2","100"};
	CVAR_REGISTER ( &sk_bigmomma_dmg_blast3 );// {"sk_bigmomma_dmg_blast3","100"};

	CVAR_REGISTER ( &sk_bigmomma_radius_blast1 );// {"sk_bigmomma_radius_blast1","250"};
	CVAR_REGISTER ( &sk_bigmomma_radius_blast2 );// {"sk_bigmomma_radius_blast2","250"};
	CVAR_REGISTER ( &sk_bigmomma_radius_blast3 );// {"sk_bigmomma_radius_blast3","250"};

	// Gargantua
	CVAR_REGISTER ( &sk_gargantua_health1 );// {"sk_gargantua_health1","0"};
	CVAR_REGISTER ( &sk_gargantua_health2 );// {"sk_gargantua_health2","0"};
	CVAR_REGISTER ( &sk_gargantua_health3 );// {"sk_gargantua_health3","0"};

	CVAR_REGISTER ( &sk_gargantua_dmg_slash1 );// {"sk_gargantua_dmg_slash1","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_slash2 );// {"sk_gargantua_dmg_slash2","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_slash3 );// {"sk_gargantua_dmg_slash3","0"};

	CVAR_REGISTER ( &sk_gargantua_dmg_fire1 );// {"sk_gargantua_dmg_fire1","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_fire2 );// {"sk_gargantua_dmg_fire2","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_fire3 );// {"sk_gargantua_dmg_fire3","0"};

	CVAR_REGISTER ( &sk_gargantua_dmg_stomp1 );// {"sk_gargantua_dmg_stomp1","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_stomp2 );// {"sk_gargantua_dmg_stomp2","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_stomp3	);// {"sk_gargantua_dmg_stomp3","0"};


	// Hassassin
	CVAR_REGISTER ( &sk_hassassin_health1 );// {"sk_hassassin_health1","0"};
	CVAR_REGISTER ( &sk_hassassin_health2 );// {"sk_hassassin_health2","0"};
	CVAR_REGISTER ( &sk_hassassin_health3 );// {"sk_hassassin_health3","0"};


	// Headcrab
	CVAR_REGISTER ( &sk_headcrab_health1 );// {"sk_headcrab_health1","0"};
	CVAR_REGISTER ( &sk_headcrab_health2 );// {"sk_headcrab_health2","0"};
	CVAR_REGISTER ( &sk_headcrab_health3 );// {"sk_headcrab_health3","0"};

	CVAR_REGISTER ( &sk_headcrab_dmg_bite1 );// {"sk_headcrab_dmg_bite1","0"};
	CVAR_REGISTER ( &sk_headcrab_dmg_bite2 );// {"sk_headcrab_dmg_bite2","0"};
	CVAR_REGISTER ( &sk_headcrab_dmg_bite3 );// {"sk_headcrab_dmg_bite3","0"};


	// Hgrunt 
	CVAR_REGISTER ( &sk_hgrunt_health1 );// {"sk_hgrunt_health1","0"};
	CVAR_REGISTER ( &sk_hgrunt_health2 );// {"sk_hgrunt_health2","0"};
	CVAR_REGISTER ( &sk_hgrunt_health3 );// {"sk_hgrunt_health3","0"};

	CVAR_REGISTER ( &sk_hgrunt_kick1 );// {"sk_hgrunt_kick1","0"};
	CVAR_REGISTER ( &sk_hgrunt_kick2 );// {"sk_hgrunt_kick2","0"};
	CVAR_REGISTER ( &sk_hgrunt_kick3 );// {"sk_hgrunt_kick3","0"};

	CVAR_REGISTER ( &sk_hgrunt_pellets1 );
	CVAR_REGISTER ( &sk_hgrunt_pellets2 );
	CVAR_REGISTER ( &sk_hgrunt_pellets3 );

	CVAR_REGISTER ( &sk_hgrunt_gspeed1 );
	CVAR_REGISTER ( &sk_hgrunt_gspeed2 );
	CVAR_REGISTER ( &sk_hgrunt_gspeed3 );

	// HORNET
	CVAR_REGISTER( &sk_hornet_dmg1 );// {"sk_hornet_dmg1","0"};
	CVAR_REGISTER( &sk_hornet_dmg2 );// {"sk_hornet_dmg2","0"};
	CVAR_REGISTER( &sk_hornet_dmg3 );// {"sk_hornet_dmg3","0"};

	// Houndeye
	CVAR_REGISTER ( &sk_houndeye_health1 );// {"sk_houndeye_health1","0"};
	CVAR_REGISTER ( &sk_houndeye_health2 );// {"sk_houndeye_health2","0"};
	CVAR_REGISTER ( &sk_houndeye_health3 );// {"sk_houndeye_health3","0"};

	CVAR_REGISTER ( &sk_houndeye_dmg_blast1 );// {"sk_houndeye_dmg_blast1","0"};
	CVAR_REGISTER ( &sk_houndeye_dmg_blast2 );// {"sk_houndeye_dmg_blast2","0"};
	CVAR_REGISTER ( &sk_houndeye_dmg_blast3 );// {"sk_houndeye_dmg_blast3","0"};


	// ISlave
	CVAR_REGISTER ( &sk_islave_health1 );// {"sk_islave_health1","0"};
	CVAR_REGISTER ( &sk_islave_health2 );// {"sk_islave_health2","0"};
	CVAR_REGISTER ( &sk_islave_health3 );// {"sk_islave_health3","0"};

	CVAR_REGISTER ( &sk_islave_dmg_claw1 );// {"sk_islave_dmg_claw1","0"};
	CVAR_REGISTER ( &sk_islave_dmg_claw2 );// {"sk_islave_dmg_claw2","0"};
	CVAR_REGISTER ( &sk_islave_dmg_claw3 );// {"sk_islave_dmg_claw3","0"};

	CVAR_REGISTER ( &sk_islave_dmg_clawrake1	);// {"sk_islave_dmg_clawrake1","0"};
	CVAR_REGISTER ( &sk_islave_dmg_clawrake2	);// {"sk_islave_dmg_clawrake2","0"};
	CVAR_REGISTER ( &sk_islave_dmg_clawrake3	);// {"sk_islave_dmg_clawrake3","0"};
		
	CVAR_REGISTER ( &sk_islave_dmg_zap1 );// {"sk_islave_dmg_zap1","0"};
	CVAR_REGISTER ( &sk_islave_dmg_zap2 );// {"sk_islave_dmg_zap2","0"};
	CVAR_REGISTER ( &sk_islave_dmg_zap3 );// {"sk_islave_dmg_zap3","0"};


	// Icthyosaur
	CVAR_REGISTER ( &sk_ichthyosaur_health1	);// {"sk_ichthyosaur_health1","0"};
	CVAR_REGISTER ( &sk_ichthyosaur_health2	);// {"sk_ichthyosaur_health2","0"};
	CVAR_REGISTER ( &sk_ichthyosaur_health3	);// {"sk_ichthyosaur_health3","0"};

	CVAR_REGISTER ( &sk_ichthyosaur_shake1	);// {"sk_ichthyosaur_health3","0"};
	CVAR_REGISTER ( &sk_ichthyosaur_shake2	);// {"sk_ichthyosaur_health3","0"};
	CVAR_REGISTER ( &sk_ichthyosaur_shake3	);// {"sk_ichthyosaur_health3","0"};



	// Leech
	CVAR_REGISTER ( &sk_leech_health1 );// {"sk_leech_health1","0"};
	CVAR_REGISTER ( &sk_leech_health2 );// {"sk_leech_health2","0"};
	CVAR_REGISTER ( &sk_leech_health3 );// {"sk_leech_health3","0"};

	CVAR_REGISTER ( &sk_leech_dmg_bite1 );// {"sk_leech_dmg_bite1","0"};
	CVAR_REGISTER ( &sk_leech_dmg_bite2 );// {"sk_leech_dmg_bite2","0"};
	CVAR_REGISTER ( &sk_leech_dmg_bite3 );// {"sk_leech_dmg_bite3","0"};


	// Controller
	CVAR_REGISTER ( &sk_controller_health1 );
	CVAR_REGISTER ( &sk_controller_health2 );
	CVAR_REGISTER ( &sk_controller_health3 );

	CVAR_REGISTER ( &sk_controller_dmgzap1 );
	CVAR_REGISTER ( &sk_controller_dmgzap2 );
	CVAR_REGISTER ( &sk_controller_dmgzap3 );

	CVAR_REGISTER ( &sk_controller_speedball1 );
	CVAR_REGISTER ( &sk_controller_speedball2 );
	CVAR_REGISTER ( &sk_controller_speedball3 );

	CVAR_REGISTER ( &sk_controller_dmgball1 );
	CVAR_REGISTER ( &sk_controller_dmgball2 );
	CVAR_REGISTER ( &sk_controller_dmgball3 );

	// Nihilanth
	CVAR_REGISTER ( &sk_nihilanth_health1 );// {"sk_nihilanth_health1","0"};
	CVAR_REGISTER ( &sk_nihilanth_health2 );// {"sk_nihilanth_health2","0"};
	CVAR_REGISTER ( &sk_nihilanth_health3 );// {"sk_nihilanth_health3","0"};

	CVAR_REGISTER ( &sk_nihilanth_zap1 );
	CVAR_REGISTER ( &sk_nihilanth_zap2 );
	CVAR_REGISTER ( &sk_nihilanth_zap3 );

	// Scientist
	CVAR_REGISTER ( &sk_scientist_health1 );// {"sk_scientist_health1","0"};
	CVAR_REGISTER ( &sk_scientist_health2 );// {"sk_scientist_health2","0"};
	CVAR_REGISTER ( &sk_scientist_health3 );// {"sk_scientist_health3","0"};


	// Snark
	CVAR_REGISTER ( &sk_snark_health1 );// {"sk_snark_health1","0"};
	CVAR_REGISTER ( &sk_snark_health2 );// {"sk_snark_health2","0"};
	CVAR_REGISTER ( &sk_snark_health3 );// {"sk_snark_health3","0"};

	CVAR_REGISTER ( &sk_snark_dmg_bite1 );// {"sk_snark_dmg_bite1","0"};
	CVAR_REGISTER ( &sk_snark_dmg_bite2 );// {"sk_snark_dmg_bite2","0"};
	CVAR_REGISTER ( &sk_snark_dmg_bite3 );// {"sk_snark_dmg_bite3","0"};

	CVAR_REGISTER ( &sk_snark_dmg_pop1 );// {"sk_snark_dmg_pop1","0"};
	CVAR_REGISTER ( &sk_snark_dmg_pop2 );// {"sk_snark_dmg_pop2","0"};
	CVAR_REGISTER ( &sk_snark_dmg_pop3 );// {"sk_snark_dmg_pop3","0"};



	// Zombie
	CVAR_REGISTER ( &sk_zombie_health1 );// {"sk_zombie_health1","0"};
	CVAR_REGISTER ( &sk_zombie_health2 );// {"sk_zombie_health3","0"};
	CVAR_REGISTER ( &sk_zombie_health3 );// {"sk_zombie_health3","0"};

	CVAR_REGISTER ( &sk_zombie_dmg_one_slash1 );// {"sk_zombie_dmg_one_slash1","0"};
	CVAR_REGISTER ( &sk_zombie_dmg_one_slash2 );// {"sk_zombie_dmg_one_slash2","0"};
	CVAR_REGISTER ( &sk_zombie_dmg_one_slash3 );// {"sk_zombie_dmg_one_slash3","0"};

	CVAR_REGISTER ( &sk_zombie_dmg_both_slash1 );// {"sk_zombie_dmg_both_slash1","0"};
	CVAR_REGISTER ( &sk_zombie_dmg_both_slash2 );// {"sk_zombie_dmg_both_slash2","0"};
	CVAR_REGISTER ( &sk_zombie_dmg_both_slash3 );// {"sk_zombie_dmg_both_slash3","0"};


	//Turret
	CVAR_REGISTER ( &sk_turret_health1 );// {"sk_turret_health1","0"};
	CVAR_REGISTER ( &sk_turret_health2 );// {"sk_turret_health2","0"};
	CVAR_REGISTER ( &sk_turret_health3 );// {"sk_turret_health3","0"};


	// MiniTurret
	CVAR_REGISTER ( &sk_miniturret_health1 );// {"sk_miniturret_health1","0"};
	CVAR_REGISTER ( &sk_miniturret_health2 );// {"sk_miniturret_health2","0"};
	CVAR_REGISTER ( &sk_miniturret_health3 );// {"sk_miniturret_health3","0"};


	// Sentry Turret
	CVAR_REGISTER ( &sk_sentry_health1 );// {"sk_sentry_health1","0"};
	CVAR_REGISTER ( &sk_sentry_health2 );// {"sk_sentry_health2","0"};
	CVAR_REGISTER ( &sk_sentry_health3 );// {"sk_sentry_health3","0"};


	// PLAYER WEAPONS

	// Crowbar whack
	CVAR_REGISTER ( &sk_plr_crowbar1 );// {"sk_plr_crowbar1","0"};
	CVAR_REGISTER ( &sk_plr_crowbar2 );// {"sk_plr_crowbar2","0"};
	CVAR_REGISTER ( &sk_plr_crowbar3 );// {"sk_plr_crowbar3","0"};

	CVAR_REGISTER( &sk_plr_crowbar_mp1 );// {"sk_plr_crowbar_mp1","0"};
	CVAR_REGISTER( &sk_plr_crowbar_mp2 );// {"sk_plr_crowbar_mp2","0"};
	CVAR_REGISTER( &sk_plr_crowbar_mp3 );// {"sk_plr_crowbar_mp3","0"};

	// Glock Round
	CVAR_REGISTER ( &sk_plr_9mm_bullet1 );// {"sk_plr_9mm_bullet1","0"};
	CVAR_REGISTER ( &sk_plr_9mm_bullet2 );// {"sk_plr_9mm_bullet2","0"};
	CVAR_REGISTER ( &sk_plr_9mm_bullet3 );// {"sk_plr_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_plr_9mm_bullet_mp1 );// {"sk_plr_9mm_bullet_mp1","0"};
	CVAR_REGISTER( &sk_plr_9mm_bullet_mp2 );// {"sk_plr_9mm_bullet_mp2","0"};
	CVAR_REGISTER( &sk_plr_9mm_bullet_mp3 );// {"sk_plr_9mm_bullet_mp3","0"};

	// 357 Round
	CVAR_REGISTER ( &sk_plr_357_bullet1 );// {"sk_plr_357_bullet1","0"};
	CVAR_REGISTER ( &sk_plr_357_bullet2 );// {"sk_plr_357_bullet2","0"};
	CVAR_REGISTER ( &sk_plr_357_bullet3 );// {"sk_plr_357_bullet3","0"};

	CVAR_REGISTER( &sk_plr_357_bullet_mp1 );// {"sk_plr_357_bullet_mp1","0"};
	CVAR_REGISTER( &sk_plr_357_bullet_mp2 );// {"sk_plr_357_bullet_mp2","0"};
	CVAR_REGISTER( &sk_plr_357_bullet_mp3 );// {"sk_plr_357_bullet_mp3","0"};

	// MP5 Round
	CVAR_REGISTER ( &sk_plr_9mmAR_bullet1 );// {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER ( &sk_plr_9mmAR_bullet2 );// {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER ( &sk_plr_9mmAR_bullet3 );// {"sk_plr_9mmAR_bullet3","0"};

	CVAR_REGISTER( &sk_plr_9mmAR_bullet_mp1 );// {"sk_plr_9mmAR_bullet_mp1","0"};
	CVAR_REGISTER( &sk_plr_9mmAR_bullet_mp2 );// {"sk_plr_9mmAR_bullet_mp2","0"};
	CVAR_REGISTER( &sk_plr_9mmAR_bullet_mp3 );// {"sk_plr_9mmAR_bullet_mp3","0"};

	// M203 grenade
	CVAR_REGISTER ( &sk_plr_9mmAR_grenade1 );// {"sk_plr_9mmAR_grenade1","0"};
	CVAR_REGISTER ( &sk_plr_9mmAR_grenade2 );// {"sk_plr_9mmAR_grenade2","0"};
	CVAR_REGISTER ( &sk_plr_9mmAR_grenade3 );// {"sk_plr_9mmAR_grenade3","0"};

	CVAR_REGISTER( &sk_plr_9mmAR_grenade_mp1 );// {"sk_plr_9mmAR_grenade_mp1","0"};
	CVAR_REGISTER( &sk_plr_9mmAR_grenade_mp2 );// {"sk_plr_9mmAR_grenade_mp2","0"};
	CVAR_REGISTER( &sk_plr_9mmAR_grenade_mp3 );// {"sk_plr_9mmAR_grenade_mp3","0"};

	// Shotgun buckshot
	CVAR_REGISTER ( &sk_plr_buckshot1 );// {"sk_plr_buckshot1","0"};
	CVAR_REGISTER ( &sk_plr_buckshot2 );// {"sk_plr_buckshot2","0"};
	CVAR_REGISTER ( &sk_plr_buckshot3 );// {"sk_plr_buckshot3","0"};

	CVAR_REGISTER( &sk_plr_buckshot_mp1 );// {"sk_plr_buckshot_mp1","0"};
	CVAR_REGISTER( &sk_plr_buckshot_mp2 );// {"sk_plr_buckshot_mp2","0"};
	CVAR_REGISTER( &sk_plr_buckshot_mp3 );// {"sk_plr_buckshot_mp3","0"};

	// Crossbow
	CVAR_REGISTER ( &sk_plr_xbow_bolt_monster1 );// {"sk_plr_xbow_bolt1","0"};
	CVAR_REGISTER ( &sk_plr_xbow_bolt_monster2 );// {"sk_plr_xbow_bolt2","0"};
	CVAR_REGISTER ( &sk_plr_xbow_bolt_monster3 );// {"sk_plr_xbow_bolt3","0"};

	CVAR_REGISTER ( &sk_plr_xbow_bolt_client1 );// {"sk_plr_xbow_bolt1","0"};
	CVAR_REGISTER ( &sk_plr_xbow_bolt_client2 );// {"sk_plr_xbow_bolt2","0"};
	CVAR_REGISTER ( &sk_plr_xbow_bolt_client3 );// {"sk_plr_xbow_bolt3","0"};

	CVAR_REGISTER( &sk_plr_xbow_bolt_client_mp1 );// {"sk_plr_xbow_bolt_mp1","0"};
	CVAR_REGISTER( &sk_plr_xbow_bolt_client_mp2 );// {"sk_plr_xbow_bolt_mp2","0"};
	CVAR_REGISTER( &sk_plr_xbow_bolt_client_mp3 );// {"sk_plr_xbow_bolt_mp3","0"};

	// RPG
	CVAR_REGISTER ( &sk_plr_rpg1 );// {"sk_plr_rpg1","0"};
	CVAR_REGISTER ( &sk_plr_rpg2 );// {"sk_plr_rpg2","0"};
	CVAR_REGISTER ( &sk_plr_rpg3 );// {"sk_plr_rpg3","0"};

	CVAR_REGISTER( &sk_plr_rpg_mp1 );// {"sk_plr_rpg_mp1","0"};
	CVAR_REGISTER( &sk_plr_rpg_mp2 );// {"sk_plr_rpg_mp2","0"};
	CVAR_REGISTER( &sk_plr_rpg_mp3 );// {"sk_plr_rpg_mp3","0"};

	// Gauss Gun
	CVAR_REGISTER ( &sk_plr_gauss1 );// {"sk_plr_gauss1","0"};
	CVAR_REGISTER ( &sk_plr_gauss2 );// {"sk_plr_gauss2","0"};
	CVAR_REGISTER ( &sk_plr_gauss3 );// {"sk_plr_gauss3","0"};


	// Egon Gun
	CVAR_REGISTER ( &sk_plr_egon_narrow1 );// {"sk_plr_egon_narrow1","0"};
	CVAR_REGISTER ( &sk_plr_egon_narrow2 );// {"sk_plr_egon_narrow2","0"};
	CVAR_REGISTER ( &sk_plr_egon_narrow3 );// {"sk_plr_egon_narrow3","0"};

	CVAR_REGISTER( &sk_plr_egon_narrow_mp1 );// {"sk_plr_egon_narrow_mp1","0"};
	CVAR_REGISTER( &sk_plr_egon_narrow_mp2 );// {"sk_plr_egon_narrow_mp2","0"};
	CVAR_REGISTER( &sk_plr_egon_narrow_mp3 );// {"sk_plr_egon_narrow_mp3","0"};

	CVAR_REGISTER ( &sk_plr_egon_wide1 );// {"sk_plr_egon_wide1","0"};
	CVAR_REGISTER ( &sk_plr_egon_wide2 );// {"sk_plr_egon_wide2","0"};
	CVAR_REGISTER ( &sk_plr_egon_wide3 );// {"sk_plr_egon_wide3","0"};

	CVAR_REGISTER( &sk_plr_egon_wide_mp1 );// {"sk_plr_egon_wide_mp1","0"};
	CVAR_REGISTER( &sk_plr_egon_wide_mp2 );// {"sk_plr_egon_wide_mp2","0"};
	CVAR_REGISTER( &sk_plr_egon_wide_mp3 );// {"sk_plr_egon_wide_mp3","0"};

	// Hand Grendade
	CVAR_REGISTER ( &sk_plr_hand_grenade1 );// {"sk_plr_hand_grenade1","0"};
	CVAR_REGISTER ( &sk_plr_hand_grenade2 );// {"sk_plr_hand_grenade2","0"};
	CVAR_REGISTER ( &sk_plr_hand_grenade3 );// {"sk_plr_hand_grenade3","0"};

	CVAR_REGISTER( &sk_plr_hand_grenade_mp1 );// {"sk_plr_hand_grenade_mp1","0"};
	CVAR_REGISTER( &sk_plr_hand_grenade_mp2 );// {"sk_plr_hand_grenade_mp2","0"};
	CVAR_REGISTER( &sk_plr_hand_grenade_mp3 );// {"sk_plr_hand_grenade_mp3","0"};

	// Satchel Charge
	CVAR_REGISTER ( &sk_plr_satchel1 );// {"sk_plr_satchel1","0"};
	CVAR_REGISTER ( &sk_plr_satchel2 );// {"sk_plr_satchel2","0"};
	CVAR_REGISTER ( &sk_plr_satchel3 );// {"sk_plr_satchel3","0"};

	CVAR_REGISTER( &sk_plr_satchel_mp1 );// {"sk_plr_satchel_mp1","0"};
	CVAR_REGISTER( &sk_plr_satchel_mp2 );// {"sk_plr_satchel_mp2","0"};
	CVAR_REGISTER( &sk_plr_satchel_mp3 );// {"sk_plr_satchel_mp3","0"};

	// Tripmine
	CVAR_REGISTER ( &sk_plr_tripmine1 );// {"sk_plr_tripmine1","0"};
	CVAR_REGISTER ( &sk_plr_tripmine2 );// {"sk_plr_tripmine2","0"};
	CVAR_REGISTER ( &sk_plr_tripmine3 );// {"sk_plr_tripmine3","0"};

	CVAR_REGISTER( &sk_plr_tripmine_mp1 );// {"sk_plr_tripmine_mp1","0"};
	CVAR_REGISTER( &sk_plr_tripmine_mp2 );// {"sk_plr_tripmine_mp2","0"};
	CVAR_REGISTER( &sk_plr_tripmine_mp3 );// {"sk_plr_tripmine_mp3","0"};

#if USE_OPFOR
	// Knife whack
	CVAR_REGISTER( &sk_plr_knife1 );// {"sk_plr_knife1","0"};
	CVAR_REGISTER( &sk_plr_knife2 );// {"sk_plr_knife2","0"};
	CVAR_REGISTER( &sk_plr_knife3 );// {"sk_plr_knife3","0"};

	// Pipe wrench whack
	CVAR_REGISTER ( &sk_plr_pipewrench1 );// {"sk_plr_pipewrench1","0"};
	CVAR_REGISTER ( &sk_plr_pipewrench2 );// {"sk_plr_pipewrench2","0"};
	CVAR_REGISTER ( &sk_plr_pipewrench3 );// {"sk_plr_pipewrench3","0"};

	// Grapple
	CVAR_REGISTER( &sk_plr_grapple1 );// {"sk_plr_grapple1","0"};
	CVAR_REGISTER( &sk_plr_grapple2 );// {"sk_plr_grapple2","0"};
	CVAR_REGISTER( &sk_plr_grapple3 );// {"sk_plr_grapple3","0"};

	// 556 Round
	CVAR_REGISTER( &sk_plr_556_bullet1 );// {"sk_plr_556_bullet1","0"};
	CVAR_REGISTER( &sk_plr_556_bullet2 );// {"sk_plr_556_bullet2","0"};
	CVAR_REGISTER( &sk_plr_556_bullet3 );// {"sk_plr_556_bullet3","0"};

	// 762 Round
	CVAR_REGISTER( &sk_plr_762_bullet1 );// {"sk_plr_762_bullet1","0"};
	CVAR_REGISTER( &sk_plr_762_bullet2 );// {"sk_plr_762_bullet2","0"};
	CVAR_REGISTER( &sk_plr_762_bullet3 );// {"sk_plr_762_bullet3","0"};

	// Desert Eagle Round
	CVAR_REGISTER( &sk_plr_eagle1 );// {"sk_plr_eagle1","0"};
	CVAR_REGISTER( &sk_plr_eagle2 );// {"sk_plr_eagle2","0"};
	CVAR_REGISTER( &sk_plr_eagle3 );// {"sk_plr_eagle3","0"};

	// Shock Roach
	CVAR_REGISTER( &sk_plr_shockroachs1 );// {"sk_plr_shockroachs1","0"};
	CVAR_REGISTER( &sk_plr_shockroachs2 );// {"sk_plr_shockroachs2","0"};
	CVAR_REGISTER( &sk_plr_shockroachs3 );// {"sk_plr_shockroachs3","0"};

	CVAR_REGISTER( &sk_plr_shockroachm1 );// {"sk_plr_shockroachm1","0"};
	CVAR_REGISTER( &sk_plr_shockroachm2 );// {"sk_plr_shockroachm2","0"};
	CVAR_REGISTER( &sk_plr_shockroachm3 );// {"sk_plr_shockroachm3","0"};

	// Displacer Cannon
	CVAR_REGISTER( &sk_plr_displacer_other1 );// {"sk_plr_displacer_other1","0"};
	CVAR_REGISTER( &sk_plr_displacer_other2 );// {"sk_plr_displacer_other2","0"};
	CVAR_REGISTER( &sk_plr_displacer_other3 );// {"sk_plr_displacer_other3","0"};

	CVAR_REGISTER( &sk_plr_displacer_radius1 );// {"sk_plr_displacer_radius1","0"};
	CVAR_REGISTER( &sk_plr_displacer_radius2 );// {"sk_plr_displacer_radius2","0"};
	CVAR_REGISTER( &sk_plr_displacer_radius3 );// {"sk_plr_displacer_radius3","0"};

	// Spore
	CVAR_REGISTER( &sk_plr_spore1 );// {"sk_plr_spore1","0"};
	CVAR_REGISTER( &sk_plr_spore2 );// {"sk_plr_spore2","0"};
	CVAR_REGISTER( &sk_plr_spore3 );// {"sk_plr_spore3","0"};
#endif

	// WORLD WEAPONS
	CVAR_REGISTER ( &sk_12mm_bullet1 );// {"sk_12mm_bullet1","0"};
	CVAR_REGISTER ( &sk_12mm_bullet2 );// {"sk_12mm_bullet2","0"};
	CVAR_REGISTER ( &sk_12mm_bullet3 );// {"sk_12mm_bullet3","0"};

	CVAR_REGISTER ( &sk_9mmAR_bullet1 );// {"sk_9mm_bullet1","0"};
	CVAR_REGISTER ( &sk_9mmAR_bullet2 );// {"sk_9mm_bullet1","0"};
	CVAR_REGISTER ( &sk_9mmAR_bullet3 );// {"sk_9mm_bullet1","0"};

	CVAR_REGISTER ( &sk_9mm_bullet1 );// {"sk_9mm_bullet1","0"};
	CVAR_REGISTER ( &sk_9mm_bullet2 );// {"sk_9mm_bullet2","0"};
	CVAR_REGISTER ( &sk_9mm_bullet3 );// {"sk_9mm_bullet3","0"};


	// HORNET
	CVAR_REGISTER( &sk_plr_hornet_dmg1 );// {"sk_plr_hornet_dmg1","0"};
	CVAR_REGISTER( &sk_plr_hornet_dmg2 );// {"sk_plr_hornet_dmg2","0"};
	CVAR_REGISTER( &sk_plr_hornet_dmg3 );// {"sk_plr_hornet_dmg3","0"};

	CVAR_REGISTER( &sk_plr_hornet_dmg_mp1 );// {"sk_plr_hornet_dmg_mp1","0"};
	CVAR_REGISTER( &sk_plr_hornet_dmg_mp2 );// {"sk_plr_hornet_dmg_mp2","0"};
	CVAR_REGISTER( &sk_plr_hornet_dmg_mp3 );// {"sk_plr_hornet_dmg_mp3","0"};

	// HEALTH/SUIT CHARGE DISTRIBUTION
	CVAR_REGISTER ( &sk_suitcharger1 );
	CVAR_REGISTER ( &sk_suitcharger2 );
	CVAR_REGISTER ( &sk_suitcharger3 );

	CVAR_REGISTER( &sk_suitcharger_mp1 );
	CVAR_REGISTER( &sk_suitcharger_mp2 );
	CVAR_REGISTER( &sk_suitcharger_mp3 );

	CVAR_REGISTER ( &sk_battery1 );
	CVAR_REGISTER ( &sk_battery2 );
	CVAR_REGISTER ( &sk_battery3 );

	CVAR_REGISTER ( &sk_healthcharger1 );
	CVAR_REGISTER ( &sk_healthcharger2 );
	CVAR_REGISTER ( &sk_healthcharger3 );

	CVAR_REGISTER ( &sk_healthkit1 );
	CVAR_REGISTER ( &sk_healthkit2 );
	CVAR_REGISTER ( &sk_healthkit3 );

	CVAR_REGISTER ( &sk_scientist_heal1 );
	CVAR_REGISTER ( &sk_scientist_heal2 );
	CVAR_REGISTER ( &sk_scientist_heal3 );

// monster damage adjusters
	CVAR_REGISTER ( &sk_monster_head1 );
	CVAR_REGISTER ( &sk_monster_head2 );
	CVAR_REGISTER ( &sk_monster_head3 );

	CVAR_REGISTER ( &sk_monster_chest1 );
	CVAR_REGISTER ( &sk_monster_chest2 );
	CVAR_REGISTER ( &sk_monster_chest3 );

	CVAR_REGISTER ( &sk_monster_stomach1 );
	CVAR_REGISTER ( &sk_monster_stomach2 );
	CVAR_REGISTER ( &sk_monster_stomach3 );

	CVAR_REGISTER ( &sk_monster_arm1 );
	CVAR_REGISTER ( &sk_monster_arm2 );
	CVAR_REGISTER ( &sk_monster_arm3 );

	CVAR_REGISTER ( &sk_monster_leg1 );
	CVAR_REGISTER ( &sk_monster_leg2 );
	CVAR_REGISTER ( &sk_monster_leg3 );

// player damage adjusters
	CVAR_REGISTER ( &sk_player_head1 );
	CVAR_REGISTER ( &sk_player_head2 );
	CVAR_REGISTER ( &sk_player_head3 );

	CVAR_REGISTER ( &sk_player_chest1 );
	CVAR_REGISTER ( &sk_player_chest2 );
	CVAR_REGISTER ( &sk_player_chest3 );

	CVAR_REGISTER ( &sk_player_stomach1 );
	CVAR_REGISTER ( &sk_player_stomach2 );
	CVAR_REGISTER ( &sk_player_stomach3 );

	CVAR_REGISTER ( &sk_player_arm1 );
	CVAR_REGISTER ( &sk_player_arm2 );
	CVAR_REGISTER ( &sk_player_arm3 );

	CVAR_REGISTER ( &sk_player_leg1 );
	CVAR_REGISTER ( &sk_player_leg2 );
	CVAR_REGISTER ( &sk_player_leg3 );
// END REGISTER CVARS FOR SKILL LEVEL STUFF

	//Link user messages now.
	LinkUserMessages();

	SERVER_COMMAND( "exec skill.cfg\n" );

	if( !g_Server.Initialize() )
	{
		ShutdownGame();
	}
}

void GameDLLShutdown()
{
	g_Server.Shutdown();
}