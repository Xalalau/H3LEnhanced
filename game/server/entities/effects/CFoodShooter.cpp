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
	auto pGib = CGib::GibCreate("models/fgibs.mdl");
	pGib->m_bloodColor = BLOOD_COLOR_YELLOW;

	if (GetBody() <= 1)
	{
		ALERT(at_aiconsole, "FoodShooter Body is <= 1!\n");
	}

	pGib->SetBody(RANDOM_LONG(0, GetBody() - 1));

	return pGib;
}