// ##############################
// HU3-LIFE trigger_cmd
// ##############################
// Trigger que roda comandos de console

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBaseTrigger.h"

#include "CTriggerCMD.h"

BEGIN_DATADESC( CTriggerCMD )
	DEFINE_FIELD(m_Command, FIELD_STRING),
END_DATADESC()

LINK_ENTITY_TO_CLASS(trigger_cmd, CTriggerCMD);

void CTriggerCMD::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "command"))
	{
		m_Command = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseTrigger::KeyValue(pkvd);
}

// !!!HACK - overloaded HEALTH to avoid adding new field
void CTriggerCMD::Touch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
	{
		// only clients may trigger these events
		return;
	}
	RunCommand();
}

void CTriggerCMD::Spawn(void)
{
	if (m_Command == NULL)
		ALERT(at_console, "Command is null");
	InitTrigger();
}

void CTriggerCMD::Use(CBaseEntity pActivator, CBaseEntity pCaller, USE_TYPE useType, float value)
{
	RunCommand();
}

// only plays for ONE client, so only use in single play!
void CTriggerCMD::RunCommand()
{
	ASSERT(!FStrEq(STRING(m_Command), ""));

	edict_t *pClient;

	// manually find the single player. 
	pClient = g_engfuncs.pfnPEntityOfEntIndex(1);

	// Can't play if the client is not connected!
	if (!pClient)
		return;

	CLIENT_COMMAND(pClient, "%s\n", STRING(m_Command));

	SetTouch(NULL);
	UTIL_Remove(this);
}
