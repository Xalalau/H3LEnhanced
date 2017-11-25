#include "extdll.h"
#include "util.h"

#include "Angelscript/CHLASManager.h"

#include "CHLASClientInitializer.h"

bool CHLASClientInitializer::RegisterCoreAPI( CASManager& manager )
{
	CHLASBaseInitializer::RegisterCoreAPI( manager );

	//auto& engine = *manager.GetEngine();

	return true;
}