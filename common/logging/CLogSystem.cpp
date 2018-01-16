#include <mutex>
#include <sstream>
#include <string>
#include <utility>

//Include before spdlog headers to avoid conflicts with Windows types - Solokiller
#include "extdll.h"

#include <spdlog/spdlog.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/file_sinks.h>
#include <spdlog/sinks/null_sink.h>

#include "CBaseGameInterface.h"

#include "CConsoleLogSink.h"
#include "CLogSystem.h"
#include "LogDefs.h"
#include "Logging.h"

namespace logging
{
namespace
{
CLogSystem g_LogSystem;
}

CLogSystem& LogSystem()
{
	return g_LogSystem;
}

CLogSystem::CLogSystem() = default;
CLogSystem::~CLogSystem() = default;

bool CLogSystem::Initialize()
{
	//Configure global logging settings
	//Set up the error handler
	spdlog::set_error_handler(
		[ & ]( const std::string& szErrorMessage )
		{
			this->LogErrorHandler( szErrorMessage );
		}
	);

	//Default pattern is year-month-day format
	spdlog::set_pattern( "[%Y-%m-%d %T.%e] [%n] [%l] %v" );

	//Prefix all logs with the name of the library that created it
	SetBasePath( "logs/" LIBRARY_NAME "_" );

	UTIL_AddCommand( "log_level_" LIBRARY_NAME,
		[]
		{
			LogSystem().Command_LogLevel();
		}
	);

	UTIL_AddCommand( "log_to_file_" LIBRARY_NAME,
		[]
		{
			LogSystem().Command_LogToFile();
		}
	);

	UTIL_AddCommand( "log_list_loggers_" LIBRARY_NAME,
		[]
		{
			LogSystem().Command_ListLoggers();
		}
	);

	UTIL_AddCommand( "log_test_logger_" LIBRARY_NAME,
		[]
		{
			LogSystem().Command_TestLogger();
		}
	);
	
	//Create shared sinks
	m_NullSink = std::make_shared<spdlog::sinks::null_sink<LoggingMutex_t>>();

	if( UTIL_CheckParm( "-condebug" ) )
	{
		m_DebugSink = std::make_shared<LogSink_t>( PrepareFilename( "condebug" ), 0, 0 );

		if( !m_DebugSink )
		{
			Con_Printf( "Fatal error: Couldn't create condebug log sink\n" );
			return false;
		}
	}

	m_ConsoleSink = std::make_shared<CConsoleLogSink<LoggingMutex_t>>();

	m_LogDistSink = std::make_shared<LOGGING_FACTORY( spdlog::sinks::dist_sink )>();

	if( !m_ConsoleSink || !m_LogDistSink )
	{
		Con_Printf( "Fatal error: Couldn't create log sinks\n" );
		return false;
	}

	m_State = State::ACTIVE;

	//Create global, non configurable loggers
	null_logger = CreateNullLogger( "null_logger" );
	con = CreateConsoleLogger( "console" );
	log = CreateMultiLogger( "log" );

	if( !null_logger || !con || !log )
	{
		return false;
	}

	//TODO: add startup config, add option to control logging - Solokiller

	return true;
}

void CLogSystem::Shutdown()
{
	//So that there's a "Log file closed" message with a timestamp.
	if( IsLogToFileEnabled() )
		DisableLogToFile();

	m_State = State::UNINITIALIZED;

	{
		//Drop all loggers we created before internally
		std::shared_ptr<spdlog::logger>* const loggers[] = 
		{
			&log,
			&con,
			&null_logger
		};

		for( auto logger : loggers )
		{
			DropLogger( *logger );
			( *logger ).reset();
		}
	}

	m_LogDistSink.reset();

	m_LogFileSink.reset();
	m_DebugSink.reset();
	m_ConsoleSink.reset();
	m_NullSink.reset();
}

//Helper function to create loggers and handle the exception
template<typename FACTORY>
auto CreateSpdLogObject( const std::string& szName, FACTORY factory )
{
	try
	{
		return factory();
	}
	catch( const spdlog::spdlog_ex& e )
	{
		//Should always be shown so devs can catch it
		Con_Printf( "Failed to create spdlog object \"%s\": %s\n", szName.c_str(), e.what() );
	}

	return decltype( factory() ){};
}

std::shared_ptr<spdlog::logger> CLogSystem::CreateBasicLogger( const std::string& logger_name, const spdlog::filename_t& filename, bool truncate )
{
	return CreateSpdLogObject( logger_name,
		[ & ]()
		{
			const auto szCompleteFilename{ PrepareFilename( filename ) };

			return spdlog::LOGGING_FACTORY( basic_logger )( logger_name, szCompleteFilename, truncate );
		}
	);
}

std::shared_ptr<spdlog::logger> CLogSystem::CreateRotatingLogger( const std::string& logger_name, const spdlog::filename_t& filename, size_t max_file_size, size_t max_files )
{
	return CreateSpdLogObject( logger_name,
		[ & ]()
		{
			const auto szCompleteFilename{ PrepareFilename( filename ) };

			return spdlog::LOGGING_FACTORY( rotating_logger )( logger_name, szCompleteFilename, max_file_size, max_files );
		}
	);
}

std::shared_ptr<spdlog::logger> CLogSystem::CreateDailyLogger( const std::string& logger_name, const spdlog::filename_t& filename, int hour, int minute )
{
	return CreateSpdLogObject( logger_name,
		[ & ]()
		{
			const auto szCompleteFilename{ PrepareFilename( filename ) };

			auto sink = std::make_shared<LogSink_t>(
				PrepareFilename( szCompleteFilename ),
				hour,
				minute
			);

			return spdlog::create( logger_name, sink );
		}
	);
}

std::shared_ptr<spdlog::logger> CLogSystem::CreateConsoleLogger( const std::string& logger_name )
{
	if( m_State != State::ACTIVE )
		Con_DPrintf( "Warning: log system has not initialized, console loggers may not function properly\n" );

	return CreateSpdLogObject( logger_name,
		[ & ]()
		{
			std::vector<spdlog::sink_ptr> sinks;

			//Log to console and debug log if enabled
			sinks.emplace_back( m_ConsoleSink );

			if( m_DebugSink )
				sinks.emplace_back( m_DebugSink );

			return spdlog::create( logger_name, sinks.begin(), sinks.end() );
		}
	);
}

std::shared_ptr<spdlog::logger> CLogSystem::CreateMultiLogger( const std::string& logger_name )
{
	if( m_State != State::ACTIVE )
		Con_DPrintf( "Warning: log system has not fully initialized, multi loggers may not function properly\n" );

	return CreateSpdLogObject( logger_name,
		[ & ]()
		{
			std::vector<spdlog::sink_ptr> sinks;

			sinks.emplace_back( m_ConsoleSink );

			if( m_DebugSink )
				sinks.emplace_back( m_DebugSink );

			sinks.emplace_back( m_LogDistSink );

			return spdlog::create( logger_name, sinks.begin(), sinks.end() );
		}
	);
}

std::shared_ptr<spdlog::logger> CLogSystem::CreateNullLogger( const std::string& logger_name )
{
	return CreateSpdLogObject( logger_name,
		[ & ]()
		{
			return spdlog::create( logger_name, m_NullSink );
		}
	);
}

std::shared_ptr<spdlog::logger> CLogSystem::GetLogger( const std::string& szLoggerName )
{
	return spdlog::get( szLoggerName );
}

void CLogSystem::DropLogger( const std::shared_ptr<spdlog::logger>& logger )
{
	ASSERT( logger );

	if( !logger )
		return;

	spdlog::drop( logger->name() );
}

spdlog::level::level_enum CLogSystem::GetLogLevel( const std::string& szLoggerName ) const
{
	auto logger = spdlog::get( szLoggerName );

	if( !logger )
		return spdlog::level::off;

	return logger->level();
}

bool CLogSystem::SetLogLevel( spdlog::logger& logger, spdlog::level::level_enum level )
{
	logger.set_level( level );

	return true;
}

bool CLogSystem::SetLogLevel( spdlog::logger& logger, const char* pszLogLevel )
{
	try
	{
		return SetLogLevel( logger, LogLevelFromString( pszLogLevel ) );
	}
	catch( const std::runtime_error& )
	{
		return false;
	}
}

bool CLogSystem::SetLogLevel( const std::string& szLoggerName, spdlog::level::level_enum level )
{
	auto logger = spdlog::get( szLoggerName );

	if( !logger )
		return false;

	return SetLogLevel( *logger, level );
}

bool CLogSystem::SetLogLevel( const std::string& szLoggerName, const char* pszLogLevel )
{
	auto logger = spdlog::get( szLoggerName );

	if( !logger )
		return false;

	return SetLogLevel( *logger, pszLogLevel );
}

void CLogSystem::SetBasePath( std::string&& szPath )
{
	UTIL_FixSlashes( &szPath[ 0 ] );

	m_szBasePath = GameInterface()->GetGameDirectory();
	m_szBasePath += FILESYSTEM_PATH_SEPARATOR_CHAR;

	if( !szPath.empty() )
		m_szBasePath += szPath;

	//Note: this uses the given path because g_pFileSystem already handles the game directory name in its search paths
	const auto lastSlash = szPath.find_last_of( FILESYSTEM_PATH_SEPARATOR_CHAR );

	const auto szDirectoryPart = lastSlash != szPath.npos ? szPath.substr( 0, lastSlash ) : szPath;

	//Make sure the directories exist
	g_pFileSystem->CreateDirHierarchy( szDirectoryPart.c_str(), nullptr );

	//TODO: update existing sinks? - Solokiller
}

bool CLogSystem::IsLogToFileEnabled() const
{
	return !!m_LogFileSink;
}

void CLogSystem::SetLogToFile( const bool bState )
{
	if( bState )
		EnableLogToFile();
	else
		DisableLogToFile();
}

void CLogSystem::EnableLogToFile()
{
	if( !IsLogToFileEnabled() )
	{
		const char szBaseFilename[] = "L";

		const auto szBaseName = PrepareFilename( szBaseFilename );

		auto logFile = CreateLogSink( szBaseFilename );

		if( logFile )
		{
			m_LogFileSink = logFile;
			Con_Printf( "%s logging data to file %s\n", LIBRARY_NAME, szBaseName.c_str() );
			m_LogDistSink->add_sink( m_LogFileSink );
		}
	}
}

void CLogSystem::DisableLogToFile()
{
	if( IsLogToFileEnabled() )
	{
		log->critical( "Log file closed" );
		m_LogDistSink->remove_sink( m_LogFileSink );
		m_LogFileSink.reset();
	}

	Con_Printf( "%s logging disabled.\n", LIBRARY_NAME );
}

void CLogSystem::Command_LogLevel()
{
	if( Cmd_ArgC() < 2 )
	{
		Con_Printf( "Usage: log_level_%s <logger name> [<log level>]\nAvailable log levels:\n", LIBRARY_NAME );

		for( auto pszLevel : spdlog::level::level_names )
		{
			Con_Printf( "%s\n", pszLevel );
		}

		return;
	}

	const char* pszLoggerName = Cmd_ArgV( 1 );

	auto logger = GetLogger( pszLoggerName );

	if( !logger )
	{
		Con_Printf( "Logger not found: \"%s\"\n", pszLoggerName );
		return;
	}

	if( Cmd_ArgC() < 3 )
	{
		Con_Printf( "Log level for logger \"%s\": \"%s\"\n", pszLoggerName, spdlog::level::to_str( logger->level() ) );
		return;
	}

	const char* pszLogLevel = Cmd_ArgV( 2 );

	if( SetLogLevel( *logger, pszLogLevel ) )
	{
		Con_Printf( "Log level for logger \"%s\" set to \"%s\"\n", pszLoggerName, pszLogLevel );
	}
	else
	{
		Con_Printf( "Couldn't set log level for logger \"%s\" to \"%s\"\n", pszLoggerName, pszLogLevel );
	}
}

void CLogSystem::Command_LogToFile()
{
	//Mimics the engine's "log" command
	if( Cmd_ArgC() < 2 )
	{
		Con_Printf( "Usage: log_to_file_%s < on | off >\n", LIBRARY_NAME );

		if( m_LogFileSink )
			Con_Printf( "currently logging\n" );
		else
			Con_Printf( "not currently logging\n" );

		return;
	}

	const char* const pszNewState = Cmd_ArgV( 1 );

	if( !strcmp( pszNewState, "on" ) )
	{
		EnableLogToFile();
	}
	else if( !strcmp( pszNewState, "off" ) )
	{
		DisableLogToFile();
	}
	else
	{
		Con_Printf( "log_to_file_%s:  unknown parameter %s, 'on' and 'off' are valid\n", LIBRARY_NAME, pszNewState );
	}
}

void CLogSystem::Command_ListLoggers()
{
	size_t uiCount = 0;

	Con_Printf( "Available loggers:\n" );

	spdlog::apply_all(
		[ & ]( const std::shared_ptr<spdlog::logger>& logger )
		{
			++uiCount;

			Con_Printf( "%s\n", logger->name().c_str() );
		}
	);

	Con_Printf( "%u loggers\n", uiCount );
}

void CLogSystem::Command_TestLogger()
{
	const auto iArgC = Cmd_ArgC();

	if( iArgC < 3 )
	{
		Con_Printf( "Usage: log_test_logger_%s < name > < level > [ < arg >... ]\n", LIBRARY_NAME );
		return;
	}

	const char* pszLoggerName = Cmd_ArgV( 1 );

	auto logger = GetLogger( pszLoggerName );

	if( !logger )
	{
		Con_Printf( "Logger not found: \"%s\"\n", pszLoggerName );
		return;
	}

	try
	{
		const char* pszLogLevel = Cmd_ArgV( 2 );

		const auto level = LogLevelFromString( pszLogLevel );

		//Unfortunately Cmd_Args isn't available on the client side, so we need a cross-dll solution
		std::stringstream stream;

		//This won't catch quotes or redundant spaces or anything, but it's enough to test the logger
		for( int iArg = 3; iArg < iArgC; ++iArg )
		{
			stream << Cmd_ArgV( iArg );

			if( iArg + 1 < iArgC )
				stream << ' ';
		}

		logger->log( level, "Test string: {}", stream.str() );
	}
	catch( const std::runtime_error& e )
	{
		Con_Printf( "%s\n", e.what() );
	}
}

void CLogSystem::LogErrorHandler( const std::string& szErrorMessage )
{
	Con_Printf( "Error in spdlog while logging: \"%s\"\n", szErrorMessage.c_str() );
}

std::string CLogSystem::PrepareFilename( const std::string& szFilename ) const
{
	//Put the log directory in the game directory
	//TODO: could check here if users already added the game directory to help flag misuse - Solokiller
	return m_szBasePath + szFilename;
}

std::shared_ptr<CLogSystem::LogSink_t> CLogSystem::CreateLogSink( const std::string& szBaseName )
{
	return CreateSpdLogObject( szBaseName,
		[ & ]()
		{
			return std::make_shared<LogSink_t>(
				PrepareFilename( szBaseName ),
				0,
				0
			);
		}
	);
}
}
