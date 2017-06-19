// #################################
// HU3-LIFE trigger_weapon_condition
// #################################
// O objetivo desse trigger eh ativar uma primeira entidade caso o jogador possua uma arma ou uma segunda caso nao ele possua.
// Contudo, para que o codigo seja compativel com o modo coop eu estou verificando o estado de todos os jogadores de uma vez,
// fazendo que qualquer um que esteja armado force o trigger a chamar a primeira entidade.

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBaseTrigger.h"
#include "CBasePlayer.h"

#include "CTriggerWeaponCondition.h"

BEGIN_DATADESC( CWeaponCondition )
	DEFINE_FIELD(m_TargetIfPlyHasWpn, FIELD_STRING),
	DEFINE_FIELD(m_TargetIfPlyDoesntHaveWpn, FIELD_STRING),
END_DATADESC()

LINK_ENTITY_TO_CLASS(trigger_weapon_condition, CWeaponCondition);

void CWeaponCondition::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "targetifplyhaswpn"))
	{
		m_TargetIfPlyHasWpn = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	if (FStrEq(pkvd->szKeyName, "targetifplydoesnthavewpn"))
	{
		m_TargetIfPlyDoesntHaveWpn = ALLOC_STRING(pkvd->szValue);
			pkvd->fHandled = true;
	}
	else
		CBaseTrigger::KeyValue(pkvd);
}

void CWeaponCondition::Touch(CBaseEntity *pOther)
{
	// So clientes ativam esse trigger
	if (!pOther->IsPlayer())
		return;

	WeaponConditional();
}

void CWeaponCondition::Spawn(void)
{
	InitTrigger();
}

void CWeaponCondition::Use(CBaseEntity pActivator, CBaseEntity pCaller, USE_TYPE useType, float value)
{
	WeaponConditional();
}

void CWeaponCondition::WeaponConditional()
{
	// Para verificar se um jogador possue alguma arma eu estou checando se o campo "pev->weaponmodel" esta configurado
	// Ele fica vazio se o jogador ainda nao pegou nenhuma arma ou fica com o nome do worldmodel da ultima arma coletada

	int plyHasWeapon = 0, plyQuant = 0, i = 1;
	CBaseEntity* hu3Player = nullptr;

	// Primeiro eu pego o numero de jogadores, no minimo tem 1 (funciona no singleplayer)
	while ((hu3Player = UTIL_FindEntityByClassname(hu3Player, "player")) != nullptr)
		plyQuant++;

	// Agora preciso rodar por todos os jogadores e ver se algum possui um nome de worldmodel de arma registrado
	while (hu3Player = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(i)))
	{
		// Se o campo do worldmodel tiver tamanho 0, nenhuma arma foi coletada ainda, caso contrario temos um jogador armado
		if (strlen(hu3Player->GetWeaponModelName()) > 0)
		{
			plyHasWeapon = 1;
			break;
		}

		// Se o jogador atual estiver desarmado, vamos verificar os outros
		i++;
		if ( i > plyQuant )
			break;
	}

	// Momento principal: se algum jogador estiver armado, chama a primeira entidade, caso contrario, chama a segunda
	if (plyHasWeapon)
	{
		// Primeira entidade
		if (strcmp(STRING(m_TargetIfPlyHasWpn), "") != 0)
			FireTargets(STRING(m_TargetIfPlyHasWpn), m_hActivator, this, USE_TOGGLE, 0);
	}
	else
	{
		// Segunda entidade
		if (strcmp(STRING(m_TargetIfPlyDoesntHaveWpn), "") != 0)
			FireTargets(STRING(m_TargetIfPlyDoesntHaveWpn), m_hActivator, this, USE_TOGGLE, 0);
	}

	// Removemos o "trigger_weapon_condition" para terminar
	SetTouch(NULL);
	UTIL_Remove(this);
}
