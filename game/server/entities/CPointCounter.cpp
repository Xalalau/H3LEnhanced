/* 
	HU3-LIFE point_counter
	Entidade que conta ativações para ativar o target.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include <string>
#include "CPointEntity.h"

#include "CPointCounter.h"

BEGIN_DATADESC(CPointCounter)
	DEFINE_FIELD(m_iCount, FIELD_INTEGER),
	DEFINE_FIELD(m_sMessageTitle, FIELD_STRING),
	DEFINE_FIELD(m_sMessageFinished, FIELD_STRING),
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
	else if (FStrEq(pkvd->szKeyName, "ptitle"))
	{
		m_sMessageTitle = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;

		// Configura os parametros do texto
		// TODO: Deixar o jogador setar isso? (Copia do CGameText, nao reinventar a roda!..)
		m_sCountText.x = -1;
		m_sCountText.y = 0.8;
		m_sCountText.effect = 2;
		m_sCountText.channel = 4;

		// Cores
		m_sCountText.r1 = 210;
		m_sCountText.g1 = 210;
		m_sCountText.b1 = 210;
		m_sCountText.a1 = 255;

		m_sCountText.r2 = 240;
		m_sCountText.g2 = 110;
		m_sCountText.b2 = 0;
		m_sCountText.a2 = 255;

		// Tempo de Efeitos
		m_sCountText.fadeinTime = 0.05;
		m_sCountText.fadeoutTime = 0.5;
		m_sCountText.holdTime = 15;
		m_sCountText.fxTime = 0.10;
	}
	else if (FStrEq(pkvd->szKeyName, "pfinishedtitle"))
	{
		m_sMessageFinished = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

void CPointCounter::Spawn(void)
{
	// Se não tivermos uma contagem, resetamos para 1.
	if (m_iCount <= 0) { m_iCount = 1; }
	m_iCountTotal = m_iCount;
}

void CPointCounter::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Mostrar progresso
	char buffer[100];
	sprintf(buffer, "%s: %d de %d", STRING(m_sMessageTitle), (m_iCountTotal - m_iCount) + 1, m_iCountTotal);

	UTIL_HudMessageAll(m_sCountText, buffer);

	// Ao ativar, subtraimos 1 da contagem.
	m_iCount--;

	// Ao chegar em zero, ativamos o target final
	if (m_iCount == 0)
	{
		UTIL_HudMessageAll(m_sCountText, STRING(m_sMessageFinished));
		SUB_UseTargets(pActivator, USE_TOGGLE, 0);
		m_iCount = 1;
	}
}