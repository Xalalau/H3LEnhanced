/* 
	HU3-LIFE point_counter
	Entidade que conta ativações para ativar o target.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CPointEntity.h"

#include "CPointCounter.h"

LINK_ENTITY_TO_CLASS(point_counter, CPointCounter);

void CPointCounter::Spawn(void)
{
	// Se não tivermos uma contagem, resetamos para 1.
	if (m_flCount <= 0)
		m_flCount = 1;

	//ALERT(at_console, "Count: %f", m_flCount);
}

void CPointCounter::KeyValue(KeyValueData *pkvd)
{
	// Pegamos a entrada flcount do hammer (fgd) e adicionamos a variavel.
	if (FStrEq(pkvd->szKeyName, "flcount"))
	{
		m_flCount = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

void CPointCounter::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Pega o target da entidade, se nao tem a quem mandar output, retorna.
	CBaseEntity* pTarget = UTIL_FindEntityByTargetname(nullptr, GetTarget());
	if (!pTarget)
		return;

	// Ao ativar, subtraimos 1 da contagem.
	m_flCount--;

	// Ao chegar em zero, ativamos o target final
	if (m_flCount == 0)
	{
		if (pTarget)
		{
			//FireTargets(GetTarget(), pActivator, this, useType, value);
			//ALERT(at_console, "Triggered!! %s", MAKE_STRING(GetTarget()));
		}
	}

	//ALERT(at_console, "Count Modified: %f", m_flCount);
}