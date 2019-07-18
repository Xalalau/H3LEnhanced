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

	// Pego o vetor da posicao atual
	m_vecPosition1 = GetAbsOrigin();

	// Calculo o vetor deslocamento (creditos para a Valve):
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2 = m_vecPosition1 + (pev->movedir * (fabs(pev->movedir.x * (pev->size.x - 2)) + fabs(pev->movedir.y * (pev->size.y - 2)) + fabs(pev->movedir.z * (pev->size.z - 2)) - m_flLip));

	ASSERTSZ(m_vecPosition1 != m_vecPosition2, "door start/end positions are equal");

	// Deixo o interior de LinearMoveDone() acessivel
	blockThink = false;
}

void CFuncParent::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Movimento do func_parent
	LinearMove(pev, m_vecPosition2, speed);
	// Movimento dos parents
	parent01 = ProcessMovement(parent01, parent01_name);
	parent02 = ProcessMovement(parent02, parent02_name);
	parent03 = ProcessMovement(parent03, parent03_name);
	parent04 = ProcessMovement(parent04, parent04_name);
	parent05 = ProcessMovement(parent05, parent05_name);
	parent06 = ProcessMovement(parent06, parent06_name);
	parent07 = ProcessMovement(parent07, parent07_name);
	parent08 = ProcessMovement(parent08, parent08_name);
	parent09 = ProcessMovement(parent09, parent09_name);
	parent10 = ProcessMovement(parent10, parent10_name);
	parent11 = ProcessMovement(parent11, parent11_name);
	parent12 = ProcessMovement(parent12, parent12_name);
	parent13 = ProcessMovement(parent13, parent13_name);
	parent14 = ProcessMovement(parent14, parent14_name);
	parent15 = ProcessMovement(parent15, parent15_name);
	parent16 = ProcessMovement(parent16, parent16_name);
}

// Coordena as configuracoes e chamadas de movimento (eh a principal)
entvars_t* CFuncParent::ProcessMovement(entvars_t *parent, string_t targetName)
{
	parent = SetEntVars_t(targetName);
	if (parent != NULL)
	{
		parent->movetype = MOVETYPE_PUSH;
		LinearMove(parent, m_vecPosition2, speed);

		return parent;
	}

	return nullptr;
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

// Efetivamente move as entidades
void CFuncParent::LinearMove(entvars_t *entity, Vector vecDest, float flSpeed)
{
	// Ja chegou?
	ASSERTSZ(flSpeed != 0, "LinearMove:  no speed is defined!");
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
		// Paro a entidade e todos os parents
		if (pev->velocity > 0)
			pev->velocity = 0;
		if (parent01)
			if (parent01->velocity > 0)
					parent01->velocity = 0;
		if (parent02)
			if (parent02->velocity > 0)
				parent02->velocity = 0;
		if (parent03)
			if (parent03->velocity > 0)
				parent03->velocity = 0;
		if (parent04)
			if (parent04->velocity > 0)
				parent04->velocity = 0;
		if (parent05)
			if (parent05->velocity > 0)
				parent05->velocity = 0;
		if (parent06)
			if (parent06->velocity > 0)
				parent06->velocity = 0;
		if (parent07)
			if (parent07->velocity > 0)
				parent07->velocity = 0;
		if (parent08)
			if (parent08->velocity > 0)
				parent08->velocity = 0;
		if (parent09)
			if (parent09->velocity > 0)
				parent09->velocity = 0;
		if (parent10)
			if (parent10->velocity > 0)
				parent10->velocity = 0;
		if (parent11)
			if (parent11->velocity > 0)
				parent11->velocity = 0;
		if (parent12)
			if (parent12->velocity > 0)
				parent12->velocity = 0;
		if (parent13)
			if (parent13->velocity > 0)
				parent13->velocity = 0;
		if (parent14)
			if (parent14->velocity > 0)
				parent14->velocity = 0;
		if (parent15)
			if (parent15->velocity > 0)
				parent15->velocity = 0;
		if (parent16)
			if (parent16->velocity > 0)
				parent16->velocity = 0;

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