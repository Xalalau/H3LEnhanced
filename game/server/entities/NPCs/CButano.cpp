#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Monsters.h"
#include "Schedule.h"
#include "CZombie.h"

#include "CButano.h"

LINK_ENTITY_TO_CLASS( monster_butano, CButano );

//=========================================================
// Spawn
//=========================================================
void CButano :: Spawn()
{
	Precache( );

	SetModel( "models/zombie.mdl");
	SetSize( VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_GREEN;
	pev->health			= gSkillData.GetZombieHealth();
	pev->view_ofs		= VEC_VIEW;// position of the eyes relative to monster's origin.
	m_flFieldOfView		= -1.0;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_HUNT;
	m_afCapability		= bits_CAP_DOORS_GROUP;

	SetNewSpawn();

	MonsterInit();
}

// Busca pontos de spawn (info_targets) e escolhe um (aleatoriamente)
// Essa funcao ja eh preparada para o modo COOP
void CButano::SetNewSpawn()
{
	CBaseEntity* pSpawnPoint = nullptr;
	CBaseEntity* hu3Player = nullptr;
	int plyQuant = 0, targetsQuant = 0;
	Vector InfoPositions[100], dist;

	// Numero de jogadores
	while ((hu3Player = UTIL_FindEntityByClassname(hu3Player, "player")) != nullptr)
		plyQuant++;

	// Escolher um jogador aleatoriamente
	hu3Player = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(RANDOM_LONG(1, plyQuant)));

	// Numero de info_target validos e suas posicoes
	while ((pSpawnPoint = UTIL_FindEntityByClassname(pSpawnPoint, "info_target")) != nullptr)
	{
		dist = pSpawnPoint->GetAbsOrigin() - hu3Player->GetAbsOrigin();

		//ALERT(at_console, "############### TAMANHO: %f!\n", pos.Length());

		if (dist.Length() > 100 && dist.Length() < 500)
		{
			InfoPositions[targetsQuant] = pSpawnPoint->GetAbsOrigin();
			targetsQuant++;
		}
	}

	// Spawno o NPC aleatoriamente em uma posicao valida e proximo ao player escolhido
	if (targetsQuant > 0)
		pev->origin = InfoPositions[RANDOM_LONG(0, targetsQuant)];
}