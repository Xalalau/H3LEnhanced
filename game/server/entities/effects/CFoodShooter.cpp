// ##############################
// HU3-LIFE LANCADOR DE COMIDAS
// ##############################

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CGib.h"
#include "CGibShooter.h"

#include "CFoodShooter.h"

LINK_ENTITY_TO_CLASS(foodshooter, CFoodShooter);

void CFoodShooter::Precache(void)
{
	m_iGibModelIndex = PRECACHE_MODEL("models/fgibs.mdl");
}

CGib *CFoodShooter::CreateGib(void)
{
	CGib *pGib = GetClassPtr((CGib *)NULL);
	pGib->GibCreate("models/fgibs.mdl");
	pGib->m_bloodColor = BLOOD_COLOR_YELLOW;

	if (pev->body <= 1)
	{
		ALERT(at_aiconsole, "FoodShooter Body is <= 1!\n");
	}

	pGib->pev->body = RANDOM_LONG(0, pev->body - 1);

	return pGib;
}
