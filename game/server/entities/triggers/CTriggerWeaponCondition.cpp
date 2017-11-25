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

#include "CTriggerWeaponCondition.h"

BEGIN_DATADESC( CWeaponCondition )
	DEFINE_FIELD(m_TargetIfSomePlyHasWpnAndAmmo, FIELD_STRING),
	DEFINE_FIELD(m_TargetIfSomePlyHasWpn, FIELD_STRING),
	DEFINE_FIELD(m_TargetIfPlysAreDisarmed, FIELD_STRING),
END_DATADESC()

LINK_ENTITY_TO_CLASS(trigger_weapon_condition, CWeaponCondition);

void CWeaponCondition::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "targetifsomeplyhaswpnandammo"))
	{
		m_TargetIfSomePlyHasWpnAndAmmo = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "targetifsomeplyhaswpn"))
	{
		m_TargetIfSomePlyHasWpn = ALLOC_STRING(pkvd->szValue);
			pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "targetifplysaredisarmed"))
	{
		m_TargetIfPlysAreDisarmed = ALLOC_STRING(pkvd->szValue);
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
	CBaseEntity* hu3Player = nullptr;
	CBasePlayer* hu3PlayerCBP = nullptr;
	string_t entity;

	// Vou rodar por todos os jogadores e ver se algum possui arma e municao
	while ((hu3Player = UTIL_FindEntityByClassname(hu3Player, "player")) != nullptr)
	{
		// Pego o jogador na classe CBasePlayer
		hu3PlayerCBP = (CBasePlayer *)hu3Player;

		// Verifico se o jogador esta armado
		if (hu3PlayerCBP->HasWeapons())
		{
			// Ativamos a variavel plyHasWeapon:
			plyHasWeapon = 1;

			// Entao vamos verificar se esse jogador eh perigoso: ele possui municao?
			if (hu3PlayerCBP->HasAnyAmmo())
			{
				// Ativamos a variavel plyHasAmmo:
				plyHasAmmo = 1;

				// Nao temos mais necessidade de checar mais jogadores, ja alcancamos a ativacao maxima
				break;
			}
		}
	}

	// Agora vou chamar a outra entidade de acordo com o que verificamos na partida:

	// Se existirem jogadores armados e com municao, pego a primeira entidade
	if (plyHasWeapon && plyHasAmmo)
		entity = m_TargetIfSomePlyHasWpnAndAmmo;
	// Se existirem jogadores armados mas todos sem municao, pego a segunda entidade
	else if (plyHasWeapon)
		entity = m_TargetIfSomePlyHasWpn;
	// Se todos os jogadores estiverem desarmados, pego a terceira entidade
	else
		entity = m_TargetIfPlysAreDisarmed;

	// Chamo a entidade selecionada
	if (strcmp(STRING(entity), "") != 0)
		FireTargets(STRING(entity), m_hActivator, this, USE_TOGGLE, 0);

	// E para finalizar, removemos o "trigger_weapon_condition" do mapa
	SetTouch(NULL);
	UTIL_Remove(this);
}