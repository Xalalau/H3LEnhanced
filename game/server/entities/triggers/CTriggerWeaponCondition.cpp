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

	ProcessConditions(pOther);
}

void CWeaponCondition::Spawn(void)
{
	InitTrigger();
}

void CWeaponCondition::Use(CBaseEntity pActivator, CBaseEntity pCaller, USE_TYPE useType, float value)
{
	// NOTA: nao tenho certeza se estou fazendo a chama direito aqui, tem que testar essa ativacao!
	ProcessConditions(&pActivator);
}

void CWeaponCondition::ProcessConditions(CBaseEntity *pOther)
{
	// Verifica todos os jogadores e chama outra entidade de acordo com as condicoes

	// Para checar rapidamente se um jogador possue alguma arma eu estou vendo se o campo "pev->weaponmodel" esta configurado
	// Ele fica vazio se o jogador ainda nao pegou nenhuma arma ou fica com o nome do worldmodel da ultima arma coletada

	int plyHasWeapon = 0, plyHasAmmo = 0, plyQuant = 0, i = 1;
	CBaseEntity* hu3Player = nullptr;

	// Primeiro eu pego o numero de jogadores, no minimo tem 1 (isso tambem funciona no singleplayer)
	while ((hu3Player = UTIL_FindEntityByClassname(hu3Player, "player")) != nullptr)
		plyQuant++;

	// Agora preciso rodar por todos os jogadores e ver se algum possui um nome de worldmodel de arma registrado
	while (hu3Player = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(i)))
	{
		// Se o campo do worldmodel tiver tamanho 0, nenhuma arma ainda foi coletada, caso contrario temos um jogador armado
		if (strlen(hu3Player->GetWeaponModelName()) > 0)
		{
			// Ativamos a variavel plyHasWeapon:
			plyHasWeapon = 1;

			// Entao vamos verificar se esse jogador eh perigoso: ele possui municao?
			if (CheckAmmoQuantity(pOther) == 1)
			{
				// Ativamos a variavel plyHasAmmo:
				plyHasAmmo = 1;
				break;
			}
		}

		// Se o jogador atual for pelo menos inofensivo (nao tiver municao), ainda temos que verificar os outros...
		i++;
		if ( i > plyQuant )
			break;
	}

	// Hora de chamar a outra entidade de acordo com o que verificamos na partida:
	CallTheOtherEntity(plyHasWeapon, plyHasAmmo);

	// Para finalizar, removemos o "trigger_weapon_condition" do mapa
	SetTouch(NULL);
	UTIL_Remove(this);
}

int CWeaponCondition::CheckAmmoQuantity(CBaseEntity *pOther)
{
	// Checa se o jogador possui alguma municao guardada
	// Retorna 1 caso sim ou 0 caso nao

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;
	CBasePlayerWeapon *pWeapon;
	int i, j = 0;

	// Vamos ver todas as armas possiveis desse player agora...
	for (i = 0; i < MAX_WEAPON_SLOTS; i++)
	{
		// Tenta acessar a particao atual de slots de armas
		pWeapon = pPlayer->m_rgpPlayerItems[i];

		// Se existir alguma primeira arma, vamos checar se ha municao nela...
		while (pWeapon)
		{
			// Quantidade da municao 1 em uso
			j = pWeapon->m_iClip;

			// Quantidade da municao 1 guardada
			if (pWeapon->pszAmmo1())
				j = j + (*pPlayer).m_rgAmmo[pPlayer->GetAmmoIndex(pWeapon->pszAmmo1())];

			// Quantidade da municao 2 guardada
			if (pWeapon->pszAmmo2())
				j = j + (*pPlayer).m_rgAmmo[pPlayer->GetAmmoIndex(pWeapon->pszAmmo2())];

			// Print maroto de testes
			//ALERT(at_console, "################# Muni_ativa: %d / Muni_A: %d / Muni_B: %d\n", pWeapon->m_iClip, (*pPlayer).m_rgAmmo[pPlayer->GetAmmoIndex(pWeapon->pszAmmo1())], (*pPlayer).m_rgAmmo[pPlayer->GetAmmoIndex(pWeapon->pszAmmo2())]);

			// Se houver alguma municao disponivel, retornamos com a resposta "1"
			if (j > 0)
				return 1;

			// Se nao houver municao aqui, vamos testar as proximas armas dessa particao de slots...
			pWeapon = pWeapon->m_pNext;
		}
	}

	// Nenhuma municao foi encontrada nesse jogador, portanto retornamos um "0"
	return 0;
}

void CWeaponCondition::CallTheOtherEntity(int plyHasWeapon, int plyHasAmmo)
{
	// Hora H: vamos chamar a entidade correta para a nossa partida

	// Se qualquer jogador estiver armado e com municao:
	if (plyHasWeapon && plyHasAmmo)
	{
		// Chamo a primeira entidade
		if (strcmp(STRING(m_TargetIfSomePlyHasWpnAndAmmo), "") != 0)
			FireTargets(STRING(m_TargetIfSomePlyHasWpnAndAmmo), m_hActivator, this, USE_TOGGLE, 0);
	}
	// Se qualquer jogador estiver armado mas todos estiverem sem municao:
	else if (plyHasWeapon)
	{
		// Chamo a segunda entidade
		if (strcmp(STRING(m_TargetIfSomePlyHasWpn), "") != 0)
			FireTargets(STRING(m_TargetIfSomePlyHasWpn), m_hActivator, this, USE_TOGGLE, 0);
	}
	// Se todos os jogadores estiverem desarmados:
	else
	{
		// Chamo a terceira entidade
		if (strcmp(STRING(m_TargetIfPlysAreDisarmed), "") != 0)
			FireTargets(STRING(m_TargetIfPlysAreDisarmed), m_hActivator, this, USE_TOGGLE, 0);
	}
}