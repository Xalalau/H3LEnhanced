#include "cl_util.h"

#include "MessageHandler.h"

namespace msghandler
{
namespace
{
//Enables more precise debugging than the engine's debug code since we've got our own handlers in addition to the engine hooks.
static cvar_t* msghandler_debug = nullptr;
}

void Initialize()
{
	msghandler_debug = CVAR_CREATE( "msghandler_debug", "0", 0 );
}

int MsgFunc_MessageHandlers( const char* pszName, int iSize, void* pBuf )
{
	return MessageHandlers().Invoke( pszName, iSize, pBuf );
}
}

bool CMessageHandlers::Invoke( const char* pszName, int iSize, void* pBuf )
{
	auto index = m_MessageHandlers.Find( pszName );

	if( index != m_MessageHandlers.InvalidIndex() )
	{
		auto& handler = m_MessageHandlers.Element( index );

		if( msghandler::msghandler_debug->value )
		{
			Con_Printf( "msghandler: Dispatching message \"%s\", size %d\n", pszName, iSize );
		}

		return ( *handler )( pszName, iSize, pBuf );
	}

	if( m_pFallbackHandlers )
		return m_pFallbackHandlers->Invoke( pszName, iSize, pBuf );

	if( msghandler::msghandler_debug->value )
	{
		Con_Printf( "msghandler: Couldn't find handler for message \"%s\", size %d\n", pszName, iSize );
	}

	return false;
}

void CMessageHandlers::SetFallbackHandlers( CMessageHandlers* pFallbackHandlers )
{
	//Null it out in case there's a problem.
	m_pFallbackHandlers = nullptr;

	if( pFallbackHandlers )
	{
		for( auto pHandlers = pFallbackHandlers; pHandlers; pHandlers = pHandlers->GetFallbackHandlers() )
		{
			if( pHandlers == this )
			{
				//This can cause infinite recursion.
				ASSERT( false );
				Con_Printf( "Tried to add a fallback message handler that was set to fall back to this handler!\n" );
				return;
			}
		}
	}

	m_pFallbackHandlers = pFallbackHandlers;
}

void CMessageHandlers::HookMessage( const char* pszName )
{
	gEngfuncs.pfnHookUserMsg( pszName, &msghandler::MsgFunc_MessageHandlers );
}
