// ##############################
// HU3-LIFE ENTIDADE ARRASTADORA
// ##############################
// Arrasta outras entidades consigo ao se mover.

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBaseDoor.h"
#include "SaveRestore.h"
#include "entities/DoorConstants.h"

#include "CFuncParent.h"

BEGIN_DATADESC(CFuncParent)
	DEFINE_FIELD(speed, FIELD_FLOAT),
	DEFINE_FIELD(wait, FIELD_FLOAT),

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

LINK_ENTITY_TO_CLASS(func_parent, CFuncParent);

void CFuncParent::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "wait"))
	{
		wait = atof(pkvd->szValue);
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

void CFuncParent::Spawn()
{
	// Defino os angulos do movimento
	SetMovedir(this);

	// Defino a velocidade
	if (pev->speed == 0)
		pev->speed = 100;
	speed = pev->speed;

	// Modifico as propriedades basicas da entidade
	pev->movetype = MOVETYPE_PUSH;
	SetAbsOrigin(GetAbsOrigin());
	SetModel(STRING(pev->model));

	// Deixo o interior de LinearMoveDone() acessivel
	blockThink = false;
}

void CFuncParent::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Movimento do func_parent
	LinearMove(pev, SetMoveVector(pev), speed);
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
}

// Coordena as configuracoes e chamadas de movimento (eh a principal)
void CFuncParent::ProcessMovement(entvars_t *parent, string_t targetName)
{
	parent = SetEntVars_t(targetName);
	if (parent != NULL)
	{
		parent->movetype = MOVETYPE_PUSH;
		LinearMove(parent, SetMoveVector(parent), speed);
	}
}

// Encontra entidades no mapa
entvars_t * CFuncParent::SetEntVars_t(string_t targetName)
{
	edict_t* pentTarget = NULL;

	pentTarget = FIND_ENTITY_BY_STRING(pentTarget, "targetname", STRING(targetName));

	if (FNullEnt(pentTarget))
		return NULL;
	else
		return &pentTarget->v;
}

// Define o vetor de deslocamento
Vector CFuncParent::SetMoveVector(entvars_t *entity)
{
	// Pego o vetor da posicao atual
	m_vecPosition1 = GetAbsOrigin();

	// Acerto a direcao do movimento:
	if (entity != pev)
		entity->movedir = pev->movedir;

	// Calculo o vetor deslocamento (creditos para a Valve):
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2 = m_vecPosition1 + (pev->movedir * (fabs(pev->movedir.x * (pev->size.x - 2)) + fabs(pev->movedir.y * (pev->size.y - 2)) + fabs(pev->movedir.z * (pev->size.z - 2)) - m_flLip));
	
	ASSERTSZ(m_vecPosition1 != m_vecPosition2, "door start/end positions are equal");

	return m_vecPosition2;
}

// Efetivamente move as entidades
void CFuncParent::LinearMove(entvars_t *entity, Vector vecDest, float flSpeed)
{
	ASSERTSZ(flSpeed != 0, "LinearMove:  no speed is defined!");

	// Ja chegou?
	if (vecDest == entity->origin)
		return;

	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDest - GetAbsOrigin();

	// divide vector length by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to LinearMoveDone when dest is reached
	entity->nextthink = entity->ltime + flTravelTime;
	if (entity == pev)
	{
		parent_time = entity->ltime + flTravelTime + wait;
		SetThink(&CFuncParent::LinearMoveDone);
	}

	// scale the destdelta vector by the time spent traveling to get velocity
	entity->velocity = vecDestDelta / flTravelTime;
}

void CFuncParent::LinearMoveDone(void)
{
	if (!blockThink)
	{
		// Paro a entidade
		if (pev->velocity > 0)
			pev->velocity = 0;

		// Verifico se ela ja pode iniciar a proxima entidade (target)
		if (pev->ltime > parent_time)
		{
			blockThink = true;

			if (strcmp(STRING(pev->target), "") != 0)
				FireTargets(STRING(pev->target), m_hActivator, this, USE_TOGGLE, 0);
		}
		else
			// Se ainda ouver delay para contabilizar, continuo rodando essa funcao...
			pev->nextthink = pev->ltime + 0.1;
	}
}