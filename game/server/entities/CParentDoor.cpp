// ##############################
// HU3-LIFE ENTIDADE ARRASTADORA
// ##############################
// Arrasta solids e point_ents ao se mover.
// Nao funciona com coisas animadas (elas nao param) (TODO - Ajeitar isso)

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBaseDoor.h"
#include "SaveRestore.h"
#include "entities/DoorConstants.h"

#include "CParentDoor.h"

BEGIN_DATADESC(CParentDoor)
	DEFINE_FIELD(m_bHealthValue, FIELD_CHARACTER),
	DEFINE_FIELD(m_bMoveSnd, FIELD_CHARACTER),
	DEFINE_FIELD(m_bStopSnd, FIELD_CHARACTER),

	DEFINE_FIELD(m_bLockedSound, FIELD_CHARACTER),
	DEFINE_FIELD(m_bLockedSentence, FIELD_CHARACTER),
	DEFINE_FIELD(m_bUnlockedSound, FIELD_CHARACTER),
	DEFINE_FIELD(m_bUnlockedSentence, FIELD_CHARACTER),

	DEFINE_FIELD(speed, FIELD_FLOAT),

	DEFINE_FIELD(door, FIELD_EVARS),
	DEFINE_FIELD(destination, FIELD_VECTOR),

	DEFINE_FIELD(parent01_name, FIELD_STRING),
	DEFINE_FIELD(parent01, FIELD_EVARS),
	DEFINE_FIELD(parent02_name, FIELD_STRING),
	DEFINE_FIELD(parent02, FIELD_EVARS),
	DEFINE_FIELD(parent03_name, FIELD_STRING),
	DEFINE_FIELD(parent03, FIELD_EVARS),
	DEFINE_FIELD(parent04_name, FIELD_STRING),
	DEFINE_FIELD(parent04, FIELD_EVARS),
	DEFINE_FIELD(parent05_name, FIELD_STRING),
	DEFINE_FIELD(parent05, FIELD_EVARS),
	DEFINE_FIELD(parent06_name, FIELD_STRING),
	DEFINE_FIELD(parent06, FIELD_EVARS),
	DEFINE_FIELD(parent07_name, FIELD_STRING),
	DEFINE_FIELD(parent07, FIELD_EVARS),
	DEFINE_FIELD(parent08_name, FIELD_STRING),
	DEFINE_FIELD(parent08, FIELD_EVARS),
	DEFINE_FIELD(parent09_name, FIELD_STRING),
	DEFINE_FIELD(parent09, FIELD_EVARS),
	DEFINE_FIELD(parent10_name, FIELD_STRING),
	DEFINE_FIELD(parent10, FIELD_EVARS),
	DEFINE_FIELD(parent11_name, FIELD_STRING),
	DEFINE_FIELD(parent11, FIELD_EVARS),
	DEFINE_FIELD(parent12_name, FIELD_STRING),
	DEFINE_FIELD(parent12, FIELD_EVARS),
	DEFINE_FIELD(parent13_name, FIELD_STRING),
	DEFINE_FIELD(parent13, FIELD_EVARS),
	DEFINE_FIELD(parent14_name, FIELD_STRING),
	DEFINE_FIELD(parent14, FIELD_EVARS),
	DEFINE_FIELD(parent15_name, FIELD_STRING),
	DEFINE_FIELD(parent15, FIELD_EVARS),
	DEFINE_FIELD(parent16_name, FIELD_STRING),
	DEFINE_FIELD(parent16, FIELD_EVARS),
END_DATADESC()

LINK_ENTITY_TO_CLASS(func_parent, CParentDoor);
//
// Cache user-entity-field values until spawn is called.
//

void CParentDoor::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "skin"))//skin is used for content type
	{
		pev->skin = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "movesnd"))
	{
		m_bMoveSnd = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "stopsnd"))
	{
		m_bStopSnd = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "healthvalue"))
	{
		m_bHealthValue = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "locked_sound"))
	{
		m_bLockedSound = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "locked_sentence"))
	{
		m_bLockedSentence = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "unlocked_sound"))
	{
		m_bUnlockedSound = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "unlocked_sentence"))
	{
		m_bUnlockedSentence = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "WaveHeight"))
	{
		pev->scale = atof(pkvd->szValue) * (1.0 / 8.0);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent01"))
	{
		parent01_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent02"))
	{
		parent02_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent03"))
	{
		parent03_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent04"))
	{
		parent04_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent05"))
	{
		parent05_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent06"))
	{
		parent06_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent07"))
	{
		parent07_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent08"))
	{
		parent08_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent09"))
	{
		parent09_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent10"))
	{
		parent10_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent11"))
	{
		parent11_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent12"))
	{
		parent12_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent13"))
	{
		parent13_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent14"))
	{
		parent14_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent15"))
	{
		parent15_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "parent16"))
	{
		parent16_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CBaseToggle::KeyValue(pkvd);
}

void CParentDoor::Spawn()
{
	Precache();

	// Modifico propriedades da entidade para que ela se mova
	PrepareModel();

	// Defino os angulos do movimento
	SetMovedir(this);

	// Defino a velocidade
	speed = pev->speed;

	if (pev->speed == 0)
		pev->speed = 100;
	if (pev->dmg == 0)
		pev->dmg = 2;

	// Salvo a entidade principal pev em door
	door = pev;

	// Calculo o vetor posicao e saldo em destination
	m_vecPosition1 = GetAbsOrigin();
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2 = m_vecPosition1 + (pev->movedir * (fabs(pev->movedir.x * (pev->size.x - 2)) + fabs(pev->movedir.y * (pev->size.y - 2)) + fabs(pev->movedir.z * (pev->size.z - 2)) - m_flLip));
	ASSERTSZ(m_vecPosition1 != m_vecPosition2, "door start/end positions are equal");
	destination = m_vecPosition2;
}

//
// Used by SUB_UseTargets, when a door is the target of a button.
//
void CParentDoor::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Movimento do func_parent
	LinearMove(destination, speed);
	// Movimento dos parents
	ProcessMovement(parent01, parent01_name);
	ProcessMovement(parent02, parent02_name);
	ProcessMovement(parent03, parent03_name);
	ProcessMovement(parent04, parent04_name);
	ProcessMovement(parent05, parent05_name);
	ProcessMovement(parent06, parent06_name);
	ProcessMovement(parent07, parent07_name);
	ProcessMovement(parent08, parent08_name);
	ProcessMovement(parent09, parent09_name);
	ProcessMovement(parent10, parent10_name);
	ProcessMovement(parent11, parent11_name);
	ProcessMovement(parent12, parent12_name);
	ProcessMovement(parent13, parent13_name);
	ProcessMovement(parent14, parent14_name);
	ProcessMovement(parent15, parent15_name);
	ProcessMovement(parent16, parent16_name);
	// Reassociacao do pev original
	pev = door;
}

// Modifica propriedades da entidade para que possa se mover
void CParentDoor::PrepareModel()
{
	if (pev->skin == 0)
	{//normal door
		if (FBitSet(pev->spawnflags, SF_DOOR_PASSABLE))
			pev->solid = SOLID_NOT;
		else
			pev->solid = SOLID_BSP;
	}
	else
	{// special contents
		pev->solid = SOLID_NOT;
		SetBits(pev->spawnflags, SF_DOOR_SILENT);	// water is silent for now
	}

	pev->movetype = MOVETYPE_PUSH;
	SetAbsOrigin(GetAbsOrigin());
	SetModel(STRING(pev->model));
}

// Faz as entidades se moverem
void CParentDoor::ProcessMovement(entvars_t *parent, string_t targetName)
{
	if ((pev = parent) == NULL)
	{
		pev = parent = SetEntVars_t(targetName);
		if (parent != NULL)
		{
			PrepareModel();
			LinearMove(destination, speed);
		}
	}
	else
		LinearMove(destination, speed);
}

// Encontra entidades no mapa
entvars_t * CParentDoor::SetEntVars_t(string_t targetName)
{
	edict_t* pentTarget = NULL;
	pentTarget = FIND_ENTITY_BY_STRING(pentTarget, "targetname", STRING(targetName));
	if (FNullEnt(pentTarget))
		return NULL;
	return &pentTarget->v;
}