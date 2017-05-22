/* 
	HU3-LIFE point_counter
	Entidade que conta ativações para ativar o target.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CPointEntity.h"

#include "CPointCounter.h"

BEGIN_DATADESC(CPointCounter)
	DEFINE_FIELD(m_iCount, FIELD_INTEGER),
END_DATADESC()

LINK_ENTITY_TO_CLASS(point_counter, CPointCounter);

void CPointCounter::KeyValue(KeyValueData *pkvd)
{
	// Pegamos a entrada flcount do hammer (fgd) e adicionamos a variavel.
	if (FStrEq(pkvd->szKeyName, "picount"))
	{
		m_iCount = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

void CPointCounter::Spawn(void)
{
	// Se não tivermos uma contagem, resetamos para 1.
	if (m_iCount <= 0) { m_iCount = 1; }
}

void CPointCounter::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Ao ativar, subtraimos 1 da contagem.
	m_iCount--;

	// Ao chegar em zero, ativamos o target final
	if (m_iCount == 0)
	{
		SUB_UseTargets(pActivator, USE_TOGGLE, 0);
		//ALERT(at_console, "Triggered!!");
		m_iCount = 1;
	}
}