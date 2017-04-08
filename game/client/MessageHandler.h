#ifndef GAME_CLIENT_MESSAGEHANDLER_H
#define GAME_CLIENT_MESSAGEHANDLER_H

#include <functional>
#include <type_traits>

#include "tier0/platform.h"

#include <UtlDict.h>

#include "shared_game_utils.h"

/**
*	Stores network message handlers so the engine can invoke them correctly.
*/
class CMessageHandlers final
{
private:
	using Handler_t = std::function<bool( const char*, int, void* )>;

public:
	CMessageHandlers() = default;
	~CMessageHandlers()
	{
		m_MessageHandlers.PurgeAndDeleteElements();
	}

private:
	//Overload for bool return
	template<typename T, typename RET, typename... ARGS, std::enable_if_t<std::is_same<RET, bool>::value, bool> = false>
	static auto CreateLambda( T* pHandler, RET ( T::* handlerFn )( ARGS... ) )
	{
		return [ = ]( const char* pszName, int iSize, void* pBuf )
		{
			return ( pHandler->*handlerFn )( pszName, iSize, pBuf );
		};
	}

	//Overload for void return
	template<typename T, typename RET, typename... ARGS, std::enable_if_t<std::is_same<RET, void>::value, bool> = false>
	static auto CreateLambda( T* pHandler, RET( T::* handlerFn )( ARGS... ) )
	{
		return [ = ]( const char* pszName, int iSize, void* pBuf )
		{
			( pHandler->*handlerFn )( pszName, iSize, pBuf );

			return true;
		};
	}

	//If not bool or void, you can't add a handler.

public:
	/**
	*	Adds a handler.
	*	If you get a compile error, check to make sure the handler returns either bool or void. Those are the only supported return types.
	*	@param pszName Name of the message to handle
	*	@param handler Handler to invoke handlerFn on
	*	@param handlerFn Handler function to invoke
	*/
	template<typename T, typename RET>
	void Add( const char* pszName, T& handler, RET ( T::* handlerFn )( const char*, int, void* ) )
	{
		ASSERT( pszName );
		ASSERT( *pszName );
		ASSERT( handlerFn );

		{
			auto index = m_MessageHandlers.Find( pszName );

			//Already in the map.
			if( index != m_MessageHandlers.InvalidIndex() )
			{
				Con_DPrintf( "Tried to add a message handler for message \"%s\" twice!\n", pszName );
				return;
			}
		}

		auto lambda = CreateLambda( &handler, handlerFn );

		m_MessageHandlers.Insert( pszName, new Handler_t( lambda ) );

		//Make sure it's hooked in the engine as well.
		HookMessage( pszName );
	}

	/**
	*	Invokes a message handler for pszName.
	*	If no handler exists for the given message, and a fallback handler is set, the fallback will be checked as well.
	*/
	bool Invoke( const char* pszName, int iSize, void* pBuf );

	CMessageHandlers* GetFallbackHandlers() { return m_pFallbackHandlers; }

	/**
	*	Sets the fallback handler to use.
	*/
	void SetFallbackHandlers( CMessageHandlers* pFallbackHandlers );

private:
	static void HookMessage( const char* pszName );

private:
	//We need to store pointers here because the utility containers don't copy construct; they memcpy.
	//TODO: fix this bug in the containers? - Solokiller
	CUtlDict<Handler_t*, unsigned short> m_MessageHandlers;

	CMessageHandlers* m_pFallbackHandlers = nullptr;

private:
	CMessageHandlers( const CMessageHandlers& ) = delete;
	CMessageHandlers& operator=( const CMessageHandlers& ) = delete;
};

namespace msghandler
{
void Initialize();

/**
*	The function used to invoke all message handlers. Fallback handlers are used for conditional invocation.
*/
int MsgFunc_MessageHandlers( const char* pszName, int iSize, void* pBuf );
}

#endif //GAME_CLIENT_MESSAGEHANDLER_H
