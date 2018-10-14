// #################################
// HU3-LIFE trigger_weapon_condition
// #################################
// O objetivo desse trigger eh ativar uma primeira entidade caso o jogador possua uma arma e esteja com alguma municao ou uma
// segunda caso ele possua uma arma mas esteja totalmente sem municao ou ainda uma terceira caso ele nao possua nenhuma arma.

// Mas note que visando a compatibilidade com o modo coop nos estamos verificando o estado de todos os jogadores de uma vez,
// de modo que as entidades sao selecionadas de acordo com a avaliacao geral dos players. Por exemplo, basta um ter armas e
// municao para que a primeira entidade seja chamada.

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBaseTrigger.h"
#include "CBasePlayer.h"
#include "Weapons.h"
#include "gamerules/GameRules.h"

#include "CTriggerWeaponCondition.h"

BEGIN_DATADESC( CWeaponCondition )
	DEFINE_FIELD(m_TargetHasWpnAndAmmo, FIELD_STRING),
	DEFINE_FIELD(m_TargetHasWpn, FIELD_STRING),
	DEFINE_FIELD(m_TargetDisarmed, FIELD_STRING),
END_DATADESC()

LINK_ENTITY_TO_CLASS(trigger_weapon_condition, CWeaponCondition);

void CWeaponCondition::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "haswpnandammo"))
	{
		m_TargetHasWpnAndAmmo = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "haswpn"))
	{
		m_TargetHasWpn = ALLOC_STRING(pkvd->szValue);
			pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "isdisarmed"))
	{
		m_TargetDisarmed = ALLOC_STRING(pkvd->szValue);
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

	ProcessConditions();
}

void CWeaponCondition::Spawn(void)
{
	InitTrigger();
}

void CWeaponCondition::Use(CBaseEntity pActivator, CBaseEntity pCaller, USE_TYPE useType, float value)
{
	ProcessConditions();
}

// Verifica todos os jogadores e chama outra entidade de acordo com as condicoes
void CWeaponCondition::ProcessConditions()
{
	int plyHasWeapon = 0, plyHasAmmo = 0;
	string_t entity;

	// Vou rodar por todos os jogadores e ver se algum possui arma e municao
	for (int i = 1; i < gpGlobals->maxClients; i++)
	{
		CBasePlayer *hu3Player = UTIL_PlayerByIndex(i);

		if (hu3Player)
		{
			// Se estiver em multiplayer, verifico se o jogador está online
			if (g_pGameRules && g_pGameRules->IsMultiplayer())
				if ( ! hu3Player->IsConnected())
					return;		

			// Verifico se o jogador esta armado
			if (hu3Player->HasWeapons())
			{
				// Ativamos a variavel plyHasWeapon:
				plyHasWeapon = 1;

				// Entao vamos verificar se esse jogador eh perigoso: ele possui municao?
				if (hu3Player->HasAnyAmmo())
				{
					// Ativamos a variavel plyHasAmmo:
					plyHasAmmo = 1;

					// Nao temos mais necessidade de checar mais jogadores, ja alcancamos a ativacao maxima
					break;
				}
			}
		}
	}

	// Agora vou chamar a outra entidade de acordo com o que verificamos na partida:

	// Se existirem jogadores armados e com municao, pego a primeira entidade
	if (plyHasWeapon && plyHasAmmo)
		entity = m_TargetHasWpnAndAmmo;
	// Se existirem jogadores armados mas todos sem municao, pego a segunda entidade
	else if (plyHasWeapon)
		entity = m_TargetHasWpn;
	// Se todos os jogadores estiverem desarmados, pego a terceira entidade
	else
		entity = m_TargetDisarmed;

	// Chamo a entidade selecionada
	if (strcmp(STRING(entity), "") != 0)
		FireTargets(STRING(entity), m_hActivator, this, USE_TOGGLE, 0);

	// E para finalizar, removemos o "trigger_weapon_condition" do mapa
	SetTouch(NULL);
	UTIL_Remove(this);
}