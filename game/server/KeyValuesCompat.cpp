#include "vstdlib/IKeyValuesSystem.h"
#include "KeyValues.h"

//The server can just use KeyValuesSystem() directly. - Solokiller
IKeyValuesSystem *keyvalues()
{
	return KeyValuesSystem();
}
