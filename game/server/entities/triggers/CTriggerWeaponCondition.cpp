// #################################
// HU3-LIFE trigger_weapon_condition
// #################################
// Trigger que detecta a situacao de armamento do jogador e chama outras entidades para reagir a essa situacao

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

// Essa funcao foi feita para funcionar no modo coop!
void CWeaponCondition::WeaponConditional()
{
	int plyHasWeapon = 0, plyQuant = 0, i = 1;
	CBaseEntity* hu3Player = nullptr;

	// Numero de jogadores
	while ((hu3Player = UTIL_FindEntityByClassname(hu3Player, "player")) != nullptr)
		plyQuant++;

	// Vamos rodar todos os jogadores e ver se alguem possui arma. Se sim, ela tem que estar ativa e tem que ter um modelo de terceira pessoa
	while (hu3Player = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(i)))
	{
		// Existe alguma arma para esse slot, no geral?
		if (hu3Player->GetWeaponModelName())
		{
			// Essa arma esta ativa no jogador? Possui nome configurado?
			if (strlen(hu3Player->GetWeaponModelName()) > 0)
			{
				plyHasWeapon = 1;
				break;
			}
		}

		// Verificar proximo jogador...
		i++;
		if ( i > plyQuant )
			break;
	}

	// Se algum jogador tiver arma, chama uma entidade, caso contrario, chama outra
	if (plyHasWeapon)
	{
		if (strcmp(STRING(m_TargetIfPlyHasWpn), "") != 0)
			FireTargets(STRING(m_TargetIfPlyHasWpn), m_hActivator, this, USE_TOGGLE, 0);
	}
	else
	{
		if (strcmp(STRING(m_TargetIfPlyDoesntHaveWpn), "") != 0)
			FireTargets(STRING(m_TargetIfPlyDoesntHaveWpn), m_hActivator, this, USE_TOGGLE, 0);
	}

	SetTouch(NULL);
	UTIL_Remove(this);
}
