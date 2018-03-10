// ##############################
// HU3-LIFE trigger_cmd
// ##############################
/*
Entidade que executa comandos de console nos seguintes alvos:

1) "clients" = em cada jogador;
2) "server" = no servidor;
3) "activator" = no jogador que ativou o trigger;
4) "random client" = em um jogador qualquer.

Ela eh ativada apenas pelo contato de jogadores.
Ela So funciona uma unica vez.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBaseTrigger.h"

#include "CTriggerCMD.h"

LINK_ENTITY_TO_CLASS(trigger_cmd, CTriggerCMD);

void CTriggerCMD::KeyValue(KeyValueData *pkvd)
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
		CBaseTrigger::KeyValue(pkvd);
}

void CTriggerCMD::Spawn(void)
{
	// Precisa dos argumentos
	if ((m_command == NULL) || (m_target == NULL))
		ALERT(at_console, "Estao faltando argumentos num trigger_cmd... Ele nao funcionara!\n");

	InitTrigger();
}

void CTriggerCMD::Touch(CBaseEntity *pOther)
{
	// Ativamento apenas por jogadores
	if (!pOther->IsPlayer())
		return;

	// Precisa dos argumentos
	if ((m_command == NULL) || (m_target == NULL))
	{
		ALERT(at_console, "Estao faltando argumentos num trigger_cmd... Ignorando-o!\n");
		return;
	}

	// Rodar comando uma unica vez no servidor
	if (strcmp(STRING(m_target), "server") == 0)
	{
		SERVER_COMMAND(UTIL_VarArgs("%s\n", STRING(m_command)));
	}
	// Rodar comando em cada jogador
	else if (strcmp(STRING(m_target), "clients") == 0)
	{
		edict_t *hu3Player;
		int i = 1;

		while ((hu3Player = g_engfuncs.pfnPEntityOfEntIndex(i)) != nullptr)
		{
			CLIENT_COMMAND(hu3Player, "%s\n", STRING(m_command));
			i++;
		}
	}
	// Rodar comando no jogador que ativou o trigger
	else if (strcmp(STRING(m_target), "activator") == 0)
	{
		CLIENT_COMMAND(g_engfuncs.pfnPEntityOfEntIndex(pOther->entindex()), "%s\n", STRING(m_command));
	}
	// Rodar comando em algum jogador qualquer
	else if (strcmp(STRING(m_target), "random client") == 0)
	{
		int i = 1;

		while (g_engfuncs.pfnPEntityOfEntIndex(i) != nullptr)
			i++;

		CLIENT_COMMAND(g_engfuncs.pfnPEntityOfEntIndex(RANDOM_LONG(1, i)), "%s\n", STRING(m_command));
	}

	// Remover a entidade
	UTIL_Remove(this);
}