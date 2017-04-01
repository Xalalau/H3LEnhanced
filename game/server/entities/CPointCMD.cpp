// ##############################
// HU3-LIFE point_cmd
// ##############################
// Entidade que roda comandos de console

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CPointEntity.h"

#include "CPointCMD.h"

BEGIN_DATADESC( CPointCMD )
	DEFINE_FIELD(m_Command, FIELD_STRING),
END_DATADESC()

LINK_ENTITY_TO_CLASS(point_cmd, CPointCMD);

void CPointCMD::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "command"))
	{
		m_Command = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

void CPointCMD::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;

	if (pev->scale > 0)
		pev->nextthink = gpGlobals->time + 1.0;
}

void CPointCMD::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	RunCommand();
}

// only plays for ONE client, so only use in single play!
void CPointCMD::Think(void)
{
	edict_t *pClient;

	// manually find the single player. 
	pClient = g_engfuncs.pfnPEntityOfEntIndex(1);

	// Can't play if the client is not connected!
	if (!pClient)
		return;

	pev->nextthink = gpGlobals->time + 0.5;

	if ((pClient->v.origin - pev->origin).Length() <= pev->scale)
		RunCommand();

}

void CPointCMD::RunCommand()
{
	ASSERT(!FStrEq(STRING(m_Command), ""));

	edict_t *pClient;

	// manually find the single player. 
	pClient = g_engfuncs.pfnPEntityOfEntIndex(1);

	// Can't play if the client is not connected!
	if (!pClient)
		return;

	CLIENT_COMMAND(pClient, "%s\n", STRING(m_Command));
}
