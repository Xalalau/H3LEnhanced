#ifndef COMMON_LOGGING_LOGDEFS_H
#define COMMON_LOGGING_LOGDEFS_H

#include <stdexcept>
#include <type_traits>

#include <spdlog/common.h>

//Support for multithreaded logging, if it's ever needed
//Define USE_MULTITHREAD_LOGGING to enable
#ifdef USE_MULTITHREAD_LOGGING
const bool LOGGING_MULTITHREADED = true;
#define LOGGING_SUFFIX _mt
using LoggingMutex_t = std::mutex;
#else
const bool LOGGING_MULTITHREADED = false;
#define LOGGING_SUFFIX _st
using LoggingMutex_t = spdlog::details::null_mutex;
#endif

#define __LOGGING_FACTORY( name, suffix ) name ## suffix
#define _LOGGING_FACTORY( name, suffix ) __LOGGING_FACTORY(name, suffix )
/**
*	@brief Used to create loggers that can be made multithreaded automatically
*/
#define LOGGING_FACTORY( name ) _LOGGING_FACTORY( name, LOGGING_SUFFIX )

/**
*	@brief Converts a string containing a long form spdlog log level to an enum value
*	@param pszLogLevel Log level as a string
*	@throws std::runtime_error If the log level is unknown
*/
inline spdlog::level::level_enum LogLevelFromString( const char* const pszLogLevel )
{
	ASSERT( pszLogLevel );

	std::underlying_type_t<spdlog::level::level_enum> index = 0;

	for( auto pszLevel : spdlog::level::level_names )
	{
		if( !strcmp( pszLevel, pszLogLevel ) )
		{
			const auto level = static_cast<spdlog::level::level_enum>( index );

			return level;
		}

		++index;
	}

	throw std::runtime_error( "Unknown log level" );
}

#endif //COMMON_LOGGING_LOGDEFS_H
