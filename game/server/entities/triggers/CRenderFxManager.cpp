#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CRenderFxManager.h"

LINK_ENTITY_TO_CLASS( env_render, CRenderFxManager );

void CRenderFxManager::Spawn( void )
{
	SetSolidType( SOLID_NOT );
}

void CRenderFxManager::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( HasTarget() )
	{
		CBaseEntity* pTarget = nullptr;
		while( ( pTarget = UTIL_FindEntityByTargetname( pTarget, GetTarget() ) ) != nullptr )
		{
			if( !pTarget->GetSpawnFlags().Any( SF_RENDER_MASKFX ) )
				pTarget->SetRenderFX( GetRenderFX() );
			if( !pTarget->GetSpawnFlags().Any( SF_RENDER_MASKAMT ) )
				pTarget->SetRenderAmount( GetRenderAmount() );
			if( !pTarget->GetSpawnFlags().Any( SF_RENDER_MASKMODE ) )
				pTarget->SetRenderMode( GetRenderMode() );
			if( !pTarget->GetSpawnFlags().Any( SF_RENDER_MASKCOLOR ) )
				pTarget->SetRenderColor( GetRenderColor() );
		}
	}
}