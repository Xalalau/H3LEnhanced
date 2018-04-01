/*
	Hu3-Life Entity System

	point_timer
	Temporizador de disparo de entidades

	by: NickMBR Mar/2018
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CPointEntity.h"

#include "CPointTimer.h"

BEGIN_DATADESC(CPointTimer)
	DEFINE_FIELD(m_iDuration, FIELD_INTEGER),
	DEFINE_FIELD(m_iRepeat, FIELD_INTEGER),
	DEFINE_FIELD(m_flendTime, FIELD_TIME),
	DEFINE_FIELD(e_Target, FIELD_ENTITY),

	DEFINE_THINKFUNC(TimerThink),
	DEFINE_USEFUNC(TimerStart),
END_DATADESC()

LINK_ENTITY_TO_CLASS(point_timer, CPointTimer);

// Pega os valores das variaveis do FGD
void CPointTimer::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "tduration"))
	{
		m_iDuration = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else if (FStrEq(pkvd->szKeyName, "trepeat"))
	{
		m_iRepeat = atof(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

// Seta as variaveis iniciais para 0 e realiza algumas checagens
// Se tudo estiver OK, seta o metodo USE para TimerStart
void CPointTimer::Spawn(void)
{
	m_flendTime = 0;

	// Checagem de argumentos, não aceitar entidade se argumentos forem nulos ou vazios
	if ((m_iDuration == NULL || m_iDuration <= 0) || (m_iRepeat == NULL || m_iRepeat <= 0))
	{
		ALERT(at_console, "point_timer detectado com argumentos faltantes, a entidade nao rodara!, Limite: %i, Repeticao: %i\n", m_iDuration, m_iRepeat);
		SetThink(NULL);
		SetUse(NULL); // Nao sera ativada por nada
		return;
	}

	SetUse(&CPointTimer::TimerStart); // Seta o metodo de ativacao
}

void CPointTimer::TimerThink(void)
{
	// Verifica se o tempo ja ultrapassou o limite, reseta ou executa o target
	if (gpGlobals->time > m_flendTime)
	{
		SetThink(NULL);
		SetUse(&CPointTimer::TimerStart); // Ativa novamente o uso dessa entidade
		m_flendTime = 0; // Reseta o tempo limite
	}
	else
	{
		SetUse(NULL); // Desativa o uso da entidade ate que ela termine seu processo
		SetNextThink(gpGlobals->time + m_iRepeat);

		SUB_UseTargets(e_Target, USE_TOGGLE, 0);
	}
}

void CPointTimer::TimerStart(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	m_flendTime = gpGlobals->time + m_iDuration;
	e_Target = pActivator; // Hackzinho maroto para passar o pActivator ao Think para economizar tempo KK --SORRY

	if (m_flendTime > 0 && e_Target != NULL)
	{
		// ALERT(at_console, "point_timer iniciou com: Limite: %i, Repeticao: %i, Inicio: %.2f, Final: %.2f\n", m_iDuration, m_iRepeat, m_flstartTime, m_flendTime);
		SetThink(&CPointTimer::TimerThink);
		SetNextThink(gpGlobals->time + m_iRepeat);
	}
	else
	{
		ALERT(at_console, "point_timer detectado com argumentos faltantes, a entidade nao rodara!, Limite: %i, Repeticao: %i\n", m_iDuration, m_iRepeat);
		SetThink(NULL);
		return;
	}
	
}