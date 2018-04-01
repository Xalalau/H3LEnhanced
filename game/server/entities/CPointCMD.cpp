// ##############################
// HU3-LIFE point_cmd
// ##############################
/*
Entidade que executa comandos de console nos seguintes alvos:

1) "clients" = em cada jogador;
2) "server" = no servidor;
4) "random client" = em um jogador qualquer.

Ela eh ativada apenas por chamada de outras entidades (via target).
Ela So funciona uma unica vez.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CPointEntity.h"

#include "CPointCMD.h"

LINK_ENTITY_TO_CLASS(point_cmd, CPointCMD);

void CPointCMD::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "command")) // Comando
	{
		m_command = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "hu3target")) // Alvo. Obs: hu3target porque target ja esta em uso pelo jogo
	{
		m_target = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

void CPointCMD::Spawn(void)
{
	// Precisa dos argumentos
	if ((m_command == NULL) || (m_target == NULL))
		ALERT(at_console, "Estao faltando argumentos num point_cmd... Ele nao funcionara!\n");

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;

	if (pev->scale > 0)
		pev->nextthink = gpGlobals->time + 1.0;
}

void CPointCMD::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Precisa dos argumentos
	if ((m_command == NULL) || (m_target == NULL))
	{
		ALERT(at_console, "Estao faltando argumentos num trigger_cmd... Ignorando-o!\n");
		return;
	}

	ALERT(at_console, "MODO ESCOLHIDO: %s\n", STRING(m_target));

	// Rodar comando uma unica vez no servidor
	if (strcmp(STRING(m_target), "server") == 0)
	{
		SERVER_COMMAND(UTIL_VarArgs("%s\n", STRING(m_command)));
	}
	// Rodar comando em cada jogador
	else if (strcmp(STRING(m_target), "clients") == 0)
	{
		ALERT(at_console, "MODO CLIENTS\n");
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			edict_t *hu3Player = g_engfuncs.pfnPEntityOfEntIndex(i);
			if (hu3Player)
			{
				CBaseEntity *pEnt = CBaseEntity::Instance(hu3Player);
				if (pEnt && pEnt->IsPlayer())
					CLIENT_COMMAND(hu3Player, "%s\n", STRING(m_command));
			}
		}
	}
	// Rodar comando em algum jogador qualquer
	else if (strcmp(STRING(m_target), "randomclient") == 0)
	{
		edict_t *hu3Player = g_engfuncs.pfnPEntityOfEntIndex(RANDOM_LONG(1, gpGlobals->maxClients));
		if (hu3Player)
		{
			ALERT(at_console, "ENTIDADE ESCOLHIDA = %s\n", STRING(hu3Player));
			CBaseEntity *pEnt = CBaseEntity::Instance(hu3Player);
			if (pEnt && pEnt->IsPlayer())
				CLIENT_COMMAND(hu3Player, "%s\n", STRING(m_command));
		}
	}

	// Remover a entidade
	UTIL_Remove(this);
}